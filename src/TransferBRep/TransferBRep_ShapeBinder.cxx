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

#include <TransferBRep_ShapeBinder.hxx>
#include <TransferBRep.hxx>
#include <TopoDS_Shape.hxx>

IMPLEMENT_STANDARD_RTTIEXT(TransferBRep_ShapeBinder,Transfer_Binder)

Handle(Standard_Type) TransferBRep_ShapeBinder::ResultType () const
{ return STANDARD_TYPE(TopoDS_TShape); }

Standard_CString TransferBRep_ShapeBinder::ResultTypeName () const
{ return TransferBRep::TypeName (myResult); }
