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

#ifndef _IGESControl_Controller_HeaderFile
#define _IGESControl_Controller_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineHandle.hxx>
#include <Standard_Boolean.hxx>
#include <XSControl_Controller.hxx>
#include <Handle_Interface_InterfaceModel.hxx>
#include <Transfer_ActorOfTransientProcess.hxx>
#include <IFSelect_ReturnStatus.hxx>
#include <Transfer_FinderProcess.hxx>
#include <Standard_Integer.hxx>
#include <XSControl_WorkSession.hxx>

class Interface_InterfaceModel;
class Transfer_ActorOfTransientProcess;
class TopoDS_Shape;
class Transfer_FinderProcess;
class XSControl_WorkSession;
class Standard_Transient;
class Handle(Standard_Type);
class Handle(XSControl_Controller);
class IGESControl_Controller;

DEFINE_STANDARD_HANDLE(IGESControl_Controller,XSControl_Controller)

//! Controller for IGES-5.1 <br>
class IGESControl_Controller : public XSControl_Controller {

public:

  //! Initializes the use of IGES Norm (the first time) and returns <br>
//!           a Controller for IGES-5.1 <br>
//!           If <modefnes> is True, sets it to internal FNES format <br>
  Standard_EXPORT   IGESControl_Controller(const Standard_Boolean modefnes = Standard_False);
  //! Creates a new empty Model ready to receive data of the Norm. <br>
//!           It is taken from IGES Template Model <br>
  Standard_EXPORT     Handle_Interface_InterfaceModel NewModel() const;
  //! Returns the Actor for Read attached to the pair (norm,appli) <br>
//!           It is an Actor from IGESToBRep, adapted from an IGESModel : <br>
//!           Unit, tolerances <br>
  Standard_EXPORT     Handle_Transfer_ActorOfTransientProcess ActorRead(const Handle(Interface_InterfaceModel)& model) const;
  //! Takes one Shape and transfers it to the InterfaceModel <br>
//!           (already created by NewModel for instance) <br>
//!           <modetrans> is to be interpreted by each kind of XstepAdaptor <br>
//!           Returns a status : 0 OK  1 No result  2 Fail  -1 bad modeshape <br>
//!               -2 bad model (requires an IGESModel) <br>
//!           modeshape : 0  groupe of face (version < 5.1) <br>
//!                       1  BREP-version 5.1 of IGES <br>
  Standard_EXPORT   virtual  IFSelect_ReturnStatus TransferWriteShape(const TopoDS_Shape& shape,const Handle(Transfer_FinderProcess)& FP,const Handle(Interface_InterfaceModel)& model,const Standard_Integer modetrans = 0) const;
  //! Standard Initialisation. It creates a Controller for IGES and <br>
//!           records it to various names, available to select it later <br>
//!           Returns True when done, False if could not be done <br>
//!           Also, it creates and records an Adaptor for FNES <br>
  Standard_EXPORT   static  Standard_Boolean Init() ;
  
  Standard_EXPORT   virtual  void Customise(Handle(XSControl_WorkSession)& WS) ;




  DEFINE_STANDARD_RTTI(IGESControl_Controller)

protected:




private: 


Standard_Boolean themode;


};





// other Inline functions and methods (like "C++: function call" methods)


#endif
