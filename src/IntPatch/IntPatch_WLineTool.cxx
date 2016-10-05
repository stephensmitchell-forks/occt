// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#include <IntPatch_WLineTool.hxx>

#include <Adaptor3d_HSurface.hxx>
#include <Adaptor3d_TopolTool.hxx>
#include <ElCLib.hxx>
#include <ElSLib.hxx>
#include <IntPatch_SpecialPoints.hxx>

// It is pure empirical value.
const Standard_Real IntPatch_WLineTool::myMaxConcatAngle = M_PI/6;

//Bit-mask is used for information about 
//the operation made in
//IntPatch_WLineTool::ExtendTwoWLines(...) method.
enum
{
  IntPatchWT_EnAll = 0x00,
  IntPatchWT_DisLastLast = 0x01,
  IntPatchWT_DisLastFirst = 0x02,
  IntPatchWT_DisFirstLast = 0x04,
  IntPatchWT_DisFirstFirst = 0x08
};

enum IntPatchWT_WLsConnectionType
{
  IntPatchWT_NotConnected,
  IntPatchWT_Singular,
  IntPatchWT_EachOther
};

//=======================================================================
//function : MinMax
//purpose  : Replaces theParMIN = MIN(theParMIN, theParMAX),
//                    theParMAX = MAX(theParMIN, theParMAX).
//
//           Static subfunction in IsSeamOrBound.
//=======================================================================
static inline void MinMax(Standard_Real& theParMIN, Standard_Real& theParMAX)
{
  if(theParMIN > theParMAX)
  {
    const Standard_Real aTmp = theParMAX;
    theParMAX = theParMIN;
    theParMIN = aTmp;
  }
}

//=======================================================================
//function : IsOnPeriod
//purpose  : Checks, if [theU1, theU2] intersects the period-value
//            (k*thePeriod, where k is an integer number (k = 0, +/-1, +/-2, ...).
//
//           Returns:
//            0 - if interval [theU1, theU2] does not intersect the "period-value"
//                or if thePeriod == 0.0;
//            1 - if interval (theU1, theU2) intersect the "period-value".
//            2 - if theU1 or/and theU2 lie ON the "period-value"
//
//ATTENTION!!!
//  If (theU1 == theU2) then this function will return only both 0 or 2.
//=======================================================================
static Standard_Integer IsOnPeriod(const Standard_Real theU1,
                                   const Standard_Real theU2,
                                   const Standard_Real thePeriod)
{
  if(thePeriod < RealSmall())
    return 0;

  //If interval [theU1, theU2] intersect seam-edge then there exists an integer
  //number N such as 
  //    (theU1 <= T*N <= theU2) <=> (theU1/T <= N <= theU2/T),
  //where T is the period.
  //I.e. the inerval [theU1/T, theU2/T] must contain at least one
  //integer number. In this case, Floor(theU1/T) and Floor(theU2/T)
  //return different values or theU1/T is strictly integer number.
  //Examples:
  //  1. theU1/T==2.8, theU2/T==3.5 => Floor(theU1/T) == 2, Floor(theU2/T) == 3.
  //  2. theU1/T==2.0, theU2/T==2.6 => Floor(theU1/T) == Floor(theU2/T) == 2.

  const Standard_Real aVal1 = theU1/thePeriod,
                      aVal2 = theU2/thePeriod;
  const Standard_Integer aPar1 = static_cast<Standard_Integer>(Floor(aVal1));
  const Standard_Integer aPar2 = static_cast<Standard_Integer>(Floor(aVal2));
  if(aPar1 != aPar2)
  {//Interval (theU1, theU2] intersects seam-edge
    if(IsEqual(aVal2, static_cast<Standard_Real>(aPar2)))
    {//aVal2 is an integer number => theU2 lies ON the "seam-edge"
      return 2;
    }

    return 1;
  }

  //Here, aPar1 == aPar2. 

  if(IsEqual(aVal1, static_cast<Standard_Real>(aPar1)))
  {//aVal1 is an integer number => theU1 lies ON the "seam-edge"
    return 2;
  }

  //If aVal2 is a true integer number then always (aPar1 != aPar2).

  return 0;
}

//=======================================================================
//function : IsSeamOrBound
//purpose  : Returns TRUE if segment [thePtf, thePtl] intersects "seam-edge"
//            (if it exist) or surface boundaries and both thePtf and thePtl do
//            not match "seam-edge" or boundaries.
//           Point thePtmid lies in this segment (in both 3D and 2D-space).
//           If thePtmid match "seam-edge" or boundaries strictly 
//            (without any tolerance) then the function will return TRUE.
//            See comments in function body for detail information.
//=======================================================================
static Standard_Boolean IsSeamOrBound(const IntSurf_PntOn2S& thePtf,
                                      const IntSurf_PntOn2S& thePtl,
                                      const IntSurf_PntOn2S& thePtmid,
                                      const Standard_Real theU1Period,
                                      const Standard_Real theU2Period,
                                      const Standard_Real theV1Period,
                                      const Standard_Real theV2Period,
                                      const Standard_Real theUfSurf1,
                                      const Standard_Real theUlSurf1,
                                      const Standard_Real theVfSurf1,
                                      const Standard_Real theVlSurf1,
                                      const Standard_Real theUfSurf2,
                                      const Standard_Real theUlSurf2,
                                      const Standard_Real theVfSurf2,
                                      const Standard_Real theVlSurf2)
{
  Standard_Real aU11 = 0.0, aU12 = 0.0, aV11 = 0.0, aV12 = 0.0;
  Standard_Real aU21 = 0.0, aU22 = 0.0, aV21 = 0.0, aV22 = 0.0;
  thePtf.Parameters(aU11, aV11, aU12, aV12);
  thePtl.Parameters(aU21, aV21, aU22, aV22);

  MinMax(aU11, aU21);
  MinMax(aV11, aV21);
  MinMax(aU12, aU22);
  MinMax(aV12, aV22);

  if((aU11 - theUfSurf1)*(aU21 - theUfSurf1) < 0.0)
  {//Interval [aU11, aU21] intersects theUfSurf1
    return Standard_True;
  }

  if((aU11 - theUlSurf1)*(aU21 - theUlSurf1) < 0.0)
  {//Interval [aU11, aU21] intersects theUlSurf1
    return Standard_True;
  }

  if((aV11 - theVfSurf1)*(aV21 - theVfSurf1) < 0.0)
  {//Interval [aV11, aV21] intersects theVfSurf1
    return Standard_True;
  }

  if((aV11 - theVlSurf1)*(aV21 - theVlSurf1) < 0.0)
  {//Interval [aV11, aV21] intersects theVlSurf1
    return Standard_True;
  }

  if((aU12 - theUfSurf2)*(aU22 - theUfSurf2) < 0.0)
  {//Interval [aU12, aU22] intersects theUfSurf2
    return Standard_True;
  }

  if((aU12 - theUlSurf2)*(aU22 - theUlSurf2) < 0.0)
  {//Interval [aU12, aU22] intersects theUlSurf2
    return Standard_True;
  }

  if((aV12 - theVfSurf2)*(aV22 - theVfSurf2) < 0.0)
  {//Interval [aV12, aV22] intersects theVfSurf2
    return Standard_True;
  }

  if((aV12 - theVlSurf2)*(aV22 - theVlSurf2) < 0.0)
  {//Interval [aV12, aV22] intersects theVlSurf2
    return Standard_True;
  }

  if(IsOnPeriod(aU11, aU21, theU1Period))
    return Standard_True;

  if(IsOnPeriod(aV11, aV21, theV1Period))
    return Standard_True;

  if(IsOnPeriod(aU12, aU22, theU2Period))
    return Standard_True;

  if(IsOnPeriod(aV12, aV22, theV2Period))
    return Standard_True;

  /*
    The segment [thePtf, thePtl] does not intersect the boundaries and
    the seam-edge of the surfaces.
    Nevertheless, following situation is possible:

                  seam or
                   bound
                     |
        thePtf  *    |
                     |
                     * thePtmid
          thePtl  *  |
                     |

    This case must be processed, too.
  */

  Standard_Real aU1 = 0.0, aU2 = 0.0, aV1 = 0.0, aV2 = 0.0;
  thePtmid.Parameters(aU1, aV1, aU2, aV2);

  if(IsEqual(aU1, theUfSurf1) || IsEqual(aU1, theUlSurf1))
    return Standard_True;

  if(IsEqual(aU2, theUfSurf2) || IsEqual(aU2, theUlSurf2))
    return Standard_True;

  if(IsEqual(aV1, theVfSurf1) || IsEqual(aV1, theVlSurf1))
    return Standard_True;

  if(IsEqual(aV2, theVfSurf2) || IsEqual(aV2, theVlSurf2))
    return Standard_True;

  if(IsOnPeriod(aU1, aU1, theU1Period))
    return Standard_True;

  if(IsOnPeriod(aU2, aU2, theU2Period))
    return Standard_True;

  if(IsOnPeriod(aV1, aV1, theV1Period))
    return Standard_True;

  if(IsOnPeriod(aV2, aV2, theV2Period))
    return Standard_True;

  return Standard_False;
}

//=======================================================================
//function : IsIntersectionPoint
//purpose  : Returns True if thePmid is intersection point
//            between theS1 and theS2 with given tolerance.
//           In this case, parameters of thePmid on every quadric
//            will be recomputed and returned.
//=======================================================================
static Standard_Boolean IsIntersectionPoint(const gp_Pnt& thePmid,
                                            const Handle(Adaptor3d_HSurface)& theS1,
                                            const Handle(Adaptor3d_HSurface)& theS2,
                                            const IntSurf_PntOn2S& theRefPt,
                                            const Standard_Real theTol,
                                            const Standard_Real* const theArrPeriods,
                                            IntSurf_PntOn2S& theNewPt)
{
  Standard_Real aU1 = 0.0, aV1 = 0.0, aU2 = 0.0, aV2 = 0.0;
  
  switch(theS1->GetType())
  {
  case GeomAbs_Plane:
    ElSLib::Parameters(theS1->Plane(), thePmid, aU1, aV1);
    break;

  case GeomAbs_Cylinder:
    ElSLib::Parameters(theS1->Cylinder(), thePmid, aU1, aV1);
    break;

  case GeomAbs_Sphere:
    ElSLib::Parameters(theS1->Sphere(), thePmid, aU1, aV1);
    break;

  case GeomAbs_Cone:
    ElSLib::Parameters(theS1->Cone(), thePmid, aU1, aV1);
    break;

  case GeomAbs_Torus:
    ElSLib::Parameters(theS1->Torus(), thePmid, aU1, aV1);
    break;

  default:
    return Standard_False;
  }

  switch(theS2->GetType())
  {
  case GeomAbs_Plane:
    ElSLib::Parameters(theS2->Plane(), thePmid, aU2, aV2);
    break;

  case GeomAbs_Cylinder:
    ElSLib::Parameters(theS2->Cylinder(), thePmid, aU2, aV2);
    break;

  case GeomAbs_Sphere:
    ElSLib::Parameters(theS2->Sphere(), thePmid, aU2, aV2);
    break;

  case GeomAbs_Cone:
    ElSLib::Parameters(theS2->Cone(), thePmid, aU2, aV2);
    break;

  case GeomAbs_Torus:
    ElSLib::Parameters(theS2->Torus(), thePmid, aU2, aV2);
    break;

  default:
    return Standard_False;
  }

  theNewPt.SetValue(thePmid, aU1, aV1, aU2, aV2);

  IntPatch_SpecialPoints::AdjustPointAndVertex(theRefPt, theArrPeriods, theNewPt);

  const gp_Pnt aP1(theS1->Value(aU1, aV1));
  const gp_Pnt aP2(theS2->Value(aU2, aV2));

  return (aP1.SquareDistance(aP2) <= theTol*theTol);
}

//=======================================================================
//function : ExtendFirst
//purpose  : Adds thePOn2S to the begin of theWline
//=======================================================================
static void ExtendFirst(const Handle(IntPatch_WLine)& theWline,
                        const IntSurf_PntOn2S& theAddedPt)
{
  Standard_Real aU1 = 0.0, aV1 = 0.0, aU2 = 0.0, aV2 = 0.0;
  theAddedPt.Parameters(aU1, aV1, aU2, aV2);

  if(theAddedPt.IsSame(theWline->Point(1), Precision::Confusion()))
  {
    theWline->Curve()->Value(1, theAddedPt);
    for(Standard_Integer i = 1; i <= theWline->NbVertex(); i++)
    {
      IntPatch_Point &aVert = theWline->ChangeVertex(i);
      if(aVert.ParameterOnLine() != 1)
        break;

      aVert.SetParameters(aU1, aV1, aU2, aV2);
      aVert.SetValue(theAddedPt.Value());
    }

    return;
  }

  theWline->Curve()->InsertBefore(1, theAddedPt);

  for(Standard_Integer i = 1; i <= theWline->NbVertex(); i++)
  {
    IntPatch_Point &aVert = theWline->ChangeVertex(i);

    if(aVert.ParameterOnLine() == 1)
    {
      aVert.SetParameters(aU1, aV1, aU2, aV2);
      aVert.SetValue(theAddedPt.Value());
    }
    else
    {
      aVert.SetParameter(aVert.ParameterOnLine()+1);
    }
  }
}

//=======================================================================
//function : ExtendLast
//purpose  : Adds thePOn2S to the end of theWline
//=======================================================================
static void ExtendLast(const Handle(IntPatch_WLine)& theWline,
                        const IntSurf_PntOn2S& theAddedPt)
{
  Standard_Real aU1 = 0.0, aV1 = 0.0, aU2 = 0.0, aV2 = 0.0;
  theAddedPt.Parameters(aU1, aV1, aU2, aV2);

  const Standard_Integer aNbPnts = theWline->NbPnts();
  if(theAddedPt.IsSame(theWline->Point(aNbPnts), Precision::Confusion()))
  {
    theWline->Curve()->Value(aNbPnts, theAddedPt);
  }
  else
  {
    theWline->Curve()->Add(theAddedPt);
  }

  for(Standard_Integer i = theWline->NbVertex(); i >= 1; i--)
  {
    IntPatch_Point &aVert = theWline->ChangeVertex(i);
    if(aVert.ParameterOnLine() != aNbPnts)
      break;

    aVert.SetParameters(aU1, aV1, aU2, aV2);
    aVert.SetValue(theAddedPt.Value());
    aVert.SetParameter(theWline->NbPnts());
  }
}

//=========================================================================
// function: IsOutOfDomain
// purpose : Checks, if 2D-representation of thePOn2S is in surfaces domain,
//            defined by bounding-boxes theBoxS1 and theBoxS2
//=========================================================================
static Standard_Boolean IsOutOfDomain(const Bnd_Box2d& theBoxS1,
                                      const Bnd_Box2d& theBoxS2,
                                      const IntSurf_PntOn2S &thePOn2S,
                                      const Standard_Real* const theArrPeriods)
{
  Standard_Real aU1 = 0.0, aV1 = 0.0, aU2 = 0.0, aV2 = 0.0;
  Standard_Real aU1min = 0.0, aU1max = 0.0, aV1min = 0.0, aV1max = 0.0;
  Standard_Real aU2min = 0.0, aU2max = 0.0, aV2min = 0.0, aV2max = 0.0;

  thePOn2S.Parameters(aU1, aV1, aU2, aV2);

  theBoxS1.Get(aU1min, aV1min, aU1max, aV1max);
  theBoxS2.Get(aU2min, aV2min, aU2max, aV2max);

  aU1 = ElCLib::InPeriod(aU1, aU1min, aU1min + theArrPeriods[0]);
  aV1 = ElCLib::InPeriod(aV1, aV1min, aV1min + theArrPeriods[1]);
  aU2 = ElCLib::InPeriod(aU2, aU2min, aU2min + theArrPeriods[2]);
  aV2 = ElCLib::InPeriod(aV2, aV2min, aV2min + theArrPeriods[3]);

  return (theBoxS1.IsOut(gp_Pnt2d(aU1, aV1)) ||
          theBoxS2.IsOut(gp_Pnt2d(aU2, aV2)));
}

//=======================================================================
//function : CheckArgumentsToExtend
//purpose  : Check if extending is possible
//            (see IntPatch_WLineTool::ExtendTwoWLines)
//=======================================================================
static IntPatchWT_WLsConnectionType
                    CheckArgumentsToExtend(const Handle(Adaptor3d_HSurface)& theS1,
                                           const Handle(Adaptor3d_HSurface)& theS2,
                                           const IntSurf_PntOn2S& thePtWL1,
                                           const IntSurf_PntOn2S& thePtWL2,
                                           IntSurf_PntOn2S& theNewPoint,
                                           const gp_Vec& theVec1,
                                           const gp_Vec& theVec2,
                                           const gp_Vec& theVec3,
                                           const Bnd_Box2d& theBoxS1,
                                           const Bnd_Box2d& theBoxS2,
                                           const Standard_Real theToler3D,
                                           const Standard_Real* const theArrPeriods)
{
  const Standard_Real aSqToler = theToler3D*theToler3D;

  if(theVec3.SquareMagnitude() <= aSqToler)
  {
    return IntPatchWT_NotConnected;
  }

  if((theVec1.Angle(theVec2) > IntPatch_WLineTool::myMaxConcatAngle) ||
     (theVec1.Angle(theVec3) > IntPatch_WLineTool::myMaxConcatAngle) ||
     (theVec2.Angle(theVec3) > IntPatch_WLineTool::myMaxConcatAngle))
  {
    return IntPatchWT_NotConnected;
  }

  const gp_Pnt aPmid(0.5*(thePtWL1.Value().XYZ()+thePtWL2.Value().XYZ()));

  Standard_Real aNewPar[4] = {0.0, 0.0, 0.0, 0.0};

  //Left-bottom corner
  Standard_Real aParLBC[4] = {0.0, 0.0, 0.0, 0.0};
  theBoxS1.Get(aParLBC[0], aParLBC[1], aNewPar[0], aNewPar[0]);  
  theBoxS2.Get(aParLBC[2], aParLBC[3], aNewPar[0], aNewPar[0]);

  if(!IsIntersectionPoint(aPmid, theS1, theS2, thePtWL1, theToler3D,
                          theArrPeriods, theNewPoint))
  {
    return IntPatchWT_NotConnected;
  }

  if(IsOutOfDomain(theBoxS1, theBoxS2, theNewPoint, theArrPeriods))
  {
    return IntPatchWT_NotConnected;
  }

  Standard_Real aParWL1[4] = {0.0, 0.0, 0.0, 0.0},
                aParWL2[4] = {0.0, 0.0, 0.0, 0.0};
  
  thePtWL1.Parameters(aParWL1[0], aParWL1[1], aParWL1[2], aParWL1[3]);
  thePtWL2.Parameters(aParWL2[0], aParWL2[1], aParWL2[2], aParWL2[3]);
  theNewPoint.Parameters(aNewPar[0], aNewPar[1], aNewPar[2], aNewPar[3]);

  Standard_Boolean isOnBoundary = Standard_False;
  for(Standard_Integer i = 0; i < 4; i++)
  {
    if(IsOnPeriod(aParWL1[i] - aParLBC[i], aParWL2[i] - aParLBC[i], theArrPeriods[i]))
    {
      //Check, if we intersect surface boundary when we will extend Wline1 or Wline2
      //to theNewPoint
      MinMax(aParWL1[i], aParWL2[i]);
      if(theArrPeriods[i] > 0.0)
      {
        if(aNewPar[i] > aParWL2[i])
        {
          //Source situation:
          //
          //---*---------------*------------*-----
          // aParWL1[i]   aParWL2[i]    aNewPar[i]
          //
          //After possible adjusting:
          //
          //---*---------------*------------*-----
          // aParWL1[i]   aNewPar[i]    aParWL2[i]
          //
          //Now we will be able to extend every WLine to
          //aNewPar[i] to make them close to each other.
          //However, it is necessary to add check if we
          //intersect boundary.
          const Standard_Real aPar = aParWL1[i] +
                theArrPeriods[0]*Ceiling((aNewPar[i]-aParWL1[i])/theArrPeriods[0]);
          aParWL1[i] = aParWL2[i];
          aParWL2[i] = aPar;
        }
        else if(aNewPar[i] < aParWL1[i])
        {
          //See comments to main "if".
          //Source situation:
          //
          //---*---------------*------------*-----
          // aNewPar[i]    aParWL1[i]   aParWL2[i]    
          //
          //After possible adjusting:
          //
          //---*---------------*------------*-----
          // aParWL1[i]   aNewPar[i]    aParWL2[i]
          
          const Standard_Real aPar = aParWL2[i] - 
                theArrPeriods[0]*Ceiling((aParWL2[i]-aNewPar[i])/theArrPeriods[0]);
          aParWL2[i] = aParWL1[i];
          aParWL1[i] = aPar;
        }
      }

      if( IsOnPeriod(aParWL1[i] - aParLBC[i], aNewPar[i] - aParLBC[i], theArrPeriods[0]) ||
          IsOnPeriod(aNewPar[i] - aParLBC[i], aParWL2[i] - aParLBC[i], theArrPeriods[0]))
      {
        return IntPatchWT_NotConnected;
      }
      else
      {
        isOnBoundary = Standard_True;
      }
    }
  }

  if(isOnBoundary)
  {
    return IntPatchWT_Singular;
  }

  return IntPatchWT_EachOther;
}

//=======================================================================
//function : CheckArgumentsToJoin
//purpose  : Check if joining is possible
//            (see IntPatch_WLineTool::JoinWLines(...))
//=======================================================================
Standard_Boolean CheckArgumentsToJoin(const gp_Vec& theVec1,
                                      const gp_Vec& theVec2)
{
  // [0, PI] - range
  const Standard_Real anAngle = theVec1.Angle(theVec2);

  return (anAngle < IntPatch_WLineTool::myMaxConcatAngle);
}

//=======================================================================
//function : ExtendTwoWLFirstFirst
//purpose  : Performs extending theWLine1 and theWLine2 through their
//            respecting start point.
//=======================================================================
static void ExtendTwoWLFirstFirst(const Handle(Adaptor3d_HSurface)& theS1,
                                  const Handle(Adaptor3d_HSurface)& theS2,
                                  const Handle(IntPatch_WLine)& theWLine1,
                                  const Handle(IntPatch_WLine)& theWLine2,
                                  const IntSurf_PntOn2S& thePtWL1,
                                  const IntSurf_PntOn2S& thePtWL2,
                                  const gp_Vec& theVec1,
                                  const gp_Vec& theVec2,
                                  const gp_Vec& theVec3,
                                  const Bnd_Box2d& theBoxS1,
                                  const Bnd_Box2d& theBoxS2,
                                  const Standard_Real theToler3D,
                                  const Standard_Real* const theArrPeriods,
                                  unsigned int &theCheckResult,
                                  Standard_Boolean &theHasBeenJoined)
{
  IntSurf_PntOn2S aPOn2S;
  const IntPatchWT_WLsConnectionType aCheckRes = 
                      CheckArgumentsToExtend(theS1, theS2, thePtWL1, thePtWL2, aPOn2S,
                                             theVec1, theVec2, theVec3,
                                             theBoxS1, theBoxS2,
                                             theToler3D, theArrPeriods);

  if(aCheckRes != IntPatchWT_NotConnected)
    theCheckResult |= (IntPatchWT_DisFirstLast | IntPatchWT_DisLastFirst);
  else
    return;

  IntPatch_SpecialPoints::AdjustPointAndVertex(thePtWL1, theArrPeriods, aPOn2S);
  ExtendFirst(theWLine1, aPOn2S);
  IntPatch_SpecialPoints::AdjustPointAndVertex(thePtWL2, theArrPeriods, aPOn2S);
  ExtendFirst(theWLine2, aPOn2S);

  if(theHasBeenJoined || (aCheckRes == IntPatchWT_Singular))
    return;

  Standard_Real aPrm = theWLine1->Vertex(1).ParameterOnLine();
  while(theWLine1->Vertex(1).ParameterOnLine() == aPrm)
    theWLine1->RemoveVertex(1);

  aPrm = theWLine2->Vertex(1).ParameterOnLine();
  while(theWLine2->Vertex(1).ParameterOnLine() == aPrm)
    theWLine2->RemoveVertex(1);

  const Standard_Integer aNbPts = theWLine2->NbPnts();
  for(Standard_Integer aNPt = 2; aNPt <= aNbPts; aNPt++)
  {
    const IntSurf_PntOn2S& aPt = theWLine2->Point(aNPt);
    theWLine1->Curve()->InsertBefore(1, aPt);
  }

  for(Standard_Integer aNVtx = 1; aNVtx <= theWLine1->NbVertex(); aNVtx++)
  {
    IntPatch_Point &aVert = theWLine1->ChangeVertex(aNVtx);
    const Standard_Real aCurParam = aVert.ParameterOnLine();
    aVert.SetParameter(aNbPts+aCurParam-1);
  }

  for(Standard_Integer aNVtx = 1; aNVtx <= theWLine2->NbVertex(); aNVtx++)
  {
    IntPatch_Point &aVert = theWLine2->ChangeVertex(aNVtx);
    const Standard_Real aCurParam = aVert.ParameterOnLine();
    aVert.SetParameter(aNbPts-aCurParam+1);
    theWLine1->AddVertex(aVert, Standard_True);
  }

  theHasBeenJoined = Standard_True;
}

//=======================================================================
//function : ExtendTwoWLFirstLast
//purpose  : Performs extending theWLine1 through its start point and theWLine2
//            through its end point.
//=======================================================================
static void ExtendTwoWLFirstLast(const Handle(Adaptor3d_HSurface)& theS1,
                                 const Handle(Adaptor3d_HSurface)& theS2,
                                 const Handle(IntPatch_WLine)& theWLine1,
                                 const Handle(IntPatch_WLine)& theWLine2,
                                 const IntSurf_PntOn2S& thePtWL1,
                                 const IntSurf_PntOn2S& thePtWL2,
                                 const gp_Vec& theVec1,
                                 const gp_Vec& theVec2,
                                 const gp_Vec& theVec3,
                                 const Bnd_Box2d& theBoxS1,
                                 const Bnd_Box2d& theBoxS2,
                                 const Standard_Real theToler3D,
                                 const Standard_Real* const theArrPeriods,
                                 unsigned int &theCheckResult,
                                 Standard_Boolean &theHasBeenJoined)
{
  IntSurf_PntOn2S aPOn2S;
  const IntPatchWT_WLsConnectionType aCheckRes = 
                      CheckArgumentsToExtend(theS1, theS2, thePtWL1, thePtWL2, aPOn2S,
                                             theVec1, theVec2, theVec3,
                                             theBoxS1, theBoxS2,
                                             theToler3D, theArrPeriods);

  if(aCheckRes != IntPatchWT_NotConnected)
    theCheckResult |= IntPatchWT_DisLastLast;
  else
    return;

  IntPatch_SpecialPoints::AdjustPointAndVertex(thePtWL1, theArrPeriods, aPOn2S);
  ExtendFirst(theWLine1, aPOn2S);
  IntPatch_SpecialPoints::AdjustPointAndVertex(thePtWL2, theArrPeriods, aPOn2S);
  ExtendLast (theWLine2, aPOn2S);

  if(theHasBeenJoined || (aCheckRes == IntPatchWT_Singular))
    return;

  Standard_Real aPrm = theWLine1->Vertex(1).ParameterOnLine();
  while(theWLine1->Vertex(1).ParameterOnLine() == aPrm)
    theWLine1->RemoveVertex(1);

  aPrm = theWLine2->Vertex(theWLine2->NbVertex()).ParameterOnLine();
  while(theWLine2->Vertex(theWLine2->NbVertex()).ParameterOnLine() == aPrm)
    theWLine2->RemoveVertex(theWLine2->NbVertex());

  const Standard_Integer aNbPts = theWLine2->NbPnts();
  for(Standard_Integer aNPt = aNbPts - 1; aNPt >= 1; aNPt--)
  {
    const IntSurf_PntOn2S& aPt = theWLine2->Point(aNPt);
    theWLine1->Curve()->InsertBefore(1, aPt);
  }

  for(Standard_Integer aNVtx = 1; aNVtx <= theWLine1->NbVertex(); aNVtx++)
  {
    IntPatch_Point &aVert = theWLine1->ChangeVertex(aNVtx);
    const Standard_Real aCurParam = aVert.ParameterOnLine();
    aVert.SetParameter(aNbPts+aCurParam-1);
  }

  for(Standard_Integer aNVtx = theWLine2->NbVertex(); aNVtx >= 1; aNVtx--)
  {
    const IntPatch_Point &aVert = theWLine2->Vertex(aNVtx);
    theWLine1->AddVertex(aVert, Standard_True);
  }

  theHasBeenJoined = Standard_True;
}

//=======================================================================
//function : ExtendTwoWLLastFirst
//purpose  : Performs extending theWLine1 through its end point and theWLine2
//            through its start point.
//=======================================================================
static void ExtendTwoWLLastFirst(const Handle(Adaptor3d_HSurface)& theS1,
                                 const Handle(Adaptor3d_HSurface)& theS2,
                                 const Handle(IntPatch_WLine)& theWLine1,
                                 const Handle(IntPatch_WLine)& theWLine2,
                                 const IntSurf_PntOn2S& thePtWL1,
                                 const IntSurf_PntOn2S& thePtWL2,
                                 const gp_Vec& theVec1,
                                 const gp_Vec& theVec2,
                                 const gp_Vec& theVec3,
                                 const Bnd_Box2d& theBoxS1,
                                 const Bnd_Box2d& theBoxS2,
                                 const Standard_Real theToler3D,
                                 const Standard_Real* const theArrPeriods,
                                 unsigned int &theCheckResult,
                                 Standard_Boolean &theHasBeenJoined)
{
  IntSurf_PntOn2S aPOn2S;
  const IntPatchWT_WLsConnectionType aCheckRes = 
                      CheckArgumentsToExtend(theS1, theS2, thePtWL1, thePtWL2, aPOn2S,
                                             theVec1, theVec2, theVec3,
                                             theBoxS1, theBoxS2,
                                             theToler3D, theArrPeriods);

  if(aCheckRes != IntPatchWT_NotConnected)
    theCheckResult |= IntPatchWT_DisLastLast;
  else
    return;

  IntPatch_SpecialPoints::AdjustPointAndVertex(thePtWL1, theArrPeriods, aPOn2S);
  ExtendLast (theWLine1, aPOn2S);
  IntPatch_SpecialPoints::AdjustPointAndVertex(thePtWL2, theArrPeriods, aPOn2S);
  ExtendFirst(theWLine2, aPOn2S);

  if(theHasBeenJoined || (aCheckRes == IntPatchWT_Singular))
  {
    return;
  }

  Standard_Real aPrm = theWLine1->Vertex(theWLine1->NbVertex()).ParameterOnLine();
  while(theWLine1->Vertex(theWLine1->NbVertex()).ParameterOnLine() == aPrm)
    theWLine1->RemoveVertex(theWLine1->NbVertex());

  aPrm = theWLine2->Vertex(1).ParameterOnLine();
  while(theWLine2->Vertex(1).ParameterOnLine() == aPrm)
    theWLine2->RemoveVertex(1);

  const Standard_Integer aNbPts = theWLine1->NbPnts();
  for(Standard_Integer aNPt = 2; aNPt <= theWLine2->NbPnts(); aNPt++)
  {
    const IntSurf_PntOn2S& aPt = theWLine2->Point(aNPt);
    theWLine1->Curve()->Add(aPt);
  }

  for(Standard_Integer aNVtx = 1; aNVtx <= theWLine2->NbVertex(); aNVtx++)
  {
    IntPatch_Point &aVert = theWLine2->ChangeVertex(aNVtx);
    const Standard_Real aCurParam = aVert.ParameterOnLine();
    aVert.SetParameter(aNbPts+aCurParam-1);
    theWLine1->AddVertex(aVert, Standard_False);
  }

  theHasBeenJoined = Standard_True;
}

//=======================================================================
//function : ExtendTwoWLLastLast
//purpose  : 
//=======================================================================
static void ExtendTwoWLLastLast(const Handle(Adaptor3d_HSurface)& theS1,
                                const Handle(Adaptor3d_HSurface)& theS2,
                                const Handle(IntPatch_WLine)& theWLine1,
                                const Handle(IntPatch_WLine)& theWLine2,
                                const IntSurf_PntOn2S& thePtWL1,
                                const IntSurf_PntOn2S& thePtWL2,
                                const gp_Vec& theVec1,
                                const gp_Vec& theVec2,
                                const gp_Vec& theVec3,
                                const Bnd_Box2d& theBoxS1,
                                const Bnd_Box2d& theBoxS2,
                                const Standard_Real theToler3D,
                                const Standard_Real* const theArrPeriods,
                                unsigned int &theCheckResult,
                                Standard_Boolean &theHasBeenJoined)
{
  IntSurf_PntOn2S aPOn2S;
  const IntPatchWT_WLsConnectionType aCheckRes = 
                      CheckArgumentsToExtend(theS1, theS2, thePtWL1, thePtWL2, aPOn2S,
                                             theVec1, theVec2, theVec3,
                                             theBoxS1, theBoxS2,
                                             theToler3D, theArrPeriods);
  
  if(aCheckRes != IntPatchWT_NotConnected)
    theCheckResult |= IntPatchWT_DisLastLast;
  else
    return;

  IntPatch_SpecialPoints::AdjustPointAndVertex(thePtWL1, theArrPeriods, aPOn2S);
  ExtendLast(theWLine1, aPOn2S);
  IntPatch_SpecialPoints::AdjustPointAndVertex(thePtWL2, theArrPeriods, aPOn2S);
  ExtendLast(theWLine2, aPOn2S);

  if(theHasBeenJoined || (aCheckRes == IntPatchWT_Singular))
    return;

  Standard_Real aPrm = theWLine1->Vertex(theWLine1->NbVertex()).ParameterOnLine();
  while(theWLine1->Vertex(theWLine1->NbVertex()).ParameterOnLine() == aPrm)
    theWLine1->RemoveVertex(theWLine1->NbVertex());

  aPrm = theWLine2->Vertex(theWLine2->NbVertex()).ParameterOnLine();
  while(theWLine2->Vertex(theWLine2->NbVertex()).ParameterOnLine() == aPrm)
    theWLine2->RemoveVertex(theWLine2->NbVertex());

  const Standard_Integer aNbPts = theWLine1->NbPnts() + theWLine2->NbPnts();
  for(Standard_Integer aNPt = theWLine2->NbPnts()-1; aNPt >= 1; aNPt--)
  {
    const IntSurf_PntOn2S& aPt = theWLine2->Point(aNPt);
    theWLine1->Curve()->Add(aPt);
  }

  for(Standard_Integer aNVtx = theWLine2->NbVertex(); aNVtx >= 1; aNVtx--)
  {
    IntPatch_Point &aVert = theWLine2->ChangeVertex(aNVtx);
    const Standard_Real aCurParam = aVert.ParameterOnLine();
    aVert.SetParameter(aNbPts - aCurParam);
    theWLine1->AddVertex(aVert, Standard_False);
  }

  theHasBeenJoined = Standard_True;
}

//=======================================================================
//function : JoinWLines
//purpose  :
//=======================================================================
void IntPatch_WLineTool::JoinWLines(IntPatch_SequenceOfLine& theSlin,
                                    IntPatch_SequenceOfPoint& theSPnt,
                                    const Standard_Real theTol3D,
                                    const Standard_Real theU1Period,
                                    const Standard_Real theU2Period,
                                    const Standard_Real theV1Period,
                                    const Standard_Real theV2Period,
                                    const Standard_Real theUfSurf1,
                                    const Standard_Real theUlSurf1,
                                    const Standard_Real theVfSurf1,
                                    const Standard_Real theVlSurf1,
                                    const Standard_Real theUfSurf2,
                                    const Standard_Real theUlSurf2,
                                    const Standard_Real theVfSurf2,
                                    const Standard_Real theVlSurf2)
{
  if(theSlin.Length() == 0)
    return;

  for(Standard_Integer aNumOfLine1 = 1; aNumOfLine1 <= theSlin.Length(); aNumOfLine1++)
  {
    Handle(IntPatch_WLine) aWLine1 (Handle(IntPatch_WLine)::DownCast(theSlin.Value(aNumOfLine1)));

    if(aWLine1.IsNull())
    {//We must have failed to join not-point-lines
      continue;
    }

    const Standard_Integer aNbPntsWL1 = aWLine1->NbPnts();
    const IntSurf_PntOn2S& aPntFW1 = aWLine1->Point(1);
    const IntSurf_PntOn2S& aPntLW1 = aWLine1->Point(aNbPntsWL1);

    for(Standard_Integer aNPt = 1; aNPt <= theSPnt.Length(); aNPt++)
    {
      const IntSurf_PntOn2S aPntCur = theSPnt.Value(aNPt).PntOn2S();

      if( aPntCur.IsSame(aPntFW1, Precision::Confusion()) ||
        aPntCur.IsSame(aPntLW1, Precision::Confusion()))
      {
        theSPnt.Remove(aNPt);
        aNPt--;
      }
    }

    Standard_Boolean hasBeenRemoved = Standard_False;
    for(Standard_Integer aNumOfLine2 = aNumOfLine1 + 1; aNumOfLine2 <= theSlin.Length(); aNumOfLine2++)
    {
      Handle(IntPatch_WLine) aWLine2 (Handle(IntPatch_WLine)::DownCast(theSlin.Value(aNumOfLine2)));

      if(aWLine2.IsNull())
        continue;

      const Standard_Integer aNbPntsWL2 = aWLine2->NbPnts();

      const IntSurf_PntOn2S& aPntFWL1 = aWLine1->Point(1);
      const IntSurf_PntOn2S& aPntLWL1 = aWLine1->Point(aNbPntsWL1);

      const IntSurf_PntOn2S& aPntFWL2 = aWLine2->Point(1);
      const IntSurf_PntOn2S& aPntLWL2 = aWLine2->Point(aNbPntsWL2);

      if(aPntFWL1.IsSame(aPntFWL2, Precision::Confusion()))
      {
        const IntSurf_PntOn2S& aPt1 = aWLine1->Point(2);
        const IntSurf_PntOn2S& aPt2 = aWLine2->Point(2);

        Standard_Boolean aCond = 
              CheckArgumentsToJoin(gp_Vec(aPntFWL1.Value(), aPt1.Value()),
                                   gp_Vec(aPt2.Value(), aPntFWL2.Value()));

        aCond = aCond && !IsSeamOrBound(aPt1, aPt2, aPntFWL1,
                                        theU1Period, theU2Period,
                                        theV1Period, theV2Period,
                                        theUfSurf1, theUlSurf1,
                                        theVfSurf1, theVlSurf1,
                                        theUfSurf2, theUlSurf2,
                                        theVfSurf2, theVlSurf2);
          
        if(aCond)
        {
          aWLine1->ClearVertexes();
          for(Standard_Integer aNPt = 1; aNPt <= aNbPntsWL2; aNPt++)
          {
            const IntSurf_PntOn2S& aPt = aWLine2->Point(aNPt);
            aWLine1->Curve()->InsertBefore(1, aPt);
          }

          aWLine1->ComputeVertexParameters(theTol3D);

          theSlin.Remove(aNumOfLine2);
          aNumOfLine2--;
          hasBeenRemoved = Standard_True;

          continue;
        }
      }

      if(aPntFWL1.IsSame(aPntLWL2, Precision::Confusion()))
      {
        const IntSurf_PntOn2S& aPt1 = aWLine1->Point(2);
        const IntSurf_PntOn2S& aPt2 = aWLine2->Point(aNbPntsWL2-1);

        Standard_Boolean aCond = 
              CheckArgumentsToJoin(gp_Vec(aPntFWL1.Value(), aPt1.Value()),
                                   gp_Vec(aPt2.Value(), aPntLWL2.Value()));

        aCond = aCond && !IsSeamOrBound(aPt1, aPt2, aPntFWL1,
                                        theU1Period, theU2Period,
                                        theV1Period, theV2Period,
                                        theUfSurf1, theUlSurf1,
                                        theVfSurf1, theVlSurf1,
                                        theUfSurf2, theUlSurf2,
                                        theVfSurf2, theVlSurf2);

        if(aCond)
        {
          aWLine1->ClearVertexes();
          for(Standard_Integer aNPt = aNbPntsWL2; aNPt >= 1; aNPt--)
          {
            const IntSurf_PntOn2S& aPt = aWLine2->Point(aNPt);
            aWLine1->Curve()->InsertBefore(1, aPt);
          }

          aWLine1->ComputeVertexParameters(theTol3D);

          theSlin.Remove(aNumOfLine2);
          aNumOfLine2--;
          hasBeenRemoved = Standard_True;

          continue;
        }
      }

      if(aPntLWL1.IsSame(aPntFWL2, Precision::Confusion()))
      {
        const IntSurf_PntOn2S& aPt1 = aWLine1->Point(aNbPntsWL1-1);
        const IntSurf_PntOn2S& aPt2 = aWLine2->Point(2);

        Standard_Boolean aCond = 
              CheckArgumentsToJoin(gp_Vec(aPt1.Value(), aPntLWL1.Value()),
                                   gp_Vec(aPntFWL2.Value(), aPt2.Value()));

        aCond = aCond && !IsSeamOrBound(aPt1, aPt2, aPntLWL1,
                                        theU1Period, theU2Period,
                                        theV1Period, theV2Period,
                                        theUfSurf1, theUlSurf1,
                                        theVfSurf1, theVlSurf1,
                                        theUfSurf2, theUlSurf2,
                                        theVfSurf2, theVlSurf2);

        if(aCond)
        {
          aWLine1->ClearVertexes();
          for(Standard_Integer aNPt = 1; aNPt <= aNbPntsWL2; aNPt++)
          {
            const IntSurf_PntOn2S& aPt = aWLine2->Point(aNPt);
            aWLine1->Curve()->Add(aPt);
          }

          aWLine1->ComputeVertexParameters(theTol3D);

          theSlin.Remove(aNumOfLine2);
          aNumOfLine2--;
          hasBeenRemoved = Standard_True;

          continue;
        }
      }

      if(aPntLWL1.IsSame(aPntLWL2, Precision::Confusion()))
      {
        const IntSurf_PntOn2S& aPt1 = aWLine1->Point(aNbPntsWL1-1);
        const IntSurf_PntOn2S& aPt2 = aWLine2->Point(aNbPntsWL2-1);

        Standard_Boolean aCond = 
              CheckArgumentsToJoin(gp_Vec(aPt1.Value(), aPntLWL1.Value()),
                                   gp_Vec(aPntLWL2.Value(), aPt2.Value()));

        aCond = aCond && !IsSeamOrBound(aPt1, aPt2, aPntLWL1, 
                                        theU1Period, theU2Period,
                                        theV1Period, theV2Period,
                                        theUfSurf1, theUlSurf1,
                                        theVfSurf1, theVlSurf1,
                                        theUfSurf2, theUlSurf2,
                                        theVfSurf2, theVlSurf2);

        if(aCond)
        {
          aWLine1->ClearVertexes();
          for(Standard_Integer aNPt = aNbPntsWL2; aNPt >= 1; aNPt--)
          {
            const IntSurf_PntOn2S& aPt = aWLine2->Point(aNPt);
            aWLine1->Curve()->Add(aPt);
          }

          aWLine1->ComputeVertexParameters(theTol3D);

          theSlin.Remove(aNumOfLine2);
          aNumOfLine2--;
          hasBeenRemoved = Standard_True;

          continue;
        }
      }
    }

    if(hasBeenRemoved)
      aNumOfLine1--;
  }
}

//=======================================================================
//function : ExtendTwoWLines
//purpose  : Performs extending theWLine1 and theWLine2 through their
//            respecting end point.
//=======================================================================
void IntPatch_WLineTool::ExtendTwoWLines(IntPatch_SequenceOfLine& theSlin,
                                         const Handle(Adaptor3d_HSurface)& theS1,
                                         const Handle(Adaptor3d_HSurface)& theS2,
                                         const Standard_Real theToler3D,
                                         const Standard_Real* const theArrPeriods,
                                         const Bnd_Box2d& theBoxS1,
                                         const Bnd_Box2d& theBoxS2)
{
  if(theSlin.Length() < 2)
    return;

  gp_Vec aVec1, aVec2, aVec3;

  for(Standard_Integer aNumOfLine1 = 1; aNumOfLine1 <= theSlin.Length(); aNumOfLine1++)
  {
    Handle(IntPatch_WLine) aWLine1 (Handle(IntPatch_WLine)::
                                    DownCast(theSlin.Value(aNumOfLine1)));

    if(aWLine1.IsNull())
    {//We must have failed to join not-point-lines
      continue;
    }
    
    const Standard_Integer aNbPntsWL1 = aWLine1->NbPnts();

    if(aWLine1->Vertex(1).ParameterOnLine() != 1)
      continue;

    if(aWLine1->Vertex(aWLine1->NbVertex()).ParameterOnLine() != aWLine1->NbPnts())
      continue;

    const IntSurf_PntOn2S& aPntFWL1 = aWLine1->Point(1);
    const IntSurf_PntOn2S& aPntFp1WL1 = aWLine1->Point(2);

    const IntSurf_PntOn2S& aPntLWL1 = aWLine1->Point(aNbPntsWL1);
    const IntSurf_PntOn2S& aPntLm1WL1 = aWLine1->Point(aNbPntsWL1-1);

    //Enable/Disable of some ckeck. Bit-mask is used for it.
    //E.g. if 1st point of aWLine1 matches with
    //1st point of aWLine2 then we do not need in check
    //1st point of aWLine1 and last point of aWLine2 etc.
    unsigned int aCheckResult = IntPatchWT_EnAll;

    //If aWLine1 is already connected with another Wline then
    //there is no point in extending.
    for(Standard_Integer aNumOfLine2 = aNumOfLine1 + 1;
        aNumOfLine2 <= theSlin.Length(); aNumOfLine2++)
    {
      Handle(IntPatch_WLine) aWLine2 (Handle(IntPatch_WLine)::
                                    DownCast(theSlin.Value(aNumOfLine2)));

      if(aWLine2.IsNull())
        continue;

      const IntSurf_PntOn2S& aPntFWL2 = aWLine2->Point(1);
      const IntSurf_PntOn2S& aPntLWL2 = aWLine2->Point(aWLine2->NbPnts());

      if( aPntFWL1.IsSame(aPntFWL2, theToler3D) ||
          aPntFWL1.IsSame(aPntLWL2, theToler3D) )
      {
        aCheckResult |= IntPatchWT_DisFirstFirst | IntPatchWT_DisFirstLast;
      }

      if( aPntLWL1.IsSame(aPntFWL2, theToler3D) ||
          aPntLWL1.IsSame(aPntFWL2, theToler3D))
      {
        aCheckResult |= IntPatchWT_DisLastFirst | IntPatchWT_DisLastLast;
      }
    }

    if(aCheckResult == (IntPatchWT_DisFirstFirst | IntPatchWT_DisFirstLast |
                        IntPatchWT_DisLastFirst | IntPatchWT_DisLastLast))
      continue;

    for(Standard_Integer aNumOfLine2 = aNumOfLine1 + 1;
        aNumOfLine2 <= theSlin.Length(); aNumOfLine2++)
    {
      Handle(IntPatch_WLine) aWLine2 (Handle(IntPatch_WLine)::
                                    DownCast(theSlin.Value(aNumOfLine2)));

      if(aWLine2.IsNull())
        continue;

      if(aWLine2->Vertex(1).ParameterOnLine() != 1)
        continue;

      if(aWLine2->Vertex(aWLine2->NbVertex()).ParameterOnLine() != aWLine2->NbPnts())
        continue;

      Standard_Boolean hasBeenJoined = Standard_False;

      const Standard_Integer aNbPntsWL2 = aWLine2->NbPnts();

      const IntSurf_PntOn2S& aPntFWL2 = aWLine2->Point(1);
      const IntSurf_PntOn2S& aPntFp1WL2 = aWLine2->Point(2);

      const IntSurf_PntOn2S& aPntLWL2 = aWLine2->Point(aNbPntsWL2);
      const IntSurf_PntOn2S& aPntLm1WL2 = aWLine2->Point(aNbPntsWL2-1);
      
      //if(!(aCheckResult & IntPatchWT_DisFirstFirst))
      {// First/First
        aVec1.SetXYZ(aPntFp1WL1.Value().XYZ() - aPntFWL1.Value().XYZ());
        aVec2.SetXYZ(aPntFWL2.Value().XYZ() - aPntFp1WL2.Value().XYZ());
        aVec3.SetXYZ(aPntFWL1.Value().XYZ() - aPntFWL2.Value().XYZ());

        ExtendTwoWLFirstFirst(theS1, theS2, aWLine1, aWLine2, aPntFWL1, aPntFWL2,
                              aVec1, aVec2, aVec3, theBoxS1, theBoxS2, theToler3D,
                              theArrPeriods, aCheckResult, hasBeenJoined);
      }

      if(!(aCheckResult & IntPatchWT_DisFirstLast))
      {// First/Last
        aVec1.SetXYZ(aPntFp1WL1.Value().XYZ() - aPntFWL1.Value().XYZ());
        aVec2.SetXYZ(aPntLWL2.Value().XYZ() - aPntLm1WL2.Value().XYZ());
        aVec3.SetXYZ(aPntFWL1.Value().XYZ() - aPntLWL2.Value().XYZ());

        ExtendTwoWLFirstLast(theS1, theS2, aWLine1, aWLine2, aPntFWL1, aPntLWL2,
                             aVec1, aVec2, aVec3, theBoxS1, theBoxS2, theToler3D,
                             theArrPeriods, aCheckResult, hasBeenJoined);
      }

      if(!(aCheckResult & IntPatchWT_DisLastFirst))
      {// Last/First
        aVec1.SetXYZ(aPntLWL1.Value().XYZ() - aPntLm1WL1.Value().XYZ());
        aVec2.SetXYZ(aPntFp1WL2.Value().XYZ() - aPntFWL2.Value().XYZ());
        aVec3.SetXYZ(aPntFWL2.Value().XYZ() - aPntLWL1.Value().XYZ());

        ExtendTwoWLLastFirst(theS1, theS2, aWLine1, aWLine2, aPntLWL1, aPntFWL2,
                             aVec1, aVec2, aVec3, theBoxS1, theBoxS2, theToler3D,
                             theArrPeriods, aCheckResult, hasBeenJoined);
      }

      if(!(aCheckResult & IntPatchWT_DisLastLast))
      {// Last/Last
        aVec1.SetXYZ(aPntLWL1.Value().XYZ() - aPntLm1WL1.Value().XYZ());
        aVec2.SetXYZ(aPntLm1WL2.Value().XYZ() - aPntLWL2.Value().XYZ());
        aVec3.SetXYZ(aPntLWL2.Value().XYZ() - aPntLWL1.Value().XYZ());

        ExtendTwoWLLastLast(theS1, theS2, aWLine1, aWLine2, aPntLWL1, aPntLWL2,
                            aVec1, aVec2, aVec3, theBoxS1, theBoxS2, theToler3D,
                            theArrPeriods, aCheckResult, hasBeenJoined);
      }

      if(hasBeenJoined)
      {
        theSlin.Remove(aNumOfLine2);
        aNumOfLine2--;
      }
    }
  }
}
