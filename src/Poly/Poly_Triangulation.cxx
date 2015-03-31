// Created on: 1995-03-06
// Created by: Laurent PAINNOT
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

#include <Poly_Triangulation.hxx>

#include <gp_Pnt.hxx>
#include <Standard_NullObject.hxx>
#include <TColgp_HArray1OfPnt2d.hxx>
#include <TShort_HArray1OfShortReal.hxx>

IMPLEMENT_STANDARD_HANDLE (Poly_Triangulation,MMgt_TShared)
IMPLEMENT_STANDARD_RTTIEXT(Poly_Triangulation,MMgt_TShared)

//=======================================================================
//function : Poly_Triangulation
//purpose  : 
//=======================================================================

Poly_Triangulation::Poly_Triangulation(const Standard_Integer NbNodes, 
                                       const Standard_Integer NbTriangles,
                                       const Standard_Boolean UVNodes) :
  myDeflection (0)
{
  myNodes.SetValue (NbNodes - 1, gp_Pnt());
  myTriangles.SetValue (NbTriangles - 1, Poly_Triangle());
  if (UVNodes)
  {
    myUVNodes.SetValue (NbNodes - 1, gp_Pnt2d());
  }
}

//=======================================================================
//function : Poly_Triangulation
//purpose  : 
//=======================================================================

Poly_Triangulation::Poly_Triangulation(const TColgp_Array1OfPnt&    Nodes, 
                                       const Poly_Array1OfTriangle& Triangles) :
  myDeflection (0)
{
  for (Standard_Integer anIndex = Nodes.Upper(); anIndex >= Nodes.Lower(); anIndex--)
  {
    myNodes.SetValue (anIndex - 1, Nodes (anIndex));
  }
  for (Standard_Integer anIndex = Triangles.Upper(); anIndex >= Triangles.Lower(); anIndex--)
  {
    myTriangles.SetValue (anIndex - 1, Triangles (anIndex));
  }
}



//=======================================================================
//function : Poly_Triangulation
//purpose  : 
//=======================================================================

Poly_Triangulation::Poly_Triangulation(const TColgp_Array1OfPnt&    Nodes,
                                       const TColgp_Array1OfPnt2d&  UVNodes,
                                       const Poly_Array1OfTriangle& Triangles) :
  myDeflection (0)
{
  for (Standard_Integer anIndex = Nodes.Upper(); anIndex >= Nodes.Lower(); anIndex--)
  {
    myNodes.SetValue (anIndex - 1, Nodes (anIndex));
  }
  for (Standard_Integer anIndex = UVNodes.Upper(); anIndex >= UVNodes.Lower(); anIndex--)
  {
    myUVNodes.SetValue (anIndex - 1, UVNodes (anIndex));
  }
  for (Standard_Integer anIndex = Triangles.Upper(); anIndex >= Triangles.Lower(); anIndex--)
  {
    myTriangles.SetValue (anIndex - 1, Triangles (anIndex));
  }
}

//=======================================================================
//function : Deflection
//purpose  : 
//=======================================================================

Standard_Real Poly_Triangulation::Deflection() const 
{
  return myDeflection;
}

//=======================================================================
//function : Deflection
//purpose  : 
//=======================================================================

void Poly_Triangulation::Deflection(const Standard_Real D)
{
  myDeflection = D;
}



//=======================================================================
//function : RemoveUVNodes
//purpose  : 
//=======================================================================

void Poly_Triangulation::RemoveUVNodes()
{
  myUVNodes.Clear();
}

//=======================================================================
//function : Node
//purpose  : 
//=======================================================================

const gp_Pnt& Poly_Triangulation::Node (const Standard_Integer theIndex) const 
{
  if (theIndex < 1 || theIndex > myNodes.Size())
  {
    Standard_OutOfRange::Raise ("Poly_Triangulation::Node : index out of range");
  }
  return myNodes.Value (theIndex - 1);
}

//=======================================================================
//function : ChangeNode
//purpose  : 
//=======================================================================

gp_Pnt& Poly_Triangulation::ChangeNode (const Standard_Integer theIndex)
{
  if (theIndex < 1 || theIndex > myNodes.Size())
  {
    Standard_OutOfRange::Raise ("Poly_Triangulation::ChangeNode : index out of range");
  }
  return myNodes.ChangeValue (theIndex - 1);
}

//=======================================================================
//function : UVNode
//purpose  : 
//=======================================================================

const gp_Pnt2d& Poly_Triangulation::UVNode (const Standard_Integer theIndex) const 
{
  if (myUVNodes.IsEmpty() || theIndex < 1 || theIndex > myUVNodes.Size())
  {
    Standard_OutOfRange::Raise ("Poly_Triangulation::UVNode : index out of range");
  }
  return myUVNodes.Value (theIndex - 1);
}

//=======================================================================
//function : ChangeUVNode
//purpose  : 
//=======================================================================

gp_Pnt2d& Poly_Triangulation::ChangeUVNode (const Standard_Integer theIndex)
{
  if (myUVNodes.IsEmpty() || theIndex < 1 || theIndex > myUVNodes.Size())
  {
    Standard_OutOfRange::Raise ("Poly_Triangulation::ChangeUVNode : index out of range");
  }
  return myUVNodes.ChangeValue (theIndex - 1);
}

//=======================================================================
//function : Triangle
//purpose  : 
//=======================================================================

Standard_Integer Poly_Triangulation::AddTriangle (const Poly_Triangle& theTriangle)
{
  if  (theTriangle(1) < 1 || theTriangle(1) > myNodes.Size()
    || theTriangle(2) < 1 || theTriangle(2) > myNodes.Size()
    || theTriangle(3) < 1 || theTriangle(3) > myNodes.Size())
  {
    Standard_OutOfRange::Raise ("Poly_Triangulation::AddTriangle : index out of range");
  }
  myTriangles.Append (theTriangle);
  return myTriangles.Size();
}

//=======================================================================
//function : Triangle
//purpose  : 
//=======================================================================

const Poly_Triangle& Poly_Triangulation::Triangle (const Standard_Integer theIndex) const 
{
  if (theIndex < 1 || theIndex > myTriangles.Size())
  {
    Standard_OutOfRange::Raise ("Poly_Triangulation::Triangle : index out of range");
  }
  return myTriangles.Value (theIndex - 1);
}

//=======================================================================
//function : ChangeTriangle
//purpose  : 
//=======================================================================

Poly_Triangle& Poly_Triangulation::ChangeTriangle (const Standard_Integer theIndex)
{
  if (theIndex < 1 || theIndex > myTriangles.Size())
  {
    Standard_OutOfRange::Raise ("Poly_Triangulation::ChangeTriangle : index out of range");
  }
  return myTriangles.ChangeValue (theIndex - 1);
}

//=======================================================================
//function : SetNormals
//purpose  : 
//=======================================================================

void Poly_Triangulation::SetNormals
                        (const Handle(TShort_HArray1OfShortReal)& theNormals)
{

  if (theNormals.IsNull() || theNormals->Length() != 3 * NbNodes())
  {
    Standard_DomainError::Raise("Poly_Triangulation::SetNormals : wrong length");
  }

  for (Standard_Integer anIndex = theNormals->Upper(); anIndex >= theNormals->Lower(); anIndex--)
  {
    myNormals.SetValue (anIndex, (theNormals->Value (anIndex)));
  }

}

//=======================================================================
//function : Normal
//purpose  : 
//=======================================================================

const gp_Dir Poly_Triangulation::Normal (const Standard_Integer theIndex) const
{

  if (myNormals.IsEmpty() || theIndex < 1 || theIndex > myNodes.Size())
  {
    Standard_NullObject::Raise("Poly_Triangulation::Normal : empty array or index out of range");
  }

  Standard_Integer anIndex = (theIndex - 1) * 3;
  return gp_Dir (myNormals (anIndex),
                 myNormals (anIndex + 1),
                 myNormals (anIndex + 2));
}

//=======================================================================
//function : SetNormal
//purpose  : 
//=======================================================================

void Poly_Triangulation::SetNormal (const Standard_Integer theIndex, const gp_Dir& theNormal)
{

  if (myNormals.IsEmpty() || theIndex < 1 || theIndex > myNodes.Size())
  {
    Standard_NullObject::Raise("Poly_Triangulation::SetNormal : empty array or index out of range");
  }

  Standard_Integer anIndex = (theIndex - 1) * 3;
  myNormals.ChangeValue (anIndex)     = (Standard_ShortReal) theNormal.X();
  myNormals.ChangeValue (anIndex + 1) = (Standard_ShortReal) theNormal.Y();
  myNormals.ChangeValue (anIndex + 2) = (Standard_ShortReal) theNormal.Z();

}

//=======================================================================
//function : HasNormals
//purpose  : 
//=======================================================================

Standard_Boolean Poly_Triangulation::HasNormals() const
{

  if(myNormals.IsEmpty() || myNormals.Length() != 3 * NbNodes())
  {
    return Standard_False;
  }
  return Standard_True;
}

Standard_Integer Poly_Triangulation::AddQuad (const Standard_Integer theFirstTriangleIndex, const Standard_Integer theSecondTriangleIndex)
{
  if (theFirstTriangleIndex < 1 || theFirstTriangleIndex > myTriangles.Size()
    || theSecondTriangleIndex < 1 || theSecondTriangleIndex > myTriangles.Size())
  {
    Standard_OutOfRange::Raise ("Poly_Triangulation::AddQuad : index out of range");
  }
  if (myPolygons.PolygonsIndexes.IsEmpty())
  {
    myPolygons.PolygonsIndexes.SetValue (myTriangles.Size() - 1, 0);
  }
  myPolygons.NbQuads++;
  myPolygons.NbUsedTriangles += 2;
  myPolygons.PolygonsIndexes (theFirstTriangleIndex) = myPolygons.NbQuads;
  myPolygons.PolygonsIndexes (theSecondTriangleIndex) = myPolygons.NbQuads;

  return myPolygons.NbQuads;
}

void Poly_Triangulation::Quad (const Standard_Integer theIndex, Standard_Integer& theFirstTriangleIndex, Standard_Integer& theSecondTriangleIndex)
{
  Standard_Integer aFirstTriangleIndex (0), aSecondTriangleIndex (0);
  if (myPolygons.NbQuads == 0 || theIndex > myPolygons.NbQuads)
  {
    theFirstTriangleIndex  = 0;
    theSecondTriangleIndex = 0;
    Standard_OutOfRange::Raise ("Poly_Triangulation::Quad : quads are empty or index out of range");
  }
  Standard_Integer anIndex = 0;
  while (anIndex < myPolygons.PolygonsIndexes.Size() - 1)
  {
    if (myPolygons.PolygonsIndexes (anIndex) == theIndex)
    {
      aFirstTriangleIndex = anIndex + 1;
      break;
    }
    anIndex++;
  }
  anIndex++;
  while (anIndex < myPolygons.PolygonsIndexes.Size())
  {
    if (myPolygons.PolygonsIndexes (anIndex) == theIndex)
    {
      aSecondTriangleIndex = anIndex + 1;
      break;
    }
    anIndex++;
  }
  if (aFirstTriangleIndex != 0 && aSecondTriangleIndex != 0)
  {
    theFirstTriangleIndex = aFirstTriangleIndex;
    theSecondTriangleIndex = aSecondTriangleIndex;
    return;
  }
  else
  {
    Standard_OutOfRange::Raise ("Poly_Triangulation::Quad : quad with given index does not exist");
  }
}
