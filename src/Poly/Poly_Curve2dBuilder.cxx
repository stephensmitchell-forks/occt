// File:        Poly_Curve2dBuilder.cxx
// Created:     01.07.2010
// Author:      Mikhail SAZONOV
// Copyright:   Open Cascade 2010

#include <Poly_Curve2dBuilder.hxx>

//=======================================================================
//function : Poly_Curve2dBuilder
//purpose  : 
//=======================================================================

Poly_Curve2dBuilder::Poly_Curve2dBuilder
                   (const Standard_Real theTol,
                    const Standard_Real theGTol,
                    const Handle_NCollection_BaseAllocator& theAlloc)
: myTol (theTol),
  myGTol (theGTol),
  myIsTreatOpenCurves (Standard_False),
  myIsShellMode (Standard_False),
  myAlloc (theAlloc),
  myTempAlloc (new NCollection_IncAllocator(4000)),
  myHelper (theTol, myTempAlloc),
  myLoopMaker (Standard_False, & myHelper, myTempAlloc)
{
  if (theAlloc.IsNull())
    myAlloc = new NCollection_IncAllocator(4000);
}

//=======================================================================
//function : AddSegment
//purpose  : 
//=======================================================================

void Poly_Curve2dBuilder::AddSegment(const Poly_Segment2d& theSeg,
                                     const Standard_Boolean theAllowDelay)
{
  Poly_MakeLoops::Link aLink = myHelper.AddSegment(theSeg);
  if (aLink.node1 > 0 && aLink.node2 > 0 && aLink.node1 != aLink.node2)
    myLoopMaker.AddLink(aLink);
  else if (theAllowDelay)
    myDelayedSegList.Append (theSeg);
}

//=======================================================================
//function : AddCurve
//purpose  : 
//=======================================================================

void Poly_Curve2dBuilder::AddCurve(const Handle(Poly_PolyCurve2d)& theCurve,
                                   const Standard_Boolean theAllowDelay)
{
  Poly_MakeLoops::ListOfLink aLinks(myAlloc);
  myHelper.AddCurve(theCurve, aLinks);
  if (!aLinks.IsEmpty()) {
    Poly_MakeLoops::ListOfLink::Iterator itLinks (aLinks);
    for (; itLinks.More(); itLinks.Next())
      myLoopMaker.AddLink(itLinks.Value());
  }
  else if (theAllowDelay)
    myDelayedCrvList.Append (theCurve);
}

//=======================================================================
//function : SetTreatOpenCurves
//purpose  : Defines whether it is necessary to treat open curves
//=======================================================================
void Poly_Curve2dBuilder::SetTreatOpenCurves
                                  (Standard_Boolean theIsTreatOpenCurves)
{
    myIsTreatOpenCurves = theIsTreatOpenCurves;
}

//=======================================================================
//function : SetShellMode
//purpose  : Defines whether it is necessary to take into account
//           the links orientations while building chains
//=======================================================================
void Poly_Curve2dBuilder::SetShellMode(Standard_Boolean theIsShellMode)
{
    myIsShellMode = theIsShellMode;
}

//=======================================================================
//function : GetResult
//purpose  : 
//=======================================================================

const Handle(Poly_PolyCurve2d)& Poly_Curve2dBuilder::GetResult() const
{
  return myResult;
}

//=======================================================================
//function : sewFreeNodes
//purpose  : 
//=======================================================================

void Poly_Curve2dBuilder::sewFreeNodes ()
{
  if (myGTol <= myTol)
    return;

  myHelper.LinkFreeNodes (myGTol, myLoopMaker);
}

//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================

Standard_Integer Poly_Curve2dBuilder::Perform()
{
  Standard_Integer aStatus = 0;

  // Try to add delayed segments and curves. It can be the case that
  // second time a small link can be added, as its ends will coincide with
  // different already existing nodes.
  NCollection_List<Poly_Segment2d>::Iterator itDelSeg (myDelayedSegList);
  for (; itDelSeg.More(); itDelSeg.Next())
    AddSegment (itDelSeg.Value(), Standard_False);
  Poly_ListOfCurve2d::Iterator itDelCrv (myDelayedCrvList);
  for (; itDelCrv.More(); itDelCrv.Next())
    AddCurve (itDelCrv.Value(), Standard_False);

  // Perform sewing of left free nodes with greater tolerance
  sewFreeNodes();

  // find and remove inside links 
  // (a link that has at both ends more than one adjacent link)
  //removeInsideLinks();

  // invoke loop maker algorithm
  Standard_Integer aLMStatus = myLoopMaker.Perform();
  Standard_Integer aHasHangingLinks = aLMStatus & Poly_MakeLoops::RC_HangingLinks;
  Standard_Integer anIsLoopsDone = aLMStatus & Poly_MakeLoops::RC_LoopsDone;

  if (aHasHangingLinks) aStatus |= CB_HangingSegments;
  if ((myIsTreatOpenCurves == Standard_False || !aHasHangingLinks) &&
      !anIsLoopsDone)
  {
    aStatus |= CB_Empty;
    return aStatus;
  }

  // iterate on loops and make curves
  myResult = Poly_PolyCurve2d::Create
    (myAlloc, Poly_PolyCurve2d::CurveType_SetOfBoundaries);
  //set location to created curve
  myResult->SetLoc(myLoc);

  Standard_Integer i, n, nbCurv = 0;
  if (anIsLoopsDone) {
    Handle(Poly_PolyCurve2d) aCurve;
    n = myLoopMaker.GetNbLoops();
    for (i = 1; i <= n; i++) {
      aCurve = Poly_PolyCurve2d::Create(myAlloc);
      aStatus |= buildLoop(myLoopMaker.GetLoop(i), aCurve);
      if (!aCurve.IsNull())
      {
        aCurve->SetLoc(myLoc);
        myResult->AddChildCurve (aCurve);
        nbCurv++;
      }
    }
  }

  if (myIsTreatOpenCurves == Standard_True && aHasHangingLinks)
  {
    Poly_MakeLoops::ListOfLink aListOfLinks;
    myLoopMaker.GetHangingLinks( aListOfLinks );

    NCollection_List<Handle_Poly_PolyCurve2d> aCurves;
    buildChains(aListOfLinks, aCurves);
    if (aCurves.Extent() > 0)
    {
      NCollection_List<Handle_Poly_PolyCurve2d>::Iterator anIter(aCurves);
      for (; anIter.More(); anIter.Next()) {
        Handle_Poly_PolyCurve2d& aCurve = anIter.ChangeValue();
        aCurve->SetLoc(myLoc);
        myResult->AddChildCurve (aCurve);
        nbCurv++;
      }
    }
  }

  if (!nbCurv)
    aStatus |= CB_Empty;

  return aStatus;
}

//=======================================================================
//function : buildLoop
//purpose  : build one loop from the list of the links
//=======================================================================
Standard_Integer Poly_Curve2dBuilder::buildLoop
                  (const Poly_MakeLoops::ListOfLink& theLinks, Handle_Poly_PolyCurve2d& theCurve)
{
  Standard_Integer aStatus = 0;

  Poly_MakeLoops::Loop::Iterator itLinks (theLinks);
  for (; itLinks.More(); itLinks.Next()) {
    const Poly_MakeLoops::Link& aLink = itLinks.Value();
    Poly_Segment2d aLinkSeg;
    Handle(Poly_PolyCurve2d) aLinkCurve;
    if (!myHelper.GetLinkData(aLink, aLinkCurve, aLinkSeg)) {
      aStatus |= CB_HangingSegments;
      theCurve.Nullify();
      break;
    }
    if (aLinkCurve.IsNull())
      // the link is a simple segment
      theCurve->CreateSegment(aLinkSeg);
    else {
      // the link is an open curve, join its segments
      Poly_Seg2dIterator itSeg = aLinkCurve->SegmentIterator();
      Standard_Boolean isFirst = Standard_True;
      for (; itSeg.More(); itSeg.Next()) {
        Poly_Segment2d& aSeg = theCurve->CreateSegment(itSeg.Value());
        if (isFirst)
          aSeg.SetPoint (0, aLinkSeg.Point(0));
        isFirst = Standard_False;
      }
      const_cast<Poly_Segment2d&>
        (theCurve->SegmentIterator().ChangeValue().Previous())
        .SetPoint(1, aLinkSeg.Point(1));
    }
  }
  return aStatus;
}

//=======================================================================
//function : buildChains
//purpose  : build chains from the list of the hanging links
//=======================================================================
void Poly_Curve2dBuilder::buildChains(const Poly_MakeLoops::ListOfLink& theLinks,
                                      NCollection_List<Handle_Poly_PolyCurve2d>& theCurves)
{
  NCollection_Map<Poly_MakeLoops::Link> aProcessedLinks;
  Poly_MakeLoops::ListOfLink aCurveLinks;
  Poly_MakeLoops::Loop::Iterator anIterLinks (theLinks);

  for (; anIterLinks.More(); anIterLinks.Next()) {

    const Poly_MakeLoops::Link& aCurLink = anIterLinks.Value(); 
    if (aProcessedLinks.Contains(aCurLink) == Standard_True) {
      continue;
    }

    aCurveLinks.Append(aCurLink);
    aProcessedLinks.Add(aCurLink);
    Standard_Boolean isNextLinkFound = Standard_True;
    while (isNextLinkFound == Standard_True) {
      const Poly_MakeLoops::Link& aFirstLink = aCurveLinks.First();
      Standard_Integer aCurNode = 
        (aFirstLink.IsReversed()) ? aFirstLink.node2 : aFirstLink.node1;
      isNextLinkFound = Standard_False;
      const Poly_MakeLoops::ListOfLink& anAdjLinks = myHelper.GetAdjacentLinks(aCurNode);
      Poly_MakeLoops::Loop::Iterator anIterAdjLinks(anAdjLinks);
      for (; anIterAdjLinks.More(); anIterAdjLinks.Next())  {
        const Poly_MakeLoops::Link& anAdjLink = anIterAdjLinks.Value();
        if (aProcessedLinks.Contains(anAdjLink) == Standard_False) {
          Poly_MakeLoops::Link aLink(anAdjLink.node1, anAdjLink.node2);
          if (aCurNode == aLink.node2) {
            isNextLinkFound = Standard_True;
          } else if ((aCurNode == aLink.node1)&&(myIsShellMode == Standard_False)) {
            aLink.Reverse();
            isNextLinkFound = Standard_True;
          }
          if (isNextLinkFound == Standard_True) {
            aCurveLinks.Prepend(aLink);
            aProcessedLinks.Add(aLink);
            break;
          }
        }
      }
    }

    isNextLinkFound = Standard_True;
    while (isNextLinkFound == Standard_True)  {
      const Poly_MakeLoops::Link& aLastLink = aCurveLinks.Last();
      Standard_Integer aCurNode = 
        (aLastLink.IsReversed()) ? aLastLink.node1 : aLastLink.node2;
      isNextLinkFound = Standard_False;
      const Poly_MakeLoops::ListOfLink& anAdjLinks = myHelper.GetAdjacentLinks(aCurNode);
      Poly_MakeLoops::Loop::Iterator anIterAdjLinks(anAdjLinks);
      for (; anIterAdjLinks.More(); anIterAdjLinks.Next())  {
        const Poly_MakeLoops::Link& anAdjLink = anIterAdjLinks.Value();
        if (aProcessedLinks.Contains(anAdjLink) == Standard_False) {
          Poly_MakeLoops::Link aLink(anAdjLink.node1, anAdjLink.node2);
          if (aCurNode == aLink.node1) {
            isNextLinkFound = Standard_True;
          } else if ((aCurNode == aLink.node2)&&(myIsShellMode == Standard_False)) {
            aLink.Reverse();
            isNextLinkFound = Standard_True;
          }
          if (isNextLinkFound == Standard_True) {
            aCurveLinks.Append(aLink);
            aProcessedLinks.Add(aLink);
            break;
          }
        }
      }
    }

    Poly_MakeLoops::Loop::Iterator anIterC(aCurveLinks);
    if (anIterC.More()) {
      Handle_Poly_PolyCurve2d aCurve = Poly_PolyCurve2d::Create
                                       (myAlloc, Poly_PolyCurve2d::CurveType_Indefinite);
      for (; anIterC.More(); anIterC.Next()) {
        const Poly_MakeLoops::Link& aLink = anIterC.Value();
        Standard_Boolean isReversed(aLink.IsReversed() != Standard_False);
        Poly_Segment2d aLinkSeg;
        Handle(Poly_PolyCurve2d) aLinkCurve;        
        if (myHelper.GetLinkData(aLink, aLinkCurve, aLinkSeg)) {
          if (aLinkCurve.IsNull()) {
            // the link is a simple segment
            aCurve->CreateSegment(aLinkSeg, isReversed, Standard_True);
          } else {
            // the link is an open curve, join its segments
            Poly_Seg2dIterator::Sense aSense = Poly_Seg2dIterator::Forward;
            if (isReversed) {
              aSense = Poly_Seg2dIterator::Reverse;
            }
              
            Standard_Boolean isFirst = Standard_True;
            Poly_Seg2dIterator itSeg = aLinkCurve->SegmentIterator(aSense);
            for (; itSeg.More(); itSeg.Next()) {
              Poly_Segment2d& aSeg = 
                aCurve->CreateSegment(itSeg.Value(), isReversed, Standard_True);
              if (isFirst)
                aSeg.SetPoint (0, aLinkSeg.Point(isReversed));
              isFirst = Standard_False;
            }
            const_cast<Poly_Segment2d&>(aCurve->LastElement())
              .SetPoint(1, aLinkSeg.Point(!isReversed));
          }
        }
      }
      theCurves.Append(aCurve);
      aCurveLinks.Clear();
    }

  }
}

//=======================================================================
//function : removeInsideLinks
//purpose  : 
//=======================================================================

void Poly_Curve2dBuilder::removeInsideLinks()
{
  // iterate all nodes considering those having more than 2 links only
  Standard_Integer aNode;
  for (aNode=1; aNode < myHelper.GetNbNodes(); aNode++) {
    const Poly_MakeLoops::ListOfLink& aLinks = myHelper.GetAdjacentLinks(aNode);
    Standard_Integer nbLinks = countNotHangingLinks(aLinks);
    if (nbLinks < 3)
      continue;

    // select a link having a multi-connected node on the other end,
    // providing that other links have no such condition
    Poly_MakeLoops::Link aSelLink;
    Poly_MakeLoops::ListOfLink::Iterator itLinks(aLinks);
    for (; itLinks.More(); itLinks.Next()) {
      const Poly_MakeLoops::Link& aLink = itLinks.Value();
      Standard_Integer aNode1 = (aLink.node1 == aNode ?
                                 aLink.node2 : aLink.node1);
      const Poly_MakeLoops::ListOfLink& aLinks1 =
        myHelper.GetAdjacentLinks(aNode1);
      nbLinks = countNotHangingLinks(aLinks1);
      if (nbLinks > 2) {
        if (aSelLink.IsNull())
          // it is a first such link, select it
          aSelLink = aLink;
        else {
          // the second such link, so abandon procedure for this node
          aSelLink.Nullify();
          break;
        }
      }
    }
    if (aSelLink.IsNull())
      continue;

    // remove the link from the graph
    myLoopMaker.SetLinkOrientation(aSelLink,Poly_MakeLoops::LF_None);
  }
}

//=======================================================================
//function : countNotHangingLinks
//purpose  : 
//=======================================================================

Standard_Integer Poly_Curve2dBuilder::countNotHangingLinks
                   (const Poly_MakeLoops::ListOfLink& theLinks)
{
  Standard_Integer cnt = 0;
  Poly_MakeLoops::ListOfLink::Iterator itLinks(theLinks);
  for (; itLinks.More(); itLinks.Next()) {
    const Poly_MakeLoops::Link& aLink = itLinks.Value();
    const Poly_MakeLoops::ListOfLink& aLinks1 =
      myHelper.GetAdjacentLinks(aLink.node1);
    if (aLinks1.Extent() < 2)
      continue;
    const Poly_MakeLoops::ListOfLink& aLinks2 =
      myHelper.GetAdjacentLinks(aLink.node2);
    if (aLinks2.Extent() >= 2)
      cnt++;
  }
  return cnt;
}
