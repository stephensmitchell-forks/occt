// Created on: 1996-03-26
// Created by: Christian CAILLET
// Copyright (c) 1996-1999 Matra Datavision
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

#ifndef _XSSelect_SelectForTransfer_HeaderFile
#define _XSSelect_SelectForTransfer_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <IFSelect_SelectExtract.hxx>
class Transfer_TransientProcess;
class Interface_InterfaceModel;
class TCollection_AsciiString;


class XSSelect_SelectForTransfer;
DEFINE_STANDARD_HANDLE(XSSelect_SelectForTransfer, IFSelect_SelectExtract)

//! This selection selects the entities which are recognised for
//! transfer by an Actor for Read : current one or another one.
//!
//! An Actor is an operator which runs transfers from interface
//! entities to objects for Imagine. It has a method to recognize
//! the entities it can process (by default, it recognises all,
//! this method can be redefined).
//!
//! A TransferProcess brings an Actor, according to the currently
//! selected norm and transfer conditions.
class XSSelect_SelectForTransfer : public IFSelect_SelectExtract
{
 public:

  //! Creates a SelectForTransfer, which will work with the
  //! currently defined Actor brought by the TransferProcess
  XSSelect_SelectForTransfer(const Handle(Transfer_TransientProcess)& TP) : theTP(TP) {}
  
  //! Returns True for an Entity which is recognized by the Actor,
  //! either the precised one, or the one defined by TransferProcess
  Standard_EXPORT Standard_Boolean Sort (const Standard_Integer rank, const Handle(Standard_Transient)& ent, const Handle(Interface_InterfaceModel)& model) const Standard_OVERRIDE;
  
  //! Returns a text defining the criterium : "Recognized for Transfer [(current actor)]"
  Standard_EXPORT TCollection_AsciiString ExtractLabel() const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(XSSelect_SelectForTransfer,IFSelect_SelectExtract)

 protected:

  Handle(Transfer_TransientProcess) theTP;
};

#endif // _XSSelect_SelectForTransfer_HeaderFile
