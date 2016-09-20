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

#include <TransferBRep_ShapeListBinder.hxx>
#include <TopoDS_Compound.hxx>
#include <BRep_Builder.hxx>

IMPLEMENT_STANDARD_RTTIEXT(TransferBRep_ShapeListBinder,Transfer_Binder)

Standard_Boolean TransferBRep_ShapeListBinder::IsMultiple () const
{ return (myShapes.Length() > 1); }

Handle(Standard_Type) TransferBRep_ShapeListBinder::ResultType () const
{ return STANDARD_TYPE(TransferBRep_ShapeListBinder); }

Standard_CString TransferBRep_ShapeListBinder::ResultTypeName () const
{ return "list(TopoDS_Shape)"; }

Standard_Boolean TransferBRep_ShapeListBinder::GetOneShape (TopoDS_Shape &theShape) const
{
  const Standard_Integer aNbOfShapes = myShapes.Length();
  if (aNbOfShapes == 0) return Standard_False;
  if (aNbOfShapes == 1) theShape = myShapes.First();
  else {
    // Make a compound from the sequence
    TopoDS_Compound aComp;
    BRep_Builder aBuilder;
    aBuilder.MakeCompound(aComp);
    for (Standard_Integer i=1; i<=aNbOfShapes; i++)
      aBuilder.Add(aComp, myShapes.Value(i));
    theShape = aComp;
  }
  return Standard_True;
}
