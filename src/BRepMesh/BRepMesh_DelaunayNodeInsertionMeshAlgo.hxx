// Created on: 2016-07-07
// Copyright (c) 2016 OPEN CASCADE SAS
// Created by: Oleg AGASHIN
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

#ifndef _BRepMesh_DelaunayNodeInsertionMeshAlgo_HeaderFile
#define _BRepMesh_DelaunayNodeInsertionMeshAlgo_HeaderFile

#include <BRepMesh_NodeInsertionMeshAlgo.hxx>

//! Extends base Delaunay meshing algo in order to enable 
//! possibility of addition of free vertices into the mesh.
template<class RangeSplitter>
class BRepMesh_DelaunayNodeInsertionMeshAlgo : public BRepMesh_NodeInsertionMeshAlgo<RangeSplitter, BRepMesh_DelaunayBaseMeshAlgo>
{
private:
  // Typedef for OCCT RTTI
  typedef BRepMesh_NodeInsertionMeshAlgo<RangeSplitter, BRepMesh_DelaunayBaseMeshAlgo> InsertionBaseClass;

public:

  //! Constructor.
  Standard_EXPORT BRepMesh_DelaunayNodeInsertionMeshAlgo()
  {
  }

  //! Destructor.
  Standard_EXPORT virtual ~BRepMesh_DelaunayNodeInsertionMeshAlgo()
  {
  }

  DEFINE_STANDARD_RTTI_INLINE(BRepMesh_DelaunayNodeInsertionMeshAlgo, InsertionBaseClass)

protected:

  //! Perfroms processing of generated mesh. Generates surface nodes and inserts them into structure.
  Standard_EXPORT virtual void postProcessMesh(BRepMesh_Delaun& theMesher) Standard_OVERRIDE
  {
    InsertionBaseClass::postProcessMesh(theMesher);

    const Handle(IMeshData::ListOfPnt2d) aSurfaceNodes =
      getRangeSplitter().GenerateSurfaceNodes(getParameters());

    insertNodes(aSurfaceNodes, theMesher);
  }

  //! Inserts nodes into mesh.
  Standard_EXPORT Standard_Boolean insertNodes(
    const Handle(IMeshData::ListOfPnt2d)& theNodes,
    BRepMesh_Delaun&                      theMesher)
  {
    if (theNodes.IsNull() || theNodes->IsEmpty())
    {
      return Standard_False;
    }

    IMeshData::VectorOfInteger aVertexIndexes(theNodes->Size(), getAllocator());
    IMeshData::ListOfPnt2d::Iterator aNodesIt(*theNodes);
    for (Standard_Integer aNodeIt = 1; aNodesIt.More(); aNodesIt.Next(), ++aNodeIt)
    {
      const gp_Pnt2d& aPnt2d = aNodesIt.Value();
      if (getClassifier()->Perform(aPnt2d) == TopAbs_IN)
      {
        aVertexIndexes.Append(registerNode(getRangeSplitter().Point(aPnt2d),
                                           aPnt2d, BRepMesh_Free, Standard_False));
      }
    }

    theMesher.AddVertices(aVertexIndexes);
    return !aVertexIndexes.IsEmpty();
  }
};

#endif
