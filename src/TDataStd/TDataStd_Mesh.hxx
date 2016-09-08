// Created on: 2015-12-10
// Created by: Vlad Romashko
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

#ifndef _TDataStd_Mesh_HeaderFile
#define _TDataStd_Mesh_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Poly_Mesh.hxx>
#include <TDF_Attribute.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>
#include <Standard_OStream.hxx>
class Standard_GUID;
class TDF_Label;
class TDF_Attribute;
class TDF_RelocationTable;

class TDataStd_Mesh;
DEFINE_STANDARD_HANDLE(TDataStd_Mesh, TDF_Attribute)

//! An Ocaf attribute containing a mesh (Poly_Mesh).
//! It duplicates all methods from Poly_Mesh (and Poly_Triangulation).
//! It is highly recommended to modify the mesh through the methods of this attribute,
//! but not directly via the underlying Poly_Mesh object.
//! In this case Undo/Redo will work fine and robust.
class TDataStd_Mesh : public TDF_Attribute
{
public:
  
  //! Static methods
  //  ==============

  //! Returns the ID of the mesh attribute.
  Standard_EXPORT static const Standard_GUID& GetID();
  
  //! Finds or creates a mesh attribute.
  Standard_EXPORT static Handle(TDataStd_Mesh) Set(const TDF_Label& label);

  //! Finds or creates a mesh attribute.
  //! Initializes the attribute by a mesh (Poly_Mesh) object.
  //! If the mesh consists of only triangles,
  //! you may put Poly_Triangulation object as a 2nd parameter of this method.
  Standard_EXPORT static Handle(TDataStd_Mesh) Set(const TDF_Label& label, const Handle(Poly_Mesh)& mesh);
  
  //! Object methods
  //  ==============

  //! A constructor.
  //! Don't use it directly, 
  //! use please the static method Set(),
  //! which returns the attribute attached to a label.
  Standard_EXPORT TDataStd_Mesh();
  
  //! Sets the mesh.
  //! If the mesh consists of only triangles,
  //! you may put Poly_Triangulation object.
  Standard_EXPORT void Set(const Handle(Poly_Mesh)& mesh);

  //! Returns the underlying mesh.
  Standard_EXPORT const Handle(Poly_Mesh)& Get() const;


  //! Poly_Mesh methods
  //  =================

  //! The methods are "covered" by this attribute to prevent direct modification of the mesh.
  //! There is no performance problem to call Poly_Mesh method through this attribute.
  //! The most of the methods are considered as "inline" by the compiler in release mode.

  //! Returns the deflection of this triangulation.
  Standard_EXPORT Standard_Real Deflection() const;

  //! Sets the deflection of this triangulation to theDeflection.
  //! See more on deflection in Polygon2D
  Standard_EXPORT void Deflection (const Standard_Real theDeflection);

  //! Deallocates the UV nodes.
  Standard_EXPORT void RemoveUVNodes();

  //! @return the number of nodes for this triangulation.
  Standard_EXPORT Standard_Integer NbNodes() const;

  //! @return the number of triangles for this triangulation.
  Standard_EXPORT Standard_Integer NbTriangles() const;

  //! @return Standard_True if 2D nodes are associated with 3D nodes for this triangulation.
  Standard_EXPORT Standard_Boolean HasUVNodes() const;

  //! Adds Node to the triangulation. If triangulation has UVNodes or Normals
  //! they will be expanded and set to zero values to match the new number of nodes.
  //! @return index of the added Node.
  Standard_EXPORT Standard_Integer AddNode (const gp_Pnt& theNode);

  //! @return node at the given index.
  //! Raises Standard_OutOfRange exception if theIndex is less than 1 or greater than NbNodes.
  Standard_EXPORT const gp_Pnt& Node (const Standard_Integer theIndex) const;

  //! The method differs from Poly_Mesh!
  //! Sets a node at the given index.
  //! Raises Standard_OutOfRange exception if theIndex is less than 1 or greater than NbNodes.
  Standard_EXPORT void SetNode (const Standard_Integer theIndex, const gp_Pnt& theNode);

  //! @return UVNode at the given index.
  //! Raises Standard_OutOfRange exception if theIndex is less than 1 or greater than NbNodes.
  Standard_EXPORT const gp_Pnt2d& UVNode (const Standard_Integer theIndex) const;

  //! The method differs from Poly_Mesh!
  //! Sets a UVNode at the given index.
  //! Raises Standard_OutOfRange exception if theIndex is less than 1 or greater than NbNodes.
  Standard_EXPORT void SetUVNode (const Standard_Integer theIndex, const gp_Pnt2d& theUVNode);

  //! Adds triangle to the triangulation.
  //! @return index of the added triangle.
  Standard_EXPORT Standard_Integer AddTriangle (const Poly_Triangle& theTriangle);

  //! @return triangle at the given index.
  //! Raises Standard_OutOfRange exception if theIndex is less than 1 or greater than NbTriangles.
  Standard_EXPORT const Poly_Triangle& Triangle (const Standard_Integer theIndex) const;

  //! The method differs from Poly_Mesh!
  //! Sets a triangle at the given index.
  //! Raises Standard_OutOfRange exception if theIndex is less than 1 or greater than NbTriangles.
  Standard_EXPORT void SetTriangle (const Standard_Integer theIndex, const Poly_Triangle& theTriangle);

  //! Sets the table of node normals.
  //! Raises exception if length of theNormals != 3 * NbNodes
  Standard_EXPORT void SetNormals (const Handle(TShort_HArray1OfShortReal)& theNormals);

  //! Changes normal at the given index.
  //! Raises Standard_OutOfRange exception.
  Standard_EXPORT void SetNormal (const Standard_Integer theIndex,
                                  const gp_Dir&          theNormal);

  //! Returns Standard_True if nodal normals are defined.
  Standard_EXPORT Standard_Boolean HasNormals() const;

  //! @return normal at the given index.
  //! Raises Standard_OutOfRange exception.
  Standard_EXPORT const gp_Dir Normal (const Standard_Integer theIndex) const;

  //! Adds element to the mesh.
  //! @param theN1 index of the first node.
  //! @param theN2 index of the second node.
  //! @param theN3 index of the third node.
  //! @return index of the added element.
  Standard_EXPORT Standard_Integer AddElement (const Standard_Integer theN1,
                                               const Standard_Integer theN2,
                                               const Standard_Integer theN3);

  //! Adds element to the mesh.
  //! @param theN1 index of the first node.
  //! @param theN2 index of the second node.
  //! @param theN3 index of the third node.
  //! @param theN4 index of the fourth node.
  //! @return index of the added element.
  Standard_EXPORT Standard_Integer AddElement (const Standard_Integer theN1,
                                               const Standard_Integer theN2,
                                               const Standard_Integer theN3,
                                               const Standard_Integer theN4);

  //! @return the number of elements for this mesh.
  Standard_EXPORT Standard_Integer NbElements() const;

  //! @return the number of quads for this mesh.
  Standard_EXPORT Standard_Integer NbQuads() const;

  //! @return element at the given index.
  //! Raises Standard_OutOfRange exception if theIndex is less than 1 or greater than NbElements.
  Standard_EXPORT const Poly_Element& Element (const Standard_Integer theIndex) const;

  //! @return nodes of the element at the given index.
  //! Raises Standard_OutOfRange exception if theIndex is less than 1 or greater than NbElements.
  Standard_EXPORT void Element (const Standard_Integer theIndex,
                                Standard_Integer& theN1,
                                Standard_Integer& theN2,
                                Standard_Integer& theN3,
                                Standard_Integer& theN4) const;

  //! Sets an element at the given index.
  //! Raises Standard_OutOfRange exception if theIndex is less than 1 or greater than NbElements.
  Standard_EXPORT void SetElement (const Standard_Integer theIndex, const Poly_Element& theElement);


  //! Inherited attribute methods
  //  ===========================

  Standard_EXPORT const Standard_GUID& ID() const Standard_OVERRIDE;
  
  Standard_EXPORT void Restore (const Handle(TDF_Attribute)& With) Standard_OVERRIDE;
  
  Standard_EXPORT Handle(TDF_Attribute) NewEmpty() const Standard_OVERRIDE;
  
  Standard_EXPORT void Paste (const Handle(TDF_Attribute)& Into, const Handle(TDF_RelocationTable)& RT) const Standard_OVERRIDE;
  
  Standard_EXPORT virtual Standard_OStream& Dump (Standard_OStream& anOS) const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTI_INLINE(TDataStd_Mesh,TDF_Attribute)

private:

  Handle(Poly_Mesh) myMesh;
};

#endif // _TDataStd_Mesh_HeaderFile
