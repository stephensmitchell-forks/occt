// Created on: 2014-06-18
// Created by: Oleg AGASHIN
// Copyright (c) 2011-2014 OPEN CASCADE SAS
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

#include <BRepMesh_WireInterferenceChecker.hxx>
#include <BRepMesh_GeomTool.hxx>
#include <Precision.hxx>

// TODO: remove this variable after implementation of LoopChecker2d.
static const Standard_Real MIN_LOOP_S = 2 * M_PI * 2.E-5;

#ifdef HAVE_TBB
//=======================================================================
//function : Constructor
//purpose  :
//=======================================================================
BRepMesh_WireInterferenceChecker::BRepMesh_WireInterferenceChecker(
  const BRepMesh::Array1OfSegmentsTree& theWires,
  BRepMesh_Status*                      theStatus,
  Standard_Mutex*                       theMutex)
: myWires (theWires),
  myStatus(theStatus),
  myMutex (theMutex)
{
}

//=======================================================================
//function : Checker's body
//purpose  : 
//=======================================================================
void BRepMesh_WireInterferenceChecker::operator ()(
  const tbb::blocked_range<Standard_Integer>& theWireRange) const
{
  for (Standard_Integer i = theWireRange.begin(); i != theWireRange.end(); ++i)
    this->operator ()(i);
}
#else
//=======================================================================
//function : Constructor
//purpose  : 
//=======================================================================
BRepMesh_WireInterferenceChecker::BRepMesh_WireInterferenceChecker(
  const BRepMesh::Array1OfSegmentsTree& theWires,
  BRepMesh_Status*                      theStatus)
: myWires (theWires),
  myStatus(theStatus)
{
}
#endif

bool ArePointsOnDifferentSides(const gp_Pnt2d & point1, const gp_Pnt2d & point2, const BRepMesh::Segment & segment1, const BRepMesh::Segment & segment2)
{
  return 0 > (((segment1.EndPnt.X()-segment1.StartPnt.X())*(point1.Y()-segment1.StartPnt.Y())-(point1.X()-segment1.StartPnt.X())*(segment1.EndPnt.Y()-segment1.StartPnt.Y())) * 
              ((segment2.EndPnt.X()-segment2.StartPnt.X())*(point2.Y()-segment2.StartPnt.Y())-(point2.X()-segment2.StartPnt.X())*(segment2.EndPnt.Y()-segment2.StartPnt.Y())));
}

const BRepMesh::Segment& GetSegmentFromArray(Standard_Integer idx, const BRepMesh::HArray1OfSegments& segsarray)
{
  if (idx < segsarray->Lower())
  {
    idx = segsarray->Upper();
  }
  if (idx > segsarray->Upper())
  {
    idx = segsarray->Lower();
  }
  return segsarray->Value(idx);
}

bool CheckSelftIntersectArea(Standard_Integer idx1, Standard_Integer idx2, const BRepMesh::HArray1OfSegments& segsarray, const gp_Pnt2d & intPnt)
{
  gp_XY aPrevVec;
  Standard_Real aSumS = 0.;
  const gp_XY& aRefPnt  = intPnt.Coord();
  for (Standard_Integer i = idx1; i < idx2; ++i)
  {
    const BRepMesh::Segment& aSeg = segsarray->Value(i);
    gp_XY aCurVec = aSeg.EndPnt - aRefPnt;

    if (aCurVec.SquareModulus() < gp::Resolution())
      continue;

    if (aPrevVec.SquareModulus() > gp::Resolution())
      aSumS += aPrevVec ^ aCurVec;

    aPrevVec = aCurVec;
  }

  return (Abs(aSumS / 2.) < MIN_LOOP_S);
}

//=======================================================================
//function : Checker's body
//purpose  : 
//=======================================================================
void BRepMesh_WireInterferenceChecker::operator ()(
  const Standard_Integer& theWireId) const
{
  if (*myStatus == BRepMesh_SelfIntersectingWire)
    return;

  const BRepMesh::SegmentsTree&      aWireSegTree1  = myWires(theWireId);
  const BRepMesh::HArray1OfSegments& aWireSegments1 = aWireSegTree1.first;
  const BRepMesh::HBndBox2dTree&     aWireBoxTree1  = aWireSegTree1.second;

  for (Standard_Integer aWireIt = theWireId; aWireIt <= myWires.Upper(); ++aWireIt)
  {
#ifdef HAVE_TBB
    // Break execution in case if flag was raised by another thread
    if (*myStatus == BRepMesh_SelfIntersectingWire)
      return;
#endif

    const Standard_Boolean isSelfIntCheck = (aWireIt == theWireId);
    const BRepMesh::SegmentsTree& aWireSegTree2 = 
      isSelfIntCheck ? aWireSegTree1 : myWires(aWireIt);

    const BRepMesh::HArray1OfSegments& aWireSegments2 = aWireSegTree2.first;
    const BRepMesh::HBndBox2dTree&     aWireBoxTree2  = aWireSegTree2.second;

    BRepMesh_WireChecker::BndBox2dTreeSelector aSelector (aWireSegments2->Size());

    Standard_Integer aSegmentId1 = aWireSegments1->Lower();
    for (; aSegmentId1 <= aWireSegments1->Upper(); ++aSegmentId1)
    {
#ifdef HAVE_TBB
      // Break execution in case if flag was raised by another thread
      if (*myStatus == BRepMesh_SelfIntersectingWire)
        return;
#endif

      aSelector.Clear();
      aSelector.SetBox(aWireBoxTree1->FindNode(aSegmentId1).Bnd());
      if (isSelfIntCheck)
        aSelector.SetSkippedIndex(aSegmentId1);

      if (aWireBoxTree2->Select(aSelector) == 0)
        continue;

      const BRepMesh::Segment& aSegment1 = aWireSegments1->Value(aSegmentId1);
      const BRepMesh::Array1OfInteger& aSelected = aSelector.Indices();
      const Standard_Integer aSelectedNb = aSelector.IndicesNb();
      for (Standard_Integer aBndIt = 0; aBndIt < aSelectedNb; ++aBndIt)
      {
#ifdef HAVE_TBB
        // Break execution in case if flag was raised by another thread
        if (*myStatus == BRepMesh_SelfIntersectingWire)
          return;
#endif

        const Standard_Integer aSegmentId2 = aSelected(aBndIt);
        const BRepMesh::Segment& aSegment2 = aWireSegments2->Value(aSegmentId2);

        gp_Pnt2d aIntPnt;
        BRepMesh_GeomTool::IntFlag aIntStatus = BRepMesh_GeomTool::IntSegSeg(
          aSegment1.StartPnt, aSegment1.EndPnt, 
          aSegment2.StartPnt, aSegment2.EndPnt,
          Standard_True, Standard_True,
          aIntPnt);

        if (aIntStatus == BRepMesh_GeomTool::Cross || aIntStatus == BRepMesh_GeomTool::Glued || aIntStatus == BRepMesh_GeomTool::PointOnSegment)
        {
          // TODO: remove this block after implementation of LoopChecker2d.
          if (isSelfIntCheck && CheckSelftIntersectArea(aSegmentId1, aSegmentId2, aWireSegments1, aIntPnt))
          {
            continue;
          }

#ifdef HAVE_TBB
          Standard_Mutex::Sentry aSentry(myMutex);
#endif
          *myStatus = BRepMesh_SelfIntersectingWire;
          return;
        }

        if (aIntStatus == BRepMesh_GeomTool::EndPointTouch && (!isSelfIntCheck || abs(aSegmentId1 - aSegmentId2) > 2))
        {
          bool segmentsAreEqual = false;
          gp_Pnt2d otherPnt1;
          gp_Pnt2d otherPnt2;
          const BRepMesh::Segment * refSegment1 = &aSegment2;
          const BRepMesh::Segment * refSegment2 = NULL;
          Standard_Boolean s1s2 = aSegment1.StartPnt.IsEqual(aSegment2.StartPnt, Precision::PConfusion());
          Standard_Boolean s1e2 = aSegment1.StartPnt.IsEqual(aSegment2.EndPnt, Precision::PConfusion());
          Standard_Boolean e1e2 = aSegment1.EndPnt.IsEqual(aSegment2.EndPnt, Precision::PConfusion());
          Standard_Boolean e1s2 = aSegment1.EndPnt.IsEqual(aSegment2.StartPnt, Precision::PConfusion());

          if ((s1s2 && e1e2) || (s1e2 && e1s2))
          {
            segmentsAreEqual = true;
          }
          else
          {
            otherPnt1 = (s1s2 || s1e2)? aSegment1.EndPnt : aSegment1.StartPnt;
            otherPnt2 = GetSegmentFromArray(aSegmentId1 + ((s1s2 || s1e2)? -1 : 1), aWireSegments1).StartPnt;
            refSegment2 = &(GetSegmentFromArray(aSegmentId2 + ((s1s2 || e1s2)? -1 : 1), aWireSegments2));
          }

          if (!segmentsAreEqual && NULL != refSegment1 && NULL != refSegment2 && ArePointsOnDifferentSides(otherPnt1, otherPnt2, *refSegment1, *refSegment2))
          {
            if (isSelfIntCheck && CheckSelftIntersectArea(aSegmentId1, aSegmentId2, aWireSegments1, aIntPnt))
            {
              continue;
            }

#ifdef HAVE_TBB
            Standard_Mutex::Sentry aSentry(myMutex);
#endif
            *myStatus = BRepMesh_SelfIntersectingWire;
            return;
          }
        }

      }
    }
  }
}
