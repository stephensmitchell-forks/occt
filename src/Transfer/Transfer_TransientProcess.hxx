// Created on: 1996-09-04
// Created by: Christian CAILLET
// Copyright (c) 1996-1999 Matra Datavision
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

#ifndef _Transfer_TransientProcess_HeaderFile
#define _Transfer_TransientProcess_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TColStd_SequenceOfTransient.hxx>
#include <Transfer_Process.hxx>
class Dico_DictionaryOfTransient;
class Message_Messenger;
class Interface_HGraph;
class Interface_EntityIterator;

class Transfer_TransientProcess;
DEFINE_STANDARD_HANDLE(Transfer_TransientProcess, Transfer_Process)

//! Adds specific features to the generic definition :
//! TransientProcess is intended to work from an InterfaceModel
//! to a set of application objects.
//!
//! Hence, some informations about starting entities can be gotten
//! from the model : for Trace, CheckList, Integrity Status
class Transfer_TransientProcess : public Transfer_Process
{
 public:
  
  //! Sets TransientProcess at initial state, with an initial size
  Transfer_TransientProcess(const Standard_Integer nb = 10000)
  : Transfer_Process (nb)
  {}

  //! Sets a Graph : superseedes SetModel if already done
  Standard_EXPORT void SetGraph (const Handle(Interface_HGraph)& HG);
  
  Standard_Boolean HasGraph() const { return !thegraph.IsNull(); }
  
  const Handle(Interface_HGraph) & HGraph() const { return thegraph; }
  
  //! Sets a Context : according to receiving appli, to be
  //! interpreted by the Actor
  Standard_EXPORT void SetContext (const Standard_CString name, const Handle(Standard_Transient)& ctx);
  
  //! Returns the Context attached to a name, if set and if it is
  //! Kind of the type, else a Null Handle
  //! Returns True if OK, False if no Context
  Standard_EXPORT Standard_Boolean GetContext (const Standard_CString name, const Handle(Standard_Type)& type, Handle(Standard_Transient)& ctx) const;
  
  //! Returns (modifiable) the whole definition of Context
  //! Rather for internal use (ex.: preparing and setting in once)
  Handle(Dico_DictionaryOfTransient)& Context() { return thectx; }
  
  TColStd_SequenceOfTransient & RootsForTransfer() { return thetrroots; }

  DEFINE_STANDARD_RTTIEXT(Transfer_TransientProcess,Transfer_Process)

 protected:
  
  //! Specific number of a source object for check-list : Number in model
  Standard_EXPORT Standard_Integer CheckNum (const Handle(Standard_Transient)& ent) const Standard_OVERRIDE;
  
  //! Specific printing to trace an entity : prints label and type
  //! (if model is set)
  Standard_EXPORT void PrintTrace (const Handle(Standard_Transient)& start, const Handle(Message_Messenger)& S) const Standard_OVERRIDE;

 private:

  Handle(Interface_HGraph) thegraph;
  Handle(Dico_DictionaryOfTransient) thectx;
  TColStd_SequenceOfTransient thetrroots;
};

#endif // _Transfer_TransientProcess_HeaderFile
