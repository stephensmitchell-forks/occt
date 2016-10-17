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
#include <XSDRAW_SessionPilot.hxx>
#include <Interface_CheckIterator.hxx>
#include <Interface_InterfaceModel.hxx>
#include <Interface_Macros.hxx>
#include <Interface_Static.hxx>
#include <Interface_MSG.hxx>
#include <Message.hxx>
#include <Message_Messenger.hxx>
#include <TColStd_HSequenceOfHAsciiString.hxx>
#include <TColStd_HSequenceOfTransient.hxx>
#include <Transfer_Binder.hxx>
#include <Transfer_FinderProcess.hxx>
#include <Transfer_ResultFromModel.hxx>
#include <Transfer_TransientProcess.hxx>
#include <XSControl_Controller.hxx>
#include <XSSelect_SelectForTransfer.hxx>
#include <IFSelect_WorkSession.hxx>
#include <XSDRAW.hxx>
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
static Interface_ReturnStatus XSControl_xinit(const Handle(XSDRAW_SessionPilot)& pilot)
{
  Standard_Integer argc = pilot->NbWords();
  const Standard_CString arg1 = pilot->Arg(1);
  //        ****    xinit        ****
  if (argc > 1)
    return (pilot->Session()->SelectNorm(arg1) ? Interface_RetDone : Interface_RetFail);
  const Handle(XSControl_Controller) &aCtl = pilot->Session()->NormAdaptor();
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  sout<<"Selected Norm:"<<(aCtl.IsNull()? "" : aCtl->Name())<<endl;
  return Interface_RetVoid;
}


//=======================================================================
//function : xnorm
//=======================================================================
static Interface_ReturnStatus XSControl_xnorm(const Handle(XSDRAW_SessionPilot)& pilot)
{
  Standard_Integer argc = pilot->NbWords();
  const Standard_CString arg1 = pilot->Arg(1);
  //        ****    xnorm        ****
  const Handle(IFSelect_WorkSession) &WS = pilot->Session();
  Handle(XSControl_Controller) control = WS->NormAdaptor();
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  if (argc == 1)
    sout<<"Current Norm. xnorm newnorm to change"<<endl;
  else sout<<"Current Norm :"<<endl;
  if (control.IsNull()) sout<<"no norm currently defined"<<endl;
  else sout<<"  Norm  Name : "<<control->Name()<<endl;
  if (argc == 1) return Interface_RetVoid;

  control = XSControl_Controller::Recorded(arg1);
  if (control.IsNull()) {
    sout<<" No norm named : "<<arg1<<endl;
    return Interface_RetError;
  }

  WS->SetController(control);
  sout<<"new norm : "<<control->Name()<<endl;
  return Interface_RetDone;
}


//=======================================================================
//function : newmodel
//=======================================================================
static Interface_ReturnStatus XSControl_newmodel(const Handle(XSDRAW_SessionPilot)& pilot)
{
  //        ****    newmodel        ****
  if (!pilot->Session()->NewModel().IsNull()) return Interface_RetDone;
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  sout<<"No new Model produced"<<endl;
  return Interface_RetFail;
}


//=======================================================================
//function : tpclear
//=======================================================================
static Interface_ReturnStatus XSControl_tpclear(const Handle(XSDRAW_SessionPilot)& pilot)
{
  //        ****    tpclear/twclear        ****
  const Standard_Boolean modew = (pilot->Word(0).Value(2) == 'w');
  const Handle(Transfer_FinderProcess)    &FP = pilot->Session()->WriterProcess();
  const Handle(Transfer_TransientProcess) &TP = pilot->Session()->ReaderProcess();
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  if (modew) { if(!FP.IsNull()) FP->Clear(); else sout<<"No Transfer Write"<<endl; }
  else       { if(!TP.IsNull()) TP->Clear(); else sout<<"No Transfer Read"<<endl; }
  return Interface_RetDone;
}


//=======================================================================
//function : tpstat
//=======================================================================
static Standard_Integer BinderStatus (const Handle(Transfer_Binder)& binder, char* mess)
{
  Standard_Integer stat = 0;
  mess[0] = '\0';
  if (binder.IsNull())  {  sprintf (mess,"(no data recorded)");  return 0;  }
  Interface_CheckStatus cst = binder->Check()->Status();
  if (cst == Interface_CheckOK) {
    stat = 11;
    if (binder->HasResult()) sprintf(mess,"%s",binder->ResultTypeName());
    else { sprintf(mess,"(no result)"); stat = 1; }
  } else if (cst == Interface_CheckWarning) {
    stat = 12;
    if (binder->HasResult()) sprintf(mess,"%s  (+ warning)",binder->ResultTypeName());
    else { sprintf(mess,"(warning)"); stat = 2; }
  } else if (cst == Interface_CheckFail) {
    stat = 13;
    if (binder->HasResult()) sprintf(mess,"%s  (+ FAIL)",binder->ResultTypeName());
    else { sprintf(mess,"(FAIL)"); stat = 3; }
  }
  return stat;
}

static void PrintPercent(const Handle(Message_Messenger)& sout, const Standard_CString mess,
                         const Standard_Integer nb, const Standard_Integer nl)
{
  if (nb <= 0 || nl == 0) return;
  sout<<"******      "<<mess<<": ";
  if      (nb == nl)       sout<<"100 %"<<endl;
  else if (nb*100/nl == 0) sout<<"< 1 %"<<endl;
  else            sout<<(nb*100/nl < 10 ? "  " : " ")<<nb*100/nl<<" %"<<endl;
}

static void PrintStatsOnList(const Handle(Transfer_TransientProcess)& TP,
                             const Handle(TColStd_HSequenceOfTransient)& list,
                             const Standard_Integer what,
                             const Standard_Integer mode)
{
  Handle(Message_Messenger) sout = TP->Messenger();
  char mess[250];
  if (TP.IsNull()) return;
  if (what == 0) {  TP->PrintStats(sout);  return;  }

  sout<<"\n*******************************************************************\n";
  sout << "******        Statistics on Transfer Process (Read)          ******"<<endl;
  if (what == 1) sout << "******        Individual Transfers  (Roots)                  ******\n";
  if (what == 2) sout << "******        All recorded data about Transfer               ******\n";
  if (what == 3) sout << "******        Abnormal records                               ******\n";
  if (what == 1 || what == 2 || what == 3) {
    if (mode == 0) sout<<"******        (n0s of recorded entities)                     ******\n";
    if (mode == 1) sout<<"******        (per entity : type + result)                   ******\n";
    if (mode == 2) sout<<"******        (per entity : type + result/status)            ******\n";
    if (mode == 3) sout<<"******        (count per type of entity)                     ******\n";
    if (mode == 4) sout<<"******        (count per type of result)                     ******\n";
    if (mode == 5) sout<<"******   (count per couple entity-type / result-type/status) ******\n";
    if (mode == 6) sout<<"******   (list per couple entity-type / result-type/status)  ******\n";
  }
  if (what == 4) sout << "******        Check messages                                 ******\n";
  if (what == 5) sout << "******        Fail  messages                                 ******\n";
  sout<<"*******************************************************************\n";

  //  Cas what = 1,2,3 : contenu du TP (binders)

  const Standard_Boolean nolist = list.IsNull();
  Handle(Interface_InterfaceModel) model = TP->Model();
  if (what >= 1 && what <= 3) {

    Standard_Integer stat;
    Standard_Integer nbv = 0, nbw = 0, nbf = 0, nbr = 0, nbrw = 0, nbrf = 0, nbnr = 0, nbi = 0;
    Transfer_TransientProcess::Iterator itrp(Standard_True);
    if (what == 1) itrp = TP->RootResult(Standard_True);
    if (what == 2) itrp = TP->CompleteResult(Standard_True);
    if (what == 3) itrp = TP->AbnormalResult();
    Standard_Integer i = 0, nb = itrp.Number();
    if (!nolist) itrp.Filter (list);
    Standard_Integer nl = itrp.Number();    // apres filtrage
    Handle(IFSelect_SignatureList) counter;
    if (mode > 2) counter = new IFSelect_SignatureList (mode == 6);
    Standard_Boolean notrec = (!nolist && mode > 2);  // noter les "no record"
    IFSelect_PrintCount pcm = IFSelect_CountByItem;
    if (mode == 6) pcm = IFSelect_ListByItem;

    sout  <<"****        Entities in Model   : "<<model->NbEntities()<<endl;
    sout  <<"****        Nb Items (Transfer) : "<<nb<<endl;
    if (!nolist)
      sout<<"****        Nb Items (Listed)   : "<<nl<<endl;

    for (itrp.Start(); itrp.More(); itrp.Next()) {
      nbi ++;
      Handle(Transfer_Binder) binder = itrp.Value();
      const Handle(Standard_Transient) &ent = itrp.SourceObject();
      if (binder.IsNull())  {
	nbnr ++;
	if (notrec) counter->Add(ent,"(not recorded)");
	else if (mode == 1 || mode == 2) {
	  sout<<"["<<Interface_MSG::Blanks (nbi,4)<<nbi<<" ]:";
	  model->Print (ent,sout);
	  sout<<"   "<<model->TypeName(ent,Standard_False)<<"  (not recorded)"<<endl;
	  continue;
	}
      }
      if (mode == 0)  {  sout<<"  "<<model->Number(ent); continue;  }
      if (mode != 3) {
	stat = BinderStatus(binder,mess);
        // 0 Binder Null.   1 void  2 Warning seul  3 Fail seul
        // 11 Resultat OK. 12 Resultat+Warning. 13 Resultat+Fail
	if (stat ==  0 || stat == 1) nbv ++;
	if (stat ==  2) nbw ++;
	if (stat ==  3) nbf ++;
	if (stat == 11) nbr ++;
	if (stat == 12) nbrw ++;
	if (stat == 13) nbrf ++;
      }

      //  mode : 0 list num;  1 : num+label + type + result (abrege);  2 : complet
      if (mode == 1 || mode == 2) {
	sout<<"["<<Interface_MSG::Blanks (i,4)<<i<<" ]:";
	model->Print (ent,sout);
	sout<<"   "<<model->TypeName(ent,Standard_False);
	sout<<"	Result:"<<mess<<endl;
	if (mode == 1) continue;

	const Handle(Interface_Check)& ch = binder->Check();
	Standard_Integer newi,newnbw = ch->NbWarnings(), newnbf = ch->NbFails();

	if (newnbw > 0) {
	  sout<<" - Warnings : "<<newnbw<<":\n";
	  for (newi = 1; newi <= newnbw; newi ++) sout<<ch->CWarning(newi)<<endl;
	}
	if (newnbf > 0) {
	  sout<<" - Fails : "<<newnbf<<":\n";
	  for (newi = 1; newi <= newnbf; newi ++) sout<<ch->CFail(newi)<<endl;
	}
	continue;
      }

      //  mode : 3, counts per type of starting entity (class type)
      //         4 : counts per result type and/or status
      //         5 : counts per couple (starting type / result type/status)
      //         6 : idem plus gives for each item, the list of numbers of
      //                  entities in the starting model
      if (mode >= 3 && mode <= 6) {
        //IFSelect_PrintCount newpcm = IFSelect_CountByItem;
        //if (mode == 6) newpcm = IFSelect_ListByItem;
	if (mode == 3) counter->Add (ent,model->TypeName(ent,Standard_False));
	if (mode == 4) counter->Add (ent,mess);
	if (mode >= 5) {
	  TCollection_AsciiString mest (model->TypeName(ent,Standard_False));
	  mest.AssignCat("	-> ");
	  mest.AssignCat(mess);
          //sprintf(mest,"%s	-> %s",model->TypeName(ent,Standard_False),mess);
	  counter->Add (ent,mest.ToCString());
	}
      }

      //    Fin de l iteration
    }
    if (!counter.IsNull()) counter->PrintList(sout,model,pcm);
    else sout<<endl;
    //    Pourcentages
    if (mode != 3 && nbi > 0) {
      sout << "******        Percentages according Transfer Status          ******"<<endl;
      PrintPercent (sout,"Result          ",nbr+nbrw,nl);
      PrintPercent (sout,"Result + FAIL   ",nbrf,nl);
      PrintPercent (sout,"FAIL, no Result ",nbf,nl);
      PrintPercent (sout,"Just Warning    ",nbw,nl);
      PrintPercent (sout,"Nothing Recorded",nbnr,nl);
    }
    return;
  }

  //  Cas  what = 4,5 : check-list

  if (what == 4 || what == 5) {

    Interface_CheckIterator chl = TP->CheckList(Standard_False);
    chl.SetName("** TRANSFER READ CHECK **");
    if (mode == 0) chl.Print (sout,model,(what == 5));
    else {
      IFSelect_PrintCount pcm = IFSelect_CountByItem;
      if (mode == 2) pcm = IFSelect_ListByItem;
      Handle(IFSelect_CheckCounter) counter = new IFSelect_CheckCounter(Standard_True);
      counter->Analyse   (chl,model,Standard_True,(what == 5));
      counter->PrintList (sout,model,pcm);
    }
  }

}

static Interface_ReturnStatus XSControl_tpstat(const Handle(XSDRAW_SessionPilot)& pilot)
{
  Standard_Integer argc = pilot->NbWords();
  const Standard_CString arg1 = pilot->Arg(1);
  //const Standard_CString arg2 = pilot->Arg(2);
  const Handle(Transfer_TransientProcess) &TP = pilot->Session()->ReaderProcess();
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  if (TP.IsNull()) { sout<<"No Transfer Read"<<endl; return Interface_RetError;}
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
    if (mod1 < -1) return Interface_RetError;
    return Interface_RetVoid;
  }

  if (!TP.IsNull()) {
    sout<<"TransferRead :";
    if (TP->Model() != pilot->Session()->Model()) sout<<"Model differs from the session";
    Handle(TColStd_HSequenceOfTransient) list =
      XSDRAW_SelectFunctions::GiveList(pilot->Session(),pilot->CommandPart(2));
    PrintStatsOnList (TP,list,mod1,mod2);
  }
  else sout<<"TransferRead : not defined"<<endl;
  return Interface_RetVoid;
}


//=======================================================================
//function : tpent
//=======================================================================
static Interface_ReturnStatus XSControl_tpent(const Handle(XSDRAW_SessionPilot)& pilot)
{
  const Standard_Integer argc = pilot->NbWords();
  //        ****    tpent        ****
  const Handle(Message_Messenger) &sout = Message::DefaultMessenger();
  if (argc < 2) { sout<<"Give ENTITY NUMBER (IN MODEL TransferProcess)"<<endl; return Interface_RetError; }

  const Standard_CString arg1 = pilot->Arg(1);
  Standard_Integer num = atoi(arg1);

  const Handle(Transfer_TransientProcess) &TP = pilot->Session()->ReaderProcess();
  if (TP.IsNull()) { sout<<"No Transfer Process"<<endl; return Interface_RetError; }

  const Handle(Interface_InterfaceModel) &model = TP->Model();
  if (model.IsNull())  return Interface_RetFail;

  if (num <= 0 || num > model->NbEntities()) { sout<<"Number not in [1 - "<<model->NbEntities()<<"]"<<endl; return Interface_RetError; }
  Handle(Standard_Transient) ent = model->Value(num);
  const Standard_Integer index = TP->MapIndex  (ent);
  if (index == 0) { sout<<"Entity "<<num<<"  not recorded in transfer"<<endl; return Interface_RetError; }

  if (!TP->PrintTransferStatus(index,sout))
    sout<<" - Index="<<index<<" incorrect"<<endl;
  return Interface_RetVoid;
}


//=======================================================================
//function : tpitem
//=======================================================================
static Interface_ReturnStatus XSControl_tpitem(const Handle(XSDRAW_SessionPilot)& pilot)
{
  const Standard_Integer argc = pilot->NbWords();
  //        ****    tpitem/tproot/twitem/twroot        ****
  const Handle(Message_Messenger) &sout = Message::DefaultMessenger();
  if (argc < 2) { sout<<"Give ITEM NUMBER (in TransferProcess)"<<endl; return Interface_RetError; }

  const Standard_CString arg1 = pilot->Arg(1);
  Standard_Integer num = atoi(arg1);

  if (pilot->Word(0).Value(3) == 'r') num = -num;

  Handle(Transfer_Process) TP;
  if (pilot->Word(0).Value(2) == 'w') {
    TP = pilot->Session()->WriterProcess();
  }
  else {
    TP = pilot->Session()->ReaderProcess();
  }
  if (TP.IsNull()) { sout<<"No Transfer Process"<<endl; return Interface_RetError; }

  if (!TP->PrintTransferStatus(num,sout))
    sout<<" - Num="<<num<<" incorrect"<<endl;
  return Interface_RetVoid;
}


//=======================================================================
//function : trecord
//=======================================================================
static Interface_ReturnStatus XSControl_trecord(const Handle(XSDRAW_SessionPilot)& pilot)
{
  Standard_Integer argc = pilot->NbWords();
  const Standard_CString arg1 = pilot->Arg(1);
  const Handle(Transfer_TransientProcess) &TP = pilot->Session()->ReaderProcess();
//        ****    trecord : ReaderProcess        ****
  Standard_Boolean tous = (argc == 1);
  Standard_Integer num = -1;
  const Handle(Interface_InterfaceModel) &mdl = pilot->Session()->Model();
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  if (mdl.IsNull() || TP.IsNull())
    { sout<<" init not done"<<endl; return Interface_RetError; }
  if (!tous) num = atoi(arg1);
  //    Enregistrer les racines
  if (tous) {
    Standard_Integer nb = TP->NbRoots();
    sout<<" Recording "<<nb<<" Roots"<<endl;
    for (Standard_Integer i = 1; i <= nb; i ++) {
      if (TP->RecordResult (TP->Root(i))) sout<<" Root n0."<<i<<endl;
      else sout<<" Root n0."<<i<<" not recorded"<<endl;
    }
  } else {
    if (num < 1 ||  num > mdl->NbEntities()) sout<<"incorrect number:"<<num<<endl;
    else if (TP->RecordResult(mdl->Value(num))) sout<<" Entity n0."<<num<<endl;
    else sout<<" Entity n0."<<num<<" not recorded"<<endl;
  }
  return Interface_RetDone;
}


//=======================================================================
//function : trstat
//=======================================================================
static Interface_ReturnStatus XSControl_trstat(const Handle(XSDRAW_SessionPilot)& pilot)
{
  const Handle(IFSelect_WorkSession) &WS = pilot->Session();
  Standard_Integer argc = pilot->NbWords();
  const Standard_CString arg1 = pilot->Arg(1);
  const Handle(Message_Messenger) &sout = Message::DefaultMessenger();
//        ****    trstat : ReaderProcess        ****
  const Handle(Transfer_TransientProcess) &TP = WS->ReaderProcess();
  if (TP.IsNull()) { sout<<" init not done"<<endl; return Interface_RetError; }
  const Handle(Interface_InterfaceModel) &mdl = TP->Model();
  if (mdl.IsNull()) { sout<<" No model"<<endl; return Interface_RetError; }
  sout<<" Statistics : FileName : "<<endl;
  if (argc == 1) {
    // stats generales
    //  A ameliorer ... !
    sout<<"\n*******************************************************************\n";
    sout << "******        Statistics on Transfer (Read)                  ******"<<endl;
    sout<<"\n*******************************************************************\n";
    //  reste  what = 10 : on liste les racines des final results
    sout << "******        Final Results                                  ******"<<endl;
    Handle(TColStd_HSequenceOfTransient) list = TP->RecordedList();
    Standard_Integer i, nb = list->Length();
    sout<<"****    Nb Recorded : "<<nb<<" : entities n0s : ";
    for (i = 1; i <= nb; i ++) {
      Handle(Standard_Transient) ent = list->Value(i);
      sout<<"  "<<mdl->Number(ent);
    }
    sout<<endl;

  } else {
    // stats unitaires
    Standard_Integer num = atoi(arg1);
    if (num < 1 || num > mdl->NbEntities()) { sout<<" incorrect number:"<<arg1<<endl; return Interface_RetError; }
    const Handle(Standard_Transient) &ent = mdl->Value(num);
    if (ent.IsNull()) { sout<<" Entity "<<num<<" is null"<<endl; return Interface_RetError; }
    if (!TP->IsRecorded(ent)) { sout<<" Entity "<<num<<" not recorded"<<endl; return Interface_RetError; }
    Handle(Transfer_ResultFromModel) RM = TP->FinalResult(ent);
    Handle(TColStd_HSequenceOfTransient) list = TP->CheckedList(ent);
    Standard_Integer i, nb = list->Length();
    if (nb > 0) sout<<" Entities implied by Check/Result :"<<nb<<" i.e.:";
    for (i = 1; i <= nb; i ++) { sout<<"  "; mdl->Print(list->Value(i),sout); }
    sout<<endl;
    if (RM.IsNull()) { sout<<" no other info"<<endl; return Interface_RetVoid; }
    Interface_CheckIterator chl = RM->CheckList(Standard_False);
    WS->PrintCheckList(chl,Standard_False,IFSelect_EntitiesByItem);
  }
  return Interface_RetVoid;
}


//=======================================================================
//function : trbegin
//=======================================================================
static Interface_ReturnStatus XSControl_trbegin(const Handle(XSDRAW_SessionPilot)& pilot)
{
  //        ****    trbegin : ReaderProcess        ****
  Handle(Transfer_TransientProcess) TP = pilot->Session()->ReaderProcess();
  Standard_Boolean init = TP.IsNull();
  if (pilot->NbWords() > 1) { if (pilot->Arg(1)[0] == 'i') init = Standard_True; }
  if (init) {
    pilot->Session()->InitTransferReader (0);
    TP = pilot->Session()->ReaderProcess();
    if (TP.IsNull()) { 
      Handle(Message_Messenger) sout = Message::DefaultMessenger();
      sout<<" init not done or failed"<<endl; 
      return Interface_RetError; 
    }
  }
  pilot->Session()->InitTransferReader (4);
  return Interface_RetDone;
}


//=======================================================================
//function : tread
//=======================================================================
static Interface_ReturnStatus XSControl_tread(const Handle(XSDRAW_SessionPilot)& pilot)
{
  const Handle(IFSelect_WorkSession) &WS = pilot->Session();
  Standard_Integer argc = pilot->NbWords();
  //        ****    tread : ReaderProcess        ****
  const Handle(Message_Messenger) &sout = Message::DefaultMessenger();
  Handle(TColStd_HSequenceOfTransient) list;
  if (argc < 2) {
    Handle(Standard_Transient) sel = WS->NamedItem("xst-model-roots");
    if (sel.IsNull()) { sout<<"Select Roots absent"<<endl; return Interface_RetError; }
    list = WS->GiveList(sel);
    sout<<" Transfer of all ("<<list->Length()<<") roots"<<endl;
  } else {
    Handle(TColStd_HSequenceOfTransient) list = XSDRAW_SelectFunctions::GiveList(WS,pilot->CommandPart(1));
    sout<<" Transfer of "<<list->Length()<<" entities"<<endl;
  }
  const Standard_Integer nb = WS->TransferList(list);
  sout<<" Gives "<<nb<<" results"<<endl;
  return Interface_RetDone;
}


//=======================================================================
//function : trtp
//=======================================================================
static Interface_ReturnStatus XSControl_trtp(const Handle(XSDRAW_SessionPilot)& pilot)
{
  //        ****    TReader -> TProcess         ****
  const Handle(Transfer_TransientProcess) &TP = pilot->Session()->ReaderProcess();
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  if (TP.IsNull()) sout<<" No TransferProcess"<<endl;
  return Interface_RetVoid;
}


//=======================================================================
//function : tptr
//=======================================================================
static Interface_ReturnStatus XSControl_tptr(const Handle(XSDRAW_SessionPilot)& pilot)
{
  //        ****    TProcess -> TReader         ****
  pilot->Session()->InitTransferReader (0);

  const Handle(Transfer_TransientProcess) &TP = pilot->Session()->ReaderProcess();
  if (!TP.IsNull()) {
    Standard_Integer i, nb = TP->NbRoots();
    for (i = 1; i <= nb; i ++) TP->RecordResult(TP->Root(i));
  }

  return Interface_RetDone;
}


//=======================================================================
//function : twstat
//=======================================================================
static Interface_ReturnStatus XSControl_twstat(const Handle(XSDRAW_SessionPilot)& pilot)
{
  const Handle(Transfer_FinderProcess) &FP = pilot->Session()->WriterProcess();
  //        ****    twstat        ****
  //  Pour Write
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  if (!FP.IsNull()) {
    sout<<"TransferWrite:";
    FP->PrintStats (sout);
  }
  else sout<<"TransferWrite: not defined"<<endl;
  return Interface_RetVoid;
}


//=======================================================================
//function : settransfert
//=======================================================================
static Interface_ReturnStatus XSControl_settransfert(const Handle(XSDRAW_SessionPilot)& pilot)
{
  //        ****    SelectForTransfer           ****
  return pilot->RecordItem(new XSSelect_SelectForTransfer(pilot->Session()->ReaderProcess()));
}


//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void XSDRAW_ControlFunctions::Init ()
{
  static int initactor = 0;
  if (initactor) return;  initactor = 1;

  XSDRAW::SetGroup("DE: General");

  XSDRAW::AddFunc ("xinit","[norm:string to change norme] reinitialises according to the norm",XSControl_xinit);
  XSDRAW::AddFunc ("xnorm","displays current norm   +norm : changes it",XSControl_xnorm);

  XSDRAW::AddFunc ("newmodel","produces a new empty model, for the session",XSControl_newmodel);

  XSDRAW::AddFunc ("tpclear","Clears  TransferProcess (READ)",XSControl_tpclear);
  XSDRAW::AddFunc ("twclear","Clears  TransferProcess (WRITE)",XSControl_tpclear);

  XSDRAW::AddFunc ("tpstat","Statistics on TransferProcess (READ)",XSControl_tpstat);

  XSDRAW::AddFunc ("tpent","[num:integer] Statistics on an entity of the model (READ)",XSControl_tpent);

  XSDRAW::AddFunc ("tpitem","[num:integer] Statistics on an ITEM of transfer (READ)",XSControl_tpitem);
  XSDRAW::AddFunc ("tproot","[num:integer] Statistics on a ROOT of transfer (READ)",XSControl_tpitem);
  XSDRAW::AddFunc ("twitem","[num:integer] Statistics on an ITEM of transfer (WRITE)",XSControl_tpitem);
  XSDRAW::AddFunc ("twroot","[num:integer] Statistics on a ROOT of transfer (WRITE)",XSControl_tpitem);

  XSDRAW::AddFunc ("trecord","record : all root results; or num : for entity n0.num",XSControl_trecord);
  XSDRAW::AddFunc ("trstat","general statistics;  or num : stats on entity n0 num",XSControl_trstat);
  XSDRAW::AddFunc ("trbegin","begin-transfer-reader [init]",XSControl_trbegin);
  XSDRAW::AddFunc ("tread","transfers all roots, or num|sel|sel num : entity list, by transfer-reader",XSControl_tread);

  XSDRAW::AddFunc ("trtp","feeds commands tp... with results from tr...",XSControl_trtp);
  XSDRAW::AddFunc ("tptr","feeds tr... from tp... (may be incomplete)",XSControl_tptr);

  XSDRAW::AddFunc ("twstat","Statistics on TransferProcess (WRITE)",XSControl_twstat);

  XSDRAW::AddFSet ("selecttransfer","selection (recognize from transfer actor)",XSControl_settransfert);
}
