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

#include <BSplSLib_Cache.hxx>
#include <BSplSLib.hxx>

#include <NCollection_LocalArray.hxx>

#include <TColgp_HArray2OfPnt.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <TColStd_HArray2OfReal.hxx>


IMPLEMENT_STANDARD_RTTIEXT(BSplSLib_Cache,Standard_Transient)

//! Converts handle of array of Standard_Real into the pointer to Standard_Real
static Standard_Real* ConvertArray(const Handle(TColStd_HArray2OfReal)& theHArray)
{
  const TColStd_Array2OfReal& anArray = theHArray->Array2();
  return (Standard_Real*) &(anArray(anArray.LowerRow(), anArray.LowerCol()));
}


BSplSLib_Cache::BSplSLib_Cache()
  : myIsRational(Standard_False)
{
  mySpanStart[0] = mySpanStart[1] = 0.0;
  mySpanLength[0] = mySpanLength[1] = 0.0;
  mySpanLengthRec[0] = mySpanLengthRec[1] = 0.0;
  mySpanIndex[0] = mySpanIndex[1] = 0;
  mySpanIndexMin[0] = mySpanIndexMin[1] = 0;
  mySpanIndexMax[0] = mySpanIndexMax[1] = 0;
  myDegree[0] = myDegree[1] = 0;
}


Standard_Boolean BSplSLib_Cache::IsCacheValid(Standard_Real theParameterU,
                                              Standard_Real theParameterV) const
{
  Standard_Real aDelta0 = theParameterU - mySpanStart[0];
  Standard_Real aDelta1 = theParameterV - mySpanStart[1];
  return ((aDelta0 >= -mySpanLength[0] || mySpanIndex[0] == mySpanIndexMin[0]) &&
          (aDelta0 <   mySpanLength[0] || mySpanIndex[0] == mySpanIndexMax[0]) &&
          (aDelta1 >= -mySpanLength[1] || mySpanIndex[1] == mySpanIndexMin[1]) &&
          (aDelta1 <   mySpanLength[1] || mySpanIndex[1] == mySpanIndexMax[1]));
}


void BSplSLib_Cache::BuildCache(const Standard_Integer      theDegreeU, 
                                const Standard_Boolean      thePeriodicU, 
                                const TColStd_Array1OfReal& theFlatKnotsU, 
                                const Standard_Integer      theCachedSpanU,
                                const Standard_Integer      theDegreeV,
                                const Standard_Boolean      thePeriodicV, 
                                const TColStd_Array1OfReal& theFlatKnotsV, 
                                const Standard_Integer      theCachedSpanV,
                                const TColgp_Array2OfPnt&   thePoles,
                                const TColStd_Array2OfReal* theWeights)
{
  Standard_Integer aMinDegree = Min(theDegreeU, theDegreeV);
  Standard_Integer aMaxDegree = Max(theDegreeU, theDegreeV);

  // Change the size of cached data if needed
  myIsRational = (theWeights != NULL);
  Standard_Integer aPWColNumber = myIsRational ? 4 : 3;
  if (theDegreeU > myDegree[0] || theDegreeV > myDegree[1])
    myPolesWeights = new TColStd_HArray2OfReal(1, aMaxDegree + 1, 1, aPWColNumber * (aMinDegree + 1));

  myDegree[0] = theDegreeU;
  myDegree[1] = theDegreeV;
  mySpanIndex[0] = theCachedSpanU;
  mySpanIndex[1] = theCachedSpanV;

  mySpanLength[0] = (theFlatKnotsU.Value(mySpanIndex[0] + 1) - theFlatKnotsU.Value(mySpanIndex[0])) * 0.5;
  mySpanLengthRec[0] = 1.0 / mySpanLength[0];
  mySpanStart[0]  = theFlatKnotsU.Value(mySpanIndex[0]) + mySpanLength[0];

  mySpanLength[1] = (theFlatKnotsV.Value(mySpanIndex[1] + 1) - theFlatKnotsV.Value(mySpanIndex[1])) * 0.5;
  mySpanLengthRec[1] = 1.0 / mySpanLength[1];
  mySpanStart[1] = theFlatKnotsV.Value(mySpanIndex[1]) + mySpanLength[1];

  mySpanIndexMin[0] = thePeriodicU ? 0 : theDegreeU + 1;
  mySpanIndexMax[0] = theFlatKnotsU.Length() - 1 - theDegreeU;
  mySpanIndexMin[1] = thePeriodicV ? 0 : theDegreeV + 1;
  mySpanIndexMax[1] = theFlatKnotsV.Length() - 1 - theDegreeV;

  // Calculate new cache data
  BSplSLib::BuildCache(mySpanStart[0],  mySpanStart[1], 
                       mySpanLength[0], mySpanLength[1], 
                       thePeriodicU,    thePeriodicV, 
                       theDegreeU,      theDegreeV, 
                       mySpanIndex[0],  mySpanIndex[1], 
                       theFlatKnotsU,   theFlatKnotsV, 
                       thePoles, theWeights, myPolesWeights->ChangeArray2());
}


void BSplSLib_Cache::D0(const Standard_Real theU, 
                        const Standard_Real theV, 
                              gp_Pnt&       thePoint) const
{
  Standard_Real aNewU = (theU - mySpanStart[0]) / mySpanLength[0];
  Standard_Real aNewV = (theV - mySpanStart[1]) / mySpanLength[1];

  Standard_Real* aPolesArray = ConvertArray(myPolesWeights);
  Standard_Real aPoint[4];

  Standard_Integer aDimension = myIsRational ? 4 : 3;
  Standard_Integer aCacheCols = myPolesWeights->RowLength();
  Standard_Integer aMinMaxDegree[2] = {Min(myDegree[0], myDegree[1]), 
                                       Max(myDegree[0], myDegree[1])};
  Standard_Real aParameters[2];
  if (myDegree[0] > myDegree[1])
  {
    aParameters[0] = aNewV;
    aParameters[1] = aNewU;
  }
  else
  {
    aParameters[0] = aNewU;
    aParameters[1] = aNewV;
  }

  NCollection_LocalArray<Standard_Real> aTransientCoeffs(aCacheCols); // array for intermediate results

  // Calculate intermediate value of cached polynomial along columns
  PLib::NoDerivativeEvalPolynomial(aParameters[1], aMinMaxDegree[1],
                                   aCacheCols, aMinMaxDegree[1] * aCacheCols,
                                   aPolesArray[0], aTransientCoeffs[0]);

  // Calculate total value
  PLib::NoDerivativeEvalPolynomial(aParameters[0], aMinMaxDegree[0],
                                   aDimension, aDimension * aMinMaxDegree[0],
                                   aTransientCoeffs[0], aPoint[0]);

  thePoint.SetCoord(aPoint[0], aPoint[1], aPoint[2]);
  if (myIsRational)
    thePoint.ChangeCoord().Divide(aPoint[3]);
}


void BSplSLib_Cache::D1(const Standard_Real theU, 
                        const Standard_Real theV, 
                              gp_Pnt&       thePoint, 
                              gp_Vec&       theTangentU, 
                              gp_Vec&       theTangentV) const
{
  Standard_Real aNewU = (theU - mySpanStart[0]) * mySpanLengthRec[0];
  Standard_Real aNewV = (theV - mySpanStart[1]) * mySpanLengthRec[1];

  Standard_Real* aPolesArray = ConvertArray(myPolesWeights);
  static const Standard_Integer PNT_DERIV_LEN = 16;
  Standard_Real aPntDeriv[PNT_DERIV_LEN]; // result storage (point and derivative coordinates)
  memset(aPntDeriv, 0, PNT_DERIV_LEN * sizeof(Standard_Real));

  Standard_Integer aDimension = myIsRational ? 4 : 3;
  Standard_Integer aCacheCols = myPolesWeights->RowLength();
  Standard_Integer aMinMaxDegree[2] = {Min(myDegree[0], myDegree[1]), 
                                       Max(myDegree[0], myDegree[1])};

  Standard_Real aParameters[2];
  if (myDegree[0] > myDegree[1])
  {
    aParameters[0] = aNewV;
    aParameters[1] = aNewU;
  }
  else
  {
    aParameters[0] = aNewU;
    aParameters[1] = aNewV;
  }

  NCollection_LocalArray<Standard_Real> aTransientCoeffs(aCacheCols<<1); // array for intermediate results

  // Calculate intermediate values and derivatives of bivariate polynomial along variable with maximal degree
  PLib::EvalPolynomial(aParameters[1], 1, aMinMaxDegree[1], aCacheCols, aPolesArray[0], aTransientCoeffs[0]);

  // Calculate a point on surface and a derivative along variable with minimal degree
  PLib::EvalPolynomial(aParameters[0], 1, aMinMaxDegree[0], aDimension, aTransientCoeffs[0], aPntDeriv[0]);

  // Calculate derivative along variable with maximal degree
  PLib::NoDerivativeEvalPolynomial(aParameters[0], aMinMaxDegree[0], aDimension, 
                                   aMinMaxDegree[0] * aDimension, aTransientCoeffs[aCacheCols], 
                                   aPntDeriv[aDimension<<1]);

  Standard_Real* aResult = aPntDeriv;
  Standard_Real aTempStorage[PNT_DERIV_LEN];
  if (myIsRational) // calculate derivatives divided by weight's derivatives
  {
    BSplSLib::RationalDerivative(1, 1, 1, 1, aPntDeriv[0], aTempStorage[0]);
    aResult = aTempStorage;
    aDimension--;
  }

  thePoint.SetCoord(aResult[0], aResult[1], aResult[2]);
  if (myDegree[0] > myDegree[1])
  {
    theTangentV.SetCoord(aResult[aDimension], aResult[aDimension + 1], aResult[aDimension + 2]);
    Standard_Integer aShift = aDimension<<1;
    theTangentU.SetCoord(aResult[aShift], aResult[aShift + 1], aResult[aShift + 2]);
  }
  else
  {
    theTangentU.SetCoord(aResult[aDimension], aResult[aDimension + 1], aResult[aDimension + 2]);
    Standard_Integer aShift = aDimension<<1;
    theTangentV.SetCoord(aResult[aShift], aResult[aShift + 1], aResult[aShift + 2]);
  }
  theTangentU.Multiply(mySpanLengthRec[0]);
  theTangentV.Multiply(mySpanLengthRec[1]);
}


void BSplSLib_Cache::D2(const Standard_Real theU, 
                        const Standard_Real theV, 
                              gp_Pnt&       thePoint, 
                              gp_Vec&       theTangentU, 
                              gp_Vec&       theTangentV, 
                              gp_Vec&       theCurvatureU, 
                              gp_Vec&       theCurvatureV, 
                              gp_Vec&       theCurvatureUV) const
{
  Standard_Real aNewU = (theU - mySpanStart[0]) * mySpanLengthRec[0];
  Standard_Real aNewV = (theV - mySpanStart[1]) * mySpanLengthRec[1];

  Standard_Real* aPolesArray = ConvertArray(myPolesWeights);
  static const Standard_Integer PNT_DERIV_LEN = 36;
  Standard_Real aPntDeriv[PNT_DERIV_LEN]; // result storage (point and derivative coordinates)
  memset(aPntDeriv, 0, PNT_DERIV_LEN * sizeof(Standard_Real));

  Standard_Integer aDimension = myIsRational ? 4 : 3;
  Standard_Integer aCacheCols = myPolesWeights->RowLength();
  Standard_Integer aMinMaxDegree[2] = {Min(myDegree[0], myDegree[1]), 
                                       Max(myDegree[0], myDegree[1])};

  Standard_Real aParameters[2];
  if (myDegree[0] > myDegree[1])
  {
    aParameters[0] = aNewV;
    aParameters[1] = aNewU;
  }
  else
  {
    aParameters[0] = aNewU;
    aParameters[1] = aNewV;
  }

  NCollection_LocalArray<Standard_Real> aTransientCoeffs(3 * aCacheCols); // array for intermediate results
  // Calculating derivative to be evaluate and
  // nulling transient coefficients when max or min derivative is less than 2
  Standard_Integer aMinMaxDeriv[2] = {Min(2, aMinMaxDegree[0]), 
                                      Min(2, aMinMaxDegree[1])};
  for (Standard_Integer i = aMinMaxDeriv[1] + 1; i < 3; i++)
  {
    Standard_Integer index = i * aCacheCols;
    for (Standard_Integer j = 0; j < aCacheCols; j++) 
      aTransientCoeffs[index++] = 0.0;
  }

  // Calculate intermediate values and derivatives of bivariate polynomial along variable with maximal degree
  PLib::EvalPolynomial(aParameters[1], aMinMaxDeriv[1], aMinMaxDegree[1], 
                       aCacheCols, aPolesArray[0], aTransientCoeffs[0]);

  // Calculate a point on surface and a derivatives along variable with minimal degree
  PLib::EvalPolynomial(aParameters[0], aMinMaxDeriv[0], aMinMaxDegree[0], 
                       aDimension, aTransientCoeffs[0], aPntDeriv[0]);

  // Calculate derivative along variable with maximal degree and mixed derivative
  PLib::EvalPolynomial(aParameters[0], 1, aMinMaxDegree[0], aDimension, 
                       aTransientCoeffs[aCacheCols], aPntDeriv[3 * aDimension]);

  // Calculate second derivative along variable with maximal degree
  PLib::NoDerivativeEvalPolynomial(aParameters[0], aMinMaxDegree[0], aDimension, 
                                   aMinMaxDegree[0] * aDimension, aTransientCoeffs[aCacheCols<<1], 
                                   aPntDeriv[6 * aDimension]);

  Standard_Real* aResult = aPntDeriv;
  Standard_Real aTempStorage[PNT_DERIV_LEN];
  if (myIsRational) // calculate derivatives divided by weight's derivatives
  {
    BSplSLib::RationalDerivative(2, 2, 2, 2, aPntDeriv[0], aTempStorage[0]);
    aResult = aTempStorage;
    aDimension--;
  }

  thePoint.SetCoord(aResult[0], aResult[1], aResult[2]);
  if (myDegree[0] > myDegree[1])
  {
    theTangentV.SetCoord(aResult[aDimension], aResult[aDimension + 1], aResult[aDimension + 2]);
    Standard_Integer aShift = aDimension<<1;
    theCurvatureV.SetCoord(aResult[aShift], aResult[aShift + 1], aResult[aShift + 2]);
    aShift += aDimension;
    theTangentU.SetCoord(aResult[aShift], aResult[aShift + 1], aResult[aShift + 2]);
    aShift += aDimension;
    theCurvatureUV.SetCoord(aResult[aShift], aResult[aShift + 1], aResult[aShift + 2]);
    aShift += (aDimension << 1);
    theCurvatureU.SetCoord(aResult[aShift], aResult[aShift + 1], aResult[aShift + 2]);
  }
  else
  {
    theTangentU.SetCoord(aResult[aDimension], aResult[aDimension + 1], aResult[aDimension + 2]);
    Standard_Integer aShift = aDimension<<1;
    theCurvatureU.SetCoord(aResult[aShift], aResult[aShift + 1], aResult[aShift + 2]);
    aShift += aDimension;
    theTangentV.SetCoord(aResult[aShift], aResult[aShift + 1], aResult[aShift + 2]);
    aShift += aDimension;
    theCurvatureUV.SetCoord(aResult[aShift], aResult[aShift + 1], aResult[aShift + 2]);
    aShift += (aDimension << 1);
    theCurvatureV.SetCoord(aResult[aShift], aResult[aShift + 1], aResult[aShift + 2]);
  }
  theTangentU.Multiply(mySpanLengthRec[0]);
  theTangentV.Multiply(mySpanLengthRec[1]);
  theCurvatureU.Multiply(mySpanLengthRec[0] * mySpanLengthRec[0]);
  theCurvatureV.Multiply(mySpanLengthRec[1] * mySpanLengthRec[1]);
  theCurvatureUV.Multiply(mySpanLengthRec[0] * mySpanLengthRec[1]);
}

