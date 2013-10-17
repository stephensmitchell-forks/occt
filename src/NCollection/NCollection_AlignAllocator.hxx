// Created on: 2013-10-11
// Created by: Artem ZHIDKOV
// Copyright (c) 2002-2013 OPEN CASCADE SAS
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

#ifndef _NCollection_AlignAllocator_HeaderFile
#define _NCollection_AlignAllocator_HeaderFile

#include <NCollection_BaseAllocator.hxx>


//======================================================
// Definitions for SSE and memory alignment
//======================================================

// The macro for alignment of data for SSE and AVX vectorization 
#if (defined(__AVX__) || (defined(_M_IX86_FP) && _M_IX86_FP>2))
# define DATA_ALIGNMENT 32
# define NEED_DATA_ALIGN
#elif (defined(__SSE__) || defined(__SSE2__) || defined(__SSE3__) || defined(__SSE4_1__) || defined(__SSE4_2__) || defined(_M_X64) || (defined(_M_IX86_FP) && _M_IX86_FP>0))
# define DATA_ALIGNMENT 16
# define NEED_DATA_ALIGN
#else
# define DATA_ALIGNMENT 4
#endif

// The macro for array (pointer) alignment according DATA_ALIGNMENT
#ifdef NEED_DATA_ALIGN
# ifdef _MSC_VER
#     define MEMALIGN(thePtrDef) __declspec(align(DATA_ALIGNMENT)) thePtrDef
# else
#     define MEMALIGN(thePtrDef) thePtrDef__attribute__((aligned(DATA_ALIGNMENT)))
# endif
#else
# define MEMALIGN(thePtrDef) thePtrDef
#endif
//======================================================


class Handle_NCollection_AlignAllocator;

/** \brief Allocates aligned memory
  */
class NCollection_AlignAllocator //: public NCollection_BaseAllocator
{
public:
  Standard_EXPORT static void* Allocate(const Standard_Size& theSize, 
                                        const Standard_Size& theAlignment = 4);
  Standard_EXPORT static void  Free(void * anAddress);

protected:
  //! Constructor - prohibited
  NCollection_AlignAllocator(void) {};

private:
  //! Copy constructor - prohibited
  NCollection_AlignAllocator(const NCollection_AlignAllocator&);

public:
// Declaration of CASCADE RTTI
DEFINE_STANDARD_RTTI (NCollection_AlignAllocator)
};

// Definition of HANDLE object using Standard_DefineHandle.hxx
DEFINE_STANDARD_HANDLE (NCollection_AlignAllocator, NCollection_BaseAllocator)

#endif
