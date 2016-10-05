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

#ifndef _TransferBRep_HeaderFile
#define _TransferBRep_HeaderFile

#include <Standard.hxx>
#include <Standard_Handle.hxx>

#include <TColStd_HSequenceOfTransient.hxx>
#include <TopAbs_Orientation.hxx>
#include <TopTools_HSequenceOfShape.hxx>
class TopoDS_Shape;
class Transfer_Binder;
class Transfer_Process;
class Transfer_TransientProcess;
class TransferBRep_ShapeMapper;

//! This package gathers services to simply read files and convert
//! them to Shapes from CasCade. IE. it can be used in conjunction
//! with purely CasCade software
class TransferBRep 
{
 public:

  //! Get the Shape recorded in a Binder
  //! If the Binder brings a multiple result, search for the Shape
  Standard_EXPORT static TopoDS_Shape ShapeResult (const Handle(Transfer_Binder)& binder);
  
  //! Get the Shape recorded in a TransientProcess as result of the
  //! Transfer of an entity. I.E. in the binder bound to that Entity
  //! If no result or result not a single Shape, returns a Null Shape
  Standard_EXPORT static TopoDS_Shape ShapeResult (const Handle(Transfer_TransientProcess)& TP, const Handle(Standard_Transient)& ent);
  
  //! Sets a Shape as a result for a starting entity <ent>
  //! (reverse of ShapeResult)
  //! It simply creates a ShapeBinder then binds it to the entity
  Standard_EXPORT static void SetShapeResult (const Handle(Transfer_TransientProcess)& TP, const Handle(Standard_Transient)& ent, const TopoDS_Shape &theShape);
  
  //! Gets the Shapes recorded in a TransientProcess as result of a
  //! Transfer, considers roots only or all results according
  //! <rootsonly>, returns them as a HSequence
  Standard_EXPORT static Handle(TopTools_HSequenceOfShape) Shapes (const Handle(Transfer_TransientProcess)& TP, const Standard_Boolean rootsonly = Standard_True);
  
  //! Gets the Shapes recorded in a TransientProcess as result of a
  //! Transfer, for a given list of starting entities, returns
  //! the shapes as a HSequence
  Standard_EXPORT static Handle(TopTools_HSequenceOfShape) Shapes (const Handle(Transfer_TransientProcess)& TP, const Handle(TColStd_HSequenceOfTransient)& list);
  
  //! Returns a ShapeMapper for a given Shape (location included)
  //! Either <shape> is already mapped, then its Mapper is returned
  //! Or it is not, then a new one is created then returned, BUT
  //! it is not mapped here (use Bind to do this)
  Standard_EXPORT static Handle(TransferBRep_ShapeMapper) ShapeMapper (const Handle(Transfer_Process)& FP, const TopoDS_Shape& theShape);

  //! Returns Type Name (string)
  //! Here, the true name of the Type of a Shape
  Standard_EXPORT static Standard_CString TypeName (const TopoDS_Shape &theShape);
};

#endif // _TransferBRep_HeaderFile
