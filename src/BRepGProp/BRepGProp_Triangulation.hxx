// Created on: 
// Created by: 
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

#ifndef _BRepGProp_Triangulation_HeaderFile
#define _BRepGProp_Triangulation_HeaderFile

#include <GProp_GProps.hxx>
#include <TopLoc_Location.hxx>

class TopoDS_Face;
class Poly_Triangulation;

//! Computes the global properties on triangulation in 3D space.
class BRepGProp_Triangulation : public GProp_GProps
{
public:
  DEFINE_STANDARD_ALLOC

  enum Property
  {
    Surface,
    Volume
  };

  Standard_EXPORT BRepGProp_Triangulation();

  Standard_EXPORT void SetFace(const TopoDS_Face&);
  Standard_EXPORT void SetTriangulation(const Handle(Poly_Triangulation)& theTriangulation,
                                        const TopLoc_Location& theLocation);

  Standard_EXPORT Standard_Boolean Perform(Property theProperty, const gp_Pnt& theCenter);

private:
  Handle(Poly_Triangulation) myTriangulation;
  TopLoc_Location            myLocation;
};

#endif // _BRepGProp_Triangulation_HeaderFile
