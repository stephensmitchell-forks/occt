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
#include <NCollection_LocalArray.hxx>

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
public:
  static const Standard_Integer MAX_SPANS_COUNT = 10;

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
  //! \return Standard_True, if the span is already cached
  Standard_Boolean SetLatestCache(const Standard_Integer theSpanIndex);

  //! Return last used cache
  const Handle(CACHE_TYPE)& LastCache() const
  { return myLastCache.myCache; }

  //! Return index of last cached span
  Standard_Integer LastSpan() const
  { return myLastCache.mySpan; }

private:
  //! Shift theNbElems of cached spans to release a position for last used span
  void ShiftCachedSpans(Standard_Integer theNbElems);

  BSplCLib_MultiSpanCacheStorage(const BSplCLib_MultiSpanCacheStorage&);
  const BSplCLib_MultiSpanCacheStorage& operator=(const BSplCLib_MultiSpanCacheStorage&);

private:
  struct IndexedSpan
  {
    Standard_Integer mySpan;
    Standard_Integer myIndex;

    IndexedSpan(const Standard_Integer theSpan = 0, const Standard_Integer theIndex = 0)
      : mySpan(theSpan), myIndex(theIndex)
    {}
  };

  typedef NCollection_LocalArray<Handle(CACHE_TYPE), MAX_SPANS_COUNT> CacheArray;
  typedef NCollection_LocalArray<IndexedSpan, MAX_SPANS_COUNT>        SpanArray;


  Standard_Integer            myMaxSpansCount; ///< maximal number of spans to be cached
  CacheArray                  myCaches;        ///< list of caches
  SpanArray                   myCachedSpans;   ///< indices of used spans, which are sorted from last used till oldest
  Standard_Integer            myNbSpans;       ///< number of spans already cached

  struct {
    Standard_Integer   mySpan;
    Handle(CACHE_TYPE) myCache;
  }                           myLastCache;     ///< last used cache

  Standard_Boolean            mySingleSpan;    ///< Standard_True if the curve has just one span
};

#include <BSplCLib_MultiSpanCacheStorage.gxx>

#endif
