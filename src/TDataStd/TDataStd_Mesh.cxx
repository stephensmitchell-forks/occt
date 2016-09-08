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

#include <TDataStd_Mesh.hxx>
#include <Standard_GUID.hxx>
#include <Standard_Type.hxx>
#include <TDF_Attribute.hxx>
#include <TDF_Label.hxx>
#include <TDF_RelocationTable.hxx>

//=======================================================================
//function : GetID
//purpose  : Returns the ID of the mesh attribute.
//=======================================================================
const Standard_GUID& TDataStd_Mesh::GetID() 
{ 
  static Standard_GUID TDataStd_MeshID ("D7E3F1CF-38A4-4DCA-94F4-51C31F3FCBA5");
  return TDataStd_MeshID; 
}

//=======================================================================
//function : Set
//purpose  : Finds or creates a mesh attribute.
//=======================================================================
Handle(TDataStd_Mesh) TDataStd_Mesh::Set(const TDF_Label& label) 
{
  Handle(TDataStd_Mesh) A;
  if (!label.FindAttribute (TDataStd_Mesh::GetID(), A)) 
  {
    A = new TDataStd_Mesh;
    label.AddAttribute(A);
  }
  return A;
}

//=======================================================================
//function : Set
//purpose  : Finds or creates a mesh attribute.
//           Initializes the attribute by a mesh (Poly_Mesh) object.
//           If the mesh consists of only triangles,
//           you may put Poly_Triangulation object as a 2nd parameter of this method.
//=======================================================================
Handle(TDataStd_Mesh) TDataStd_Mesh::Set(const TDF_Label& label, const Handle(Poly_Mesh)& mesh)
{
   Handle(TDataStd_Mesh) M = TDataStd_Mesh::Set(label);
   M->Set(mesh);
   return M;
}

//=======================================================================
//function : TDataStd_Mesh
//purpose  : A constructor.
//           Don't use it directly, 
//           use please the static method Set(),
//           which returns the attribute attached to a label.
//=======================================================================
TDataStd_Mesh::TDataStd_Mesh() 
{

}
  
//=======================================================================
//function : TDataStd_Mesh
//purpose  : Sets the mesh.
//           If the mesh consists of only triangles,
//           you may put Poly_Triangulation object.
//=======================================================================
void TDataStd_Mesh::Set(const Handle(Poly_Mesh)& mesh)
{
  Backup();
  myMesh = mesh;
}

//=======================================================================
//function : TDataStd_Mesh
//purpose  : Returns the underlying mesh.
//=======================================================================
const Handle(Poly_Mesh)& TDataStd_Mesh::Get() const
{
  return myMesh;
}

// Poly_Mesh methods

// The methods are "covered" by this attribute to prevent direct modification of the mesh.
// There is no performance problem to call Poly_Mesh method through this attribute.
// The most of the methods are considered as "inline" by the compiler in release mode.

//=======================================================================
//function : Deflection
//purpose  : Returns the deflection of this triangulation.
//=======================================================================
Standard_Real TDataStd_Mesh::Deflection() const
{
  return myMesh->Deflection();
}

//=======================================================================
//function : Deflection
//purpose  : Sets the deflection of this triangulation to theDeflection.
//           See more on deflection in Polygon2D
//=======================================================================
void TDataStd_Mesh::Deflection (const Standard_Real theDeflection)
{
  Backup();
  myMesh->Deflection(theDeflection);
}

//=======================================================================
//function : RemoveUVNodes
//purpose  : Deallocates the UV nodes.
//=======================================================================
void TDataStd_Mesh::RemoveUVNodes()
{
  Backup();
  myMesh->RemoveUVNodes();
}

//=======================================================================
//function : NbNodes
//purpose  : return the number of nodes for this triangulation.
//=======================================================================
Standard_Integer TDataStd_Mesh::NbNodes() const
{
  return myMesh->NbNodes();
}

//=======================================================================
//function : NbTriangles
//purpose  : return the number of triangles for this triangulation.
//=======================================================================
Standard_Integer TDataStd_Mesh::NbTriangles() const
{
  return myMesh->NbTriangles();
}

//=======================================================================
//function : HasUVNodes
//purpose  : return Standard_True if 2D nodes are associated with 3D nodes for this triangulation.
//=======================================================================
Standard_Boolean TDataStd_Mesh::HasUVNodes() const
{
  return myMesh->HasUVNodes();
}

//=======================================================================
//function : AddNode
//purpose  : Adds Node to the triangulation. If triangulation has UVNodes or Normals
//           they will be expanded and set to zero values to match the new number of nodes.
//           return index of the added Node.
//=======================================================================
Standard_Integer TDataStd_Mesh::AddNode (const gp_Pnt& theNode)
{
  Backup();
  return myMesh->AddNode(theNode);
}

//=======================================================================
//function : Node
//purpose  : return node at the given index.
//           Raises Standard_OutOfRange exception if theIndex is less than 1 or greater than NbNodes.
//=======================================================================
const gp_Pnt& TDataStd_Mesh::Node (const Standard_Integer theIndex) const
{
  return myMesh->Node(theIndex);
}

//=======================================================================
//function : SetNode
//purpose  : The method differs from Poly_Mesh
//           Sets a node at the given index.
//           Raises Standard_OutOfRange exception if theIndex is less than 1 or greater than NbNodes.
//=======================================================================
void TDataStd_Mesh::SetNode (const Standard_Integer theIndex, const gp_Pnt& theNode)
{
  Backup();
  myMesh->ChangeNode(theIndex) = theNode;
}

//=======================================================================
//function : UVNode
//purpose  : return UVNode at the given index.
//           Raises Standard_OutOfRange exception if theIndex is less than 1 or greater than NbNodes.
//=======================================================================
const gp_Pnt2d& TDataStd_Mesh::UVNode (const Standard_Integer theIndex) const
{
  return myMesh->UVNode(theIndex);
}

//=======================================================================
//function : SetUVNode
//purpose  : The method differs from Poly_Mesh
//           Sets a UVNode at the given index.
//           Raises Standard_OutOfRange exception if theIndex is less than 1 or greater than NbNodes.
//=======================================================================
void TDataStd_Mesh::SetUVNode (const Standard_Integer theIndex, const gp_Pnt2d& theUVNode)
{
  Backup();
  myMesh->ChangeUVNode(theIndex) = theUVNode;
}

//=======================================================================
//function : AddTriangle
//purpose  : Adds triangle to the triangulation.
//           return index of the added triangle.
//=======================================================================
Standard_Integer TDataStd_Mesh::AddTriangle (const Poly_Triangle& theTriangle)
{
  Backup();
  return myMesh->AddTriangle(theTriangle);
}

//=======================================================================
//function : Triangle
//purpose  : return triangle at the given index.
//           Raises Standard_OutOfRange exception if theIndex is less than 1 or greater than NbTriangles.
//=======================================================================
const Poly_Triangle& TDataStd_Mesh::Triangle (const Standard_Integer theIndex) const
{
  return myMesh->Triangle(theIndex);
}

//=======================================================================
//function : SetTriangle
//purpose  : The method differs from Poly_Mesh
//           Sets a triangle at the given index.
//           Raises Standard_OutOfRange exception if theIndex is less than 1 or greater than NbTriangles.
//=======================================================================
void TDataStd_Mesh::SetTriangle (const Standard_Integer theIndex, const Poly_Triangle& theTriangle)
{
  Backup();
  myMesh->ChangeTriangle(theIndex) = theTriangle;
}

//=======================================================================
//function : SetNormals
//purpose  : Sets the table of node normals.
//           Raises exception if length of theNormals = 3 * NbNodes
//=======================================================================
void TDataStd_Mesh::SetNormals (const Handle(TShort_HArray1OfShortReal)& theNormals)
{
  Backup();
  myMesh->SetNormals(theNormals);
}

//=======================================================================
//function : SetNormal
//purpose  : Changes normal at the given index.
//           Raises Standard_OutOfRange exception.
//=======================================================================
void TDataStd_Mesh::SetNormal (const Standard_Integer theIndex,
                               const gp_Dir&          theNormal)
{
  Backup();
  myMesh->SetNormal(theIndex, theNormal);
}

//=======================================================================
//function : HasNormals
//purpose  : Returns Standard_True if nodal normals are defined.
//=======================================================================
Standard_Boolean TDataStd_Mesh::HasNormals() const
{
  return myMesh->HasNormals();
}

//=======================================================================
//function : Normal
//purpose  : return normal at the given index.
//           Raises Standard_OutOfRange exception.
//=======================================================================
const gp_Dir TDataStd_Mesh::Normal (const Standard_Integer theIndex) const
{
  return myMesh->Normal(theIndex);
}

//=======================================================================
//function : AddElement
//purpose  :
//=======================================================================
Standard_Integer TDataStd_Mesh::AddElement (const Standard_Integer theN1,
                                            const Standard_Integer theN2,
                                            const Standard_Integer theN3)
{
  Backup();
  return myMesh->AddElement(theN1, theN2, theN3);
}

//=======================================================================
//function : AddElement
//purpose  :
//=======================================================================
Standard_Integer TDataStd_Mesh::AddElement (const Standard_Integer theN1,
                                            const Standard_Integer theN2,
                                            const Standard_Integer theN3,
                                            const Standard_Integer theN4)
{
  Backup();
  return myMesh->AddElement(theN1, theN2, theN3, theN4);
}

//=======================================================================
//function : NbElements
//purpose  : return the number of elements for this mesh.
//=======================================================================
Standard_Integer TDataStd_Mesh::NbElements() const
{
  return myMesh->NbElements();
}

//=======================================================================
//function : NbQuads
//purpose  : return the number of quads for this mesh.
//=======================================================================
Standard_Integer TDataStd_Mesh::NbQuads() const
{
  return myMesh->NbQuads();
}

//=======================================================================
//function : Element
//purpose  : return element at the given index.
//           Raises Standard_OutOfRange exception if theIndex is less than 1 or greater than NbElements.
//=======================================================================
const Poly_Element& TDataStd_Mesh::Element (const Standard_Integer theIndex) const
{
  return myMesh->Element(theIndex);
}

//=======================================================================
//function : Element
//purpose  : return nodes of the element at the given index.
//           Raises Standard_OutOfRange exception if theIndex is less than 1 or greater than NbElements.
//=======================================================================
void TDataStd_Mesh::Element (const Standard_Integer theIndex,
                            Standard_Integer& theN1,
                            Standard_Integer& theN2,
                            Standard_Integer& theN3,
                            Standard_Integer& theN4) const
{
  myMesh->Element(theIndex, theN1, theN2, theN3, theN4);
}

//=======================================================================
//function : SetElement
//purpose  : Sets an element at the given index.
//           Raises Standard_OutOfRange exception if theIndex is less than 1 or greater than NbElements.
//=======================================================================
void TDataStd_Mesh::SetElement (const Standard_Integer theIndex, const Poly_Element& theElement)
{
  Backup();
  myMesh->SetElement(theIndex, theElement);
}

//=======================================================================
//function : ID
//purpose  : 
//=======================================================================
const Standard_GUID& TDataStd_Mesh::ID () const 
{ 
  return GetID(); 
}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================
Handle(TDF_Attribute) TDataStd_Mesh::NewEmpty () const
{  
  return new TDataStd_Mesh(); 
}

//=======================================================================
//function : Restore
//purpose  : 
//=======================================================================
void TDataStd_Mesh::Restore(const Handle(TDF_Attribute)& With) 
{
  myMesh.Nullify();
  Handle(TDataStd_Mesh) M = Handle(TDataStd_Mesh)::DownCast(With);
  if (!M->myMesh.IsNull())
  {
    Handle(Poly_Triangulation) T = M->myMesh->Copy();
    if (!T.IsNull())
        myMesh = Handle(Poly_Mesh)::DownCast(T);
  }
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================
void TDataStd_Mesh::Paste (const Handle(TDF_Attribute)& Into,
                           const Handle(TDF_RelocationTable)& ) const
{
  Handle(TDataStd_Mesh) M = Handle(TDataStd_Mesh)::DownCast(Into);
  M->myMesh.Nullify();
  if (!myMesh.IsNull())
  {
      Handle(Poly_Triangulation) T = myMesh->Copy();
      if (!T.IsNull())
        M->myMesh = Handle(Poly_Mesh)::DownCast(T);
  }
}

//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================
Standard_OStream& TDataStd_Mesh::Dump (Standard_OStream& anOS) const
{  
  anOS << "Mesh";
  //TODO: Make a good dump.
  return anOS;
}
