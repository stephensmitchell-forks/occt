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

#ifndef _MeshVS_LODDataSource_Header
#define _MeshVS_LODDataSource_Header

#include <AIS_InteractiveObject.hxx>
#include <MeshVS_DataSource.hxx>
#include <Poly_Array1OfTriangle.hxx>
#include <Poly_Triangle.hxx>
#include <StlMesh_Mesh.hxx>

//! TODO: VPA: think how to avoid copying mesh data. Now it is done for returning useless maps
//!            in GetAllElements() and GetAllNodes() because they must return const links. Moreover,
//!            copying data allows to avoid cases when indexation in Poly_Triangluation inner
//!            arrays starts from arbitrary numbers (if it is really possible).
class MeshVS_LODDataSource : public MeshVS_DataSource
{
public:

  Standard_EXPORT MeshVS_LODDataSource (const Handle(StlMesh_Mesh)& theMesh);

  Standard_EXPORT virtual ~MeshVS_LODDataSource() {};

  //! Returns geometry information about node or element
  //! ID is the numerical identificator of node or element
  //! IsElement indicates this ID describe node ( if Standard_False ) or element ( if Standard_True )
  //! Coords is an array of co-ordinates of node(s).
  //! For node it is only 3 numbers: X, Y, Z in the strict order
  //! For element it is 3*n numbers, where n is number of this element vertices
  //! The order is strict also: X1, Y1, Z1, X2,...., where Xi, Yi, Zi are co-ordinates of vertices
  //! NbNodes is number of nodes. It is recommended this parameter to be set to 1 for node.
  //! Type is type of node or element (from enumeration). It is recommended this parameter to be set to
  //! MeshVS_ET_Node for node.
  Standard_EXPORT virtual Standard_Boolean GetGeom (const Standard_Integer theId,
                                                    const Standard_Boolean theIsElement,
                                                    TColStd_Array1OfReal&  theCoords,
                                                    Standard_Integer&      theNodesNb,
                                                    MeshVS_EntityType&     theEntityType) const Standard_OVERRIDE;

  //! This method is similar to GetGeom, but returns only element or node type.
  Standard_EXPORT virtual Standard_Boolean GetGeomType (const Standard_Integer theId,
                                                        const Standard_Boolean theIsElement,
                                                        MeshVS_EntityType&     theEntityType) const Standard_OVERRIDE;

  //! This method returns pointer which represents element or node data structure.
  //! This address will be saved in MeshVS_MeshEntityOwner, so that you can access to data structure fast
  //! by the method Owner(). In the redefined method you can return NULL.
  //! ID is the numerical identificator of node or element
  //! IsElement indicates this ID describe node ( if Standard_False ) or element ( if Standard_True )
  Standard_EXPORT virtual Standard_Address GetAddr (const Standard_Integer theId,
                                                    const Standard_Boolean theIsElement) const Standard_OVERRIDE;

  //! This method returns information about nodes this element consist of.
  //! ID is the numerical identificator of element.
  //! NodeIDs is the output array of nodes IDs in correct order,
  //! the same as coordinates returned by GetGeom().
  //! NbNodes is number of nodes (number of items set in NodeIDs).
  //! Returns False if element does not exist
  Standard_EXPORT virtual Standard_Boolean GetNodesByElement (const Standard_Integer   theId,
                                                              TColStd_Array1OfInteger& theNodeIds,
                                                              Standard_Integer&        theNodesNb) const Standard_OVERRIDE;

  //! This method returns map of all nodes the object consist of.
  Standard_EXPORT virtual const TColStd_PackedMapOfInteger& GetAllNodes() const Standard_OVERRIDE;

  //! This method returns map of all elements the object consist of.
  Standard_EXPORT virtual const TColStd_PackedMapOfInteger& GetAllElements() const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTI (MeshVS_LODDataSource, MeshVS_DataSource)

private:
  TColStd_PackedMapOfInteger myNodeIdxs;
  TColStd_PackedMapOfInteger myTriangleIdxs;
  TColgp_Array1OfPnt         myNodes;
  Poly_Array1OfTriangle      myTriangles;
};

DEFINE_STANDARD_HANDLE (MeshVS_LODDataSource, MeshVS_DataSource)

#endif // _MeshVS_LODDataSource_Header
