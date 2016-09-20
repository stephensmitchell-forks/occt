// Created on: 1994-10-03
// Created by: Christian CAILLET
// Copyright (c) 1994-1999 Matra Datavision
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

#ifndef _TransferBRep_ShapeListBinder_HeaderFile
#define _TransferBRep_ShapeListBinder_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TopoDS_Shape.hxx>
#include <TopTools_SequenceOfShape.hxx>
#include <Transfer_Binder.hxx>

//! This binder binds several (a list of) shapes with a starting
//! entity, when this entity itself corresponds to a simple list
//! of shapes. Each part is not seen as a sub-result of an
//! independant componant, but as an item of a built-in list
class TransferBRep_ShapeListBinder : public Transfer_Binder
{
 public:
  
  TransferBRep_ShapeListBinder() {}
  
  Standard_EXPORT virtual Standard_Boolean IsMultiple() const Standard_OVERRIDE;
  
  Standard_EXPORT Handle(Standard_Type) ResultType() const Standard_OVERRIDE;
  
  Standard_EXPORT Standard_CString ResultTypeName() const Standard_OVERRIDE;
  
  //! Adds an item to the result list
  void AddResult (const TopoDS_Shape &theShape) { myShapes.Append(theShape); }

  const TopTools_SequenceOfShape & Result() const { return myShapes; }

  //! Provides result as one shape
  Standard_EXPORT Standard_Boolean GetOneShape(TopoDS_Shape &theShape) const;

  DEFINE_STANDARD_RTTIEXT(TransferBRep_ShapeListBinder,Transfer_Binder)

 private:

  TopTools_SequenceOfShape myShapes;
};

#endif // _TransferBRep_ShapeListBinder_HeaderFile
