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

#ifndef _BSplCLib_MultiSpanCacheStorage_Headerfile
#define _BSplCLib_MultiSpanCacheStorage_Headerfile

#include <BSplCLib_Cache.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_List.hxx>
#include <NCollection_SparseArray.hxx>

//! \brief Base container for list of caches for Bezier and B-spline curves or surfaces.
//!
//! Manages list of cached spans for 2D/3D Bezier or B-spline curve/surface.
//! Maximal number of cached spans is limited by myMaxSpansCount variable.
//!
//! Caches are sorted by frequency of accessing. If there is no free room for new cache,
//! the cache which is used rarely will be replaced.
//!
//! Applicable template arguments:
//!  * CACHE_TYPE = BSplCLib_Cache
//!  * CACHE_TYPE = BSplSLib_Cache
template<class CACHE_TYPE>
class BSplCLib_MultiSpanCacheStorage
{
  typedef NCollection_SparseArray<Handle(CACHE_TYPE)> CacheArray;
  typedef NCollection_List<Standard_Integer>          SpanArray;

protected:
  static const Standard_Integer MAX_SPANS_COUNT = 10;

public:
  //! Construct multi-span cache with given parameters
  //! \param theMaxSpans  [in] maximal spans to be cached
  BSplCLib_MultiSpanCacheStorage(const Standard_Integer theMaxSpans = MAX_SPANS_COUNT);

  //! Change number of cached spans
  Standard_EXPORT void SetMaxSpansCached(const Standard_Integer theMaxSpans);

  //! Clean all cached data
  Standard_EXPORT virtual void Clear();

protected:
  //! Changes flag whether the curve/surface has just one span
  void SetSingleSpan(Standard_Boolean theIsSignleSpan)
  { mySingleSpan = theIsSignleSpan; }

  //! Return the single-span flag
  Standard_Boolean IsSingleSpan() const
  { return mySingleSpan; }

  //! Allocate memory for new cache
  //! \param theSpanIndex [in]  index of the span
  //! \param theCache     [out] cache created
  //! \return Standard_True, if the created cache should be rebuild
  Standard_Boolean CreateNewCache(const Standard_Integer theSpanIndex, Handle(CACHE_TYPE)& theCache);

  //! Lift last used cache onto the top
  //! \param theSpanIndex  [in]  index of latest used span
  //! \param theIndexInMap [in]  index of a cached span in the map (should be explicitly calculated, if 0)
  //! \return Standard_True, if the span is already cached
  Standard_Boolean SetLatestCache(const Standard_Integer theSpanIndex);

  ///! Remove oldest cache
  ///! \return the cache has been removed from the list
  Handle(CACHE_TYPE) RemoveFirstUsedCache();

  //! Return last used cache
  const Handle(CACHE_TYPE)& LastCache() const
  { return myLastCache.myCache; }

  //! Return index of last cached span
  const Standard_Integer& LastSpan() const
  { return myLastCache.mySpan; }

private:
  BSplCLib_MultiSpanCacheStorage(const BSplCLib_MultiSpanCacheStorage&);
  const BSplCLib_MultiSpanCacheStorage& operator=(const BSplCLib_MultiSpanCacheStorage&);

private:
  Standard_Integer            myMaxSpansCount; ///< maximal number of spans to be cached
  CacheArray                  myCaches;        ///< list of caches
  SpanArray                   myCachedSpans;   ///< indices of used spans, which are sorted from oldest till last used

  struct {
    Standard_Integer   mySpan;
    Handle(CACHE_TYPE) myCache;
  }                           myLastCache;     ///< last used cache

  Standard_Boolean            mySingleSpan;    ///< Standard_True if the curve has just one span
};

#include <BSplCLib_MultiSpanCacheStorage.gxx>

#endif
