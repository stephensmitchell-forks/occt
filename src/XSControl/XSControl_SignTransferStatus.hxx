// Copyright (c) 2013 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

#ifndef _XSControl_SignTransferStatus_HeaderFile
#define _XSControl_SignTransferStatus_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineHandle.hxx>
#include <XSControl_TransferReader.hxx>
#include <Transfer_TransientProcess.hxx>
#include <IFSelect_Signature.hxx>
#include <Standard_CString.hxx>
#include <Handle_Standard_Transient.hxx>
#include <Handle_Interface_InterfaceModel.hxx>

class Standard_Transient;
class Interface_InterfaceModel;
class Handle(Standard_Type);
class Handle(IFSelect_Signature);
class XSControl_SignTransferStatus;

DEFINE_STANDARD_HANDLE(XSControl_SignTransferStatus,IFSelect_Signature)

//! This Signatures gives the Transfer Status of an entity, as <br>
//!           recorded in a TransferProcess. It can be : <br>
//!           - Void : not recorded, or recorded as void with no message <br>
//!             (attributes are not taken into account) <br>
//!           - Warning : no result, warning message(s), no fail <br>
//!           - Fail : no result, fail messages (with or without warning) <br>
//!           - Result.. : result, no message (neither warning nor fail) <br>
//!             Result.. i.e. Result:TypeName of the result <br>
//!           - Result../Warning : result, with warning but no fail <br>
//!           - Result../Fail : result, with fail (.e. bad result) <br>
//!           - Fail on run : no result yet recorded, no message, but <br>
//!             an exception occurred while recording the result <br>
//!             (this should not appear and indicates a programming error) <br>
class XSControl_SignTransferStatus : public IFSelect_Signature
{
public:

  //! Creates a SignTransferStatus, not initialised <br>
  //!           it gives nothing (empty string) <br>
  Standard_EXPORT   XSControl_SignTransferStatus();
  //! Creates a SignTransferStatus, which will work on the current <br>
  //!           TransientProcess brought by the TransferReader (its MapReader) <br>
  Standard_EXPORT   XSControl_SignTransferStatus(const Handle(XSControl_TransferReader)& TR);
  //! Sets a TransferReader to work <br>
  Standard_EXPORT     void SetReader(const Handle(XSControl_TransferReader)& TR) ;
  //! Sets a precise map to sign entities <br>
  //!           This definition oversedes the creation with a TransferReader <br>
  Standard_EXPORT     void SetMap(const Handle(Transfer_TransientProcess)& TP) ;
  //! Returns the TransientProcess used as precised one <br>
  //!           Returns a Null Handle for a creation from a TransferReader <br>
  //!           without any further setting <br>
  Standard_EXPORT     Handle(Transfer_TransientProcess) Map() const;
  //! Returns the Reader (if created with a Reader) <br>
  //!           Returns a Null Handle if not created with a Reader <br>
  Standard_EXPORT     Handle(XSControl_TransferReader) Reader() const;
  //! Returns the Signature for a Transient object, as its transfer <br>
  //!           status <br>
  Standard_EXPORT     Standard_CString Value(const Handle(Standard_Transient)& ent,
                                             const Handle(Interface_InterfaceModel)& model) const;

  DEFINE_STANDARD_RTTI(XSControl_SignTransferStatus)

private:

  Handle(XSControl_TransferReader) theTR;
  Handle(Transfer_TransientProcess) theTP;
};
#endif
