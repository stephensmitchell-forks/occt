// Created on: 1993-04-22
// Created by: Christian CAILLET
// Copyright (c) 1993-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#ifndef _Interface_HGraph_HeaderFile
#define _Interface_HGraph_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>
#include <MMgt_TShared.hxx>

#include <Interface_Graph.hxx>
class Interface_InterfaceModel;

class Interface_HGraph;
DEFINE_STANDARD_HANDLE(Interface_HGraph, MMgt_TShared)

//! This class allows to store a redefinable Graph, via a Handle
//! (usefull for an Object which can work on several successive
//! Models, with the same general conditions)
class Interface_HGraph : public MMgt_TShared
{
 public:
  
  //! Creates an HGraph directly from a Graph.
  //! Remark that the starting Graph is duplicated
  Interface_HGraph(const Interface_Graph& agraph)
  : thegraph (agraph)
  {}
  
  //! Creates an HGraph with a Graph created from <amodel>, works with the GTool in the model
  Interface_HGraph(const Handle(Interface_InterfaceModel)& amodel, const Standard_Boolean theModeStats = Standard_True)
  : thegraph (amodel,theModeStats)
  {}
  
  //! Returns the Graph contained in <me>, for Read Only Operations
  //! Remark that it is returns as "const &"
  //! Getting it in a new variable instead of a reference would be
  //! a pitty, because all the graph's content would be duplicated
  const Interface_Graph& Graph() const { return thegraph; }
  
  //! Same as above, but for Read-Write Operations
  //! Then, The Graph will be modified in the HGraph itself
  Interface_Graph& CGraph() { return thegraph; }

  DEFINE_STANDARD_RTTIEXT(Interface_HGraph,MMgt_TShared)

 private:

  Interface_Graph thegraph;
};

#endif // _Interface_HGraph_HeaderFile
