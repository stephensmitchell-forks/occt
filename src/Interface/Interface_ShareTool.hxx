// Created on: 1993-02-02
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

#ifndef _Interface_ShareTool_HeaderFile
#define _Interface_ShareTool_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Interface_HGraph.hxx>
class Interface_InterfaceModel;
class Interface_EntityIterator;
class Message_Messenger;


//! Builds the Graph of Dependancies, from the General Service
//! "Shared" -> builds for each Entity of a Model, the Shared and
//! Sharing Lists, and gives access to them.
//! Allows to complete with Implied References (which are not
//! regarded as Shared Entities, but are nevertheless Referenced),
//! this can be usefull for Reference Checking
class Interface_ShareTool 
{
 public:

  DEFINE_STANDARD_ALLOC
  
  //! Creates a ShareTool from a Model and builds all required data,
  //! works with the GTool of the Model
  Interface_ShareTool(const Handle(Interface_InterfaceModel)& amodel)
  : theHGraph(new Interface_HGraph(amodel))
  {}

  //! Creates a ShareTool from an already defined Graph
  //! Remark that the data of the Graph are copied
  Interface_ShareTool(const Interface_Graph& agraph)
  : theHGraph(new Interface_HGraph(agraph.Model()))
  {}

  //! Completes the Graph by Adding Implied References. Hence, they
  //! are considered as Sharing References in all the other queries
  Interface_ShareTool(const Handle(Interface_HGraph)& ahgraph)
  : theHGraph(ahgraph)
  {}

  //! Returns the data used by the ShareTool to work
  //! Can then be used directly (read only)
  const Interface_Graph& Graph() const { return theHGraph->Graph(); }

  //! Returns True if <ent> is Shared by other Entities in the Model
  Standard_EXPORT Standard_Boolean IsShared (const Handle(Standard_Transient)& ent) const;
  
  //! Returns the count of Sharing Entities of an Entity, which
  //! are Kind of a given Type
  //szv_c1:Standard_EXPORT Standard_Integer NbTypedSharings (const Handle(Standard_Transient)& ent, const Handle(Standard_Type)& atype) const;
  
  //! Returns the complete list of entities shared by <ent> at any
  //! level, including <ent> itself
  //! If <ent> is the Model, considers the concatenation of
  //! AllShared for each root
  //! If <rootlast> is True (D), the list starts with lower level
  //! entities and ends by the root. Else, the root is first and
  //! the lower level entities are at end
  Standard_EXPORT Interface_EntityIterator All (const Handle(Standard_Transient)& ent, const Standard_Boolean rootlast = Standard_True) const;

 private:

  Handle(Interface_HGraph) theHGraph;
};

#endif // _Interface_ShareTool_HeaderFile
