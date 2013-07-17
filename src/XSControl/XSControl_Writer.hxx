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

#ifndef _XSControl_Writer_HeaderFile
#define _XSControl_Writer_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Macro.hxx>
#include <XSControl_WorkSession.hxx>
#include <Standard_CString.hxx>
#include <Standard_Boolean.hxx>
#include <Handle_Interface_InterfaceModel.hxx>
#include <IFSelect_ReturnStatus.hxx>
#include <Standard_Integer.hxx>

class XSControl_WorkSession;
class Interface_InterfaceModel;
class TopoDS_Shape;


//! This class gives a simple way to create then write a <br>
//!           Model compliant to a given norm, from a Shape <br>
//!           The model can then be edited by tools by other appropriate tools <br>
class XSControl_Writer  {
public:

  DEFINE_STANDARD_ALLOC

  //! Creates a Writer from scratch <br>
  Standard_EXPORT   XSControl_Writer();
  //! Creates a Writer from scratch, with a norm name which <br>
  //!           identifie a Controller <br>
  Standard_EXPORT   XSControl_Writer(const Standard_CString norm);
  //! Creates a Writer from an already existing Session <br>
  //!           If <scratch> is True (D), clears already recorded data <br>
  Standard_EXPORT   XSControl_Writer(const Handle(XSControl_WorkSession)& WS,const Standard_Boolean scratch = Standard_True);
  //! Sets a specific norm to <me> <br>
  //!           Returns True if done, False if <norm> is not available <br>
  Standard_EXPORT     Standard_Boolean SetNorm(const Standard_CString norm) ;
  //! Sets a specific session to <me> <br>
  Standard_EXPORT     void SetWS(const Handle(XSControl_WorkSession)& WS,const Standard_Boolean scratch = Standard_True) ;
  //! Returns the session used in <me> <br>
  Standard_EXPORT     Handle_XSControl_WorkSession WS() const;
  //! Returns the produced model. Produces a new one if not yet done <br>
  //!           or if <newone> is True <br>
  //!           This method allows for instance to edit product or header <br>
  //!           data before writing <br>
  Standard_EXPORT     Handle_Interface_InterfaceModel Model(const Standard_Boolean newone = Standard_False) ;
  //! Transfers a Shape according to the mode <br>
  Standard_EXPORT     IFSelect_ReturnStatus TransferShape(const TopoDS_Shape& sh,const Standard_Integer mode = 0) ;
  //! Writes the produced model <br>
  Standard_EXPORT     IFSelect_ReturnStatus WriteFile(const Standard_CString filename) ;
  //! Prints Statistics about Transfer <br>
  Standard_EXPORT     void PrintStatsTransfer(const Standard_Integer what,const Standard_Integer mode = 0) const;


private:

  Handle_XSControl_WorkSession thesession;
};
#endif
