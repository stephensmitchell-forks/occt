// Copyright (c) 1995-1999 Matra Datavision
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

#include <BRepGProp_Triangulation.hxx>
#include <BRep_Tool.hxx>
#include <NCollection_Map.hxx>

//=======================================================================
//function : Constructor
//purpose  : 
//=======================================================================
BRepGProp_Triangulation::BRepGProp_Triangulation()
{
}

//=======================================================================
//function : SetFace
//purpose  : 
//=======================================================================
void BRepGProp_Triangulation::SetFace(const TopoDS_Face& theFace)
{
  TopLoc_Location aLocation;
  Handle(Poly_Triangulation) aTriangulation = BRep_Tool::Triangulation(theFace, aLocation);
  SetTriangulation(aTriangulation, aLocation);
}

//=======================================================================
//function : SetTriangulation
//purpose  : 
//=======================================================================
void BRepGProp_Triangulation::SetTriangulation(
  const Handle(Poly_Triangulation)& theTriangulation,
  const TopLoc_Location& theLocation)
{
  myTriangulation = theTriangulation;
  myLocation = theLocation;
}

//=======================================================================
//function : triangleSurface
//purpose  : calculate the surface of a triangle specified by 3 points
//=======================================================================
inline Standard_Real triangleSurface(const gp_Pnt& n1, const gp_Pnt& n2, const gp_Pnt& n3)
{
  gp_Vec a(n1, n2);
  gp_Vec b(n1, n3);
  gp_Vec s = a.Crossed(b);
  return 0.5 * s.Magnitude();
}

//=======================================================================
//function : signedVolume
//purpose  : calculate the volume of a tetrahedron built on the given triangle
//           topped by the origin (the given center)
//=======================================================================
inline Standard_Real signedTetraVolume(const gp_Pnt& theCenter, const gp_Pnt& n1, const gp_Pnt& n2, const gp_Pnt& n3)
{
  gp_Vec p1(theCenter, n1), p2(theCenter, n2), p3(theCenter, n3);
  return p1.Dot(p2.Crossed(p3)) / 6.0f;
}

//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================
Standard_Boolean BRepGProp_Triangulation::Perform(Property theProperty, const gp_Pnt& theCenter)
{
  dim = 0;
  if (myTriangulation.IsNull())
    return Standard_False;

  Standard_Integer n1, n2, n3;
  for (Standard_Integer i = 1, n = myTriangulation->NbTriangles(); i <= n; i++)
  {
    const Poly_Triangle& aTriangle = myTriangulation->Triangle(i);
    aTriangle.Get(n1, n2, n3);
    if (theProperty == Volume)
      dim += signedTetraVolume(theCenter,
        myTriangulation->Node(n1),
        myTriangulation->Node(n2),
        myTriangulation->Node(n3));
    else if (theProperty == Surface)
      dim += triangleSurface(myTriangulation->Node(n1),
        myTriangulation->Node(n2),
        myTriangulation->Node(n3));
    else
      return Standard_False;
  }

  return Standard_True;
}
