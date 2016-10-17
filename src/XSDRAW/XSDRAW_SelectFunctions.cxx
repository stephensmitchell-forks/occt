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

//#58 rln 28.12.98 Versioning

#include <IFSelect_CheckCounter.hxx>
#include <IFSelect_GraphCounter.hxx>
#include <IFSelect_IntParam.hxx>
#include <IFSelect_SelectDeduct.hxx>
#include <IFSelect_SelectDiff.hxx>
#include <IFSelect_SelectEntityNumber.hxx>
#include <IFSelect_SelectErrorEntities.hxx>
#include <IFSelect_SelectIncorrectEntities.hxx>
#include <IFSelect_SelectIntersection.hxx>
#include <IFSelect_Selection.hxx>
#include <IFSelect_SelectModelEntities.hxx>
#include <IFSelect_SelectModelRoots.hxx>
#include <IFSelect_SelectPointed.hxx>
#include <IFSelect_SelectRange.hxx>
#include <IFSelect_SelectRoots.hxx>
#include <IFSelect_SelectShared.hxx>
#include <IFSelect_SelectSharing.hxx>
#include <IFSelect_SelectSignature.hxx>
#include <IFSelect_SelectSuite.hxx>
#include <IFSelect_SelectUnion.hxx>
#include <IFSelect_SelectUnknownEntities.hxx>
#include <IFSelect_Signature.hxx>
#include <IFSelect_SignatureList.hxx>
#include <IFSelect_SignCounter.hxx>
#include <IFSelect_SignType.hxx>
#include <Interface_Category.hxx>
#include <Interface_CheckIterator.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_InterfaceModel.hxx>
#include <Interface_Macros.hxx>
#include <Interface_MSG.hxx>
#include <Interface_Static.hxx>
#include <Interface_Version.hxx>
#include <Message.hxx>
#include <Message_Messenger.hxx>
#include <Standard_Transient.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_HAsciiString.hxx>
#include <TColStd_HSequenceOfAsciiString.hxx>
#include <TColStd_HSequenceOfHAsciiString.hxx>
#include <TColStd_HSequenceOfTransient.hxx>
#include <TColStd_MapOfInteger.hxx>
#include <XSControl_Controller.hxx>
#include <XSDRAW.hxx>
#include <XSDRAW_SelectFunctions.hxx>
#include <XSDRAW_SessionPilot.hxx>
#include <IFSelect_WorkSession.hxx>


#include <stdio.h>

//  Functions definit un certain nombre de commandes
//  enregistrees dans le Dictionnaire de Activator (par des Act unitaires)
//  Les actions elles-memes sont regroupees en fin de fichier

//  Les definitions

static Interface_ReturnStatus funstatus
  (const Handle(XSDRAW_SessionPilot)& )
{
//        ****    Version & cie     ****
  //#58 rln
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  sout<<"Processor Version : "<<XSTEP_PROCESSOR_VERSION<<endl;
  sout<<"OL Version        : "<<XSTEP_SYSTEM_VERSION<<endl;
  sout<<"Configuration     : "<<XSTEP_Config<<endl;
  sout<<"UL Names          : "<<XSTEP_ULNames<<endl;
  return Interface_RetVoid;
}

static Interface_ReturnStatus fun3
  (const Handle(XSDRAW_SessionPilot)& pilot)
{
  const Handle(IFSelect_WorkSession) &WS = pilot->Session();
  Standard_Integer argc = pilot->NbWords();
  const Standard_CString arg1 = pilot->Arg(1);
//        ****    XRead / Load         ****
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  if (argc < 2) { sout<<"Read/Load : give file name !"<<endl; return Interface_RetError; }
  if (WS->NormAdaptor()->Protocol().IsNull()) { sout<<"Protocol not defined"<<endl; return Interface_RetError; }
  if (WS->NormAdaptor().IsNull()) { sout<<"Controller not defined"<<endl; return Interface_RetError; }

  Interface_ReturnStatus status = WS->ReadFile (arg1);
// status : 0 OK, 1 erreur lecture, 2 Fail(try/catch),
//          -1 fichier non trouve, -2 lecture faite mais resultat vide
  switch (status) {
    case Interface_RetVoid  : sout<<"file:"<<arg1<<" gives empty result"<<endl; break;
    case Interface_RetError : sout<<"file:"<<arg1<<" could not be opened"<<endl; break;
    case Interface_RetDone  : sout<<"file:"<<arg1<<" read"<<endl; break;
    case Interface_RetFail  : sout<<"file:"<<arg1<<" : error while reading"<<endl; break;
    case Interface_RetStop  : sout<<"file:"<<arg1<<" : EXCEPTION while reading"<<endl; break;
    default : sout<<"file:"<<arg1<<" could not be read"<<endl; break;
  }
  if (status != Interface_RetDone) return status;
  return status;
}

static Interface_ReturnStatus fun7
  (const Handle(XSDRAW_SessionPilot)& pilot)
{
  const Handle(IFSelect_WorkSession) &WS = pilot->Session();
  Standard_Integer argc = pilot->NbWords();
  const Standard_CString arg1 = pilot->Arg(1);
//        ****    Entity Label       ****
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  if (argc < 2) { sout<<"Give entity number"<<endl; return Interface_RetError; }
  if (WS->Model().IsNull()) { sout<<"No loaded model, abandon"<<endl; return Interface_RetError; }
  Standard_Integer nument = WS->NumberFromLabel (arg1);
  if (nument <= 0 || nument > WS->NbStartingEntities())
    { sout<<"Not a suitable number: "<<arg1<<endl;  return Interface_RetError; }
  sout<<"N0."<<nument<<" ->Label in Model : ";
  WS->Model()->PrintLabel(WS->StartingEntity(nument),sout);  
  sout<<endl;
  return Interface_RetVoid;
}

static Interface_ReturnStatus fun8
  (const Handle(XSDRAW_SessionPilot)& pilot)
{
  const Handle(IFSelect_WorkSession) &WS = pilot->Session();
  Standard_Integer argc = pilot->NbWords();
  const Standard_CString arg1 = pilot->Arg(1);
//        ****    Entity Number      ****
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  if (argc < 2) { sout<<"Give label to search"<<endl; return Interface_RetError; }
  if (WS->Model().IsNull()) { sout<<"No loaded model, abandon"<<endl; return Interface_RetError; }
  const Handle(Interface_InterfaceModel) &model = WS->Model();
  Standard_Integer i, cnt = 0;
  Standard_Boolean exact = Standard_False;
  sout<<" **  Search Entity Number for Label : "<<arg1<<endl;
  for (i = model->NextNumberForLabel (arg1, 0, exact)  ; i != 0;
       i = model->NextNumberForLabel (arg1, i, exact)) {
    cnt ++;
    sout<<" **  Found n0/id:"; model->Print (model->Value(i),sout); sout<<endl;
  }
  
  if (cnt == 0) sout<<" **  No Match"<<endl;
  else if (cnt == 1) sout<<" **  1 Match"<<endl;
  else sout<<cnt<<" Matches"<<endl;
  return Interface_RetVoid;
}

static Interface_ReturnStatus fun9
  (const Handle(XSDRAW_SessionPilot)& pilot)
{
//        ****    List Types         ****
  const Handle(IFSelect_WorkSession) &WS = pilot->Session();
  Handle(IFSelect_Signature) signtype = WS->SignType();
  if (signtype.IsNull()) signtype = new IFSelect_SignType;
  Handle(IFSelect_SignCounter) counter =
    new IFSelect_SignCounter(signtype,Standard_False);
  return pilot->ExecuteCounter (counter,1);
}

static Interface_ReturnStatus funcount
  (const Handle(XSDRAW_SessionPilot)& pilot)
{
  const Handle(IFSelect_WorkSession) &WS = pilot->Session();
  Standard_Integer argc = pilot->NbWords();
  const Standard_CString arg0 = pilot->Arg(0);
  const Standard_CString arg1 = pilot->Arg(1);
  Standard_Boolean listmode = (arg0[0] == 'l');
//        ****    List Counter         ****

  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  if (argc < 2) {
    sout<<"Designer signature ou compteur, + facultatif selection + facultatif entite"<<endl;
    sout<<" signature/compteur seul -> tout le modele"<<endl
      <<  " sign/compteur + selection -> cette selection, evaluation normale"<<endl
	<<" sign/compteur + sel + num -> cette selection evaluee sur entite n0 num"<<endl;
    return Interface_RetError;
  }
  DeclareAndCast(IFSelect_SignCounter,counter,WS->NamedItem(arg1));
  if (counter.IsNull()) {
    DeclareAndCast(IFSelect_Signature,signa,WS->NamedItem(arg1));
    if (!signa.IsNull()) counter = new IFSelect_SignCounter(signa,Standard_False,listmode);
  }

//  Ajout : si Selection, on applique un GraphCounter
//   Et en ce cas, on peut en avoir plusieurs : la limite est le mot-cle "on"
  Standard_Integer onflag = 0;
  Standard_Integer i; // svv Jan11 2000 : porting on DEC
  for (i = 2; i < argc; i ++) {
    if (!strcmp (pilot->Arg(i),"on")) { onflag = i; break; }
  }

  Handle(IFSelect_Selection) sel = WS->GiveSelection(arg1);
  DeclareAndCast(IFSelect_SelectDeduct,seld,sel);
  if (!seld.IsNull()) {
//  Si onflag, faire une SelectSuite
    if (onflag > 2) {
      Handle(IFSelect_SelectSuite) suite = new IFSelect_SelectSuite;
      for (i = 1; i < onflag; i ++) {
	sel = WS->GiveSelection(pilot->Arg(i));
	if (!suite->AddInput(sel)) {
	  sout<<"Incorrect definition for applied selection"<<endl;
	  return Interface_RetError;
	}
      }
      seld = suite;
    }

    Handle(IFSelect_GraphCounter) gc = new IFSelect_GraphCounter(Standard_False,listmode);
    gc->SetApplied (seld);
    counter = gc;
  }

  if (counter.IsNull()) {
    sout<<"Neither Counter nor Signature : "<<arg1<<endl;
    return Interface_RetError;
  }

  if (onflag == 0) onflag = 1;
  IFSelect_PrintCount pcm = IFSelect_ListByItem;
  if (arg0[0] == 'c') pcm = IFSelect_CountByItem;
  if (arg0[0] == 's') pcm = IFSelect_CountSummary;
  return pilot->ExecuteCounter (counter,onflag+1, pcm);
}

static Interface_ReturnStatus funsigntype
  (const Handle(XSDRAW_SessionPilot)& pilot)
{
  const Handle(IFSelect_WorkSession) &WS = pilot->Session();
  Standard_Integer argc = pilot->NbWords();
  const Standard_CString arg1 = pilot->Arg(1);
//        ****    Sign Type              ****
  Handle(IFSelect_Signature) signtype = WS->SignType();
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  if (signtype.IsNull()) sout<<"signtype actually undefined"<<endl;
  else {
    Handle(TCollection_HAsciiString) str = WS->Name (signtype);
    Standard_Integer id = WS->ItemIdent (signtype);
    sout<<signtype->Label()<<endl;
    if (str.IsNull()) {
      if (id > 0) sout<<"signtype : item n0 "<<id<<endl;
    } else {
      sout<<"signtype : also named as "<<str->ToCString()<<endl;
    }
  }
  if (argc < 2) sout<<"signtype newitem  to change, signtype . to clear"<<endl;
  else {
    if (arg1[0] == '.' && arg1[1] == '\0') {
      signtype.Nullify();
      sout<<"signtype now cleared"<<endl;
    } else {
      signtype = GetCasted(IFSelect_Signature,WS->NamedItem(arg1));
      if (signtype.IsNull()) { sout<<"Not a Signature : "<<arg1<<endl; return Interface_RetError; }
      else sout<<"signtype now set to "<<arg1<<endl;
    }
    WS->SetSignType(signtype);
    return Interface_RetDone;
  }
  return Interface_RetVoid;
}

static Interface_ReturnStatus funsigncase
  (const Handle(XSDRAW_SessionPilot)& pilot)
{
  const Handle(IFSelect_WorkSession) &WS = pilot->Session();
  const Standard_CString arg1 = pilot->Arg(1);
//        ****    Sign Case              ****
  Handle(IFSelect_Signature) signcase = GetCasted(IFSelect_Signature,WS->NamedItem(arg1));
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  if (signcase.IsNull()) sout<<"Not a Signature : "<<arg1<<endl;
  else {
    Standard_Boolean hasmin,hasmax;  Standard_Integer valmin,valmax;
    if (signcase->IsIntCase(hasmin,valmin,hasmax,valmax)) {
      sout<<"Signature "<<arg1<<" : Integer Case";
      if (hasmin) sout<<" - Mini:"<<valmin;
      if (hasmax) sout<<" - Maxi:"<<valmax;
      sout<<endl;
    }
    Handle(TColStd_HSequenceOfAsciiString) caselist = signcase->CaseList();
    if (caselist.IsNull()) sout<<"Signature "<<arg1<<" : no predefined case, see command  count "<<arg1<<endl;
    else {
      Standard_Integer i, nb = caselist->Length();
      sout<<"Signature "<<arg1<<" : "<<nb<<" basic cases :"<<endl;
      for (i = 1; i <= nb; i ++) sout<<"  "<<caselist->Value(i);
      sout<<endl;
    }
  }
  return Interface_RetVoid;
}


static Interface_ReturnStatus fun10
  (const Handle(XSDRAW_SessionPilot)& pilot)
{
  const Handle(IFSelect_WorkSession) &WS = pilot->Session();
  Standard_Integer argc = pilot->NbWords();
  const Standard_CString arg1 = pilot->Arg(1);
//        ****    Entity Status          ****
  Standard_Integer i,nb;
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  if (argc < 2) {
    nb = Interface_Category::NbCategories();
    sout<<" Categories defined :"<<nb<<" i.e. :\n";
    for (i = 0; i <= nb; i ++)
      sout<<"Cat."<<i<<"  : "<<Interface_Category::Name(i)<<"\n";
    sout<<" On a given entity : give its number"<<endl;
    return Interface_RetVoid;
  }
  Standard_Integer num = pilot->Number(arg1);
  if (num <= 0 || num > WS->NbStartingEntities())
    { sout<<"Not a suitable entity number : "<<arg1<<endl; return Interface_RetError; }
  Handle(Standard_Transient) ent = WS->StartingEntity(num);
  WS->PrintEntityStatus(ent,sout);
  return Interface_RetVoid;
}

static Interface_ReturnStatus fun11
  (const Handle(XSDRAW_SessionPilot)& pilot)
{
  const Handle(IFSelect_WorkSession) &WS = pilot->Session();
  const Standard_CString arg1 = pilot->Arg(1);
//        ****    DumpModel (Data)  ****
  Standard_Integer niv = 0;
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  switch (arg1[0]) {
    case '?' :
     sout<<"? for this help, else give a listing mode (first letter suffices) :\n"
        <<" general    General Statistics\n roots    Roots\n"
	<<" entities   All Entities\n"
	<<" listfails  CheckList (fails)    per entity\n"
	<<" messages   CheckList (complete) per entity\n"
	<<" fails      CheckList (fails)    per message (counting)\n"
	<<" check      CheckList (complete) per message (counting)\n"
	<<" totalcheck CheckList (complete) per message (listing n0 ents)\n"
	<<" FAILS      CheckList (fails)    per message (listing complete)\n"
	<<" TOTALCHECK CheckList (complete) per message (listing complete)"<<endl;
     return Interface_RetVoid;
    case 'g' : niv = 0; break;
    case 'r' : niv = 1; break;
    case 'e' : niv = 2; break;
    case 'l' : niv = 3; break;
    case 'm' : niv = 4; break;
    case 'c' : niv = 5; break;
    case 't' : niv = 6; break;
    case 'T' : niv = 7; break;
    case 'f' : niv = 8; break;
    case 'F' : niv =10; break;
    default  : sout<<"Unknown Mode .  data tout court pour help"<<endl; return Interface_RetError;
  }
  WS->TraceDumpModel(niv);
  return Interface_RetVoid;
}

static Interface_ReturnStatus fundumpent
  (const Handle(XSDRAW_SessionPilot)& pilot)
{
  const Handle(IFSelect_WorkSession) &WS = pilot->Session();
  Standard_Integer argc = pilot->NbWords();
  const Handle(XSControl_Controller) &WL = WS->NormAdaptor();
  Standard_Integer levdef=0,levmax=10,level;
  WL->DumpLevels (levdef,levmax);
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  if (argc < 2 || (argc == 2 && levmax < 0)) {
    sout<<"Give n0 or id of entity";
    if (levmax < 0) sout<<"  and dump level"<<endl;
    else sout<<"  + optinal, dump level in [0 - "<<levmax<<"] , default = "<<levdef<<endl;
    for (level = 0; level <= levmax; level ++) {
      Standard_CString help = WL->DumpHelp (level);
      if (help[0] != '\0') sout<<level<<" : "<<help<<endl;
    }
    return Interface_RetError;
  }

  const Standard_CString arg1 = pilot->Arg(1);
  const Standard_CString arg2 = pilot->Arg(2);
  Standard_Integer num = pilot->Number(arg1);
  if (num == 0) return Interface_RetError;
  level = levdef;
  if (argc > 2) level = atoi(arg2);
  Handle(Standard_Transient) ent = WS->StartingEntity(num);
  if ( ent.IsNull() ) {
    sout << "No entity with given id " << arg1 << " (" << num << ") is found in the current model" << endl;
  }
  else {
    sout << "  --   DUMP  Entity n0 " << num << "  level " << level << endl;
    WL->DumpEntity (WS->Model(),WS->NormAdaptor()->Protocol(),ent,sout,level);

    Interface_CheckIterator chl = WS->CheckOne (ent);
    if (!chl.IsEmpty(Standard_False)) chl.Print(sout,WS->Model(),Standard_False);
  }
//  sout << flush;

  return Interface_RetVoid;
}

static Interface_ReturnStatus funsign
  (const Handle(XSDRAW_SessionPilot)& pilot)
{
  const Handle(IFSelect_WorkSession) &WS = pilot->Session();
  Standard_Integer argc = pilot->NbWords();
  const Standard_CString arg1 = pilot->Arg(1);
  const Standard_CString arg2 = pilot->Arg(2);
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  if (argc < 3) { sout<<" Give signature name + n0 or id of entity"<<endl; return Interface_RetError; }
  DeclareAndCast(IFSelect_Signature,sign,WS->NamedItem(arg1));
  if (sign.IsNull()) { sout<<"Not a signature : "<<arg1<<endl; return Interface_RetError; }
  Standard_Integer num = pilot->Number(arg2);
  Handle(Standard_Transient) ent = WS->StartingEntity (num);
  if (num == 0) return Interface_RetError;
  sout<<"Entity n0 "<<num<<" : "<<WS->SignValue(sign,ent)<<endl;
  return Interface_RetVoid;
}

static Interface_ReturnStatus funqp
  (const Handle(XSDRAW_SessionPilot)& pilot)
{
  const Handle(IFSelect_WorkSession) &WS = pilot->Session();
  Standard_Integer argc = pilot->NbWords();
  const Standard_CString arg1 = pilot->Arg(1);
  const Standard_CString arg2 = pilot->Arg(2);
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  if (argc < 3) { sout<<" Give 2 numeros or labels : dad son"<<endl; return Interface_RetError; }
  Standard_Integer n1 = WS->NumberFromLabel(arg1);
  Standard_Integer n2 = WS->NumberFromLabel(arg2);
  sout<<"QueryParent for dad:"<<arg1<<":"<<n1<<" and son:"<<arg2<<":"<<n2<<endl;
  Standard_Integer qp = WS->QueryParent(WS->StartingEntity(n1),WS->StartingEntity(n2));
  if (qp < 0) sout<<arg1<<" is not super-entity of "<<arg2<<endl;
  else if (qp == 0) sout<<arg1<<" is same as "<<arg2<<endl;
  else sout<<arg1<<" is super-entity of "<<arg2<<" , max level found="<<qp<<endl;
//  sout<<" Trouve "<<qp<<endl;
  return Interface_RetVoid;
}

static Interface_ReturnStatus fun13
  (const Handle(XSDRAW_SessionPilot)& pilot)
{
//        ****    ListItems         ****
  pilot->Session()->ListItems(pilot->Arg(1));  return Interface_RetVoid;
}

static Interface_ReturnStatus fun14
  (const Handle(XSDRAW_SessionPilot)& pilot)
{
  Standard_Integer argc = pilot->NbWords();
  const Standard_CString arg1 = pilot->Arg(1);
//        ****    NewInt            ****
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  if (argc < 1) { sout<<"Donner la valeur entiere pour IntParam"<<endl; return Interface_RetError; }
  Handle(IFSelect_IntParam) intpar = new IFSelect_IntParam;
  if (argc >= 1)       intpar->SetValue(atoi(arg1));
  return pilot->RecordItem (intpar);
}

static Interface_ReturnStatus fun15
  (const Handle(XSDRAW_SessionPilot)& pilot)
{
  const Handle(IFSelect_WorkSession) &WS = pilot->Session();
  Standard_Integer argc = pilot->NbWords();
  const Standard_CString arg1 = pilot->Arg(1);
  const Standard_CString arg2 = pilot->Arg(2);
//        ****    SetInt            ****
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  if (argc < 3)
    { sout<<"Donner 2 arguments : nom Parametre et Valeur"<<endl; return Interface_RetError; }
  Standard_Integer val = atoi(arg2);
  DeclareAndCast(IFSelect_IntParam,par,WS->NamedItem(arg1));
  if (!WS->SetIntValue(par,val)) return Interface_RetFail;
  return Interface_RetDone;
}

static Interface_ReturnStatus fun16
  (const Handle(XSDRAW_SessionPilot)& pilot)
{
  Standard_Integer argc = pilot->NbWords();
  const Standard_CString arg1 = pilot->Arg(1);
//        ****    NewText           ****
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  if (argc < 1) { sout<<"Donner la valeur texte pour TextParam"<<endl; return Interface_RetError; }
  Handle(TCollection_HAsciiString) textpar = new TCollection_HAsciiString();
  if (argc >= 1) textpar->AssignCat(arg1);
  return pilot->RecordItem (textpar);
}

static Interface_ReturnStatus fun17
  (const Handle(XSDRAW_SessionPilot)& pilot)
{
  const Handle(IFSelect_WorkSession) &WS = pilot->Session();
  Standard_Integer argc = pilot->NbWords();
  const Standard_CString arg1 = pilot->Arg(1);
  const Standard_CString arg2 = pilot->Arg(2);
//        ****    SetText           ****
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  if (argc < 3)
    { sout<<"Donner 2 arguments : nom Parametre et Valeur"<<endl; return Interface_RetError; }
  DeclareAndCast(TCollection_HAsciiString,par,WS->NamedItem(arg1));
  if (!WS->SetTextValue(par,arg2)) return Interface_RetFail;
  return Interface_RetDone;
}

static Interface_ReturnStatus fun19
  (const Handle(XSDRAW_SessionPilot)& pilot)
{
  const Handle(IFSelect_WorkSession) &WS = pilot->Session();
  Standard_Integer argc = pilot->NbWords();
  const Standard_CString arg1 = pilot->Arg(1);
//        ****    DumpSel           ****
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  if (argc < 2) { sout<<"Give 1 argument : Selection Name"<<endl; return Interface_RetError; }
  WS->DumpSelection (GetCasted(IFSelect_Selection,WS->NamedItem(arg1)));
  return Interface_RetVoid;
}

static Interface_ReturnStatus fun20
  (const Handle(XSDRAW_SessionPilot)& pilot)
{
  const Handle(IFSelect_WorkSession) &WS = pilot->Session();
  Standard_Integer argc = pilot->NbWords();
//        ****    EvalSel           ****
//        ****    GiveList          ****
//        ****    GiveShort GivePointed  ****
//        ****    MakeList          ****
  char mode = pilot->Arg(0)[0];  // givelist/makelist
  if (mode == 'g') mode = pilot->Arg(0)[4];  // l list  s short  p pointed
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  if (argc < 2) { sout<<"Give Entity ID, or Selection Name [+ optional other selection or entity]"<<endl; return Interface_RetError; }

//    MakeList : sur Pointed existante ou a creer
  Handle(IFSelect_SelectPointed) pnt;
  if (mode == 'm') {
    const Standard_CString arg1 = pilot->Arg(1);
    Handle(Standard_Transient) item = WS->NamedItem (arg1);
    pnt = GetCasted(IFSelect_SelectPointed,item);
    if (!pnt.IsNull()) {
      sout<<arg1<<":Already existing Selection for List, cleared then filled"<<endl;
      pnt->Clear();
    } else if (!item.IsNull()) {
      sout<<arg1<<":Already existing Item not for a List, command ignored"<<endl;
      return Interface_RetFail;
    } else {
      pnt = new IFSelect_SelectPointed;
      WS->AddNamedItem (arg1,pnt);
    }
  }

  Handle(TColStd_HSequenceOfTransient) result =
    XSDRAW_SelectFunctions::GiveList (WS,pilot->CommandPart( (mode == 'm' ? 2 : 1) ));
  if (result.IsNull()) return Interface_RetError;
  Interface_EntityIterator iter (result);
  sout<<pilot->CommandPart( (mode == 'm' ? 2 : 1) )<<" : ";
  if      (mode == 'l')   WS->ListEntities (iter,0);
  else if (mode == 's' || mode == 'm') WS->ListEntities (iter,2);
  else if (mode == 'p') {
    sout<<iter.NbEntities()<<" Entities : ";
    for (iter.Start(); iter.More(); iter.Next())
      sout<<" +"<<WS->StartingNumber (iter.Value());
    sout<<endl;
  }

  if (!pnt.IsNull()) {
    pnt->SetList (result);
    sout<<"List set to a SelectPointed : "<<pilot->Arg(1)<<endl;
    sout<<"Later editable by command setlist"<<endl;
  }

  return Interface_RetVoid;
}

static Interface_ReturnStatus fun20c
  (const Handle(XSDRAW_SessionPilot)& pilot)
{
  const Handle(IFSelect_WorkSession) &WS = pilot->Session();
  Standard_Integer argc = pilot->NbWords();
//        ****    GiveCount         ****
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  if (argc < 2) { sout<<"Give Entity ID, or Selection Name [+ optional other selection or entity]"<<endl; return Interface_RetError; }
  Handle(TColStd_HSequenceOfTransient) result =
    XSDRAW_SelectFunctions::GiveList (WS,pilot->CommandPart(1));
  if (result.IsNull()) return Interface_RetError;
  sout<<pilot->CommandPart(1)<<" : List of "<<result->Length()<<" Entities"<<endl;
  return Interface_RetVoid;
}

static Interface_ReturnStatus funselsuite
  (const Handle(XSDRAW_SessionPilot)& pilot)
{
  const Handle(IFSelect_WorkSession) &WS = pilot->Session();
  Standard_Integer argc = pilot->NbWords();
//        ****    SelSuite         ****
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  if (argc < 2) { sout<<"Give Entity ID, or Selection Name [+ optional other selection or entity]"<<endl; return Interface_RetError; }
  Handle(IFSelect_SelectSuite) selsuite = new IFSelect_SelectSuite;

  for (Standard_Integer i = 1; i < argc; i ++) {
    Handle(IFSelect_Selection) sel = WS->GiveSelection(pilot->Arg(i));
    if (!selsuite->AddInput(sel)) {
      sout<<pilot->Arg(i-1)<<" : not a SelectDeduct, no more can be added. Abandon"<<endl;
      return Interface_RetError;
    }
  }
  selsuite->SetLabel (pilot->CommandPart(1));
  return pilot->RecordItem (selsuite);
}

static Interface_ReturnStatus fun21
  (const Handle(XSDRAW_SessionPilot)& pilot)
{
  const Handle(IFSelect_WorkSession) &WS = pilot->Session();
//        ****    ClearItems           ****
  WS->ClearItems();
  return Interface_RetDone;
}

static Interface_ReturnStatus fun22
  (const Handle(XSDRAW_SessionPilot)& pilot)
{
  const Handle(IFSelect_WorkSession) &WS = pilot->Session();
  Standard_Integer argc = pilot->NbWords();
  const Standard_CString arg1 = pilot->Arg(1);
//        ****    ClearData           ****
  Standard_Integer mode = -1;
  if (argc >= 2) {
    if (arg1[0] == 'a') mode = 1;
    if (arg1[0] == 'g') mode = 2;
    if (arg1[0] == 'c') mode = 3;
    if (arg1[0] == 'p') mode = 4;
    if (arg1[0] == '?') mode = -1;
  }
  else mode = 0;
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  if (mode <= 0) {
    if (mode < 0) sout<<"Give a suitable mode";
    sout<<"  Available Modes :\n"
      <<" a : all data    g : graph+check  c : check  p : selectpointed"<<endl;
    return (mode < 0 ? Interface_RetError : Interface_RetVoid);
  }
  WS->ClearData (mode);
  return Interface_RetDone;
}

static Interface_ReturnStatus fun27
  (const Handle(XSDRAW_SessionPilot)& pilot)
{
  const Handle(IFSelect_WorkSession) &WS = pilot->Session();
  Standard_Integer argc = pilot->NbWords();
  const Standard_CString arg1 = pilot->Arg(1);
  const Standard_CString arg2 = pilot->Arg(2);
//        ****    Param(Value)         ****
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  if (argc < 2) {
    Handle(TColStd_HSequenceOfHAsciiString) li = Interface_Static::Items();
    Standard_Integer i,nb = li->Length();
    sout<<" List of parameters : "<<nb<<" items : "<<endl;
    for (i = 1; i <= nb; i ++) {
      sout<<li->Value(i);
      sout<<" : "<<Interface_Static::CVal(li->Value(i)->ToCString())<<endl;
    }
    return Interface_RetVoid;
  } else if (atoi(arg1) > 0) {
    Standard_Integer use = atoi (arg1);
    WS->TraceStatics (use);
  } else {
    if (argc > 2) sout<<"     FORMER STATUS of Static Parameter "<<arg1<<endl;
    else          sout<<"     ACTUAL STATUS of Static Parameter "<<arg1<<endl;
    if (!Interface_Static::IsPresent(arg1)) { sout<<" Parameter "<<arg1<<" undefined"<<endl; return Interface_RetError; }
    if (!Interface_Static::IsSet(arg1)) sout<<" Parameter "<<arg1<<" not valued"<<endl;
    else if (argc == 2) Interface_Static::Static (arg1) -> Print(sout);
    else sout<<" Value : "<<Interface_Static::CVal(arg1)<<endl;

    if (argc == 2) sout<<"To modify, param name_param new_val"<<endl;
    else {
      sout<<" New demanded value : "<<arg2;
      if (Interface_Static::SetCVal (arg1,arg2))
	{  sout<<"   OK"<<endl;  return Interface_RetDone;  }
      else  {  sout <<" , refused"<<endl;  return Interface_RetError;  }
    }
  }
  return Interface_RetVoid;
}

static Interface_ReturnStatus fun28
  (const Handle(XSDRAW_SessionPilot)& pilot)
{
  Standard_Integer argc = pilot->NbWords();
  const Standard_CString arg1 = pilot->Arg(1);
  const Standard_CString arg2 = pilot->Arg(2);
  const Standard_CString arg3 = pilot->Arg(3);
//        ****    DefParam         ****
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  if (argc < 2) {
    Handle(TColStd_HSequenceOfHAsciiString) li = Interface_Static::Items();
    Standard_Integer i,nb = li->Length();
    sout<<" List of parameters : "<<nb<<" items :"<<endl;
    for (i = 1; i <= nb; i ++) sout<<"  "<<li->Value(i)->ToCString();
    sout<<endl<<"  defparam name_param  to known more about one"<<endl;
    sout<<"  defparam nom_param e options  to edit a definition"<<endl;

  } else if (argc == 2) {
    sout<<" Definition of Parameter : "<<arg1<<endl;
    Handle(Interface_Static) unst = Interface_Static::Static (arg1);
    if (unst.IsNull()) sout<<"  undefined"<<endl;
    else unst->Print(sout);
    return Interface_RetVoid;

  } else if (arg2[0] == 'i') {
//  initialisation : arg1=nompar  a2='i'  a3=family  a4=type  [a5=val]
    if (argc < 5) { sout<<" name init family type [valinit]"<<endl; return Interface_RetVoid; }
    char typ = (pilot->Arg(4))[0];
    Standard_Boolean ok= (argc==5 ? Interface_Static::Init(arg3,arg1,typ) :
			  Interface_Static::Init(arg3,arg1,typ,pilot->Arg(5)));
    return (ok ? Interface_RetDone : Interface_RetFail);

  } else if (arg2[0] == 'e') {
//  edition : arg1=nompar  arg2='e' arg3=option  arg4...=parametres option
    char comm[100];
    if (argc < 4) {
      sout<<" give name and options !  Options (according type), 1 a time\n"
	  <<" imin ival / imax ival / rmin rval / rmax rval /\n"
	  <<" enum stnum / enum stnum match / eval e1 e2 e3 ... (maxi 10)\n"
	  <<endl;
      return Interface_RetVoid;
    }
    if (argc > 4) sout<<"Only the command and ONE more arg are considered"<<endl;
    sprintf(comm,"%s %s",pilot->Arg(3),pilot->Arg(4));
    sout<<"Editing parameter : "<<arg1<<" , by command : "<<comm<<endl;

    Handle(Interface_Static) unst = Interface_Static::Static (arg1);
    if (unst.IsNull()) { sout<<arg1<<"  undefined"<<endl; return Interface_RetError; }
    if (Interface_Static::Init(unst->Family(),arg1,'&',comm))
      {  sout<<"Editing done"<<endl;  return Interface_RetDone;  }
    else  {  sout<<"Command not processed : "<<comm<<endl; return Interface_RetFail;  }
  }
  sout<<"Unknown Option : "<<arg2<<endl;
  return Interface_RetVoid;
}

static Interface_ReturnStatus fun70
  (const Handle(XSDRAW_SessionPilot)& pilot)
{
  const Handle(IFSelect_WorkSession) &WS = pilot->Session();
  Standard_Integer argc = pilot->NbWords();
  const Standard_CString arg1 = pilot->Arg(1);
//        ****    SelToggle         ****
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  if (argc < 2) { sout<<"Donner Nom de Selection"<<endl; return Interface_RetError;  }
  DeclareAndCast(IFSelect_Selection,sel,WS->NamedItem(arg1));
  if (!WS->ToggleSelectExtract(sel))
    { sout<<"Pas une SelectExtract : "<<arg1<<endl; return Interface_RetFail;  }
  if (WS->IsReversedSelectExtract(sel)) sout<<arg1<<" a present Reversed"<<endl;
  else sout<<arg1<<" a present Directe"<<endl;
  return Interface_RetDone;
}

static Interface_ReturnStatus fun71
  (const Handle(XSDRAW_SessionPilot)& pilot)
{
  const Handle(IFSelect_WorkSession) &WS = pilot->Session();
  Standard_Integer argc = pilot->NbWords();
  const Standard_CString arg1 = pilot->Arg(1);
  const Standard_CString arg2 = pilot->Arg(2);
//        ****    SelInput          ****
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  if (argc < 3) { sout<<"Donner Noms Selections cible et input"<<endl; return Interface_RetError; }
  DeclareAndCast(IFSelect_Selection,sel,WS->NamedItem(arg1));
  DeclareAndCast(IFSelect_Selection,sou,WS->NamedItem(arg2));
  if (sel.IsNull() || sou.IsNull())
    {  sout<<"Incorrect : "<<arg1<<","<<arg2<<endl;  return Interface_RetError;  }
  if (!WS->SetInputSelection(sel,sou)) { 
    sout<<"Nom incorrect ou Selection "<<arg1<<" ni Extract ni Deduct"<<endl;
    return Interface_RetFail;
  }
  return Interface_RetDone;
}

static Interface_ReturnStatus fun72
  (const Handle(XSDRAW_SessionPilot)& pilot)
{
//        ****    SelModelRoots     ****
  return pilot->RecordItem (new IFSelect_SelectModelRoots);
}

static Interface_ReturnStatus fun73
  (const Handle(XSDRAW_SessionPilot)& pilot)
{
  const Handle(IFSelect_WorkSession) &WS = pilot->Session();
  Standard_Integer argc = pilot->NbWords();
  const Standard_CString arg1 = pilot->Arg(1);
  const Standard_CString arg2 = pilot->Arg(2);
//        ****    SelRange          ****
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  if (argc >= 2 && arg1[0] == '?') argc = 1;
  if (argc < 2) {
    sout<<"Donner la description du SelectRange"
      <<"    Formes admises :\n <n1> <n2>  : Range de <n1> a <n2>\n"
      <<" <n1> tout seul : Range n0 <n1>\n  from <n1>  : Range From <n1>\n"
      <<"  until <n2> : Range Until <n2>"<<endl;
    return Interface_RetVoid;
  }

  Handle(IFSelect_IntParam) low,up;
  Handle(IFSelect_SelectRange) sel;
//                                         Range From
  if (pilot->Word(1).IsEqual("from")) {
    if (argc < 3) { sout<<"Forme admise : from <i>"<<endl; return Interface_RetError; }
    low = GetCasted(IFSelect_IntParam,WS->NamedItem(arg2));
    sel = new IFSelect_SelectRange;
    sel->SetFrom (low);
//                                         Range Until
  } else if (pilot->Word(1).IsEqual("until")) {
    if (argc < 3) { sout<<"Forme admise : until <i>"<<endl; return Interface_RetError; }
    up  = GetCasted(IFSelect_IntParam,WS->NamedItem(arg2));
    sel = new IFSelect_SelectRange;
    sel->SetUntil (up);
//                                         Range One (n-th)
  } else if (argc < 3) {
    low = GetCasted(IFSelect_IntParam,WS->NamedItem(arg1));
    sel = new IFSelect_SelectRange;
    sel->SetOne (low);
//                                         Range (from-to)
  } else {
    low = GetCasted(IFSelect_IntParam,WS->NamedItem(arg1));
    up  = GetCasted(IFSelect_IntParam,WS->NamedItem(arg2));
    sel = new IFSelect_SelectRange;
    sel->SetRange (low,up);
  }
  return pilot->RecordItem (sel);
}

static Interface_ReturnStatus fun74
  (const Handle(XSDRAW_SessionPilot)& pilot)
{
//        ****    SelRoots          ****
  return pilot->RecordItem (new IFSelect_SelectRoots);
}

static Interface_ReturnStatus fun75
  (const Handle(XSDRAW_SessionPilot)& pilot)
{
//        ****    SelShared         ****
  return pilot->RecordItem (new IFSelect_SelectShared);
}

static Interface_ReturnStatus fun76
  (const Handle(XSDRAW_SessionPilot)& pilot)
{
  const Handle(IFSelect_WorkSession) &WS = pilot->Session();
  Standard_Integer argc = pilot->NbWords();
  const Standard_CString arg1 = pilot->Arg(1);
  const Standard_CString arg2 = pilot->Arg(2);
//        ****    SelDiff           ****
  Handle(IFSelect_Selection) sel = new IFSelect_SelectDiff;
  if (sel.IsNull()) return Interface_RetFail;
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  if (argc < 3) sout<<"Diff sans input : ne pas oublier de les definir (ctlmain, ctlsec)!"<<endl;
  DeclareAndCast(IFSelect_Selection,selmain,WS->NamedItem(arg1));
  DeclareAndCast(IFSelect_Selection,selsec ,WS->NamedItem(arg2));
  if (argc >= 2)
    if (!WS->SetControl(sel,selmain,Standard_True))
      sout<<"Echec ControlMain:"<<arg1<<" , a refaire (ctlmain)"<<endl;
  if (argc >= 3)
    if (!WS->SetControl(sel,selsec,Standard_False))
      sout<<"Echec ControlSecond:"<<arg2<<" , a refaire (ctlsec)"<<endl;
  return pilot->RecordItem (sel);
}

static Interface_ReturnStatus fun77
  (const Handle(XSDRAW_SessionPilot)& pilot)
{
  const Handle(IFSelect_WorkSession) &WS = pilot->Session();
  Standard_Integer argc = pilot->NbWords();
  const Standard_CString arg1 = pilot->Arg(1);
  const Standard_CString arg2 = pilot->Arg(2);
//        ****    SelControlMain       ****
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  if (argc < 3) { sout<<"Donner Noms de Control et MainInput"<<endl; return Interface_RetError; }
  DeclareAndCast(IFSelect_Selection,sel,WS->NamedItem(arg1));
  DeclareAndCast(IFSelect_Selection,selmain,WS->NamedItem(arg2));
  if (WS->SetControl(sel,selmain,Standard_True)) return Interface_RetDone;
  sout<<"Nom incorrect ou Selection "<<arg1<<" pas de type Control"<<endl;
  return Interface_RetFail;
}

static Interface_ReturnStatus fun78
  (const Handle(XSDRAW_SessionPilot)& pilot)
{
  const Handle(IFSelect_WorkSession) &WS = pilot->Session();
  Standard_Integer argc = pilot->NbWords();
  const Standard_CString arg1 = pilot->Arg(1);
  const Standard_CString arg2 = pilot->Arg(2);
//        ****    SelControlSecond       ****
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  if (argc < 3) { sout<<"Donner Noms de Control et SecondInput"<<endl; return Interface_RetError; }
  DeclareAndCast(IFSelect_Selection,sel,WS->NamedItem(arg1));
  DeclareAndCast(IFSelect_Selection,seldif,WS->NamedItem(arg2));
  if (WS->SetControl(sel,seldif,Standard_False))  return Interface_RetDone;
  sout<<"Nom incorrect ou Selection "<<arg1<<" pas de type Control"<<endl;
  return Interface_RetFail;
}

static Interface_ReturnStatus fun79
  (const Handle(XSDRAW_SessionPilot)& pilot)
{
//        ****    SelModelAll       ****
  return pilot->RecordItem (new IFSelect_SelectModelEntities);
}

static Interface_ReturnStatus fun80
  (const Handle(XSDRAW_SessionPilot)& pilot)
{
  const Handle(IFSelect_WorkSession) &WS = pilot->Session();
  Standard_Integer argc = pilot->NbWords();
  const Standard_CString arg1 = pilot->Arg(1);
  const Standard_CString arg2 = pilot->Arg(2);
//        ****    SelCombAdd        ****
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  if (argc < 3) { sout<<"Donner n0 Combine et une Input"<<endl; return Interface_RetError; }
  DeclareAndCast(IFSelect_Selection,sel,WS->NamedItem(arg1));
  DeclareAndCast(IFSelect_Selection,seladd,WS->NamedItem(arg2));
  if (WS->CombineAdd(sel,seladd)) return Interface_RetDone;
  sout<<"Nom incorrect ou Selection "<<arg1<<" pas Combine"<<endl;
  return Interface_RetFail;
}

static Interface_ReturnStatus fun81
  (const Handle(XSDRAW_SessionPilot)& pilot)
{
  const Handle(IFSelect_WorkSession) &WS = pilot->Session();
  Standard_Integer argc = pilot->NbWords();
  const Standard_CString arg1 = pilot->Arg(1);
  const Standard_CString arg2 = pilot->Arg(2);
//        ****    SelCombRem        ****
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  if (argc < 3) { sout<<"Donner n0 Combine et RANG a supprimer"<<endl; return Interface_RetError; }
  DeclareAndCast(IFSelect_Selection,sel,WS->NamedItem(arg1));
  DeclareAndCast(IFSelect_Selection,inp,WS->NamedItem(arg2));
  if (WS->CombineRemove(sel,inp)) return Interface_RetDone;
  sout<<"Nom incorrect ou Selection "<<arg1<<" ni Union ni Intersection"<<endl;
  return Interface_RetFail;
}

static Interface_ReturnStatus fun82
  (const Handle(XSDRAW_SessionPilot)& pilot)
{
  const Handle(IFSelect_WorkSession) &WS = pilot->Session();
  Standard_Integer argc = pilot->NbWords();
  const Standard_CString arg1 = pilot->Arg(1);
//        ****    SelEntNumber      ****
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  if (argc < 2) { sout<<"Donner Nom IntParam pour n0 Entite"<<endl; return Interface_RetError; }
  DeclareAndCast(IFSelect_IntParam,par,WS->NamedItem(arg1));
  Handle(IFSelect_SelectEntityNumber) sel = new IFSelect_SelectEntityNumber;
  sel->SetNumber(par);
  return pilot->RecordItem (sel);
}

static Interface_ReturnStatus fun83
  (const Handle(XSDRAW_SessionPilot)& pilot)
{
//        ****    SelUnion          ****
  return pilot->RecordItem (new IFSelect_SelectUnion);
}

static Interface_ReturnStatus fun84
  (const Handle(XSDRAW_SessionPilot)& pilot)
{
//        ****    SelIntersection   ****
  return pilot->RecordItem (new IFSelect_SelectIntersection);
}

static Interface_ReturnStatus fun85
  (const Handle(XSDRAW_SessionPilot)& pilot)
{
  Standard_Integer argc = pilot->NbWords();
  const Standard_CString arg1 = pilot->Arg(1);
//        ****    SelTextType Exact ****
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  if (argc < 2) { sout<<"Donner le TYPE a selectionner"<<endl; return Interface_RetError; }
  return pilot->RecordItem (new IFSelect_SelectSignature
			    (new IFSelect_SignType,arg1,Standard_True));
}

static Interface_ReturnStatus fun86
  (const Handle(XSDRAW_SessionPilot)& pilot)
{
//        ****    SelErrorEntities  ****
  return pilot->RecordItem (new IFSelect_SelectErrorEntities);
}
      
static Interface_ReturnStatus fun87
  (const Handle(XSDRAW_SessionPilot)& pilot)
{
//        ****    SelUnknownEntities  **
  return pilot->RecordItem (new IFSelect_SelectUnknownEntities);
}

static Interface_ReturnStatus fun88
  (const Handle(XSDRAW_SessionPilot)& pilot)
{
//        ****    SelSharing        ****
  return pilot->RecordItem (new IFSelect_SelectSharing);
}

static Interface_ReturnStatus fun89
  (const Handle(XSDRAW_SessionPilot)& pilot)
{
  Standard_Integer argc = pilot->NbWords();
  const Standard_CString arg1 = pilot->Arg(1);
//        ****    SelTextType Contain **
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  if (argc < 2) { sout<<"Donner le TYPE a selectionner"<<endl; return Interface_RetError; }
  return pilot->RecordItem (new IFSelect_SelectSignature
			    (new IFSelect_SignType,arg1,Standard_False));
}

static Interface_ReturnStatus fun90
  (const Handle(XSDRAW_SessionPilot)& pilot)
{
//        ****    SelPointed        ****
  Handle(IFSelect_SelectPointed) sp = new IFSelect_SelectPointed;
  if (pilot->NbWords() > 1) {
    Handle(TColStd_HSequenceOfTransient) list = XSDRAW_SelectFunctions::GiveList
    (pilot->Session(),pilot->CommandPart(1));
    if (list.IsNull()) return Interface_RetFail;
    Handle(Message_Messenger) sout = Message::DefaultMessenger();
    sout<<"SelectPointed : "<<list->Length()<<" entities"<<endl;
    sp->AddList (list);
  }
  return pilot->RecordItem (sp);
}

static Interface_ReturnStatus fun91
  (const Handle(XSDRAW_SessionPilot)& pilot)
{
  const Handle(IFSelect_WorkSession) &WS = pilot->Session();
  Standard_Integer argc = pilot->NbWords();
  const Standard_CString arg1 = pilot->Arg(1);
//        ****    SetPointed (edit) / SetList (edit)    ****
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  if (argc < 2) {
    sout<<"Donner NOM SelectPointed + Option(s) :\n"
        <<" aucune : liste des entites pointees\n"
	<<" 0: Clear  +nn ajout entite nn  -nn enleve nn  /nn toggle nn"<<endl;
	return Interface_RetError;
  }
  DeclareAndCast(IFSelect_SelectPointed,sp,WS->NamedItem(arg1));
  if (sp.IsNull()) { sout<<"Pas une SelectPointed:"<<arg1<<endl; return Interface_RetError; }
  const Handle(Interface_InterfaceModel) &model = WS->Model();  // pour Print
  if (argc == 2) {    // listage simple
    Standard_Integer nb = sp->NbItems();
    sout<<" SelectPointed : "<<arg1<<" : "<<nb<<" Items :"<<endl;
    for (Standard_Integer i = 1; i <= nb; i ++) {
      Handle(Standard_Transient) pointed = sp->Item(i);
      Standard_Integer id = WS->StartingNumber(pointed);
      if (id == 0) sout <<" (inconnu)";
      else  {  sout<<"  "; model->Print(pointed,sout);  }
    }
    if (nb > 0) sout<<endl;
    return Interface_RetDone;
  }

  for (Standard_Integer ia = 2; ia < argc ; ia ++) {
    const TCollection_AsciiString argi = pilot->Word(ia);
    Standard_Integer id = pilot->Number(&(argi.ToCString())[1]);
    if (id == 0) {
      if (!argi.IsEqual("0")) sout<<"Incorrect,ignore:"<<argi<<endl;
      else {  sout<<"Clear SelectPointed"<<endl; sp->Clear(); }
    } else if (argi.Value(1) == '-') {
      Handle(Standard_Transient) item = WS->StartingEntity(id);
      if (sp->Remove(item)) sout<<"Removed:no."<<id;
      else sout<<" Echec Remove "<<id;
      sout<<": "; model->Print(item,sout);  sout<<endl;
    } else if (argi.Value(1) == '/') {
      Handle(Standard_Transient) item = WS->StartingEntity(id);
      if (sp->Remove(item)) sout<<"Toggled:n0."<<id;
      else sout<<" Echec Toggle "<<id;
      sout<<": "; model->Print(item,sout);  sout<<endl;
    } else if (argi.Value(1) == '+') {
      Handle(Standard_Transient) item = WS->StartingEntity(id);
      if (sp->Add(item)) sout<<"Added:no."<<id;
      else sout<<" Echec Add "<<id;
      sout<<": "; model->Print(item,sout);  sout<<endl;
    } else {
      sout<<"Ignore:"<<argi<<" , donner n0 PRECEDE de + ou - ou /"<<endl;
    }
  }
  return Interface_RetDone;
}

static Interface_ReturnStatus fun92
  (const Handle(XSDRAW_SessionPilot)& pilot)
{
  const Handle(IFSelect_WorkSession) &WS = pilot->Session();
//        ****    SelIncorrectEntities  ****
  WS->ComputeCheck();
  return pilot->RecordItem (new IFSelect_SelectIncorrectEntities);
}

static Interface_ReturnStatus fun93
  (const Handle(XSDRAW_SessionPilot)& pilot)
{
  Standard_Integer argc = pilot->NbWords();
  const Standard_CString arg1 = pilot->Arg(1);
  const Standard_CString arg2 = pilot->Arg(2);
  const Handle(IFSelect_WorkSession) &WS = pilot->Session();
//        ****    SelSignature        ****
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  if (argc < 3) { sout<<"Give name of Signature or Counter, text + option exact(D) else contains"<<endl; return Interface_RetError; }
  Standard_Boolean exact = Standard_True;
  if (argc > 3) { if (pilot->Arg(3)[0] == 'c') exact = Standard_False; }

  DeclareAndCast(IFSelect_Signature,sign,WS->NamedItem(arg1));
  DeclareAndCast(IFSelect_SignCounter,cnt,WS->NamedItem(arg1));
  Handle(IFSelect_SelectSignature) sel;

  if (!sign.IsNull())     sel = new IFSelect_SelectSignature (sign,arg2,exact);
  else if (!cnt.IsNull()) sel = new IFSelect_SelectSignature (cnt,arg2,exact);
  else { sout<<arg1<<":neither Signature nor Counter"<<endl; return Interface_RetError; }

  return pilot->RecordItem(sel);
}

static Interface_ReturnStatus fun94
  (const Handle(XSDRAW_SessionPilot)& pilot)
{
  Standard_Integer argc = pilot->NbWords();
  const Standard_CString arg1 = pilot->Arg(1);
  const Handle(IFSelect_WorkSession) &WS = pilot->Session();
//        ****    SignCounter        ****
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  if (argc < 2) { sout<<"Donner nom signature"<<endl; return Interface_RetError; }
  DeclareAndCast(IFSelect_Signature,sign,WS->NamedItem(arg1));
  if (sign.IsNull()) { sout<<arg1<<":pas une signature"<<endl; return Interface_RetError; }
  Handle(IFSelect_SignCounter) cnt = new IFSelect_SignCounter (sign,Standard_True,Standard_True);
  return pilot->RecordItem(cnt);
}

static Interface_ReturnStatus funbselected
  (const Handle(XSDRAW_SessionPilot)& pilot)
{
  const Handle(IFSelect_WorkSession) &WS = pilot->Session();
  Standard_Integer argc = pilot->NbWords();
  const Standard_CString arg1 = pilot->Arg(1);
//        ****    NbSelected = GraphCounter        ****
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  if (argc < 2) { sout<<"Donner nom selection (deduction) a appliquer"<<endl; return Interface_RetError; }
  DeclareAndCast(IFSelect_SelectDeduct,applied,WS->GiveSelection(arg1));
  if (applied.IsNull()) { sout<<arg1<<":pas une SelectDeduct"<<endl; return Interface_RetError; }
  Handle(IFSelect_GraphCounter) cnt = new IFSelect_GraphCounter (Standard_True,Standard_True);
  cnt->SetApplied (applied);
  return pilot->RecordItem(cnt);
}

//  #########################################
//  ####    FONCTIONS COMPLEMENTAIRES    ####
//  #########################################

    Handle(Standard_Transient)  XSDRAW_SelectFunctions::GiveEntity
  (const Handle(IFSelect_WorkSession)& WS,
   const Standard_CString name)
{
  Handle(Standard_Transient) ent;  // demarre a Null
  Standard_Integer num = GiveEntityNumber(WS,name);
  if (num > 0) ent = WS->StartingEntity(num);
  return ent;
}

    Standard_Integer  XSDRAW_SelectFunctions::GiveEntityNumber
  (const Handle(IFSelect_WorkSession)& WS,
   const Standard_CString name)
{
  Standard_Integer num = 0;
  if (!name || name[0] == '\0') {
    char ligne[80];  ligne[0] = '\0';
    cin >> ligne;
//    cin.clear();  cin.getline (ligne,79);
    if (ligne[0] == '\0') return 0;
    num    = WS->NumberFromLabel (ligne);
  }
  else num = WS->NumberFromLabel (name);
  return num;
}

    Handle(TColStd_HSequenceOfTransient)  XSDRAW_SelectFunctions::GiveList
  (const Handle(IFSelect_WorkSession)& WS,
   const Standard_CString first, const Standard_CString second)
{
  return WS->GiveList (first,second);
}


//  #########################################
//  ####    INITIALISATIONS              ####
//  #########################################

void  XSDRAW_SelectFunctions::Init ()
{
  static int initactor = 0;
  if (initactor) return;  initactor = 1;

  XSDRAW::SetGroup("DE: General");
  XSDRAW::AddFunc("xstatus","Lists XSTEP Status : Version, System Name ...",funstatus);
  //szv_c1:XSDRAW::AddFunc("handler","Toggle status catch Handler Error of the session",fun1);
  XSDRAW::AddFunc("xload","file:string  : Read File -> Load Model",fun3);
  XSDRAW::AddFunc("xread","file:string  : Read File -> Load Model",fun3);
  //szv_c1:XSDRAW::AddFunc("writeall","file:string  : Write all model (no split)",fun4);
  //szv_c1:XSDRAW::AddFunc("writesel","file:string sel:Selection : Write Selected (no split)",fun5);
  //szv_c1:XSDRAW::AddFunc("writeent","file:string  n1ent n2ent...:integer : Write Entite(s) (no split)",fun6);
  //szv_c1:XSDRAW::AddFunc("writent", "file:string  n1ent n2ent...:integer : Write Entite(s) (no split)",fun6);
  XSDRAW::AddFunc("elabel","nument:integer   : Displays Label Model of an entity",fun7);
  XSDRAW::AddFunc("enum","label:string  : Displays entities n0.s of which Label Model ends by..",fun8);

  XSDRAW::AddFunc("listtypes","List nb entities per type. Optional selection name  else all model",fun9);
  XSDRAW::AddFunc("count","Count : counter [selection]",funcount);
  XSDRAW::AddFunc("listcount","List Counted : counter [selection [nument]]",funcount);
  XSDRAW::AddFunc("sumcount","Summary Counted : counter [selection [nument]]",funcount);
  XSDRAW::AddFunc("signtype","Sign Type [newone]",funsigntype);
  XSDRAW::AddFunc("signcase","signature : displays possible cases",funsigncase);

  XSDRAW::AddFunc("estatus","ent/nument : displays status of an entity",fun10);
  XSDRAW::AddFunc("data","Data (DumpModel); whole help : data tout court",fun11);
  XSDRAW::AddFunc("entity","give n0 ou id of entity [+ level]",fundumpent);
  XSDRAW::AddFunc("signature","signature name + n0/ident entity",funsign);
  XSDRAW::AddFunc("queryparent"," give 2 n0s/labels of entities : dad son",funqp);

  //szv_c1:XSDRAW::AddFunc("dumpshare","Dump Share (dispatches, IntParams)",fun12);
  XSDRAW::AddFunc("listitems","List Items [label else all]  ->Type,Label[,Name]",fun13);
  XSDRAW::AddFSet("integer","value:integer : cree un IntParam",fun14);
  XSDRAW::AddFunc("setint","name:IntParam   newValue:integer  : Change valeur IntParam",fun15);
  XSDRAW::AddFSet("text","value:string  : cree un TextParam",fun16);
  XSDRAW::AddFunc("settext","Name:TextParam  newValue:string   : Change valeur TextParam",fun17);
  XSDRAW::AddFunc("dumpsel","Dump Selection suivi du Nom de la Selection a dumper",fun19);
  XSDRAW::AddFunc("evalsel","name:Selection [num/sel]  : Evalue une Selection",fun20);
  XSDRAW::AddFunc("givelist","num/sel [num/sel ...]  : Evaluates GiveList",fun20);
  XSDRAW::AddFunc("giveshort","num/sel [num/sel ...]  : GiveList in short form",fun20);
  XSDRAW::AddFunc("givepointed","num/sel [num/sel ...]  : GiveList to fill a SelectPointed",fun20);
  XSDRAW::AddFunc("makelist","listname [givelist] : Makes a List(SelectPointed) from GiveList",fun20);
  XSDRAW::AddFunc("givecount","num/sel [num/sel ...]  : Counts GiveList",fun20c);
  XSDRAW::AddFSet("selsuite","sel sel ...  : Creates a SelectSuite",funselsuite);
  XSDRAW::AddFunc("clearitems","Clears all items (selections, dispatches, etc)",fun21);
  XSDRAW::AddFunc("cleardata","mode:a-g-c-p  : Clears all or some data (model, check...)",fun22);

  //szv_c1:XSDRAW::AddFunc("itemlabel","xxx xxx : liste items having this label",fun24);
  //szv_c1:XSDRAW::AddFunc("xsave","filename:string  : sauve items-session",fun25);
  //szv_c1:XSDRAW::AddFunc("xrestore","filename:string  : restaure items-session",fun26);
  XSDRAW::AddFunc("param","nompar:string : displays parameter value; + nompar val : changes it",fun27);
  XSDRAW::AddFunc("defparam","nompar:string : display def. param; also : nompar edit, nompar init",fun28);

  //szv_c1:XSDRAW::AddFunc("sentfiles","Lists files sent from last Load",fun29);
  //szv_c1:XSDRAW::AddFunc("fileprefix","prefix:string    : definit File Prefix",fun30);
  //szv_c1:XSDRAW::AddFunc("fileext","extent:string    : definit File Extension",fun31);
  //szv_c1:XSDRAW::AddFunc("fileroot","disp:Dispatch  root:string  : definit File Root sur un Dispatch",fun32);
  //szv_c1:XSDRAW::AddFunc("filedef","defroot:string   : definit File DefaultRoot",fun33);
  //szv_c1:XSDRAW::AddFunc("evalfile","Evaluation du FileNaming et memorisation",fun34);
  //szv_c1:XSDRAW::AddFunc("clearfile","Efface la liste d'EvalFile",fun35);
  //szv_c1:XSDRAW::AddFunc("xsplit","[disp:Dispatch  sinon tout]  : Split, la grande affaire !",fun36);
  //szv_c1:XSDRAW::AddFunc("remaining","options... : Remaining Entities, help complet par  remaining ?",fun37);
  //szv_c1:XSDRAW::AddFunc("setcontent","sel:Selection mode:k ou r  : Restreint contenu du modele",fun38);

  //szv_c1:XSDRAW::AddFunc("listmodif","List Final Modifiers",fun40);
  //szv_c1:XSDRAW::AddFunc("dumpmodif","modif:Modifier  : Affiche le Statut d'un Modifier",fun41);
  //szv_c1:XSDRAW::AddFunc("modifsel","modif:Modifier [sel:Selection]  : Change/Annule Selection de Modifier",fun42);
  //szv_c1:XSDRAW::AddFunc("setapplied","modif:Modifier [name:un item sinon sortie fichier]  : Applique un Modifier",fun43);
  //szv_c1:XSDRAW::AddFunc("resetapplied","modif:Modifier  : Enleve un Modifier de la sortie fichier",fun44);
  //szv_c1:XSDRAW::AddFunc("modifmove","modif:Modifier M(model)/F(file) avant,apres:integer  : Deplace un Modifier (sortie fichier)",fun45);

  //szv_c1:XSDRAW::AddFunc("dispsel","disp:Dispatch sel:Selection  -> Selection Finale de Dispatch",fun51);
  //szv_c1:XSDRAW::AddFSet("dispone","cree DispPerOne",fun_dispone);
  //szv_c1:XSDRAW::AddFSet("dispglob","cree DispGlobal",fun_dispglob);
  //szv_c1:XSDRAW::AddFSet("dispcount","count:IntParam  : cree DispPerCount",fun_dispcount);
  //szv_c1:XSDRAW::AddFSet("dispfile","files:IntParam  : cree DispPerFiles",fun_dispfiles);
  //szv_c1:XSDRAW::AddFSet("dispsign","sign:Signature  : cree DispPerSignature",fun_dispsign);
  //szv_c1:XSDRAW::AddFunc("dumpdisp","disp:Dispatch   : Affiche le Statut d'un Dispatch",fun56);

  //szv_c1:XSDRAW::AddFunc("xremove","nom  : Remove a Control Item de la Session",fun57);
  //szv_c1:XSDRAW::AddFunc("evaldisp","mode=[0-3]  disp:Dispatch  : Evaluates one or more Dispatch(es)",fun58);
  //szv_c1:XSDRAW::AddFunc("evaladisp","mode=[0-3]  disp:Dispatch [givelist]  : Evaluates a Dispatch (on a GiveList)",fun_evaladisp);
  //szv_c1:XSDRAW::AddFunc("writedisp","filepattern  disp:Dispatch [givelist]  : Writes Entities by Splitting by a Dispatch",fun_writedisp);
  //szv_c1:XSDRAW::AddFunc("evalcomplete","Evaluation Complete de la Repartition",fun59);

  //szv_c1:XSDRAW::AddFunc("runcheck","affiche LastRunCheckList (write,modif)",fun60);
  //szv_c1:XSDRAW::AddFunc("runtranformer","transf:Transformer  : Applique un Transformer",fun61);
  //szv_c1:XSDRAW::AddFSet("copy","cree TransformStandard, option Copy, vide",fun62);
  //szv_c1:XSDRAW::AddFSet("onthespot","cree TransformStandard, option OntheSpot, vide",fun63);
  //szv_c1:XSDRAW::AddFunc("runcopy","modif:ModelModifier [givelist] : Run <modif> via TransformStandard option Copy",fun6465);
  //szv_c1:XSDRAW::AddFunc("runonthespot","modif:ModelModifier [givelist] : Run <modif> via TransformStandard option OnTheSpot",fun6465);
  //szv_c1:XSDRAW::AddFSet("reorder","[f ou t] reordonne le modele",fun66);

  XSDRAW::AddFunc("toggle","sel:Selection genre Extract  : Toggle Direct/Reverse",fun70);
  XSDRAW::AddFunc("input","sel:Selection genre Deduct ou Extract  input:Selection  : Set Input",fun71);
  XSDRAW::AddFSet("modelroots","cree SelectModelRoots",fun72);
  XSDRAW::AddFSet("range","options... : cree SelectRange ...; tout court pour help",fun73);
  XSDRAW::AddFSet("roots","cree SelectRoots (local roots)",fun74);
  XSDRAW::AddFSet("shared","cree SelectShared",fun75);
  XSDRAW::AddFSet("diff","[main:Selection diff:Selection]  : cree SelectDiff",fun76);
  XSDRAW::AddFunc("selmain","sel:Selection genre Control  main:Selection  : Set Main Input",fun77);
  XSDRAW::AddFunc("selsecond","sel:Selection genre Control  sec:Selection   : Set Second Input",fun78);
  XSDRAW::AddFSet("modelall","cree SelectModelAll",fun79);
  XSDRAW::AddFunc("seladd","sel:Selection genre Combine  input:Selection  : Add Selection",fun80);
  XSDRAW::AddFunc("selrem","sel:Selection genre Combine  input:Selection  : Remove Selection",fun81);
  XSDRAW::AddFSet("number","num:IntParam  : Cree SelectEntityNumber",fun82);

  XSDRAW::AddFSet("union","cree SelectUnion (vide), cf aussi combadd, combrem",fun83);
  XSDRAW::AddFSet("intersect","cree SelectIntersection (vide), cf aussi combadd, combrem",fun84);
  XSDRAW::AddFSet("typexact","type:string  : cree SelectTextType Exact",fun85);
  XSDRAW::AddFSet("errors","cree SelectErrorEntities (from file)",fun86);
  XSDRAW::AddFSet("unknown","cree SelectUnknownEntities",fun87);
  XSDRAW::AddFSet("sharing","cree SelectSharing",fun88);
  XSDRAW::AddFSet("typecontain","type:string  : cree SelectTextType Contains",fun89);
  XSDRAW::AddFSet("pointed","cree SelectPointed [num/sel num/sel]",fun90);
  XSDRAW::AddFunc("setpointed","sel:SelectPointed  : edition SelectPointed. tout court pour help",fun91);
  XSDRAW::AddFunc("setlist","sel:SelectPointed  : edition SelectPointed. tout court pour help",fun91);
  XSDRAW::AddFSet("incorrect","cree SelectIncorrectEntities (computed)",fun92);

  XSDRAW::AddFSet("signsel","sign:Signature|cnt:Counter text:string [e(D)|c] : cree SelectSignature",fun93);
  XSDRAW::AddFSet("signcounter","sign:Signature : cree SignCounter",fun94);
  XSDRAW::AddFSet("nbselected","applied:Selection : cree GraphCounter(=NbSelected)",funbselected);

  //szv_c1:XSDRAW::AddFunc("editlist","editor or editform : lists defs + values",fun_editlist);
  //szv_c1:XSDRAW::AddFunc("editvalue","editform paramname [newval or .] : lists-changes a value",fun_editvalue);
  //szv_c1:XSDRAW::AddFunc("editclear","editform [paramname] : clears edition on all or one param",fun_editclear);
  //szv_c1:XSDRAW::AddFunc("editload","editform [entity-id] : loads from model or an entity",fun_editload);
  //szv_c1:XSDRAW::AddFunc("editapply","editform [keep] : applies on loaded data",fun_editapply);
}
