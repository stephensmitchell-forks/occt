// Created on: 2015-11-27
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

#pragma warning (disable: 4706)
#pragma warning (disable: 4701)
#pragma warning (disable: 4127)

#include <stdio.h>
#include <BRepMesh_MinStCut.hxx>

Graph::Graph (void (*theErrFun) (char*))
{
  myErrorFun = theErrFun;
  myNodeBlockFirst = NULL;
  myArcForBlockFirst = NULL;
  myArcREvBlockFirst = NULL;
  myFlow = 0;
}

Graph::~Graph()
{
  while (myNodeBlockFirst)
  {
    NodeBlock* aNext = myNodeBlockFirst->Next;
    delete myNodeBlockFirst;
    myNodeBlockFirst = aNext;
  }

  while (myArcForBlockFirst)
  {
    ArcForBlock* next = myArcForBlockFirst->Next;
    delete myArcForBlockFirst->Start;
    myArcForBlockFirst = next;
  }

  while (myArcREvBlockFirst)
  {
    ArcRevBlock* next = myArcREvBlockFirst->Next;
    delete myArcREvBlockFirst->Start;
    myArcREvBlockFirst = next;
  }
}

Graph::NodeId Graph::AddNode()
{
  Node* aNode;

  if (!myNodeBlockFirst || myNodeBlockFirst->Current+1 > &myNodeBlockFirst->Nodes[NODE_BLOCK_SIZE-1])
  {
    NodeBlock* next = myNodeBlockFirst;
    myNodeBlockFirst = (NodeBlock*) new NodeBlock;

    if (!myNodeBlockFirst) { if (myErrorFun) { (*myErrorFun) ("Not enough memory!"); } exit (1); }

    myNodeBlockFirst->Current = & (myNodeBlockFirst->Nodes[0]);
    myNodeBlockFirst->Next = next;
  }

  aNode = myNodeBlockFirst->Current ++;
  aNode->FirstOut = (ArcForward*) 0;
  aNode->FirstIn = (ArcReverse*) 0;

  aNode->TrCapacity = 0;

  return (NodeId) aNode;
}

void Graph::AddEdge (NodeId theFromNode, NodeId theToNode, CapacityType theCapacity, CapacityType theReverseCapacity)
{
  ArcForward* anArcFor;
  ArcReverse* anArcRev;

  if (!myArcForBlockFirst || myArcForBlockFirst->Current+1 > &myArcForBlockFirst->ArcsFor[ARC_BLOCK_SIZE])
  {
    ArcForBlock* aNext = myArcForBlockFirst;
    char* aPtr = new char[sizeof (ArcForBlock) +1];

    if (!aPtr) { if (myErrorFun) { (*myErrorFun) ("Not enough memory!"); } exit (1); }

    if ( (int) aPtr & 1) { myArcForBlockFirst = (ArcForBlock*) (aPtr + 1); }
    else              { myArcForBlockFirst = (ArcForBlock*) aPtr; }

    myArcForBlockFirst->Start = aPtr;
    myArcForBlockFirst->Current = & (myArcForBlockFirst->ArcsFor[0]);
    myArcForBlockFirst->Next = aNext;
  }

  if (!myArcREvBlockFirst || myArcREvBlockFirst->Current+1 > &myArcREvBlockFirst->ArcsRev[ARC_BLOCK_SIZE])
  {
    ArcRevBlock* aNext = myArcREvBlockFirst;
    char* aPter = new char[sizeof (ArcRevBlock) +1];

    if (!aPter) { if (myErrorFun) { (*myErrorFun) ("Not enough memory!"); } exit (1); }

    if ( (int) aPter & 1) { myArcREvBlockFirst = (ArcRevBlock*) (aPter + 1); }
    else              { myArcREvBlockFirst = (ArcRevBlock*) aPter; }

    myArcREvBlockFirst->Start = aPter;
    myArcREvBlockFirst->Current = & (myArcREvBlockFirst->ArcsRev[0]);
    myArcREvBlockFirst->Next = aNext;
  }

  anArcFor = myArcForBlockFirst->Current ++;
  anArcRev = myArcREvBlockFirst->Current ++;

  anArcRev->Sister = (ArcForward*) theFromNode;
  anArcFor->Shift  = (int) theToNode;
  anArcFor->ResidualCap = theCapacity;
  anArcFor->ReverseResidualCap = theReverseCapacity;

  ( (Node*) theFromNode)->FirstOut =
    (ArcForward*) ( (int) ( ( (Node*) theFromNode)->FirstOut) + 1);
  ( (Node*) theToNode)->FirstIn =
    (ArcReverse*) ( (int) ( ( (Node*) theToNode)->FirstIn) + 1);
}

void Graph::SetTWeights (NodeId theNode, CapacityType theCapacityToSource, CapacityType theCapacityToSink)
{
  myFlow += (theCapacityToSource < theCapacityToSink) ? theCapacityToSource : theCapacityToSink;
  ( (Node*) theNode)->TrCapacity = theCapacityToSource - theCapacityToSink;
}

void Graph::AddTWeights (NodeId theNode, CapacityType theCapacityToSource, CapacityType theCapacityToSink)
{
  register CapacityType delta = ( (Node*) theNode)->TrCapacity;

  if (delta > 0) { theCapacityToSource += delta; }
  else           { theCapacityToSink   -= delta; }

  myFlow += (theCapacityToSource < theCapacityToSink) ? theCapacityToSource : theCapacityToSink;
  ( (Node*) theNode)->TrCapacity = theCapacityToSource - theCapacityToSink;
}

void Graph::prepareGraph()
{
  Node* aNode;
  ArcForBlock* anArcBlockFor, *anArcBlockForFirst;
  ArcRevBlock* anArcBlockRev, *anArcBlockRevFirst, *anArcBlockREvScan;
  ArcForward* anArcFor;
  ArcReverse* anArcRev, *anArcRevSCan, anArcREvTmp;
  NodeBlock* aNodeBlock;
  bool aForwardFlag = false, aReverseFlag = false;
  int aK;

  if (!myArcREvBlockFirst)
  {
    NodeId aNodeFrom = AddNode(), aNodeTo = AddNode();
    AddEdge (aNodeFrom, aNodeTo, 1, 0);
  }

  /* FIRST STAGE */
  anArcREvTmp.Sister = NULL;

  for (anArcRev=myArcREvBlockFirst->Current; anArcRev<&myArcREvBlockFirst->ArcsRev[ARC_BLOCK_SIZE]; anArcRev++)
  {
    anArcRev->Sister = NULL;
  }

  anArcBlockFor = anArcBlockForFirst = myArcForBlockFirst;
  anArcBlockRev = anArcBlockRevFirst = anArcBlockREvScan = myArcREvBlockFirst;
  anArcFor = &anArcBlockFor->ArcsFor[0];
  anArcRev = anArcRevSCan = &anArcBlockRev->ArcsRev[0];

  for (aNodeBlock=myNodeBlockFirst; aNodeBlock; aNodeBlock=aNodeBlock->Next)
  {
    for (aNode=&aNodeBlock->Nodes[0]; aNode<aNodeBlock->Current; aNode++)
    {
      /* outgoing arcs */
      aK = (int) aNode->FirstOut;

      if (anArcFor + aK > &anArcBlockFor->ArcsFor[ARC_BLOCK_SIZE])
      {
        if (aK > ARC_BLOCK_SIZE) { if (myErrorFun) { (*myErrorFun) ("# of arcs per node exceeds block size!"); } exit (1); }

        if (aForwardFlag) { anArcBlockFor = NULL; }
        else          { anArcBlockFor = anArcBlockFor->Next; anArcBlockREvScan = anArcBlockREvScan->Next; }

        if (anArcBlockFor == NULL)
        {
          ArcForBlock* next = myArcForBlockFirst;
          char* ptr = new char[sizeof (ArcForBlock) +1];

          if (!ptr) { if (myErrorFun) { (*myErrorFun) ("Not enough memory!"); } exit (1); }

          if ( (int) ptr & 1) { myArcForBlockFirst = (ArcForBlock*) (ptr + 1); }
          else              { myArcForBlockFirst = (ArcForBlock*) ptr; }

          myArcForBlockFirst->Start = ptr;
          myArcForBlockFirst->Current = & (myArcForBlockFirst->ArcsFor[0]);
          myArcForBlockFirst->Next = next;
          anArcBlockFor = myArcForBlockFirst;
          aForwardFlag = true;
        }
        else { anArcRevSCan = &anArcBlockREvScan->ArcsRev[0]; }

        anArcFor = &anArcBlockFor->ArcsFor[0];
      }

      if (anArcBlockREvScan)
      {
        anArcRevSCan += aK;
        aNode->Parent = (ArcForward*) anArcRevSCan;
      }
      else { aNode->Parent = (ArcForward*) &anArcREvTmp; }

      anArcFor += aK;
      aNode->FirstOut = anArcFor;
      anArcBlockFor->last_node = aNode;

      /* incoming arcs */
      aK = (int) aNode->FirstIn;

      if (anArcRev + aK > &anArcBlockRev->ArcsRev[ARC_BLOCK_SIZE])
      {
        if (aK > ARC_BLOCK_SIZE) { if (myErrorFun) { (*myErrorFun) ("# of arcs per node exceeds block size!"); } exit (1); }

        if (aReverseFlag) { anArcBlockRev = NULL; }
        else          { anArcBlockRev = anArcBlockRev->Next; }

        if (anArcBlockRev == NULL)
        {
          ArcRevBlock* next = myArcREvBlockFirst;
          char* ptr = new char[sizeof (ArcRevBlock) +1];

          if (!ptr) { if (myErrorFun) { (*myErrorFun) ("Not enough memory!"); } exit (1); }

          if ( (int) ptr & 1) { myArcREvBlockFirst = (ArcRevBlock*) (ptr + 1); }
          else              { myArcREvBlockFirst = (ArcRevBlock*) ptr; }

          myArcREvBlockFirst->Start = ptr;
          myArcREvBlockFirst->Current = & (myArcREvBlockFirst->ArcsRev[0]);
          myArcREvBlockFirst->Next = next;
          anArcBlockRev = myArcREvBlockFirst;
          aReverseFlag = true;
        }

        anArcRev = &anArcBlockRev->ArcsRev[0];
      }

      anArcRev += aK;
      aNode->FirstIn = anArcRev;
      anArcBlockRev->last_node = aNode;
    }

    aNode->FirstOut = anArcFor;
    aNode->FirstIn  = anArcRev;
  }

  for (anArcBlockFor=myArcForBlockFirst; anArcBlockFor; anArcBlockFor=anArcBlockFor->Next)
  {
    anArcBlockFor->Current = anArcBlockFor->last_node->FirstOut;
  }

  for (anArcBlockFor=anArcBlockForFirst, anArcBlockRev=anArcBlockRevFirst;
       anArcBlockFor;
       anArcBlockFor=anArcBlockFor->Next, anArcBlockRev=anArcBlockRev->Next)
    for (anArcFor=&anArcBlockFor->ArcsFor[0], anArcRev=&anArcBlockRev->ArcsRev[0];
         anArcFor<&anArcBlockFor->ArcsFor[ARC_BLOCK_SIZE];
         anArcFor++, anArcRev++)
    {
      ArcForward* anArcForward;
      ArcReverse* anArcReverse;
      Node* aNodeFrom;
      int aShift = 0, aShiftNew;
      CapacityType aRCap, aReverseRCap, aRCapNew, aReverseRCapNew;

      if (! (aNodeFrom= (Node*) (anArcRev->Sister))) { continue; }

      anArcForward = anArcFor;
      anArcReverse = anArcRev;

      do
      {
        anArcReverse->Sister = NULL;

        aShiftNew = (int)(((char*) (anArcForward->Shift)) - (char*) aNodeFrom);
        aRCapNew = anArcForward->ResidualCap;
        aReverseRCapNew = anArcForward->ReverseResidualCap;

        if (aShift)
        {
          anArcForward->Shift = aShift;
          anArcForward->ResidualCap = aRCap;
          anArcForward->ReverseResidualCap = aReverseRCap;
        }

        aShift = aShiftNew;
        aRCap = aRCapNew;
        aReverseRCap = aReverseRCapNew;

        anArcForward = -- aNodeFrom->FirstOut;

        if ( (ArcReverse*) (aNodeFrom->Parent) != &anArcREvTmp)
        {
          aNodeFrom->Parent = (ArcForward*) ( ( (ArcReverse*) (aNodeFrom->Parent)) - 1);
          anArcReverse = (ArcReverse*) (aNodeFrom->Parent);
        }
      }
      while (aNodeFrom= (Node*) (anArcReverse->Sister));

      anArcForward->Shift = aShift;
      anArcForward->ResidualCap = aRCap;
      anArcForward->ReverseResidualCap = aReverseRCap;
    }

  for (anArcBlockFor=myArcForBlockFirst; anArcBlockFor; anArcBlockFor=anArcBlockFor->Next)
  {
    aNode = anArcBlockFor->last_node;
    anArcFor = aNode->FirstOut;
    anArcBlockFor->Current->Shift     = anArcFor->Shift;
    anArcBlockFor->Current->ResidualCap     = anArcFor->ResidualCap;
    anArcBlockFor->Current->ReverseResidualCap = anArcFor->ReverseResidualCap;
    anArcFor->Shift = (int) (anArcBlockFor->Current + 1);
    aNode->FirstOut = (ArcForward*) ( ( (char*) anArcFor) - 1);
  }

  /* THIRD STAGE */
  for (anArcBlockRev=myArcREvBlockFirst; anArcBlockRev; anArcBlockRev=anArcBlockRev->Next)
  {
    anArcBlockRev->Current = anArcBlockRev->last_node->FirstIn;
  }

  for (aNodeBlock = myNodeBlockFirst; aNodeBlock; aNodeBlock=aNodeBlock->Next)
    for (aNode = &aNodeBlock->Nodes[0]; aNode<aNodeBlock->Current; aNode++)
    {
      ArcForward* aForwardFirst, *aForwardLAst;

      aForwardFirst = aNode->FirstOut;

      if (IS_ODD (aForwardFirst))
      {
        aForwardFirst = (ArcForward*) ( ( (char*) aForwardFirst) + 1);
        aForwardLAst = (ArcForward*) ( (aForwardFirst ++)->Shift);
      }
      else { aForwardLAst = (aNode + 1)->FirstOut; }

      for (anArcFor=aForwardFirst; anArcFor<aForwardLAst; anArcFor++)
      {
        Node* to = NEIGHBOR_NODE (aNode, anArcFor->Shift);
        anArcRev = -- to->FirstIn;
        anArcRev->Sister = anArcFor;
      }
    }

  for (anArcBlockRev=myArcREvBlockFirst; anArcBlockRev; anArcBlockRev=anArcBlockRev->Next)
  {
    aNode = anArcBlockRev->last_node;
    anArcRev = aNode->FirstIn;
    anArcBlockRev->Current->Sister = anArcRev->Sister;
    anArcRev->Sister = (ArcForward*) (anArcBlockRev->Current + 1);
    aNode->FirstIn = (ArcReverse*) ( ( (char*) anArcRev) - 1);
  }
}

#define TERMINAL ( (ArcForward *) 1 )
#define ORPHAN   ( (ArcForward *) 2 )

#define INFINITE_D 1000000000

inline void Graph::setActive (Node* theNode)
{
  if (!theNode->Next)
  {
    if (myQueueLast[1]) { myQueueLast[1] -> Next = theNode; }
    else               { myQueueFirst[1]        = theNode; }

    myQueueLast[1] = theNode;
    theNode -> Next = theNode;
  }
}

inline Graph::Node* Graph::nextActive()
{
  Node* aNode;

  while (true)
  {
    if (! (aNode=myQueueFirst[0]))
    {
      myQueueFirst[0] = aNode = myQueueFirst[1];
      myQueueLast[0]  = myQueueLast[1];
      myQueueFirst[1] = NULL;
      myQueueLast[1]  = NULL;

      if (!aNode) { return NULL; }
    }

    if (aNode->Next == aNode) { myQueueFirst[0] = myQueueLast[0] = NULL; }
    else              { myQueueFirst[0] = aNode -> Next; }

    aNode -> Next = NULL;

    if (aNode->Parent) { return aNode; }
  }
}

void Graph::maxflowInit()
{
  Node* aNode;
  NodeBlock* aNodeBlock;

  myQueueFirst[0] = myQueueLast[0] = NULL;
  myQueueFirst[1] = myQueueLast[1] = NULL;
  myOrphanFirst = NULL;

  for (aNodeBlock=myNodeBlockFirst; aNodeBlock; aNodeBlock=aNodeBlock->Next)
  {
    for (aNode=&aNodeBlock->Nodes[0]; aNode<aNodeBlock->Current; aNode++)
    {
      aNode -> Next = NULL;
      aNode -> TimeStamp = 0;

      if (aNode->TrCapacity > 0)
      {
        aNode -> IsSink = 0;
        aNode -> Parent = TERMINAL;
        setActive (aNode);
        aNode -> TimeStamp = 0;
        aNode -> Distance = 1;
      }
      else if (aNode->TrCapacity < 0)
      {
        aNode -> IsSink = 1;
        aNode -> Parent = TERMINAL;
        setActive (aNode);
        aNode -> TimeStamp = 0;
        aNode -> Distance = 1;
      }
      else
      {
        aNode -> Parent = NULL;
      }
    }
  }

  myTime = 0;
}

void Graph::augment (Node* theSStart, Node* theTStart, CapacityType* theCapMiddle, CapacityType* theRevCapMiddle)
{
  Node* aNode;
  ArcForward* anArc;
  CapacityType aBottleneck;
  NodePtr* aNodePtr;

  aBottleneck = *theCapMiddle;

  for (aNode=theSStart; ;)
  {
    anArc = aNode -> Parent;

    if (anArc == TERMINAL) { break; }

    if (IS_ODD (anArc))
    {
      anArc = MAKE_EVEN (anArc);

      if (aBottleneck > anArc->ResidualCap) { aBottleneck = anArc -> ResidualCap; }

      aNode = NEIGHBOR_NODE_REV (aNode, anArc -> Shift);
    }
    else
    {
      if (aBottleneck > anArc->ReverseResidualCap) { aBottleneck = anArc -> ReverseResidualCap; }

      aNode = NEIGHBOR_NODE (aNode, anArc -> Shift);
    }
  }

  if (aBottleneck > aNode->TrCapacity) { aBottleneck = aNode -> TrCapacity; }

  for (aNode=theTStart; ;)
  {
    anArc = aNode -> Parent;

    if (anArc == TERMINAL) { break; }

    if (IS_ODD (anArc))
    {
      anArc = MAKE_EVEN (anArc);

      if (aBottleneck > anArc->ReverseResidualCap) { aBottleneck = anArc -> ReverseResidualCap; }

      aNode = NEIGHBOR_NODE_REV (aNode, anArc -> Shift);
    }
    else
    {
      if (aBottleneck > anArc->ResidualCap) { aBottleneck = anArc -> ResidualCap; }

      aNode = NEIGHBOR_NODE (aNode, anArc -> Shift);
    }
  }

  if (aBottleneck > - aNode->TrCapacity) { aBottleneck = - aNode -> TrCapacity; }


  *theRevCapMiddle += aBottleneck;
  *theCapMiddle -= aBottleneck;

  for (aNode=theSStart; ;)
  {
    anArc = aNode -> Parent;

    if (anArc == TERMINAL) { break; }

    if (IS_ODD (anArc))
    {
      anArc = MAKE_EVEN (anArc);
      anArc -> ReverseResidualCap += aBottleneck;
      anArc -> ResidualCap -= aBottleneck;

      if (!anArc->ResidualCap)
      {
        aNode -> Parent = ORPHAN;
        aNodePtr = myNodePtrBLock -> New();
        aNodePtr -> Ptr = aNode;
        aNodePtr -> Next = myOrphanFirst;
        myOrphanFirst = aNodePtr;
      }

      aNode = NEIGHBOR_NODE_REV (aNode, anArc -> Shift);
    }
    else
    {
      anArc -> ResidualCap += aBottleneck;
      anArc -> ReverseResidualCap -= aBottleneck;

      if (!anArc->ReverseResidualCap)
      {
        aNode -> Parent = ORPHAN;
        aNodePtr = myNodePtrBLock -> New();
        aNodePtr -> Ptr = aNode;
        aNodePtr -> Next = myOrphanFirst;
        myOrphanFirst = aNodePtr;
      }

      aNode = NEIGHBOR_NODE (aNode, anArc -> Shift);
    }
  }

  aNode -> TrCapacity -= aBottleneck;

  if (!aNode->TrCapacity)
  {
    aNode -> Parent = ORPHAN;
    aNodePtr = myNodePtrBLock -> New();
    aNodePtr -> Ptr = aNode;
    aNodePtr -> Next = myOrphanFirst;
    myOrphanFirst = aNodePtr;
  }

  for (aNode=theTStart; ;)
  {
    anArc = aNode -> Parent;

    if (anArc == TERMINAL) { break; }

    if (IS_ODD (anArc))
    {
      anArc = MAKE_EVEN (anArc);
      anArc -> ResidualCap += aBottleneck;
      anArc -> ReverseResidualCap -= aBottleneck;

      if (!anArc->ReverseResidualCap)
      {
        aNode -> Parent = ORPHAN;
        aNodePtr = myNodePtrBLock -> New();
        aNodePtr -> Ptr = aNode;
        aNodePtr -> Next = myOrphanFirst;
        myOrphanFirst = aNodePtr;
      }

      aNode = NEIGHBOR_NODE_REV (aNode, anArc -> Shift);
    }
    else
    {
      anArc -> ReverseResidualCap += aBottleneck;
      anArc -> ResidualCap -= aBottleneck;

      if (!anArc->ResidualCap)
      {
        aNode -> Parent = ORPHAN;
        aNodePtr = myNodePtrBLock -> New();
        aNodePtr -> Ptr = aNode;
        aNodePtr -> Next = myOrphanFirst;
        myOrphanFirst = aNodePtr;
      }

      aNode = NEIGHBOR_NODE (aNode, anArc -> Shift);
    }
  }

  aNode -> TrCapacity += aBottleneck;

  if (!aNode->TrCapacity)
  {
    aNode -> Parent = ORPHAN;
    aNodePtr = myNodePtrBLock -> New();
    aNodePtr -> Ptr = aNode;
    aNodePtr -> Next = myOrphanFirst;
    myOrphanFirst = aNodePtr;
  }

  myFlow += aBottleneck;
}

void Graph::processSourceOrphan (Node* theNode)
{
  Node* aNode;
  ArcForward* anArc0For, *anArc0ForFirst, *anArc0ForLast;
  ArcReverse* anArc0Rev, *anArc0RevFirst, *anArc0RevLast;
  ArcForward* anArc0Min = NULL, *anArc;
  NodePtr* aNodePtr;
  int aDist, aDistMin = INFINITE_D;

  anArc0ForFirst = theNode -> FirstOut;

  if (IS_ODD (anArc0ForFirst))
  {
    anArc0ForFirst = (ArcForward*) ( ( (char*) anArc0ForFirst) + 1);
    anArc0ForLast = (ArcForward*) ( (anArc0ForFirst ++) -> Shift);
  }
  else { anArc0ForLast = (theNode + 1) -> FirstOut; }

  anArc0RevFirst = theNode -> FirstIn;

  if (IS_ODD (anArc0RevFirst))
  {
    anArc0RevFirst = (ArcReverse*) ( ( (char*) anArc0RevFirst) + 1);
    anArc0RevLast  = (ArcReverse*) ( (anArc0RevFirst ++) -> Sister);
  }
  else { anArc0RevLast = (theNode + 1) -> FirstIn; }


  for (anArc0For=anArc0ForFirst; anArc0For<anArc0ForLast; anArc0For++)
    if (anArc0For->ReverseResidualCap)
    {
      aNode = NEIGHBOR_NODE (theNode, anArc0For -> Shift);

      if (!aNode->IsSink && (anArc=aNode->Parent))
      {
        /* checking the origin of j */
        aDist = 0;

        while (true)
        {
          if (aNode->TimeStamp == myTime)
          {
            aDist += aNode -> Distance;
            break;
          }

          anArc = aNode -> Parent;
          aDist ++;

          if (anArc==TERMINAL)
          {
            aNode -> TimeStamp = myTime;
            aNode -> Distance = 1;
            break;
          }

          if (anArc==ORPHAN) { aDist = INFINITE_D; break; }

          if (IS_ODD (anArc))
          { aNode = NEIGHBOR_NODE_REV (aNode, MAKE_EVEN (anArc) -> Shift); }
          else
          { aNode = NEIGHBOR_NODE (aNode, anArc -> Shift); }
        }

        if (aDist<INFINITE_D)
        {
          if (aDist<aDistMin)
          {
            anArc0Min = anArc0For;
            aDistMin = aDist;
          }

          for (aNode=NEIGHBOR_NODE (theNode, anArc0For->Shift); aNode->TimeStamp!=myTime;)
          {
            aNode -> TimeStamp = myTime;
            aNode -> Distance = aDist --;
            anArc = aNode->Parent;

            if (IS_ODD (anArc))
            { aNode = NEIGHBOR_NODE_REV (aNode, MAKE_EVEN (anArc) -> Shift); }
            else
            { aNode = NEIGHBOR_NODE (aNode, anArc -> Shift); }
          }
        }
      }
    }

  for (anArc0Rev=anArc0RevFirst; anArc0Rev<anArc0RevLast; anArc0Rev++)
  {
    anArc0For = anArc0Rev -> Sister;

    if (anArc0For->ResidualCap)
    {
      aNode = NEIGHBOR_NODE_REV (theNode, anArc0For -> Shift);

      if (!aNode->IsSink && (anArc=aNode->Parent))
      {
        aDist = 0;

        while (true)
        {
          if (aNode->TimeStamp == myTime)
          {
            aDist += aNode -> Distance;
            break;
          }

          anArc = aNode -> Parent;
          aDist ++;

          if (anArc==TERMINAL)
          {
            aNode -> TimeStamp = myTime;
            aNode -> Distance = 1;
            break;
          }

          if (anArc==ORPHAN) { aDist = INFINITE_D; break; }

          if (IS_ODD (anArc))
          { aNode = NEIGHBOR_NODE_REV (aNode, MAKE_EVEN (anArc) -> Shift); }
          else
          { aNode = NEIGHBOR_NODE (aNode, anArc -> Shift); }
        }

        if (aDist<INFINITE_D)
        {
          if (aDist<aDistMin)
          {
            anArc0Min = MAKE_ODD (anArc0For);
            aDistMin = aDist;
          }

          for (aNode=NEIGHBOR_NODE_REV (theNode,anArc0For->Shift); aNode->TimeStamp!=myTime;)
          {
            aNode -> TimeStamp = myTime;
            aNode -> Distance = aDist --;
            anArc = aNode->Parent;

            if (IS_ODD (anArc))
            { aNode = NEIGHBOR_NODE_REV (aNode, MAKE_EVEN (anArc) -> Shift); }
            else
            { aNode = NEIGHBOR_NODE (aNode, anArc -> Shift); }
          }
        }
      }
    }
  }

  if (theNode->Parent = anArc0Min)
  {
    theNode -> TimeStamp = myTime;
    theNode -> Distance = aDistMin + 1;
  }
  else
  {
    theNode -> TimeStamp = 0;

    for (anArc0For=anArc0ForFirst; anArc0For<anArc0ForLast; anArc0For++)
    {
      aNode = NEIGHBOR_NODE (theNode, anArc0For -> Shift);

      if (!aNode->IsSink && (anArc=aNode->Parent))
      {
        if (anArc0For->ReverseResidualCap) { setActive (aNode); }

        if (anArc!=TERMINAL && anArc!=ORPHAN && IS_ODD (anArc) && NEIGHBOR_NODE_REV (aNode, MAKE_EVEN (anArc)->Shift) ==theNode)
        {
          aNode -> Parent = ORPHAN;
          aNodePtr = myNodePtrBLock -> New();
          aNodePtr -> Ptr = aNode;

          if (myOrphanLast) { myOrphanLast -> Next = aNodePtr; }
          else             { myOrphanFirst        = aNodePtr; }

          myOrphanLast = aNodePtr;
          aNodePtr -> Next = NULL;
        }
      }
    }

    for (anArc0Rev=anArc0RevFirst; anArc0Rev<anArc0RevLast; anArc0Rev++)
    {
      anArc0For = anArc0Rev -> Sister;
      aNode = NEIGHBOR_NODE_REV (theNode, anArc0For -> Shift);

      if (!aNode->IsSink && (anArc=aNode->Parent))
      {
        if (anArc0For->ResidualCap) { setActive (aNode); }

        if (anArc!=TERMINAL && anArc!=ORPHAN && !IS_ODD (anArc) && NEIGHBOR_NODE (aNode, anArc->Shift) ==theNode)
        {
          aNode -> Parent = ORPHAN;
          aNodePtr = myNodePtrBLock -> New();
          aNodePtr -> Ptr = aNode;

          if (myOrphanLast) { myOrphanLast -> Next = aNodePtr; }
          else             { myOrphanFirst        = aNodePtr; }

          myOrphanLast = aNodePtr;
          aNodePtr -> Next = NULL;
        }
      }
    }
  }
}

void Graph::processSinkOrphan (Node* theNode)
{
  Node* aNode;
  ArcForward* anArc0For, *anArc0ForFirst, *anArc0ForLast;
  ArcReverse* anArc0Rev, *anArc0RevFirst, *anArc0RevLast;
  ArcForward* anArc0Min = NULL, *anArc;
  NodePtr* aNodePtr;
  int aDist, aDistMin = INFINITE_D;

  anArc0ForFirst = theNode -> FirstOut;

  if (IS_ODD (anArc0ForFirst))
  {
    anArc0ForFirst = (ArcForward*) ( ( (char*) anArc0ForFirst) + 1);
    anArc0ForLast = (ArcForward*) ( (anArc0ForFirst ++) -> Shift);
  }
  else { anArc0ForLast = (theNode + 1) -> FirstOut; }

  anArc0RevFirst = theNode -> FirstIn;

  if (IS_ODD (anArc0RevFirst))
  {
    anArc0RevFirst = (ArcReverse*) ( ( (char*) anArc0RevFirst) + 1);
    anArc0RevLast  = (ArcReverse*) ( (anArc0RevFirst ++) -> Sister);
  }
  else { anArc0RevLast = (theNode + 1) -> FirstIn; }


  for (anArc0For=anArc0ForFirst; anArc0For<anArc0ForLast; anArc0For++)
    if (anArc0For->ResidualCap)
    {
      aNode = NEIGHBOR_NODE (theNode, anArc0For -> Shift);

      if (aNode->IsSink && (anArc=aNode->Parent))
      {
        aDist = 0;

        while (true)
        {
          if (aNode->TimeStamp == myTime)
          {
            aDist += aNode -> Distance;
            break;
          }

          anArc = aNode -> Parent;
          aDist ++;

          if (anArc==TERMINAL)
          {
            aNode -> TimeStamp = myTime;
            aNode -> Distance = 1;
            break;
          }

          if (anArc==ORPHAN) { aDist = INFINITE_D; break; }

          if (IS_ODD (anArc))
          { aNode = NEIGHBOR_NODE_REV (aNode, MAKE_EVEN (anArc) -> Shift); }
          else
          { aNode = NEIGHBOR_NODE (aNode, anArc -> Shift); }
        }

        if (aDist<INFINITE_D)
        {
          if (aDist<aDistMin)
          {
            anArc0Min = anArc0For;
            aDistMin = aDist;
          }

          for (aNode=NEIGHBOR_NODE (theNode, anArc0For->Shift); aNode->TimeStamp!=myTime;)
          {
            aNode -> TimeStamp = myTime;
            aNode -> Distance = aDist --;
            anArc = aNode->Parent;

            if (IS_ODD (anArc))
            { aNode = NEIGHBOR_NODE_REV (aNode, MAKE_EVEN (anArc) -> Shift); }
            else
            { aNode = NEIGHBOR_NODE (aNode, anArc -> Shift); }
          }
        }
      }
    }

  for (anArc0Rev=anArc0RevFirst; anArc0Rev<anArc0RevLast; anArc0Rev++)
  {
    anArc0For = anArc0Rev -> Sister;

    if (anArc0For->ReverseResidualCap)
    {
      aNode = NEIGHBOR_NODE_REV (theNode, anArc0For -> Shift);

      if (aNode->IsSink && (anArc=aNode->Parent))
      {
        aDist = 0;

        while (true)
        {
          if (aNode->TimeStamp == myTime)
          {
            aDist += aNode -> Distance;
            break;
          }

          anArc = aNode -> Parent;
          aDist ++;

          if (anArc==TERMINAL)
          {
            aNode -> TimeStamp = myTime;
            aNode -> Distance = 1;
            break;
          }

          if (anArc==ORPHAN) { aDist = INFINITE_D; break; }

          if (IS_ODD (anArc))
          { aNode = NEIGHBOR_NODE_REV (aNode, MAKE_EVEN (anArc) -> Shift); }
          else
          { aNode = NEIGHBOR_NODE (aNode, anArc -> Shift); }
        }

        if (aDist<INFINITE_D)
        {
          if (aDist<aDistMin)
          {
            anArc0Min = MAKE_ODD (anArc0For);
            aDistMin = aDist;
          }

          for (aNode=NEIGHBOR_NODE_REV (theNode,anArc0For->Shift); aNode->TimeStamp!=myTime;)
          {
            aNode -> TimeStamp = myTime;
            aNode -> Distance = aDist --;
            anArc = aNode->Parent;

            if (IS_ODD (anArc))
            { aNode = NEIGHBOR_NODE_REV (aNode, MAKE_EVEN (anArc) -> Shift); }
            else
            { aNode = NEIGHBOR_NODE (aNode, anArc -> Shift); }
          }
        }
      }
    }
  }

  if (theNode->Parent = anArc0Min)
  {
    theNode -> TimeStamp = myTime;
    theNode -> Distance = aDistMin + 1;
  }
  else
  {
    theNode -> TimeStamp = 0;

    for (anArc0For=anArc0ForFirst; anArc0For<anArc0ForLast; anArc0For++)
    {
      aNode = NEIGHBOR_NODE (theNode, anArc0For -> Shift);

      if (aNode->IsSink && (anArc=aNode->Parent))
      {
        if (anArc0For->ResidualCap) { setActive (aNode); }

        if (anArc!=TERMINAL && anArc!=ORPHAN && IS_ODD (anArc) && NEIGHBOR_NODE_REV (aNode, MAKE_EVEN (anArc)->Shift) ==theNode)
        {
          aNode -> Parent = ORPHAN;
          aNodePtr = myNodePtrBLock -> New();
          aNodePtr -> Ptr = aNode;

          if (myOrphanLast) { myOrphanLast -> Next = aNodePtr; }
          else             { myOrphanFirst        = aNodePtr; }

          myOrphanLast = aNodePtr;
          aNodePtr -> Next = NULL;
        }
      }
    }

    for (anArc0Rev=anArc0RevFirst; anArc0Rev<anArc0RevLast; anArc0Rev++)
    {
      anArc0For = anArc0Rev -> Sister;
      aNode = NEIGHBOR_NODE_REV (theNode, anArc0For -> Shift);

      if (aNode->IsSink && (anArc=aNode->Parent))
      {
        if (anArc0For->ReverseResidualCap) { setActive (aNode); }

        if (anArc!=TERMINAL && anArc!=ORPHAN && !IS_ODD (anArc) && NEIGHBOR_NODE (aNode, anArc->Shift) ==theNode)
        {
          aNode -> Parent = ORPHAN;
          aNodePtr = myNodePtrBLock -> New();
          aNodePtr -> Ptr = aNode;

          if (myOrphanLast) { myOrphanLast -> Next = aNodePtr; }
          else             { myOrphanFirst        = aNodePtr; }

          myOrphanLast = aNodePtr;
          aNodePtr -> Next = NULL;
        }
      }
    }
  }
}

Graph::FlowType Graph::MaximumFlow()
{
  Node* aNode0, *aNode1, *aCurrentNode = NULL, *aSStart, *aTStart;
  CapacityType* aCapacityMiddle, *aRevCapacityMiddle;
  ArcForward* anArcFor, *anArcForFirst, *anArcForLast;
  ArcReverse* anArcRev, *anArcRevFirst, *anArcRevLAst;
  NodePtr* aNodePtr, *aNodePtrNext;

  prepareGraph();
  maxflowInit();
  myNodePtrBLock = new DBlock<NodePtr> (NODEPTR_BLOCK_SIZE, myErrorFun);

  while (true)
  {
    if (aNode0=aCurrentNode)
    {
      aNode0 -> Next = NULL;

      if (!aNode0->Parent) { aNode0 = NULL; }
    }

    if (!aNode0)
    {
      if (! (aNode0 = nextActive())) { break; }
    }

    aSStart = NULL;

    anArcForFirst = aNode0 -> FirstOut;

    if (IS_ODD (anArcForFirst))
    {
      anArcForFirst = (ArcForward*) ( ( (char*) anArcForFirst) + 1);
      anArcForLast = (ArcForward*) ( (anArcForFirst ++) -> Shift);
    }
    else { anArcForLast = (aNode0 + 1) -> FirstOut; }

    anArcRevFirst = aNode0 -> FirstIn;

    if (IS_ODD (anArcRevFirst))
    {
      anArcRevFirst = (ArcReverse*) ( ( (char*) anArcRevFirst) + 1);
      anArcRevLAst = (ArcReverse*) ( (anArcRevFirst ++) -> Sister);
    }
    else { anArcRevLAst = (aNode0 + 1) -> FirstIn; }

    if (!aNode0->IsSink)
    {
      for (anArcFor=anArcForFirst; anArcFor<anArcForLast; anArcFor++)
        if (anArcFor->ResidualCap)
        {
          aNode1 = NEIGHBOR_NODE (aNode0, anArcFor -> Shift);

          if (!aNode1->Parent)
          {
            aNode1 -> IsSink = 0;
            aNode1 -> Parent = MAKE_ODD (anArcFor);
            aNode1 -> TimeStamp = aNode0 -> TimeStamp;
            aNode1 -> Distance = aNode0 -> Distance + 1;
            setActive (aNode1);
          }
          else if (aNode1->IsSink)
          {
            aSStart = aNode0;
            aTStart = aNode1;
            aCapacityMiddle     = & (anArcFor -> ResidualCap);
            aRevCapacityMiddle = & (anArcFor -> ReverseResidualCap);
            break;
          }
          else if (aNode1->TimeStamp <= aNode0->TimeStamp &&
                   aNode1->Distance > aNode0->Distance)
          {
            aNode1 -> Parent = MAKE_ODD (anArcFor);
            aNode1 -> TimeStamp = aNode0 -> TimeStamp;
            aNode1 -> Distance = aNode0 -> Distance + 1;
          }
        }

      if (!aSStart)
        for (anArcRev=anArcRevFirst; anArcRev<anArcRevLAst; anArcRev++)
        {
          anArcFor = anArcRev -> Sister;

          if (anArcFor->ReverseResidualCap)
          {
            aNode1 = NEIGHBOR_NODE_REV (aNode0, anArcFor -> Shift);

            if (!aNode1->Parent)
            {
              aNode1 -> IsSink = 0;
              aNode1 -> Parent = anArcFor;
              aNode1 -> TimeStamp = aNode0 -> TimeStamp;
              aNode1 -> Distance = aNode0 -> Distance + 1;
              setActive (aNode1);
            }
            else if (aNode1->IsSink)
            {
              aSStart = aNode0;
              aTStart = aNode1;
              aCapacityMiddle     = & (anArcFor -> ReverseResidualCap);
              aRevCapacityMiddle = & (anArcFor -> ResidualCap);
              break;
            }
            else if (aNode1->TimeStamp <= aNode0->TimeStamp &&
                     aNode1->Distance > aNode0->Distance)
            {
              aNode1 -> Parent = anArcFor;
              aNode1 -> TimeStamp = aNode0 -> TimeStamp;
              aNode1 -> Distance = aNode0 -> Distance + 1;
            }
          }
        }
    }
    else
    {
      for (anArcFor=anArcForFirst; anArcFor<anArcForLast; anArcFor++)
        if (anArcFor->ReverseResidualCap)
        {
          aNode1 = NEIGHBOR_NODE (aNode0, anArcFor -> Shift);

          if (!aNode1->Parent)
          {
            aNode1 -> IsSink = 1;
            aNode1 -> Parent = MAKE_ODD (anArcFor);
            aNode1 -> TimeStamp = aNode0 -> TimeStamp;
            aNode1 -> Distance = aNode0 -> Distance + 1;
            setActive (aNode1);
          }
          else if (!aNode1->IsSink)
          {
            aSStart = aNode1;
            aTStart = aNode0;
            aCapacityMiddle     = & (anArcFor -> ReverseResidualCap);
            aRevCapacityMiddle = & (anArcFor -> ResidualCap);
            break;
          }
          else if (aNode1->TimeStamp <= aNode0->TimeStamp &&
                   aNode1->Distance > aNode0->Distance)
          {
            aNode1 -> Parent = MAKE_ODD (anArcFor);
            aNode1 -> TimeStamp = aNode0 -> TimeStamp;
            aNode1 -> Distance = aNode0 -> Distance + 1;
          }
        }

      for (anArcRev=anArcRevFirst; anArcRev<anArcRevLAst; anArcRev++)
      {
        anArcFor = anArcRev -> Sister;

        if (anArcFor->ResidualCap)
        {
          aNode1 = NEIGHBOR_NODE_REV (aNode0, anArcFor -> Shift);

          if (!aNode1->Parent)
          {
            aNode1 -> IsSink = 1;
            aNode1 -> Parent = anArcFor;
            aNode1 -> TimeStamp = aNode0 -> TimeStamp;
            aNode1 -> Distance = aNode0 -> Distance + 1;
            setActive (aNode1);
          }
          else if (!aNode1->IsSink)
          {
            aSStart = aNode1;
            aTStart = aNode0;
            aCapacityMiddle     = & (anArcFor -> ResidualCap);
            aRevCapacityMiddle = & (anArcFor -> ReverseResidualCap);
            break;
          }
          else if (aNode1->TimeStamp <= aNode0->TimeStamp &&
                   aNode1->Distance > aNode0->Distance)
          {
            aNode1 -> Parent = anArcFor;
            aNode1 -> TimeStamp = aNode0 -> TimeStamp;
            aNode1 -> Distance = aNode0 -> Distance + 1;
          }
        }
      }
    }

    myTime ++;

    if (aSStart)
    {
      aNode0 -> Next = aNode0;
      aCurrentNode = aNode0;

      augment (aSStart, aTStart, aCapacityMiddle, aRevCapacityMiddle);

      while (aNodePtr=myOrphanFirst)
      {
        aNodePtrNext = aNodePtr -> Next;
        aNodePtr -> Next = NULL;

        while (aNodePtr=myOrphanFirst)
        {
          myOrphanFirst = aNodePtr -> Next;
          aNode0 = aNodePtr -> Ptr;
          myNodePtrBLock -> Delete (aNodePtr);

          if (!myOrphanFirst) { myOrphanLast = NULL; }

          if (aNode0->IsSink) { processSinkOrphan (aNode0); }
          else            { processSourceOrphan (aNode0); }
        }

        myOrphanFirst = aNodePtrNext;
      }
    }
    else { aCurrentNode = NULL; }
  }

  delete myNodePtrBLock;

  return myFlow;
}

Graph::TerminalType Graph::Label (NodeId i)
{
  if ( ( (Node*) i)->Parent && ! ( (Node*) i)->IsSink) { return SOURCE; }

  return SINK;
}
