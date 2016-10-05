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


#include <Interface_InterfaceModel.hxx>
#include <Interface_Macros.hxx>
#include <Message_Messenger.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <TopoDS_Shape.hxx>
#include <Transfer_ActorOfFinderProcess.hxx>
#include <Transfer_FinderProcess.hxx>
#include <Transfer_SimpleBinderOfTransient.hxx>
#include <XSControl_Controller.hxx>
#include <XSControl_TransferWriter.hxx>

IMPLEMENT_STANDARD_RTTIEXT(XSControl_TransferWriter,MMgt_TShared)


//=======================================================================
//function : Clear
//purpose  : 
//=======================================================================

void XSControl_TransferWriter::Clear (const Standard_Integer mode)
{
  if (mode < 0 || myProcess.IsNull())
    myProcess = new Transfer_FinderProcess;
  else myProcess->Clear();
}

//  ##########    LES ACTIONS    ##########

//=======================================================================
//function : TransferWriteShape
//purpose  : 
//=======================================================================

IFSelect_ReturnStatus XSControl_TransferWriter::TransferWriteShape
  (const Handle(Interface_InterfaceModel)& model,
   const TopoDS_Shape& shape)
{
  if (myController.IsNull()) return IFSelect_RetError;
  if (model.IsNull()) return IFSelect_RetVoid;

  IFSelect_ReturnStatus status = IFSelect_RetVoid;
  if (myProcess.IsNull()) myProcess = new Transfer_FinderProcess;
//  effacer l actor : Controller s en charge
  const Handle(Transfer_ActorOfFinderProcess) nulact;
  myProcess->SetActor (nulact);
  const Handle(Message_Messenger) &sout = myProcess->Messenger();
  try {
    OCC_CATCH_SIGNALS
    //  A ameliorer ... !
    sout<<"\n*******************************************************************\n";
    sout << "******        Statistics on Transfer (Write)                 ******"<<endl;
    sout<<"\n*******************************************************************\n";
    sout << "******        Transfer Mode = "<<myTransferMode;
    Standard_CString modehelp = myController->ModeWriteHelp (myTransferMode);
    if (modehelp && modehelp[0] != 0) sout<<"  I.E.  "<<modehelp;
    sout<<"       ******"<<endl;
    sout << "******        Transferring Shape, ShapeType = " << shape.ShapeType();
    sout<<"                      ******"<<endl;
    status = myController->TransferWriteShape (shape,myProcess,model,myTransferMode);
  }
  catch(Standard_Failure) {
    sout<<"****  ****  TransferWriteShape, EXCEPTION : "; 
    sout<<Standard_Failure::Caught()->GetMessageString(); 
    sout<<endl;
    status = IFSelect_RetFail;
  }
  return status;
}
