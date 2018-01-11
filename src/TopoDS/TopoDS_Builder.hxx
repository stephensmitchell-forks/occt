// Created on: 1991-02-21
// Created by: Remi Lequette
// Copyright (c) 1991-1999 Matra Datavision
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

#ifndef _TopoDS_Builder_HeaderFile
#define _TopoDS_Builder_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

class Standard_NullObject;
class TopoDS_FrozenShape;
class TopoDS_UnCompatibleShapes;
class TopoDS_Shape;
class TopoDS_TShape;
class TopoDS_Wire;
class TopoDS_Shell;
class TopoDS_Solid;
class TopoDS_CompSolid;
class TopoDS_Compound;


//! A  Builder is used   to  create  Topological  Data
//! Structures.It is the root of the Builder class hierarchy.
//!
//! There are three groups of methods in the Builder :
//!
//! The Make methods create Shapes.
//!
//! The Add method includes a Shape in another Shape.
//!
//! The Remove  method  removes a  Shape from an other
//! Shape.
//!
//! This   Builder does not  provide   methods to Make
//! Vertices,  Edges, Faces,  Shells  or Solids. These
//! methods are  provided  in  the inherited  Builders
//! as they must provide the geometry.
//!
//! The Add method check for the following rules :
//!
//! - Any SHAPE can be added in a COMPOUND.
//!
//! - Only SOLID can be added in a COMPSOLID.
//!
//! - Only SHELL, EDGE and VERTEX can be added in a SOLID.
//! EDGE and VERTEX as to be INTERNAL or EXTERNAL.
//!
//! - Only FACE can be added in a SHELL.
//!
//! - Only WIRE and VERTEX can be added in a FACE.
//! VERTEX as to be INTERNAL or EXTERNAL.
//!
//! - Only EDGE can be added in a WIRE.
//!
//! - Only VERTEX can be added in an EDGE.
//!
//! - Nothing can be added in a VERTEX.
class TopoDS_Builder 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Make an empty Wire.
    static void MakeWire (TopoDS_Wire& W);
  
  //! Make an empty Shell.
    static void MakeShell (TopoDS_Shell& S);
  
  //! Make a Solid covering the whole 3D space.
    static void MakeSolid (TopoDS_Solid& S);
  
  //! Make an empty Composite Solid.
    static void MakeCompSolid (TopoDS_CompSolid& C);
  
  //! Make an empty Compound.
    static void MakeCompound (TopoDS_Compound& C);
  
  //! Add the Shape C in the Shape S.
  //! Exceptions
  //! - TopoDS_FrozenShape if S is not free and cannot be modified.
  //! - TopoDS__UnCompatibleShapes if S and C are not compatible.
  Standard_EXPORT static void Add (TopoDS_Shape& S, const TopoDS_Shape& C);
  
  //! Remove the Shape C from the Shape S.
  //! Exceptions
  //! TopoDS_FrozenShape if S is frozen and cannot be modified.
  Standard_EXPORT static void Remove (TopoDS_Shape& S, const TopoDS_Shape& C);




protected:

  
  //! The basic method to make  a Shape, used by all the
  //! Make methods.
  Standard_EXPORT void static MakeShape (TopoDS_Shape& S, const Handle(TopoDS_TShape)& T);




private:





};


#include <TopoDS_Builder.lxx>





#endif // _TopoDS_Builder_HeaderFile
