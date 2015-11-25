// Created on: 2015-10-23
// Created by: Varvara POSKONINA
// Copyright (c) 2005-2014 OPEN CASCADE SAS
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

#include <MeshVS_LODDataSource.hxx>

#include <Graphic3d_ArrayOfPoints.hxx>
#include <Graphic3d_ArrayOfTriangles.hxx>
#include <Graphic3d_ArrayOfSegments.hxx>
#include <Graphic3d_LOD.hxx>
#include <MeshVS_Drawer.hxx>
#include <MeshVS_DrawerAttribute.hxx>
#include <MeshVS_Mesh.hxx>
#include <MeshVS_MapOfTwoNodes.hxx>
#include <MeshVS_MeshPrsBuilder.hxx>
#include <MeshVS_SymmetricPairHasher.hxx>
#include <MeshVS_Tool.hxx>
#include <Prs3d_Presentation.hxx>
#include <Prs3d_Root.hxx>
#include <TColStd_HPackedMapOfInteger.hxx>
#include <TColStd_MapIteratorOfPackedMapOfInteger.hxx>
#include <TColgp_SequenceOfXYZ.hxx>
#include <StlMesh_SequenceOfMeshTriangle.hxx>
#include <StlMesh_MeshTriangle.hxx>

//=======================================================================
// function : MeshVS_LODDataSource
// purpose  :
//=======================================================================
MeshVS_LODDataSource::MeshVS_LODDataSource (const Handle(StlMesh_Mesh)& theMesh)
  : myTriangles (1, theMesh->Triangles().Length()),
    myNodes     (1, theMesh->Vertices().Length())
{
  if (theMesh.IsNull())
    return;

  const TColgp_SequenceOfXYZ& aVerts = theMesh->Vertices();
  for (Standard_Integer aNodeIdx = 1; aNodeIdx <= aVerts.Length(); ++aNodeIdx)
  {
    myNodeIdxs.Add (aNodeIdx);
    myNodes.ChangeValue (aNodeIdx) = aVerts (aNodeIdx);
  }

  const StlMesh_SequenceOfMeshTriangle& aTriangles = theMesh->Triangles();
  for (Standard_Integer aTrgIdx = 1; aTrgIdx <= aTriangles.Length(); ++aTrgIdx)
  {
    myTriangleIdxs.Add (aTrgIdx);
    const Handle(StlMesh_MeshTriangle)& aTriangle = aTriangles.Value (aTrgIdx);
    Standard_Integer aNodeIdxs[3];
    Standard_Real aNormalX, aNormalY, aNormalZ;
    aTriangle->GetVertexAndOrientation (aNodeIdxs[0], aNodeIdxs[1], aNodeIdxs[2], aNormalX, aNormalY, aNormalZ);
    myTriangles.ChangeValue (aTrgIdx) = Poly_Triangle (aNodeIdxs[0], aNodeIdxs[1], aNodeIdxs[2]);
  }
}

//=======================================================================
// function : GetGeom
// purpose  : Returns full info about the entity according to the index and type of element given
//=======================================================================
Standard_Boolean MeshVS_LODDataSource::GetGeom (const Standard_Integer theId,
                                                const Standard_Boolean theIsElement,
                                                TColStd_Array1OfReal&  theCoords,
                                                Standard_Integer&      theNodesNb,
                                                MeshVS_EntityType&     theEntityType) const
{
  if (theId < 1)
    return Standard_False;

  if (theIsElement)
  {
    if (theId > myTriangleIdxs.Extent() || theCoords.Length() < 9)
      return Standard_False;

    Standard_Integer aNode1, aNode2, aNode3;
    myTriangles.Value (theId).Get (aNode1, aNode2, aNode3);
    gp_Pnt aPnts[3] = { myNodes.Value (aNode1),
                        myNodes.Value (aNode2),
                        myNodes.Value (aNode3) };
    for (Standard_Integer aPntIdx = 0, anIdx = 1; aPntIdx < 3; ++aPntIdx)
    {
      for (Standard_Integer aCoordIdx = 0; aCoordIdx < 3; aCoordIdx++)
      {
        theCoords (anIdx++) = aPnts[aPntIdx].XYZ().GetData()[aCoordIdx];
      }
    }

    theNodesNb = 3;
    theEntityType = MeshVS_ET_Face;

    return Standard_True;
  }
  else
  {
    if (theId > myNodeIdxs.Extent() || theCoords.Length() < 3)
      return Standard_False;

    const gp_Pnt& aNode = myNodes.Value (theId);
    theCoords (1) = aNode.X();
    theCoords (2) = aNode.Y();
    theCoords (3) = aNode.Z();

    theNodesNb = 1;
    theEntityType = MeshVS_ET_Node;

    return Standard_True;
  }
}

//=======================================================================
// function : GetGeomType
// purpose  : Returns type of geometry according to the index and type of element given
//=======================================================================
Standard_Boolean MeshVS_LODDataSource::GetGeomType (const Standard_Integer /*theId*/,
                                                    const Standard_Boolean theIsElement,
                                                    MeshVS_EntityType&     theEntityType) const
{
  theEntityType = theIsElement ? MeshVS_ET_Face : MeshVS_ET_Node;
  return Standard_True;
}

//=======================================================================
// function : GetAddr
// purpose  :
//=======================================================================
Standard_Address MeshVS_LODDataSource::GetAddr (const Standard_Integer /*theId*/,
                                                const Standard_Boolean /*theIsElement*/) const
{
  return NULL;
}

//=======================================================================
// function : GetNodesByElement
// purpose  : Returns nodes forming a triangle with id theId
//=======================================================================
Standard_Boolean MeshVS_LODDataSource::GetNodesByElement (const Standard_Integer   theId,
                                                          TColStd_Array1OfInteger& theNodeIds,
                                                          Standard_Integer&        theNodesNb) const
{
  if (theId < 1 || theId > myTriangleIdxs.Extent() || theNodeIds.Length() < 3)
    return Standard_False;

  const Standard_Integer aLowerIdx = theNodeIds.Lower();
  myTriangles.Value (theId).Get (theNodeIds.ChangeValue (aLowerIdx),
                                 theNodeIds.ChangeValue (aLowerIdx + 1),
                                 theNodeIds.ChangeValue (aLowerIdx + 2));
  theNodesNb = 3;
  return Standard_True;
}

//=======================================================================
// function : GetAllNodes
// purpose  : This method makes storing usless map excusable
//=======================================================================
const TColStd_PackedMapOfInteger& MeshVS_LODDataSource::GetAllNodes() const
{
  return myNodeIdxs;
}

//=======================================================================
// function : GetAllElements
// purpose  : This method makes storing usless map excusable
//=======================================================================
const TColStd_PackedMapOfInteger& MeshVS_LODDataSource::GetAllElements() const
{
  return myTriangleIdxs;
}
