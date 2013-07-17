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

#ifndef _Transfer_TransferOutput_HeaderFile
#define _Transfer_TransferOutput_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Macro.hxx>
#include <Transfer_TransientProcess.hxx>
#include <Handle_Interface_InterfaceModel.hxx>
#include <Transfer_ActorOfTransientProcess.hxx>
#include <Handle_Standard_Transient.hxx>
#include <Handle_Interface_Protocol.hxx>
#include <Standard_Boolean.hxx>

class Transfer_TransientProcess;
class Interface_InterfaceModel;
class Standard_NoSuchObject;
class Transfer_TransferFailure;
class Transfer_ActorOfTransientProcess;
class Standard_Transient;
class Interface_Protocol;
class Interface_Graph;
class Interface_EntityIterator;

//! A TransferOutput is a Tool which manages the transfer of <br>
//!           entities created by an Interface, stored in an InterfaceModel, <br>
//!           into a set of Objects suitable for an Application <br>
//!           Objects to be transferred are given, by method Transfer <br>
//!           (which calls Transfer from TransientProcess) <br>
//!           A default action is available to get all roots of the Model <br>
//!           Result is given as a TransferIterator (see TransferProcess) <br>
//!           Also, it is possible to pilot directly the TransientProcess <br>
class Transfer_TransferOutput
{
public:

  DEFINE_STANDARD_ALLOC

  //! Creates a TransferOutput ready to use, with a TransientProcess <br>
  Standard_EXPORT   Transfer_TransferOutput(const Handle(Transfer_ActorOfTransientProcess)& actor,
                                            const Handle(Interface_InterfaceModel)& amodel);
                                            
  //! Creates a TransferOutput from an already existing <br>
  //!           TransientProcess, and a Model <br>//! Returns (by Reference, hence can be changed) the Mode for <br>
  //!           Scope Management. False (D) means Scope is ignored. <br>
  //!           True means that each individual Transfer (direct or through <br>
  //!           TransferRoots) is regarded as one Scope <br>
  Standard_EXPORT   Transfer_TransferOutput(const Handle(Transfer_TransientProcess)& proc,
                                            const Handle(Interface_InterfaceModel)& amodel);
  
  //! Returns the Starting Model <br>
  Standard_EXPORT     Handle_Interface_InterfaceModel Model() const;
  
  //! Returns the TransientProcess used to work <br>
  Standard_EXPORT     Handle_Transfer_TransientProcess TransientProcess() const;
  
  //! Transfer checks that all taken Entities come from the same <br>
  //!           Model, then calls Transfer from TransientProcess <br>
  Standard_EXPORT     void Transfer(const Handle(Standard_Transient)& obj);
  
  //! Runs transfer on the roots of the Interface Model <br>
  //!           The Roots are computed with a ShareFlags created from a <br>
  //!           Protocol given as Argument <br>
  Standard_EXPORT     void TransferRoots(const Handle(Interface_Protocol)& protocol);
  
  //! Runs transfer on the roots defined by a Graph of dependences <br>
  //!           (which detains also a Model and its Entities) <br>
  //!           Roots are computed with a ShareFlags created from the Graph <br>
  Standard_EXPORT     void TransferRoots(const Interface_Graph& G);
  
  //! Runs transfer on the roots of the Interface Model <br>
  //!           Remark : the Roots are computed with a ShareFlags created <br>
  //!           from the Active Protocol <br>
  Standard_EXPORT     void TransferRoots();
  
  //! Returns the list of Starting Entities with these criteria : <br>
  //!         - <normal> False, gives the entities bound with ABNORMAL STATUS <br>
  //!           (e.g. : Fail recorded, Exception raised during Transfer) <br>
  //!         - <normal> True, gives Entities with or without a Result, but <br>
  //!           with no Fail, no Exception (Warnings are not counted) <br>
  //!         - <roots> False, considers all entities recorded (either for <br>
  //!           Result, or for at least one Fail or Warning message) <br>
  //!         - <roots> True (Default), considers only roots of Transfer <br>
  //!           (the Entities recorded at highest level) <br>
  //!          This method is based on AbnormalResult from TransferProcess <br>
  Standard_EXPORT     Interface_EntityIterator ListForStatus (const Standard_Boolean normal,
                                                             const Standard_Boolean roots = Standard_True) const;
  //! Fills a Model with the list determined by ListForStatus <br>
  //!           This model starts from scratch (made by NewEmptyModel from the <br>
  //!           current Model), then is filled by AddWithRefs <br>
  //! <br>
  //!           Useful to get separately from a transfer, the entities which <br>
  //!           have caused problem, in order to furtherly analyse them (with <br>
  //!           normal = False), or the "good" entities, to obtain a data set <br>
  //!           "which works well" (with normal = True) <br>
  Standard_EXPORT     Handle_Interface_InterfaceModel ModelForStatus (const Handle(Interface_Protocol)& protocol,
                                                                     const Standard_Boolean normal,
                                                                     const Standard_Boolean roots = Standard_True) const;

private:

Handle_Transfer_TransientProcess theproc;
Handle_Interface_InterfaceModel themodel;


};
#endif
