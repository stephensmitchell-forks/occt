// Copyright (c) 1999-2013 OPEN CASCADE SAS
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
// and conditions governing the rights and limitations under the License..

#ifndef _TransferBRep_ShapeBinder_HeaderFile
#define _TransferBRep_ShapeBinder_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineHandle.hxx>
#include <TransferBRep_BinderOfShape.hxx>
#include <TopAbs_ShapeEnum.hxx>

class Standard_TypeMismatch;
class TopoDS_Shape;
class TopoDS_Vertex;
class TopoDS_Edge;
class TopoDS_Wire;
class TopoDS_Face;
class TopoDS_Shell;
class TopoDS_Solid;
class TopoDS_CompSolid;
class TopoDS_Compound;
class Standard_Transient;
class Handle(Standard_Type);
class Handle(TransferBRep_BinderOfShape);
class TransferBRep_ShapeBinder;

DEFINE_STANDARD_HANDLE(TransferBRep_ShapeBinder,TransferBRep_BinderOfShape)

//! A ShapeBinder is a BinderOfShape with some additional services <br>
//!           to cast the Result under various kinds of Shapes <br>
class TransferBRep_ShapeBinder : public TransferBRep_BinderOfShape
{

public:

  //! Creates an empty ShapeBinder <br>
  Standard_EXPORT   TransferBRep_ShapeBinder();
  //! Creates a ShapeBinder with a result <br>
  Standard_EXPORT   TransferBRep_ShapeBinder(const TopoDS_Shape& res);
  //! Returns the Type of the Shape Result (under TopAbs form) <br>
  Standard_EXPORT     TopAbs_ShapeEnum ShapeType() const;
  
  Standard_EXPORT     TopoDS_Vertex Vertex() const;
  
  Standard_EXPORT     TopoDS_Edge Edge() const;
  
  Standard_EXPORT     TopoDS_Wire Wire() const;
  
  Standard_EXPORT     TopoDS_Face Face() const;
  
  Standard_EXPORT     TopoDS_Shell Shell() const;
  
  Standard_EXPORT     TopoDS_Solid Solid() const;
  
  Standard_EXPORT     TopoDS_CompSolid CompSolid() const;
  
  Standard_EXPORT     TopoDS_Compound Compound() const;


  DEFINE_STANDARD_RTTI(TransferBRep_ShapeBinder)

};
#endif
