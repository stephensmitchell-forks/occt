// Copyright (c) 2015 OPEN CASCADE SAS
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

#include <Poly_Mesh.hxx>

#include <Standard_DefineHandle.hxx>

IMPLEMENT_STANDARD_HANDLE  (Poly_Mesh, Poly_Triangulation)
IMPLEMENT_STANDARD_RTTIEXT (Poly_Mesh, Poly_Triangulation)

//=======================================================================
//function : Poly_Mesh
//purpose  :
//=======================================================================

Poly_Mesh::Poly_Mesh (const Standard_Boolean theHasUVNodes)
: Poly_Triangulation (0, 0, theHasUVNodes),
  myNbQuads (0)
{}

//=======================================================================
//function : AddTriangle
//purpose  :
//=======================================================================

Standard_Integer Poly_Mesh::AddTriangle (const Poly_Triangle& theTriangle)
{
  Standard_Integer anIndex = Poly_Triangulation::AddTriangle (theTriangle);
  return AddElement (Poly_Element (anIndex, 0));
}

//=======================================================================
//function : AddElement
//purpose  :
//=======================================================================

Standard_Integer Poly_Mesh::AddElement (const Poly_Element& theElement)
{
  myElements.Append (theElement);
  if (theElement.Value (2) != 0)
  {
    myNbQuads++;
  }
  return myElements.Size();
}

//=======================================================================
//function : Element
//purpose  :
//=======================================================================

const Poly_Element& Poly_Mesh::Element (const Standard_Integer theIndex)
{
  Standard_OutOfRange_Raise_if (theIndex < 1 || theIndex > myElements.Size(),
                                "Poly_Element::Element : index out of range");
  return myElements.Value (theIndex - 1);
}

//=======================================================================
//function : SetElement
//purpose  :
//=======================================================================

void Poly_Mesh::SetElement (const Standard_Integer theIndex, const Poly_Element& theElement)
{
  Standard_OutOfRange_Raise_if (theIndex < 1 || theIndex > myElements.Size(),
                                "Poly_Element::SetElement : index out of range");

  if (myElements.Value (theIndex - 1).Value (2) == 0 && theElement.Value (2) != 0)
  {
    myNbQuads++;
  }
  else if (myElements.Value (theIndex - 1).Value (2) != 0 && theElement.Value (2) == 0)
  {
    myNbQuads--;
  }

  myElements.SetValue (theIndex - 1, theElement);
}
