#include <Geom_BSplineCache.hxx>

#ifndef _NCollection_AlignAllocator_HeaderFile
#include <NCollection_AlignAllocator.hxx>
#endif

IMPLEMENT_STANDARD_RTTIEXT(Geom_BSplineCache, MMgt_TShared)

Geom_BSplineCache::Geom_BSplineCache(const Standard_Integer theRowLower,    const Standard_Integer theRowUpper,
                                     const Standard_Integer theColumnLower, const Standard_Integer theColumnUpper)
  : myCacheArray( 
      *(Standard_Real*)NCollection_AlignAllocator::Allocate( 
                            (theRowUpper-theRowLower+1)*(theColumnUpper-theColumnLower+1)*sizeof(Standard_Real), 
                            DATA_ALIGNMENT),
      theRowLower, theRowUpper, theColumnLower, theColumnUpper)
  {}

void Geom_BSplineCache::Destroy()
{
  Standard_Real* aPtr = (Standard_Real*) &(myCacheArray(myCacheArray.LowerCol(), myCacheArray.LowerRow()));
  if (aPtr)
    NCollection_AlignAllocator::Free(aPtr);
}

