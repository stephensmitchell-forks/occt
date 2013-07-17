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

#ifndef _Transfer_DispatchControl_HeaderFile
#define _Transfer_DispatchControl_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineHandle.hxx>
#include <Transfer_TransientProcess_Handle.hxx>
#include <Handle_Interface_InterfaceModel.hxx>
#include <Interface_CopyControl.hxx>
#include <Handle_Standard_Transient.hxx>
#include <Standard_Boolean.hxx>
class Transfer_TransientProcess;
class Interface_InterfaceModel;
class Interface_InterfaceError;
class Standard_Transient;
class Handle(Standard_Type);
class Handle(Interface_CopyControl);
class Transfer_DispatchControl;

DEFINE_STANDARD_HANDLE(Transfer_DispatchControl,Interface_CopyControl)


//! This is an auxiliary class for TransferDispatch, which allows <br>
//!           to record simple copies, as CopyControl from Interface, but <br>
//!           based on a TransientProcess. Hence, it allows in addition <br>
//!           more actions (such as recording results of adaptations) <br>
class Transfer_DispatchControl : public Interface_CopyControl
{

public:

  //! Creates the DispatchControl, ready for use <br>
  Standard_EXPORT   Transfer_DispatchControl(const Handle(Interface_InterfaceModel)& model,const Handle(Transfer_TransientProcess)& TP);
  //! Returns the content of the DispatchControl : it can be used <br>
  //!           for a direct call, if the basic methods do not suffice <br>
  Standard_EXPORT    const Handle_Transfer_TransientProcess& TransientProcess() const;
  //! Returns the Model from which the transfer is to be done <br>
  Standard_EXPORT    const Handle_Interface_InterfaceModel& StartingModel() const;
  //! Clears the List of Copied Results <br>
  Standard_EXPORT     void Clear() ;
  //! Binds a (Transient) Result to a (Transient) Starting Entity <br>
  Standard_EXPORT     void Bind(const Handle(Standard_Transient)& ent,const Handle(Standard_Transient)& res) ;
  //! Searches for the Result bound to a Starting Entity <br>
  //!           If Found, returns True and fills <res> <br>
  //!           Else, returns False and nullifies <res> <br>
  Standard_EXPORT     Standard_Boolean Search(const Handle(Standard_Transient)& ent,Handle(Standard_Transient)& res) const;

  DEFINE_STANDARD_RTTI(Transfer_DispatchControl)

private: 

Handle_Transfer_TransientProcess theTP;
Handle_Interface_InterfaceModel themodel;
};
#endif
