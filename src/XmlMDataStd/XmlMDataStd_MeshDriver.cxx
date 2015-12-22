// Created on: 2015-12-15
// Created by: Vlad Romashko
// Copyright (c) 2007-2014 OPEN CASCADE SAS
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

#include <CDM_MessageDriver.hxx>
#include <NCollection_LocalArray.hxx>
#include <Standard_Type.hxx>
#include <TDF_Attribute.hxx>
#include <XmlMDataStd_MeshDriver.hxx>
#include <XmlObjMgt.hxx>
#include <XmlObjMgt_Persistent.hxx>
#include <TDataStd_Mesh.hxx>
#include <LDOM_OSStream.hxx>
#include <BRepTools_ShapeSet.hxx>

IMPLEMENT_STANDARD_RTTIEXT(XmlMDataStd_MeshDriver,XmlMDF_ADriver)
IMPLEMENT_DOMSTRING (MeshString, "mesh")
IMPLEMENT_DOMSTRING (NullString, "null")
IMPLEMENT_DOMSTRING (ExistString, "exists")

//=======================================================================
//function : XmlMDataStd_MeshDriver
//purpose  : Constructor
//=======================================================================
XmlMDataStd_MeshDriver::XmlMDataStd_MeshDriver(const Handle(CDM_MessageDriver)& theMsgDriver)
     : XmlMDF_ADriver (theMsgDriver, NULL)
{

}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================
Handle(TDF_Attribute) XmlMDataStd_MeshDriver::NewEmpty() const
{
  return new TDataStd_Mesh();
}

//=======================================================================
//function : Paste
//purpose  : persistent -> transient (retrieve)
//=======================================================================
Standard_Boolean XmlMDataStd_MeshDriver::Paste(const XmlObjMgt_Persistent&  theSource,
                                               const Handle(TDF_Attribute)& theTarget,
                                               XmlObjMgt_RRelocationTable&  ) const
{
  const XmlObjMgt_Element& element = theSource;
  Handle(TDataStd_Mesh) mesh = Handle(TDataStd_Mesh)::DownCast(theTarget);

  // Read the FirstIndex; if the attribute is absent initialize to 1
  XmlObjMgt_DOMString meshStatus = element.getAttribute(::MeshString());
  if (meshStatus == NULL ||
      meshStatus.Type() != LDOMBasicString::LDOM_AsciiDoc ||
      strcmp(meshStatus.GetString(), ::ExistString().GetString())) 
  {
    // No mesh.
    return Standard_True;
  }

  // Get mesh as a string.
  const XmlObjMgt_DOMString& data = XmlObjMgt::GetStringValue(element);
  std::stringstream stream(std::string(data.GetString()));

  // Read the mesh.
  BRepTools_ShapeSet shapeSet;
  TColStd_IndexedMapOfTransient meshes;
  shapeSet.ReadMeshes(stream, meshes);

  // Set mesh.
  if (!meshes.IsEmpty())
  {
    // We expect only one mesh.
    Handle(Poly_Mesh) M = Handle(Poly_Mesh)::DownCast(meshes(1));
    if (!M.IsNull())
      mesh->Set(M);
  }

  return Standard_True;
}

//=======================================================================
//function : Paste
//purpose  : transient -> persistent (store)
//=======================================================================
void XmlMDataStd_MeshDriver::Paste(const Handle(TDF_Attribute)& theSource,
                                   XmlObjMgt_Persistent&        theTarget,
                                   XmlObjMgt_SRelocationTable&  ) const
{
  const Handle(TDataStd_Mesh) meshAttr = Handle(TDataStd_Mesh)::DownCast(theSource);
  if (meshAttr->Get().IsNull())
    theTarget.Element().setAttribute(::MeshString(), ::NullString());
  else
  {
    theTarget.Element().setAttribute(::MeshString(), ::ExistString());
    
    // Analyse the size of the mesh
    // (to allocate properly the string array).
    const Handle(Poly_Mesh)& mesh = meshAttr->Get();
    Standard_Integer size = mesh->NbNodes();
    size *= 3; // 3 coordinates for a node
    size *= 8; // 8 characters are used to represent a coordinate (double) in XML
    size += 4 * 5 * mesh->NbElements(); // space for elements (triangles and quadrangles)
    size *= 2; // just in case :-)
    if (!size)
      size = 1;

    // Allocate a string stream.
    LDOM_OSStream stream(size);

    // Write the mesh.
    BRepTools_ShapeSet shapeSet;
    TColStd_IndexedMapOfTransient meshes;
    meshes.Add(mesh);
    shapeSet.WriteMeshes(stream, meshes, Standard_True/*compact*/);
    stream<<ends;

    Standard_Character* dump = (Standard_Character*)stream.str(); // copying! Don't forget to delete it.
    XmlObjMgt::SetStringValue(theTarget, dump, Standard_True);
    delete[] dump;
  }
}
