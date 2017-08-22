// Created on: 2017-08-22
// Created by: Sergey SLYADNEV
// Copyright (c) 2017 OPEN CASCADE SAS
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

// Own include
#include <XCAFDoc_AssemblyGraph.hxx>

// OCCT includes
#include <TColStd_MapIteratorOfPackedMapOfInteger.hxx>
#include <TDataStd_Name.hxx>
#include <TDataStd_TreeNode.hxx>
#include <TDF_ChildIterator.hxx>
#include <TDF_LabelSequence.hxx>
#include <TDF_Tool.hxx>
#include <XCAFDoc.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include <XCAFDoc_ShapeTool.hxx>

//-----------------------------------------------------------------------------

#define NodeLetter "N"
#define Whitespace "    "

//-----------------------------------------------------------------------------

XCAFDoc_AssemblyGraph::XCAFDoc_AssemblyGraph(const Handle(TDocStd_Document)& M,
                                             const bool                      withParts)
: Standard_Transient (),
  m_model            (M),
  m_bWithParts       (withParts)
{
  this->buildGraph();
}

//-----------------------------------------------------------------------------

void XCAFDoc_AssemblyGraph::Dump(Standard_OStream& out) const
{
  // Directed graph header
  out << "digraph core_AssemblyGraph {\n";
  out << "\n";

  // Dump nodes with attributes
  const NCollection_IndexedMap<XCAFDoc_ObjectId>& nodes = this->GetNodes();
  //
  for ( int n = 1; n <= nodes.Extent(); ++n )
  {
    // Get name of persistent object
    TCollection_ExtendedString name;
    this->getObjectName(nodes(n), name);

    // Generate label
    TCollection_AsciiString label(name);
    label += "\\n"; label += nodes(n);

    // Dump node with label
    out << Whitespace << NodeLetter << n << " [label=\"" << label.ToCString() << "\"];\n";
  }
  out << "\n";

  // Dump arcs
  for ( t_adjacency::Iterator it(m_arcs); it.More(); it.Next() )
  {
    const int                         parentId = it.Key();
    const TColStd_PackedMapOfInteger& children = it.Value();

    // Loop over the children
    for ( TColStd_MapIteratorOfPackedMapOfInteger cit(children); cit.More(); cit.Next() )
    {
      const int childId = cit.Key();

      out << Whitespace
          << NodeLetter << parentId
          << " -> "
          << NodeLetter << childId
          << ";\n";
    }
  }

  out << "\n";
  out << "}\n";
}

//-----------------------------------------------------------------------------

void XCAFDoc_AssemblyGraph::CalculateSummary(int& numRoots,
                                             int& numSubassemblies,
                                             int& numPartOccurrences,
                                             int& numParts) const
{
  numRoots           = 0;
  numSubassemblies   = 0;
  numPartOccurrences = 0;
  numParts           = 0;

  // Loop over the nodes
  for ( int n = 1; n <= this->GetNodes().Extent(); ++n )
  {
    if ( !m_nodeTypes.IsBound(n) ) continue; // This should never happen

    switch ( m_nodeTypes(n) )
    {
      case NodeType_Root:           ++numRoots;           break;
      case NodeType_Subassembly:    ++numSubassemblies;   break;
      case NodeType_PartOccurrence: ++numPartOccurrences; break;
      case NodeType_Part:           ++numParts;           break;
      default: break;
    }
  }
}

//-----------------------------------------------------------------------------

void XCAFDoc_AssemblyGraph::buildGraph()
{
  // Get shape tool
  Handle(XCAFDoc_ShapeTool)
    shapeTool = XCAFDoc_DocumentTool::ShapeTool( m_model->Main() );

  // We start from those shapes which are "free" in terms of XDE
  TDF_LabelSequence roots;
  //
  shapeTool->GetFreeShapes(roots);
  //
  for ( TDF_LabelSequence::Iterator it(roots); it.More(); it.Next() )
  {
    const TDF_Label& label = it.Value();

    // Get entry of the current label
    XCAFDoc_ObjectId objectId;
    TDF_Tool::Entry(label, objectId);

    // Free shapes are root nodes of the assembly graph
    const int iObjectId = m_nodes.Add(objectId);

    // Mark as root
    m_nodeTypes.Bind(iObjectId, NodeType_Root);
    //
    m_roots.Add(iObjectId);

    // Add components (the objects nested into the current one)
    this->addComponents(label, iObjectId);
  }
}

//-----------------------------------------------------------------------------

void XCAFDoc_AssemblyGraph::addComponents(const TDF_Label& parent,
                                          const int        iParentId)
{
  // Get shape tool
  Handle(XCAFDoc_ShapeTool)
    shapeTool = XCAFDoc_DocumentTool::ShapeTool( m_model->Main() );

  const bool isSubassembly = shapeTool->IsAssembly(parent);

  // Bind topological type. We check that no attribute is associated to
  // prevent multiple tagging of the same node from different directions
  // of traversal.
  if ( !m_nodeTypes.IsBound(iParentId) )
  {
    if ( isSubassembly )
      m_nodeTypes.Bind(iParentId, NodeType_Subassembly);
    else
    {
      m_nodeTypes.Bind(iParentId, NodeType_PartOccurrence);

      // If parts are requested to participate in the graph, we add more nodes
      if ( m_bWithParts )
      {
        // Get entry of the current label which is the original label
        XCAFDoc_ObjectId partId;
        TDF_Tool::Entry(parent, partId);

        // Add node
        const int iPartId = m_nodes.Add(partId);

        // Add arc
        if ( !m_arcs.IsBound(iParentId) )
          m_arcs.Bind( iParentId, TColStd_PackedMapOfInteger() );
        //
        m_arcs(iParentId).Add(iPartId);

        // Bind type
        if ( !m_nodeTypes.IsBound(iPartId) )
          m_nodeTypes.Bind(iPartId, NodeType_Part);
      }
    }
  }

  if ( !isSubassembly )
    return; // We have to return here in order to prevent iterating by
            // sub-labels. For parts, sub-labels are used to encode
            // metadata which is out of interest in conceptual design
            // intent represented by assembly graph.

  // Loop over the children (persistent representation of "part-of" relation)
  for ( TDF_ChildIterator cit(parent); cit.More(); cit.Next() )
  {
    TDF_Label child = cit.Value();

    // Get entry of the current label
    XCAFDoc_ObjectId childId;
    TDF_Tool::Entry(child, childId);

    // Add node
    const int iChildId = m_nodes.Add(childId);

    // Add arc
    if ( !m_arcs.IsBound(iParentId) )
      m_arcs.Bind( iParentId, TColStd_PackedMapOfInteger() );
    //
    m_arcs(iParentId).Add(iChildId);

    // Jump to the referred object (the original)
    TDF_Label childOriginal;
    Handle(TDataStd_TreeNode) jumpTreeNode;
    child.FindAttribute(XCAFDoc::ShapeRefGUID(), jumpTreeNode);
    //
    if ( !jumpTreeNode.IsNull() && jumpTreeNode->HasFather() )
      childOriginal = jumpTreeNode->Father()->Label(); // Declaration-level origin

    // Process children: add components recursively
    if ( !childOriginal.IsNull() )
      this->addComponents(childOriginal, iChildId);
  }
}

//-----------------------------------------------------------------------------

bool XCAFDoc_AssemblyGraph::getObjectName(const XCAFDoc_ObjectId&     id,
                                          TCollection_ExtendedString& name) const
{
  // Get label for object ID
  TDF_Label label;
  TDF_Tool::Label(m_model->GetData(), id, label);

  // Access name
  Handle(TDataStd_Name) nameAttr;
  if ( !label.FindAttribute(TDataStd_Name::GetID(), nameAttr) )
  {
    name = "";
    return false;
  }
  //
  name = nameAttr->Get();
  return true;
}
