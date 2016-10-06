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


#include <DBRep.hxx>
#include <Dico_DictionaryOfInteger.hxx>
#include <Draw_Appli.hxx>
#include <IFSelect_SessionPilot.hxx>
#include <Interface_InterfaceModel.hxx>
#include <Interface_Macros.hxx>
#include <Interface_Protocol.hxx>
#include <Message.hxx>
#include <Message_Messenger.hxx>
#include <Standard_Transient.hxx>
#include <TCollection_AsciiString.hxx>
#include <TColStd_HSequenceOfAsciiString.hxx>
#include <TopoDS_Shape.hxx>
#include <Transfer_FinderProcess.hxx>
#include <Transfer_TransientProcess.hxx>
#include <TransferBRep.hxx>
#include <XSControl_Controller.hxx>
#include <XSControl_TransferReader.hxx>
#include <XSControl_WorkSession.hxx>
#include <XSDRAW.hxx>
#include <XSDRAW_Vars.hxx>
#include <XSDRAW_SelectFunctions.hxx>
#include <XSDRAW_ControlFunctions.hxx>
#include <XSDRAW_ShapeFunctions.hxx>

#include <stdio.h>
static int deja = 0, dejald = 0;

static Handle(Dico_DictionaryOfInteger)       theolds;
static Handle(TColStd_HSequenceOfAsciiString) thenews;

static Handle(IFSelect_SessionPilot)    thepilot;  // detient Session, Model

static Standard_Integer XSTEPDRAWRUN (Draw_Interpretor& , Standard_Integer argc, const char** argv)
{
  TCollection_AsciiString mess;
  for (Standard_Integer i = 0; i < argc; i ++) {
    mess.AssignCat(argv[i]);  mess.AssignCat(" ");
  }

  IFSelect_ReturnStatus stat = thepilot->Execute (mess.ToCString());
  if (stat == IFSelect_RetError || stat == IFSelect_RetFail) return 1;
  else return 0;
}

    void  XSDRAW::ChangeCommand
  (const Standard_CString oldname, const Standard_CString newname)
{
  if (theolds.IsNull()) theolds = new Dico_DictionaryOfInteger;
  Standard_Integer num = 0;
  if (newname[0] != '\0') {
    if (thenews.IsNull()) thenews = new TColStd_HSequenceOfAsciiString();
    TCollection_AsciiString newstr(newname);
    thenews->Append(newstr);
    num = thenews->Length();
  }
  theolds->SetItem (oldname,num);
}

void  XSDRAW::RemoveCommand (const Standard_CString oldname)
{
  ChangeCommand (oldname,"");
}

void XSDRAW::LoadSession ()
{
  static int gInit= 0;
  if (!gInit) {
    gInit = 1;
    thepilot   = new IFSelect_SessionPilot("XSTEP-DRAW>");
    Handle(XSControl_WorkSession) WS = new XSControl_WorkSession;
    WS->SetVars (new XSDRAW_Vars);
    thepilot->SetSession (WS);

    XSDRAW_SelectFunctions::Init();
    XSDRAW_ControlFunctions::Init();
    XSDRAW_ShapeFunctions::Init ();
  }
}

void XSDRAW::LoadDraw (Draw_Interpretor& theCommands)
{
  if (dejald) return;  dejald = 1;
//  Pour tout faire d un coup : BRepTest & cie:
  LoadSession();

  //skl: we remove commands "x" and "exit" in order to this commands are
  //     performed not in IFSelect_SessionPilot but in standard Tcl interpretor
  XSDRAW::RemoveCommand("x");
  XSDRAW::RemoveCommand("exit");

  Handle(TColStd_HSequenceOfAsciiString) list = IFSelect_Activator::Commands(0);
  TCollection_AsciiString com;
  Standard_Integer i, nb = list->Length();
  for (i = 1; i <= nb; i ++) {
    Handle(IFSelect_Activator) act;
    Standard_Integer nact, num = -1;
    char help[200];
    com = list->Value(i);
    if (!theolds.IsNull())
      if (theolds->HasItem(com.ToCString())) num = theolds->Item(com.ToCString());
    if (num == 0) continue;
    if (!IFSelect_Activator::Select(com.ToCString(),nact,act))
      Sprintf (help,"type :  xhelp %s for help",com.ToCString());
    else if (!act.IsNull()) strcpy(help,act->Help(nact));
    if (num < 0) theCommands.Add (com.ToCString(),help,XSTEPDRAWRUN,act->Group());
    else theCommands.Add (thenews->Value(num).ToCString(),help,XSTEPDRAWRUN,act->Group());
  }
}

Standard_Integer XSDRAW::Execute (const Standard_CString command, const Standard_CString varname)
{
  char mess[100];
  Sprintf (mess,command,varname);
  thepilot->Execute (mess);
  return 1;  // stat ?
}

const Handle(IFSelect_SessionPilot) & XSDRAW::Pilot ()
{  return thepilot;  }

Handle(XSControl_WorkSession) XSDRAW::Session (const Handle(IFSelect_SessionPilot) &thePilot)
{  return Handle(XSControl_WorkSession)::DownCast((thePilot.IsNull()? thepilot : thePilot)->Session());  }

#include <IFSelect_SelectModelEntities.hxx>
#include <IFSelect_SelectModelRoots.hxx>
#include <XSControl_SelectForTransfer.hxx>
#include <XSControl_SignTransferStatus.hxx>
#include <XSControl_ConnectedShapes.hxx>
#include <IFSelect_SignType.hxx>
#include <IFSelect_SignAncestor.hxx>
#include <IFSelect_SignCounter.hxx>
#include <IFSelect_SignCategory.hxx>
#include <IFSelect_SignValidity.hxx>
#include <IFSelect_DispPerOne.hxx>
#include <IFSelect_DispPerCount.hxx>
#include <IFSelect_IntParam.hxx>
#include <IFSelect_DispPerFiles.hxx>
#include <IFSelect_DispPerSignature.hxx>
#include <IFSelect_SelectPointed.hxx>
#include <IFSelect_SelectSharing.hxx>
#include <IFSelect_SelectShared.hxx>
#include <IFSelect_GraphCounter.hxx>
#include <Interface_Static.hxx>
#include <IFSelect_ParamEditor.hxx>
#include <IFSelect_EditForm.hxx>

void XSDRAW::SetController (const Handle(XSControl_Controller)& control)
{
  if (thepilot.IsNull()) LoadSession();
  if (control.IsNull()) cout<<"XSTEP Controller not defined"<<endl;
  else if (Session().IsNull()) cout<<"XSTEP Session badly or not defined"<<endl;
  else {
    Handle(XSControl_WorkSession) WS = Session();

    if (WS->NamedItem("xst-model-all").IsNull()) {

      Handle(IFSelect_SelectModelEntities) sle = new IFSelect_SelectModelEntities;
      WS->AddNamedItem ("xst-model-all",sle);

      Handle(IFSelect_SelectModelRoots)    slr = new IFSelect_SelectModelRoots;
      WS->AddNamedItem ("xst-model-roots",slr);

      if(strcasecmp(WS->SelectedNorm(),"STEP")) {
        Handle(XSControl_SelectForTransfer) st1 = new XSControl_SelectForTransfer;
        st1->SetInput (slr);
        st1->SetReader (WS->TransferReader());
        WS->AddNamedItem ("xst-transferrable-roots",st1);
      }

      Handle(XSControl_SelectForTransfer) st2 = new XSControl_SelectForTransfer;
      st2->SetInput (sle);
      st2->SetReader (WS->TransferReader());
      WS->AddNamedItem ("xst-transferrable-all",st2);
   
      Handle(XSControl_SignTransferStatus) strs = new XSControl_SignTransferStatus;
      strs->SetReader (WS->TransferReader());
      WS->AddNamedItem ("xst-transfer-status",strs);
  
      Handle(XSControl_ConnectedShapes) scs = new XSControl_ConnectedShapes;
      scs->SetReader (WS->TransferReader());
      WS->AddNamedItem ("xst-connected-faces",scs);

      Handle(IFSelect_SignType) stp = new IFSelect_SignType (Standard_False);
      WS->AddNamedItem ("xst-long-type",stp);

      Handle(IFSelect_SignType) stc = new IFSelect_SignType (Standard_True);
      WS->AddNamedItem ("xst-type",stc);

      WS->AddNamedItem ("xst-ancestor-type",new IFSelect_SignAncestor);
      WS->AddNamedItem ("xst-types",new IFSelect_SignCounter(stp,Standard_False,Standard_True));
      WS->AddNamedItem ("xst-category",new IFSelect_SignCategory);
      WS->AddNamedItem ("xst-validity",new IFSelect_SignValidity);

      Handle(IFSelect_DispPerOne) dispone = new IFSelect_DispPerOne;
      dispone->SetFinalSelection(slr);
      WS->AddNamedItem ("xst-disp-one",dispone);

      Handle(IFSelect_DispPerCount) dispcount = new IFSelect_DispPerCount;
      Handle(IFSelect_IntParam) intcount = new IFSelect_IntParam;
      intcount->SetValue(5);
      dispcount->SetCount(intcount);
      dispcount->SetFinalSelection(slr);
      WS->AddNamedItem ("xst-disp-count",dispcount);

      Handle(IFSelect_DispPerFiles) dispfiles = new IFSelect_DispPerFiles;
      Handle(IFSelect_IntParam) intfiles = new IFSelect_IntParam;
      intfiles->SetValue(10);
      dispfiles->SetCount(intfiles);
      dispfiles->SetFinalSelection(slr);
      WS->AddNamedItem ("xst-disp-files",dispfiles);

      Handle(IFSelect_DispPerSignature) dispsign = new IFSelect_DispPerSignature;
      dispsign->SetSignCounter(new IFSelect_SignCounter(Handle(IFSelect_Signature)(stc)));
      dispsign->SetFinalSelection(slr);
      WS->AddNamedItem ("xst-disp-sign",dispsign);

      // Not used directly but useful anyway
      WS->AddNamedItem ("xst-pointed",new IFSelect_SelectPointed);
      WS->AddNamedItem ("xst-sharing",new IFSelect_SelectSharing);
      WS->AddNamedItem ("xst-shared",new IFSelect_SelectShared);
      WS->AddNamedItem ("xst-nb-selected",new IFSelect_GraphCounter);

      WS->SetSignType( stp );
    }

    Handle(TColStd_HSequenceOfHAsciiString) listat = Interface_Static::Items();
    Handle(IFSelect_ParamEditor) paramed = IFSelect_ParamEditor::StaticEditor (listat,"All Static Parameters");
    WS->AddNamedItem ("xst-static-params-edit",paramed);
    Handle(IFSelect_EditForm) paramform = paramed->Form(Standard_False);
    WS->AddNamedItem ("xst-static-params",paramform);

    WS->SetController (control);
  }
}


    Handle(XSControl_Controller) XSDRAW::Controller ()
      {  return Session()->NormAdaptor();  }

    Standard_Boolean  XSDRAW::SetNorm
  (const Standard_CString norm)
{
  return Session()->SelectNorm (norm);
}

    Handle(Interface_Protocol)   XSDRAW::Protocol ()
      {  return thepilot->Session()->Protocol();  }

    Handle(Interface_InterfaceModel)  XSDRAW::Model ()
      {  return thepilot->Session()->Model();  }

    void  XSDRAW::SetModel
  (const Handle(Interface_InterfaceModel)& model, const Standard_CString file)
{
  thepilot->Session()->SetModel(model);
  if (file && file[0] != '\0') thepilot->Session()->SetLoadedFile(file);
}

    Handle(Interface_InterfaceModel)  XSDRAW::NewModel ()
      {  return Session()->NewModel();  }

    Handle(Standard_Transient)  XSDRAW::Entity (const Standard_Integer num)
      {  return thepilot->Session()->StartingEntity(num);  }

    Standard_Integer  XSDRAW::Number (const Handle(Standard_Transient)& ent)
      {  return thepilot->Session()->StartingNumber(ent);  }

    void  XSDRAW::SetTransferProcess (const Handle(Standard_Transient)& ATP)
{
  DeclareAndCast(Transfer_FinderProcess,FP,ATP);
  DeclareAndCast(Transfer_TransientProcess,TP,ATP);

//   Cas FinderProcess    ==> TransferWriter
  if (!FP.IsNull())  Session()->SetMapWriter(FP);

//   Cas TransientProcess ==> TransferReader
  if (!TP.IsNull()) {
    if (!TP->Model().IsNull() && TP->Model() != Session()->Model())
      Session()->SetModel (TP->Model());
    Session()->SetMapReader(TP);
  }
}

    Handle(Transfer_TransientProcess)  XSDRAW::TransientProcess  ()
      {  return Session()->TransferReader()->TransientProcess();  }

    Handle(Transfer_FinderProcess)  XSDRAW::FinderProcess  ()
      {  return Session()->TransferWriter()->FinderProcess();  }


    void XSDRAW::InitTransferReader (const Standard_Integer mode)
{
//   0 nullify  1 clear
//   2 init TR avec contenu TP (roots)  3 init TP avec contenu TR
//   4 init avec model (debut scratch)
  Session()->InitTransferReader(mode);
}

    Handle(XSControl_TransferReader)  XSDRAW::TransferReader ()
      {  return Session()->TransferReader();  }


//  ############  AUXILIAIRES  #############

    Handle(Standard_Transient)  XSDRAW::GetEntity (const Standard_CString name)
      {  return  XSDRAW_SelectFunctions::GiveEntity (Session(),name);  }

    Standard_Integer  XSDRAW::GetEntityNumber  (const Standard_CString name)
      {  return  XSDRAW_SelectFunctions::GiveEntityNumber (Session(),name);  }


    Handle(TColStd_HSequenceOfTransient)  XSDRAW::GetList
  (const Standard_CString first, const Standard_CString second)
{
  Handle(TColStd_HSequenceOfTransient) list;
  if (!first || first[0] == '\0') {
    char ligne[80];  ligne[0] = '\0'; char truc;
    cin >> ligne;  Standard_Size ln = strlen(ligne);
    char *ff = &ligne[0], *ss = NULL;
    cin.get(truc);  if (truc != '\n') { cin>>&ligne[ln+1]; ss = &ligne[ln+1]; }
    return  XSDRAW::GetList (ff,ss);
  }
  return XSDRAW_SelectFunctions::GiveList (Session(),first,second);
}


Standard_Boolean XSDRAW::FileAndVar
  (const Standard_CString file, const Standard_CString var,
   const Standard_CString def,
   TCollection_AsciiString& resfile,   TCollection_AsciiString& resvar)
{
  return XSDRAW_ShapeFunctions::FileAndVar(Session(),file,var,def,resfile,resvar);
}

Standard_Integer XSDRAW::MoreShapes (Handle(TopTools_HSequenceOfShape)& list, const Standard_CString name)
{
  return XSDRAW_ShapeFunctions::MoreShapes(Session(),list,name);
}

//  FONCTION POUR LE DEBUG

Standard_Integer XSDRAW_WHAT (const Handle(Standard_Transient)& ent)
{
  if (ent.IsNull()) { cout<<"(Null Handle)"<<endl; return 0; }
  Handle(Interface_InterfaceModel) model = XSDRAW::Model();
  if (model.IsNull()) { cout<<"(No model)  Type:"<<ent->DynamicType()->Name()<<endl; return 0; }
  cout<<" Num/Id :";  
  model->Print (ent, Message::DefaultMessenger(), 0);
  cout<<"  --  Recorded Type:"<<model->TypeName (ent)<<endl;
  return model->Number(ent);
}

//! Act gives a simple way to define and add functions to be ran
//! from a SessionPilot, as follows :
//!
//! Define a function as
//! static IFSelect_RetStatus myfunc
//! (const Standard_CString name,
//! const Handle(IFSelect_SessionPilot)& pilot)
//! { ... }
//! When ran, it receives the exact name (string) of the called
//! function, and the SessionPilot which brings other infos
//!
//! Add it by
//! XSDRAW_Act::AddFunc (name,help,myfunc);
//! for a normal function, or
//! XSDRAW_Act::AddFSet (name,help,myfunc);
//! for a function which is intended to create a control item
//! name and help are given as CString
//!
//! Then, it is available for run
class XSDRAW_Act : public IFSelect_Activator
{
 public:
  
  //! Creates an Act with a name, help and a function
  //! mode (Add or AddSet) is given when recording
  XSDRAW_Act(const Standard_CString name, const Standard_CString help, const XSDRAW_ActFunc func)
  : thename (name) , thehelp (help) , thefunc (func)
  {}
  
  //! Execution of Command Line. remark that <number> is senseless
  //! because each Act brings one and only one function
  Standard_EXPORT virtual IFSelect_ReturnStatus Do (const Standard_Integer, const Handle(IFSelect_SessionPilot)& pilot) Standard_OVERRIDE
  { return (thefunc? thefunc(pilot) : IFSelect_RetVoid); }
  
  //! Short Help for commands : returns the help given to create
  Standard_EXPORT virtual Standard_CString Help (const Standard_Integer) const Standard_OVERRIDE
  { return thehelp.ToCString(); }

  DEFINE_STANDARD_RTTI_INLINE(XSDRAW_Act,IFSelect_Activator)

 private:

  TCollection_AsciiString thename;
  TCollection_AsciiString thehelp;
  XSDRAW_ActFunc thefunc;
};

static TCollection_AsciiString thedefgr, thedefil;

void XSDRAW::SetGroup (const Standard_CString group, const Standard_CString file)
{
  thedefgr.Clear();  if (group[0] != '\0') thedefgr.AssignCat(group);
  thedefil.Clear();  if (file [0] != '\0') thedefil.AssignCat(file);
}

void XSDRAW::AddFunc (const Standard_CString name, const Standard_CString help, const XSDRAW_ActFunc func)
{
  Handle(XSDRAW_Act) act = new XSDRAW_Act (name,help,func);
  if (thedefgr.Length() > 0) act->SetForGroup (thedefgr.ToCString());
  act->Add    (1,name);
}

void XSDRAW::AddFSet (const Standard_CString name, const Standard_CString help, const XSDRAW_ActFunc func)
{
  Handle(XSDRAW_Act) act = new XSDRAW_Act (name,help,func);
  if (thedefgr.Length() > 0)
    act->SetForGroup (thedefgr.ToCString(),thedefil.ToCString());
  act->AddSet (1,name);
}
