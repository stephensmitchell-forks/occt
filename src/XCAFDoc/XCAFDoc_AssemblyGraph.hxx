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

#ifndef _XCAFDoc_AssemblyGraph_HeaderFile
#define _XCAFDoc_AssemblyGraph_HeaderFile

// XDE includes
#include <XCAFDoc_ObjectId.hxx>

// Other OCCT includes
#include <NCollection_DataMap.hxx>
#include <TColStd_PackedMapOfInteger.hxx>
#include <TDocStd_Document.hxx>

//! \brief Assembly graph.
//!
//! This tool gives clear OCAF-agnostic interface to
//! the assembly structure of a product. A graph is essentially a set of
//! nodes {N} and a set of arcs {A} as defined formally.
//!
//! <pre>
//!   G = <N, A>
//! </pre>
//!
//! Using this tool, you can map XDE assembly items to a formal graph
//! structure. Each node in the graph preserves a link to the data storage
//! (OCAF document) by means of \ref XCAFDoc_ObjectId.
//!
//! \sa XCAFDoc_ObjectId
class XCAFDoc_AssemblyGraph : public Standard_Transient
{
public:

  //! \brief Type of the graph node.
  enum NodeType
  {
    NodeType_UNDEFINED = 0,  //!< Undefined node type.
    //
    NodeType_Root,           //!< Root node (has no entry arcs).
    NodeType_Subassembly,    //!< Intermediate node (non-leaf node which has entry arcs).
    NodeType_PartOccurrence, //!< Part usage occurrence.
    NodeType_Part            //!< Optional leaf node to represent parts. Note that
                             //!< this node type is activated by a dedicated flag in
                             //!< the constructor. If activated, the part occurrence nodes
                             //!< are not leafs anymore.
  };

public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(XCAFDoc_AssemblyGraph, Standard_Transient)

public:

  //! \brief Graph iterator.
  class Iterator
  {
  public:

    //! Default ctor.
    Iterator() : m_iCurrentIndex(0) {}

    //! ctor accepting the assembly graph to iterate.
    //! \param[in] asmGraph assembly graph to iterate.
    Iterator(const Handle(XCAFDoc_AssemblyGraph)& asmGraph)
    {
      this->Init(asmGraph);
    }

  public:

    //! Initializes iterator with assembly graph.
    //! \param[in] asmGraph assembly graph to iterate.
    void Init(const Handle(XCAFDoc_AssemblyGraph)& asmGraph)
    {
      m_graph         = asmGraph;
      m_iCurrentIndex = 1;
    }

    //! Checks if there are more graph nodes to iterate.
    //! \return true/false.
    bool More() const
    {
      return m_iCurrentIndex <= m_graph->GetNodes().Extent();
    }

    //! \return 1-based ID of the current node.
    int GetCurrentNode() const
    {
      return m_iCurrentIndex;
    }

    //! Moves iterator to the next position.
    void Next()
    {
      ++m_iCurrentIndex;
    }

  protected:

    Handle(XCAFDoc_AssemblyGraph) m_graph;         //!< Assembly graph to iterate.
    int                           m_iCurrentIndex; //!< Current 1-based node ID.

  };

public:

  //! Type definition for graph adjacency matrix. This is how parent-component
  //! links are realized in the assembly graph.
  typedef NCollection_DataMap<int, TColStd_PackedMapOfInteger> t_adjacency;

public:

  //! \brief Initializes graph from Data Model.
  //!
  //! Construction of a formal graph will be done immediately at ctor.
  //!
  //! \param[in] M         Data Model to iterate.
  //! \param[in] withParts indicates whether to add nodes representing the
  //!                      instanced parts to the assembly graph.
  Standard_EXPORT
    XCAFDoc_AssemblyGraph(const Handle(TDocStd_Document)& M,
                          const bool                      withParts = false);

public:

  //! \brief Dumps graph structure to output stream.
  //!
  //! The output format is DOT. You may use graph rendering tools like
  //! Graphviz to parse the output.
  //!
  //! \param[out] out output stream.
  Standard_EXPORT void
    Dump(Standard_OStream& out) const;

  //! \brief Calculates short summary for the assembly.
  //!
  //! Short summary gives you the total number of nodes of a particular
  //! type. Note that the number of parts will be calculated only if parts
  //! are available in the graph (which is the option by construction).
  //!
  //! \param[out] numRoots           number of root nodes.
  //! \param[out] numSubassemblies   number of subassembly nodes.
  //! \param[out] numPartOccurrences number of part usage occurrence nodes.
  //! \param[out] numParts           number of parts (if available).
  Standard_EXPORT void
    CalculateSummary(int& numRoots,
                     int& numSubassemblies,
                     int& numPartOccurrences,
                     int& numParts) const;

public:

  //! \brief Returns IDs of the root nodes.
  //! \return IDs of the root nodes.
  const TColStd_PackedMapOfInteger& GetRoots() const
  {
    return m_roots;
  }

  //! \brief Checks whether the assembly graph contains (n1, n2) directed arc.
  //! \param[in] n1 one-based ID of the first node.
  //! \param[in] n2 one-based ID of the second node.
  //! \return true/false.
  bool HasArc(const int n1, const int n2) const
  {
    if ( !this->HasChildren(n1) )
      return false;

    return this->GetChildren(n1).Contains(n2);
  }

  //! \brief Checks whether children exist for the given node.
  //! \param[in] oneBasedNodeId one-based node ID.
  //! \return true/false.
  bool HasChildren(const int oneBasedNodeId) const
  {
    return m_arcs.IsBound(oneBasedNodeId);
  }

  //! \brief Returns IDs of child nodes for the given node.
  //! \param[in] oneBasedNodeId one-based node ID.
  //! \return set of child IDs.
  const TColStd_PackedMapOfInteger& GetChildren(const int oneBasedNodeId) const
  {
    return m_arcs(oneBasedNodeId);
  }

  //! \brief Returns the node type from \ref NodeType enum.
  //! \param[in] oneBasedNodeId one-based node ID.
  //! \return node type.
  //! \sa NodeType
  NodeType GetNodeType(const int oneBasedNodeId) const
  {
    if ( !m_nodeTypes.IsBound(oneBasedNodeId) )
      return NodeType_UNDEFINED;

    return m_nodeTypes(oneBasedNodeId);
  }

  //! \brief returns object ID by node ID.
  //! \param[in] oneBasedNodeId one-based node ID.
  //! \return persistent ID.
  const XCAFDoc_ObjectId& GetPersistentId(const int oneBasedNodeId) const
  {
    return m_nodes(oneBasedNodeId);
  }

  //! \brief Returns the unordered set of graph nodes.
  //! \return graph nodes.
  const NCollection_IndexedMap<XCAFDoc_ObjectId>& GetNodes() const
  {
    return m_nodes;
  }

  //! \brief Returns the number of graph nodes.
  //! \return number of graph nodes.
  int GetNumberOfNodes() const
  {
    return m_nodes.Extent();
  }

  //! \brief Returns the collection of graph arcs in form of adjacency matrix.
  //! \return graph arcs.
  const t_adjacency& GetArcs() const
  {
    return m_arcs;
  }

  //! \brief Returns the number of graph arcs.
  //! \return number of graph arcs.
  int GetNumberOfArcs() const
  {
    int numArcs = 0;
    //
    for ( t_adjacency::Iterator it(m_arcs); it.More(); it.Next() )
      numArcs += it.Value().Extent();

    return numArcs;
  }

protected:

  //! Builds graph out of OCAF XDE structure.
  Standard_EXPORT void
    buildGraph();

  //! Adds components for the given root to the graph structure.
  //! \param[in] parent    OCAF label of the parent object.
  //! \param[in] iParentId ID of the already registered node
  //!                      representing the parent object in the assembly
  //!                      graph being populated.
  Standard_EXPORT void
    addComponents(const TDF_Label& parent,
                  const int        iParentId);

  //! Returns object name for the given persistent ID.
  //! \param[in]  id   persistent ID.
  //! \param[out] name object name.
  //! \return false if no name is associated with the object or the object
  //!         does not exist.
  Standard_EXPORT bool
    getObjectName(const XCAFDoc_ObjectId&     id,
                  TCollection_ExtendedString& name) const;

protected:

  // INPUTS
  Handle(TDocStd_Document) m_model;      //!< Data Model instance.
  bool                     m_bWithParts; //!< Indicates whether to include parts to the graph.

  // OUTPUTS
  TColStd_PackedMapOfInteger               m_roots;     //!< IDs of the root nodes.
  NCollection_IndexedMap<XCAFDoc_ObjectId> m_nodes;     //!< Graph nodes.
  t_adjacency                              m_arcs;      //!< "Part-of" relations.
  NCollection_DataMap<int, NodeType>       m_nodeTypes; //!< Node types (cached for efficiency).

};

#endif
