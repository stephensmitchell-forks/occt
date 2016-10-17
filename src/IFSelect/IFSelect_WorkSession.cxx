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

#include <IFSelect_WorkSession.hxx>

#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <TColStd_HSequenceOfInteger.hxx>
#include <TColStd_MapOfInteger.hxx>
#include <Dico_DictionaryOfTransient.hxx>
#include <Message_Messenger.hxx>
#include <Interface_Macros.hxx>
#include <Interface_GTool.hxx>
#include <Interface_HGraph.hxx>
#include <Interface_MSG.hxx>
#include <Interface_Category.hxx>
#include <Interface_CheckIterator.hxx>
#include <Interface_CheckTool.hxx>
#include <Interface_ShareFlags.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_GeneralModule.hxx>
#include <IFSelect_CheckCounter.hxx>
#include <IFSelect_IntParam.hxx>
#include <IFSelect_SelectPointed.hxx>
#include <IFSelect_SelectionIterator.hxx>
#include <IFSelect_Signature.hxx>
#include <IFSelect_SignCounter.hxx>
#include <IFSelect_SignValidity.hxx>
#include <IFSelect_SelectCombine.hxx>
#include <IFSelect_SelectControl.hxx>
#include <IFSelect_SelectSignature.hxx>
#include <IFSelect_SelectModelEntities.hxx>
#include <IFSelect_Vars.hxx>
#include <XSControl_Controller.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IFSelect_WorkSession,XSControl_WorkSession)

#define Flag_Incorrect 2 //szv_c1: duplicated in XSControl_WorkSession
//  (Bit Map n0 2)

static Standard_Boolean errhand;  //szv_c1: duplicated in XSControl_WorkSession


//=======================================================================
//function : Constructor
//purpose  : 
//=======================================================================

IFSelect_WorkSession::IFSelect_WorkSession ()
: myNames(new Dico_DictionaryOfTransient),
  myVars(new IFSelect_Vars)
{
}


//=======================================================================
//function : SetController
//purpose  : 
//=======================================================================

void IFSelect_WorkSession::SetController (const Handle(XSControl_Controller)& ctl)
{
  ClearItems();

  XSControl_WorkSession::SetController(ctl);
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

void IFSelect_WorkSession::ClearData (const Standard_Integer mode)
{
  switch (mode) {
    case 4 : {
//  MISE A JOUR des SelectPointed  C-A-D  on efface leur contenu
//  AINSI que des editeurs (en fait, les EditForm)
//  Des compteurs  C-A-D  on efface leur contenu (a reevaluer)
      Handle(TColStd_HSequenceOfInteger) list = ItemIdents(STANDARD_TYPE(IFSelect_SelectPointed));
      Standard_Integer nb = list->Length();
      Standard_Integer i; // svv #1 
      for (i = 1; i <= nb; i ++) {
        DeclareAndCast(IFSelect_SelectPointed,sp,Item(list->Value(i)));
        if (!sp.IsNull()) sp->Clear();
      }
      list = ItemIdents(STANDARD_TYPE(IFSelect_SignatureList));
      nb = list->Length();
      for (i = 1; i <= nb; i ++) {
        DeclareAndCast(IFSelect_SignatureList,sl,Item(list->Value(i)));
        if (!sl.IsNull()) sl->Clear();
        DeclareAndCast(IFSelect_SignCounter,sc,sl);
        if (!sc.IsNull()) sc->SetSelMode(-1);
      }
      myItems.Clear();
      break;
    }
	default : XSControl_WorkSession::ClearData(mode);
  }
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

void IFSelect_WorkSession::SetSignType (const Handle(IFSelect_Signature)& signtype)
{
  thegtool->SetSignType (signtype);
  if (signtype.IsNull()) myNames->RemoveItem ("xst-sign-type");
  else myNames->SetItem ("xst-sign-type",signtype);
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Handle(IFSelect_Signature)  IFSelect_WorkSession::SignType () const
{
  return Handle(IFSelect_Signature)::DownCast (thegtool->SignType());
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Standard_Integer IFSelect_WorkSession::NbStartingEntities () const
{
  return (myModel.IsNull()? 0 : myModel->NbEntities());
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Handle(Standard_Transient) IFSelect_WorkSession::StartingEntity (const Standard_Integer num) const
{
  Handle(Standard_Transient) res;  // Null par defaut
  if (myModel.IsNull()) return res;
  if (num < 1 || num > myModel->NbEntities()) return res;
  return myModel->Value(num);
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Standard_Integer IFSelect_WorkSession::StartingNumber (const Handle(Standard_Transient)& ent) const
{
  return (myModel.IsNull()? 0 : myModel->Number(ent));
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Standard_Integer IFSelect_WorkSession::NumberFromLabel
  (const Standard_CString val, const Standard_Integer afternum) const
{
  Standard_Integer i, cnt = 0, num = atoi(val);
  if (num > 0 || myModel.IsNull()) return num;    // un n0 direct : gagne !
//  Sinon, on considere que c est un label; a traiter en CaseNonSensitive ...
  if (num > myModel->NbEntities())  { num = 0; return num; }
  Standard_Boolean exact = Standard_False;
  Standard_Integer after = (afternum >= 0 ? afternum : -afternum);
  for (i = myModel->NextNumberForLabel (val, after, exact)  ; i != 0;
       i = myModel->NextNumberForLabel (val, i, exact)) {
    cnt ++;
    if (num <= 0) num = i;
  }
  if (cnt == 1) return num;
  num = -num;

  return num;
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Handle(TCollection_HAsciiString) IFSelect_WorkSession::EntityName (const Handle(Standard_Transient)& ent) const
{
  if (myModel.IsNull() || ent.IsNull()) return 0;
  Interface_ShareTool sht(thegraph->Graph());

  Standard_Integer CN;
  Handle(Interface_GeneralModule) module;
  if (!thegtool->Select (ent,module,CN)) return 0;
  return module->Name (CN,ent,sht);
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Standard_Integer IFSelect_WorkSession::CategoryNumber (const Handle(Standard_Transient)& ent) const
{
  return (myModel.IsNull()? -1 : myModel->CategoryNumber(StartingNumber(ent)));
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Handle(TColStd_HSequenceOfTransient) IFSelect_WorkSession::Shareds (const Handle(Standard_Transient)& ent)
{
  Handle(TColStd_HSequenceOfTransient) list;
  if (!ComputeGraph()) return list;
  if (StartingNumber(ent) == 0) return list;
  return thegraph->Graph().Shareds(ent).Content();
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Handle(TColStd_HSequenceOfTransient) IFSelect_WorkSession::Sharings (const Handle(Standard_Transient)& ent)
{
  Handle(TColStd_HSequenceOfTransient) list;
  if (!ComputeGraph()) return list;
  if (StartingNumber(ent) == 0) return list;
  return thegraph->Graph().Sharings(ent).Content();
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Interface_CheckIterator IFSelect_WorkSession::CheckOne
  (const Handle(Standard_Transient)& ent,
   const Standard_Boolean complete)
{
  Interface_CheckIterator checks;
  checks.SetModel(myModel);
  if (!IsLoaded()) {
    checks.CCheck(0)->AddFail("DATA NOT AVAILABLE FOR CHECK");
    return checks;
  }
  Standard_Integer num = -1;
  if (ent.IsNull() || ent == myModel) num = 0;
  else num = myModel->Number(ent);

  Handle(Interface_Check) ach = myModel->Check (num,Standard_True);
  if (complete) ach->GetMessages (myModel->Check (num,Standard_False));
  if (num > 0) ach->SetEntity(ent);
  checks.Add (ach,num);
  checks.SetName ("Data Check (One Entity)");
  return checks;
}


//  #####################################################################
//  ....                        LES VARIABLES                        ....

//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Standard_Boolean IFSelect_WorkSession::HasName (const Handle(Standard_Transient)& item) const
{
  if (item.IsNull()) return Standard_False;
  Standard_Integer id = myItems.FindIndex(item);
  if (id == 0) return Standard_False;
  Handle(Standard_Transient) att = myItems.FindFromIndex(id);
  if (att.IsNull()) return Standard_False;
  return att->IsKind(STANDARD_TYPE(TCollection_HAsciiString));
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Handle(TCollection_HAsciiString) IFSelect_WorkSession::Name (const Handle(Standard_Transient)& item) const
{
  Handle(TCollection_HAsciiString) res;
  if (item.IsNull()) return res;
  Standard_Integer id = myItems.FindIndex(item);
  if (id == 0) return res;  // Null
  Handle(Standard_Transient) att = myItems.FindFromIndex(id);
  return GetCasted(TCollection_HAsciiString,att);
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Standard_Integer IFSelect_WorkSession::AddItem
  (const Handle(Standard_Transient)& item)
{
  return AddNamedItem("",item);
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Standard_Integer IFSelect_WorkSession::AddNamedItem
  (const Standard_CString name, const Handle(Standard_Transient)& item)
{
  if (item.IsNull()) return 0;

  if (name[0] == '#' || name[0] == '!') return 0;
// #nnn : pas un nom mais un numero. !... : reserve (interdit pour un nom)
//   nom deja pris : on ecrase l ancienne valeur
  if (name[0] != '\0') {
    Standard_Boolean deja;
    Handle(Standard_Transient)& newitem = myNames->NewItem(name,deja);
    newitem = item;
  }

  Standard_Integer id = myItems.FindIndex(item);
  if (id > 0) {
    Handle(Standard_Transient)& att = myItems.ChangeFromIndex(id);
    if (name[0] != '\0') {
      att = new TCollection_HAsciiString(name);
    }
    else if (att.IsNull()) att = item;
  }
  else {
    if (name[0] != '\0')
      id = myItems.Add(item,new TCollection_HAsciiString(name));
    else id = myItems.Add(item,item);
  }

  return id;
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Standard_Boolean IFSelect_WorkSession::RemoveItem (const Handle(Standard_Transient)& item)
{
  if (item.IsNull()) return Standard_False;
  Standard_Integer id = myItems.FindIndex(item);
  if (id == 0) return Standard_False;
  Handle(Standard_Transient)& att = myItems.ChangeFromIndex(id);
  if (att.IsNull()) return Standard_False;    // deja annulle

//  Marquer "Removed" dans la Map (on ne peut pas la vider)
  if (att->IsKind(STANDARD_TYPE(TCollection_HAsciiString))) {
    if (!myNames->RemoveItem
	(GetCasted(TCollection_HAsciiString,att)->ToCString()))
      return Standard_False;
  }
  att.Nullify();  // cf ChangeFromIndex
  return Standard_True;
}


// ..  Recherche par label : recherche en liste(noms) ou iterative


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Standard_Integer IFSelect_WorkSession::NextIdentForLabel
  (const Standard_CString label, const Standard_Integer id,
   const Standard_Integer mode) const
{
  Standard_Integer nb = MaxIdent();
  for (Standard_Integer i = id+1; i <= nb; i ++) {
    Handle(TCollection_HAsciiString) lab = ItemLabel(i);
    if (lab.IsNull()) continue;
    switch (mode) {
      case 0 : if (!strcmp(lab->ToCString(),label)) return i;  break; // switch
      case 1 : if (lab->Search(label) == 1 ) return i;  break;
      case 2 : if (lab->Search(label)  > 0 ) return i;  break;
      default : break;  // break du switch
    }
  }
  return 0;  // ici : pas trouve
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Standard_Integer IFSelect_WorkSession::MaxIdent () const
{
  return myItems.Extent();
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Handle(Standard_Transient) IFSelect_WorkSession::Item
  (const Standard_Integer id) const
{
  Handle(Standard_Transient) res;
  if (id <= 0 || id > MaxIdent()) return res;
  if (myItems.FindFromIndex(id).IsNull()) return res;
  return myItems.FindKey(id);
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Standard_Integer IFSelect_WorkSession::ItemIdent
  (const Handle(Standard_Transient)& item) const
{
  if (item.IsNull()) return 0;
  Standard_Integer id = myItems.FindIndex(item);
  if (id == 0) return 0;
  if (myItems.FindFromIndex(id).IsNull()) return 0;
  return id;
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Handle(Standard_Transient) IFSelect_WorkSession::NamedItem
  (const Standard_CString name) const
{
  Handle(Standard_Transient) res;
  if (name[0] == '\0') return res;
  if (name[0] == '#') {    // #nnn : pas un nom mais un n0 id.
    Standard_Integer id = atoi( &name[1] );
    return Item(id);
  }
  if (!myNames->GetItem(name,res)) res.Nullify();
  return res;
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Handle(Standard_Transient) IFSelect_WorkSession::NamedItem
  (const Handle(TCollection_HAsciiString)& name) const
{
  Handle(Standard_Transient) res;
  if (!name.IsNull()) res = NamedItem (name->ToCString());
  return res;
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Standard_Integer IFSelect_WorkSession::NameIdent
  (const Standard_CString name) const
{
  Handle(Standard_Transient) res;
  if (name[0] == '\0') return 0;
  if (name[0] == '#') {    // #nnn : pas un nom mais un n0 id.
    Standard_Integer id = atoi( &name[1] );
    return id;
  }
  if (!myNames->GetItem(name,res)) return 0;
  return ItemIdent(res);
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

void IFSelect_WorkSession::ClearItems ()
{
  myNames->Clear();
  myItems.Clear();
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Handle(TCollection_HAsciiString) IFSelect_WorkSession::ItemLabel
  (const Standard_Integer id) const
{
  Handle(TCollection_HAsciiString) res;
  Handle(Standard_Transient) var = Item(id);
  if (var.IsNull()) return res;
  DeclareAndCast(TCollection_HAsciiString,text,var);
  if (!text.IsNull()) {
    res = new TCollection_HAsciiString("Text:");
    res->AssignCat(text);
    return res;
  }
  DeclareAndCast(IFSelect_IntParam,intpar,var);
  if (!intpar.IsNull()) {
    res = new TCollection_HAsciiString(intpar->Value());
    res->Insert(1,"Integer:");
    return res;
  }
  DeclareAndCast(IFSelect_Selection,sel,var);
  if (!sel.IsNull()) {
    res = new TCollection_HAsciiString("Selection:");
    res->AssignCat(sel->Label().ToCString());
    return res;
  }
  DeclareAndCast(IFSelect_SignatureList,slc,var);
  if (!slc.IsNull()) {
    res = new TCollection_HAsciiString("Counter:");
    res->AssignCat(slc->Name());
    return res;
  }
  DeclareAndCast(IFSelect_Signature,sig,var);
  if (!sig.IsNull()) {
    res = new TCollection_HAsciiString("Signature:");
    res->AssignCat(sig->Name());
    return res;
  }
  res = new TCollection_HAsciiString("VariableType:");
  res->AssignCat(var->DynamicType()->Name());
  return res;
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Handle(TColStd_HSequenceOfInteger) IFSelect_WorkSession::ItemIdents (const Handle(Standard_Type)& type) const
{
  Handle(TColStd_HSequenceOfInteger) list =
    new TColStd_HSequenceOfInteger();
  Standard_Integer nb = myItems.Extent();
  for (Standard_Integer i = 1; i <= nb; i ++) {
    if (myItems.FindKey(i)->IsKind(type)) list->Append(i);
  }
  return list;
}


//  #################################################################
//  ....                Parametres (Int et Text)                ....


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Handle(IFSelect_IntParam) IFSelect_WorkSession::IntParam (const Standard_Integer id) const
{  return Handle(IFSelect_IntParam)::DownCast(Item(id));  }


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Standard_Integer IFSelect_WorkSession::IntValue (const Handle(IFSelect_IntParam)& par) const
{
  if (!par.IsNull()) return par->Value();
  else return 0;
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Standard_Boolean IFSelect_WorkSession::SetIntValue (const Handle(IFSelect_IntParam)& par, const Standard_Integer val)
{
  if (ItemIdent(par) == 0) return Standard_False;
  par->SetValue(val);
  return Standard_True;
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Handle(TCollection_HAsciiString) IFSelect_WorkSession::TextParam (const Standard_Integer id) const
{  return Handle(TCollection_HAsciiString)::DownCast(Item(id));  }


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

TCollection_AsciiString IFSelect_WorkSession::TextValue (const Handle(TCollection_HAsciiString)& par) const 
{
  if (!par.IsNull()) return TCollection_AsciiString(par->ToCString());
  else return TCollection_AsciiString();
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Standard_Boolean  IFSelect_WorkSession::SetTextValue (const Handle(TCollection_HAsciiString)& par, const Standard_CString val)
{
  if (ItemIdent(par) == 0) return Standard_False;
  par->Clear();  par->AssignCat(val);
  return Standard_True;
}


//  ########################################################################
//  ....                           SIGNATURES                           ....

//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Handle(IFSelect_Signature) IFSelect_WorkSession::Signature (const Standard_Integer id) const
{  return GetCasted(IFSelect_Signature,Item(id));  }

Standard_CString IFSelect_WorkSession::SignValue (const Handle(IFSelect_Signature)& sign, const Handle(Standard_Transient)& ent) const
{
  if (sign.IsNull() || myModel.IsNull()) return "";
  if (StartingNumber(ent) == 0) return "";
  return sign->Value (ent,myModel);
}


//  ########################################################################
//  ....                        SELECTIONS & Cie                        ....

//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Handle(IFSelect_Selection) IFSelect_WorkSession::Selection (const Standard_Integer id) const
{  return GetCasted(IFSelect_Selection,Item(id));  }


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Interface_EntityIterator IFSelect_WorkSession::EvalSelection (const Handle(IFSelect_Selection)& sel) const
{
  Interface_EntityIterator iter;
  if (errhand) {
    errhand = Standard_False;
    try {
      OCC_CATCH_SIGNALS
      iter = EvalSelection(sel);    // appel normal (donc, code pas duplique)
    }
    catch (Standard_Failure) {
      Handle(Message_Messenger) sout = Message::DefaultMessenger();
      sout<<"    ****    Interruption EvalSelection par Exception :   ****\n";
      sout<<Standard_Failure::Caught()->GetMessageString();
      sout<<"\n    Abandon"<<endl;
    }
    errhand = Standard_True;
    return iter;
  }

  if (thegraph.IsNull()) return iter;
 iter = sel->UniqueResult(thegraph->Graph());
  return iter;
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Handle(TColStd_HSequenceOfTransient) IFSelect_WorkSession::SelectionResult (const Handle(IFSelect_Selection)& sel) const
{
  Handle(TColStd_HSequenceOfTransient)  res;
  if (errhand) {
    errhand = Standard_False;
    try {
      OCC_CATCH_SIGNALS
      res = SelectionResult(sel);    // appel normal (->code unique)
    }
    catch (Standard_Failure) {
      Handle(Message_Messenger) sout = Message::DefaultMessenger();
      sout<<"    ****    Interruption SelectionResult par Exception :   ****\n";
      sout<<Standard_Failure::Caught()->GetMessageString();
      sout<<"\n    Abandon"<<endl;
    }
    errhand = Standard_True;
    return res;
  }

  if (!IsLoaded()) {
    cout<< " ***  Data for Evaluation not available  ***"<<endl;
    return new TColStd_HSequenceOfTransient();
  }
//  if (ItemIdent(sel) == 0)
  if (sel.IsNull())
    {  cout << " Selection :  Unknown"<<endl;  return res;  } //cout<<Handle
  return EvalSelection (sel).Content();
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Handle(TColStd_HSequenceOfTransient) IFSelect_WorkSession::SelectionResultFromList
  (const Handle(IFSelect_Selection)& sel,
   const Handle(TColStd_HSequenceOfTransient)& list) const
{
  if (list.IsNull()) return SelectionResult (sel);
  DeclareAndCast(IFSelect_SelectDeduct,deduct,sel);
  if (deduct.IsNull()) return SelectionResult (sel);

//   On va chercher la derniere deduction de la chaine des inputs
  Handle(IFSelect_Selection) ssel, newinput;
  ssel = sel;
  Standard_Integer i, nb = MaxIdent();
  for (i = 1; i <= nb * 2; i ++) {
    newinput  = deduct->Input();
    deduct = GetCasted(IFSelect_SelectDeduct,newinput);
    if (deduct.IsNull()) break;
    ssel   = newinput;
  }
  
//  on y est (enfin, on devrait)
//  ssel est la derniere selection auscultee,  deduct son downcast
//  input son Input (nulle  si sel  pas une deduction)
  deduct = GetCasted(IFSelect_SelectDeduct,ssel);

  deduct->Alternate()->SetList (list);

//   On execute puis on nettoie
  Handle(TColStd_HSequenceOfTransient) res = SelectionResult (sel);
////  deduct->SetInput (newinput);
  return res;
}

//  ######################################################################
//  ....                        Les COMPTEURS                         ....

//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Handle(IFSelect_SignCounter) IFSelect_WorkSession::SignCounter (const Standard_Integer id) const
{  return GetCasted(IFSelect_SignCounter,Item(id));  }


//  #########################################################################
//  ....    Action de Transfert proprement dite : la grande affaire !    ....

//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Interface_EntityIterator IFSelect_WorkSession::SentList
  (const Standard_Integer newcount) const
{
  Interface_EntityIterator iter;
  if (!IsLoaded()) return iter;
  const Interface_Graph& G = thegraph->Graph();
  Standard_Integer nb = G.Size();
  Standard_Integer i;
  for ( i = 1; i <= nb; i ++) {
    Standard_Integer stat = G.Status(i);
    if ( (stat > 0 && newcount < 0) || stat == newcount)
      iter.GetOneItem(G.Entity(i));
  }
  return iter;
}


//  ################################################################
//  ....        Actions particulieres sur les Selections        ....

//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Standard_Boolean IFSelect_WorkSession::IsReversedSelectExtract
  (const Handle(IFSelect_Selection)& sel) const
{
  if (ItemIdent(sel) == 0) return Standard_False;
  DeclareAndCast(IFSelect_SelectExtract,sxt,sel);
  if (sxt.IsNull()) return Standard_False;
  return (!sxt->IsDirect());
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Standard_Boolean IFSelect_WorkSession::ToggleSelectExtract
  (const Handle(IFSelect_Selection)& sel)
{
  if (ItemIdent(sel) == 0) return Standard_False;
  DeclareAndCast(IFSelect_SelectExtract,sxt,sel);
  if (sxt.IsNull()) return Standard_False;
  sxt->SetDirect(!sxt->IsDirect());
  return Standard_True;
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Standard_Boolean IFSelect_WorkSession::SetInputSelection
  (const Handle(IFSelect_Selection)& sel,
   const Handle(IFSelect_Selection)& inp)
{
  if (ItemIdent(sel) == 0) return Standard_False;
  if (!inp.IsNull() && ItemIdent(inp) == 0) return Standard_False;
  DeclareAndCast(IFSelect_SelectExtract,sxt,sel);
  if (!sxt.IsNull()) { sxt->SetInput(inp);  return Standard_True; }
  DeclareAndCast(IFSelect_SelectDeduct,sdt,sel);
  if (!sdt.IsNull()) { sdt->SetInput(inp);  return Standard_True; }
  return Standard_False;
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Standard_Boolean IFSelect_WorkSession::SetControl
  (const Handle(IFSelect_Selection)& sel,
   const Handle(IFSelect_Selection)& sc,
   const Standard_Boolean formain)
{
  DeclareAndCast(IFSelect_SelectControl,dsel,sel);
  if (ItemIdent(dsel) == 0) return Standard_False;
  if (ItemIdent(sc)   == 0) return Standard_False;
  if (formain) dsel->SetMainInput   (sc);
  else         dsel->SetSecondInput (sc);
  return Standard_True;
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Standard_Integer IFSelect_WorkSession::CombineAdd
  (const Handle(IFSelect_Selection)& sel,
   const Handle(IFSelect_Selection)& seladd,
   const Standard_Integer atnum)
{
  DeclareAndCast(IFSelect_SelectCombine,csel,sel);
  if (ItemIdent(csel) == 0) return 0;
  if (ItemIdent(seladd) == 0) return 0;
  csel->Add (seladd,atnum);
  return csel->NbInputs();
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Standard_Boolean IFSelect_WorkSession::CombineRemove
  (const Handle(IFSelect_Selection)& selcomb,
   const Handle(IFSelect_Selection)& selrem)
{
  DeclareAndCast(IFSelect_SelectCombine,csel,selcomb);
  if (ItemIdent(csel) == 0) return Standard_False;
  if (ItemIdent(selrem) == 0) return Standard_False;
  Standard_Integer nb = csel->NbInputs();
  for (Standard_Integer i = nb; i > 0; i --) {
    if (csel->Input(i) == selrem) {
      csel->Remove(i);
      return Standard_True;
    }
  }
  return Standard_True;
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Handle(IFSelect_Selection) IFSelect_WorkSession::GiveSelection
  (const Standard_CString selname) const
{
  char nomsel[500];
  Standard_Integer np = -1, nf = -1, nivp = 0;
  for (Standard_Integer n = 0; selname[n] != '\0'; n ++) {
    nomsel[n] = selname[n];  nomsel[n+1] = '\0';
    if (selname[n] == '(') { np = n; nivp ++; }
    if (selname[n] == ')') { nivp --;  if (nivp <= 0) nf = n; }
////    if (selname[n] == ' ') { nb = n; break; }
  }

  Handle(IFSelect_Selection) sel;
  if (np >= 0) nomsel[np] = 0;  if (nf >= 0) nomsel[nf] = '\0';
  Handle(Standard_Transient) item = NamedItem(nomsel);

//  Parentheses ? essayer Signature (plus tard : Selection parametree)
//  NB : on compte les niveaux de parentheses (imbrications repercutees)
  if (np > 0 && nf > 0) {
    Handle(IFSelect_SelectSignature) selsign;
    Standard_Integer debsign = np+1;

    DeclareAndCast(IFSelect_Signature,sign,item);
    DeclareAndCast(IFSelect_SignCounter,cnt,item);
    if (!sign.IsNull()) selsign =
      new IFSelect_SelectSignature (sign,&nomsel[debsign],Standard_False);
    else if (!cnt.IsNull()) selsign =
      new IFSelect_SelectSignature (cnt,&nomsel[debsign],Standard_False);
    else {
      cout<<selname<<" : neither Signature nor Counter"<<endl;
      return sel;
    }

    selsign->SetInput (new IFSelect_SelectModelEntities);  // par defaut
    sel = selsign;
  }

  else sel = GetCasted(IFSelect_Selection,item);

  return sel;
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Handle(TColStd_HSequenceOfTransient) IFSelect_WorkSession::GiveList (const Handle(Standard_Transient)& obj) const
{
//   Deja une liste
  DeclareAndCast(TColStd_HSequenceOfTransient,list,obj);
  if (!list.IsNull()) return list;

//   Rien du tout : retourne rien du tout
  if (obj.IsNull()) return list;

//   Une selection : son resultat (standard)
  DeclareAndCast(IFSelect_Selection,sel,obj);
  if (!sel.IsNull()) {
    Interface_EntityIterator iter = EvalSelection(sel);
    return iter.Content();
  }

//   Le modele : son contenu
  list = new TColStd_HSequenceOfTransient();
  if (obj == myModel) {
    Standard_Integer i, nb = myModel->NbEntities();
    for (i = 1; i <= nb; i ++)  list->Append (myModel->Value(i));
  }

//   Une entite du modele : cette entite
  else if (StartingNumber(obj) > 0) list->Append (obj);

//   Un Texte : son interpretation
  else {
    DeclareAndCast(TCollection_HAsciiString,str,obj);
    if (!str.IsNull()) return GiveList (str->ToCString());
  }

//  Si c est pas tout ca : une liste vide
  return list;
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Handle(TColStd_HSequenceOfTransient) IFSelect_WorkSession::GiveList (const Standard_CString first, const Standard_CString second) const
{
  Handle(TColStd_HSequenceOfTransient) list; 
  if (!first || first[0] == '\0') return list;
  if (first[0]  == ' ') return GiveList (&first[1],second);
  if (second && second[0] == ' ') return GiveList (first,&second[1]);

//   list NULLE sera interpretee comme SelectionResult (selection toute crue)
//   sinon comme SelectionResultFromList
  if (second && second[0] != '\0') list = GiveList (second,"");

  list = GiveListFromList (first,list);
  return list;
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Handle(TColStd_HSequenceOfTransient) IFSelect_WorkSession::GiveListFromList (const Standard_CString selname, const Handle(Standard_Transient)& ent) const
{
  Handle(TColStd_HSequenceOfTransient) list;
  Standard_Integer num;

//   LISTE DEFINIE D OFFICE (en ce cas, la liste d entree est ignoree)
  if (selname[0] == '(') {
//  liste d entites donnees a la queue leu leu : (ID,ID,ID...)
    char entid[50];     Standard_Integer i,j = 0;
    TColStd_MapOfInteger numap;
    list = new TColStd_HSequenceOfTransient();
    for (i = 1; selname[i] != '\0'; i ++) {
      if (selname[i] == ' ') continue;
      if (selname[i] == ',' || selname[i] == ')') {
	entid[j] = '\0';
	if (j == 0) continue;  j = 0;
	num = NumberFromLabel (entid);
	if (num <= 0 || !numap.Add (num)) continue;
	Handle(Standard_Transient) anent = StartingEntity(num);
	if (!anent.IsNull()) list->Append (anent);
	if (selname[i] == ')') break;
	continue;
      }
      entid[j] = selname[i];  j ++;
    }
    return list;
  }
  num = NumberFromLabel (selname);
  if (num > 0)  return GiveList(StartingEntity(num));

//  Autres cas : y atil une liste d entree.
//   Si OUI -> SelectionResultFromList.  Si NON -> SelectionResult
//   Si une entite isolee -> on en fait une liste

  list = GiveList(ent);  // ent NULL -> list NULL sinon intreprete

//    Decomposition term1 term2 ...

  char nomsel[500];  nomsel[0] = '\0';
  Standard_Integer n= 0 , nb = -1;
  for (n = 0; selname[n] != '\0'; n ++) {
    nomsel[n] = selname[n];  nomsel[n+1] = '\0';
//    if (selname[n] == '(') { np = n; nivp ++; }
//    if (selname[n] == ')') { nivp --;  if (nivp <= 0) nf = n; }
    if (selname[n] == ' ') { nb = n; nomsel[n] = '\0'; break; }
  }
  if (nomsel[0] == '\0') return list;

  Handle(IFSelect_Selection) sel = GiveSelection (nomsel);
  if (sel.IsNull())  {
    cout<<"Neither Entity Number/Label nor Selection :"<<nomsel<<endl;
    return list;
  }

  if (nb > 0) list = GiveListFromList (&selname[nb+1],list);

  if (list.IsNull()) list = SelectionResult (sel);
  else               list = SelectionResultFromList (sel,list);

  return list;
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Standard_Integer IFSelect_WorkSession::QueryParent
  (const Handle(Standard_Transient)& entdad,
   const Handle(Standard_Transient)& entson) const
{
  Standard_Integer ndad = StartingNumber(entdad);
  Standard_Integer nson = StartingNumber(entson);
  if (ndad < 1 || nson < 1) return -1;
  if (ndad == nson) return 0;
//  on va calculer : pour chaque pere immediat, de <son>, status avec <dad> + 1
//  nb : pas protege contre les boucles ...
  Handle(TColStd_HSequenceOfTransient) list =
    thegraph->Graph().Sharings(entson).Content();
  if (list.IsNull()) return -1;
  Standard_Integer i, nb = list->Length();
  for (i = 1; i <= nb; i ++) {
    if (list->Value(i) == entdad) return 1;
    Standard_Integer stat = QueryParent ( entdad,list->Value(i) );
    if (stat >= 0) return stat+1;
  }
  return -1;  // not yet implemented ...
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

void IFSelect_WorkSession::TraceStatics
  (const Standard_Integer use, const Standard_Integer mode) const
{
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  if (use > 0) {
    if (mode == 0)   sout<<"******************************************"<<endl;
    if        (use == 1) {
      if (mode == 0) sout<<"*****      General  Parameters       *****"<<endl;
    } else if (use == 2) {
      if (mode == 0) sout<<"*****            Load  File          *****"<<endl;
    } else if (use == 3) {
      if (mode == 0) sout<<"*****            Write File          *****"<<endl;
    } else if (use == 4) {
      if (mode == 0) sout<<"*****            Split File          *****"<<endl;
    } else if (use == 5) {
      if (mode == 0) sout<<"*****        Transfer (Read)         *****"<<endl;
    } else if (use == 6) {
      if (mode == 0) sout<<"*****        Transfer (Write)        *****"<<endl;
    }
    if (mode == 0)   sout<<"******************************************"<<endl<<endl;
  }

//    Echainements particuliers (use > 0)
  if (use == 5) {
    TraceStatics (-2,mode);
    if (mode == 0) sout<<endl;
  } else if (use == 4 || use == 6) {
    TraceStatics (-3,mode);
    if (mode == 0) sout<<endl;
  }

//    Fin
  if (use > 0) {
    if (mode == 0)   sout<<"******************************************"<<endl<<endl;
  }
}

//  ####    ####    ####    ####    ####    ####    ####    ####    ####
//  ....        ListItems        ....

//=======================================================================
//function : 
//purpose  : 
//=======================================================================

void IFSelect_WorkSession::ListItems (const Standard_CString lab) const 
{
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  sout<< "        **********  Items in Session  **********"<<endl;
  Standard_Integer nb = MaxIdent();
  Handle(TCollection_HAsciiString) str;
  if (lab[0] != '\0') str = new TCollection_HAsciiString (lab);
  for (Standard_Integer i = 1; i <= nb; i ++) {
    const Handle(Standard_Transient)& var = myItems.FindKey(i);
    Handle(TCollection_HAsciiString) label = ItemLabel(i);
    if (label.IsNull()) continue;    //  -> item supprime
    if (!str.IsNull())  {  if (label->Location(str,1,label->Length()) == 0) continue; }
    sout<<"#"<<i;
    if (HasName(var)) sout<<"	- Named : "<<Name(var)->ToCString()<<"	- ";
    else sout<<" - (no name) - ";
    sout<<var->DynamicType()->Name()<<endl<<"    "<<label->ToCString()<<endl;
  }
}

//  ####    ####    ####    ####    ####    ####    ####    ####    ####
//  ....        DumpSelection        ....

//=======================================================================
//function : 
//purpose  : 
//=======================================================================

void IFSelect_WorkSession::DumpSelection (const Handle(IFSelect_Selection)& sel) const
{
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  if (ItemIdent(sel) == 0) {
    sout << "Selection :  Unknown"<<endl;  //sout<<Handle
    return;
  }
  sout << "        **********  Selection";
  if (HasName(sel)) sout << " , Name : " << Name(sel)->ToCString();
  sout <<"  **********"<<endl;
  sout<< "Label : " << sel->Label() << " . Input(s) : "<< endl;
  Standard_Integer nb = 0;
  IFSelect_SelectionIterator iter; sel->FillIterator(iter);
  for (; iter.More(); iter.Next()) {
    nb ++; 
    Handle(IFSelect_Selection) newsel = iter.Value();
    sout<<" -- "<<newsel->Label()<<endl;
  }
  sout << " Nb Inputs:"<<nb<<endl;
}

//=======================================================================
//function : 
//purpose  : 
//=======================================================================

void IFSelect_WorkSession::DumpModel (const Standard_Integer level, const Handle(Message_Messenger)& S)
{
  if (!IsLoaded())
    {  S<< " ***  Data for List not available  ***"<<endl;  return;  }
  S << "\n        *****************************************************************\n";
  if (theloaded.Length() > 0)
    S << "        ********  Loaded File : "<<theloaded.ToCString()<<Interface_MSG::Blanks(32-theloaded.Length())<<" ********"<<endl;
  else S << "        ********  No name for Loaded File"<<endl;
  if (level == 0) {
    S<<"        ********  Short Dump of Header                           ********\n";
  S << "        *****************************************************************\n\n";
    myModel->DumpHeader(S);  S<<endl;
  }

  Standard_Integer nbent = myModel->NbEntities();
  Standard_Integer nbr = 0;
  Interface_ShareFlags shar(thegraph->Graph());

  for (Standard_Integer i = 1; i <= nbent; i ++) {
    if (!shar.IsShared(myModel->Value(i))) nbr ++;
  }
  S << "        *****************************************************************\n";
  S << "        ********  Model : "<<nbent<<" Entities, of which "<<nbr<<" Root(s)\n";
  S << "        *****************************************************************\n"<<endl;

  if (level <= 0) return;
  else if (level == 1) {
    S<<"        ********  Root Entities  ********      ";
    ListEntities (shar.RootEntities(),1);
  } else if (level == 2) {
    S<<"        ********  Complete List  ********      ";
    ListEntities (myModel->Entities(),1);
  } else if (level > 2) {
    IFSelect_PrintCount mode = IFSelect_ItemsByEntity;
    if (level == 5 || level ==  8) mode = IFSelect_CountByItem;
    if (level == 6 || level ==  9) mode = IFSelect_ListByItem;
    if (level == 7 || level == 10) mode = IFSelect_EntitiesByItem;
    PrintCheckList (ModelCheckList(),Standard_False, mode);
  } else {
    if (level == 3) S << "        ********  Check Model (Fails)  ********"<<endl;
    else            S << "        ********  Check Model (Complete)  ********"<<endl;
    Interface_CheckTool CT (Graph());
    Interface_CheckIterator C;
    {
      try {
        OCC_CATCH_SIGNALS
        if (level == 3) C = CT.CheckList();
        else    C = CT.CompleteCheckList();
      }
      catch (Standard_Failure) {
        Handle(Message_Messenger) sout = Message::DefaultMessenger();
        sout<<"    ****    Interruption DumpModel (Check) par Exception    ****\n";
        S<<"  ** **  Exception Raised during Check !  ** **\n";
        S<<"  -->  what could be determined is listed"<<endl;
      }
    }

//  Check List : si vide (pas demandee), naturellement passee
    try {
      OCC_CATCH_SIGNALS
      C.Print(S,myModel, (level == 3));
    }
    catch (Standard_Failure) {
      Handle(Message_Messenger) sout = Message::DefaultMessenger();
      sout<<"    ****    Interruption DumpModel par Exception :   ****\n";
      sout<<Standard_Failure::Caught()->GetMessageString();
      sout<<"\n    Abandon"<<endl;
    }

  }
  S<<endl<<"There are "<<nbent<<" Entities, of which "<<nbr<<" Root(s)"<<endl;
}

//  ....        TraceDumpModel        ....  (Model + CheckList)

//=======================================================================
//function : 
//purpose  : 
//=======================================================================

void IFSelect_WorkSession::TraceDumpModel (const Standard_Integer mode)
{
  const Handle(Message_Messenger) &sout = Message::DefaultMessenger();
  DumpModel (mode,sout);
}

//  ....        PrintEntityStatus        ....

//=======================================================================
//function : 
//purpose  : 
//=======================================================================

void IFSelect_WorkSession::PrintEntityStatus
  (const Handle(Standard_Transient)& ent, const Handle(Message_Messenger)& S)
{
  Standard_Integer i,nb;
  Standard_Integer num = StartingNumber(ent);
  if (num == 0)  {  cout<<" --  PrintEntityStatus : unknown"<<endl;  return;  }

  S <<"  Ent. n0/id:   ";
  myModel->Print(ent,S);
  Handle(TCollection_HAsciiString) hname = EntityName(ent);
  if (!hname.IsNull() && hname->Length() > 0) S<<"	Name:"<<hname->ToCString();
  S<<endl;
  Handle(IFSelect_Signature) signtype = SignType();
  if (signtype.IsNull()) S<<"  Type(CDL):"<<ent->DynamicType()->Name()<<endl;
  else S <<"  Type:"<<signtype->Value (ent,myModel)<<endl;
  S <<"    Category : " <<CategoryName (ent)
    <<"    Validity : " <<ValidityName (ent) << endl;
  Interface_CheckIterator chl = CheckOne (ent);
  chl.Print (S,myModel,Standard_False,Standard_False);

  Handle(TColStd_HSequenceOfTransient) list = Sharings(ent);
  if (list.IsNull()) S<<"  Root"<<endl;
  else {
    nb = list->Length();
    if (nb == 0) S<<"  Root";
    else S<<"  Super-entities:"<<nb<<" : (n0/id):";
    for (i = 1; i <= nb; i ++)  {  S<<" "; myModel->Print(list->Value(i),S);  }
    S<<endl;
  }
  list = Shareds (ent);
  if (list.IsNull()) S<<"  No sub-entity"<<endl;
  else {
    nb = list->Length();
    if (nb == 0) S<<"  No sub-entity";
    else S<<"  Sub-entities:"<<nb<<" , i.e. (n0/id):";
    for (i = 1; i <= nb; i ++)  {  S<<" "; myModel->Print(list->Value(i),S);  }
    S<<endl;
  }
}

//  ....        PrintCheckList        ....

//=======================================================================
//function : 
//purpose  : 
//=======================================================================

void IFSelect_WorkSession::PrintCheckList
  (const Interface_CheckIterator& checklist,
   const Standard_Boolean failsonly, const IFSelect_PrintCount mode) const
{
//  mode : 0  comptage   1 n0s entites   2 n0s+id ents
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  if (mode == IFSelect_ItemsByEntity) checklist.Print (sout,myModel,failsonly);
  else {
    Interface_CheckIterator chks = checklist;
    Handle(IFSelect_CheckCounter) counter =
      new IFSelect_CheckCounter (mode>1 && mode != IFSelect_CountSummary);
    counter->Analyse (chks,myModel,Standard_True,failsonly);
    counter->PrintList  (sout,myModel,mode);
  }
}


//              ##########################################
//              #########    Fonctions complementaires
//              ##########################################

//  ####    ####    ####    ####    ####    ####    ####    ####    ####
//  ....      Routine Interne : ListEntities

//=======================================================================
//function : 
//purpose  : 
//=======================================================================

void IFSelect_WorkSession::ListEntities
  (const Interface_EntityIterator& iter, const Standard_Integer mmode) const 
{
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  int titre = 0;
  Standard_Integer mode = (mmode < 0 ? -mmode : mmode);
  if (mmode >= 0) sout << " List of " << iter.NbEntities() << " Entities :"<<endl;
  if (!IsLoaded())
    {  sout<< " ***  Data for List not available  ***"<<endl;  return;  }
  Interface_ShareFlags tool(thegraph->Graph());

  try {
    OCC_CATCH_SIGNALS
    int newcount = -1; int mods = 0; int cnt = 0;
    for (iter.Start(); iter.More(); iter.Next()) {
      if (!titre && mode == 1) sout
	<< "Number/Id.           Category Validity    Type\n-----------          ----...."<<endl;
//          123456789 123456789 123456  123456789 123456789 123456
      if (!titre && mode == 0) sout<<"  Keys : R Root   ? Unknown   * Unloaded"<<endl;
      if (!titre && mode == 2) sout<<"(";
      titre = 1;
      Handle(Standard_Transient) ent = iter.Value();
      Standard_Integer num = myModel->Number(ent);
      if (mode == 1) {
    // n0 id (root?) category validity tracetype
	sout<<Interface_MSG::Blanks (num,6);
	myModel->Print(ent,sout,0);
	if (!tool.IsShared(ent)) sout << " #ROOT#";
	else                     sout << "       ";
	Standard_Integer catnum = myModel->CategoryNumber(num);
	if (catnum > 0) sout<<"  "<<Interface_Category::Name (catnum);
	sout << "  (" << ValidityName (ent) << ")  ";

	sout<<" Type:"<<myModel->TypeName (ent, Standard_False)<<endl;
      } else if (mode == 2) {
	newcount ++;
	if (newcount > 0) sout<<",";
	sout<<num;
      } else {
	newcount ++;  mods = 0; cnt ++;
	if      (newcount >= 10) { sout << endl<<"["<<cnt<<"]:"; newcount = 1; }
	if (newcount > 0)  sout << "	";
	myModel->Print(ent,sout,0);
	if (!tool.IsShared(ent)) { if(mods == 0) sout<<"("; sout<<"R"; mods++; }
	if (myModel->IsUnknownEntity(num)) { sout<<(mods==0 ? '(' : ' ')<<"?"; mods ++; }
	if (myModel->IsRedefinedContent(num)) { sout<<(mods==0 ? '(' : ' ')<<"*"; mods ++; }
	if (mods) { sout<<")"; newcount ++; }
      }
    }
    if (mode == 0) sout<<endl;
    if (mode == 2) sout<<")"<<endl;
  }
  catch (Standard_Failure) {
    sout<<"    ****    Interruption ListEntities par Exception :   ****\n";
    sout<<Standard_Failure::Caught()->GetMessageString();
    sout<<"\n    Abandon"<<endl;
  }
}


//=======================================================================
//function : TransferList
//purpose  : 
//=======================================================================

Standard_Integer IFSelect_WorkSession::TransferList (const Handle(TColStd_HSequenceOfTransient)& list)
{
  if (myModel.IsNull()) return 0;

  //c1:if (myTP.IsNull())  {  if (!BeginTransfer()) return 0;  }

  const Handle(Message_Messenger) &sout = myReaderProcess->Messenger();
  const Standard_Integer level = myReaderProcess->TraceLevel();

  //c1:if (myGraph.IsNull()) myTP->SetModel(myModel);
  //c1:else                  myTP->SetGraph(myGraph);

  Standard_Integer i,nb = list->Length();

  //   Pour le log-file
  if (level > 0) {
    sout<<"\n*******************************************************************\n";
    sout << "******           Transferring a list of "<<Interface_MSG::Blanks(nb,5)<<" Entities       ******"<<endl;
    sout<<"\n*******************************************************************\n";

    Handle(IFSelect_SignatureList) sl = new IFSelect_SignatureList;
    for (i = 1; i <= nb; i ++)
      sl->Add (list->Value(i), myModel->TypeName(list->Value(i),Standard_False));
    sl->SetName ("Entities to Transfer");
    sl->PrintCount (sout);
    sout<<"\n*******************************************************************\n";
  }

  Standard_Integer res = 0;

  for (i = 1; i <= nb; i ++)
  {
    const Handle(Standard_Transient) &ent = list->Value(i);

    if (level > 1) {
      const Standard_Integer num = myModel->Number(ent);
      Handle(TCollection_HAsciiString) lab = myModel->StringLabel(ent);
      sout<<"\n*******************************************************************\n";
      sout << "******           Transferring one Entity                     ******"<<endl;
      if (!lab.IsNull())
        sout<<"******    N0 in file : "<<Interface_MSG::Blanks(num,5)<<num
	        <<"      Ident : "<<lab->ToCString()
	        <<Interface_MSG::Blanks(14 - lab->Length())<<"******\n";
      sout<<"******    Type : "<<myModel->TypeName(ent,Standard_False)
          << Interface_MSG::Blanks((Standard_Integer) (44 - strlen(myModel->TypeName(ent,Standard_False))))
	      << "******";
      sout<<"\n*******************************************************************\n";
    }

    res += myReaderProcess->TransferOne(ent);
  }
  return res;
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Standard_CString IFSelect_WorkSession::CategoryName (const Handle(Standard_Transient)& ent) const
{
  return Interface_Category::Name (CategoryNumber (ent));
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Standard_CString IFSelect_WorkSession::ValidityName (const Handle(Standard_Transient)& ent) const
{
  return ((StartingNumber(ent) == 0)? "" : IFSelect_SignValidity::CVal (ent,myModel));
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Interface_CheckIterator IFSelect_WorkSession::ModelCheckList ()
{
  Interface_CheckIterator checks;
  if (!IsLoaded()) {
    checks.CCheck(0)->AddFail("DATA NOT AVAILABLE FOR CHECK");
    return checks;
  }
  Interface_CheckTool cht(Graph());
  checks = cht.CompleteCheckList();
  checks.SetName("Model Complete Check List");
  return checks;
}
