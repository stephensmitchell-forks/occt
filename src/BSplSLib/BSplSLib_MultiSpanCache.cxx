// Copyright (c) 2014 OPEN CASCADE SAS
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

#include <BSplSLib_MultiSpanCache.hxx>

#include <BSplCLib.hxx>
#include <ElCLib.hxx>
#include <Precision.hxx>

BSplSLib_MultiSpanCache::BSplSLib_MultiSpanCache(const Standard_Integer      theDegreeU,
                                                 const Standard_Boolean      thePeriodicU,
                                                 const TColStd_Array1OfReal& theFlatKnotsU,
                                                 const Standard_Integer      theDegreeV,
                                                 const Standard_Boolean      thePeriodicV,
                                                 const TColStd_Array1OfReal& theFlatKnotsV,
                                                 const TColgp_Array2OfPnt&   thePoles,
                                                 const TColStd_Array2OfReal* theWeights,
                                                 const Standard_Integer      theMaxSpans)
  : myDegreeU(theDegreeU),
    myDegreeV(theDegreeV),
    myPeriodicU(thePeriodicU),
    myPeriodicV(thePeriodicV),
    myFlatKnotsU(theFlatKnotsU),
    myFlatKnotsV(theFlatKnotsV),
    myPoles(thePoles),
    myWeights(theWeights),
    mySingleSpanU(0),
    mySingleSpanV(0)
{
  Clear();

  Standard_Integer aMaxSpansCount = theMaxSpans;

  myFirstKnotU = myFlatKnotsU.Value(myFlatKnotsU.Lower() + myDegreeU);
  myLastKnotU  = myFlatKnotsU.Value(myFlatKnotsU.Upper() - myDegreeU);
  myFirstKnotV = myFlatKnotsV.Value(myFlatKnotsV.Lower() + myDegreeV);
  myLastKnotV  = myFlatKnotsV.Value(myFlatKnotsV.Upper() - myDegreeV);

  // Check the surface has one span at all
  Standard_Boolean isSingleSpanU = !myPeriodicU && myFlatKnotsU.Length() == (myDegreeU + 1) * 2;
  Standard_Boolean isSingleSpanV = !myPeriodicV && myFlatKnotsV.Length() == (myDegreeV + 1) * 2;
  if (isSingleSpanU || isSingleSpanV)
  {
    Standard_Integer aTemp;
    SpanIndex(myFirstKnotU, myFirstKnotV, 
              isSingleSpanU ? mySingleSpanU : aTemp,
              isSingleSpanV ? mySingleSpanV : aTemp);

    if (isSingleSpanU && isSingleSpanV)
    {
      SetSingleSpan(Standard_True);
      aMaxSpansCount = 1;
    }
  }

  SetMaxSpansCached(aMaxSpansCount);
}

void BSplSLib_MultiSpanCache::Clear()
{
  BSplCLib_MultiSpanCacheStorage<BSplSLib_Cache>::Clear();

  myLastU.myParameter = Precision::Infinite();
  myLastU.mySpan = -1;
  myLastV.myParameter = Precision::Infinite();
  myLastV.mySpan = -1;
}

void BSplSLib_MultiSpanCache::D0(const Standard_Real theU,
                                 const Standard_Real theV,
                                       gp_Pnt&       thePoint)
{
  Standard_Real aU = theU;
  Standard_Real aV = theV;
  Handle(BSplSLib_Cache) aCache = FindCache(aU, aV);
  aCache->D0(aU, aV, thePoint);
}

void BSplSLib_MultiSpanCache::D1(const Standard_Real theU,
                                 const Standard_Real theV, 
                                       gp_Pnt&       thePoint, 
                                       gp_Vec&       theTangentU, 
                                       gp_Vec&       theTangentV)
{
  Standard_Real aU = theU;
  Standard_Real aV = theV;
  Handle(BSplSLib_Cache) aCache = FindCache(aU, aV);
  aCache->D1(aU, aV, thePoint, theTangentU, theTangentV);
}

void BSplSLib_MultiSpanCache::D2(const Standard_Real theU,
                                 const Standard_Real theV, 
                                       gp_Pnt&       thePoint, 
                                       gp_Vec&       theTangentU, 
                                       gp_Vec&       theTangentV, 
                                       gp_Vec&       theCurvatureU, 
                                       gp_Vec&       theCurvatureV, 
                                       gp_Vec&       theCurvatureUV)
{
  Standard_Real aU = theU;
  Standard_Real aV = theV;
  Handle(BSplSLib_Cache) aCache = FindCache(aU, aV);
  aCache->D2(aU, aV, thePoint, theTangentU, theTangentV,
             theCurvatureU, theCurvatureV, theCurvatureUV);
}

Standard_Integer BSplSLib_MultiSpanCache::SpanIndex(Standard_Real&    theU,
                                                    Standard_Real&    theV,
                                                    Standard_Integer& theSpanIndexU,
                                                    Standard_Integer& theSpanIndexV) const
{
  // Normalize the parameters for periodical B-splines
  if (myPeriodicU && (theU > myLastKnotU || theU < myFirstKnotU))
    theU = ElCLib::InPeriod(theU, myFirstKnotU, myLastKnotU);
  if (myPeriodicV && (theV > myLastKnotV || theV < myFirstKnotV))
    theV = ElCLib::InPeriod(theV, myFirstKnotV, myLastKnotV);

  // Do not want to use LocateParameter due to lack of performance.
  // Just check the last used cache is valid for the given parameters.
  // In case the cache is correct, values of theSpanIndexU and theSpanIndexV have no sense.
  if (!LastCache().IsNull() && (IsSingleSpan() || LastCache()->IsCacheValid(theU, theV)))
    return LastSpan();

  // Get indices along each parameter (U, V)
  theSpanIndexU = mySingleSpanU;
  theSpanIndexV = mySingleSpanV;
  if (theSpanIndexU == 0)
  {
    if (theU == myLastU.myParameter) // the parameters should be equal to a bit
      theSpanIndexU = myLastU.mySpan;
    else
      BSplCLib::LocateParameter(myDegreeU, myFlatKnotsU, BSplCLib::NoMults(), theU,
                                myPeriodicU, theSpanIndexU, theU);
  }
  if (theSpanIndexV == 0)
  {
    if (theV == myLastV.myParameter) // the parameters should be equal to a bit
      theSpanIndexV = myLastV.mySpan;
    else
      BSplCLib::LocateParameter(myDegreeV, myFlatKnotsV, BSplCLib::NoMults(), theV,
                                myPeriodicV, theSpanIndexV, theV);
  }

  return myFlatKnotsV.Length() * theSpanIndexU + theSpanIndexV;
}

const Handle(BSplSLib_Cache)& BSplSLib_MultiSpanCache::FindCache(Standard_Real& theU,
                                                                 Standard_Real& theV)
{
  Standard_Integer aSpanIndexU = 0;
  Standard_Integer aSpanIndexV = 0;
  Standard_Integer aSpanIndex = SpanIndex(theU, theV, aSpanIndexU, aSpanIndexV);
  // check the cache is used at previous call,
  // if not, try to find it in the list of already cached spans
  if (aSpanIndex == LastSpan() || SetLatestCache(aSpanIndex))
    return LastCache();
  // store last found indices
  myLastU.myParameter = theU;
  myLastU.mySpan = aSpanIndexU;
  myLastV.myParameter = theV;
  myLastV.mySpan = aSpanIndexV;
  // build new cache
  return NewCache(aSpanIndexU, aSpanIndexV, aSpanIndex);
}

const Handle(BSplSLib_Cache)&
BSplSLib_MultiSpanCache::NewCache(const Standard_Integer theSpanIndexU,
                                  const Standard_Integer theSpanIndexV,
                                  const Standard_Integer theSpanIndexFull)
{
  Handle(BSplSLib_Cache) aNewCache;
  if (CreateNewCache(theSpanIndexFull, aNewCache))
    aNewCache->BuildCache(myDegreeU, myPeriodicU, myFlatKnotsU, theSpanIndexU,
                          myDegreeV, myPeriodicV, myFlatKnotsV, theSpanIndexV,
                          myPoles, myWeights);
  return LastCache();
}
