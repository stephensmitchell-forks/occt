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

#ifndef _Transfer_TransferDispatch_HeaderFile
#define _Transfer_TransferDispatch_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Macro.hxx>
#include <Interface_CopyTool.hxx>
#include <Handle_Interface_InterfaceModel.hxx>
#include <Interface_GeneralLib.hxx>
#include <Handle_Interface_Protocol.hxx>
#include <Transfer_TransientProcess.hxx>
#include <Standard_Boolean.hxx>
#include <Handle_Standard_Transient.hxx>

class Interface_InterfaceError;
class Interface_InterfaceModel;
class Interface_Protocol;
class Transfer_TransientProcess;
class Standard_Transient;


//! A TransferDispatch is aimed to dispatch Entities between two <br>
//!           Interface Models, by default by copying them, as CopyTool, but <br>
//!           with more capabilities of adapting : Copy is redefined to <br>
//!           firstly pass the hand to a TransferProcess. If this gives no <br>
//!           result, standard Copy is called. <br>
//! <br>
//!           This allow, for instance, to modify the copied Entity (such as <br>
//!           changing a Name for a VDA Entity), or to do a deeper work <br>
//!           (such as Substituting a kind of Entity to another one). <br>
//! <br>
//!           For these reasons, TransferDispatch is basically a CopyTool, <br>
//!           but uses a more sophiscated control, which is TransferProcess, <br>
//!           and its method Copy is redefined <br>
class Transfer_TransferDispatch  : public Interface_CopyTool
{
public:

  DEFINE_STANDARD_ALLOC

  //! Creates a TransferDispatch from a Model. Works with a General <br>
  //!           Service Library, given as an Argument <br>
  //!           A TransferDispatch is created as a CopyTool in which the <br>
  //!           Control is set to TransientProcess <br>
  Standard_EXPORT   Transfer_TransferDispatch(const Handle(Interface_InterfaceModel)& amodel,const Interface_GeneralLib& lib);
  //! Same as above, but Library is defined through a Protocol <br>
  Standard_EXPORT   Transfer_TransferDispatch(const Handle(Interface_InterfaceModel)& amodel,const Handle(Interface_Protocol)& protocol);
  //! Same as above, but works with the Active Protocol <br>
  Standard_EXPORT   Transfer_TransferDispatch(const Handle(Interface_InterfaceModel)& amodel);
  //! Returns the content of Control Object, as a TransientProcess <br>
  Standard_EXPORT     Handle(Transfer_TransientProcess) TransientProcess() const;
  //! Copies an Entity by calling the method Transferring from the <br>
  //!           TransferProcess. If this called produces a Null Binder, then <br>
  //!           the standard, inherited Copy is called <br>
  Standard_EXPORT   virtual  Standard_Boolean Copy (const Handle(Standard_Transient)& entfrom,
                                                    Handle(Standard_Transient)& entto,
                                                    const Standard_Boolean mapped,
                                                    const Standard_Boolean errstat) ;
};
#endif
