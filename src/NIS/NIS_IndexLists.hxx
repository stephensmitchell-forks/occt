// File:      NIS_IndexLists.hxx
// Created:   19.07.2011 15:29
// Author:    
// Copyright: Open Cascade 2011

#ifndef NIS_IndexLists_HeaderFile
#define NIS_IndexLists_HeaderFile

#include <Standard_TypeDef.hxx>
class NCollection_BaseAllocator;
class Handle_NCollection_BaseAllocator;

class NIS_IndexLists 
{
 public:
  // ---------- PUBLIC METHODS ----------


  /**
   * Constructor. 
   */
  Standard_EXPORT NIS_IndexLists        (); 

  /**
   * Reset all data memebers and free all allocated memory.
   * Called from the destructor of the container class, also can be used
   * to re-initialize the data set.
   * The allocator parameter should be the same as previously passed to
   * methods InitArray() and SetArray().
   */
  Standard_EXPORT void              Clear (NCollection_BaseAllocator* theAlloc);

  /**
   * Create a copy of object.
   * @param theAl
   *   Allocator where the Dest should store its private data.
   * @param theDest
   *   <tt>[in-out]</tt> The target object where the data are copied. If
   *   passed NULL then the target should be created.
   */
  Standard_EXPORT void              Clone
                                (const Handle_NCollection_BaseAllocator& theAl,
                                 NIS_IndexLists&      theDest) const;


  /**
   * Initialize the array of indexes
   * @param theIndexType
   *   0:8bit, 1:16bit, 2:32bit
   * @param theNLists
   *   Number of separate array of indexes. If set to 0, array are cancelled
   * @param theAlloc
   *   Allocator that is called for array memory.
   */
  Standard_EXPORT void              InitArray
                                        (const unsigned int      theIndexType,
                                         const Standard_Integer  theNLists,
                                         NCollection_BaseAllocator * theAlloc);

  /**
   * Set the actual number of used arrays. The length can not exceed value specified in InitArray
   * @param theNLists
   *   Number of separate array of indexes. If set to 0, array are cancelled
   */
  Standard_EXPORT void              SetRealNumbersOfArrays
                                        (const Standard_Integer theNLists);

  /**
   * Store the element index in array
   * @param theIndArray
   *   Index of the Array, should be less than the number of arrays that is
   *   defined in InitArray() and can be returned by NArrays().
   * @param theIndInArray
   *   Index of the element in the Array. Should be less than the parameter theSz
   *   in the corresponding previous SetArray() call.
   * @param theIndex
   *   Index of the element in the given position of the Array. 
   */
  Standard_EXPORT void              SetArrayIndex
                                        (const Standard_Size    theIndArray,
                                         const Standard_Size    theIndInArray,
                                         const Standard_Integer theIndex);

  /**
   * Allocate a single array. The array can be filled by indices using
   * the method SetArrayIndex().
   * @param theIndArray
   *   Index of the array, should be [0...myNbArrays-1]
   * @param theSz
   *   Number of elements in the array.
   * @param theAlloc
   *   Allocator, should be the same as passed to InitArray() before.
   */
  Standard_EXPORT void              SetArray
                                        (const Standard_Size      theIndArray,
                                         const Standard_Integer   theSz,
                                         NCollection_BaseAllocator * theAlloc);

  /**
   * Store the array elements indexes
   * @param theIndArray
   *   Index of the Array, should be less than the number of arrays that is
   *   defined in InitArray() and can be returned by NArrays().
   * @param theNbIndexes
   *   size of array   
   * @param theIdxList
   *   Array of elements. The size must be equal theNbIndexes
   * @param theAlloc
   *   Allocator, should be the same as passed to InitArray() before.
   * @param theStartIdx
   *   Index that must be added to the element in the array to change
   *   the indexation, 0 by default.
   */
  Standard_EXPORT void              SetArray
                                        (const Standard_Size     theIndArray,
                                         const Standard_Integer  theNbIndexes, 
                                         const int*              theIdxList,
                                         NCollection_BaseAllocator * theAlloc,
                                         const Standard_Integer  theStartIdx=0);

  /**
   * Get the element with index 'theIndInArray' from the array
   * number 'theIndArray'.
   */
  Standard_EXPORT Standard_Integer ArrayIndex
                                     (const Standard_Size    theIndArray,
                                      const Standard_Integer theIndInArr) const;

  /**
   * Get the number of elements for the array number 'theIndArray'.
   */
  Standard_EXPORT Standard_Size NArrayIndexes
                                     (const Standard_Integer theIndArray) const;

  /**
   * Get the array of elements for the array number 'theIndArray'.
   */
  Standard_EXPORT void* ArrayIndexes (const Standard_Size theIndArray) const;

  /**
   * Query the number of arrays.
   */
  inline Standard_Integer           NArrays () const
  { return static_cast<Standard_Integer>(myNbArrays); }

 protected:
  // ---------- PROTECTED METHODS ----------

 private:
  // ---------- PRIVATE (PROHIBITED) METHODS ----------

 private:
  // ---------- PRIVATE FIELDS ----------  
  Standard_Integer**            mypIndexes;       ///< Array of indexes  

  unsigned int                  myNbArrays  : 24; ///< Number of arrays
  unsigned int                  myIndexType : 2;  ///< 0:8bit, 1:16bit, 2:32bit
};

#endif
