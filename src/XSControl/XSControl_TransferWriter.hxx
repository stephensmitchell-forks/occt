// Created on: 1996-03-13
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

#ifndef _XSControl_TransferWriter_HeaderFile
#define _XSControl_TransferWriter_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <MMgt_TShared.hxx>
#include <IFSelect_ReturnStatus.hxx>
#include <Transfer_FinderProcess.hxx>
class XSControl_Controller;
class Standard_Transient;
class Interface_InterfaceModel;
class TopoDS_Shape;
class Interface_CheckIterator;


class XSControl_TransferWriter;
DEFINE_STANDARD_HANDLE(XSControl_TransferWriter, MMgt_TShared)

//! TransferWriter gives help to control transfer to write a file
//! after having converted data from Cascade/Imagine
//!
//! It works with a Controller (which itself can work with an
//! Actor to Write) and a FinderProcess. It records results and
//! checks
class XSControl_TransferWriter : public MMgt_TShared
{
 public:
  
  //! Creates a TransferWriter, empty, ready to run
  //! with an empty FinderProcess (but no controller, etc)
  XSControl_TransferWriter()
  : myProcess(new Transfer_FinderProcess),
    myTransferMode(0)
  {}
  
  //! Returns the FinderProcess itself
  const Handle(Transfer_FinderProcess) & FinderProcess() const { return myProcess; }
  
  //! Sets a new FinderProcess and forgets the former one
  void SetFinderProcess (const Handle(Transfer_FinderProcess)& theFP) { myProcess = theFP; }

  //! Returns the currently used Controller
  const Handle(XSControl_Controller) & Controller() const { return myController; }
  
  //! Sets a new Controller, also sets a new FinderProcess
  Standard_EXPORT void SetController (const Handle(XSControl_Controller)& theCtl)
  {
    myController = theCtl;
    Clear(-1);
  }
  
  //! Clears recorded data according a mode
  //! 0 clears FinderProcess (results, checks)
  //! -1 create a new FinderProcess
  Standard_EXPORT void Clear (const Standard_Integer theMode);
  
  //! Returns the current Transfer Mode (an Integer)
  //! It will be interpreted by the Controller to run Transfers
  //! This call form could be later replaced by more specific ones
  //! (parameters suited for each norm / transfer case)
  Standard_Integer TransferMode() const { return myTransferMode; }

  //! Changes the Transfer Mode
  void SetTransferMode (const Standard_Integer theMode) { myTransferMode = theMode; }
  
  //! Transfers a Shape from CasCade to a model of current norm,
  //! according to the last call to SetTransferMode
  //! Works by calling the Controller
  //! Returns status : =0 if OK, >0 if error during transfer, <0 if
  //! transfer badly initialised
  Standard_EXPORT IFSelect_ReturnStatus TransferWriteShape (const Handle(Interface_InterfaceModel)& theModel, const TopoDS_Shape& theShape);

  DEFINE_STANDARD_RTTIEXT(XSControl_TransferWriter,MMgt_TShared)

 private:

  Handle(XSControl_Controller) myController;
  Handle(Transfer_FinderProcess) myProcess;
  Standard_Integer myTransferMode;
};

#endif // _XSControl_TransferWriter_HeaderFile
