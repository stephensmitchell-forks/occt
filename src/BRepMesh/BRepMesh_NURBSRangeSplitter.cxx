// Created on: 2016-04-19
// Copyright (c) 2016 OPEN CASCADE SAS
// Created by: Oleg AGASHIN
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#include <BRepMesh_NURBSRangeSplitter.hxx>
#include <GeomLib.hxx>
#include <GCPnts_AbscissaPoint.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <IMeshTools_Parameters.hxx>
#include <GeomAbs_IsoType.hxx>
#include <BRepMesh_GeomTool.hxx>
#include <NCollection_Handle.hxx>

namespace
{
  class AnalyticalFilter
  {
  public:
    //! Constructor.
    AnalyticalFilter(
      const IMeshData::IFaceHandle&             theDFace,
      const GeomAbs_IsoType                     theIsoType,
      const Handle(IMeshData::SequenceOfReal)&  theParams,
      const Handle(IMeshData::SequenceOfReal)&  theControlParams,
      const Handle(IMeshData::MapOfReal)&       theParamsForbiddenToRemove,
      const Handle(IMeshData::MapOfReal)&       theControlParamsForbiddenToRemove)
      : myDFace(theDFace),
        mySurface(myDFace->GetSurface()->ChangeSurface().Surface().Surface()),
        myIsoU(theIsoType == GeomAbs_IsoU),
        myParams(theParams),
        myControlParams(theControlParams),
        myParamsForbiddenToRemove(theParamsForbiddenToRemove),
        myControlParamsForbiddenToRemove(theControlParamsForbiddenToRemove),
        myAllocator(new NCollection_IncAllocator(IMeshData::MEMORY_BLOCK_SIZE_HUGE)),
        myControlParamsToRemove(new IMeshData::MapOfReal(1, myAllocator))
    {
    }

    //! Returns map of parameters supposed to be removed.
    const Handle(IMeshData::MapOfReal)& GetControlParametersToRemove(
      const IMeshTools_Parameters& theParameters)
    {
      myParameters = theParameters;

      Standard_Integer aStartIndex, aEndIndex;
      if (myIsoU)
      {
        aStartIndex = 1;
        aEndIndex = myParams->Length();
      }
      else
      {
        aStartIndex = 2;
        aEndIndex = myParams->Length() - 1;
      }

      for (Standard_Integer i = aStartIndex; i <= aEndIndex; ++i)
      {
        myCurrParam = myParams->Value(i);
        myIso = new GeomAdaptor_Curve(myIsoU ? mySurface->UIso(myCurrParam) : mySurface->VIso(myCurrParam));

        myPrevControlParam = myControlParams->Value(1);
        myIso->D1(myPrevControlParam, myPrevControlPnt, myPrevControlVec);
        for (Standard_Integer j = 2; j <= myControlParams->Length();)
        {
          j += checkControlPointAndMoveOn(j);
        }
      }

      return myControlParamsToRemove;
    }

  private:

    //! Checks the given control point for deviation.
    //! Returns number of steps to be used to move point iterator.
    Standard_Integer checkControlPointAndMoveOn(const Standard_Integer theIndex)
    {
      Standard_Integer aMoveSteps = 0;
      myCurrControlParam = myControlParams->Value(theIndex);
      myIso->D1(myCurrControlParam, myCurrControlPnt, myCurrControlVec);

      const Standard_Real aMidParam = 0.5 * (myPrevControlParam + myCurrControlParam);
      const gp_Pnt aMidPnt = myIso->Value(aMidParam);

      const Standard_Real aSqDist = BRepMesh_GeomTool::SquareDeflectionOfSegment(
        myPrevControlPnt, myCurrControlPnt, aMidPnt);

      const Standard_Real aSqMaxDeflection = myDFace->GetDeflection() * myDFace->GetDeflection();
      if (aSqDist > aSqMaxDeflection &&
          aSqDist > myParameters.MinSize * myParameters.MinSize)
      {
        // insertion 
        myControlParams->InsertBefore(theIndex, aMidParam);
      }
      else
      {
        //put regular grig for normals
        if (checkParamForHugeNormalDeviation(aMidParam))
        {
          myControlParams->InsertBefore(theIndex, aMidParam);
          return aMoveSteps;
        }

        // Here we should leave at least 3 parameters as far as
        // we must have at least one parameter related to surface
        // internals in order to prevent movement of triangle body
        // outside the surface in case of highly curved ones, e.g.
        // BSpline springs.
        if (aSqDist < aSqMaxDeflection &&
            myControlParams->Length() > 3 &&
            theIndex < myControlParams->Length())
        {
          // Remove too dense points
          const Standard_Real aTmpParam = myControlParams->Value(theIndex + 1);
          if (checkParameterForDeflectionAndUpdateCache(aTmpParam))
          {
            ++aMoveSteps;
          }
        }

        myPrevControlParam = myCurrControlParam;
        myPrevControlPnt   = myCurrControlPnt;
        myPrevControlVec   = myCurrControlVec;

        ++aMoveSteps;
      }

      return aMoveSteps;
    }

    //! Checks normals at three points (prevParam, theParam, currParam) for deviation of normals.
    Standard_Boolean checkParamForHugeNormalDeviation(const Standard_Real theParam)
    {
      gp_Pnt2d aStPnt1, aStPnt2;
      if (myIsoU)
      {
        aStPnt1 = gp_Pnt2d(myCurrParam, myPrevControlParam);
        aStPnt2 = gp_Pnt2d(myCurrParam, theParam);
      }
      else
      {
        aStPnt1 = gp_Pnt2d(myPrevControlParam, myCurrParam);
        aStPnt2 = gp_Pnt2d(theParam, myCurrParam);
      }

      gp_Dir N1(0, 0, 1), N2(0, 0, 1);
      const Standard_Integer aSt1 = GeomLib::NormEstim(mySurface, aStPnt1, Precision::Confusion(), N1);
      const Standard_Integer aSt2 = GeomLib::NormEstim(mySurface, aStPnt2, Precision::Confusion(), N2);

      const Standard_Real aAngle = N2.Angle(N1);
      if (aSt1 < 1 && aSt2 < 1 && aAngle > myParameters.Angle)
      {
        const Standard_Real aLen = GCPnts_AbscissaPoint::Length(
          *myIso, myPrevControlParam, theParam, myDFace->GetDeflection());

        if (aLen > myParameters.MinSize)
        {
          // insertion 
          return Standard_True;
        }
      }

      return Standard_False;
    }

    //! Checks whether the given param suits specified deflection. Updates cache.
    Standard_Boolean checkParameterForDeflectionAndUpdateCache(const Standard_Real theParam)
    {
      gp_Pnt aTmpPnt;
      gp_Vec aTmpVec;
      myIso->D1(theParam, aTmpPnt, aTmpVec);

      const Standard_Real aTmpMidParam = 0.5 * (myPrevControlParam + theParam);
      const gp_Pnt        aTmpMidPnt = myIso->Value(aTmpMidParam);

      // Lets check next parameter.
      // If it also fits deflection, we can remove previous parameter.
      const Standard_Real aSqDist = BRepMesh_GeomTool::SquareDeflectionOfSegment(
        myPrevControlPnt, aTmpPnt, aTmpMidPnt);

      if (aSqDist < myDFace->GetDeflection() * myDFace->GetDeflection())
      {
        // Lets check parameters for angular deflection.
        if (myPrevControlVec.SquareMagnitude() < gp::Resolution() ||
            aTmpVec.SquareMagnitude()          < gp::Resolution() ||
            myPrevControlVec.Angle(aTmpVec)    < myParameters.Angle)
        {
          // For current Iso line we can remove this parameter.
          myControlParamsToRemove->Add(myCurrControlParam);
          myCurrControlParam = theParam;
          myCurrControlPnt   = aTmpPnt;
          myCurrControlVec   = aTmpVec;
          return Standard_True;
        }
        else
        {
          // We have found a place on the surface refusing 
          // removement of this parameter.
          myParamsForbiddenToRemove       ->Add(myCurrParam);
          myControlParamsForbiddenToRemove->Add(myCurrControlParam);
        }
      }

      return Standard_False;
    }

  private:

    IMeshData::IFaceHandle                myDFace;
    Handle(Geom_Surface)                  mySurface;
    Standard_Boolean                      myIsoU;
    Handle(IMeshData::SequenceOfReal)     myParams;
    Handle(IMeshData::SequenceOfReal)     myControlParams;

    Handle(IMeshData::MapOfReal)          myParamsForbiddenToRemove;
    Handle(IMeshData::MapOfReal)          myControlParamsForbiddenToRemove;

    Handle(NCollection_IncAllocator)      myAllocator;
    Handle(IMeshData::MapOfReal)          myControlParamsToRemove;


    IMeshTools_Parameters                 myParameters;
    NCollection_Handle<GeomAdaptor_Curve> myIso;

    Standard_Real                         myCurrParam;

    Standard_Real                         myCurrControlParam;
    gp_Pnt                                myCurrControlPnt;
    gp_Vec                                myCurrControlVec;

    Standard_Real                         myPrevControlParam;
    gp_Pnt                                myPrevControlPnt;
    gp_Vec                                myPrevControlVec;
  };
}

//=======================================================================
// Function: AdjustRange
// Purpose : 
//=======================================================================
void BRepMesh_NURBSRangeSplitter::AdjustRange()
{
  BRepMesh_DefaultRangeSplitter::AdjustRange();
  mySurfaceType = GetSurface()->GetType();

  if (mySurfaceType == GeomAbs_BezierSurface ||
      mySurfaceType == GeomAbs_BSplineSurface)
  {
    const std::pair<Standard_Real, Standard_Real>& aRangeU = GetRangeU();
    const std::pair<Standard_Real, Standard_Real>& aRangeV = GetRangeV();

    if (mySurfaceType == GeomAbs_BezierSurface)
    {
      myIsValid = !(aRangeU.first  < -0.5 ||
                    aRangeU.second >  1.5 ||
                    aRangeV.first  < -0.5 ||
                    aRangeV.second >  1.5);

      if (!myIsValid)
      {
        return;
      }
    }

    amplifyParameters();
  }
}

//=======================================================================
// Function: amplifyParameters
// Purpose : 
//=======================================================================
void BRepMesh_NURBSRangeSplitter::amplifyParameters()
{
  const GeomAbs_Shape aContinuity = GeomAbs_CN;
  for (Standard_Integer i = 0; i < 2; ++i)
  {
    const Standard_Boolean isU = (i == 0);
    const Standard_Integer aIntervalsNb = isU ?
      GetSurface()->NbUIntervals(aContinuity) :
      GetSurface()->NbVIntervals(aContinuity);

    const std::pair<Standard_Real, Standard_Real>& aRange = isU ?
      GetRangeU() :
      GetRangeV();

    IMeshData::IMapOfReal& aParams = isU ?
      GetParametersU() :
      GetParametersV();

    if (aIntervalsNb < aParams.Size())
    {
      continue;
    }

    TColStd_Array1OfReal aIntervals(1, aIntervalsNb + 1);
    if (isU)
    {
      GetSurface()->UIntervals(aIntervals, aContinuity);
    }
    else
    {
      GetSurface()->VIntervals(aIntervals, aContinuity);
    }

    for (Standard_Integer j = 1; j <= aIntervals.Upper(); ++j)
    {
      const Standard_Real aParam = aIntervals(j);
      if (aParam > aRange.first &&
          aParam < aRange.second)
      {
        aParams.Add(aParam);
      }
    }
  }
}

//=======================================================================
// Function: GenerateSurfaceNodes
// Purpose : 
//=======================================================================
Handle(IMeshData::ListOfPnt2d) BRepMesh_NURBSRangeSplitter::GenerateSurfaceNodes(
  const IMeshTools_Parameters& theParameters) const
{
  const std::pair<Standard_Real, Standard_Real>& aRangeU = GetRangeU();
  const std::pair<Standard_Real, Standard_Real>& aRangeV = GetRangeV();
  const std::pair<Standard_Real, Standard_Real>& aDelta  = GetDelta ();

  const Standard_Real                 aDefFace = GetDFace()->GetDeflection();
  const Handle(BRepAdaptor_HSurface)& gFace    = GetSurface();
  Handle(Geom_Surface)                aSurface = gFace->ChangeSurface().Surface().Surface();

  const Handle(NCollection_IncAllocator) aTmpAlloc =
    new NCollection_IncAllocator(IMeshData::MEMORY_BLOCK_SIZE_HUGE);

  const Handle(IMeshData::SequenceOfReal) aParams[2] = {
    computeGrainAndFilterParameters(myUParams, gFace->UResolution(aDefFace),
      (aRangeU.second - aRangeU.first), aDelta.first,  theParameters, aTmpAlloc),

    computeGrainAndFilterParameters(myVParams, gFace->VResolution(aDefFace),
      (aRangeV.second - aRangeV.first), aDelta.second, theParameters, aTmpAlloc)
  };

  // check intermediate isolines
  Handle(IMeshData::MapOfReal) aFixedParams[2] = {
    new IMeshData::MapOfReal(1, aTmpAlloc),
    new IMeshData::MapOfReal(1, aTmpAlloc)
  };

  const Handle(IMeshData::MapOfReal) aParamsToRemove[2] = {
    AnalyticalFilter(GetDFace(), GeomAbs_IsoV, aParams[1], aParams[0],
      aFixedParams[1], aFixedParams[0]).GetControlParametersToRemove(theParameters),

    AnalyticalFilter(GetDFace(), GeomAbs_IsoU, aParams[0], aParams[1],
      aFixedParams[0], aFixedParams[1]).GetControlParametersToRemove(theParameters),
  };

  aParamsToRemove[0]->Subtract(*aFixedParams[0]);
  aParamsToRemove[1]->Subtract(*aFixedParams[1]);

  // insert nodes of the regular grid
  Handle(IMeshData::ListOfPnt2d) aNodes = new IMeshData::ListOfPnt2d(
    new NCollection_IncAllocator(IMeshData::MEMORY_BLOCK_SIZE_HUGE));

  // insert nodes of the regular grid
  for (Standard_Integer i = 1; i <= aParams[0]->Length(); ++i)
  {
    const Standard_Real aParam1 = aParams[0]->Value(i);
    if (aParamsToRemove[0]->Contains(aParam1))
    {
      continue;
    }

    for (Standard_Integer j = 1; j <= aParams[1]->Length(); ++j)
    {
      const Standard_Real aParam2 = aParams[1]->Value(j);
      if (aParamsToRemove[1]->Contains(aParam2))
      {
        continue;
      }

      aNodes->Append(gp_Pnt2d(aParam1, aParam2));
    }
  }

  return aNodes;
}

//=======================================================================
//function : computeGrainAndFilterParameters
//purpose  : 
//=======================================================================
Handle(IMeshData::SequenceOfReal) BRepMesh_NURBSRangeSplitter::computeGrainAndFilterParameters(
  const IMeshData::IMapOfReal&            theSourceParams,
  const Standard_Real                     theTol2d,
  const Standard_Real                     theRangeDiff,
  const Standard_Real                     theDelta,
  const IMeshTools_Parameters&            theParameters,
  const Handle(NCollection_IncAllocator)& theAllocator) const
{
  // Sort and filter sequence of parameters
  Standard_Real aMinDiff = Precision::PConfusion();
  if (theDelta < 1.)
  {
    aMinDiff /= theDelta;
  }

  aMinDiff = Max(theParameters.MinSize, aMinDiff);

  const Standard_Real aDiffMaxLim = 0.1 * theRangeDiff;
  const Standard_Real aDiffMinLim = Max(0.005 * theRangeDiff, 2. * theTol2d);
  const Standard_Real aDiff = Max(theParameters.MinSize, Min(aDiffMaxLim, aDiffMinLim));
  return filterParameters(theSourceParams, aMinDiff, aDiff, theAllocator);
}

//=======================================================================
//function : filterParameters
//purpose  : 
//=======================================================================
Handle(IMeshData::SequenceOfReal) BRepMesh_NURBSRangeSplitter::filterParameters(
  const IMeshData::IMapOfReal&            theParams,
  const Standard_Real                     theMinDist,
  const Standard_Real                     theFilterDist,
  const Handle(NCollection_IncAllocator)& theAllocator) const
{
  Handle(IMeshData::SequenceOfReal) aResult = new IMeshData::SequenceOfReal(theAllocator);

  // Sort sequence of parameters
  const Standard_Integer anInitLen = theParams.Extent();

  TColStd_Array1OfReal aParamArray(1, anInitLen);
  Standard_Integer j;
  for (j = 1; j <= anInitLen; j++)
    aParamArray(j) = theParams(j);

  std::sort(aParamArray.begin(), aParamArray.end());

  // mandatory pre-filtering using the first (minimal) filter value
  Standard_Integer aParamLength = 1;
  for (j = 2; j <= anInitLen; j++)
  {
    if ((aParamArray(j) - aParamArray(aParamLength)) > theMinDist)
    {
      if (++aParamLength < j)
        aParamArray(aParamLength) = aParamArray(j);
    }
  }

  //perform filtering on series
  Standard_Real aLastAdded, aLastCandidate;
  Standard_Boolean isCandidateDefined = Standard_False;
  aLastAdded = aParamArray(1);
  aLastCandidate = aLastAdded;
  aResult->Append(aLastAdded);

  for (j = 2; j < aParamLength; j++)
  {
    Standard_Real aVal = aParamArray(j);
    if (aVal - aLastAdded > theFilterDist)
    {
      //adds the parameter
      if (isCandidateDefined)
      {
        aLastAdded = aLastCandidate;
        isCandidateDefined = Standard_False;
        j--;
      }
      else
      {
        aLastAdded = aVal;
      }
      aResult->Append(aLastAdded);
      continue;
    }

    aLastCandidate = aVal;
    isCandidateDefined = Standard_True;
  }
  aResult->Append(aParamArray(aParamLength));

  return aResult;
}