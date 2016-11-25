// Created on: 1993-02-04
// Created by: Christian CAILLET
// Copyright (c) 1993-1999 Matra Datavision
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

#ifndef _Interface_FileReaderTool_HeaderFile
#define _Interface_FileReaderTool_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>
#include <TColStd_HArray1OfTransient.hxx>
class Interface_Protocol;
class Interface_FileReaderData;
class Interface_InterfaceModel;
class Message_Messenger;
class Standard_DomainError;
class Standard_NoSuchObject;
class Interface_Check;
class Standard_Transient;
class Interface_GeneralLib;
class Interface_ReaderLib;


//! Defines services which are required to load an InterfaceModel
//! from a File. Typically, it may firstly transform a system
//! file into a FileReaderData object, then work on it, not longer
//! considering file contents, to load an Interface Model.
//! It may also work on a FileReaderData already loaded.
//!
//! FileReaderTool provides, on one hand, some general services
//! which are common to all read operations but can be redefined,
//! plus general actions to be performed specifically for each
//! Norm, as deferred methods to define.
//!
//! In particular, FileReaderTool defines the Interface's Unknown
//! and Error entities
class Interface_FileReaderTool 
{
public:

  DEFINE_STANDARD_ALLOC

  //! Sets Data to a FileReaderData. Works with a Protocol
  void SetData (const Handle(Interface_FileReaderData)& reader, const Handle(Interface_Protocol)& protocol)
  {
    thereader = reader;
    theproto = protocol;
  }
  
  //! Returns the Protocol given at creation time
  const Handle(Interface_Protocol) & Protocol() const { return theproto; }
  
  //! Returns the FileReaderData which is used to work
  const Handle(Interface_FileReaderData) & Data() const { return thereader; }
  
  //! Stores a Model. Used when the Model has been loaded
  void SetModel (const Handle(Interface_InterfaceModel)& amodel) { themodel = amodel; }
  
  //! Returns the stored Model
  const Handle(Interface_InterfaceModel) & Model() const { return themodel; }
  
  //! Sets Messenger used for outputting messages
  Standard_EXPORT void SetMessenger (const Handle(Message_Messenger)& messenger);
  
  //! Returns Messenger used for outputting messages.
  //! The returned object is guaranteed to be non-null;
  //! default is Message::Messenger().
  const Handle(Message_Messenger) & Messenger() const { return themessenger; }
  
  //! Sets trace level used for outputting messages
  //! - 0: no trace at all
  //! - 1: errors
  //! - 2: errors and warnings
  //! - 3: all messages
  //! Default is 1 : Errors traced
  void SetTraceLevel (const Standard_Integer tracelev) { thetrace = tracelev; }
  
  //! Returns trace level used for outputting messages.
  Standard_Integer TraceLevel() const { return thetrace; }
  
  //! Fills records with empty entities; once done, each entity can
  //! ask the FileReaderTool for any entity referenced through an
  //! identifier. Calls Recognize which is specific to each specific
  //! type of FileReaderTool
  Standard_EXPORT void SetEntities();
  
  //! Reads and fills Entities from the FileReaderData set by
  //! SetData to an InterfaceModel.
  //! It enchains required operations, the specific ones correspond
  //! to deferred methods (below) to be defined for each Norm.
  //! It manages also error recovery and trace.
  //! Remark : it calls SetModel.
  //! It Can raise any error which can occur during a load
  //! operation, unless Error Handling is set.
  //! This method can also be redefined if judged necessary.
  Standard_EXPORT void LoadModel (const Handle(Interface_InterfaceModel)& amodel);
  
  //! Reads, Fills and Returns one Entity read from a Record of the
  //! FileReaderData. This Method manages also case of Fail or
  //! Warning, by producing a ReportEntyty plus , for a Fail, a
  //! literal Content (as an UnknownEntity). Performs also Trace
  Standard_EXPORT Handle(Standard_Transient) LoadedEntity (const Standard_Integer num);

 protected:

  //! Constructor; sets default fields
  Standard_EXPORT Interface_FileReaderTool();
  
  //! Recognizes a record with the help of Libraries. Can be used
  //! to implement the method Recognize.
  //! <rlib> is used to find Protocol and CaseNumber to apply
  //! <glib> performs the creation (by service NewVoid, or NewRead
  //! if NewVoid gave no result)
  //! <ach> is a check, which is transmitted to NewRead if it is
  //! called, gives a result but which is false
  //! <ent> is the result
  //! Returns False if recognition has failed, True else
  Standard_EXPORT Standard_Boolean RecognizeByLib (const Standard_Integer num, Interface_GeneralLib& glib, Interface_ReaderLib& rlib, Handle(Interface_Check)& ach, Handle(Standard_Transient)& ent) const;
  
  //! Recognizes a record, given its number. Specific to each
  //! Interface; called by SetEntities. It can call the basic method
  //! RecognizeByLib.
  //! Returns False if recognition has failed, True else.
  //! <ach> has not to be filled if simply Recognition has failed :
  //! it must record true error messages : RecognizeByLib can
  //! generate error messages if NewRead is called
  //!
  //! Note that it works thru a Recognizer (method Evaluate) which
  //! has to be memorized before starting
  Standard_EXPORT virtual Standard_Boolean Recognize (const Standard_Integer num, Handle(Interface_Check)& ach, Handle(Standard_Transient)& ent) = 0;
  
  //! Fills model's header; each Interface defines for its Model its
  //! own file header; this method fills it from FileReaderTool.+
  //! It is called by AnalyseFile from InterfaceModel
  Standard_EXPORT virtual void BeginRead (const Handle(Interface_InterfaceModel)& amodel) = 0;
  
  //! Fills an Entity, given record no; specific to each Interface,
  //! called by AnalyseFile from InterfaceModel (which manages its
  //! calling arguments)
  //! To work, each Interface can define a method in its proper
  //! Transient class, like this (given as an example) :
  //! AnalyseRecord (me  : mutable; FR     : in out FileReaderTool;
  //! num : Integer; acheck : in out Check)
  //! returns Boolean;
  //! and call it from AnalyseRecord
  //!
  //! Returned Value : True if the entity could be loaded, False
  //! else (in case of syntactic fail)
  Standard_EXPORT virtual Standard_Boolean AnalyseRecord (const Standard_Integer num, const Handle(Standard_Transient)& anent, Handle(Interface_Check)& acheck) = 0;
  
  //! Ends file reading after reading all the entities
  //! default is doing nothing; redefinable as necessary
  Standard_EXPORT virtual void EndRead (const Handle(Interface_InterfaceModel)& amodel) = 0;

 private:

  Handle(Interface_Protocol) theproto;
  Handle(Interface_FileReaderData) thereader;
  Handle(Interface_InterfaceModel) themodel;
  Handle(Message_Messenger) themessenger;
  Standard_Integer thetrace;
  Standard_Integer thenbrep0;
  Standard_Integer thenbreps;
  Handle(TColStd_HArray1OfTransient) thereports;
};

#endif // _Interface_FileReaderTool_HeaderFile
