// File:        Poly_Segment2dSewer.cxx
// Created:     01.07.2010
// Author:      Mikhail SAZONOV
// Copyright:   Open Cascade 2010

#include <Poly_Segment2dSewer.hxx>
#include <Poly_Segment2d.hxx>
#include <Bnd_B2d.hxx>
#include <NCollection_IncAllocator.hxx>
#include <Precision.hxx>


//=======================================================================
//function : Poly_Segment2dSewer
//purpose  : 
//=======================================================================

Poly_Segment2dSewer::Poly_Segment2dSewer
                   (const Standard_Real theTol,
                    const Handle_NCollection_IncAllocator& theAlloc)
: myTol         (theTol),
  mySqTol       (theTol * theTol),
  myNodes       (200, theAlloc),
  myLinkDataMap (1000, theAlloc),
  myCells       (Max (theTol, 0.1), theAlloc)
{
}

//=======================================================================
//function : AddSegment
//purpose  : 
//=======================================================================

Poly_MakeLoops::Link Poly_Segment2dSewer::AddSegment(const Poly_Segment2d& theSeg)
{
  Standard_Integer anId0, anId1;
  if (!findOrCreateNodes(theSeg.Point(0), theSeg.Point(1), anId0, anId1))
    return Poly_MakeLoops::Link();

  Poly_MakeLoops::Link aLink (anId0, anId1);
  if (!myLinkDataMap.IsBound(aLink)) {
    // add new link
    myLinkDataMap.Bind (aLink, LinkData(theSeg));
    myNodes(anId0-1).Links.Append(aLink);
    myNodes(anId1-1).Links.Append(aLink);
  }
  return aLink;
}

//=======================================================================
//function : AddCurve
//purpose  : 
//=======================================================================

void Poly_Segment2dSewer::AddCurve
                   (const Handle(Poly_PolyCurve2d)& theCurve,
                    Poly_MakeLoops::ListOfLink& theLinks,
                    const Standard_Boolean theAllowRecurse)
{
  if (!theCurve->NSegments())
    return;
  const Poly_Segment2d& aSeg = theCurve->SegmentIterator().Value();

  Standard_Integer anId0, anId1;
  if (!findOrCreateNodes(aSeg.Point(0), aSeg.Previous().Point(1), anId0, anId1))
    return;

  Poly_MakeLoops::Link aLink (anId0, anId1);
  if (!myLinkDataMap.IsBound(aLink)) {
    // add new link for the whole curve
    myLinkDataMap.Bind (aLink, LinkData(theCurve, 
      myNodes(anId0-1).Pnt, myNodes(anId1-1).Pnt));
    myNodes(anId0-1).Links.Append(aLink);
    myNodes(anId1-1).Links.Append(aLink);
    theLinks.Append (aLink);
    return;
  }

  if (!theAllowRecurse)
    return;

  // such link already exists, but probably this curve gives
  // another path between these two nodes (imagine two half circles),
  // so we divide the curve onto two parts and try adding them as links
  Standard_Integer aSplitNum = theCurve->NSegments() / 2;
  if (aSplitNum == 0)
    return;
  Poly_Seg2dIterator itSeg = theCurve->SegmentIterator();

  Handle(Poly_PolyCurve2d) aCurve1 =
    Poly_PolyCurve2d::Create(theCurve->Allocator());
  for (Standard_Integer i=0; itSeg.More() && i < aSplitNum; itSeg.Next(), i++)
    aCurve1->CreateSegment(itSeg.Value());
  AddCurve (aCurve1, theLinks, Standard_False);

  Handle(Poly_PolyCurve2d) aCurve2 =
    Poly_PolyCurve2d::Create(theCurve->Allocator());
  for (; itSeg.More(); itSeg.Next())
    aCurve2->CreateSegment(itSeg.Value());
  AddCurve (aCurve2, theLinks, Standard_False);
}


//=======================================================================
//function : findOrCreateNodes
//purpose  : 
//=======================================================================

Standard_Boolean Poly_Segment2dSewer::findOrCreateNodes
                   (const gp_XY& theP0, const gp_XY& theP1,
                    Standard_Integer& theId0, Standard_Integer& theId1)
{
  // find nodes with the same coordinates
  gp_XY aPMin0, aPMax0, aPMin1, aPMax1;
  theId0 = findNode (theP0, aPMin0, aPMax0);
  theId1 = findNode (theP1, aPMin1, aPMax1);

  // protect against degenerated links
  if (theId0 && theId0 == theId1)
    return Standard_False;
  if (!theId0 && !theId1 &&
      (theP1 - theP0).SquareModulus() < mySqTol)
    return Standard_False;

  // create new nodes
  if (!theId0)
    theId0 = createNode (theP0, aPMin0, aPMax0);
  if (!theId1)
    theId1 = createNode (theP1, aPMin1, aPMax1);
  return Standard_True;
}

//=======================================================================
//function : findNode
//purpose  : 
//=======================================================================

Standard_Integer Poly_Segment2dSewer::findNode (const gp_XY& thePnt,
                                                gp_XY& thePMin,
                                                gp_XY& thePMax)
{
  NodeInspector anInspector (myNodes, mySqTol, thePnt);
  Bnd_B2d aBox (thePnt, gp_XY(myTol, myTol));
  thePMin = aBox.CornerMin();
  thePMax = aBox.CornerMax();
  myCells.Inspect (thePMin, thePMax, anInspector);
  Standard_Integer anId = anInspector.GetResult();
  return anId;
}

//=======================================================================
//function : createNode
//purpose  : 
//=======================================================================

Standard_Integer Poly_Segment2dSewer::createNode (const gp_XY& thePnt,
                                                  const gp_XY& thePMin,
                                                  const gp_XY& thePMax)
{
  myNodes.Append(SewedNode()).Pnt = thePnt;
  Standard_Integer anId = myNodes.Size();
  myCells.Add (anId, thePMin, thePMax);
  return anId;
}

//=======================================================================
//function : LinkFreeNodes
//purpose  : 
//=======================================================================

void Poly_Segment2dSewer::LinkFreeNodes
                   (const Standard_Real    theTol,
                    Poly_MakeLoops2D&      theLoopMaker)
{
  Standard_Real aSqTol = theTol * theTol;
  // collect free nodes
  NCollection_CellFilter<NodeInspectorAndLinker> aCells (Max (theTol, 0.1));
  TColStd_PackedMapOfInteger anIds;
  Standard_Integer i;
  for (i=0; i < myNodes.Size(); i++) {
    const SewedNode& aNode = myNodes(i);
    if (aNode.Links.Extent() == 1) {
      Bnd_B2d aBox (aNode.Pnt, gp_XY(theTol, theTol));
      gp_XY aPMin = aBox.CornerMin();
      gp_XY aPMax = aBox.CornerMax();
      aCells.Add (i+1, aPMin, aPMax);
      anIds.Add (i+1);
    }
  }
  if (anIds.Extent() < 2)
    return;

  Poly_MakeLoops::ListOfLink aLinks;
  // for each free node, evaluate it using inspector, which
  // can create new links with other free nodes
  NodeInspectorAndLinker anInspector (myNodes, myLinkDataMap, aLinks, aSqTol);
  TColStd_MapIteratorOfPackedMapOfInteger itIds (anIds);
  for (; itIds.More(); itIds.Next()) {
    Standard_Integer aCurrId = itIds.Key();
    const SewedNode& aNode = myNodes(aCurrId-1);
    anInspector.SetCurrentNode (aCurrId);
    Bnd_B2d aBox (aNode.Pnt, gp_XY(theTol, theTol));
    gp_XY aPMin = aBox.CornerMin();
    gp_XY aPMax = aBox.CornerMax();
    aCells.Inspect (aPMin, aPMax, anInspector);
  }

  //key - degenerated node, value - node which replaces degenerated node
  NCollection_DataMap<Standard_Integer, Standard_Integer> aDegNodesMap;
  //analized nodes
  TColStd_PackedMapOfInteger aUsedNodes;

  // remove from the map the linked nodes
  Poly_MakeLoops::ListOfLink::Iterator itLnk(aLinks);
  for (; itLnk.More(); itLnk.Next()) {
    const Poly_MakeLoops::Link& aLink = itLnk.Value();
    aUsedNodes.Add(aLink.node1);
    aUsedNodes.Add(aLink.node2);
    theLoopMaker.AddLink(aLink);
  }

  // try to sew remaining free nodes with other non-free nodes
  for (itIds.Initialize(anIds); itIds.More(); itIds.Next()) {
    Standard_Integer aCurrId = itIds.Key();
    if (aUsedNodes.Contains(aCurrId))
      continue;
    const SewedNode& aCurrNode = myNodes(aCurrId-1);
    NodeInspector anInspector (myNodes, aSqTol, aCurrNode.Pnt);
    anInspector.SetExcludeId(aCurrId);
    Bnd_B2d aBox (aCurrNode.Pnt, gp_XY(theTol, theTol));
    myCells.Inspect (aBox.CornerMin(), aBox.CornerMax(), anInspector);
    Standard_Integer anId = anInspector.GetResult();
    if (aDegNodesMap.IsBound(anId)) {
      anId = aDegNodesMap.Find(anId);
    }
    aDegNodesMap.Bind(aCurrId, anId);
    aUsedNodes.Add(anId);
    if (anId > 0) {
      // merge the free node with the found one
      const Poly_MakeLoops::Link& anOldLink = aCurrNode.Links.First();
      Poly_MakeLoops::Link aNewLink = anOldLink;
      Standard_Integer anId1;
      if (aNewLink.node1 == aCurrId) {
        aNewLink.node1 = anId;
        anId1 = aNewLink.node2;
      }
      else {
        aNewLink.node2 = anId;
        anId1 = aNewLink.node1;
      }
      
      //find data linked with 'anOldLink',
      //link found data with 'aNewLink'
      if (aNewLink.node1 != aNewLink.node2 &&
          !myLinkDataMap.IsBound(aNewLink))
      {
        // add new link
        const Poly_Segment2dSewer::LinkData& aCurrLinkData =
          myLinkDataMap(anOldLink);
        if (!aCurrLinkData.Curve.IsNull())
          myLinkDataMap.Bind (aNewLink, LinkData(aCurrLinkData.Curve,
            myNodes(aNewLink.node1-1).Pnt, myNodes(aNewLink.node2-1).Pnt));
        else
          myLinkDataMap.Bind (aNewLink, LinkData(aCurrLinkData.Segment));

        //replace old link by new one
        theLoopMaker.ReplaceLink(anOldLink, aNewLink);
        //add new link to found node which replaces 'aCurrNode'
        myNodes(anId-1).Links.Append(aNewLink);
        //replace old link by new one for 'myNodes(anId1-1)'
        Poly_MakeLoops::ListOfLink::Iterator itLinks (myNodes(anId1-1).Links);
        for ( ; itLinks.More(); itLinks.Next()) {
          Poly_MakeLoops::Link& aLink = itLinks.ChangeValue();
          if (aLink.node1 == anOldLink.node1 && 
              aLink.node2 == anOldLink.node2)
          {
            aLink = aNewLink;
            break;
          }
        }
      }
    }
  }
}

//=======================================================================
//function : GetSegment
//purpose  : 
//=======================================================================

Standard_Boolean Poly_Segment2dSewer::GetLinkData
                   (const Poly_MakeLoops::Link& theLink,
                    Handle(Poly_PolyCurve2d)& theCurve,
                    Poly_Segment2d& theSeg) const
{
  if (!myLinkDataMap.IsBound(theLink))
    return Standard_False;
  theCurve = myLinkDataMap.Find(theLink).Curve;
  theSeg = myLinkDataMap.Find(theLink).Segment;
  // stitch end points of theSeg to nodes
  //ASSERT (theLink.node1 > 0 && theLink.node1 <= myNodes.Size() &&
  //        theLink.node2 > 0 && theLink.node2 <= myNodes.Size(),
  //                "Poly_Segment2dSewer::NodeInspector::Inspect");
  const SewedNode& aNode0 = myNodes(theLink.node1-1);
  const SewedNode& aNode1 = myNodes(theLink.node2-1);
  theSeg.SetPoint(0, aNode0.Pnt);
  theSeg.SetPoint(1, aNode1.Pnt);
  return Standard_True;
}

//=======================================================================
//function : GetFirstTangent
//purpose  : 
//=======================================================================

Standard_Boolean Poly_Segment2dSewer::GetFirstTangent
                   (const Poly_MakeLoops::Link& theLink,
                    gp_Dir2d& theDir) const
{
  if (myLinkDataMap.IsBound(theLink)) {
    theDir = myLinkDataMap.Find(theLink).FirstDir;
    return Standard_True;
  }
  return Standard_False;
}

//=======================================================================
//function : GetLastTangent
//purpose  : 
//=======================================================================

Standard_Boolean Poly_Segment2dSewer::GetLastTangent
                   (const Poly_MakeLoops::Link& theLink,
                    gp_Dir2d& theDir) const
{
  if (myLinkDataMap.IsBound(theLink)) {
    theDir = myLinkDataMap.Find(theLink).LastDir;
    return Standard_True;
  }
  return Standard_False;
}

//=======================================================================
//function : GetAdjacentLinks
//purpose  : 
//=======================================================================

const Poly_MakeLoops::ListOfLink&
                   Poly_Segment2dSewer::GetAdjacentLinks (int theNode) const
{
  //Poly_ASSERT (theNode > 0 && theNode <= myNodes.Size(),
  //                "Poly_Segment2dSewer::GetAdjacentLinks");
  return myNodes(theNode-1).Links;
}

//=======================================================================
//function : IsPositive2
//purpose  : Auxilary
//=======================================================================
inline Standard_Boolean IsPositive2(const Standard_Real theVal)
{
  return (theVal > Precision::Confusion()*Precision::Confusion());
}

//=======================================================================
//function : LinkData::LinkData
//purpose  : 
//=======================================================================

Poly_Segment2dSewer::LinkData::LinkData
                   (const Handle(Poly_PolyCurve2d)& theCurve,
                    const gp_XY&                    theNode1,
                    const gp_XY&                    theNode2)
  : Curve (theCurve)
{
  const Poly_Segment2d& aFirstSeg = theCurve->SegmentIterator().Value();
  gp_XY aDir;
  Standard_Boolean isNullLen;

  // find the first segment with non-null length
  const Poly_Segment2d* aSeg = & aFirstSeg;
  do {
    aDir = aSeg->Point(1) - aSeg->Point(0);
    isNullLen = !IsPositive2(aDir.SquareModulus());
    aSeg = & aSeg->Next();
  } while (isNullLen && aSeg != & aFirstSeg);

  aDir = aSeg->Previous().Point(1) - theNode1;
  if (!IsPositive2(aDir.SquareModulus()))
    aDir = theNode2 - theNode1;
  FirstDir = aDir;

  // find the last segment with non-null length
  aSeg = & aFirstSeg.Previous();
  do {
    aDir = aSeg->Point(1) - aSeg->Point(0);
    isNullLen = !IsPositive2(aDir.SquareModulus());
    aSeg = & aSeg->Previous();
  } while (isNullLen && aSeg != & aFirstSeg.Previous());

  aDir = theNode2 - aSeg->Next().Point(0);
  if (!IsPositive2(aDir.SquareModulus()))
    aDir = theNode2 - theNode1;
  LastDir = aDir;
}

//=======================================================================
//function : NodeInspector::Inspect
//purpose  : 
//=======================================================================

NCollection_CellFilter_Action Poly_Segment2dSewer::NodeInspector::Inspect
                   (const Target& theId)
{
  //Poly_ASSERT (theId > 0 && theId <= myNodes.Size(),
  //                "Poly_Segment2dSewer::NodeInspector::Inspect");
  if (theId != myExcludeId) {
    const Poly_Segment2dSewer::SewedNode& aNode = myNodes(theId-1);
    Standard_Real aSqDist = (aNode.Pnt - myPnt).SquareModulus();
    if (aSqDist < myMinSqDist) {
      myMinSqDist = aSqDist;
      myResId = theId;
    }
  }
  return CellFilter_Keep;
}

//=======================================================================
//function : NodeInspectorAndLinker
//purpose  : 
//=======================================================================

Poly_Segment2dSewer::NodeInspectorAndLinker::NodeInspectorAndLinker
        (NCollection_Vector<Poly_Segment2dSewer::SewedNode>& theNodes,
         NCollection_DataMap<Poly_MakeLoops::Link,
                             Poly_Segment2dSewer::LinkData>& theLinkDataMap,
         Poly_MakeLoops::ListOfLink&                         theOutLinks,
         const Standard_Real                                 theSqTol)
: myNodes       (theNodes),
  myLinkDataMap (theLinkDataMap),
  myOutLinks    (theOutLinks),
  mySqTol       (theSqTol),
  mySqMinDist   (mySqTol),
  myNodeId      (0),
  myNode        (NULL),
  myLinkData    (NULL),
  myLinkDir     (NULL),
  myIsOrigin    (Standard_False)
{
}

//=======================================================================
//function : NodeInspectorAndLinker::
//purpose  : 
//=======================================================================

void Poly_Segment2dSewer::NodeInspectorAndLinker::SetCurrentNode
                   (const Standard_Integer theId)
{
  //Poly_ASSERT (theId > 0 && theId <= myNodes.Size(),
  //                "Poly_Segment2dSewer::NodeInspectorAndLinker::Inspect");
  myNodeId = theId;
  myNode = & myNodes (myNodeId-1);
  myLinkData = NULL;
  myLinkDir = NULL;
  mySqMinDist = mySqTol;

  Poly_MakeLoops::Link aLink;
  Poly_MakeLoops::ListOfLink::Iterator itLink (myNode->Links);
  for (; itLink.More(); itLink.Next()) {
    aLink = itLink.Value();
    if (!myNewLinks.Contains(aLink))
      break;
  }
  if (!itLink.More())
    return;

  myLinkData = & myLinkDataMap(aLink);
  if (myNodeId == aLink.node1) {
    myLinkDir = & myLinkData->FirstDir.XY();
    myIsOrigin = Standard_True;
  }
  else {
    myLinkDir = & myLinkData->LastDir.XY();
    myIsOrigin = Standard_False;
  }
}

//=======================================================================
//function : NodeInspectorAndLinker::Inspect
//purpose  : 
//=======================================================================

NCollection_CellFilter_Action Poly_Segment2dSewer::NodeInspectorAndLinker::Inspect
                   (const Target& theId)
{
  if (theId == myNodeId)
    return CellFilter_Keep;
  if (!myLinkData || !myLinkDir)
    return CellFilter_Keep;

  //Poly_ASSERT (theId > 0 && theId <= myNodes.Size(),
  //                "Poly_Segment2dSewer::NodeInspectorAndLinker::Inspect");

  Poly_MakeLoops::Link aNewLink (myNodeId, theId);
  if (myLinkDataMap.IsBound(aNewLink))
    return CellFilter_Keep;

  Poly_Segment2dSewer::SewedNode& aTgtNode = myNodes(theId-1);

  gp_XY aVec = aTgtNode.Pnt - myNode->Pnt;
  Standard_Real aSqDist = aVec.SquareModulus();
  if (!IsPositive2(aSqDist) || aSqDist > mySqMinDist)
    return CellFilter_Keep;
  if (myIsOrigin)
    aVec.Reverse();

  Standard_Real aDist = sqrt(aSqDist);
  aVec /= aDist;

  // determine which link's continuation will be the link (myNodeId, theId),
  // for that select the link that has less angle with this link

  // the angle of aVec with myLink
  Standard_Real aProd1 = aVec.CrossMagnitude (*myLinkDir);
  Standard_Real aDot1 = aVec * (*myLinkDir);
  Standard_Real anAng1 = atan2(aProd1, aDot1);

  // find link of the target node
  Poly_MakeLoops::Link aTgtLink;
  Poly_MakeLoops::ListOfLink::Iterator itLink (aTgtNode.Links);
  for (; itLink.More(); itLink.Next()) {
    aTgtLink = itLink.Value();
    if (!myNewLinks.Contains(aTgtLink))
      break;
  }
  if (!itLink.More())
    return CellFilter_Keep;

  // determine its direction
  const gp_XY* aTgtDir;
  Standard_Boolean isTgtOrigin;
  const Poly_Segment2dSewer::LinkData* pTgtLinkData = & myLinkDataMap(aTgtLink);
  if (theId == aTgtLink.node1) {
    aTgtDir = & pTgtLinkData->FirstDir.XY();
    isTgtOrigin = Standard_True;
  }
  else {
    aTgtDir = & pTgtLinkData->LastDir.XY();
    isTgtOrigin = Standard_False;
  }
  if (! (myIsOrigin ^ isTgtOrigin))
    return CellFilter_Keep; // inconsistent orientations

  // the angle of aVec with target Link
  Standard_Real aProd2 = aVec.CrossMagnitude (*aTgtDir);
  Standard_Real aDot2 = aVec * (*aTgtDir);
  Standard_Real anAng2 = atan2(aProd2, aDot2);

  // get normal of the link with less cross product
  const Poly_Segment2dSewer::LinkData* pLinkData;
  Standard_Boolean isFirst;
  Standard_Real aMinAng;
  if (anAng1 < anAng2) {
    pLinkData = myLinkData;
    isFirst = myIsOrigin;
    aMinAng = anAng1;
  }
  else {
    pLinkData = pTgtLinkData;
    isFirst = isTgtOrigin;
    aMinAng = anAng2;
  }
  Standard_Real aMaxAllowedAng = M_PI/4.;
  if (aMinAng > aMaxAllowedAng)
    // angle is greater than critical
    return CellFilter_Keep;

  const Poly_Segment2d* pSegOfNorm;
  if (pLinkData->Curve.IsNull())
    pSegOfNorm = & pLinkData->Segment;
  else {
    pSegOfNorm = & pLinkData->Curve->SegmentIterator().Value();
    if (!isFirst)
      pSegOfNorm = & pSegOfNorm->Previous();
  }

  // create segment of the new link
  Poly_Segment2d aNewSeg;
  if (myIsOrigin) {
    aNewSeg = Poly_Segment2d (aTgtNode.Pnt, myNode->Pnt);
    aNewLink = Poly_MakeLoops::Link (theId, myNodeId);
  }
  else
    aNewSeg = Poly_Segment2d (myNode->Pnt, aTgtNode.Pnt);
  aNewSeg.CopyNormal (*pSegOfNorm);
  //gp_XYZ aNorm;
  //*pSegOfNorm->GetNormal(aNorm); 
  //aNewSeg.SetNormal(aNorm);

  // add new link
  myLinkDataMap.Bind (aNewLink, Poly_Segment2dSewer::LinkData(aNewSeg));
  myNode->Links.Append (aNewLink);
  aTgtNode.Links.Append (aNewLink);
  myNewLinks.Add (aNewLink);
  myOutLinks.Append (aNewLink);
  mySqMinDist = aSqDist;

  return CellFilter_Keep;
}

//=======================================================================
//function : OnAddLink
//purpose  : 
//=======================================================================

void Poly_Segment2dSewer::OnAddLink
                   (int , const Poly_MakeLoops::Link& ) const
{
}
