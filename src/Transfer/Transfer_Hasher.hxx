// Created on: 2016-09-27
// Created by: Sergey ZERCHANINOV
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

#ifndef _Transfer_Hasher_HeaderFile
#define _Transfer_Hasher_HeaderFile

#include <Standard.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>
class Standard_Transient;


class Transfer_Hasher
{
 public:

  //! Returns a HashCode in the range <0,Upper> for an object K
  Standard_EXPORT static Standard_Integer HashCode (const Handle(Standard_Transient)& K, const Standard_Integer Upper);
  
  //! Returns True if two objects are the same.
  Standard_EXPORT static Standard_Boolean IsEqual (const Handle(Standard_Transient)& K1, const Handle(Standard_Transient)& K2);
};

#endif // _Transfer_Hasher_HeaderFile
