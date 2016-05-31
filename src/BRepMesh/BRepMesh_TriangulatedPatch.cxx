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

#include <BRepMesh_TriangulatedPatch.hxx>
#include <BVH_LinearBuilder.hxx>

#include <set>
#include <map>

namespace {

  //! Compare functor for symmetrical pairs of integers.
  struct SymmetricalPairComparator
  {
    bool operator() (const std::pair<int, int>& theEdge1,
                      const std::pair<int, int>& theEdge2) const
    {
      const int aMin1 = std::min (theEdge1.first, theEdge1.second);
      const int aMin2 = std::min (theEdge2.first, theEdge2.second);

      return (aMin1 < aMin2) ||
        (aMin1 == aMin2 && std::max (theEdge1.first, theEdge1.second) < std::max (theEdge2.first, theEdge2.second));
    }

    bool equal (const std::pair<int, int>& theEdge1,
                const std::pair<int, int>& theEdge2) const
    {
      return std::min (theEdge1.first, theEdge1.second) == std::min (theEdge2.first, theEdge2.second)
          && std::max (theEdge1.first, theEdge1.second) == std::max (theEdge2.first, theEdge2.second);
    }
  };

  // Returns the area of triangle
  Standard_Real triangleArea (const BVH_Vec3d& theV1,
                              const BVH_Vec3d& theV2,
                              const BVH_Vec3d& theV3)
  {
    BVH_Vec3d aE1 = theV2 - theV1;
    BVH_Vec3d aE2 = theV3 - theV1;

    return BVH_Vec3d::Cross (aE1, aE2).Modulus() * 0.5;
  }

  //! Set of integers.
  typedef std::set<int> SetOfInteger;

  //! Type of edge-triangles connectivity map.
  typedef std::map<std::pair<int, int>, SetOfInteger, SymmetricalPairComparator> EdgeFaceMap;
}

// =======================================================================
// function : BRepMesh_TriangulatedPatch
// purpose  :
// =======================================================================
BRepMesh_TriangulatedPatch::BRepMesh_TriangulatedPatch (TopoDS_Face theFace)
  : BVH_Triangulation<Standard_Real, 3>(),
    myBoundaryEdgeSet (Vertices),
    myTotalArea (0.0),
    myFrontVisibility (0.0),
    myBackVisibility (0.0),
    myFrontDistance (0.0),
    myBackDistance (0.0),
    myFace (theFace)
{
  myBuilder = new BVH_LinearBuilder<Standard_Real, 3> (5 /* leaf size */, 32 /* max height */);
}

// =======================================================================
// function : Append
// purpose  :
// =======================================================================
void BRepMesh_TriangulatedPatch::Append (const Handle (BRepMesh_TriangulatedPatch)& thePatch)
{
  // Vertices
  Standard_Size aVertOffset = Vertices.size();
  Vertices.resize (Vertices.size() + thePatch->Vertices.size());

  std::copy (thePatch->Vertices.begin(), thePatch->Vertices.end(), Vertices.begin() + aVertOffset);

  // Triangles
  struct
  {
    Standard_Integer Offset;
    BVH_Vec4i operator() (const BVH_Vec4i& theTriangle) { return theTriangle + BVH_Vec4i (Offset, Offset, Offset, 0); }
  } AddOffsetHelper;

  AddOffsetHelper.Offset = static_cast<Standard_Integer> (aVertOffset);

  Standard_Size aFaceOffset = Elements.size();
  Elements.resize (Elements.size() + thePatch->Elements.size());

  std::transform (thePatch->Elements.begin(), thePatch->Elements.end(), Elements.begin() + aFaceOffset, AddOffsetHelper);

  // Area
  myTotalArea += thePatch->TotalArea();
}

// =======================================================================
// function : computeBoundareEdges
// purpose  :
// =======================================================================
void BRepMesh_TriangulatedPatch::computeBoundaryEdges()
{
  myBoundaryEdgeSet.MarkDirty();

  if (!myBoundaryEdgeSet.Elements.empty())
  {
    return;
  }

  EdgeFaceMap anEdgeFaceMap;

  // use triangle indices for connectivity extraction
  for (Standard_Size aTrgIdx = 0; aTrgIdx < Elements.size(); ++aTrgIdx)
  {
    const BVH_Vec4i& aTriangle = Elements[aTrgIdx];

    for (Standard_Integer anEdgeStart = 0, edgeEnd = 1; anEdgeStart < 3; ++anEdgeStart, edgeEnd = (anEdgeStart + 1) % 3)
    {
      std::pair<Standard_Integer, Standard_Integer> anEdge (aTriangle[anEdgeStart], aTriangle[edgeEnd]);

      anEdgeFaceMap[anEdge].insert (static_cast<Standard_Integer> (aTrgIdx));
    }
  }

  // extract boundary and non-manifold edges
  myBoundaryEdgeSet.Elements.clear();

  for (EdgeFaceMap::iterator anIter = anEdgeFaceMap.begin(); anIter != anEdgeFaceMap.end(); ++anIter)
  {
    const SetOfInteger& anAdjacentFaces = anIter->second;

    const std::pair<Standard_Integer, Standard_Integer>& anEdge = anIter->first;

    if (anAdjacentFaces.size() == 1) // boundary
    {
      myBoundaryEdgeSet.Elements.push_back (BVH_Vec4i (anEdge.first, anEdge.second, 0, 1));
    }
  }

  myBoundaryEdgeSet.BVH();
}

// =======================================================================
// function : PreprocessData
// purpose  :
// =======================================================================
void BRepMesh_TriangulatedPatch::PreprocessData()
{

  computeBoundaryEdges();
  updateArea();
}

// =======================================================================
// function : Flip
// purpose  :
// =======================================================================
void BRepMesh_TriangulatedPatch::Flip()
{
  // mark that triangles was flipped
  struct
  {
    void operator() (BVH_Vec4i& theTriangle) { theTriangle.w() = -theTriangle.w(); }
  } TriangleFlipHelper;

  std::for_each (Elements.begin(), Elements.end(), TriangleFlipHelper);

  // mark that edges was flipped
  struct
  {
    void operator() (BVH_Vec4i& theEdge) { theEdge.w() = -theEdge.w(); }
  } EdgeFlipHelper;

  std::for_each (myBoundaryEdgeSet.Elements.begin(), myBoundaryEdgeSet.Elements.end(), EdgeFlipHelper);

  FlipVisibility();
}

// =======================================================================
// function : FlipVisibility
// purpose  :
// =======================================================================
void BRepMesh_TriangulatedPatch::FlipVisibility()
{
  std::swap (myFrontVisibility, myBackVisibility);
  std::swap (myFrontDistance, myBackDistance);
}

// =======================================================================
// function : updateArea
// purpose  :
// =======================================================================
void BRepMesh_TriangulatedPatch::updateArea()
{
  myTotalArea = 0.0;

  myTriangleCDF.resize (Elements.size());

  for (Standard_Size aTrgIdx = 0; aTrgIdx < Elements.size(); ++aTrgIdx)
  {
    const BVH_Vec4i& aTriangle = Elements[aTrgIdx];

    const BVH_Vec3d& aV1 = Vertices[aTriangle.x()];
    const BVH_Vec3d& aV2 = Vertices[aTriangle.y()];
    const BVH_Vec3d& aV3 = Vertices[aTriangle.z()];

    myTriangleCDF[aTrgIdx] = triangleArea (aV1, aV2, aV3) + (aTrgIdx == 0 ? 0.0 : myTriangleCDF[aTrgIdx - 1]);
  }

  myTotalArea = myTriangleCDF.back();
  Standard_Real aInvArea = 1.0 / myTotalArea;

  for (Standard_Size aTrgIdx = 0; aTrgIdx < myTriangleCDF.size(); ++aTrgIdx)
  {
    myTriangleCDF[aTrgIdx] *= aInvArea;
  }
}
