// Created on: 1998-07-31
// Created by: Christian CAILLET
// Copyright (c) 1998-1999 Matra Datavision
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

#ifndef _XSSelect_SignTransferStatus_HeaderFile
#define _XSSelect_SignTransferStatus_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <IFSelect_Signature.hxx>
#include <TCollection_AsciiString.hxx>
class Transfer_TransientProcess;
class Interface_InterfaceModel;


class XSSelect_SignTransferStatus;
DEFINE_STANDARD_HANDLE(XSSelect_SignTransferStatus, IFSelect_Signature)

//! This Signatures gives the Transfer Status of an entity, as
//! recorded in a TransferProcess. It can be :
//! - Void : not recorded, or recorded as void with no message
//! (attributes are not taken into account)
//! - Warning : no result, warning message(s), no fail
//! - Fail : no result, fail messages (with or without warning)
//! - Result.. : result, no message (neither warning nor fail)
//! Result.. i.e. Result:TypeName of the result
//! - Result../Warning : result, with warning but no fail
//! - Result../Fail : result, with fail (.e. bad result)
//! - Fail on run : no result yet recorded, no message, but
//! an exception occurred while recording the result
//! (this should not appear and indicates a programming error)
class XSSelect_SignTransferStatus : public IFSelect_Signature
{
 public:

  //! Creates a SignTransferStatus, which will work on the current TransientProcess
  XSSelect_SignTransferStatus(const Handle(Transfer_TransientProcess)& TP) : IFSelect_Signature("Transfer Status"), theTP(TP) {}
  
  //! Returns the Signature for a Transient object, as its transfer
  //! status
  Standard_EXPORT Standard_CString Value (const Handle(Standard_Transient)& ent, const Handle(Interface_InterfaceModel)& model) const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(XSSelect_SignTransferStatus,IFSelect_Signature)

 private:

  Handle(Transfer_TransientProcess) theTP;
  mutable TCollection_AsciiString theMes;
};

#endif // _XSSelect_SignTransferStatus_HeaderFile
