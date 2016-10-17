// Created on: 1995-03-13
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

#ifndef _XSControl_Controller_HeaderFile
#define _XSControl_Controller_HeaderFile

#include <MMgt_TShared.hxx>

#include <NCollection_Vector.hxx>
#include <TCollection_AsciiString.hxx>
#include <Transfer_ActorOfTransientProcess.hxx>
#include <Transfer_ActorOfFinderProcess.hxx>
class Interface_Protocol;
class Interface_InterfaceModel;
class Interface_HArray1OfHAsciiString;
class XSControl_WorkSession;

//! This class allows a general X-STEP engine to run generic
//! functions on any interface norm, in the same way. It includes
//! the transfer operations. I.e. it gathers the already available
//! general modules, the engine has just to know it
//!
//! The important point is that a given X-STEP Controller is
//! attached to a given couple made of an Interface Norm (such as
//! IGES-5.1) and an application data model (OCCT shapes, for instance).
//!
//! Finally, a controller can be strored in a general dictionary
//! then retreived later by a static method Recorded.
//!
//! It does not manage the produced data, but the Actors make the
//! link between the norm and the application
class XSControl_Controller : public MMgt_TShared
{
 public:
  
  //! Records a controller attached to its name
  Standard_EXPORT void AutoRecord () const;
  
  //! Returns the Controller attached to a given name
  //! Returns a Null Handle if <name> is unknown
  Standard_EXPORT static Handle(XSControl_Controller) Recorded (const Standard_CString theName);
  
  //! Returns a name, as given when initializing
  Standard_CString Name () const { return myName.ToCString(); }
  
  //! Returns the Protocol attached to the Norm (from field)
  const Handle(Interface_Protocol) & Protocol () const { return myAdaptorProtocol; }
  
  //! Gives the way to Read a File and transfer it to a Model
  //! <mod> is the resulting Model, which has to be created by this
  //! method. In case of error, <mod> must be returned Null
  //! Return value is a status with free values.
  //! Simply, 0 is for "Execution OK"
  //! The Protocol can be used to work (e.g. create the Model, read
  //! and recognize the Entities)
  Standard_EXPORT virtual Standard_Integer ReadFile (const Standard_CString theFileName, Handle(Interface_InterfaceModel)& theModel) const = 0;
  
  //! Gives the way to Write a File from a Model.
  //! <ctx> contains all necessary informations : the model, the
  //! protocol, the file name, and the list of File Modifiers to be
  //! applied, also with restricted list of selected entities for
  //! each one, if required.
  //! In return, it brings the produced check-list
  Standard_EXPORT virtual Standard_Boolean WriteFile (const Standard_CString theFileName, const Handle(Interface_InterfaceModel)& theModel, Interface_CheckIterator& theChecks) const = 0;
  
  //! Creates a new empty Model ready to receive data of the Norm
  //! Used to write data from Imagine to an interface file
  Standard_EXPORT virtual Handle(Interface_InterfaceModel) NewModel() const = 0;
  
  //! Returns the Actor for Read attached to the pair (norm,appli)
  //! It can be adapted for data of the input Model, as required
  //! Can be read from field then adapted with Model as required
  Standard_EXPORT virtual Handle(Transfer_ActorOfTransientProcess) ActorRead (const Handle(Interface_InterfaceModel)& model) const;
  
  //! Returns the Actor for Write attached to the pair (norm,appli)
  //! Read from field. Can be redefined
  //const Handle(Transfer_ActorOfFinderProcess) & ActorWrite() const { return myAdaptorWrite; }
  Standard_EXPORT virtual Handle(Transfer_ActorOfFinderProcess) NewActorWrite() const = 0;
  
  //! Customises a WorkSession, by adding to it the recorded items (by AddSessionItem)
  Standard_EXPORT virtual void Customise (Handle(XSControl_WorkSession)& WS);
  
  //! Gives the way of dumping an entity under a form comprehensive
  //! for each norm. <model> helps to identify, number ... entities.
  //! <level> is to be interpreted for each norm (because of the
  //! formats which can be very different)
  Standard_EXPORT virtual void DumpEntity (const Handle(Interface_InterfaceModel)& model, const Handle(Interface_Protocol)& protocol, const Handle(Standard_Transient)& entity, const Handle(Message_Messenger)& S, const Standard_Integer level) const = 0;
  
  //! Calls deferred DumpEntity with the recorded default level
  Standard_EXPORT void DumpEntity (const Handle(Interface_InterfaceModel)& model, const Handle(Interface_Protocol)& protocol, const Handle(Standard_Transient)& entity, const Handle(Message_Messenger)& S) const;
  
  //! Records a default level and a maximum value for level
  //! level for DumpEntity can go between 0 and <max>
  //! default value will be <def>
  Standard_EXPORT void SetDumpLevels (const Standard_Integer def, const Standard_Integer max);
  
  //! Returns the recorded default and maximum dump levels
  //! If none was recorded, max is returned negative, def as zero
  Standard_EXPORT void DumpLevels (Standard_Integer& def, Standard_Integer& max) const;
  
  //! Records a short line of help for a level (0 - max)
  Standard_EXPORT void SetDumpHelp (const Standard_Integer level, const Standard_CString help);
  
  //! Returns the help line recorded for <level>, or an empty string
  Standard_EXPORT Standard_CString DumpHelp (const Standard_Integer level) const;

  DEFINE_STANDARD_RTTIEXT(XSControl_Controller,MMgt_TShared)

 protected:
  
  //! Initializing with a name
  Standard_EXPORT XSControl_Controller(const Standard_CString theName);

  //! Records the name of a Static to be traced for a given use
  Standard_EXPORT void TraceStatic (const Standard_CString theName, const Standard_Integer theUse);

  Handle(Interface_Protocol) myAdaptorProtocol;
  Handle(Transfer_ActorOfTransientProcess) myAdaptorRead;

 private:

  TCollection_AsciiString myName;
  NCollection_Vector<Handle(Standard_Transient)> myParams;
  NCollection_Vector<Standard_Integer> myParamUses;
  Standard_Integer thelevdef;
  Handle(Interface_HArray1OfHAsciiString) thelevhlp;
};

#endif // _XSControl_Controller_HeaderFile
