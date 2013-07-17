// Copyright (c) 2013 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

#ifndef _XSControl_TransferReader_HeaderFile
#define _XSControl_TransferReader_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineHandle.hxx>
#include <XSControl_Controller.hxx>
#include <TCollection_AsciiString.hxx>
#include <Handle_Interface_InterfaceModel.hxx>
#include <Handle_Interface_HGraph.hxx>
#include <Handle_Dico_DictionaryOfTransient.hxx>
#include <Transfer_ActorOfTransientProcess.hxx>
#include <Transfer_TransientProcess.hxx>
#include <TColStd_DataMapOfIntegerTransient.hxx>
#include <Handle_TopTools_HSequenceOfShape.hxx>
#include <MMgt_TShared.hxx>
#include <Standard_CString.hxx>
#include <Handle_Standard_Transient.hxx>
#include <Standard_Boolean.hxx>
#include <Handle_Standard_Type.hxx>
#include <Standard_Integer.hxx>
#include <Handle_TColStd_HSequenceOfTransient.hxx>
#include <Transfer_ResultFromModel.hxx>
#include <Interface_CheckStatus.hxx>
#include <Handle_IFSelect_SelectExtract.hxx>

class XSControl_Controller;
class Handle(XSControl_Controller);
class Interface_InterfaceModel;
class Interface_HGraph;
class Dico_DictionaryOfTransient;
class Transfer_ActorOfTransientProcess;
class Transfer_TransientProcess;
class TopTools_HSequenceOfShape;
class Standard_Transient;
class Standard_Type;
class TColStd_HSequenceOfTransient;
class Transfer_ResultFromModel;
class TopoDS_Shape;
class Interface_CheckIterator;
class Interface_Graph;
class Standard_Transient;
class Handle(Standard_Type);

DEFINE_STANDARD_HANDLE(XSControl_TransferReader,MMgt_TShared)

//! A TransferReader performs, manages, handles results of, <br>
//!           transfers done when reading a file (i.e. from entities of an <br>
//!           InterfaceModel, to objects for Imagine) <br>
//! <br>
//!           Running is organised around basic tools : TransientProcess and <br>
//!           its Actor, results are Binders and CheckIterators. It implies <br>
//!           control by a Controller (which prepares the Actor as required) <br>
//! <br>
//!           Getting results can be done directly on TransientProcess, but <br>
//!           these are immediate "last produced" results. Each transfer of <br>
//!           an entity gives a final result, but also possible intermediate <br>
//!           data, and checks, which can be attached to sub-entities. <br>
//! <br>
//!           Hence, final results (which intermediates and checks) are <br>
//!           recorded as ResultFromModel and can be queried individually. <br>
//! <br>
//!           Some more direct access are given for results which are <br>
//!           Transient or Shapes <br>
class XSControl_TransferReader : public MMgt_TShared
{
public:

  //! Creates a TransferReader, empty <br>
  Standard_EXPORT   XSControl_TransferReader();
  //! Sets a Controller. It is required to generate the Actor. <br>
  //!           Elsewhere, the Actor must be provided directly <br>
  Standard_EXPORT     void SetController(const Handle(XSControl_Controller)& control) ;
  //! Sets the Actor directly : this value will be used if the <br>
  //!           Controller is not set <br>
  Standard_EXPORT     void SetActor(const Handle(Transfer_ActorOfTransientProcess)& actor) ;
  //! Returns the Actor, determined by the Controller, or if this <br>
  //!           one is unknown, directly set. <br>
  //!           Once it has been defined, it can then be edited. <br>
  Standard_EXPORT     Handle(Transfer_ActorOfTransientProcess) Actor() ;
  //! Sets an InterfaceModel. This causes former results, computed <br>
  //!           from another one, to be lost (see also Clear) <br>
  Standard_EXPORT     void SetModel (const Handle(Interface_InterfaceModel)& model) ;
  //! Sets a Graph and its InterfaceModel (calls SetModel) <br>
  Standard_EXPORT     void SetGraph (const Handle(Interface_HGraph)& graph) ;
  //! Returns the currently set InterfaceModel <br>
  Standard_EXPORT     Handle_Interface_InterfaceModel Model() const;
  //! Sets a Context : according to receiving appli, to be <br>
  //!           interpreted by the Actor <br>
  Standard_EXPORT     void SetContext (const Standard_CString name,
                                       const Handle(Standard_Transient)& ctx) ;
  //! Returns the Context attached to a name, if set and if it is <br>
  //!           Kind of the type, else a Null Handle <br>
  //!           Returns True if OK, False if no Context <br>
  Standard_EXPORT     Standard_Boolean GetContext (const Standard_CString name,
                                                   const Handle(Standard_Type)& type,
                                                   Handle(Standard_Transient)& ctx) const;
  //! Returns (modifiable) the whole definition of Context <br>
  //!           Rather for internal use (ex.: preparing and setting in once) <br>
  Standard_EXPORT     Handle(Dico_DictionaryOfTransient)& Context() ;
  //! Sets a new value for (loaded) file name <br>
  Standard_EXPORT     void SetFileName(const Standard_CString name) ;
  //! Returns actual value of file name <br>
  Standard_EXPORT     Standard_CString FileName() const;
  //! Clears data, according mode : <br>
  //!           -1 all <br>
  //!            0 nothing done <br>
  //!           +1 final results <br>
  //!           +2 working data (model, context, transfer process) <br>
  Standard_EXPORT     void Clear(const Standard_Integer mode) ;
  //! Returns the currently used TransientProcess <br>
  //!           It is computed from the model by TransferReadRoots, or by <br>
  //!           BeginTransferRead <br>
  Standard_EXPORT     Handle(Transfer_TransientProcess) TransientProcess() const;
  //! Forces the TransientProcess <br>
  //!           Remark : it also changes the Model and the Actor, from those <br>
  //!           recorded in the new TransientProcess <br>
  Standard_EXPORT     void SetTransientProcess (const Handle(Transfer_TransientProcess)& TP) ;
  //! Records a final result of transferring an entity <br>
  //!           This result is recorded as a ResultFromModel, taken from <br>
  //!           the TransientProcess <br>
  //!           Returns True if a result is available, False else <br>
  Standard_EXPORT     Standard_Boolean RecordResult (const Handle(Standard_Transient)& ent) ;
  //! Returns True if a final result is recorded for an entity <br>
  //!           Remark that it can bring no effective result if transfer has <br>
  //!           completely failed (FinalResult brings only fail messages ...) <br>
  Standard_EXPORT     Standard_Boolean IsRecorded (const Handle(Standard_Transient)& ent) const;
  //! Returns True if a final result is recorded AND BRINGS AN <br>
  //!           EFFECTIVE RESULT (else, it brings only fail messages) <br>
  Standard_EXPORT     Standard_Boolean HasResult (const Handle(Standard_Transient)& ent) const;
  //! Returns the list of entities to which a final result is <br>
  //!           attached (i.e. processed by RecordResult) <br>
  Standard_EXPORT     Handle_TColStd_HSequenceOfTransient RecordedList() const;
  //! Note that an entity has been required for transfer but no <br>
  //!           result at all is available (typically : case not implemented) <br>
  //!           It is not an error, but it gives a specific status : Skipped <br>
  //!           Returns True if done, False if <ent> is not in starting model <br>
  Standard_EXPORT     Standard_Boolean Skip (const Handle(Standard_Transient)& ent) ;
  //! Returns True if an entity is noted as skipped <br>
  Standard_EXPORT     Standard_Boolean IsSkipped (const Handle(Standard_Transient)& ent) const;
  //! Returns True if an entity has been asked for transfert, hence <br>
  //!           it is marked, as : Recorded (a computation has ran, with or <br>
  //!           without an effective result), or Skipped (case ignored) <br>
  Standard_EXPORT     Standard_Boolean IsMarked (const Handle(Standard_Transient)& ent) const;
  //! Returns the final result recorded for an entity, as such <br>
  Standard_EXPORT     Handle_Transfer_ResultFromModel FinalResult (const Handle(Standard_Transient)& ent) const;
  //! Returns the label attached to an entity recorded for final, <br>
  //!           or an empty string if not recorded <br>
  Standard_EXPORT     Standard_CString FinalEntityLabel (const Handle(Standard_Transient)& ent) const;
  //! Returns the number attached to the entity recorded for final, <br>
  //!           or zero if not recorded (looks in the ResultFromModel) <br>
  Standard_EXPORT     Standard_Integer FinalEntityNumber (const Handle(Standard_Transient)& ent) const;
  //! Returns the final result recorded for a NUMBER of entity <br>
  //!           (internal use). Null if out of range <br>
  Standard_EXPORT     Handle_Transfer_ResultFromModel ResultFromNumber (const Standard_Integer num) const;
  //! Returns the resulting object as a Transient <br>
  //!           Null Handle if no result or result not transient <br>
  Standard_EXPORT     Handle_Standard_Transient TransientResult (const Handle(Standard_Transient)& ent) const;
  //! Returns the resulting object as a Shape <br>
  //!           Null Shape if no result or result not a shape <br>
  Standard_EXPORT     TopoDS_Shape ShapeResult(const Handle(Standard_Transient)& ent) const;
  //! Clears recorded result for an entity, according mode <br>
//!           <mode> = -1 : true, complete, clearing (erasing result) <br>
//!           <mode> >= 0 : simple "stripping", see ResultFromModel, <br>
//!             in particular, 0 for simple internal strip, <br>
//!             10 for all but final result, <br>
//!             11 for all : just label, status and filename are kept <br>
//!           Returns True when done, False if nothing was to clear <br>
  Standard_EXPORT     Standard_Boolean ClearResult (const Handle(Standard_Transient)& ent,
                                                    const Standard_Integer mode) ;
  //! Returns an entity from which a given result was produced. <br>
  //!           If <mode> = 0 (D), searches in last root transfers <br>
  //!           If <mode> = 1,     searches in last (root & sub) transfers <br>
  //!           If <mode> = 2,     searches in root recorded results <br>
  //!           If <mode> = 3,     searches in all (root & sub) recordeds <br>
  //!           <res> can be, either a transient object (result itself) or <br>
  //!           a binder. For a binder of shape, calls EntityFromShapeResult <br>
  //!           Returns a Null Handle if <res> not recorded <br>
  Standard_EXPORT     Handle_Standard_Transient EntityFromResult (const Handle(Standard_Transient)& res,
                                                                  const Standard_Integer mode = 0) const;
  //! Returns an entity from which a given shape result was produced <br>
  //!           Returns a Null Handle if <res> not recorded or not a Shape <br>
  Standard_EXPORT     Handle_Standard_Transient EntityFromShapeResult (const TopoDS_Shape& res,
                                                                       const Standard_Integer mode = 0) const;
  //! Returns the list of entities from which some shapes were <br>
  //!           produced : it corresponds to a loop on EntityFromShapeResult, <br>
  //!           but is optimised <br>
  Standard_EXPORT     Handle_TColStd_HSequenceOfTransient EntitiesFromShapeList (const Handle(TopTools_HSequenceOfShape)& res,
                                                                                 const Standard_Integer mode = 0) const;
  //! Returns the CheckList resulting from transferring <ent>, i.e. <br>
  //!           stored in its recorded form ResultFromModel <br>
  //!           (empty if transfer successful or not recorded ...) <br>
  //! <br>
  //!           If <ent> is the Model, returns the complete cumulated <br>
  //!           check-list, <level> is ignored <br>
  //! <br>
  //!           If <ent> is an entity of the Model, <level> applies as follows <br>
  //!           <level> : -1 for <ent> only, LAST transfer (TransientProcess) <br>
  //!           <level> : 0  for <ent> only (D) <br>
  //!                     1  for <ent> and its immediate subtransfers, if any <br>
  //!                     2  for <ent> and subtransferts at all levels <br>
  Standard_EXPORT     Interface_CheckIterator CheckList(const Handle(Standard_Transient)& ent,
                                                        const Standard_Integer level = 0) const;
  //! Returns True if an entity (with a final result) has checks : <br>
  //!           - failsonly = False : any kind of check message <br>
  //!           - failsonly = True  : fails only <br>
  //!           Returns False if <ent> is not recorded <br>
  Standard_EXPORT     Standard_Boolean HasChecks (const Handle(Standard_Transient)& ent,
                                                  const Standard_Boolean failsonly) const;
  //! Returns the list of starting entities to which a given check <br>
  //!           status is attached, IN FINAL RESULTS <br>
  //!           <ent> can be an entity, or the model to query all entities <br>
  //!           Below, "entities" are, either <ent> plus its sub-transferred, <br>
  //!           or all the entities of the model <br>
  //! <br>
  //!           <check> = -2 , all entities whatever the check (see result) <br>
  //!           <check> = -1 , entities with no fail (warning allowed) <br>
  //!           <check> =  0 , entities with no check at all <br>
  //!           <check> =  1 , entities with warning but no fail <br>
  //!           <check> =  2 , entities with fail <br>
  //!           <result> : if True, only entities with an attached result <br>
  //!           Remark : result True and check=0 will give an empty list <br>
  Standard_EXPORT     Handle_TColStd_HSequenceOfTransient CheckedList(const Handle(Standard_Transient)& ent,
                                                                      const Interface_CheckStatus withcheck = Interface_CheckAny,
                                                                      const Standard_Boolean result = Standard_True) const;
  //! Defines a new TransferProcess for reading transfer <br>
  //!           Returns True if done, False if data are not properly defined <br>
  //!           (the Model, the Actor for Read) <br>
  Standard_EXPORT     Standard_Boolean BeginTransfer() ;
  //! Tells if an entity is recognized as a valid candidate for <br>
  //!           Transfer. Calls method Recognize from the Actor (if known) <br>
  Standard_EXPORT     Standard_Boolean Recognize(const Handle(Standard_Transient)& ent) ;
  //! Commands the transfer on reading for an entity to data for <br>
  //!           Imagine, using the selected Actor for Read <br>
  //!           Returns count of transferred entities, ok or with fails (0/1) <br>
  //!           If <rec> is True (D), the result is recorded by RecordResult <br>
  Standard_EXPORT     Standard_Integer TransferOne(const Handle(Standard_Transient)& ent,
                                                   const Standard_Boolean rec = Standard_True);
  //! Commands the transfer on reading for a list of entities to <br>
  //!           data for Imagine, using the selected Actor for Read <br>
  //!           Returns count of transferred entities, ok or with fails (0/1) <br>
  //!           If <rec> is True (D), the results are recorded by RecordResult <br>
  Standard_EXPORT     Standard_Integer TransferList(const Handle(TColStd_HSequenceOfTransient)& list,
                                                    const Standard_Boolean rec = Standard_True);
  //! Transfers the content of the current Interface Model to <br>
  //!           data handled by Imagine, starting from its Roots (determined <br>
  //!           by the Graph <G>),  using the selected Actor for Read <br>
  //!           Returns the count of performed root transfers (i.e. 0 if none) <br>
  //!           or -1 if no actor is defined <br>
  Standard_EXPORT     Standard_Integer TransferRoots(const Interface_Graph& G) ;
  //! Clears the results attached to an entity <br>
  //!           if <ents> equates the starting model, clears all results <br>
  Standard_EXPORT     void TransferClear(const Handle(Standard_Transient)& ent,
                                         const Standard_Integer level = 0) ;
  //! Prints statistics on current Trace File, according <what> and <br>
  //!           <mode>.  See PrintStatsProcess for details <br>
  Standard_EXPORT     void PrintStats(const Standard_Integer what,const Standard_Integer mode = 0) const;
  //! Returns the CheckList resulting from last TransferRead <br>
  //!           i.e. from TransientProcess itself, recorded from last Clear <br>
  Standard_EXPORT     Interface_CheckIterator LastCheckList() const;
  //! Returns the list of entities recorded as lastly transferred <br>
  //!           i.e. from TransientProcess itself, recorded from last Clear <br>
  //!           If <roots> is True , considers only roots of transfer <br>
  //!           If <roots> is False, considers all entities bound with result <br>
  Standard_EXPORT     Handle_TColStd_HSequenceOfTransient LastTransferList(const Standard_Boolean roots) const;
  //! Returns a list of result Shapes <br>
  //!           If <rec> is True , sees RecordedList <br>
  //!           If <rec> is False, sees LastTransferList (last ROOT transfers) <br>
  //!           For each one, if it is a Shape, it is cumulated to the list <br>
  //!           If no Shape is found, returns an empty Sequence <br>
  Standard_EXPORT     Handle_TopTools_HSequenceOfShape ShapeResultList(const Standard_Boolean rec) ;
  //! This routines prints statistics about a TransientProcess <br>
  //!           It can be called, by a TransferReader, or isolately <br>
  //!           Prints are done on the default trace file <br>
  //!           <what> defines what kind of statistics are to be printed : <br>
  //!           0 : basic figures <br>
  //!           1 : root results <br>
  //!           2 : all recorded (roots, intermediate, checked entities) <br>
  //!           3 : abnormal records <br>
  //!           4 : check messages (warnings and fails) <br>
  //!           5 : fail messages <br>
  //! <br>
  //!           <mode> is used according <what> : <br>
  //!           <what> = 0 : <mode> is ignored <br>
  //!           <what> = 1,2,3 : <mode> as follows : <br>
  //!           0 (D) : just lists numbers of concerned entities in the model <br>
  //!           1 : for each entity, gives number,label, type and result <br>
  //!                  type and/or status (fail/warning...) <br>
  //!           2 : for each entity, gives maximal information (i.e. checks) <br>
  //!           3 : counts per type of starting entity (class type) <br>
  //!           4 : counts per result type and/or status <br>
  //!           5 : counts per couple (starting type / result type/status) <br>
  //!           6 : idem plus gives for each item, the list of numbers of <br>
  //!                  entities in the starting model <br>
  //! <br>
  //!           <what> = 4,5 : modes relays on an enum PrintCount : <br>
  //!           0 (D) : ItemsByEntity (sequential list by entity) <br>
  //!           1 : CountByItem <br>
  //!           2 : ShortByItem       (count + 5 first numbers) <br>
  //!           3 : ListByItem        (count + entity numbers) <br>
  //!           4 : EntitiesByItem    (count + entity numbers and labels) <br>
  Standard_EXPORT   static  void PrintStatsProcess(const Handle(Transfer_TransientProcess)& TP,
                                                   const Standard_Integer what,
                                                   const Standard_Integer mode = 0) ;
  //! Works as PrintStatsProcess, but displays data only on the <br>
  //!           entities which are in <list> (filter) <br>
  Standard_EXPORT   static  void PrintStatsOnList(const Handle(Transfer_TransientProcess)& TP,
                                                  const Handle(TColStd_HSequenceOfTransient)& list,
                                                  const Standard_Integer what,
                                                  const Standard_Integer mode = 0) ;

  DEFINE_STANDARD_RTTI(XSControl_TransferReader)

private: 


Handle(XSControl_Controller) theController;
TCollection_AsciiString theFilename;
Handle(Interface_InterfaceModel) theModel;
Handle(Interface_HGraph) theGraph;
Handle(Dico_DictionaryOfTransient) theContext;
Handle(Transfer_ActorOfTransientProcess) theActor;
Handle(Transfer_TransientProcess) theTransfer;
TColStd_DataMapOfIntegerTransient theResults;
Handle(TopTools_HSequenceOfShape) theShapeResult;
};
#endif
