// Created on: 1995-03-15
// Created by: Christian CAILLET
// Copyright (c) 1995-1999 Matra Datavision
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

#ifndef _IGESControl_Controller_HeaderFile
#define _IGESControl_Controller_HeaderFile

#include <XSControl_Controller.hxx>
class IGESData_Protocol;

//! Controller for IGES-5.1
class IGESControl_Controller : public XSControl_Controller
{
 public:
  
  //! Reads a IGES File and returns a IGES Model (into <mod>),
  //! or lets <mod> "Null" in case of Error
  //! Returns 0 if OK, 1 if Read Error, -1 if File not opened
  Standard_EXPORT virtual Standard_Integer ReadFile (const Standard_CString theFileName, Handle(Interface_InterfaceModel)& theModel) const Standard_OVERRIDE;
  
  //! Writes a File from a IGES Model (brought by <ctx>)
  //! Returns False (and writes no file) if <ctx> is not for IGES
  Standard_EXPORT virtual Standard_Boolean WriteFile (const Standard_CString theFileName, const Handle(Interface_InterfaceModel)& theModel, Interface_CheckIterator& theChecks) const Standard_OVERRIDE;
  
  //! Dumps an IGES Entity with an IGES Dumper. <level> is the one used by IGESDumper.
  Standard_EXPORT virtual void DumpEntity (const Handle(Interface_InterfaceModel)& model, const Handle(Interface_Protocol)& protocol, const Handle(Standard_Transient)& entity, const Handle(Message_Messenger)& S, const Standard_Integer level) const Standard_OVERRIDE;
  
  //! Creates a new empty Model ready to receive data of the Norm.
  //! It is taken from IGES Template Model
  Standard_EXPORT virtual Handle(Interface_InterfaceModel) NewModel() const Standard_OVERRIDE;
  
  //! Returns the Actor for Read attached to the pair (norm,appli)
  //! It is an Actor from IGESToBRep, adapted from an IGESModel :
  //! Unit, tolerances
  Standard_EXPORT virtual Handle(Transfer_ActorOfTransientProcess) ActorRead (const Handle(Interface_InterfaceModel)& model) const Standard_OVERRIDE;
  
  //! Returns the Actor for Write attached to the pair (norm,appli)
  Standard_EXPORT virtual Handle(Transfer_ActorOfFinderProcess) NewActorWrite() const Standard_OVERRIDE;
  
  //! Standard Initialisation. It creates a Controller for IGES and
  //! records it to various names, available to select it later
  //! Returns True when done, False if could not be done
  //! Also, it creates and records an Adaptor for FNES
  Standard_EXPORT static void Init();
  
  //! Defines a protocol to be adequate for IGES
  //! (encompasses ALL the IGES norm including IGESSolid, IGESAppli)
  Standard_EXPORT static const Handle(IGESData_Protocol) & DefineProtocol();

  DEFINE_STANDARD_RTTIEXT(IGESControl_Controller,XSControl_Controller)

 protected:

  //! Initializes the use of IGES Norm (the first time) and returns
  //! a Controller for IGES-5.1
  //! If <modefnes> is True, sets it to internal FNES format
  Standard_EXPORT IGESControl_Controller(const Standard_Boolean modefnes = Standard_False);

  Standard_Boolean themodefnes;
};

#endif // _IGESControl_Controller_HeaderFile
