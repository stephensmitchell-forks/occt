// Copyright (c) 1998-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#ifndef _Standard_Address_HeaderFile
#define _Standard_Address_HeaderFile

#include <Standard_Integer.hxx>

//============================================================================
//==== HashCode : Returns a HashCode CString
//============================================================================
inline Standard_Integer HashCode (const Standard_Address Value, 
				  const Standard_Integer Upper)
{
  union {Standard_Address L ;
         Standard_Integer I[2] ;} U ;
  U.I[0] = 0 ;
  U.I[1] = 0 ;
  U.L = Value;
  return HashCode( ( ( U.I[0] ^ U.I[1] ) & 0x7fffffff )  , Upper ) ;
}


//============================================================================
//==== IsSimilar : Returns Standard_True if two booleans have the same value
//============================================================================
inline Standard_Boolean IsSimilar(const Standard_Address One
				 ,const Standard_Address Two)
{ return One == Two; }

//============================================================================
// IsEqual : Returns Standard_True if two CString have the same value
//============================================================================

inline Standard_Boolean IsEqual(const Standard_Address One
			       ,const Standard_Address Two)
{ return One == Two; }

//! Workaround CLang extreme optimization, which causes failures in case when reference is passed for NULL pointer.
#ifdef __APPLE__
template<typename T> bool Standard_IS_NULL_REFERENCE (const T& theRef) __attribute__((optnone))
{
  return &theRef == NULL;
}
#else
template<typename T> bool Standard_IS_NULL_REFERENCE (const T& theRef)
{
  return &theRef == NULL;
}
#endif

#endif
