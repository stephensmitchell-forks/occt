// Created on: 1992-04-06
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

#ifndef _IGESData_IGESReaderTool_HeaderFile
#define _IGESData_IGESReaderTool_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Interface_FileReaderTool.hxx>
//class Interface_ParamList;
class Interface_Check;
class IGESData_IGESReaderData;
class IGESData_Protocol;
class IGESData_IGESModel;
class Standard_Transient;
class Interface_InterfaceModel;
class IGESData_IGESEntity;
class IGESData_DirPart;
class IGESData_ParamReader;


//! specific FileReaderTool for IGES
//! Parameters are accessed through specific objects, ParamReaders
class IGESData_IGESReaderTool
{
 public:

  DEFINE_STANDARD_ALLOC

  //! creates IGESReaderTool to work with an IGESReaderData and an IGES Protocol.
  IGESData_IGESReaderTool(const Handle(IGESData_Protocol)& theProtocol)
  : myProtocol(theProtocol)
  {}

  Standard_Integer EntityNumber () const { return thecnum; }

  const Handle(IGESData_IGESReaderData) & ReaderData() const { return myData; }

  Handle(Interface_Check) & Check() { return thechk; }

  Standard_Integer GradWeight() const { return thegradweight; }

  Standard_Real MaxWeight() const { return themaxweight; }
  
  Standard_EXPORT Standard_Integer Read (const Standard_CString theFileName, const Handle(IGESData_IGESModel)& amodel, const Standard_Boolean modefnes = Standard_False);

 private:

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

  Handle(IGESData_Protocol) myProtocol;
  Handle(IGESData_IGESReaderData) myData;

  Standard_Integer thecnum;
  Handle(Interface_Check) thechk;
  Standard_Integer thegradweight;
  Standard_Real themaxweight;
  Standard_Real thedefweight;
};

#endif // _IGESData_IGESReaderTool_HeaderFile
