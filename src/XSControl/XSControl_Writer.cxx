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

//:i1 gka 03.04.99 BUC60301 

#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <Interface_InterfaceModel.hxx>
#include <Interface_Macros.hxx>
#include <Transfer_FinderProcess.hxx>
#include <Transfer_ActorOfFinderProcess.hxx>
#include <Message_Messenger.hxx>
#include <TopoDS_Shape.hxx>
#include <XSControl_Controller.hxx>
#include <XSControl_WorkSession.hxx>
#include <XSControl_Writer.hxx>
#include <Transfer_SimpleBinderOfTransient.hxx>
#include <TransferBRep_ShapeMapper.hxx>

Interface_ReturnStatus XSControl_Writer::TransferShape (const TopoDS_Shape& theShape, const Standard_Boolean theCompGraph)
{
  if (theShape.IsNull()) return Interface_RetVoid;

  if (myWS.IsNull()) return Interface_RetVoid;

  const Handle(Interface_InterfaceModel) &aModel = myWS->Model();
  if (aModel.IsNull()) return Interface_RetVoid;

  const Handle(Transfer_FinderProcess) &aProcess = myWS->WriterProcess();
  if (aProcess.IsNull()) return Interface_RetVoid;

  const Handle(Transfer_Process::Actor) &anActor = aProcess->GetActor();
  if (anActor.IsNull()) return Interface_RetError;

  const Handle(Message_Messenger) &sout = aProcess->Messenger();

  sout<<"\n*******************************************************************";
  sout<<"\n******        Statistics on Transfer (Write)                 ******";
  sout<<"\n*******************************************************************\n";
  /*szv_c1:sout<<"\n******        Transfer Mode = "<<theMode;
  Standard_CString modehelp = anActor->TransferModeHelp(theMode);
  if (modehelp && modehelp[0] != 0) sout<<"  I.E.  "<<modehelp;
  sout<<"       ******"<<endl;*/
  //anActor->PrintInfo(sout);
  sout<<"\n******        Transferring Shape, ShapeType = " <<theShape.ShapeType()<<"                      ******"<<endl;

  //szv_c1:anActor->SetTransferMode(theMode);
  aProcess->SetModel (aModel);

  Interface_ReturnStatus status = Interface_RetFail;

  try {
    OCC_CATCH_SIGNALS

    Handle(TransferBRep_ShapeMapper) aMapper = new TransferBRep_ShapeMapper(theShape);
    aProcess->Transfer (aMapper);

    Handle(Transfer_Binder) binder = aProcess->Find (aMapper);
    while (!binder.IsNull()) {
      Handle(Transfer_SimpleBinderOfTransient) bindtr = Handle(Transfer_SimpleBinderOfTransient)::DownCast (binder);
      if (!bindtr.IsNull()) {
        const Handle(Standard_Transient) &ent = bindtr->Result();
        if (!ent.IsNull()) {
          status = Interface_RetDone;
          aModel->AddWithRefs (ent);
        }
      }
      binder = binder->NextResult();
    }
  }
  catch (Standard_Failure) {
    sout<<"****  ****  Transfer, EXCEPTION : ";
    sout<<Standard_Failure::Caught()->GetMessageString();
    sout<<endl;
    status = Interface_RetFail;
  }

  if (theCompGraph) myWS->ComputeGraph(Standard_True);

  return status;
}

Interface_ReturnStatus XSControl_Writer::WriteFile (const Standard_CString theFileName)
{
  return myWS->WriteFile(theFileName);
}
