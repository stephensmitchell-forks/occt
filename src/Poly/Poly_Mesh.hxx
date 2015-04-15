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

#ifndef _Poly_Mesh_HeaderFile
#define _Poly_Mesh_HeaderFile

#include <Poly_Element.hxx>
#include <Poly_Triangulation.hxx>

//! This class is extension for Poly_Triangulation.
//! It allows to store mesh with quad polygons as table of Poly_Element.
//! Keep in mind that when you add a triangle, it is also added to the table of elements
//! as Poly_Element. And it will have first index set to triangle index from Poly_Triangulation
//! and second index will be set to 0.
class Poly_Mesh : public Poly_Triangulation
{

public:

  //! Constructs an empty mesh.
  //! theHasUVNodes flag indicates whether 2D nodes will be associated with 3D ones, (i.e. to enable a 2D representation).
  Standard_EXPORT Poly_Mesh (const Standard_Boolean theHasUVNodes = Standard_False);

  //! Adds triangle to the mesh.
  //! @return index of the added element.
  Standard_EXPORT Standard_Integer AddTriangle (const Poly_Triangle& theTriangle) Standard_OVERRIDE;

  //! Adds element to the mesh.
  //! @return index of the added element.
  //! Raises exception if at least one of the element indices is less than 1 or greater than NbTriangles.
  Standard_EXPORT Standard_Integer AddElement (const Poly_Element& theElement);

  //! @return the number of elements for this mesh.
  Standard_Integer NbElements() const { return myElements.Size(); }

  //! @return the number of quads for this mesh.
  Standard_Integer NbQuads() const { return myNbQuads; }

  //! @return element at the given index.
  //! Raises Standard_OutOfRange exception if theIndex is less than 1 or greater than NbElements.
  Standard_EXPORT const Poly_Element& Element (const Standard_Integer theIndex);

  //! Give access to the element at the given index.
  //! Raises Standard_OutOfRange exception if theIndex is less than 1 or greater than NbElements.
  Standard_EXPORT void SetElement (const Standard_Integer theIndex, const Poly_Element& theElement);

private:

  NCollection_Vector<Poly_Element> myElements;
  Standard_Integer                 myNbQuads;

public:

  DEFINE_STANDARD_RTTI(Poly_Mesh)

};

DEFINE_STANDARD_HANDLE(Poly_Mesh, Poly_Triangulation)

#endif // _Poly_Mesh_HeaderFile
