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


#ifndef _XSControl_TransferWriter_HeaderFile
#define _XSControl_TransferWriter_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineHandle.hxx>
#include <XSControl_Controller.hxx>
#include <Transfer_FinderProcess.hxx>
#include <Standard_Integer.hxx>
#include <MMgt_TShared.hxx>
#include <Standard_Boolean.hxx>
#include <Handle_Standard_Transient.hxx>
#include <IFSelect_ReturnStatus.hxx>
#include <Handle_Interface_InterfaceModel.hxx>

class Transfer_FinderProcess;
class Standard_Transient;
class Interface_InterfaceModel;
class TopoDS_Shape;
class Interface_CheckIterator;
class Standard_Transient;
class Handle(Standard_Type);
class Handle(MMgt_TShared);
class XSControl_TransferWriter;

DEFINE_STANDARD_HANDLE(XSControl_TransferWriter,MMgt_TShared)

//! TransferWriter gives help to control transfer to write a file <br>
//!           after having converted data from Cascade/Imagine <br>
//! <br>
//!           It works with a Controller (which itself can work with an <br>
//!           Actor to Write) and a FinderProcess. It records results and <br>
//!           checks <br>
class XSControl_TransferWriter : public MMgt_TShared {

public:

  //! Creates a TransferWriter, empty, ready to run <br>
//!           with an empty FinderProcess (but no controller, etc) <br>
  Standard_EXPORT   XSControl_TransferWriter();
  //! Returns the FinderProcess itself <br>
  Standard_EXPORT     Handle_Transfer_FinderProcess FinderProcess() const;
  //! Sets a new FinderProcess and forgets the former one <br>
  Standard_EXPORT     void SetFinderProcess(const Handle(Transfer_FinderProcess)& FP) ;
  //! Returns the currently used Controller <br>
  Standard_EXPORT     Handle(XSControl_Controller) Controller() const;
  //! Sets a new Controller, also sets a new FinderProcess <br>
  Standard_EXPORT     void SetController(const Handle(XSControl_Controller)& ctl) ;
  //! Clears recorded data according a mode <br>
//!           0 clears FinderProcess (results, checks) <br>
//!          -1 create a new FinderProcess <br>
  Standard_EXPORT     void Clear(const Standard_Integer mode) ;
  //! Returns the current Transfer Mode (an Integer) <br>
//!           It will be interpreted by the Controller to run Transfers <br>
//!           This call form could be later replaced by more specific ones <br>
//!             (parameters suited for each norm / transfer case) <br>
  Standard_EXPORT     Standard_Integer TransferMode() const;
  //! Changes the Transfer Mode <br>
  Standard_EXPORT     void SetTransferMode(const Standard_Integer mode) ;
  //! Prints statistics on current Trace File, according what,mode <br>
//!           See PrintStatsProcess for details <br>
  Standard_EXPORT     void PrintStats(const Standard_Integer what,const Standard_Integer mode = 0) const;
  //! Tells if a transient object (from an application) is a valid <br>
//!           candidate for a transfer to a model <br>
//!           Asks the Controller (RecognizeWriteTransient) <br>
//!           If <obj> is a HShape, calls RecognizeShape <br>
  Standard_EXPORT     Standard_Boolean RecognizeTransient(const Handle(Standard_Transient)& obj) ;
  //! Transfers a Transient object (from an application) to a model <br>
//!           of current norm, according to the last call to SetTransferMode <br>
//!           Works by calling the Controller <br>
//!           Returns status : =0 if OK, >0 if error during transfer, <0 if <br>
//!               transfer badly initialised <br>
  Standard_EXPORT     IFSelect_ReturnStatus TransferWriteTransient(const Handle(Interface_InterfaceModel)& model,const Handle(Standard_Transient)& obj) ;
  //! Tells if a Shape is valid for a transfer to a model <br>
//!           Asks the Controller (RecognizeWriteShape) <br>
  Standard_EXPORT     Standard_Boolean RecognizeShape(const TopoDS_Shape& shape) ;
  //! Transfers a Shape from CasCade to a model of current norm, <br>
//!           according to the last call to SetTransferMode <br>
//!           Works by calling the Controller <br>
//!           Returns status : =0 if OK, >0 if error during transfer, <0 if <br>
//!               transfer badly initialised <br>
  Standard_EXPORT     IFSelect_ReturnStatus TransferWriteShape(const Handle(Interface_InterfaceModel)& model,const TopoDS_Shape& shape) ;
  //! Returns the check-list of last transfer (write), i.e. the <br>
//!           check-list currently recorded in the FinderProcess <br>
  Standard_EXPORT     Interface_CheckIterator CheckList() const;
  //! Returns the check-list of last transfer (write), but tries <br>
//!           to bind to each check, the resulting entity in the model <br>
//!           instead of keeping the original Mapper, whenever known <br>
  Standard_EXPORT     Interface_CheckIterator ResultCheckList(const Handle(Interface_InterfaceModel)& model) const;
  //! Forecast to print statitics about a FinderProcess <br>
  Standard_EXPORT   static  void PrintStatsProcess(const Handle(Transfer_FinderProcess)& TP,const Standard_Integer what,const Standard_Integer mode = 0) ;

  DEFINE_STANDARD_RTTI(XSControl_TransferWriter)

private:


  Handle(XSControl_Controller) theController;
  Handle(Transfer_FinderProcess) theTransferWrite;
  Standard_Integer theTransferMode;
};
#endif
