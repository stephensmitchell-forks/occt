// Created on: 2013-10-11
// Created by: Artem ZHIDKOV
// Copyright (c) 2009-2013 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

#include <NCollection_AlignAllocator.hxx>
#include <Standard_OutOfMemory.hxx>
#include <Standard_Mutex.hxx>


#ifdef _MSC_VER
  #include <malloc.h>
#elif (defined(__GNUC__) && __GNUC__ >= 4 && __GNUC_MINOR__ >= 1)
  #include <mm_malloc.h>
#else
  extern "C" int posix_memalign (void** thePtr, size_t theAlign, size_t theBytesCount);
#endif


IMPLEMENT_STANDARD_HANDLE (NCollection_AlignAllocator, NCollection_BaseAllocator)
IMPLEMENT_STANDARD_RTTIEXT(NCollection_AlignAllocator, NCollection_BaseAllocator)

//=======================================================================
//function : Allocate
//purpose  : 
//=======================================================================

void* NCollection_AlignAllocator::Allocate(const Standard_Size& theSize,
                                           const Standard_Size& theAlignment)
{
  void* aResult;
#if defined(_MSC_VER)
  aResult = _aligned_malloc(theSize, theAlignment);
#elif (defined(__GNUC__) && __GNUC__ >= 4 && __GNUC_MINOR__ >= 1)
  aResult = _mm_malloc(theSize, theAlignment);
#else
  if (posix_memalign(&aResult, theAlignment, theSize))
    aResult = NULL;
#endif

  if (aResult == NULL)
  {
    char aBuffer[96];
    Sprintf (aBuffer, "Failed to allocate %" PRIuPTR " bytes in global dynamic heap", theSize);
    Standard_OutOfMemory::Raise (aBuffer);
  }
  return aResult;
}

//=======================================================================
//function : Free
//purpose  : 
//=======================================================================

void NCollection_AlignAllocator::Free(void* anAddress)
{
#if defined(_MSC_VER)
  _aligned_free(anAddress);
#elif (defined(__GNUC__) && __GNUC__ >= 4 && __GNUC_MINOR__ >= 1)
  _mm_free(anAddress);
#else
  free(anAddress);
#endif
}
