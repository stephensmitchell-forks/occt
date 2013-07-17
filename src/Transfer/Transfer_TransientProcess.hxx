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

#ifndef _Transfer_TransientProcess_HeaderFile
#define _Transfer_TransientProcess_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineHandle.hxx>
#include <Standard_Transient.hxx>
#include <Handle_Interface_InterfaceModel.hxx>
#include <Handle_Interface_HGraph.hxx>
#include <Handle_Dico_DictionaryOfTransient.hxx>
#include <Handle_TColStd_HSequenceOfTransient.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_CString.hxx>
#include <Handle_Standard_Transient.hxx>
#include <Handle_Standard_Type.hxx>
#include <Handle_Message_Messenger.hxx>
#include <TColStd_HSequenceOfTransient.hxx>
#include <TColStd_MapTransientHasher.hxx>
#include <Transfer_ActorOfTransientProcess.hxx>
#include <Transfer_Binder.hxx>
#include <Transfer_TransferProcess.hxx>
#include <TColStd_MapTransientHasher.hxx>
#include <NCollection_IndexedDataMap.hxx>
#include <Transfer_IteratorOfProcessForTransient.hxx>
#include <MMgt_TShared.hxx>
#include <Transfer_TransientProcess_Handle.hxx>

class Interface_InterfaceModel;
class Interface_HGraph;
class Dico_DictionaryOfTransient;
class TColStd_HSequenceOfTransient;
class Interface_Graph;
class Standard_Transient;
class Standard_Type;
class Message_Messenger;
class Interface_EntityIterator;
class Handle(Standard_Type);
class Handle(MMgt_TShared);


//! Adds specific features to the generic definition : <br>
//!           TransientProcess is intended to work from an InterfaceModel <br>
//!           to a set of application objects. <br>
//! <br>
//!           Hence, some informations about starting entities can be gotten <br>
//!           from the model : for Trace, CheckList, Integrity Status <br>
class Transfer_TransientProcess
: public Transfer_TransferProcess < Handle(Transfer_ActorOfTransientProcess),
                                  Transfer_TransferMapOfProcessForTransient,
                                  Handle(Standard_Transient),
                                  TColStd_MapTransientHasher,
                                  Handle(TColStd_HSequenceOfTransient),
                                  Transfer_IteratorOfProcessForTransient >
{

public:

  //! Sets TransientProcess at initial state, with an initial size <br>
  Standard_EXPORT   Transfer_TransientProcess(const Standard_Integer nb = 10000);
  //! Sets an InterfaceModel, used by StartTrace, CheckList, queries <br>
  //!           on Integrity, to give informations significant for each norm. <br>
  Standard_EXPORT     void SetModel(const Handle(Interface_InterfaceModel)& model) ;
  //! Returns the Model used for StartTrace <br>
  Standard_EXPORT     Handle_Interface_InterfaceModel Model() const;
  //! Sets a Graph : superseedes SetModel if already done <br>
  Standard_EXPORT     void SetGraph(const Handle(Interface_HGraph)& HG) ;
  
  Standard_EXPORT     Standard_Boolean HasGraph() const;
  
  Standard_EXPORT     Handle_Interface_HGraph HGraph() const;
  
  Standard_EXPORT    const Interface_Graph& Graph() const;
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
  Standard_EXPORT     Handle_Dico_DictionaryOfTransient& Context() ;
  //! Specific printing to trace an entity : prints label and type <br>
  //!           (if model is set) <br>
  Standard_EXPORT   virtual  void PrintTrace (const Handle(Standard_Transient)& start,
                                              const Handle(Message_Messenger)& S) const;
  //! Specific number of a starting object for check-list : Number <br>
  //!           in model <br>
  Standard_EXPORT   virtual  Standard_Integer CheckNum(const Handle(Standard_Transient)& ent) const;
  //! Returns the list of sharings entities, AT ANY LEVEL, which are <br>
  //!           kind of a given type. Calls TypedSharings from Graph <br>
  //!           Returns an empty list if the Graph has not been aknowledged <br>
  Standard_EXPORT     Interface_EntityIterator TypedSharings (const Handle(Standard_Transient)& start,
                                                              const Handle(Standard_Type)& type) const;
  //! Tells if an entity is well loaded from file (even if its data <br>
  //!           fail on checking, they are present). Mostly often, answers <br>
  //!           True. Else, there was a syntactic error in the file. <br>
  //!           A non-loaded entity MAY NOT BE transferred, unless its Report <br>
  //!           (in the model) is interpreted <br>
  Standard_EXPORT     Standard_Boolean IsDataLoaded(const Handle(Standard_Transient)& ent) const;
  //! Tells if an entity fails on data checking (load time, <br>
  //!           syntactic, or semantic check). Normally, should answer False. <br>
  //!           It is not prudent to try transferring an entity which fails on <br>
  //!           data checking <br>
  Standard_EXPORT     Standard_Boolean IsDataFail(const Handle(Standard_Transient)& ent) const;
  //! Prints statistics on a given output, according mode <br>
  Standard_EXPORT     void PrintStats (const Standard_Integer mode,
                                       const Handle(Message_Messenger)& S) const;
  
  Standard_EXPORT     Handle_TColStd_HSequenceOfTransient RootsForTransfer() ;

  Standard_EXPORT Handle(Transfer_Binder) TransferProduct (const Handle(Standard_Transient)& start);

  Standard_EXPORT Handle(Transfer_Binder) Transferring (const Handle(Standard_Transient)& start);

  Standard_EXPORT Standard_Boolean Transfer(const Handle(Standard_Transient)& start);

  DEFINE_STANDARD_RTTI(Transfer_TransientProcess)

private: 
  Handle_Interface_InterfaceModel themodel;
  Handle_Interface_HGraph thegraph;
  Handle_Dico_DictionaryOfTransient thectx;
  Handle_TColStd_HSequenceOfTransient thetrroots;
};

#endif
