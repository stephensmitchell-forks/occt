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


#include <IFSelect_CheckCounter.hxx>
#include <IFSelect_SessionPilot.hxx>
#include <Interface_CheckIterator.hxx>
#include <Interface_InterfaceModel.hxx>
#include <Interface_Macros.hxx>
#include <Interface_Static.hxx>
#include <Message.hxx>
#include <Message_Messenger.hxx>
#include <TColStd_HSequenceOfHAsciiString.hxx>
#include <TColStd_HSequenceOfTransient.hxx>
#include <Transfer_Binder.hxx>
#include <Transfer_FinderProcess.hxx>
#include <Transfer_ResultFromModel.hxx>
#include <Transfer_TransientProcess.hxx>
#include <XSControl_Controller.hxx>
#include <XSControl_SelectForTransfer.hxx>
#include <XSControl_TransferReader.hxx>
#include <XSControl_TransferWriter.hxx>
#include <XSControl_WorkSession.hxx>
#include <XSDRAW.hxx>
#include <XSDRAW_Activator.hxx>
#include <XSDRAW_ControlFunctions.hxx>
#include <XSDRAW_SelectFunctions.hxx>

// #######################################################################
// ##									##
// ##									##
// ##				FUNCTIONS				##
// ##									##
// ##									##
// #######################################################################
//=======================================================================
//function : xinit
//=======================================================================
static IFSelect_ReturnStatus XSControl_xinit(const Handle(IFSelect_SessionPilot)& pilot)
{
  Standard_Integer argc = pilot->NbWords();
  const Standard_CString arg1 = pilot->Arg(1);
  //        ****    xinit        ****
  if (argc > 1) return (XSDRAW::Session(pilot)->SelectNorm(arg1) ?
			IFSelect_RetDone : IFSelect_RetFail);
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  sout<<"Selected Norm:"<<XSDRAW::Session(pilot)->SelectedNorm()<<endl;
  return IFSelect_RetVoid;
}


//=======================================================================
//function : xnorm
//=======================================================================
static IFSelect_ReturnStatus XSControl_xnorm(const Handle(IFSelect_SessionPilot)& pilot)
{
  Standard_Integer argc = pilot->NbWords();
  const Standard_CString arg1 = pilot->Arg(1);
  //        ****    xnorm        ****
  Handle(XSControl_WorkSession) WS = XSDRAW::Session(pilot);
  Handle(XSControl_Controller) control = WS->NormAdaptor();
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  if (argc == 1)
    sout<<"Current Norm. xnorm newnorm to change"<<endl;
  else sout<<"Current Norm :"<<endl;
  if (control.IsNull()) sout<<"no norm currently defined"<<endl;
  else
    sout<<"  Long  Name (complete) : "<<control->Name(Standard_False)<<endl
      <<  "  Short name (resource) : "<<control->Name(Standard_True)<<endl;
  if (argc == 1) return IFSelect_RetVoid;

  control = XSControl_Controller::Recorded(arg1);
  if (control.IsNull()) {
    sout<<" No norm named : "<<arg1<<endl;
    return IFSelect_RetError;
  }

  WS->SetController(control);
  sout<<"new norm : "<<control->Name()<<endl;
  return IFSelect_RetDone;
}


//=======================================================================
//function : newmodel
//=======================================================================
static IFSelect_ReturnStatus XSControl_newmodel(const Handle(IFSelect_SessionPilot)& pilot)
{
  //        ****    newmodel        ****
  if (!XSDRAW::Session(pilot)->NewModel().IsNull()) return IFSelect_RetDone;
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  sout<<"No new Model produced"<<endl;
  return IFSelect_RetFail;
}


//=======================================================================
//function : tpclear
//=======================================================================
static IFSelect_ReturnStatus XSControl_tpclear(const Handle(IFSelect_SessionPilot)& pilot)
{
  //        ****    tpclear/twclear        ****
  const Standard_Boolean modew = (pilot->Word(0).Value(2) == 'w');
  const Handle(Transfer_FinderProcess)    &FP = XSDRAW::Session(pilot)->TransferWriter()->FinderProcess();
  const Handle(Transfer_TransientProcess) &TP = XSDRAW::Session(pilot)->TransferReader()->TransientProcess();
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  if (modew) { if(!FP.IsNull()) FP->Clear(); else sout<<"No Transfer Write"<<endl; }
  else       { if(!TP.IsNull()) TP->Clear(); else sout<<"No Transfer Read"<<endl; }
  return IFSelect_RetDone;
}


//=======================================================================
//function : tpstat
//=======================================================================
static IFSelect_ReturnStatus XSControl_tpstat(const Handle(IFSelect_SessionPilot)& pilot)
{
  Standard_Integer argc = pilot->NbWords();
  const Standard_CString arg1 = pilot->Arg(1);
  //const Standard_CString arg2 = pilot->Arg(2);
  const Handle(Transfer_TransientProcess) &TP = XSDRAW::Session(pilot)->TransferReader()->TransientProcess();
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  if (TP.IsNull()) { sout<<"No Transfer Read"<<endl; return IFSelect_RetError;}
  //        ****    tpstat        ****

  Standard_Integer mod1 = -1;
  Standard_Integer mod2 = 0;
  //  g : general  c : check (compte) C (liste)   f : fails(compte)  F (liste)
  //  resultats racines :  n : n0s entites  s : status  b : binders
  //    t : compte par type  r : compte par resultat   l : liste(type-resultat)
  //   *n *s *b *t *r *l : idem sur tout
  //   ?n  etc.. : idem sur resultats anormaux
  //   ?  tout court pour help
  
  if (argc > 1) {
    char a2 = arg1[1]; if (a2 == '\0') a2 = '!';
    switch (arg1[0]) {
    case 'g' : mod1 = 0; break;
    case 'c' : mod1 = 4; mod2 = 4; break;
    case 'C' : mod1 = 4; mod2 = 2; break;
    case 'f' : mod1 = 5; mod2 = 4; break;
    case 'F' : mod1 = 5; mod2 = 2; break;
    case '*' : mod1 = 2; break;
    case '?' : mod1 = 3; break;
    default  : mod1 = 1; if (argc > 2) mod1 = 2; a2 = arg1[0]; break;
    }
    if (mod1 < 1 || mod1 > 3) a2 = '!';
    switch (a2) {
    case 'n' : mod2 = 0; break;
    case 's' : mod2 = 1; break;
    case 'b' : mod2 = 2; break;
    case 't' : mod2 = 3; break;
    case 'r' : mod2 = 4; break;
    case 'l' : mod2 = 5; break;
    case 'L' : mod2 = 6; break;
    case '!' : break;
    case '?' : mod1 = -1; break;
    default  : mod1 = -2; break;
    }
  }
  //  A present help eventuel
  if (mod1 < -1) sout<<"Unknown Mode"<<endl;
  if (mod1 < 0) {
    sout<<"Modes available :\n"
      <<"g : general    c : checks (count)  C (list)\n"
      <<"               f : fails  (count)  F (list)\n"
      <<"  n : numbers of transferred entities (on TRANSFER ROOTS)\n"
      <<"  s : their status (type entity-result , presence checks)\n"
      <<"  b : detail of binders\n"
      <<"  t : count per entity type    r : per type/status result\n"
      <<"  l : count per couple  type entity/result\n"
      <<"  L : list  per couple  type entity/result\n"
      <<"  *n  *s  *b  *t  *r  *l  *L : idem on ALL recorded items\n"
      <<"  ?n  ?s  ?b  ?t ... : idem on abnormal items\n"
      <<"  n select : n applied on a selection   idem for  s b t r l"<<endl;
    if (mod1 < -1) return IFSelect_RetError;
    return IFSelect_RetVoid;
  }

  if (!TP.IsNull()) {
    sout<<"TransferRead :";
    if (TP->Model() != pilot->Session()->Model()) sout<<"Model differs from the session";
    Handle(TColStd_HSequenceOfTransient) list =
      XSDRAW_SelectFunctions::GiveList(pilot->Session(),pilot->CommandPart(2));
    XSControl_TransferReader::PrintStatsOnList (TP,list,mod1,mod2);
  }
  else sout<<"TransferRead : not defined"<<endl;
  return IFSelect_RetVoid;
}


//=======================================================================
//function : tpent
//=======================================================================
static IFSelect_ReturnStatus XSControl_tpent(const Handle(IFSelect_SessionPilot)& pilot)
{
  Standard_Integer argc = pilot->NbWords();
  const Standard_CString arg1 = pilot->Arg(1);
  const Handle(Transfer_TransientProcess) &TP = XSDRAW::Session(pilot)->TransferReader()->TransientProcess();
  //        ****    tpent        ****
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  if (TP.IsNull()) { sout<<"No Transfer Read"<<endl; return IFSelect_RetError;}
  Handle(Interface_InterfaceModel) model = TP->Model();
  if (model.IsNull())  return IFSelect_RetFail;

  if (argc < 2) { sout<<"Give ENTITY NUMBER (IN MODEL TransferProcess)"<<endl; return IFSelect_RetError; }
  Standard_Integer num = atoi(arg1);
  if (num <= 0 || num > model->NbEntities()) { sout<<"Number not in [1 - "<<model->NbEntities()<<"]"<<endl; return IFSelect_RetError; }
  Handle(Standard_Transient) ent = model->Value(num);
  Standard_Integer index = TP->MapIndex  (ent);
  if (index == 0) sout<<"Entity "<<num<<"  not recorded in transfer"<<endl;
  else XSDRAW::Session(pilot)->PrintTransferStatus (index,Standard_False,sout);
  return IFSelect_RetVoid;
}


//=======================================================================
//function : tpitem
//=======================================================================
static IFSelect_ReturnStatus XSControl_tpitem(const Handle(IFSelect_SessionPilot)& pilot)
{
  Standard_Integer argc = pilot->NbWords();
  const Standard_CString arg1 = pilot->Arg(1);
//        ****    tpitem/tproot/twitem/twroot        ****
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  if (argc < 2) { sout<<"Give ITEM NUMBER (in TransferProcess)"<<endl; return IFSelect_RetError; }
  Standard_Integer num = atoi(arg1);
  if (pilot->Word(0).Value(3) == 'r') num = -num;
  Standard_Boolean modew = Standard_False;
  if (pilot->Word(0).Value(2) == 'w') modew = Standard_True;
  Handle(Transfer_Binder) binder;
  Handle(Standard_Transient) ent;
  if (!XSDRAW::Session(pilot)->PrintTransferStatus(num,modew,sout))
    sout<<" - Num="<<num<<" incorrect"<<endl;
  return IFSelect_RetVoid;
}


//=======================================================================
//function : trecord
//=======================================================================
static IFSelect_ReturnStatus XSControl_trecord(const Handle(IFSelect_SessionPilot)& pilot)
{
  Standard_Integer argc = pilot->NbWords();
  const Standard_CString arg1 = pilot->Arg(1);
  const Handle(Transfer_TransientProcess) &TP = XSDRAW::Session(pilot)->TransferReader()->TransientProcess();
//        ****    trecord : TransferReader        ****
  Standard_Boolean tous = (argc == 1);
  Standard_Integer num = -1;
  const Handle(Interface_InterfaceModel) &mdl = XSDRAW::Session(pilot)->Model();
  const Handle(XSControl_TransferReader) &TR  = XSDRAW::Session(pilot)->TransferReader();
  Handle(Standard_Transient) ent;
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  if (mdl.IsNull() || TR.IsNull() || TP.IsNull())
    { sout<<" init not done"<<endl; return IFSelect_RetError; }
  if (!tous) num = atoi(arg1);
  //    Enregistrer les racines
  if (tous) {
    Standard_Integer nb = TP->NbRoots();
    sout<<" Recording "<<nb<<" Roots"<<endl;
    for (Standard_Integer i = 1; i <= nb; i ++) {
      ent = TP->Root(i);
      if (TR->RecordResult (ent)) sout<<" Root n0."<<i<<endl;
      else sout<<" Root n0."<<i<<" not recorded"<<endl;
    }
  } else {
    if (num < 1 ||  num > mdl->NbEntities()) sout<<"incorrect number:"<<num<<endl;
    else if (TR->RecordResult(mdl->Value(num))) sout<<" Entity n0."<<num<<endl;
    else sout<<" Entity n0."<<num<<" not recorded"<<endl;
  }
  return IFSelect_RetDone;
}


//=======================================================================
//function : trstat
//=======================================================================
static IFSelect_ReturnStatus XSControl_trstat(const Handle(IFSelect_SessionPilot)& pilot)
{
  Standard_Integer argc = pilot->NbWords();
  const Standard_CString arg1 = pilot->Arg(1);
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
//        ****    trstat : TransferReader        ****
  const Handle(XSControl_TransferReader) &TR = XSDRAW::Session(pilot)->TransferReader();
  if (TR.IsNull()) { sout<<" init not done"<<endl; return IFSelect_RetError; }
  Handle(Interface_InterfaceModel)  mdl = TR->Model();
  if (mdl.IsNull()) { sout<<" No model"<<endl; return IFSelect_RetError; }
  sout<<" Statistics : FileName : "<<TR->FileName()<<endl;
  if (argc == 1) {
    // stats generales
    TR->PrintStats(10,0);
  } else {
    // stats unitaires
    Standard_Integer num = atoi(arg1);
    if (num < 1 || num > mdl->NbEntities()) { sout<<" incorrect number:"<<arg1<<endl; return IFSelect_RetError; }
    Handle(Standard_Transient) ent = mdl->Value(num);
    if (!TR->IsRecorded(ent)) { sout<<" Entity "<<num<<" not recorded"<<endl; return IFSelect_RetError; }
    Handle(Transfer_ResultFromModel) RM = TR->FinalResult(ent);
    Handle(TColStd_HSequenceOfTransient) list = TR->CheckedList(ent);
    Standard_Integer i, nb = list->Length();
    if (nb > 0) sout<<" Entities implied by Check/Result :"<<nb<<" i.e.:";
    for (i = 1; i <= nb; i ++) { sout<<"  "; mdl->Print(list->Value(i),sout); }
    sout<<endl;
    if (RM.IsNull()) { sout<<" no other info"<<endl; return IFSelect_RetVoid; }
    Interface_CheckIterator chl = RM->CheckList(Standard_False);
    pilot->Session()->PrintCheckList(chl,Standard_False,IFSelect_EntitiesByItem);
  }
  return IFSelect_RetVoid;
}


//=======================================================================
//function : trbegin
//=======================================================================
static IFSelect_ReturnStatus XSControl_trbegin(const Handle(IFSelect_SessionPilot)& pilot)
{
  //        ****    trbegin : TransferReader        ****
  Handle(XSControl_TransferReader) TR = XSDRAW::Session(pilot)->TransferReader();
  Standard_Boolean init = TR.IsNull();
  if (pilot->NbWords() > 1) { if (pilot->Arg(1)[0] == 'i') init = Standard_True; }
  if (init) {
    XSDRAW::Session(pilot)->InitTransferReader (0);
    TR = XSDRAW::Session(pilot)->TransferReader();
    if (TR.IsNull()) { 
      Handle(Message_Messenger) sout = Message::DefaultMessenger();
      sout<<" init not done or failed"<<endl; 
      return IFSelect_RetError; 
    }
  }
  TR->BeginTransfer();
  return IFSelect_RetDone;
}


//=======================================================================
//function : tread
//=======================================================================
static IFSelect_ReturnStatus XSControl_tread(const Handle(IFSelect_SessionPilot)& pilot)
{
  Standard_Integer argc = pilot->NbWords();
  //const Standard_CString arg1 = pilot->Arg(1);
  //        ****    tread : TransferReader        ****
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  const Handle(XSControl_TransferReader) &TR = XSDRAW::Session(pilot)->TransferReader();
  if (TR.IsNull()) { sout<<" init not done"<<endl; return IFSelect_RetError; }
  const Handle(Interface_InterfaceModel) &mdl = TR->Model();
  if (mdl.IsNull()) { sout<<" No model"<<endl; return IFSelect_RetError; }
  if (argc < 2) {
//      DeclareAndCast(IFSelect_Selection,sel,pilot->Session()->NamedItem("xst-model-roots"));
    Handle(Standard_Transient) sel = pilot->Session()->NamedItem("xst-model-roots");
    if (sel.IsNull()) { sout<<"Select Roots absent"<<endl; return IFSelect_RetError; }
    Handle(TColStd_HSequenceOfTransient) list = pilot->Session()->GiveList(sel);
    sout<<" Transferring all roots i.e. : "<<TR->TransferList(list)<<endl;
  } else {
    Handle(TColStd_HSequenceOfTransient) list =
      XSDRAW_SelectFunctions::GiveList(pilot->Session(),pilot->CommandPart(1));
    sout<<" Transfer of "<<list->Length()<<" entities"<<endl;
    Standard_Integer nb = TR->TransferList(list);
    sout<<" Gives "<<nb<<" results"<<endl;
  }
  return IFSelect_RetDone;
}


//=======================================================================
//function : trtp
//=======================================================================
static IFSelect_ReturnStatus XSControl_trtp(const Handle(IFSelect_SessionPilot)& pilot)
{
  //        ****    TReader -> TProcess         ****
  const Handle(XSControl_TransferReader) &TR = XSDRAW::Session(pilot)->TransferReader();
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  if (TR.IsNull()) sout<<" No TransferReader"<<endl;
  else if (TR->TransientProcess().IsNull()) sout<<" Transfer Reader without Process"<<endl;
  return IFSelect_RetVoid;
}


//=======================================================================
//function : tptr
//=======================================================================
static IFSelect_ReturnStatus XSControl_tptr(const Handle(IFSelect_SessionPilot)& pilot)
{
  //        ****    TProcess -> TReader         ****
  XSDRAW::Session(pilot)->InitTransferReader (3);
  return IFSelect_RetDone;
}


//=======================================================================
//function : twmode
//=======================================================================
static IFSelect_ReturnStatus XSControl_twmode(const Handle(IFSelect_SessionPilot)& pilot)
{
  Standard_Integer argc = pilot->NbWords();
  const Standard_CString arg1 = pilot->Arg(1);
  //        ****    twmode         ****
  Handle(XSControl_TransferWriter) TW = XSDRAW::Session(pilot)->TransferWriter();
  Handle(XSControl_Controller) control = XSDRAW::Session(pilot)->NormAdaptor();
  Standard_Integer modemin,modemax;
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  if (control->ModeWriteBounds (modemin,modemax)) {
    sout<<"Write Mode : allowed values  "<<modemin<<" to "<<modemax<<endl;
    for (Standard_Integer modd = modemin; modd <= modemax; modd ++) {
      sout<<modd<<"	: "<<control->ModeWriteHelp (modd)<<endl;;
    }
  }
  sout<<"Write Mode : actual = "<<TW->TransferMode()<<endl;
  if (argc <= 1) return IFSelect_RetVoid;
  Standard_Integer mod = atoi(arg1);
  sout<<"New value -> "<<arg1<<endl;
  TW->SetTransferMode(mod);
  if (!control->IsModeWrite (mod)) sout<<"Warning : this new value is not supported"<<endl;
  return IFSelect_RetDone;
}


//=======================================================================
//function : twstat
//=======================================================================
static IFSelect_ReturnStatus XSControl_twstat(const Handle(IFSelect_SessionPilot)& pilot)
{
  const Handle(Transfer_FinderProcess) &FP = XSDRAW::Session(pilot)->TransferWriter()->FinderProcess();
  //        ****    twstat        ****
  //  Pour Write
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  if (!FP.IsNull()) {
    sout<<"TransferWrite:";
    FP->PrintStats (sout);
  }
  else sout<<"TransferWrite: not defined"<<endl;
  return IFSelect_RetVoid;
}


//=======================================================================
//function : settransfert
//=======================================================================
static IFSelect_ReturnStatus XSControl_settransfert(const Handle(IFSelect_SessionPilot)& pilot)
{
  //        ****    SelectForTransfer           ****
  return pilot->RecordItem(new XSControl_SelectForTransfer(XSDRAW::Session(pilot)->TransferReader()));
}


//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void XSDRAW_ControlFunctions::Init ()
{
  static int initactor = 0;
  if (initactor) return;  initactor = 1;

  XSDRAW_Activator::SetGroup("DE: General");

  XSDRAW_Activator::AddFunc ("xinit","[norm:string to change norme] reinitialises according to the norm",XSControl_xinit);
  XSDRAW_Activator::AddFunc ("xnorm","displays current norm   +norm : changes it",XSControl_xnorm);

  XSDRAW_Activator::AddFunc ("newmodel","produces a new empty model, for the session",XSControl_newmodel);

  XSDRAW_Activator::AddFunc ("tpclear","Clears  TransferProcess (READ)",XSControl_tpclear);
  XSDRAW_Activator::AddFunc ("twclear","Clears  TransferProcess (WRITE)",XSControl_tpclear);

  XSDRAW_Activator::AddFunc ("tpstat","Statistics on TransferProcess (READ)",XSControl_tpstat);

  XSDRAW_Activator::AddFunc ("tpent","[num:integer] Statistics on an entity of the model (READ)",XSControl_tpent);

  XSDRAW_Activator::AddFunc ("tpitem","[num:integer] Statistics on ITEM of transfer (READ)"    ,XSControl_tpitem);
  XSDRAW_Activator::AddFunc ("tproot","[num:integer] Statistics on a ROOT of transfert (READ)" ,XSControl_tpitem);
  XSDRAW_Activator::AddFunc ("twitem","[num:integer] Statistics on an ITEM of transfer (WRITE)"   ,XSControl_tpitem);
  XSDRAW_Activator::AddFunc ("twroot","[num:integer] Statistics on a ROOT of transfer (WRITE)",XSControl_tpitem);

  XSDRAW_Activator::AddFunc ("trecord","record : all root results; or num : for entity n0.num",XSControl_trecord);
  XSDRAW_Activator::AddFunc ("trstat","general statistics;  or num : stats on entity n0 num",XSControl_trstat);
  XSDRAW_Activator::AddFunc ("trbegin","begin-transfer-reader [init]",XSControl_trbegin);
  XSDRAW_Activator::AddFunc ("tread","transfers all roots, or num|sel|sel num : entity list, by transfer-reader",XSControl_tread);

  XSDRAW_Activator::AddFunc ("trtp","feeds commands tp... with results from tr...",XSControl_trtp);
  XSDRAW_Activator::AddFunc ("tptr","feeds tr... from tp... (may be incomplete)",XSControl_tptr);

  XSDRAW_Activator::AddFunc ("twmode","displays mode transfer write, + num  changes it",XSControl_twmode);
  XSDRAW_Activator::AddFunc ("twstat","Statistics on TransferProcess (WRITE)",XSControl_twstat);

  XSDRAW_Activator::AddFSet ("selecttransfer","selection (recognize from transfer actor)",XSControl_settransfert);
}
