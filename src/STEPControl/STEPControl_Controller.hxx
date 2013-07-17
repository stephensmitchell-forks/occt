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

#ifndef _STEPControl_Controller_HeaderFile
#define _STEPControl_Controller_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineHandle.hxx>
#include <XSControl_Controller.hxx>
#include <Handle_Interface_InterfaceModel.hxx>
#include <Transfer_ActorOfTransientProcess.hxx>
#include <XSControl_WorkSession.hxx>
#include <IFSelect_ReturnStatus.hxx>
#include <Transfer_FinderProcess.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>

class Interface_InterfaceModel;
class Transfer_ActorOfTransientProcess;
class XSControl_WorkSession;
class TopoDS_Shape;
class Transfer_FinderProcess;
class Standard_Transient;
class Handle(Standard_Type);
class Handle(XSControl_Controller);
class STEPControl_Controller;

DEFINE_STANDARD_HANDLE(STEPControl_Controller,XSControl_Controller)

//! defines basic controller for STEP processor <br>
class STEPControl_Controller : public XSControl_Controller {

public:

  //! Initializes the use of STEP Norm (the first time) and <br>
//!           returns a Controller <br>
  Standard_EXPORT   STEPControl_Controller();
  //! Creates a new empty Model ready to receive data of the Norm. <br>
//!           It is taken from STEP Template Model <br>
  Standard_EXPORT     Handle_Interface_InterfaceModel NewModel() const;
  //! Returns a new Actor for Read attached to the pair (norm,appli) <br>
//!           It is a PmsToTopoDSAct_Actor <br>
  Standard_EXPORT     Handle_Transfer_ActorOfTransientProcess ActorRead(const Handle(Interface_InterfaceModel)& model) const;
  
  Standard_EXPORT   virtual  void Customise(Handle(XSControl_WorkSession)& WS) ;
  //! Takes one Shape and transfers it to the InterfaceModel <br>
//!           (already created by NewModel for instance) <br>
//!           <modeshape> is to be interpreted by each kind of XstepAdaptor <br>
//!           Returns a status : 0 OK  1 No result  2 Fail  -1 bad modeshape <br>
//!               -2 bad model (requires a StepModel) <br>
//!           modeshape : 1 Facetted BRep, 2 Shell, 3 Manifold Solid <br>
  Standard_EXPORT   virtual  IFSelect_ReturnStatus TransferWriteShape(const TopoDS_Shape& shape,const Handle(Transfer_FinderProcess)& FP,const Handle(Interface_InterfaceModel)& model,const Standard_Integer modetrans = 0) const;
  //! Standard Initialisation. It creates a Controller for STEP <br>
//!           and records it to various names, available to select it later <br>
//!           Returns True when done, False if could not be done <br>
  Standard_EXPORT   static  Standard_Boolean Init() ;




  DEFINE_STANDARD_RTTI(STEPControl_Controller)

protected:




private: 




};





// other Inline functions and methods (like "C++: function call" methods)


#endif
