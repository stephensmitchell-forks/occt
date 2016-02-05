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

#ifndef __GRAPH_H__
#define __GRAPH_H__

#include <assert.h>
#include <BRepMesh_Block.hxx>

#define NODE_BLOCK_SIZE 512
#define ARC_BLOCK_SIZE 1024
#define NODEPTR_BLOCK_SIZE 128

class Graph
{
public:
  typedef enum
  {
    SOURCE  = 0,
    SINK  = 1
  } TerminalType;

  typedef double CapacityType;
  typedef double FlowType;
  typedef void* NodeId;

  Graph (void (*theErrFun) (char*) = NULL);

  ~Graph();

  NodeId AddNode();

  void AddEdge (NodeId theFromNode, NodeId theToNode, CapacityType theCapNode, CapacityType theRevCapacity);

  void SetTWeights (NodeId theNode, CapacityType theCapacityToSource, CapacityType theCapacityToSink);

  void AddTWeights (NodeId theNode, CapacityType theCapacityToSource, CapacityType theCapacityToSink);

  TerminalType Label (NodeId i);

  FlowType MaximumFlow();

private:

  struct ArcForwardSt;
  struct ArcReverseSt;

#define IS_ODD(a) ((int)(a) & 1)
#define MAKE_ODD(a)  ((ArcForward *) ((int)(a) | 1))
#define MAKE_EVEN(a) ((ArcForward *) ((int)(a) & (~1)))
#define MAKE_ODD_REV(a)  ((ArcReverse *) ((int)(a) | 1))
#define MAKE_EVEN_REV(a) ((ArcReverse *) ((int)(a) & (~1)))

  typedef struct NodeSt
  {
    ArcForwardSt*  FirstOut;
    ArcReverseSt*  FirstIn;

    ArcForwardSt*  Parent;

    NodeSt*      Next;

    int       TimeStamp;
    int       Distance;
    short     IsSink;

    CapacityType      TrCapacity;
  } Node;

#define NEIGHBOR_NODE(i, shift) ((Node *) ((char *)(i) + (shift)))
#define NEIGHBOR_NODE_REV(i, shift) ((Node *) ((char *)(i) - (shift)))
  typedef struct ArcForwardSt
  {
    int Shift;
    CapacityType ResidualCap;
    CapacityType ReverseResidualCap;
  } ArcForward;

  typedef struct ArcReverseSt
  {
    ArcForward*    Sister;
  } ArcReverse;

  typedef struct NodePtrSt
  {
    NodeSt*      Ptr;
    NodePtrSt*   Next;
  } NodePtr;

  typedef struct NodeBLockSt
  {
    Node*          Current;
    struct NodeBLockSt*  Next;
    Node          Nodes[NODE_BLOCK_SIZE];
  } NodeBlock;

#define last_node LAST_NODE.LAST_NODE

  typedef struct ArcForBlockSt
  {
    char*          Start;
    ArcForward*        Current;
    struct ArcForBlockSt*  Next;
    ArcForward        ArcsFor[ARC_BLOCK_SIZE];
    union
    {
      ArcForward      Dummy;
      Node*        LAST_NODE;
    }           LAST_NODE;
  } ArcForBlock;

  typedef struct ArcRevBlockSt
  {
    char*          Start;
    ArcReverse*        Current;
    struct ArcRevBlockSt*  Next;
    ArcReverse        ArcsRev[ARC_BLOCK_SIZE];
    union
    {
      ArcReverse      Dummy;
      Node*        LAST_NODE;
    }           LAST_NODE;
  } ArcRevBlock;

  NodeBlock*     myNodeBlockFirst;
  ArcForBlock*   myArcForBlockFirst;
  ArcRevBlock*   myArcREvBlockFirst;
  DBlock<NodePtr>*   myNodePtrBLock;

  void (*myErrorFun) (char*);

  FlowType      myFlow;

  Node*        myQueueFirst[2], *myQueueLast[2];
  NodePtr*       myOrphanFirst, *myOrphanLast;
  int         myTime;

  void setActive (Node* theNode);
  Node* nextActive();

  void prepareGraph();
  void maxflowInit();
  void augment (Node* theSStart, Node* theTStart, CapacityType* theCapMiddle, CapacityType* theRevCapMiddle);
  void processSourceOrphan (Node* theNode);
  void processSinkOrphan (Node* theNode);
};

class Energy : Graph
{
public:
  typedef NodeId Var;

  typedef CapacityType Value;
  typedef FlowType TotalValue;

  Energy (void (*theErrFun) (char*) = NULL);

  ~Energy();

  Var AddVariable();

  void AddConstant (Value E);

  void AddTerm (Var x,
                Value E0, Value E1);

  void AddPairwise (Var x, Var y,
                    Value E00, Value E01,
                    Value E10, Value E11);

  TotalValue Minimize();

  int Label (Var x);

private:

  TotalValue  myEConst;
  void (*myErrorFun) (char*);
};

inline Energy::Energy (void (*err_function) (char*)) : Graph (err_function)
{
  myEConst = 0;
  myErrorFun = err_function;
}

inline Energy::~Energy() {}

inline Energy::Var Energy::AddVariable() {  return AddNode(); }

inline void Energy::AddConstant (Value A) { myEConst += A; }

inline void Energy::AddTerm (Var x,
                             Value A, Value B)
{
  AddTWeights (x, B, A);
}

inline void Energy::AddPairwise (Var x, Var y,
                                 Value A, Value B,
                                 Value C, Value D)
{
  AddTWeights (x, D, A);
  B -= A;
  C -= D;

  assert (B + C >= 0);

  if (B < 0)
  {
    AddTWeights (x, 0, B);
    AddTWeights (y, 0, -B);
    AddEdge (x, y, 0, B+C);
  }
  else if (C < 0)
  {
    AddTWeights (x, 0, -C);
    AddTWeights (y, 0, C);
    AddEdge (x, y, B+C, 0);
  }
  else
  {
    AddEdge (x, y, B, C);
  }
}

inline Energy::TotalValue Energy::Minimize() { return myEConst + MaximumFlow(); }

inline int Energy::Label (Var x) { return (int) Graph::Label (x); }

#endif
