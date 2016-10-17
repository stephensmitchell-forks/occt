// Created on: 1997-05-14
// Created by: Christian CAILLET
// Copyright (c) 1997-1999 Matra Datavision
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

#ifndef _XSControl_Writer_HeaderFile
#define _XSControl_Writer_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Interface_ReturnStatus.hxx>
class TopoDS_Shape;
class XSControl_WorkSession;


//! This class gives a simple way to create then write a
//! Model compliant to a given norm, from a Shape
//! The model can then be edited by tools by other appropriate tools
class XSControl_Writer 
{
 public:

  DEFINE_STANDARD_ALLOC

  //! Creates a Writer from session
  XSControl_Writer(const Handle(XSControl_WorkSession) &WS) : myWS(WS) {}
  
  //! Returns the session used in <me>
  const Handle(XSControl_WorkSession) & WS() const { return myWS; }
  
  //! Transfers a shape using the owned worksession
  Standard_EXPORT Interface_ReturnStatus TransferShape (const TopoDS_Shape& theShape, const Standard_Boolean theCompGraph = Standard_True);
  
  //! Writes the produced model
  Standard_EXPORT Interface_ReturnStatus WriteFile (const Standard_CString filename);

 protected:

  Handle(XSControl_WorkSession) myWS;
};

#endif // _XSControl_Writer_HeaderFile
