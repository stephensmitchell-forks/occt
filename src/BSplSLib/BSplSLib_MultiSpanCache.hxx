// Copyright (c) 2016 OPEN CASCADE SAS
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

#ifndef _BSplSLib_MultiSpanCache_Headerfile
#define _BSplSLib_MultiSpanCache_Headerfile

#include <BSplSLib_Cache.hxx>
#include <NCollection_LocalArray.hxx>

//! \brief A container for list of caches for Bezier and B-spline surfaces.
//!        Stores a 2D array of caches. Each cache is initialized in first time called.
class BSplSLib_MultiSpanCache : public Standard_Transient
{
public:
  //! Construct cache for Bezier surface with given parameters
  //! \param theDegreeU    [in] degree along the first parameter (U) of the surface
  //! \param thePeriodicU  [in] identify the surface is periodical along U axis
  //! \param theDegreeV    [in] degree along the second parameter (V) of the surface
  //! \param thePeriodicV  [in] identify the surface is periodical along V axis
  //! \param thePoles      [in] array of poles of the surface
  //! \param theWeights    [in] array of weights of corresponding poles
  //! \param theMaxSpans   [in] maximal spans to be cached
  Standard_EXPORT BSplSLib_MultiSpanCache(const Standard_Integer         theDegreeU,
                                          const Standard_Boolean         thePeriodicU,
                                          const Standard_Integer         theDegreeV,
                                          const Standard_Boolean         thePeriodicV,
                                          const TColgp_Array2OfPnt&      thePoles,
                                          const TColStd_Array2OfReal*    theWeights  = NULL);

  //! Construct multi-span cache for B-spline surface with given parameters
  //! \param theDegreeU    [in] degree along the first parameter (U) of the surface
  //! \param thePeriodicU  [in] identify the surface is periodical along U axis
  //! \param theKnotsU     [in] knots of the surface along U axis
  //! \param theMultsU     [in] multiplicities of knots along U axis
  //! \param theFlatKnotsU [in] knots of the surface (with repetition) along U axis
  //! \param theDegreeV    [in] degree alogn the second parameter (V) of the surface
  //! \param thePeriodicV  [in] identify the surface is periodical along V axis
  //! \param theKnotsV     [in] knots of the surface along V axis
  //! \param theMultsV     [in] multiplicities of knots along V axis
  //! \param theFlatKnotsV [in] knots of the surface (with repetition) along V axis
  //! \param thePoles      [in] array of poles of the surface
  //! \param theWeights    [in] array of weights of corresponding poles
  //! \param theMaxSpans   [in] maximal spans to be cached
  Standard_EXPORT BSplSLib_MultiSpanCache(const Standard_Integer         theDegreeU,
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
                                          const TColStd_Array2OfReal*    theWeights  = NULL);

  virtual ~BSplSLib_MultiSpanCache();

  //! Clean all cached data
  Standard_EXPORT void Clear();

  //! Calculates the point on the surface for specified parameters
  //! \param theU     [in]  first parameter for calculation of the value
  //! \param theV     [in]  second parameter for calculation of the value
  //! \param thePoint [out] the result of calculation (the point on the surface)
  Standard_EXPORT void D0(const Standard_Real theU, const Standard_Real theV, gp_Pnt& thePoint);

  //! Calculates the point on the surface and its first derivative
  //! \param theU        [in]  first parameter of calculation of the value
  //! \param theV        [in]  second parameter of calculation of the value
  //! \param thePoint    [out] the result of calculation (the point on the surface)
  //! \param theTangentU [out] tangent vector along U axis in the calculated point
  //! \param theTangentV [out] tangent vector along V axis in the calculated point
  Standard_EXPORT void D1(const Standard_Real theU, 
                          const Standard_Real theV, 
                                gp_Pnt&       thePoint, 
                                gp_Vec&       theTangentU, 
                                gp_Vec&       theTangentV);

  //! Calculates the point on the surface and derivatives till second order
  //! \param theU           [in]  first parameter of calculation of the value
  //! \param theV           [in]  second parameter of calculation of the value
  //! \param thePoint       [out] the result of calculation (the point on the surface)
  //! \param theTangentU    [out] tangent vector along U axis in the calculated point
  //! \param theTangentV    [out] tangent vector along V axis in the calculated point
  //! \param theCurvatureU  [out] curvature vector (2nd derivative on U) along U axis
  //! \param theCurvatureV  [out] curvature vector (2nd derivative on V) along V axis
  //! \param theCurvatureUV [out] 2nd mixed derivative on U anv V
  Standard_EXPORT void D2(const Standard_Real theU, 
                          const Standard_Real theV, 
                                gp_Pnt&       thePoint, 
                                gp_Vec&       theTangentU, 
                                gp_Vec&       theTangentV, 
                                gp_Vec&       theCurvatureU, 
                                gp_Vec&       theCurvatureV, 
                                gp_Vec&       theCurvatureUV);


  DEFINE_STANDARD_RTTI_INLINE(BSplSLib_MultiSpanCache, Standard_Transient)

private:
  //! Calculate an index of a span by the parameters on surface
  //! and corresponding span indices along each parameter.
  //! \param theU          [in]  parameter U
  //! \param theV          [in]  parameter V
  //! \param theSpanIndexU [out] index of span containing theU parameter
  //! \param theSpanIndexV [out] index of span containing theV parameter
  //! \return Index of the cache in the array
  Standard_Integer SpanIndex(const Standard_Real theU,
                             const Standard_Real theV,
                             Standard_Integer&   theSpanIndexU,
                             Standard_Integer&   theSpanIndexV) const;

  //! Find cached span containing parametric point (theU, theV).
  //! Parameters are adjusted to period for periodic surface.
  //! If such span is not cached yet, prepares new cache object.
  const Handle(BSplSLib_Cache)& FindCache(Standard_Real& theU, Standard_Real& theV);

  //! Return Standard_True if the surface has just one span
  Standard_Boolean IsSingleSpan() const
  { return mySingleSpanU != 0 && mySingleSpanV != 0; }

  BSplSLib_MultiSpanCache(const BSplSLib_MultiSpanCache&);
  const BSplSLib_MultiSpanCache& operator=(const BSplSLib_MultiSpanCache&);

private:
  typedef NCollection_LocalArray<Handle(BSplSLib_Cache), 1> CacheArray;

  Standard_Integer               myDegreeU;       ///< degree along U
  Standard_Integer               myDegreeV;       ///< degree along V
  Standard_Boolean               myPeriodicU;     ///< periodicity along U
  Standard_Boolean               myPeriodicV;     ///< periodicity along V
  const TColStd_Array1OfReal*    myKnotsU;        ///< knots along U
  const TColStd_Array1OfReal*    myKnotsV;        ///< knots along V
  const TColStd_Array1OfInteger* myMultsU;        ///< multiplicities along U
  const TColStd_Array1OfInteger* myMultsV;        ///< multiplicities along V
  const TColStd_Array1OfReal*    myFlatKnotsU;    ///< knots along U (duplicated according to multiplicity)
  const TColStd_Array1OfReal*    myFlatKnotsV;    ///< knots along V (duplicated according to multiplicity)
  Standard_Real                  myFirstKnotU;    ///< first knot along U (used for periodic normalization)
  Standard_Real                  myLastKnotU;     ///< last knot along U (used for periodic normalization)
  Standard_Real                  myFirstKnotV;    ///< first knot along V (used for periodic normalization)
  Standard_Real                  myLastKnotV;     ///< last knot along V (used for periodic normalization)
  const TColgp_Array2OfPnt&      myPoles;         ///< array of poles
  const TColStd_Array2OfReal*    myWeights;       ///< array of weights

  CacheArray                     myCaches;        ///< array of caches
  Standard_Integer               myLastCacheInd;  ///< index of last used cache

  struct {
    Standard_Real    myParameter;
    Standard_Integer mySpan;
  }                              myLastU,         ///< last U and index of the span containing it
                                 myLastV;         ///< last V and index of the span containing it

  Standard_Integer               mySingleSpanU;   ///< index of the span along U, if the surface has just one span on this parameter, otherwise is 0
  Standard_Integer               mySingleSpanV;   ///< index of the span along V, if the surface has just one span on this parameter, otherwise is 0
};

DEFINE_STANDARD_HANDLE(BSplSLib_MultiSpanCache, Standard_Transient)

#endif
