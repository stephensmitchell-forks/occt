// Created on: 1995-02-20
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

#ifndef _STEPControl_Controller_HeaderFile
#define _STEPControl_Controller_HeaderFile

#include <XSControl_Controller.hxx>

//! defines basic controller for STEP processor
class STEPControl_Controller : public XSControl_Controller
{
 public:
  
  //! Reads a STEP File and returns a STEP Model (into <mod>),
  //! or lets <mod> "Null" in case of Error
  //! Returns 0 if OK, 1 if Read Error, -1 if File not opened
  Standard_EXPORT virtual Standard_Integer ReadFile (const Standard_CString theFileName, Handle(Interface_InterfaceModel)& theModel) const Standard_OVERRIDE;
  
  //! Writes a File from a STEP Model
  //! Returns False (and writes no file) if <ctx> does not bring a
  //! STEP Model
  Standard_EXPORT virtual Standard_Boolean WriteFile (const Standard_CString theFileName, const Handle(Interface_InterfaceModel)& theModel, Interface_CheckIterator& theChecks) const Standard_OVERRIDE;
  
  //! Dumps an entity under STEP form, i.e. as a part of a Step file
  //! Works with a StepDumper.
  //! Level 0 just displays type; level 1 displays the entity itself
  //! and level 2 displays the entity plus its shared ones (one
  //! sub-level : immediately shared entities)
  Standard_EXPORT virtual void DumpEntity (const Handle(Interface_InterfaceModel)& model, const Handle(Interface_Protocol)& protocol, const Handle(Standard_Transient)& entity, const Handle(Message_Messenger)& S, const Standard_Integer level) const Standard_OVERRIDE;
  
  //! Creates a new empty Model ready to receive data of the Norm.
  //! It is taken from STEP Template Model
  Standard_EXPORT virtual Handle(Interface_InterfaceModel) NewModel() const Standard_OVERRIDE;
  
  //! Returns the Actor for Write attached to the pair (norm,appli)
  Standard_EXPORT virtual Handle(Transfer_ActorOfFinderProcess) NewActorWrite() const Standard_OVERRIDE;
  
  //! Standard Initialisation. It creates a Controller for STEP
  //! and records it to various names, available to select it later
  //! Returns True when done, False if could not be done
  Standard_EXPORT static void Init();

  DEFINE_STANDARD_RTTIEXT(STEPControl_Controller,XSControl_Controller)

 protected:

  //! Initializes the use of STEP Norm (the first time)
  Standard_EXPORT STEPControl_Controller();
};

#endif // _STEPControl_Controller_HeaderFile
