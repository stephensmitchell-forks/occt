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

#include <BRepMesh_RestoreOrientationTool.hxx>

#include <OSD_Timer.hxx>
#include <TCollection_AsciiString.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS_Compound.hxx>

#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <Poly_Triangulation.hxx>

#include <NCollection_Map.hxx>
#include <NCollection_DataMap.hxx>
#include <Message_ProgressSentry.hxx>
#include <Standard_Integer.hxx>

#include <Precision.hxx>

#include <math_BullardGenerator.hxx>

#include <set>
#include <map>
#include <memory>

#include <BRepMesh_MinStCut.hxx>

#include "Eigen/Core"
#include "Eigen/Sparse"

// #define MEASURE_PERFORMANCE

namespace
{

// The maximum positive double value.
const Standard_Real THE_MAX_VAL = std::numeric_limits<Standard_Real>::max();

// The 'small' (safe for inversion) 4D vector of doubles.
const BVH_Vec3d THE_MIN_VEC (1e-100, 1e-100, 1e-100);

// Relative weight of coherency component for optimization.
const Standard_Real COHERENCY_WEIGHT = 50.0;

// Indicates that ray does not hit any geometry.
const Standard_Integer INVALID_HIT = -1;

//! Maximum number of attempts for rejection sampling.
const int MAX_SAMPLING_ATTEMPTS = 10;

//! Samples value from array according to probabilities.
Standard_Integer sampleValue (Standard_Real theKsi, const std::vector<Standard_Real>& theCDF)
{
  return static_cast<Standard_Integer> (std::lower_bound (theCDF.begin(), theCDF.end(), theKsi) - theCDF.begin());
}

//! Utility class to manage coherence table.
class SparseCoherencyTable
{
public:

  //! Allocates square coherency table with theMaxSize * theMaxSize maximum number of elements.
  SparseCoherencyTable (Standard_Size theMaxSize)
    : myMaxSize (theMaxSize),
      mySize (0),
      myTable ((Standard_Integer)theMaxSize,
               (Standard_Integer)theMaxSize)
  {
    //
  }

  void Reserve (Standard_Size theNumElements)
  {
    myTable.reserve ((Standard_Integer)theNumElements);
  }

  //! Sets coherence value.
  void setCoherence (Standard_Size theI, Standard_Size theJ, Standard_Real theValue)
  {
    if (theI < theJ) std::swap (theI, theJ);
    myTable.coeffRef ((Standard_Integer)theI, (Standard_Integer)theJ) = static_cast<Standard_ShortReal> (theValue);
  }

  //! Returns coherence value.
  Standard_Real getCoherence (Standard_Size theI, Standard_Size theJ)
  {
    if (theI < theJ) std::swap (theI, theJ);
    return static_cast<Standard_Real> (myTable.coeff ((Standard_Integer)theI, (Standard_Integer)theJ));
  }

  //! Returns actual table size.
  Standard_Size Size()
  {
    return mySize;
  }

  //! Sets actual table size.
  void SetSize (Standard_Size theSize)
  {
    mySize = theSize;
  }

private:

  Standard_Size myMaxSize;
  Standard_Size mySize;

  Eigen::SparseMatrix<Standard_ShortReal> myTable;
};

//! Utility class to manage coherence table.
class CoherencyTable
{
public:

  //! Allocates square coherency table with theMaxSize * theMaxSize maximum number of elements.
  CoherencyTable (Standard_Size theMaxSize)
    : myMaxSize (theMaxSize),
      mySize (0),
      myTable (theMaxSize * theMaxSize, 0.f)
  {
    //
  }

  //! Sets coherence value.
  void setCoherence (Standard_Size theI, Standard_Size theJ, Standard_Real theValue)
  {
    if (theI < theJ) std::swap (theI, theJ);
    myTable[theI * myMaxSize + theJ] = static_cast<Standard_ShortReal> (theValue);
  }

  //! Returns coherence value.
  Standard_Real getCoherence (Standard_Size theI, Standard_Size theJ)
  {
    if (theI < theJ) std::swap (theI, theJ);
    return static_cast<Standard_Real> (myTable[theI * myMaxSize + theJ]);
  }

  //! Returns actual table size.
  Standard_Size Size()
  {
    return mySize;
  }

  //! Sets actual table size.
  void SetSize (Standard_Size theSize)
  {
    mySize = theSize;
  }

private:

  Standard_Size myMaxSize;
  Standard_Size mySize;

  std::vector<Standard_ShortReal> myTable;
};

//! Returns time of intersection of ray and triangle.
Standard_Real rayTriangleIntersection (const BVH_Vec3d& theOrigin,
                                       const BVH_Vec3d& theDirect,
                                       const BVH_Vec3d& thePoint0,
                                       const BVH_Vec3d& thePoint1,
                                       const BVH_Vec3d& thePoint2)
{
  const BVH_Vec3d  aEdge0  = thePoint1 - thePoint0;
  const BVH_Vec3d  aEdge1  = thePoint0 - thePoint2;

  BVH_Vec3d        aNormal = BVH_Vec3d::Cross (aEdge1, aEdge0);
  const Standard_Real aDot = aNormal.Dot (theDirect);
  if (fabs (aDot) < 1.0e-16)
  {
    return THE_MAX_VAL; // ray in the plane of triangle
  }

  const BVH_Vec3d aEdge2 = (thePoint0 - theOrigin) * (1.0 / aDot);

  const Standard_Real aTime = aNormal.Dot (aEdge2);
  if (aTime < 0.0)
  {
    return THE_MAX_VAL;
  }

  const BVH_Vec3d theInc = BVH_Vec3d::Cross (theDirect, aEdge2);

  Standard_Real U = theInc.Dot (aEdge1);
  Standard_Real V = theInc.Dot (aEdge0);

  if ((U >= 0.0) && (V >= 0.0) && (U + V <= 1.0))
  {
    return aTime;
  }

  return THE_MAX_VAL;
}

//! Checks if two boxes (AABBs) are overlapped.
bool overlapBoxes (const BVH_Vec3d& theBoxMin1,
                   const BVH_Vec3d& theBoxMax1,
                   const BVH_Vec3d& theBoxMin2,
                   const BVH_Vec3d& theBoxMax2) {

  return !(theBoxMin1.x() > theBoxMax2.x()
        || theBoxMax1.x() < theBoxMin2.x()
        || theBoxMin1.y() > theBoxMax2.y()
        || theBoxMax1.y() < theBoxMin2.y()
        || theBoxMin1.z() > theBoxMax2.z()
        || theBoxMax1.z() < theBoxMin2.z());
}

}

// =======================================================================
// function : BRepMesh_RestoreOrientationTool
// purpose  :
// =======================================================================
BRepMesh_RestoreOrientationTool::BRepMesh_RestoreOrientationTool (const bool theVisibilityOnly /* false */,
                                                                  const Standard_Integer theMinFaceRays /* 50 */,
                                                                  const Standard_Integer theMaxFaceRays /* 2000 */)
  : MinFaceRays (theMinFaceRays),
    MaxFaceRays (theMaxFaceRays),
    myIsDone (false),
    myVisibilityOnly (theVisibilityOnly)
{
  //
}

// =======================================================================
// function : BRepMesh_RestoreOrientationTool
// purpose  :
// =======================================================================
BRepMesh_RestoreOrientationTool::BRepMesh_RestoreOrientationTool (const TopoDS_Shape& theShape,
                                                                  const bool theVisibilityOnly /* false */,
                                                                  const Standard_Integer theMinFaceRays /* 50 */,
                                                                  const Standard_Integer theMaxFaceRays /* 2000 */)
  : MinFaceRays (theMinFaceRays),
    MaxFaceRays (theMaxFaceRays),
    myIsDone (false),
    myVisibilityOnly (theVisibilityOnly)
{
  Init (theShape);
}

// =======================================================================
// function : Init
// purpose  :
// =======================================================================
void BRepMesh_RestoreOrientationTool::Init (const TopoDS_Shape& theShape)
{
  myShape = TopoDS_Shape();
  if (theShape.IsNull())
  {
    Standard_NullObject::Raise();
  }

  myFaceList.clear();

  for (TopExp_Explorer anIter (theShape, TopAbs_FACE); anIter.More(); anIter.Next())
  {
    myFaceList.push_back (static_cast<const TopoDS_Face&> (anIter.Current()));
  }

  myTriangulation.Elements.clear();
  myTriangulation.Vertices.clear();

  Standard_Integer aCurrentTriangle = 0;

  for (BRepMesh_FaceList::iterator anIter = myFaceList.begin(); anIter != myFaceList.end(); ++anIter)
  {
    Handle (BRepMesh_TriangulatedPatch) aNewPatch (new BRepMesh_TriangulatedPatch(*anIter));

    Standard_Integer aTriangleCount = extractTriangulation (*anIter, *aNewPatch);

    if (aTriangleCount == 0)
    {
      continue;
    }

    aCurrentTriangle += aTriangleCount;
    myFaceIdIntervals.push_back (aCurrentTriangle - 1);
    aNewPatch->PreprocessData();

    myPatches.push_back (aNewPatch);

    extractTriangulation (*anIter, myTriangulation);
  }
}

// =======================================================================
// function : extractTriangulation
// purpose  :
// =======================================================================
Standard_Integer BRepMesh_RestoreOrientationTool::extractTriangulation (const TopoDS_Face& theFace,
                                                                        BVH_Triangulation<Standard_Real, 3>& theTriangulation)
{
  TopLoc_Location aLocation;

  Handle (Poly_Triangulation) aPolyTriangulation = BRep_Tool::Triangulation (theFace, aLocation);

  if (aPolyTriangulation.IsNull())
  {
    return 0;
  }

  bool isMirrored = aLocation.Transformation().VectorialPart().Determinant() < 0;
  bool needToFlip = static_cast<bool> (isMirrored ^ (theFace.Orientation() == TopAbs_REVERSED));

  BRepAdaptor_Surface aFaceAdaptor (theFace, Standard_False);

  const Standard_Integer aVertOffset =
    static_cast<Standard_Integer> (theTriangulation.Vertices.size()) - 1;

  for (Standard_Integer aVertIdx = 1; aVertIdx <= aPolyTriangulation->NbNodes(); ++aVertIdx)
  {
    gp_Pnt aVertex = aPolyTriangulation->Nodes().Value (aVertIdx);

    aVertex.Transform (aLocation.Transformation());

    theTriangulation.Vertices.push_back (BVH_Vec3d (aVertex.X(),
                                                    aVertex.Y(),
                                                    aVertex.Z()));
  }

  for (Standard_Integer aTriIdx = 1; aTriIdx <= aPolyTriangulation->NbTriangles(); ++aTriIdx)
  {
    Standard_Integer aVertex1;
    Standard_Integer aVertex2;
    Standard_Integer aVertex3;

    if (!needToFlip)
    {
      aPolyTriangulation->Triangles().Value (aTriIdx).Get (aVertex1,
                                                           aVertex2,
                                                           aVertex3);
    }
    else
    {
      aPolyTriangulation->Triangles().Value (aTriIdx).Get (aVertex3,
                                                           aVertex2,
                                                           aVertex1);
    }

    theTriangulation.Elements.push_back (BVH_Vec4i (aVertex1 + aVertOffset,
                                                    aVertex2 + aVertOffset,
                                                    aVertex3 + aVertOffset,
                                                    1));
  }

  theTriangulation.MarkDirty(); // triangulation needs BVH rebuilding

  return aPolyTriangulation->NbTriangles();
}

// =======================================================================
// function : computeVisibility
// purpose  :
// =======================================================================
void BRepMesh_RestoreOrientationTool::computeVisibility (BVH_Triangulation<Standard_Real, 3>& theTriangulation,
                                                         Handle (BRepMesh_TriangulatedPatch)& thePatch,
                                                         Standard_Integer theRaysNb)
{
  // deterministic sequence
  math_BullardGenerator anRNG (0);

  Standard_Real aFrontVisibility = 0.0;
  Standard_Real aBackVisibility = 0.0;

  Standard_Real aFrontDistance = 0.0;
  Standard_Real aBackDistance = 0.0;

  for (Standard_Integer anIdx = 0; anIdx < theRaysNb; ++anIdx)
  {
    // select triangle with probability proportional to triangle area
    Standard_Integer aTrgIdx = sampleValue (anRNG.NextReal(), thePatch->TriangleCDF());

    const BVH_Vec4i& aTriangle = thePatch->Elements[aTrgIdx];

    // sample point in triangle uniformly
    Standard_Real aKsi, aPsi;
    for (int i = 0; i < MAX_SAMPLING_ATTEMPTS; ++i)
    {
      aKsi = anRNG.NextReal();
      aPsi = anRNG.NextReal();

      if (aKsi + aPsi > 1.0)
      {
        break;
      }
    }

    const BVH_Vec3d& aPoint0 = thePatch->Vertices[aTriangle.x()];
    const BVH_Vec3d& aPoint1 = thePatch->Vertices[aTriangle.y()];
    const BVH_Vec3d& aPoint2 = thePatch->Vertices[aTriangle.z()];

    BVH_Vec3d aPoint (aPoint0 * (1 - aKsi - aPsi)
                    + aPoint1 * aKsi
                    + aPoint2 * aPsi);

    const BVH_Vec3d aEdge0  = aPoint1 - aPoint0;
    const BVH_Vec3d aEdge1  = aPoint0 - aPoint2;

    const BVH_Vec3d aNormal = (BVH_Vec3d::Cross (aEdge1, aEdge0).Normalized());

    // hemisphere direction
    BVH_Vec3d aDirection;
    Standard_Real aDot = 0.0;

    for (int i = 0; i < MAX_SAMPLING_ATTEMPTS; ++i)
    {
      aKsi = anRNG.NextReal();
      aPsi = anRNG.NextReal();

      Standard_Real anSqrtPsi = sqrt (aPsi);

      aDirection = BVH_Vec3d (anSqrtPsi * cos (2.f * M_PI * aKsi),
                              anSqrtPsi * sin (2.f * M_PI * aKsi),
                              sqrt (1.f - aPsi));

      aDot = aDirection.Dot (aNormal);

      if (std::abs (aDot) < 0.2)
      {
        break;
      }
    }

    if (aDot < 0.0)
    {
      aDirection = -aDirection;
    }

    const Standard_Real anEpsilon = std::max (1.0e-6, 1.0e-4 * theTriangulation.Box().Size().Modulus());

    Standard_Real aDist = 0.0;
    if (traceRay (aPoint + aNormal * anEpsilon, aDirection, theTriangulation, aDist) == INVALID_HIT)
    {
      aFrontVisibility += 1.0;
    }
    else
    {
      aFrontDistance += aDist;
    }

    if (traceRay (aPoint - aNormal * anEpsilon, -aDirection, theTriangulation, aDist) == INVALID_HIT)
    {
      aBackVisibility += 1.0;
    }
    else
    {
      aBackDistance += aDist;
    }
  }

  Standard_Real aInvRayNb = 1.0 / theRaysNb;
  thePatch->SetFrontVisibility (aFrontVisibility * aInvRayNb);
  thePatch->SetBackVisibility (aBackVisibility * aInvRayNb);
  thePatch->SetFrontDistance (aFrontDistance * aInvRayNb);
  thePatch->SetBackDistance (aBackDistance * aInvRayNb);
}

// =======================================================================
// function : traceRay
// purpose  :
// =======================================================================
Standard_Integer BRepMesh_RestoreOrientationTool::traceRay (const BVH_Vec3d& theOrigin,
                                                            const BVH_Vec3d& theDirect,
                                                            BVH_Triangulation<Standard_Real, 3>& theTriangulation,
                                                            Standard_Real& theDistance)
{
  Standard_Integer aStack[32];

  const NCollection_Handle<BVH_Tree<Standard_Real, 3> >& aBVH = theTriangulation.BVH();

  if (aBVH.IsNull())
  {
    return INVALID_HIT;
  }

  Standard_Integer aNode = 0;

  Standard_Real aMinDist = std::numeric_limits<Standard_Real>::max();
  Standard_Integer aHitTriangle = -1;

  Standard_Integer aHead = -1; // stack head position

  BVH_Vec3d anInvDirect = theDirect.cwiseAbs().cwiseMax (THE_MIN_VEC);
  anInvDirect = BVH_Vec3d (1.0 / anInvDirect.x(), 1.0 / anInvDirect.y(), 1.0 / anInvDirect.z());

  anInvDirect = BVH_Vec3d (_copysign (anInvDirect.x(), theDirect.x()),
                           _copysign (anInvDirect.y(), theDirect.y()),
                           _copysign (anInvDirect.z(), theDirect.z()));

  for (;;)
  {
    if (aBVH->IsOuter (aNode))
    {
      for (Standard_Integer anElemNb = aBVH->BegPrimitive (aNode); anElemNb <= aBVH->EndPrimitive (aNode); ++anElemNb)
      {
        const BVH_Vec4i& aTriangle = theTriangulation.Elements[anElemNb];
        const BVH_Vec3d& aVrt0 = theTriangulation.Vertices[aTriangle.x()];
        const BVH_Vec3d& aVrt1 = theTriangulation.Vertices[aTriangle.y()];
        const BVH_Vec3d& aVrt2 = theTriangulation.Vertices[aTriangle.z()];

        Standard_Real aDist = rayTriangleIntersection (theOrigin, theDirect, aVrt0, aVrt1, aVrt2);


        if (aDist < aMinDist)
        {
          aMinDist = aDist;
          aHitTriangle = anElemNb;
        }
      }

      if (aHead < 0)
      {
        break;
      }

      aNode = aStack[aHead--];
    }
    else
    {
      BVH_Vec3d aTime0 = (aBVH->MinPoint (aBVH->LeftChild (aNode)) - theOrigin) * anInvDirect;
      BVH_Vec3d aTime1 = (aBVH->MaxPoint (aBVH->LeftChild (aNode)) - theOrigin) * anInvDirect;
      
      BVH_Vec3d aTimeMax = aTime0.cwiseMax (aTime1);
      BVH_Vec3d aTimeMin = aTime0.cwiseMin (aTime1);

      aTime0 = (aBVH->MinPoint (aBVH->RightChild (aNode)) - theOrigin) * anInvDirect;
      aTime1 = (aBVH->MaxPoint (aBVH->RightChild (aNode)) - theOrigin) * anInvDirect;

      Standard_Real aTimeFinal = aTimeMax.minComp();
      Standard_Real aTimeStart = aTimeMin.maxComp();

      aTimeMax = aTime0.cwiseMax (aTime1);
      aTimeMin = aTime0.cwiseMin (aTime1);

      Standard_Real aTimeMin1 = (aTimeStart <= aTimeFinal) && (aTimeFinal >= 0.0)
                               ? aTimeStart : THE_MAX_VAL;

      aTimeFinal = aTimeMax.minComp();
      aTimeStart = aTimeMin.maxComp();

      Standard_Real aTimeMin2 = (aTimeStart <= aTimeFinal) && (aTimeFinal >= 0.f)
                               ? aTimeStart : THE_MAX_VAL;

      const bool aHitLft = (aTimeMin1 != THE_MAX_VAL);
      const bool aHitRgh = (aTimeMin2 != THE_MAX_VAL);

      if (aHitLft && aHitRgh)
      {
        int aLeft = aBVH->LeftChild (aNode);
        int aRight = aBVH->RightChild (aNode);

        aNode = (aTimeMin1 < aTimeMin2) ? aLeft : aRight;
        aStack[++aHead] = (aTimeMin2 <= aTimeMin1) ? aLeft : aRight;
      }
      else
      {
        if (aHitLft || aHitRgh)
        {
          aNode = aHitLft ? aBVH->LeftChild (aNode) : aBVH->RightChild (aNode);
        }
        else
        {
          if (aHead < 0)
          {
            break;
          }

          aNode = aStack[aHead--];
        }
      }
    }
  }

  theDistance = aMinDist;

  return aHitTriangle;
}

// =======================================================================
// function : edgeCoherence
// purpose  :
// =======================================================================
Standard_Real BRepMesh_RestoreOrientationTool::edgeCoherence (const BRepMesh_OrientedEdge& theEdge1,
                                                              const BRepMesh_TriangulatedPatch&  thePatch1,
                                                              const BRepMesh_OrientedEdge& theEdge2,
                                                              const BRepMesh_TriangulatedPatch&  thePatch2)
{
  BVH_Vec3d anE1V1;
  BVH_Vec3d anE1V2;
  BVH_Vec3d anE2V1;
  BVH_Vec3d anE2V2;

  thePatch1.GetEdgeVertices (theEdge1, anE1V1, anE1V2);
  thePatch2.GetEdgeVertices (theEdge2, anE2V1, anE2V2);

  BVH_Vec3d anEdge1 = anE1V2 - anE1V1;
  BVH_Vec3d anEdge2 = anE2V2 - anE2V1;

  Standard_Real aScalarProduct = anEdge1.Dot (anEdge2);

  Standard_Real aDist = BRepMesh_TriangulatedPatch::EdgeSet::EdgeToEdgeDistance (anE1V1, anE1V2, anE2V1, anE2V2);
  Standard_Real aSign = -_copysign (1.0, aScalarProduct);

  return aSign * sqrt (std::abs (aScalarProduct)) / (1.0 + aDist);
}

// =======================================================================
// function : computeCoherence
// purpose  :
// =======================================================================
Standard_Real BRepMesh_RestoreOrientationTool::computeCoherence (Handle (BRepMesh_TriangulatedPatch)& thePatch0,
                                                                 Handle (BRepMesh_TriangulatedPatch)& thePatch1)
{
  Standard_Real aTotalCoherence = 0.0;

  // Ensure that BVH already built before parallel section
  thePatch0->BoundaryEdgeSet().BVH();
  thePatch1->BoundaryEdgeSet().BVH();

  #pragma omp parallel for
  for (Standard_Integer anIdx = 0; anIdx < thePatch1->BoundaryEdgeSet().Size(); ++anIdx)
  {
    BRepMesh_OrientedEdge aCurrentEdge = thePatch1->BoundaryEdge (anIdx);
    BVH_Vec3d aVertex1;
    BVH_Vec3d aVertex2;
    thePatch1->GetEdgeVertices (aCurrentEdge, aVertex1, aVertex2);
    BRepMesh_OrientedEdge aClosestEdge = thePatch0->BoundaryEdge (
      thePatch0->BoundaryEdgeSet().CoherentEdge (aVertex1, aVertex2));

    #pragma omp critical
    {
      aTotalCoherence += edgeCoherence (aCurrentEdge, *thePatch1, aClosestEdge, *thePatch0);
    }
  }

  return aTotalCoherence;
}

// =======================================================================
// function : Perform
// purpose  :
// =======================================================================
void BRepMesh_RestoreOrientationTool::Perform (const Handle(Message_ProgressIndicator) thePI)
{
  Standard_Real aMaxArea = std::numeric_limits<Standard_Real>::epsilon();

#ifdef MEASURE_PERFORMANCE
  OSD_Timer aTimer;
  aTimer.Start();
#endif

  // Flipped flags for all patches.
  myFlipped.resize (myPatches.size(), 0);
  std::vector<char> aFlippedC (myPatches.size(), 0);

  Standard_Real aMaxCoherence = -std::numeric_limits<Standard_Real>::max();

  const Standard_Real anEpsilon = std::max (1.0e-4, 1.0e-2 * myTriangulation.Box().Size().Modulus());
  BVH_Vec3d anEpsilonVec (anEpsilon, anEpsilon, anEpsilon);

  std::unique_ptr<SparseCoherencyTable> aTable;

  if (!myVisibilityOnly)
  {
    aTable.reset (new SparseCoherencyTable (myPatches.size() * 3));
    aTable->Reserve (myPatches.size() * myPatches.size());
    aTable->SetSize (myPatches.size());

    if (!thePI.IsNull()) thePI->NewScope ( 80, "Coherence" );
    Message_ProgressSentry aPS (thePI, "Compute initial coherence", 0.0, (Standard_Real) (myPatches.size() - 1), 1.0);

    // Compute coherence
    for (Standard_Size i = 0; i < myPatches.size() - 1; ++i, aPS.Next())
    {
      Handle (BRepMesh_TriangulatedPatch)& aTriPatch1 = myPatches[i];

      for (Standard_Size j = i + 1; j < myPatches.size(); ++j)
      {
        Handle (BRepMesh_TriangulatedPatch)& aTriPatch2 = myPatches[j];

        if (overlapBoxes (aTriPatch1->Box().CornerMin() - anEpsilonVec,
                          aTriPatch1->Box().CornerMax() + anEpsilonVec,
                          aTriPatch2->Box().CornerMin() - anEpsilonVec,
                          aTriPatch2->Box().CornerMax() + anEpsilonVec))
        {
          Standard_Real aCoherence = computeCoherence (aTriPatch1, aTriPatch2) + computeCoherence (aTriPatch2, aTriPatch1);

          aMaxCoherence = std::max (aMaxCoherence, aCoherence);

          aTable->setCoherence (i, j, aCoherence);
        }
      }
    }

    if (!thePI.IsNull()) thePI->EndScope();

    std::vector<BRepMesh_SuperPatch> aMetaPatches;
    aMetaPatches.reserve (myPatches.size() * 3);

    // Prepare meta patches
    for (Standard_Size i = 0; i < myPatches.size(); ++i)
    {
      aMetaPatches.push_back (BRepMesh_SuperPatch (static_cast<Standard_Integer> (i)));
    }

    myPatchQueue.clear();
    myPatchQueue.reserve (myPatches.size());

    for (Standard_Size i = 0; i < myPatches.size() - 1; ++i)
    {
      for (Standard_Size j = i + 1; j < myPatches.size(); ++j)
      {
        Standard_Real aCoherence = aTable->getCoherence (i, j);
        if (aCoherence != 0.0)
        {
          myPatchQueue.push_back (BRepMesh_SuperPatchPair (&aMetaPatches[i], &aMetaPatches[j], aTable->getCoherence (i, j)));
        }
      }
    }

    std::set<Standard_Integer> aRemovedSuperPatches;

    // Greedy optimization of coherence
    while (!myPatchQueue.empty())
    {
      std::sort (myPatchQueue.begin(), myPatchQueue.end());

      const BRepMesh_SuperPatchPair& aBestPair = myPatchQueue.back();

      if (aBestPair.Coherence < 0)
      {
        // Do flip of second patch
        Standard_Integer anIndex = aBestPair.SecondPatch->Index;

        for (Standard_Size j = 0; j < aBestPair.SecondPatch->PatchIds.size(); ++j)
        {
          Standard_Integer aPatchId = aBestPair.SecondPatch->PatchIds[j];

          if (aPatchId == anIndex)
          {
            aFlippedC[aPatchId] = aFlippedC[aPatchId] == 0 ? 1 : 0;
          }

          myFlipped[aPatchId] = myFlipped[aPatchId] == 0 ? 1 : 0;
        }

        for (Standard_Size i = 0; i < aTable->Size(); ++i)
        {
          Standard_Real aCoherence = aTable->getCoherence (i, anIndex);

          aTable->setCoherence (i, anIndex, -aCoherence);
        }
      }

      aRemovedSuperPatches.insert (aBestPair.FirstPatch->Index);
      aRemovedSuperPatches.insert (aBestPair.SecondPatch->Index);

      aMetaPatches.push_back (BRepMesh_SuperPatch (*aBestPair.FirstPatch, *aBestPair.SecondPatch, (Standard_Integer)aTable->Size()));

      const BRepMesh_SuperPatch& aNewPatch = aMetaPatches.back();

      // Update coherence
      for (Standard_Size i = 0; i < aTable->Size(); ++i)
      {
        if (aRemovedSuperPatches.find ((Standard_Integer)i) == aRemovedSuperPatches.end())
        {
          Standard_Real aNewCoherence = 0.0;
          aNewCoherence += aTable->getCoherence (i, aBestPair.FirstPatch->Index);
          aNewCoherence += aTable->getCoherence (i, aBestPair.SecondPatch->Index);

          aTable->setCoherence (i, aNewPatch.Index, aNewCoherence);
        }
      }

      std::vector<BRepMesh_SuperPatchPair> aNewPatchQueue;
      aNewPatchQueue.reserve (myPatchQueue.size());

      // Update pairs
      for (Standard_Size i = 0; i < myPatchQueue.size(); ++i)
      {
        if (!myPatchQueue[i].HavePatch (aBestPair.FirstPatch->Index)
         && !myPatchQueue[i].HavePatch (aBestPair.SecondPatch->Index))
        {
          // Accept a pair
          aNewPatchQueue.push_back (myPatchQueue[i]);
        }
      }

      myPatchQueue.swap (aNewPatchQueue);

      // Add new pairs
      for (Standard_Size i = 0; i < aTable->Size(); ++i)
      {
        if (aRemovedSuperPatches.find ((Standard_Integer)i) == aRemovedSuperPatches.end())
        {
          Standard_Real aCoherence = aTable->getCoherence (i, aNewPatch.Index);

          if (aCoherence != 0.0)
          {
            myPatchQueue.push_back (BRepMesh_SuperPatchPair (&aNewPatch, &aMetaPatches[i], aCoherence));
          }
        }
      }

      aTable->SetSize (aTable->Size() + 1);
    }

#ifdef MEASURE_PERFORMANCE
    aTimer.Stop();
    std::cout << "Coherency time:  " << aTimer.ElapsedTime() << std::endl;
    aTimer.Reset();
    aTimer.Start();
#endif
  }

  // Compute max area
  for (Standard_Size i = 0; i < myPatches.size(); ++i)
  {
    aMaxArea = std::max (aMaxArea, myPatches[i]->TotalArea());
  }

  const Standard_Real anInvMaxArea = 1.0 / aMaxArea;

  const Standard_Integer aPatchesNb = static_cast<Standard_Integer> (myPatches.size());

  // Ensure that BVH already built before parallel section
  myTriangulation.BVH();

  if (!thePI.IsNull()) thePI->NewScope (myVisibilityOnly ? 100 : 20, "Visibility");
  Message_ProgressSentry aPS (thePI, "Compute visibility", 0, aPatchesNb, 1);

  // Compute visibility
  #pragma omp parallel for
  for (Standard_Integer i = 0; i < aPatchesNb; ++i)
  {
    Handle (BRepMesh_TriangulatedPatch)& aTriPatch = myPatches[i];
    Standard_Integer aRaysNb = Max (MinFaceRays, static_cast<Standard_Integer> (aTriPatch->TotalArea() * anInvMaxArea * MaxFaceRays));
    computeVisibility (myTriangulation, aTriPatch, aRaysNb);

    if (!myVisibilityOnly && myFlipped[i])
    {
      aTriPatch->FlipVisibility();
    }

    if (!thePI.IsNull())
    {
      #pragma omp critical
      {
        aPS.Next();
      }
    }
  }

  if (!thePI.IsNull()) thePI->EndScope();

#ifdef MEASURE_PERFORMANCE
  aTimer.Stop();
  std::cout << "Visibility time:  " << aTimer.ElapsedTime() << std::endl;
  aTimer.Reset();
  aTimer.Start();
#endif

  // Optimization
  Energy* aGraph = new Energy ((Standard_Integer)myPatches.size());

  std::vector<Energy::Var> aVariables (myPatches.size());

  for (Standard_Size i = 0; i < myPatches.size(); ++i)
  {
    Handle (BRepMesh_TriangulatedPatch)& aTriPatch = myPatches[i];
    aVariables[i] = aGraph->AddVariable();

    aGraph->AddTerm (aVariables[i], aTriPatch->BackVisibility() - aTriPatch->FrontVisibility(),
                                    aTriPatch->FrontVisibility() - aTriPatch->BackVisibility());
  }

  if (!myVisibilityOnly)
  {
    const Standard_Real aInvMaxCoherence = 1.0 / aMaxCoherence;

    for (Standard_Size i = 0; i < myPatches.size() - 1; ++i)
    {
      Handle (BRepMesh_TriangulatedPatch)& aTriPatch1 = myPatches[i];
      for (Standard_Size j = i + 1; j < myPatches.size(); ++j)
      {
        Handle (BRepMesh_TriangulatedPatch)& aTriPatch2 = myPatches[j];

        if (!overlapBoxes (aTriPatch1->Box().CornerMin() - anEpsilonVec,
                           aTriPatch1->Box().CornerMax() + anEpsilonVec,
                           aTriPatch2->Box().CornerMin() - anEpsilonVec,
                           aTriPatch2->Box().CornerMax() + anEpsilonVec))
        {
          continue;
        }

        bool isCoherenceFlipped = (myFlipped[i] ^ myFlipped[j] ^ aFlippedC[i] ^ aFlippedC[j]) != 0;
        Standard_Real aCoherence = aTable->getCoherence (i, j) * aInvMaxCoherence  * (isCoherenceFlipped ? -1.0 : 1.0);

        if (aCoherence > 0.0)
        {
          aCoherence *= COHERENCY_WEIGHT;

          // Prevent setting different labels for already coherent patches.
          aGraph->AddPairwise (aVariables[i], aVariables[j], 0, aCoherence, aCoherence, 0);
        }
      }
    }
  }

  try
  {
    aGraph->Minimize();
  }
  catch (std::exception& theError)
  {
    std::cout << theError.what() << std::endl;
  }

  for (Standard_Size i = 0; i < myPatches.size(); ++i)
  {
    if (aGraph->Label (aVariables[i]) == 1)
    {
      myFlipped[i] = myFlipped[i] == 0 ? 1 : 0;
    }
  }

#ifdef MEASURE_PERFORMANCE
  aTimer.Stop();
  std::cout << "Optimization time:  " << aTimer.ElapsedTime() << std::endl;
#endif

  aTable.reset();

  myIsDone = true;
}

// =======================================================================
// function : IsFlipped
// purpose  :
// =======================================================================
bool BRepMesh_RestoreOrientationTool::IsFlipped (const TopoDS_Face theFace) const
{
  if (!myIsDone)
  {
    return false;
  }

  for (Standard_Size i = 0; i < myPatches.size(); ++i)
  {
    TopoDS_Face aFace = myPatches[i]->Face();
    if (theFace == aFace)
    {
      return myFlipped[i] != 0;
    }
  }

  return false;
}

// =======================================================================
// function : IsFlipped
// purpose  :
// =======================================================================
bool BRepMesh_RestoreOrientationTool::IsFlipped (const Standard_Integer theFaceIndex) const
{
  if (!myIsDone || theFaceIndex < 0 || theFaceIndex >= (Standard_Integer) myFlipped.size())
  {
    return false;
  }

  return myFlipped[theFaceIndex] != 0;
}

// =======================================================================
// function : Shape
// purpose  :
// =======================================================================
TopoDS_Shape BRepMesh_RestoreOrientationTool::Shape() const
{
  if (myIsDone && myShape.IsNull())
  {
    BRep_Builder aBuilder;
    TopoDS_Compound aCompound;
    aBuilder.MakeCompound (aCompound);
    for (Standard_Size i = 0; i < myPatches.size(); ++i)
    {
      TopoDS_Face aFace = myPatches[i]->Face();
      if (myFlipped[i] != 0)
      {
        aFace.Reverse();
      }
      aBuilder.Add (aCompound, aFace);
    }

    myShape = aCompound;
  }

  return myShape;
}
