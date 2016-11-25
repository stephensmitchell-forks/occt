// Created on: 1992-02-11
// Created by: Christian CAILLET
// Copyright (c) 1992-1999 Matra Datavision
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

#ifndef _StepData_StepReaderTool_HeaderFile
#define _StepData_StepReaderTool_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Interface_GeneralLib.hxx>
#include <Interface_ReaderLib.hxx>
#include <Interface_FileReaderTool.hxx>
class StepData_StepReaderData;
class StepData_Protocol;
class Interface_Check;
class Interface_InterfaceModel;


//! Specific FileReaderTool for Step; works with FileReaderData
//! provides references evaluation, plus access to litteral data
//! and specific methods defined by FileReaderTool
//! Remarks : works with a ReaderLib to load Entities
class StepData_StepReaderTool  : public Interface_FileReaderTool
{
 public:

  DEFINE_STANDARD_ALLOC

  //! creates StepReaderTool to work with a StepReaderData according
  //! to a Step Protocol. Defines the ReaderLib at this time
  Standard_EXPORT StepData_StepReaderTool(const Handle(StepData_StepReaderData)& reader, const Handle(StepData_Protocol)& protocol);
  
  //! Bounds empty entities to records
  //! Works only on data entities (skips header)
  Standard_EXPORT void Prepare ();
  
  //! Bounds empty entities and sub-lists to header records
  //! works like Prepare + SetEntityNumbers, but for header
  //! (N.B.: in Header, no Ident and no reference)
  Standard_EXPORT void PrepareHeader ();

 private:
  
  //! recognizes records, by asking either ReaderLib (default) or
  //! FileRecognizer (if defined) to do so. <ach> is to call
  //! RecognizeByLib
  Standard_EXPORT virtual Standard_Boolean Recognize (const Standard_Integer num, Handle(Interface_Check)& ach, Handle(Standard_Transient)& ent) Standard_OVERRIDE;
  
  //! fills model's header; that is, gives to it Header entities
  //! and commands their loading. Also fills StepModel's Global
  //! Check from StepReaderData's GlobalCheck
  Standard_EXPORT virtual void BeginRead (const Handle(Interface_InterfaceModel)& amodel) Standard_OVERRIDE;
  
  //! fills an entity, given record no; works by using a ReaderLib
  //! to load each entity, which must be a Transient
  //! Actually, returned value is True if no fail, False else
  Standard_EXPORT Standard_Boolean AnalyseRecord (const Standard_Integer num, const Handle(Standard_Transient)& anent, Handle(Interface_Check)& acheck) Standard_OVERRIDE;
  
  //! Ends file reading after reading all the entities
  //! Here, it binds in the model, Idents to Entities (for checks)
  Standard_EXPORT virtual void EndRead (const Handle(Interface_InterfaceModel)& amodel) Standard_OVERRIDE;

  Interface_GeneralLib theglib;
  Interface_ReaderLib therlib;
};

#endif // _StepData_StepReaderTool_HeaderFile
