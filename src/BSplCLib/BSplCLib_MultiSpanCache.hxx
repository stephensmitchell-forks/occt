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

#ifndef _BSplCLib_MultiSpanCache_Headerfile
#define _BSplCLib_MultiSpanCache_Headerfile

#include <BSplCLib_Cache.hxx>
#include <NCollection_LocalArray.hxx>
#include <TColStd_Array1OfInteger.hxx>

//! \brief Base container for list of caches for Bezier and B-spline curves.
//!
//! Applicable template arguments:
//!  * POINT_TYPE = gp_Pnt2d and VEC_TYPE = gp_Vec2d
//!  * POINT_TYPE = gp_Pnt and VEC_TYPE = gp_Vec
template<class POINT_TYPE, class VEC_TYPE>
class BSplCLib_MultiSpanCacheBase
{
  typedef NCollection_Array1<POINT_TYPE> Array1OfPOINT;

public:
  //! Construct cache for Bezier curve with given parameters
  //! \param theDegree    [in] degree of the curve
  //! \param thePeriodic  [in] periodicity of the curve
  //! \param thePoles     [in] array of poles of the curve
  //! \param theWeights   [in] array of weights of corresponding poles
  //! \param theMaxSpans  [in] maximal spans to be cached
  BSplCLib_MultiSpanCacheBase(const Standard_Integer         theDegree,
                              const Standard_Boolean         thePeriodic,
                              const Array1OfPOINT&           thePoles,
                              const TColStd_Array1OfReal*    theWeights = NULL);

  //! Construct multi-span cache for B-spline curve with given parameters
  //! \param theDegree    [in] degree of the curve
  //! \param thePeriodic  [in] periodicity of the curve
  //! \param theKnots     [in] knots of the curve
  //! \param theMults     [in] multiplicities
  //! \param theFlatKnots [in] knots of the curve (with repetition)
  //! \param thePoles     [in] array of poles of the curve
  //! \param theWeights   [in] array of weights of corresponding poles
  //! \param theMaxSpans  [in] maximal spans to be cached
  BSplCLib_MultiSpanCacheBase(const Standard_Integer         theDegree,
                              const Standard_Boolean         thePeriodic,
                              const TColStd_Array1OfReal*    theKnots,
                              const TColStd_Array1OfInteger* theMults,
                              const TColStd_Array1OfReal*    theFlatKnots,
                              const Array1OfPOINT&           thePoles,
                              const TColStd_Array1OfReal*    theWeights = NULL);

  virtual ~BSplCLib_MultiSpanCacheBase();

  //! Calculates the point on the curve for specified parameters
  //! \param theParameter [in]  parameter on the curve
  //! \param thePoint     [out] the result of calculation (the point on the curve)
  Standard_EXPORT void D0(const Standard_Real theParameter, POINT_TYPE& thePoint);

  //! Calculates the point on the curve and its first derivative
  //! \param theParameter [in]  parameter on the curve
  //! \param thePoint     [out] the result of calculation (the point on the curve)
  //! \param theTangent   [out] tangent vector in the calculated point
  Standard_EXPORT void D1(const Standard_Real theParameter,
                                POINT_TYPE&   thePoint, 
                                VEC_TYPE&     theTangent);

  //! Calculates the point on the curve and derivatives till second order
  //! \param theParameter [in]  parameter on the curve
  //! \param thePoint     [out] the result of calculation (the point on the curve)
  //! \param theTangent   [out] tangent vector in the calculated point
  //! \param theCurvature [out] curvature vector (2nd derivative)
  Standard_EXPORT void D2(const Standard_Real theParameter,
                                POINT_TYPE&   thePoint, 
                                VEC_TYPE&     theTangent,
                                VEC_TYPE&     theCurvature);

  //! Calculates the point on the curve and three derivatives in the specified parameter
  //! \param theParameter [in]  parameter on the curve
  //! \param thePoint     [out] the result of calculation (the point on the curve)
  //! \param theTangent   [out] tangent vector (1st derivative) in the calculated point
  //! \param theCurvature [out] curvature vector (2nd derivative)
  //! \param theTorsion   [out] second curvature vector (3rd derivative)
  Standard_EXPORT void D3(const Standard_Real theParameter,
                                POINT_TYPE&   thePoint, 
                                VEC_TYPE&     theTangent,
                                VEC_TYPE&     theCurvature,
                                VEC_TYPE&     theTorsion);

private:
  //! Calculate an index of a span by the parameter on the curve.
  //! \param theParameter [in]  parameter on the curve
  //! \return Index of the span, used to store cache in the map.
  Standard_Integer SpanIndex(const Standard_Real theParameter) const;

  //! Find cached span containing given parameter.
  //! Parameter is adjusted to period for periodic curves.
  //! If such span is not cached yet, prepares new cache object.
  const Handle(BSplCLib_Cache)& FindCache(Standard_Real& theParameter);

  BSplCLib_MultiSpanCacheBase(const BSplCLib_MultiSpanCacheBase&);
  const BSplCLib_MultiSpanCacheBase& operator=(const BSplCLib_MultiSpanCacheBase&);

private:
  typedef NCollection_LocalArray<Handle(BSplCLib_Cache), 1> CacheArray;

  Standard_Integer               myDegree;       ///< degree of the curve
  Standard_Boolean               myPeriodic;     ///< periodicity of the curve
  Standard_Real                  myFirstKnot;    ///< value of the first knot (used for periodic normalization)
  Standard_Real                  myLastKnot;     ///< value of the lsst knot (used for periodic normalization)
  const TColStd_Array1OfReal*    myKnots;        ///< knots
  const TColStd_Array1OfInteger* myMults;        ///< multiplicities
  const TColStd_Array1OfReal*    myFlatKnots;    ///< knots duplicated according to multiplicity
  const Array1OfPOINT&           myPoles;        ///< array of poles
  const TColStd_Array1OfReal*    myWeights;      ///< array of weights

  CacheArray                     myCaches;       ///< array of caches
  Standard_Integer               myLastCacheInd; ///< index of last used cache

  Standard_Boolean               mySingleSpan;   ///< the curve consists of just one span
};


//! \brief A container for list of caches for 2D Bezier and B-spline curves.
class BSplCLib_MultiSpanCache2D : public BSplCLib_MultiSpanCacheBase<gp_Pnt2d, gp_Vec2d>, public Standard_Transient
{
public:
  Standard_EXPORT BSplCLib_MultiSpanCache2D(const Standard_Integer         theDegree,
                                            const Standard_Boolean         thePeriodic,
                                            const TColgp_Array1OfPnt2d&    thePoles,
                                            const TColStd_Array1OfReal*    theWeights  = NULL)
    : BSplCLib_MultiSpanCacheBase<gp_Pnt2d, gp_Vec2d>(theDegree, thePeriodic, thePoles, theWeights)
  {
  }

  Standard_EXPORT BSplCLib_MultiSpanCache2D(const Standard_Integer         theDegree,
                                            const Standard_Boolean         thePeriodic,
                                            const TColStd_Array1OfReal*    theKnots,
                                            const TColStd_Array1OfInteger* theMults,
                                            const TColStd_Array1OfReal*    theFlatKnots,
                                            const TColgp_Array1OfPnt2d&    thePoles,
                                            const TColStd_Array1OfReal*    theWeights  = NULL)
    : BSplCLib_MultiSpanCacheBase<gp_Pnt2d, gp_Vec2d>(theDegree, thePeriodic, theKnots, theMults,
                                                      theFlatKnots, thePoles, theWeights)
  {
  }

  DEFINE_STANDARD_RTTI_INLINE(BSplCLib_MultiSpanCache2D, Standard_Transient)
};

//! \brief A container for list of caches for 3D Bezier and B-spline curves.
class BSplCLib_MultiSpanCache3D : public BSplCLib_MultiSpanCacheBase<gp_Pnt, gp_Vec>, public Standard_Transient
{
public:
  Standard_EXPORT BSplCLib_MultiSpanCache3D(const Standard_Integer         theDegree,
                                            const Standard_Boolean         thePeriodic,
                                            const TColgp_Array1OfPnt&      thePoles,
                                            const TColStd_Array1OfReal*    theWeights  = NULL)
    : BSplCLib_MultiSpanCacheBase<gp_Pnt, gp_Vec>(theDegree, thePeriodic, thePoles, theWeights)
  {
  }

  Standard_EXPORT BSplCLib_MultiSpanCache3D(const Standard_Integer         theDegree,
                                            const Standard_Boolean         thePeriodic,
                                            const TColStd_Array1OfReal*    theKnots,
                                            const TColStd_Array1OfInteger* theMults,
                                            const TColStd_Array1OfReal*    theFlatKnots,
                                            const TColgp_Array1OfPnt&      thePoles,
                                            const TColStd_Array1OfReal*    theWeights  = NULL)
    : BSplCLib_MultiSpanCacheBase<gp_Pnt, gp_Vec>(theDegree, thePeriodic, theKnots, theMults,
                                                  theFlatKnots, thePoles, theWeights)
  {
  }

  DEFINE_STANDARD_RTTI_INLINE(BSplCLib_MultiSpanCache3D, Standard_Transient)
};

DEFINE_STANDARD_HANDLE(BSplCLib_MultiSpanCache2D, Standard_Transient)
DEFINE_STANDARD_HANDLE(BSplCLib_MultiSpanCache3D, Standard_Transient)

#include <BSplCLib_MultiSpanCache.gxx>

#endif
