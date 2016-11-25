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

//:i1 pdn 03.04.99  BUC60301  

#include <Interface_Category.hxx>
#include <Interface_Check.hxx>
#include <Interface_CheckIterator.hxx>
#include <Interface_CheckTool.hxx>
#include <Interface_Graph.hxx>
#include <Interface_GTool.hxx>
#include <Interface_HGraph.hxx>
#include <Interface_InterfaceModel.hxx>
#include <Interface_IntVal.hxx>
#include <Interface_Macros.hxx>
#include <Interface_Protocol.hxx>
#include <Interface_ShareTool.hxx>
#include <Message_Messenger.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <TColStd_HSequenceOfTransient.hxx>
#include <TopoDS_Shape.hxx>
#include <Transfer_Binder.hxx>
#include <Transfer_Finder.hxx>
#include <Transfer_FinderProcess.hxx>
#include <Transfer_ResultFromModel.hxx>
#include <Transfer_ResultFromTransient.hxx>
#include <Transfer_SimpleBinderOfTransient.hxx>
#include <Transfer_TransientProcess.hxx>
#include <Transfer_ActorOfFinderProcess.hxx>
#include <TransferBRep.hxx>
#include <XSControl_Controller.hxx>
#include <XSControl_WorkSession.hxx>

IMPLEMENT_STANDARD_RTTIEXT(XSControl_WorkSession,MMgt_TShared)

#define Flag_Incorrect 2
//  (Bit Map n0 2)

//  #################################################################

//=======================================================================
//function : XSControl_WorkSession
//purpose  : 
//=======================================================================

XSControl_WorkSession::XSControl_WorkSession ()
: thegtool(new Interface_GTool),
  thecheckdone(Standard_False),
  themodelstat(Standard_False),
  myReaderProcess(new Transfer_TransientProcess),
  myWriterProcess(new Transfer_FinderProcess)
{
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

void XSControl_WorkSession::SetModel (const Handle(Interface_InterfaceModel)& model)
{
  if (myModel != model) {
    theloaded.Clear();
    myModel = model;
  }

  if (!thegtool.IsNull()) thegtool->ClearEntities(); //smh#14 FRA62479
  myModel->SetGTool (thegtool);
  
  ComputeGraph(Standard_True);

  thecheckdone = Standard_False; // RAZ CheckList, a refaire
  
//  MISE A JOUR des SelectPointed  C-A-D  on efface leur contenu
  ClearData(4);
  ClearData(0);
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Standard_Boolean XSControl_WorkSession::ComputeGraph (const Standard_Boolean enforce)
{
  if (myModel.IsNull()) return Standard_False;
  const Standard_Integer nb = myModel->NbEntities();

  if (!enforce && !thegraph.IsNull() && (thegraph->Graph().Size() == nb))
    return Standard_True;

  thegraph.Nullify();

  if (nb == 0)
    return Standard_False;

  //  Il faut calculer le graphe pour de bon
  thegraph = new Interface_HGraph (myModel,themodelstat);
  if (themodelstat)
  {
    Standard_Integer i; // svv #1
    for (i = 1; i <= nb; i ++) thegraph->CGraph().SetStatus(i,0);
    Interface_BitMap& bm = thegraph->CGraph().CBitMap();
    bm.AddFlag();
    bm.SetFlagName (Flag_Incorrect,"Incorrect");
  }
  ComputeCheck();
  if (themodelstat)
  {
    //  Calcul des categories, a present memorisees dans le modele
    Interface_Category categ(thegtool);
    Interface_ShareTool sht(thegraph);
    Standard_Integer i =1;
    for ( ; i <= nb; i ++)
      myModel->SetCategoryNumber(i,categ.CatNum(myModel->Value(i),sht));
  }

  return Standard_True;
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Handle(Interface_HGraph) XSControl_WorkSession::HGraph ()
{
  ComputeGraph();
  return thegraph;
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

const Interface_Graph& XSControl_WorkSession::Graph ()
{
  ComputeGraph();
  if (thegraph.IsNull()) Standard_DomainError::Raise
    ("XSControl_WorkSession : Graph not available");
  return thegraph->Graph();
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Standard_Boolean XSControl_WorkSession::IsLoaded () const
{
  if (myModel.IsNull()) return Standard_False;
  if (myModel->NbEntities() == 0) return Standard_False;
  if (thegraph.IsNull()) return Standard_False;
  return (myModel->NbEntities() == thegraph->Graph().Size());
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Standard_Boolean XSControl_WorkSession::ComputeCheck ()
{
  if (thecheckdone) return Standard_True;

  if (!IsLoaded()) return Standard_False;

  Interface_Graph& CG = thegraph->CGraph();
  Interface_CheckTool cht(thegraph);
  Interface_CheckIterator checklist = cht.VerifyCheckList();
  myModel->FillSemanticChecks(checklist,Standard_False);

  if(themodelstat)
  {
    //  Et on met a jour le Graphe (BitMap) !  Flag Incorrect (STX + SEM)
    Interface_BitMap& BM = CG.CBitMap();
    BM.Init (Standard_False,Flag_Incorrect);
    Standard_Integer num, nb = CG.Size();
    for (checklist.Start(); checklist.More(); checklist.Next()) {
      const Handle(Interface_Check) chk = checklist.Value();
      if (!chk->HasFailed()) continue;
      num = checklist.Number();
      if (num > 0 && num <= nb) BM.SetTrue (num,Flag_Incorrect);
    }
    for (num = 1; num <= nb; num ++)
      if (myModel->IsErrorEntity (num)) BM.SetTrue (num,Flag_Incorrect);
  }

  thecheckdone = Standard_True;

  return Standard_True;
}


//=======================================================================
//function : ClearData
//purpose  : 
//=======================================================================

void XSControl_WorkSession::ClearData (const Standard_Integer mode)
{
  switch (mode) {
    case 1 : {
      theloaded.Clear();
      if (!myModel.IsNull()) {
        myModel->Clear();
        myModel.Nullify();
      }
      ClearData(2);  ClearData(4);
      break;
    }
    case 2 : {  thegraph.Nullify();  thecheckdone = Standard_False;  break;  }
    case 3 : {  thecheckdone = Standard_False;  break;  }
    // 5 : Transferts seuls
    // 6 : Resultats forces seuls
    case 5: {
      myReaderProcess = new Transfer_TransientProcess;
      myWriterProcess = new Transfer_FinderProcess;

      if (!myController.IsNull()) {
        myReaderProcess->SetActor(myController->ActorRead(Model())); //szv_c1:
        myWriterProcess->SetActor(myController->NewActorWrite()); //szv_c1:
      }
      break;
    }
    case 6: { myReaderProcess->ClearResults(); }
    default : break;
  }

  myReaderProcess->SetGraph (HGraph());
}


//=======================================================================
//function : SelectNorm
//purpose  : 
//=======================================================================

Standard_Boolean XSControl_WorkSession::SelectNorm(const Standard_CString normname)
{
  // Old norm and results
  //xxx1:myTransferReader->Clear();
  //  ????  En toute rigueur, menage a faire dans XWS : virer les items
  //        ( a la limite, pourquoi pas, refaire XWS en entier)

  Handle(XSControl_Controller) newadapt = XSControl_Controller::Recorded(normname);
  if (newadapt.IsNull()) return Standard_False;
  if (newadapt == myController) return Standard_True;
  SetController (newadapt);
  return Standard_True;
}


//=======================================================================
//function : SetController
//purpose  : 
//=======================================================================

void XSControl_WorkSession::SetController(const Handle(XSControl_Controller)& ctl)
{
  myController = ctl;

  const Handle(Interface_Protocol) &aProtocol = myController->Protocol();
  thegtool->SetProtocol(aProtocol);

  // Set worksession parameters from the controller
  Handle(XSControl_WorkSession) aWorkSession(this);
  myController->Customise (aWorkSession);

  myReaderProcess = new Transfer_TransientProcess;
  myReaderProcess->SetActor(myController->ActorRead(Model())); //szv_c1:

  myWriterProcess = new Transfer_FinderProcess;
  myWriterProcess->SetActor(myController->NewActorWrite()); //szv_c1:
}


//=======================================================================
//function : ReadFile
//purpose  : 
//=======================================================================

Interface_ReturnStatus XSControl_WorkSession::ReadFile (const Standard_CString theFileName)
{
  if (myController.IsNull() || myController->Protocol().IsNull())
    return Interface_RetVoid;

  Handle(Interface_InterfaceModel) model;
  Interface_ReturnStatus status = Interface_RetVoid;
  try {
    OCC_CATCH_SIGNALS
    const Standard_Integer stat = myController->ReadFile (theFileName,model);
    if (stat == 0) status = Interface_RetDone;
    else if (stat < 0) status = Interface_RetError;
    else status = Interface_RetFail;
  }
  catch(Standard_Failure) {
    const Handle(Message_Messenger) &sout = Message::DefaultMessenger();
    sout<<"    ****    Exception in XSControl_WorkSession::ReadFile :   ****\n";
    sout << Standard_Failure::Caught()->GetMessageString();
    sout<<"\n    Abandon"<<endl;
    status = Interface_RetFail;
  }
  if (status != Interface_RetDone) return status;
  if (model.IsNull()) return Interface_RetVoid;

  SetModel (model);

  theloaded = theFileName;

  return status;
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Interface_ReturnStatus XSControl_WorkSession::WriteFile (const Standard_CString theFileName)
{
  if (myController.IsNull() || myController->Protocol().IsNull())
    return Interface_RetVoid;

  const Handle(Message_Messenger) &sout = Message::DefaultMessenger();

  Interface_CheckIterator checks;
  try {
    OCC_CATCH_SIGNALS
    sout << "** WorkSession : Writing all data" << endl;
    ComputeGraph(Standard_True);
    if (!IsLoaded())
      return Interface_RetVoid;

    checks.SetName ("X-STEP WorkSession : WriteFile");
    Interface_CheckIterator checklst;
    const Standard_Boolean res = myController->WriteFile(theFileName,myModel,checklst);
    checks.Merge(checklst);
    if (!res) checks.CCheck(0)->AddFail ("WriteFile has failed");
  }
  catch (Standard_Failure) {
    sout<<"    ****    Exception in XSControl_WorkSession::WriteFile :   ****\n";
    sout<<Standard_Failure::Caught()->GetMessageString();
    sout<<"\n    Abandon"<<endl;
    return Interface_RetFail;
  }

  Handle(Interface_Check) aMainFail = checks.CCheck(0);
  if (!aMainFail.IsNull() && aMainFail->HasFailed ())
  {
    return Interface_RetStop;
  }

  theloaded = theFileName;

  return (checks.IsEmpty(Standard_True)? Interface_RetDone : Interface_RetError);
}


//              ##########################################
//              ############    RESULTATS FORCES    ######
//              ##########################################


//=======================================================================
//function : InitTransferReader
//purpose  : 
//=======================================================================

void  XSControl_WorkSession::InitTransferReader(const Standard_Integer mode)
{
  //if (mode == 0)  myTransferReader->Clear();  // full clear

  //if (myTransferReader.IsNull())
    //myTransferReader = new XSControl_TransferReader;
  //SetTransferReader (myTransferReader.IsNull()? new XSControl_TransferReader : myTransferReader);
  //{
  //if (myTransferReader != TR) //i1 pdn 03.04.99 BUC60301
    //myTransferReader = TR;

  //xxx1:myTransferReader->SetController (myController);

  //xxx1:TR->TransientProcess()->SetGraph (HGraph());
  //xxx1:if (!TR->TransientProcess().IsNull()) return;
  const Standard_Integer nbe = Model().IsNull() ? 0 : Model()->NbEntities();
  Handle(Transfer_TransientProcess) TP = new Transfer_TransientProcess(nbe+100);
  TP->SetActor(myController->ActorRead(Model())); //szv_c1:
  TP->SetGraph (HGraph());

  myReaderProcess = TP;

  // mode = 0 fait par SetTransferReader suite a Nullify
  //szv_c1: mode = 1 not used
  //szv_c1: mode = 2 not used
  //szv_c1: mode = 3 not used

  if (mode == 4) myReaderProcess->BeginTransfer(); //myTransferReader->BeginTransfer();
}

//=======================================================================
//function : SetReaderProcess
//purpose  : 
//=======================================================================

Standard_Boolean XSControl_WorkSession::SetReaderProcess (const Handle(Transfer_TransientProcess)& TP)
{
  if (TP.IsNull()) return Standard_False;
  if (TP->Model().IsNull()) TP->SetModel (Model());
  TP->SetGraph (HGraph());
  if (TP->Model() != Model()) return Standard_False;

  TP->SetActor(myController->ActorRead(Model())); //szv_c1:
  return Standard_True;
}

//              ##########################################
//              ############    TRANSFERT  WRITE
//              ##########################################

//=======================================================================
//function : NewModel
//purpose  : 
//=======================================================================

Handle(Interface_InterfaceModel) XSControl_WorkSession::NewModel ()
{
  if (myController.IsNull())
    return NULL;

  Handle(Interface_InterfaceModel) newmod = myController->NewModel();
  
  SetModel(newmod);
  /*xxx1:if(!myReaderProcess.IsNull())
    myReaderProcess->Clear();
  //clear all contains of WS
  myTransferReader->Clear();*/

  myReaderProcess = new Transfer_TransientProcess;
  myReaderProcess->SetActor(myController->ActorRead(Model())); //szv_c1:

  myWriterProcess = new Transfer_FinderProcess;
  myWriterProcess->SetActor(myController->NewActorWrite()); //szv_c1:

  return newmod;
}
