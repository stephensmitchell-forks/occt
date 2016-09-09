// Created on: 2015-08-28
// Created by: Danila ULYANOV
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

#ifndef _BRepMesh_TriangulatedPatch_HeaderFile
#define _BRepMesh_TriangulatedPatch_HeaderFile

#include <BVH_Triangulation.hxx>

#include <BRepMesh_OrientedEdge.hxx>
#include <BRepMesh_EdgeSet.hxx>

#include <TopoDS_Face.hxx>

//! Class representing a patch composed from triangles.
class BRepMesh_TriangulatedPatch : public BVH_Triangulation<Standard_Real, 3>, public Standard_Transient
{

public:

  DEFINE_STANDARD_ALLOC

  typedef BRepMesh_EdgeSet<Standard_Real, 3> EdgeSet;

public:

  //! Creates new triangulated patch.
  Standard_EXPORT BRepMesh_TriangulatedPatch(TopoDS_Face theFace);

  //! Updates edge and triangle data.
  Standard_EXPORT void PreprocessData();

  //! Flips the normals orientation of the patch.
  Standard_EXPORT void Flip();

  //! Flips only visibility coefficients of the patch.
  Standard_EXPORT void FlipVisibility();

  //! Returns triangle Id stored in 4th component of triangle record.
  Standard_Integer TriangleId (Standard_Integer theIndex) const
  {
    return Elements[theIndex].w();
  }

  //! Returns set of patch boundary edges.
  BRepMesh_EdgeSet<Standard_Real, 3>& BoundaryEdgeSet()
  {
    return myBoundaryEdgeSet;
  }

  //! Returns set of patch boundary edges.
  const BRepMesh_EdgeSet<Standard_Real, 3>& BoundaryEdgeSet() const
  {
    return myBoundaryEdgeSet;
  }

  //! Returns boundary edge by index.
  BRepMesh_OrientedEdge BoundaryEdge (Standard_Integer theIndex) const
  {
    BVH_Vec4i anEdge = myBoundaryEdgeSet.Elements[theIndex];
    return BRepMesh_OrientedEdge (anEdge.w() > 0.0 ? anEdge.x() : anEdge.y(),
                                  anEdge.w() > 0.0 ? anEdge.y() : anEdge.x());
  }

  //! Returns edge vertices.
  void GetEdgeVertices (const BRepMesh_OrientedEdge& theEdge,
                        BVH_Vec3d& theVertex1,
                        BVH_Vec3d& theVertex2) const
  {
    theVertex1 = Vertices[theEdge.FirstNode()];
    theVertex2 = Vertices[theEdge.LastNode()];
  }

  //! Appends another patch geometry.
  Standard_EXPORT void Append (const Handle (BRepMesh_TriangulatedPatch)& thePatch);

  //! Returns precomputed CDF for triangles.
  const std::vector<Standard_Real>& TriangleCDF() const
  {
    return myTriangleCDF;
  }

  //! Returns total area of the patch.
  Standard_Real TotalArea() const
  {
    return myTotalArea;
  }

  //! Sets total area of the patch.
  void SetTotalArea (Standard_Real theVal)
  {
    myTotalArea = theVal;
  }

  //! Returns front visibility of the patch.
  Standard_Real FrontVisibility() const
  {
    return myFrontVisibility;
  }

  //! Sets front visibility of the patch.
  void SetFrontVisibility (Standard_Real theVal)
  {
    myFrontVisibility = theVal;
  }

  //! Returns back visibility of the patch.
  Standard_Real BackVisibility() const
  {
    return myBackVisibility;
  }

  //! Sets back visibility of the patch.
  void SetBackVisibility (Standard_Real theVal)
  {
    myBackVisibility = theVal;
  }

  //! Returns front distance for the patch.
  Standard_Real FrontDistance() const
  {
    return myFrontDistance;
  }

  //! Sets front distance for the patch.
  void SetFrontDistance (Standard_Real theVal)
  {
    myFrontDistance = theVal;
  }

  //! Returns back distance for the patch.
  Standard_Real BackDistance() const
  {
    return myBackDistance;
  }

  //! Sets back distance for the patch.
  void SetBackDistance (Standard_Real theVal)
  {
    myBackDistance = theVal;
  }

  //! Returns original topological face.
  TopoDS_Face Face() const
  {
    return myFace;
  }

protected:

  //! Extracts edges from triangulation. Fills edge -> faces map. Extracts boundary edges. Fills edge set.
  void computeBoundaryEdges();

  //! Computes areas of triangles. Computes total patch area. Computes sampling probabilities.
  void updateArea();

private:

  //! Set of boundary edges.
  EdgeSet myBoundaryEdgeSet;

  //! Total triangle area.
  Standard_Real myTotalArea;

  //! Total front visibility.
  Standard_Real myFrontVisibility;

  //! Total back visibility.
  Standard_Real myBackVisibility;

  //! Accumulated front distance of the patch.
  Standard_Real myFrontDistance;

  //! Accumulated back distance of the patch.
  Standard_Real myBackDistance;

  //! Cumulative distribution function.
  std::vector<Standard_Real> myTriangleCDF;

  //! Reference to original topological face.
  TopoDS_Face myFace;

  DEFINE_STANDARD_RTTI_INLINE (BRepMesh_TriangulatedPatch, Standard_Transient);
};

#endif
