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

#ifndef _BRepMesh_MinStCut_HeaderFile
#define _BRepMesh_MinStCut_HeaderFile

#include <cassert>

#include <BRepMesh_Block.hxx>

#define EDGE_BLOCK_SIZE 1024
#define NODEPTR_BLOCK_SIZE 128

class GraphEval
{
public:
  typedef enum
  {
    SOURCE = 0,
    SINK = 1
  } TerminalType;

  typedef int NodeId;
  typedef double CapacityType;
  typedef double FlowType;
  typedef double TCapacityType;

  GraphEval (const int theNodeNumMax);

  ~GraphEval();

  NodeId AddNode (int theNum = 1);
  void AddEdge (NodeId theNodeFrom, NodeId theNodeTo, CapacityType theCap, CapacityType theRevCap);
  void AddTWeights (NodeId theNode, CapacityType theCapSource, CapacityType theCapSink);
  TerminalType Label (NodeId theNode);
  FlowType MaximumFlow();

private:
  struct EdgeStruct;

  typedef struct NodeStruct
  {
    EdgeStruct* First;

    EdgeStruct* Parent;
    NodeStruct* Next;
    int TS;
    int DIST;
    short IsSink;

    CapacityType TrCap;
  } Node;

  typedef struct EdgeStruct
  {
    NodeStruct* Head;
    EdgeStruct* Next;
    EdgeStruct* Sister;

    CapacityType ReverseCap;
  } Edge;

  typedef struct NodePtrStruct
  {
    NodeStruct* Ptr;
    NodePtrStruct* Next;
  } NodePtr;

  NodeStruct* m_nodes;
  int m_nodeNum, m_nodeNumMax;
  Storage<Edge>* m_edgeBlock;
  DataBlock<NodePtr>* m_nodePtrBlock;

  FlowType m_flow;

  Node* m_queueFirst[2], *m_queueLast[2];
  NodePtr* m_orphanFirst, *m_orphanLast;
  int m_time;

  void setActive (Node* theNode);
  Node* nextActive();

  void maxflowInit();
  void augment (Edge* theMiddleEdge);
  void processSourceOrphan (Node* theNode);
  void processSinkOrphan (Node* theNode);
};

class Energy : GraphEval
{
public:
  typedef NodeId Var;

  typedef CapacityType Value;
  typedef FlowType TotalValue;

  Energy (const int theMaxNodes);

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

};

inline Energy::Energy (const int theMaxNodes)
  : GraphEval (theMaxNodes)
{
}

inline Energy::~Energy() {}

inline Energy::Var Energy::AddVariable()
{
  return AddNode();
}

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
    AddEdge (x, y, 0, B + C);
  }

  else if (C < 0)
  {
    AddTWeights (x, 0, -C);
    AddTWeights (y, 0, C);
    AddEdge (x, y, B + C, 0);
  }

  else
  {
    AddEdge (x, y, B, C);
  }
}

inline Energy::TotalValue Energy::Minimize() { return MaximumFlow(); }

inline int Energy::Label (Var x) { return (int) GraphEval::Label (x); }

#endif // _BRepMesh_MinStCut_HeaderFile
