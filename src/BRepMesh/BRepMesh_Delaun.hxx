// Copyright (c) 2013-2014 OPEN CASCADE SAS
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

#ifndef _BRepMesh_Delaun_HeaderFile
#define _BRepMesh_Delaun_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Macro.hxx>

#include <BRepMesh_CircleTool.hxx>
#include <BRepMesh_Triangle.hxx>
#include <BRepMesh_Edge.hxx>
#include <BRepMesh_MapOfInteger.hxx>
#include <BRepMesh_MapOfIntegerInteger.hxx>
#include <BRepMesh_DataStructureOfDelaun.hxx>
#include <NCollection_Sequence.hxx>
#include <Message_ProgressIndicator.hxx>

class Bnd_B2d;
class Bnd_Box2d;
class BRepMesh_Array1OfVertexOfDelaun;
class BRepMesh_Vertex;
class TColStd_Array1OfInteger;
class TColStd_SequenceOfInteger;
class TColStd_MapOfInteger;

//! Compute the Delaunay's triangulation with the algorithm of Watson.
class BRepMesh_Delaun
{
public:

  DEFINE_STANDARD_ALLOC

  //! Creates the triangulation with an empty Mesh data structure.
  Standard_EXPORT BRepMesh_Delaun (BRepMesh_Array1OfVertexOfDelaun& theVertices,
                                   const Standard_Boolean           isPositive = Standard_True,
                                   const Handle(Message_ProgressIndicator) & progress = 0);

  //! Creates the triangulation with an existent Mesh data structure.
  Standard_EXPORT BRepMesh_Delaun (const Handle(BRepMesh_DataStructureOfDelaun)& theOldMesh,
                                   BRepMesh_Array1OfVertexOfDelaun&              theVertices,
                                   const Standard_Boolean                        isPositive = Standard_True,
                                   const Handle(Message_ProgressIndicator) & progress = 0);

  //! Creates the triangulation with an existant Mesh data structure.
  Standard_EXPORT BRepMesh_Delaun (const Handle(BRepMesh_DataStructureOfDelaun)& theOldMesh,
                                   TColStd_Array1OfInteger&                      theVertexIndices,
                                   const Standard_Boolean                        isPositive = Standard_True,
                                   const Handle(Message_ProgressIndicator) & progress = 0);

  //! Initializes the triangulation with an array of vertices.
  Standard_EXPORT void Init (BRepMesh_Array1OfVertexOfDelaun& theVertices);

  //! Removes a vertex from the triangulation.
  Standard_EXPORT void RemoveVertex (const BRepMesh_Vertex& theVertex);

  //! Adds some vertices into the triangulation.
  Standard_EXPORT void AddVertices (BRepMesh_Array1OfVertexOfDelaun& theVertices);

  //! Modify mesh to use the edge.
  //! @return True if done
  Standard_EXPORT Standard_Boolean UseEdge (const Standard_Integer theEdge);

  //! Gives the Mesh data structure.
  const Handle(BRepMesh_DataStructureOfDelaun)& Result() const
  {
    return myMeshData;
  }

  //! Gives the list of frontier edges.
  inline Handle(BRepMesh_MapOfInteger) Frontier() const
  {
    return getEdgesByType (BRepMesh_Frontier);
  }

  //! Gives the list of internal edges.
  inline Handle(BRepMesh_MapOfInteger) InternalEdges() const
  {
    return getEdgesByType (BRepMesh_Fixed);
  }

  //! Gives the list of free edges used only one time
  inline Handle(BRepMesh_MapOfInteger) FreeEdges() const
  {
    return getEdgesByType (BRepMesh_Free);
  }

  //! Gives vertex with the given index
  inline const BRepMesh_Vertex& GetVertex (const Standard_Integer theIndex) const
  {
    return myMeshData->GetNode (theIndex);
  }

  //! Gives edge with the given index
  inline const BRepMesh_Edge& GetEdge (const Standard_Integer theIndex) const
  {
    return myMeshData->GetLink (theIndex);
  }

  //! Gives triangle with the given index
  inline const BRepMesh_Triangle& GetTriangle (const Standard_Integer theIndex) const
  {
    return myMeshData->GetElement (theIndex);
  }

  //! Test is the given triangle contains the given vertex.
  //! If theEdgeOn != 0 the vertex lies onto the edge index
  //! returned through this parameter.
  Standard_EXPORT Standard_Boolean Contains (const Standard_Integer theTriangleId,
                                             const BRepMesh_Vertex& theVertex,
                                             Standard_Integer&      theEdgeOn) const;

protected:

  //! Gives the list of edges with type defined by the input parameter.
  //! If the given type is BRepMesh_Free returns list of edges
  //! that have number of connected elements less or equal 1.
  Handle(BRepMesh_MapOfInteger) GetEdgesByType (const BRepMesh_DegreeOfFreedom theEdgeType) const;

  //! Create super mesh and run triangulation procedure.
  void Perform (Bnd_Box2d&               theBndBox,
                TColStd_Array1OfInteger& theVertexIndices);

  //! Build the super mesh.
  void SuperMesh (const Bnd_Box2d& theBox);

  //! Computes the triangulation and adds the vertices,
  //! edges and triangles to the Mesh data structure.
  void Compute (TColStd_Array1OfInteger& theVertexIndices);

  //! Adjust the mesh on the frontier.
  void FrontierAdjust();

  //! Find left polygon of the given edge and call meshPolygon.
  void MeshLeftPolygonOf (const Standard_Integer theEdgeIndex,
                                      const Standard_Boolean isForward);

  //! Triangulatiion of a closed polygon described by the list
  //! of indexes of its edges in the structure.
  //! (negative index means reversed edge)
  void MeshPolygon (TColStd_SequenceOfInteger&     thePolygon,
                    NCollection_Sequence<Bnd_B2d>& thePolyBoxes);

  //! Triangulation of closed polygon containing only three edges.
  inline Standard_Boolean MeshElementaryPolygon (const TColStd_SequenceOfInteger& thePolygon);

  //! Creates the triangles beetween the given node and the given polyline.
  void CreateTriangles (const Standard_Integer        theVertexIndex,
                        BRepMesh_MapOfIntegerInteger& thePoly);

  //! Add a triangle based on the given oriented edges into mesh
  inline void AddTriangle (const Standard_Integer (&theEdgesId)[3],
                           const Standard_Boolean (&theEdgesOri)[3],
                           const Standard_Integer (&theNodesId)[3]);

  //! Deletes the triangle with the given index and adds the free edges into the map.
  //! When an edge is suppressed more than one time it is destroyed.
  void DeleteTriangle (const Standard_Integer        theIndex,
                       BRepMesh_MapOfIntegerInteger& theLoopEdges);

  //! Creates the triangles on new nodes.
  void CreateTrianglesOnNewVertices (TColStd_Array1OfInteger& theVertexIndices);

  //! Cleanup mesh from the free triangles.
  void CleanupMesh();

  //! Remove internal triangles from the given polygon.
  void CleanupPolygon (const TColStd_SequenceOfInteger&     thePolygon,
                       const NCollection_Sequence<Bnd_B2d>& thePolyBoxes);

  //! Checks intersection between the two segments.
  Standard_Boolean IntSegSeg (const BRepMesh_Edge&   theEdge1,
                     const BRepMesh_Edge&   theEdge2,
                     const Standard_Boolean includeEnds) const;

private:

  Handle(BRepMesh_DataStructureOfDelaun) myMeshData;
  Standard_Boolean                       myIsPositiveOrientation;
  BRepMesh_CircleTool                    myCircles;
  Standard_Integer                       mySupVert[3];
  BRepMesh_Triangle                      mySupTrian;
  Handle(Message_ProgressIndicator)      myProgress;
};

#endif
