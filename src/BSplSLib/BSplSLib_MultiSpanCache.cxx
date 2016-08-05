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

BSplSLib_MultiSpanCache::BSplSLib_MultiSpanCache(const Standard_Integer         theDegreeU,
                                                 const Standard_Boolean         thePeriodicU,
                                                 const Standard_Integer         theDegreeV,
                                                 const Standard_Boolean         thePeriodicV,
                                                 const TColgp_Array2OfPnt&      thePoles,
                                                 const TColStd_Array2OfReal*    theWeights)
  : myDegreeU(theDegreeU),
    myDegreeV(theDegreeV),
    myPeriodicU(thePeriodicU),
    myPeriodicV(thePeriodicV),
    myKnotsU(NULL),
    myKnotsV(NULL),
    myMultsU(NULL),
    myMultsV(NULL),
    myFlatKnotsU(NULL),
    myFlatKnotsV(NULL),
    myFirstKnotU(0.),
    myFirstKnotV(0.),
    myLastKnotU(1.),
    myLastKnotV(1.),
    myPoles(thePoles),
    myWeights(theWeights),
    mySingleSpanU(1),
    mySingleSpanV(1),
    myCaches(1)
{
  Clear();

  // Build cache for single span
  TColStd_Array1OfReal aFlatKnotsU(BSplCLib::FlatBezierKnots(myDegreeU), 1, (myDegreeU + 1) * 2);
  TColStd_Array1OfReal aFlatKnotsV(BSplCLib::FlatBezierKnots(myDegreeV), 1, (myDegreeV + 1) * 2);
  myCaches[myLastCacheInd] = new BSplSLib_Cache;
  myCaches[myLastCacheInd]->BuildCache(myDegreeU, myPeriodicU, aFlatKnotsU, myDegreeU + 1,
                                       myDegreeV, myPeriodicV, aFlatKnotsV, myDegreeV + 1,
                                       myPoles, myWeights);
}

BSplSLib_MultiSpanCache::BSplSLib_MultiSpanCache(const Standard_Integer         theDegreeU,
                                                 const Standard_Boolean         thePeriodicU,
                                                 const TColStd_Array1OfReal*    theKnotsU,
                                                 const TColStd_Array1OfInteger* theMultsU,
                                                 const TColStd_Array1OfReal*    theFlatKnotsU,
                                                 const Standard_Integer         theDegreeV,
                                                 const Standard_Boolean         thePeriodicV,
                                                 const TColStd_Array1OfReal*    theKnotsV,
                                                 const TColStd_Array1OfInteger* theMultsV,
                                                 const TColStd_Array1OfReal*    theFlatKnotsV,
                                                 const TColgp_Array2OfPnt&      thePoles,
                                                 const TColStd_Array2OfReal*    theWeights)
  : myDegreeU(theDegreeU),
    myDegreeV(theDegreeV),
    myPeriodicU(thePeriodicU),
    myPeriodicV(thePeriodicV),
    myKnotsU(theKnotsU),
    myKnotsV(theKnotsV),
    myMultsU(theMultsU),
    myMultsV(theMultsV),
    myFlatKnotsU(theFlatKnotsU),
    myFlatKnotsV(theFlatKnotsV),
    myFirstKnotU(theKnotsU->First()),
    myFirstKnotV(theKnotsV->First()),
    myLastKnotU(theKnotsU->Last()),
    myLastKnotV(theKnotsV->Last()),
    myPoles(thePoles),
    myWeights(theWeights),
    mySingleSpanU(0),
    mySingleSpanV(0),
    myCaches((theKnotsU->Upper() - theKnotsU->Lower()) * (theKnotsV->Upper() - theKnotsV->Lower()))
{
  Clear();

  Standard_Boolean isSingleSpanU = myKnotsU->Length() == 2;
  Standard_Boolean isSingleSpanV = myKnotsV->Length() == 2;

  // Check the surface has one span at all
  if (isSingleSpanU || isSingleSpanV)
  {
    Standard_Integer aSpanIndex; // works as temporary index if surface has more than one span
    Standard_Real aTempU = myKnotsU->First();
    Standard_Real aTempV = myKnotsV->First();
    SpanIndex(aTempU, aTempV,
              isSingleSpanU ? mySingleSpanU : aSpanIndex,
              isSingleSpanV ? mySingleSpanV : aSpanIndex);
  }
}

void BSplSLib_MultiSpanCache::Clear()
{
  myLastCacheInd = 0;
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
  const Handle(BSplSLib_Cache)& aCache = FindCache(aU, aV);
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
  const Handle(BSplSLib_Cache)& aCache = FindCache(aU, aV);
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
  const Handle(BSplSLib_Cache)& aCache = FindCache(aU, aV);
  aCache->D2(aU, aV, thePoint, theTangentU, theTangentV,
             theCurvatureU, theCurvatureV, theCurvatureUV);
}

Standard_Integer BSplSLib_MultiSpanCache::SpanIndex(const Standard_Real theU,
                                                    const Standard_Real theV,
                                                    Standard_Integer&   theSpanIndexU,
                                                    Standard_Integer&   theSpanIndexV) const
{
  // Get indices along each parameter (U, V)
  theSpanIndexU = mySingleSpanU;
  theSpanIndexV = mySingleSpanV;
  if (IsSingleSpan())
    return 0;

  if (theSpanIndexU == 0)
  {
    if (theU == myLastU.myParameter) // the parameters should be equal to a bit
      theSpanIndexU = myLastU.mySpan;
    else
    {
      Standard_Real aNewU = theU; // unused parameter, because theU is already in period
      BSplCLib::LocateParameter(myDegreeU, *myKnotsU, myMultsU, theU, myPeriodicU,
                                theSpanIndexU, aNewU);
    }
  }
  if (theSpanIndexV == 0)
  {
    if (theV == myLastV.myParameter) // the parameters should be equal to a bit
      theSpanIndexV = myLastV.mySpan;
    else
    {
      Standard_Real aNewV = theV; // unused parameter, because theV is already in period
      BSplCLib::LocateParameter(myDegreeV, *myKnotsV, myMultsV, theV, myPeriodicV,
                                theSpanIndexV, aNewV);
    }
  }
  return (theSpanIndexU - myKnotsU->Lower()) * (myKnotsV->Upper() - myKnotsV->Lower()) +
          theSpanIndexV - myKnotsV->Lower();
}

const Handle(BSplSLib_Cache)& BSplSLib_MultiSpanCache::FindCache(Standard_Real& theU, Standard_Real& theV)
{
  // Normalize the parameters for periodical B-splines
  if (myPeriodicU && (theU > myLastKnotU || theU < myFirstKnotU))
    theU = ElCLib::InPeriod(theU, myFirstKnotU, myLastKnotU);
  if (myPeriodicV && (theV > myLastKnotV || theV < myFirstKnotV))
    theV = ElCLib::InPeriod(theV, myFirstKnotV, myLastKnotV);

  // Do not want to search in due to lack of performance.
  // Just check the last used cache is valid for the given parameters.
  if (!myCaches[myLastCacheInd].IsNull() &&
      (IsSingleSpan() || myCaches[myLastCacheInd]->IsCacheValid(theU, theV)))
    return myCaches[myLastCacheInd];

  Standard_Integer aSpanIndexU = 0;
  Standard_Integer aSpanIndexV = 0;
  myLastCacheInd = SpanIndex(theU, theV, aSpanIndexU, aSpanIndexV);
  // store last found indices
  myLastU.myParameter = theU;
  myLastU.mySpan = aSpanIndexU;
  myLastV.myParameter = theV;
  myLastV.mySpan = aSpanIndexV;

  // check the cache is already built
  Handle(BSplSLib_Cache)& aCache = myCaches[myLastCacheInd];
  if (aCache.IsNull())
  {
    // calculate index of cache for the flat knots
    aSpanIndexU = BSplCLib::FlatIndex(myDegreeU, aSpanIndexU, *myMultsU, myPeriodicU);
    aSpanIndexV = BSplCLib::FlatIndex(myDegreeV, aSpanIndexV, *myMultsV, myPeriodicV);
    // build new cache
    aCache = new BSplSLib_Cache;
    aCache->BuildCache(myDegreeU, myPeriodicU, *myFlatKnotsU, aSpanIndexU,
                       myDegreeV, myPeriodicV, *myFlatKnotsV, aSpanIndexV,
                       myPoles, myWeights);
  }

  return aCache;
}
