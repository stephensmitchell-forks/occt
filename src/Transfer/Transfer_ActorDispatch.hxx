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

#ifndef _Transfer_ActorDispatch_HeaderFile
#define _Transfer_ActorDispatch_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineHandle.hxx>
#include <Handle_Transfer_ActorDispatch.hxx>
#include <Transfer_TransferDispatch.hxx>
#include <Transfer_ActorOfTransientProcess.hxx>
#include <Handle_Interface_InterfaceModel.hxx>
#include <Interface_GeneralLib.hxx>
#include <Handle_Interface_Protocol.hxx>
#include <Transfer_ActorOfTransientProcess.hxx>
#include <Handle_Transfer_Binder.hxx>
#include <Handle_Standard_Transient.hxx>
#include <Transfer_TransientProcess.hxx>

class Interface_InterfaceError;
class Interface_InterfaceModel;
class Interface_Protocol;
class Transfer_ActorOfTransientProcess;
class Transfer_TransferDispatch;
class Transfer_Binder;
class Standard_Transient;
class Transfer_TransientProcess;
class Handle(Standard_Type);
class Handle(Transfer_ActorOfTransientProcess);

DEFINE_STANDARD_HANDLE(Transfer_ActorDispatch,Transfer_ActorOfTransientProcess)

//! This class allows to work with a TransferDispatch, i.e. to <br>
//!          transfer entities from a data set to another one defined by <br>
//!          the same interface norm, with the following features : <br>
//!          - ActorDispatch itself acts as a default actor, i.e. it copies <br>
//!            entities with the general service Copy, as CopyTool does <br>
//!          - it allows to add other actors for specific ways of transfer, <br>
//!            which may include data modifications, conversions ... <br>
//!          - and other features from TransferDispatch (such as mapping <br>
//!            other than one-one) <br>
class Transfer_ActorDispatch : public Transfer_ActorOfTransientProcess {

public:

  //! Creates an ActorDispatch from a Model. Works with a General <br>
  //!           Service Library, given as an Argument <br>
  //!           This causes TransferDispatch and its TransientProcess to be <br>
  //!           created, with default actor <me> <br>
  Standard_EXPORT   Transfer_ActorDispatch(const Handle(Interface_InterfaceModel)& amodel,const Interface_GeneralLib& lib);
  
  //! Same as above, but Library is defined through a Protocol <br>
  Standard_EXPORT   Transfer_ActorDispatch(const Handle(Interface_InterfaceModel)& amodel,
                                           const Handle(Interface_Protocol)& protocol);
                                           
  //! Same as above, but works with the Active Protocol <br>
  Standard_EXPORT   Transfer_ActorDispatch(const Handle(Interface_InterfaceModel)& amodel);
  
  //! Utility which adds an actor to the default <me> (it calls <br>
  //!           SetActor from the TransientProcess) <br>
  Standard_EXPORT     void AddActor(const Handle(Transfer_ActorOfTransientProcess)& actor);
  
  //! Returns the TransferDispatch, which does the work, records <br>
  //!           the intermediate data, etc... <br>
  //!           See TransferDispatch & CopyTool, to see the available methods <br>
  Standard_EXPORT     Transfer_TransferDispatch& TransferDispatch();
  
  //! Specific action : it calls the method Transfer from CopyTool <br>
  //!           i.e. the general service Copy, then returns the Binder <br>
  //!           produced by the TransientProcess <br>
  Standard_EXPORT   virtual  Handle_Transfer_Binder Transfer(const Handle(Standard_Transient)& start,
                                                             const Handle(Transfer_TransientProcess)& TP) ;


  DEFINE_STANDARD_RTTI(Transfer_ActorDispatch)
  
private: 
  Transfer_TransferDispatch thetool;
};
#endif
