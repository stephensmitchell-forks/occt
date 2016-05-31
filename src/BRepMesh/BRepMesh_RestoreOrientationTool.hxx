// Created on: 2015-08-27
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

#ifndef _BRepMesh_RestoreOrientationTool_HeaderFile
#define _BRepMesh_RestoreOrientationTool_HeaderFile

#include <Standard.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Face.hxx>
#include <Message_ProgressIndicator.hxx>

#include <BRepMesh_TriangulatedPatch.hxx>

//! Class representing a collection of simple patches.
struct BRepMesh_SuperPatch
{
  //! Create super patch from single patch.
  BRepMesh_SuperPatch (Standard_Integer thePatchId)
    : Index (thePatchId)
  {
    PatchIds.push_back (thePatchId);
  }

  //! Create super patch from a pair of super patches.
  BRepMesh_SuperPatch (const BRepMesh_SuperPatch& theFirst,
                       const BRepMesh_SuperPatch& theSecond,
                       Standard_Integer theIndex)
    : Index (theIndex)
  {
    PatchIds.resize (theFirst.PatchIds.size() + theSecond.PatchIds.size());

    std::vector<Standard_Integer>::iterator aNextPos =
      std::copy (theFirst.PatchIds.begin(), theFirst.PatchIds.end(), PatchIds.begin());

    std::copy (theSecond.PatchIds.begin(), theSecond.PatchIds.end(), aNextPos);
  }

  std::vector<Standard_Integer> PatchIds;
  Standard_Integer Index;
};

//! Class representing a pair of super patches.
struct BRepMesh_SuperPatchPair
{
  BRepMesh_SuperPatchPair (const BRepMesh_SuperPatch* theFirstPatch = NULL,
                           const BRepMesh_SuperPatch* theSecondPatch  = NULL,
                           Standard_Real theCoherency = 0.0)
    : FirstPatch (theFirstPatch),
      SecondPatch (theSecondPatch),
      Coherence (theCoherency)
  {
    //
  }

  bool operator < (const BRepMesh_SuperPatchPair& theOther) const
  {
    return (std::abs (Coherence) < std::abs (theOther.Coherence));
  }

  bool HavePatch (Standard_Integer thePatch)
  {
    return FirstPatch->Index == thePatch || SecondPatch->Index == thePatch;
  }

  const BRepMesh_SuperPatch* FirstPatch;
  const BRepMesh_SuperPatch* SecondPatch;
  Standard_Real Coherence;
};

//! List of faces.
typedef std::vector<TopoDS_Face> BRepMesh_FaceList;

//! This tool intended to restore consistent orientation of surfaces.
//! It takes as input OCCT shape and outputs a set of re-oriented faces.
class BRepMesh_RestoreOrientationTool : public Standard_Transient
{

public:

  DEFINE_STANDARD_ALLOC

  //! Creates uninitialized orientation tool.
  Standard_EXPORT BRepMesh_RestoreOrientationTool (const bool theVisibilityOnly = false,
                                                   const Standard_Integer theMinFaceRays = 50,
                                                   const Standard_Integer theMaxFaceRays = 2000);

  //! Creates orientation tool from the given shape.
  Standard_EXPORT BRepMesh_RestoreOrientationTool (const TopoDS_Shape& theShape,
                                                   const bool theVisibilityOnly = false,
                                                   const Standard_Integer theMinFaceRays = 50,
                                                   const Standard_Integer theMaxFaceRays = 2000);

public:

  //! Loads the given shape into orientation tool.
  Standard_EXPORT void Init (const TopoDS_Shape& theShape);

  //! Performs restoring of consistent orientation.
  Standard_EXPORT void Perform (const Handle(Message_ProgressIndicator) thePI = NULL);

  //! Returns "Visibility only" mode.
  bool VisibilityOnly() const
  {
    return myVisibilityOnly;
  }

  //! Set "Visibility only" mode.
  void SetVisibilityOnly (bool isEnabled)
  {
    myVisibilityOnly = isEnabled;
  }

  //! True if the algorithm successfully performed.
  bool IsDone() const
  {
    return myIsDone;
  }

  //! Returns shape with restored normal orientation.
  Standard_EXPORT TopoDS_Shape Shape() const;

  //! Returns true if the given face should be reversed according to the algorithm.
  //! Valid only if IsDone() returns true.
  //! O(N)
  Standard_EXPORT bool IsFlipped (const TopoDS_Face theFace) const;

  //! Returns true if the given face should be reversed according to the algorithm.
  //! Valid only if IsDone() returns true.
  //! Uses indices in order provided by TopExp_Explorer starting from zero.
  //! O(1)
  Standard_EXPORT bool IsFlipped (const Standard_Integer theFaceIndex) const;

protected:

  //! Calculates the coherence value between two patches.
  static Standard_Real computeCoherence (Handle (BRepMesh_TriangulatedPatch)& thePatch0,
                                         Handle (BRepMesh_TriangulatedPatch)& thePatch1);

  //! Calculates the visibility value for the patch.
  void computeVisibility (BVH_Triangulation<Standard_Real, 3>& theTriangulation,
                          Handle (BRepMesh_TriangulatedPatch)& thePatch,
                          Standard_Integer theRaysNb);

  //! Returns the closest triangle hit by the ray.
  Standard_Integer traceRay (const BVH_Vec3d& theOrigin,
                             const BVH_Vec3d& theDirect,
                             BVH_Triangulation<Standard_Real, 3>& theTriangulation,
                             Standard_Real& theDistance);

  //! Extracts a triangulation from the face.
  static Standard_Integer extractTriangulation (const TopoDS_Face& theShape,
                                                BVH_Triangulation<Standard_Real, 3>& theTriangulation);

  //! Calculates the coherence value between two edges.
  static Standard_Real edgeCoherence (const BRepMesh_OrientedEdge& theEdge1,
                                      const BRepMesh_TriangulatedPatch&  thePatch1,
                                      const BRepMesh_OrientedEdge& theEdge2,
                                      const BRepMesh_TriangulatedPatch&  thePatch2);

public:

  // Minimum number of traced rays per face.
  Standard_Integer MinFaceRays;

  // Maximum number of traced rays per face.
  Standard_Integer MaxFaceRays;

protected:

  //! Is output ready?
  bool myIsDone;

  //! Resulted shape.
  mutable TopoDS_Shape myShape;

  //! List of triangulated faces of the shape.
  BRepMesh_FaceList myFaceList;

  //! List of patches.
  std::vector<Handle (BRepMesh_TriangulatedPatch)> myPatches;

  //! Vertex and normal data of triangulated shape.
  BVH_Triangulation<Standard_Real, 3> myTriangulation;

  std::vector<BRepMesh_SuperPatchPair> myPatchQueue;

  //! Maps Id intervals to topological faces.
  std::vector<Standard_Integer> myFaceIdIntervals;

  // Flipped flags for all faces.
  std::vector<char> myFlipped;

  //! Use only visibility metric?
  bool myVisibilityOnly;

  DEFINE_STANDARD_RTTI_INLINE (BRepMesh_RestoreOrientationTool, Standard_Transient);
};

#endif
