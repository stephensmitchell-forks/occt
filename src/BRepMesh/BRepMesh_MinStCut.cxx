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

#include <stdio.h>
#include <string.h>
#include <stdexcept>

#include <BRepMesh_MinStCut.hxx>

GraphEval::GraphEval (const int theNodeNumMax)
  : m_nodeNum (0), m_nodeNumMax (theNodeNumMax)
{
  m_nodes = (Node*) malloc (m_nodeNumMax * sizeof (Node));
  m_edgeBlock  = new Storage<Edge> (EDGE_BLOCK_SIZE);
  m_flow = 0;
}

GraphEval::~GraphEval()
{
  free (m_nodes);
  delete m_edgeBlock;
}

GraphEval::NodeId GraphEval::AddNode (int theNum)
{
  NodeId i = m_nodeNum;
  m_nodeNum += theNum;

  if (m_nodeNum > m_nodeNumMax)
  {
    throw std::runtime_error ("Error: the number of nodes is exceeded!");
  }

  memset (m_nodes + i, 0, theNum * sizeof (NodeStruct));

  return i;
}

void GraphEval::AddEdge (NodeId theNodeFrom, NodeId theNodeTo, CapacityType theCap, CapacityType theRevCap)
{
  Edge* a, *a_rev;
  Node* from = m_nodes + theNodeFrom;
  Node* to = m_nodes + theNodeTo;

  a = m_edgeBlock->New (2);
  a_rev = a + 1;

  a->Sister = a_rev;
  a_rev->Sister = a;
  a->Next = from->First;
  from->First = a;
  a_rev->Next = ( (Node*) to)->First;
  to->First = a_rev;
  a->Head = to;
  a_rev->Head = from;
  a->ReverseCap = theCap;
  a_rev->ReverseCap = theRevCap;
}

void GraphEval::AddTWeights (NodeId theNode, CapacityType theCapSource, CapacityType theCapSink)
{
  register CapacityType delta = m_nodes[theNode].TrCap;

  if (delta > 0)
  {
    theCapSource += delta;
  }

  else
  {
    theCapSink   -= delta;
  }

  m_flow += (theCapSource < theCapSink) ? theCapSource : theCapSink;
  m_nodes[theNode].TrCap = theCapSource - theCapSink;
}

#define TERM ( (Edge *) 1 )
#define ORPH ( (Edge *) 2 )

#define INFINITE 1000000000

inline void GraphEval::setActive (Node* theNode)
{
  if (!theNode->Next)
  {
    if (m_queueLast[1])
    {
      m_queueLast[1]->Next = theNode;
    }

    else
    {
      m_queueFirst[1] = theNode;
    }

    m_queueLast[1] = theNode;
    theNode->Next = theNode;
  }
}

inline GraphEval::Node* GraphEval::nextActive()
{
  Node* aNode;

  while (1)
  {
    if (! (aNode = m_queueFirst[0]))
    {
      m_queueFirst[0] = aNode = m_queueFirst[1];
      m_queueLast[0]  = m_queueLast[1];
      m_queueFirst[1] = NULL;
      m_queueLast[1]  = NULL;

      if (!aNode)
      {
        return NULL;
      }
    }

    if (aNode->Next == aNode)
    {
      m_queueFirst[0] = m_queueLast[0] = NULL;
    }

    else
    {
      m_queueFirst[0] = aNode->Next;
    }

    aNode->Next = NULL;

    if (aNode->Parent)
    {
      return aNode;
    }
  }
}

void GraphEval::maxflowInit()
{
  Node* aNode;

  m_queueFirst[0] = m_queueLast[0] = NULL;
  m_queueFirst[1] = m_queueLast[1] = NULL;
  m_orphanFirst = NULL;

  for (aNode = m_nodes; aNode < m_nodes + m_nodeNum; aNode++)
  {
    aNode->Next = NULL;
    aNode->TS = 0;

    if (aNode->TrCap > 0)
    {
      aNode->IsSink = 0;
      aNode->Parent = TERM;
      setActive (aNode);
      aNode->TS = 0;
      aNode->DIST = 1;
    }

    else if (aNode->TrCap < 0)
    {
      aNode->IsSink = 1;
      aNode->Parent = TERM;
      setActive (aNode);
      aNode->TS = 0;
      aNode->DIST = 1;
    }

    else
    {
      aNode->Parent = NULL;
    }
  }

  m_time = 0;
}

void GraphEval::augment (Edge* theMiddleEdge)
{
  Node* aNode;
  Edge* anEdge;
  TCapacityType aBottleneck;
  NodePtr* aNodePtr;

  aBottleneck = theMiddleEdge->ReverseCap;

  for (aNode = theMiddleEdge->Sister->Head; ; aNode = anEdge->Head)
  {
    anEdge = aNode->Parent;

    if (anEdge == TERM)
    {
      break;
    }

    if (aBottleneck > anEdge->Sister->ReverseCap)
    {
      aBottleneck = anEdge->Sister->ReverseCap;
    }
  }

  if (aBottleneck > aNode->TrCap)
  {
    aBottleneck = aNode->TrCap;
  }

  for (aNode = theMiddleEdge->Head; ; aNode = anEdge->Head)
  {
    anEdge = aNode->Parent;

    if (anEdge == TERM)
    {
      break;
    }

    if (aBottleneck > anEdge->ReverseCap)
    {
      aBottleneck = anEdge->ReverseCap;
    }
  }

  if (aBottleneck > - aNode->TrCap)
  {
    aBottleneck = - aNode->TrCap;
  }


  theMiddleEdge->Sister->ReverseCap += aBottleneck;
  theMiddleEdge->ReverseCap -= aBottleneck;

  for (aNode = theMiddleEdge->Sister->Head; ; aNode = anEdge->Head)
  {
    anEdge = aNode->Parent;

    if (anEdge == TERM)
    {
      break;
    }

    anEdge->ReverseCap += aBottleneck;
    anEdge->Sister->ReverseCap -= aBottleneck;

    if (!anEdge->Sister->ReverseCap)
    {
      aNode->Parent = ORPH;
      aNodePtr = m_nodePtrBlock->New();
      aNodePtr->Ptr = aNode;
      aNodePtr->Next = m_orphanFirst;
      m_orphanFirst = aNodePtr;
    }
  }

  aNode->TrCap -= aBottleneck;

  if (!aNode->TrCap)
  {
    aNode->Parent = ORPH;
    aNodePtr = m_nodePtrBlock->New();
    aNodePtr->Ptr = aNode;
    aNodePtr->Next = m_orphanFirst;
    m_orphanFirst = aNodePtr;
  }

  for (aNode = theMiddleEdge->Head; ; aNode = anEdge->Head)
  {
    anEdge = aNode->Parent;

    if (anEdge == TERM)
    {
      break;
    }

    anEdge->Sister->ReverseCap += aBottleneck;
    anEdge->ReverseCap -= aBottleneck;

    if (!anEdge->ReverseCap)
    {
      aNode->Parent = ORPH;
      aNodePtr = m_nodePtrBlock->New();
      aNodePtr->Ptr = aNode;
      aNodePtr->Next = m_orphanFirst;
      m_orphanFirst = aNodePtr;
    }
  }

  aNode->TrCap += aBottleneck;

  if (!aNode->TrCap)
  {
    aNode->Parent = ORPH;
    aNodePtr = m_nodePtrBlock->New();
    aNodePtr->Ptr = aNode;
    aNodePtr->Next = m_orphanFirst;
    m_orphanFirst = aNodePtr;
  }


  m_flow += aBottleneck;
}

void GraphEval::processSourceOrphan (Node* theNode)
{
  Node* aNode;
  Edge* anEdge0, *anEdge0Min = NULL, *anEdge;
  NodePtr* aNodePtr;
  int d, d_min = INFINITE;

  for (anEdge0 = theNode->First; anEdge0; anEdge0 = anEdge0->Next)
    if (anEdge0->Sister->ReverseCap)
    {
      aNode = anEdge0->Head;

      if (!aNode->IsSink && (anEdge = aNode->Parent))
      {
        d = 0;

        while (1)
        {
          if (aNode->TS == m_time)
          {
            d += aNode->DIST;
            break;
          }

          anEdge = aNode->Parent;
          d ++;

          if (anEdge == TERM)
          {
            aNode->TS = m_time;
            aNode->DIST = 1;
            break;
          }

          if (anEdge == ORPH)
          {
            d = INFINITE;
            break;
          }

          aNode = anEdge->Head;
        }

        if (d < INFINITE)
        {
          if (d < d_min)
          {
            anEdge0Min = anEdge0;
            d_min = d;
          }

          for (aNode = anEdge0->Head; aNode->TS != m_time; aNode = aNode->Parent->Head)
          {
            aNode->TS = m_time;
            aNode->DIST = d --;
          }
        }
      }
    }

  if (theNode->Parent = anEdge0Min)
  {
    theNode->TS = m_time;
    theNode->DIST = d_min + 1;
  }

  else
  {
    theNode->TS = 0;

    for (anEdge0 = theNode->First; anEdge0; anEdge0 = anEdge0->Next)
    {
      aNode = anEdge0->Head;

      if (!aNode->IsSink && (anEdge = aNode->Parent))
      {
        if (anEdge0->Sister->ReverseCap)
        {
          setActive (aNode);
        }

        if (anEdge != TERM && anEdge != ORPH && anEdge->Head == theNode)
        {
          aNode->Parent = ORPH;
          aNodePtr = m_nodePtrBlock->New();
          aNodePtr->Ptr = aNode;

          if (m_orphanLast)
          {
            m_orphanLast->Next = aNodePtr;
          }

          else
          {
            m_orphanFirst        = aNodePtr;
          }

          m_orphanLast = aNodePtr;
          aNodePtr->Next = NULL;
        }
      }
    }
  }
}

void GraphEval::processSinkOrphan (Node* theNode)
{
  Node* aNode;
  Edge* anEdge0, *anEdge0Min = NULL, *anEdge;
  NodePtr* aNodePtr;
  int d, d_min = INFINITE;

  for (anEdge0 = theNode->First; anEdge0; anEdge0 = anEdge0->Next)
    if (anEdge0->ReverseCap)
    {
      aNode = anEdge0->Head;

      if (aNode->IsSink && (anEdge = aNode->Parent))
      {
        d = 0;

        while (1)
        {
          if (aNode->TS == m_time)
          {
            d += aNode->DIST;
            break;
          }

          anEdge = aNode->Parent;
          d ++;

          if (anEdge == TERM)
          {
            aNode->TS = m_time;
            aNode->DIST = 1;
            break;
          }

          if (anEdge == ORPH)
          {
            d = INFINITE;
            break;
          }

          aNode = anEdge->Head;
        }

        if (d < INFINITE)
        {
          if (d < d_min)
          {
            anEdge0Min = anEdge0;
            d_min = d;
          }

          for (aNode = anEdge0->Head; aNode->TS != m_time; aNode = aNode->Parent->Head)
          {
            aNode->TS = m_time;
            aNode->DIST = d --;
          }
        }
      }
    }

  if (theNode->Parent = anEdge0Min)
  {
    theNode->TS = m_time;
    theNode->DIST = d_min + 1;
  }

  else
  {
    theNode->TS = 0;

    for (anEdge0 = theNode->First; anEdge0; anEdge0 = anEdge0->Next)
    {
      aNode = anEdge0->Head;

      if (aNode->IsSink && (anEdge = aNode->Parent))
      {
        if (anEdge0->ReverseCap)
        {
          setActive (aNode);
        }

        if (anEdge != TERM && anEdge != ORPH && anEdge->Head == theNode)
        {
          aNode->Parent = ORPH;
          aNodePtr = m_nodePtrBlock->New();
          aNodePtr->Ptr = aNode;

          if (m_orphanLast)
          {
            m_orphanLast->Next = aNodePtr;
          }

          else
          {
            m_orphanFirst        = aNodePtr;
          }

          m_orphanLast = aNodePtr;
          aNodePtr->Next = NULL;
        }
      }
    }
  }
}

GraphEval::FlowType GraphEval::MaximumFlow()
{
  Node* i, *j, *current_node = NULL;
  Edge* a;
  NodePtr* np, *np_next;

  maxflowInit();
  m_nodePtrBlock = new DataBlock<NodePtr> (NODEPTR_BLOCK_SIZE);

  while (1)
  {
    if (i = current_node)
    {
      i->Next = NULL;

      if (!i->Parent)
      {
        i = NULL;
      }
    }

    if (!i)
    {
      if (! (i = nextActive()))
      {
        break;
      }
    }

    if (!i->IsSink)
    {
      for (a = i->First; a; a = a->Next)
        if (a->ReverseCap)
        {
          j = a->Head;

          if (!j->Parent)
          {
            j->IsSink = 0;
            j->Parent = a->Sister;
            j->TS = i->TS;
            j->DIST = i->DIST + 1;
            setActive (j);
          }

          else if (j->IsSink)
          {
            break;
          }

          else if (j->TS <= i->TS &&
                   j->DIST > i->DIST)
          {
            j->Parent = a->Sister;
            j->TS = i->TS;
            j->DIST = i->DIST + 1;
          }
        }
    }

    else
    {
      for (a = i->First; a; a = a->Next)
        if (a->Sister->ReverseCap)
        {
          j = a->Head;

          if (!j->Parent)
          {
            j->IsSink = 1;
            j->Parent = a->Sister;
            j->TS = i->TS;
            j->DIST = i->DIST + 1;
            setActive (j);
          }

          else if (!j->IsSink)
          {
            a = a->Sister;
            break;
          }

          else if (j->TS <= i->TS &&
                   j->DIST > i->DIST)
          {
            j->Parent = a->Sister;
            j->TS = i->TS;
            j->DIST = i->DIST + 1;
          }
        }
    }

    m_time ++;

    if (a)
    {
      i->Next = i;
      current_node = i;

      augment (a);

      while (np = m_orphanFirst)
      {
        np_next = np->Next;
        np->Next = NULL;

        while (np = m_orphanFirst)
        {
          m_orphanFirst = np->Next;
          i = np->Ptr;
          m_nodePtrBlock->Delete (np);

          if (!m_orphanFirst)
          {
            m_orphanLast = NULL;
          }

          if (i->IsSink)
          {
            processSinkOrphan (i);
          }

          else
          {
            processSourceOrphan (i);
          }
        }

        m_orphanFirst = np_next;
      }
    }

    else
    {
      current_node = NULL;
    }
  }

  delete m_nodePtrBlock;

  return m_flow;
}

GraphEval::TerminalType GraphEval::Label (NodeId theNode)
{
  if (m_nodes[theNode].Parent && ! (m_nodes[theNode].IsSink))
  {
    return SOURCE;
  }

  return SINK;
}
