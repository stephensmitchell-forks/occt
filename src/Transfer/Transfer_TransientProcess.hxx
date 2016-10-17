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

//c1:#include <TCollection_AsciiString.hxx>
#include <TColStd_SequenceOfTransient.hxx>
#include <TColStd_DataMapOfIntegerTransient.hxx>
#include <TopoDS_Shape.hxx>
#include <Transfer_Process.hxx>
#include <Interface_CheckStatus.hxx>
class Dico_DictionaryOfTransient;
class Message_Messenger;
class Interface_HGraph;
class Interface_EntityIterator;
class Transfer_ResultFromModel;

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
  
  //! Defines a new TransferProcess for reading transfer
  //! Returns True if done, False if data are not properly defined
  //! (the Model, the Actor for Read)
  Standard_EXPORT Standard_Boolean BeginTransfer();
  
  //! Commands the transfer on reading for an entity to data for
  //! Imagine, using the selected Actor for Read
  //! Returns count of transferred entities, ok or with fails (0/1)
  //! If <rec> is True (D), the result is recorded by RecordResult
  Standard_EXPORT Standard_Integer TransferOne (const Handle(Standard_Transient)& theEnt);

  //! Records a final result of transferring an entity
  //! This result is recorded as a ResultFromModel, taken from
  //! the TransientProcess
  //! Returns True if a result is available, False else
  Standard_EXPORT Standard_Boolean RecordResult (const Handle(Standard_Transient)& theEnt);
  
  //! Returns True if a final result is recorded for an entity
  //! Remark that it can bring no effective result if transfer has
  //! completely failed (FinalResult brings only fail messages ...)
  Standard_EXPORT Standard_Boolean IsRecorded (const Handle(Standard_Transient)& theEnt) const;
  
  //! Returns the list of entities to which a final result is
  //! attached (i.e. processed by RecordResult)
  Standard_EXPORT Handle(TColStd_HSequenceOfTransient) RecordedList() const;
  
  //! Returns the final result recorded for an entity, as such
  Standard_EXPORT Handle(Transfer_ResultFromModel) FinalResult (const Handle(Standard_Transient)& theEnt) const;
  
  //! Returns the resulting object as a Shape
  //! Null Shape if no result or result not a shape
  Standard_EXPORT TopoDS_Shape ShapeResult (const Handle(Standard_Transient)& theEnt) const;

  //! Returns an entity from which a given shape result was produced
  //! If <mode> = 0 (D), searches in last root transfers
  //! If <mode> = 1,     searches in last (root & sub) transfers
  //! If <mode> = 2,     searches in root recorded results
  //! If <mode> = 3,     searches in all (root & sub) recordeds
  //! <res> can be, either a transient object (result itself) or
  //! a binder. For a binder of shape, calls EntityFromShapeResult
  //! Returns a Null Handle if <res> not recorded or not a Shape
  Standard_EXPORT Handle(Standard_Transient) EntityFromShapeResult (const TopoDS_Shape& theRes, const Standard_Integer theMode = 0) const;

  //! Returns the list of starting entities to which a given check
  //! status is attached, IN FINAL RESULTS
  //! <ent> can be an entity, or the model to query all entities
  //! Below, "entities" are, either <ent> plus its sub-transferred,
  //! or all the entities of the model
  //!
  //! <check> = -2 , all entities whatever the check (see result)
  //! <check> = -1 , entities with no fail (warning allowed)
  //! <check> =  0 , entities with no check at all
  //! <check> =  1 , entities with warning but no fail
  //! <check> =  2 , entities with fail
  //! <result> : if True, only entities with an attached result
  //! Remark : result True and check=0 will give an empty list
  Standard_EXPORT Handle(TColStd_HSequenceOfTransient) CheckedList (const Handle(Standard_Transient)& theEnt, const Interface_CheckStatus WithCheck = Interface_CheckAny, const Standard_Boolean theResult = Standard_True) const;
  
  //! Prints the transfer status of a transferred item, as beeing the Mapped n0 <num>
  //! Returns True when done, False else (i.e. num out of range)
  Standard_EXPORT virtual Standard_Boolean PrintTransferStatus (const Standard_Integer theNum, const Handle(Message_Messenger)& theMessenger) const Standard_OVERRIDE;

  void ClearResults () { myResults.Clear(); }

  DEFINE_STANDARD_RTTIEXT(Transfer_TransientProcess,Transfer_Process)

 protected:
  
  //! Specific number of a source object for check-list : Number in model
  Standard_EXPORT Standard_Integer CheckNum (const Handle(Standard_Transient)& ent) const Standard_OVERRIDE;
  
  //! Specific printing to trace an entity : prints label and type
  //! (if model is set)
  Standard_EXPORT void PrintTrace (const Handle(Standard_Transient)& start, const Handle(Message_Messenger)& S) const Standard_OVERRIDE;
  
  //! Returns the final result recorded for a NUMBER of entity
  //! (internal use). Null if out of range
  Standard_EXPORT Handle(Transfer_ResultFromModel) ResultFromNumber (const Standard_Integer theNum) const;

 private:

  Handle(Interface_HGraph) thegraph;
  Handle(Dico_DictionaryOfTransient) thectx;
  TColStd_SequenceOfTransient thetrroots;
  TColStd_DataMapOfIntegerTransient myResults;
};

#endif // _Transfer_TransientProcess_HeaderFile
