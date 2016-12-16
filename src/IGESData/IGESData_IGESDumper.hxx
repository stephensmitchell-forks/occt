// Created on: 1993-09-06
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

#ifndef _IGESData_IGESDumper_HeaderFile
#define _IGESData_IGESDumper_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

class IGESData_IGESModel;
class IGESData_IGESEntity;
class Message_Messenger;


//! Provides a way to obtain a clear Dump of an IGESEntity
//! (distinct from normalized output). It works with tools
//! attached to Entities, as for normalized Reade and Write
//!
//! For each Entity, displaying data is splitted in own data
//! (specific to each type) and other attached data, which are
//! defined for all IGES Types (either from "Directory Entry" or
//! from Lists of Associativities and Properties)
class IGESData_IGESDumper 
{
 public:

  DEFINE_STANDARD_ALLOC
  
  //! Returns an IGESDumper ready to work. The IGESModel provides
  //! the numbering of Entities : as for any InterfaceModel, it
  //! gives each Entity a number; but for IGESEntities, the "Number
  //! of Directory Entry" according to the definition of IGES Files,
  //! is also usefull
  IGESData_IGESDumper (const Handle(IGESData_IGESModel)& model) : themodel(model) {}

  //! Prints onto an output, the "Number of Directory Entry" which
  //! corresponds to an IGESEntity in the IGESModel, under the form
  //! "D#nnn" (a Null Handle gives D#0)
  Standard_EXPORT void PrintDNum (const Handle(IGESData_IGESEntity)& ent, const Handle(Message_Messenger)& S) const;
  
  //! Prints onto an output, the "Number of Directory Entry" (see
  //! PrintDNum) plus IGES Type and Form Numbers, which gives
  //! "D#nnn  Type nnn  Form nnn"
  Standard_EXPORT void PrintShort (const Handle(IGESData_IGESEntity)& ent, const Handle(Message_Messenger)& S) const;
  
  Standard_EXPORT void Dump (const Handle(IGESData_IGESEntity)& ent, const Handle(Message_Messenger)& S, const Standard_Integer own, const Standard_Integer attached = -1) const;

 private:

  Handle(IGESData_IGESModel) themodel;
};

#endif // _IGESData_IGESDumper_HeaderFile
