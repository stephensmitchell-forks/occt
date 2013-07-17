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


#ifndef _XSControl_WorkSession_HeaderFile
#define _XSControl_WorkSession_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineHandle.hxx>
#include <XSControl_Controller.hxx>
#include <XSControl_TransferReader.hxx>
#include <XSControl_TransferWriter.hxx>
#include <Handle_Dico_DictionaryOfTransient.hxx>
#include <Standard_Integer.hxx>
#include <Handle_XSControl_Vars.hxx>
#include <IFSelect_WorkSession.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_CString.hxx>
#include <Handle_Message_Messenger.hxx>
#include <Transfer_TransientProcess.hxx>
#include <Handle_Standard_Transient.hxx>
#include <Handle_Interface_InterfaceModel.hxx>
#include <Transfer_FinderProcess.hxx>
#include <IFSelect_ReturnStatus.hxx>

class XSControl_Controller;
class XSControl_TransferReader;
class XSControl_TransferWriter;
class Dico_DictionaryOfTransient;
class XSControl_Vars;
class Message_Messenger;
class Transfer_TransientProcess;
class Standard_Transient;
class Interface_InterfaceModel;
class Transfer_FinderProcess;
class TopoDS_Shape;
class Interface_CheckIterator;
class Standard_Transient;
class Handle(Standard_Type);
class Handle(IFSelect_WorkSession);

DEFINE_STANDARD_HANDLE(XSControl_WorkSession,IFSelect_WorkSession)


//! This WorkSession completes the basic one, by adding : <br>
//!           - use of Controller, with norm selection... <br>
//!           - management of transfers (both ways) with auxiliary classes <br>
//!             TransferReader and TransferWriter <br>
//!            -> these transfers may work with a Context List : its items <br>
//!               are given by the user, according to the transfer to be <br>
//!               i.e. it is interpreted by the Actors <br>
//!               Each item is accessed by a Name <br>
class XSControl_WorkSession : public IFSelect_WorkSession
{

public:

  Standard_EXPORT   XSControl_WorkSession();
  //! In addition to basic ClearData, clears Transfer and Management <br>
//!           for interactive use, for mode = 0,1,2 and over 4 <br>
//!           Plus : mode = 5 to clear Transfers (both ways) only <br>
//!                  mode = 6 to clear enforced results <br>
//!                  mode = 7 to clear transfers, results <br>
  Standard_EXPORT   virtual  void ClearData(const Standard_Integer mode) ;
  //! Selects a Norm defined by its name. <br>
//!           A Norm is described and handled by a Controller <br>
//!           Returns True if done, False if <normname> is unknown <br>
//! <br>
//!           A Profile may be set too. If no Profile is provided, the <br>
//!           current Profile for this Norm is taken <br>
//!           If the asked Profile is not defined for this Norm, it remains <br>
//!           in current Profile, returned value is True <br>
  Standard_EXPORT     Standard_Boolean SelectNorm(const Standard_CString normname,const Standard_CString profile = "") ;
  //! Sets a Profile as current for the current Norm <br>
//!           Returns True if done, False if <profile> is unknown for this norm <br>
//! <br>
//!           For more infos on Profile, query the Profile of the Controller <br>
  Standard_EXPORT     Standard_Boolean SelectProfile(const Standard_CString profile) ;
  //! Selects a Norm defined by its Controller itself <br>
  Standard_EXPORT     void SetController(const Handle(XSControl_Controller)& ctl) ;
  //! This method is called once a new norm has been successfully <br>
//!           selected. It can be redefined, default does nothing <br>
  Standard_EXPORT   virtual  void AdaptNorm() ;
  //! Returns the name of the last Selected Norm. If none is <br>
//!           defined, returns an empty string <br>
//!           By default, returns the complete name of the norm <br>
//!           If <rsc> is True, returns the short name used for resource <br>
  Standard_EXPORT     Standard_CString SelectedNorm(const Standard_Boolean rsc = Standard_False) const;
  //! Returns the norm controller itself <br>
  Standard_EXPORT     Handle_XSControl_Controller NormAdaptor() const;
  //! Returns the current Context List, Null if not defined <br>
//!           The Context is given to the TransientProcess for TransferRead <br>
  Standard_EXPORT     Handle_Dico_DictionaryOfTransient Context() const;
  //! Sets the current Context List, as a whole <br>
//!           Sets it to the TransferReader <br>
  Standard_EXPORT     void SetAllContext(const Handle(Dico_DictionaryOfTransient)& context) ;
  //! Clears the whole current Context (nullifies it) <br>
  Standard_EXPORT     void ClearContext() ;
  //! Prints the transfer status of a transferred item, as beeing <br>
//!           the Mapped n0 <num>, from MapWriter if <wri> is True, or <br>
//!           from MapReader if <wri> is False <br>
//!           Returns True when done, False else (i.e. num out of range) <br>
  Standard_EXPORT     Standard_Boolean PrintTransferStatus(const Standard_Integer num,const Standard_Boolean wri,const Handle(Message_Messenger)& S) const;
  //! Sets a Transfer Reader, by internal ways, according mode : <br>
//!           0 recreates it clear,  1 clears it (does not recreate) <br>
//!           2 aligns Roots of TransientProcess from final Results <br>
//!           3 aligns final Results from Roots of TransientProcess <br>
//!           4 begins a new transfer (by BeginTransfer) <br>
//!           5 recreates TransferReader then begins a new transfer <br>
  Standard_EXPORT     void InitTransferReader(const Standard_Integer mode) ;
  //! Sets a Transfer Reader, which manages transfers on reading <br>
  Standard_EXPORT     void SetTransferReader(const Handle(XSControl_TransferReader)& TR) ;
  //! Returns the Transfer Reader, Null if not set <br>
  Standard_EXPORT     Handle_XSControl_TransferReader TransferReader() const;
  //! Returns the TransientProcess(internal data for TransferReader) <br>
  Standard_EXPORT     Handle_Transfer_TransientProcess MapReader() const;
  //! Changes the Map Reader, i.e. considers that the new one <br>
//!           defines the relevant read results (forgets the former ones) <br>
//!           Returns True when done, False in case of bad definition, i.e. <br>
//!           if Model from TP differs from that of Session <br>
  Standard_EXPORT     Standard_Boolean SetMapReader(const Handle(Transfer_TransientProcess)& TP) ;
  //! Returns the result attached to a starting entity <br>
//!           If <mode> = 0, returns Final Result <br>
//!           If <mode> = 1, considers Last Result <br>
//!           If <mode> = 2, considers Final, else if absent, Last <br>
//!             returns it as Transient, if result is not transient returns <br>
//!             the Binder <br>
//!           <mode> = 10,11,12 idem but returns the Binder itself <br>
//!             (if it is not, e.g. Shape, returns the Binder) <br>
//!           <mode> = 20, returns the ResultFromModel <br>
  Standard_EXPORT     Handle_Standard_Transient Result(const Handle(Standard_Transient)& ent,const Standard_Integer mode) const;
  //! Commands the transfer of, either one entity, or a list <br>
//!           I.E. calls the TransferReader after having analysed <ents> <br>
//!           It is cumulated from the last BeginTransfer <br>
//!           <ents> is processed by GiveList, hence : <br>
//!           - <ents> a Selection : its SelectionResult <br>
//!           - <ents> a HSequenceOfTransient : this list <br>
//!           - <ents> the Model : in this specific case, all the roots, <br>
//!             with no cumulation of former transfers (TransferReadRoots) <br>
  Standard_EXPORT     Standard_Integer TransferReadOne(const Handle(Standard_Transient)& ents) ;
  //! Commands the transfer of all the root entities of the model <br>
//!           i.e. calls TransferRoot from the TransferReader with the Graph <br>
//!           No cumulation with former calls to TransferReadOne <br>
  Standard_EXPORT     Standard_Integer TransferReadRoots() ;
  //! produces and returns a new Model well conditionned <br>
//!           It is produced by the Norm Controller <br>
//!           It can be Null (if this function is not implemented) <br>
  Standard_EXPORT     Handle_Interface_InterfaceModel NewModel() ;
  //! Returns the Transfer Reader, Null if not set <br>
  Standard_EXPORT     Handle_XSControl_TransferWriter TransferWriter() const;
  //! Returns the FinderProcess (internal data for TransferWriter) <br>
  Standard_EXPORT     Handle_Transfer_FinderProcess MapWriter() const;
  //! Changes the Map Reader, i.e. considers that the new one <br>
//!           defines the relevant read results (forgets the former ones) <br>
//!           Returns True when done, False if <FP> is Null <br>
  Standard_EXPORT     Standard_Boolean SetMapWriter(const Handle(Transfer_FinderProcess)& FP) ;
  //! Sets a mode to transfer Shapes from CasCade to entities of the <br>
//!           current norm, which interprets it (see various Controllers) <br>
//!           This call form could be later replaced by a more general one <br>
  Standard_EXPORT     void SetModeWriteShape(const Standard_Integer mode) ;
  //! Records the current Mode to Write Shapes <br>
  Standard_EXPORT     Standard_Integer ModeWriteShape() const;
  //! Transfers a Shape from CasCade to a model of current norm, <br>
//!           according to the last call to SetModeWriteShape <br>
//!           Returns status :Done if OK, Fail if error during transfer, <br>
//!             Error if transfer badly initialised <br>
  Standard_EXPORT     IFSelect_ReturnStatus TransferWriteShape(const TopoDS_Shape& shape,const Standard_Boolean compgraph = Standard_True) ;
  //! Returns the check-list of last transfer (write) <br>
//!           It is recorded in the FinderProcess, but it must be bound with <br>
//!           resulting entities (in the resulting file model) rather than <br>
//!           with original objects (in fact, their mappers) <br>
  Standard_EXPORT     Interface_CheckIterator TransferWriteCheckList() const;
  
  Standard_EXPORT     Handle_XSControl_Vars Vars() const;
  
  Standard_EXPORT     void SetVars(const Handle(XSControl_Vars)& newvars) ;
  //! Clears binders <br>
  Standard_EXPORT     void ClearBinders() ;
  
  Standard_EXPORT     void Destroy() ;
~XSControl_WorkSession()
{
  Destroy();
}

  DEFINE_STANDARD_RTTI(XSControl_WorkSession)

private:

Handle(XSControl_Controller) theController;
Handle(XSControl_TransferReader) theTransferRead;
Handle(XSControl_TransferWriter) theTransferWrite;
Handle(Dico_DictionaryOfTransient) theContext;
Standard_Integer theModeWriteShape;
Handle(XSControl_Vars) theVars;

};
#endif
