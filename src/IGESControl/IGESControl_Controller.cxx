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

//#58 rln 28.12.98 changing default values for Global Section
//sln 14.01.2002 OCC51 : verifying whether entry model of method ActorRead is IGESDatat_IGESModel 

#include <Standard_Mutex.hxx>
#include <OSD_OpenFile.hxx>
#include <TCollection_HAsciiString.hxx>
#include <TopoDS_Shape.hxx>
#include <XSAlgo.hxx>
#include <Message_Messenger.hxx>
#include <Interface_Macros.hxx>
#include <Interface_Static.hxx>
#include <Interface_CheckIterator.hxx>
#include <Interface_ReportEntity.hxx>
#include <IGESAppli.hxx>
#include <IGESAppli_Protocol.hxx>
#include <IGESControl_AlgoContainer.hxx>
#include <IGESControl_Controller.hxx>
#include <IGESData.hxx>
#include <IGESData_IGESEntity.hxx>
#include <IGESData_IGESModel.hxx>
#include <IGESData_IGESDumper.hxx>
#include <IGESData_IGESWriter.hxx>
#include <IGESData_Protocol.hxx>
#include <IGESData_FileProtocol.hxx>
#include <IGESFile_Read.hxx>
#include <IGESSolid.hxx>
#include <IGESSolid_Protocol.hxx>
#include <IGESToBRep.hxx>
#include <IGESToBRep_Actor.hxx>
#include <IGESControl_ActorWrite.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESControl_Controller,XSControl_Controller)

class IGESControl_FloatFormat
{
  Standard_Boolean thezerosup;
  TCollection_AsciiString themainform;
  TCollection_AsciiString theformrange;
  Standard_Real therangemin;
  Standard_Real therangemax;
 public:
  IGESControl_FloatFormat ()
  : thezerosup (Standard_True), therangemin (0.1), therangemax (1000.)
  {
    const Standard_Integer digits = 12;
    char format[20];
    const char pourcent = '%';
    const char point = '.';
    Sprintf(format,  "%c%d%c%dE",pourcent,digits+2,point,digits);
    themainform.AssignCat  (format);
    Sprintf(format,  "%c%d%c%df",pourcent,digits+2,point,digits);
    theformrange.AssignCat (format);
  }
  //! Sets the Floatting Formats of IGESWriter to the recorded parameters
  void Perform (IGESData_IGESWriter& writer) const
  {
    writer.FloatWriter().SetFormat (themainform.ToCString());
    writer.FloatWriter().SetZeroSuppress (thezerosup);
    if (theformrange.Length() > 0) writer.FloatWriter().SetFormatForRange
      (theformrange.ToCString(), therangemin, therangemax);
  }
  //! Returns specific Label : for instance,
  //! "Float Format [ZeroSuppress] %E [, in range R1-R2 %f]"
  TCollection_AsciiString Label () const
  {
    TCollection_AsciiString lab("Float Format ");
    if (thezerosup) lab.AssignCat(" ZeroSup ");
    lab.AssignCat (themainform);
    if (theformrange.Length() > 0) {
      char mess[30];
      lab.AssignCat (", in range ");
      Standard_Integer convlen = Interface_FloatWriter::Convert
        (therangemin,mess,Standard_True,therangemin/2.,therangemax*2.,"%f","%f");
      mess[convlen] = ' ';  mess[convlen+1] = '\0';
      lab.AssignCat(mess);
      convlen = Interface_FloatWriter::Convert
        (therangemax,mess,Standard_True,therangemin/2.,therangemax*2.,"%f","%f");
      mess[convlen] = ':';  mess[convlen+1] = '\0';
      lab.AssignCat(mess);
      lab.AssignCat(theformrange.ToCString());
    }
    return lab;
  }
};
static const IGESControl_FloatFormat gFloatFormat;

//=======================================================================
//function : Constructor
//purpose  : 
//=======================================================================

IGESControl_Controller::IGESControl_Controller (const Standard_Boolean mod)
: XSControl_Controller ((Standard_CString ) (mod ? "FNES" : "IGES")),
  themodefnes (mod)
{
  SetDumpLevels (4,6);
  SetDumpHelp (0,"Only DNum");
  SetDumpHelp (1,"DNum, IGES Type & Form");
  SetDumpHelp (2,"Main Directory Informations");
  SetDumpHelp (3,"Complete Directory Part");
  SetDumpHelp (4,"Directory + Fields (except list contents)");
  SetDumpHelp (5,"Complete (with list contents)");
  SetDumpHelp (6,"Complete + Transformed data");

  myAdaptorProtocol = DefineProtocol();

  Handle(IGESToBRep_Actor) anactiges = new IGESToBRep_Actor;
  anactiges->SetContinuity(0);
  myAdaptorRead     = anactiges;

  //  -- STATICS

  TraceStatic ("read.iges.bspline.approxd1.mode",5);
  TraceStatic ("read.iges.bspline.continuity",5);

  TraceStatic ("write.iges.header.receiver",2);
  TraceStatic ("write.iges.header.author",2);
  TraceStatic ("write.iges.header.company",2);
  TraceStatic ("write.iges.unit",6);
  TraceStatic ("write.iges.brep.mode",6);
}

//=======================================================================
//function : ReadFile
//purpose  : 
//=======================================================================

Standard_Integer IGESControl_Controller::ReadFile (const Standard_CString theFileName, Handle(Interface_InterfaceModel)& theModel) const
{
  DeclareAndCast(IGESData_Protocol,prot,myAdaptorProtocol);

  Handle(IGESData_IGESModel) igesmod = new IGESData_IGESModel;
  char* pname=(char*) theFileName;
  Standard_Integer status = IGESFile_Read (pname,igesmod,prot);

  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  if (status < 0) sout<<"File not found : "<<theFileName<<endl;
  if (status > 0) sout<<"Error when reading file : "<<theFileName<<endl;
  if (status == 0) theModel = igesmod;
  else             theModel.Nullify();
  return status;
}

//=======================================================================
//function : WriteFile
//purpose  : 
//=======================================================================

Standard_Boolean IGESControl_Controller::WriteFile (const Standard_CString theFileName, const Handle(Interface_InterfaceModel)& theModel, Interface_CheckIterator& theChecks) const
{
//  Preparation
  DeclareAndCast(IGESData_IGESModel,igesmod,theModel);
  DeclareAndCast(IGESData_Protocol,prot,myAdaptorProtocol);
  if (igesmod.IsNull() || prot.IsNull()) return Standard_False;

  ofstream fout;
  OSD_OpenStream(fout,theFileName,ios::out );

  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  if (!fout) {
    theChecks.CCheck(0)->AddFail("IGES File could not be created");
    sout<<" - IGES File could not be created : " << theFileName << endl;
    return 0;
  }
  sout<<" IGES File Name : "<<theFileName;

  IGESData_IGESWriter VW(igesmod);  
  sout<<"("<<igesmod->NbEntities()<<" ents) ";

//  File Modifiers
  gFloatFormat.Perform(VW);
//   (impressions de mise au point)
  sout << " .. FileMod.1 "<< gFloatFormat.Label();
  sout << " (all model)";

//  Envoi
  VW.SendModel(prot);            
  sout<<" Write ";
  if (themodefnes) VW.WriteMode() = 10;
  Standard_Boolean status = VW.Print(fout);
  sout<<" Done"<<endl;

  errno = 0;
  fout.close();
  status = fout.good() && status && !errno;
  if(errno)
    sout << strerror(errno) << endl;

  return status;
}

//=======================================================================
//function : DumpEntity
//purpose  : 
//=======================================================================

void IGESControl_Controller::DumpEntity
  (const Handle(Interface_InterfaceModel)& model, 
   const Handle(Interface_Protocol)& protocol,
   const Handle(Standard_Transient)& entity,
   const Handle(Message_Messenger)& S, const Standard_Integer level) const
{
  DeclareAndCast(IGESData_IGESModel,igesmod,model);
  DeclareAndCast(IGESData_Protocol,igespro,protocol);
  DeclareAndCast(IGESData_IGESEntity,igesent,entity);
  if (igesmod.IsNull() || igespro.IsNull() || igesent.IsNull()) return;
  Standard_Integer num = igesmod->Number(igesent);
  if (num == 0) return;

  S<<" --- Entity "<<num;
  Standard_Boolean iserr = model->IsRedefinedContent(num);
  Handle(Standard_Transient) con;
  if (iserr) con = model->ReportEntity(num)->Content();
  if (entity.IsNull()) { S<<" Null"<<endl; return ;  }

//  On attaque le dump : d abord cas de l Erreur
  if (iserr) {
    S << " ERRONEOUS, Content, Type cdl : ";
    if (!con.IsNull()) S << con->DynamicType()->Name();
    else S << "(undefined)" << endl;
    igesent = GetCasted(IGESData_IGESEntity,con);
    con.Nullify();
    Handle(Interface_Check) check = model->ReportEntity(num)->Check();
    Interface_CheckIterator chlist;
    chlist.Add (check,num);
    chlist.Print (S,igesmod,Standard_False);
    if (igesent.IsNull()) return;
  }
  else S << " Type cdl : " << igesent->DynamicType()->Name();

  IGESData_IGESDumper dump(igesmod,igespro);
  try {
    OCC_CATCH_SIGNALS
    dump.Dump(igesent,S,level,(level-1)/3);
  }
  catch (Standard_Failure) {
    S << " **  Dump Interrupt **" << endl;
  }
}

//=======================================================================
//function : NewModel
//purpose  : 
//=======================================================================

Handle(Interface_InterfaceModel) IGESControl_Controller::NewModel () const
{
  //  On prend un modele qu on prepare avec les statiques enregistres
  Handle(IGESData_IGESModel) igm = IGESData::NewModel();
  IGESData_GlobalSection GS = igm->GlobalSection();

  GS.SetReceiveName (Interface_Static::Static("write.iges.header.receiver")->HStringValue());
  GS.SetUnitFlag    (Interface_Static::IVal("write.iges.unit"));
  GS.SetUnitName    (new TCollection_HAsciiString(Interface_Static::CVal("write.iges.unit")));
  GS.SetAuthorName  (Interface_Static::Static("write.iges.header.author")->HStringValue());
  GS.SetCompanyName (Interface_Static::Static("write.iges.header.company")->HStringValue());
  igm->SetGlobalSection (GS);
  return igm;
}

//=======================================================================
//function : ActorRead
//purpose  : 
//=======================================================================

Handle(Transfer_ActorOfTransientProcess) IGESControl_Controller::ActorRead (const Handle(Interface_InterfaceModel)& model) const
{
  DeclareAndCast(IGESToBRep_Actor,anactiges,myAdaptorRead);
  if (!anactiges.IsNull()) {
    // sln 14.01.2002 OCC51 : verifying whether entry model is IGESDatat_IGESModel,
    // if this condition is false new model is created
    Handle(Interface_InterfaceModel) aModel = model;
    if (model.IsNull() || !model->IsKind(STANDARD_TYPE(IGESData_IGESModel)))
      aModel = NewModel();
    anactiges->SetModel(GetCasted(IGESData_IGESModel,aModel));
    anactiges->SetContinuity(Interface_Static::IVal("read.iges.bspline.continuity"));
  }
  return myAdaptorRead;
}
  
//=======================================================================
//function : NewActorWrite
//purpose  : 
//=======================================================================

Handle(Transfer_ActorOfFinderProcess) IGESControl_Controller::NewActorWrite () const
{
  return new IGESControl_ActorWrite;
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void IGESControl_Controller::Init ()
{
  static Standard_Mutex gInitMutex;
  static volatile bool gInitDone = false;
  if (!gInitDone)
  {
    gInitMutex.Lock();
    if (!gInitDone)
    {
      //szv_c1:IGESSolid::Init();
      IGESAppli::Init();

      Handle(IGESControl_Controller) aController = new IGESControl_Controller(Standard_False);
      aController->AutoRecord();

      XSAlgo::Init();
      IGESToBRep::Init();
      IGESToBRep::SetAlgoContainer (new IGESControl_AlgoContainer());

      gInitDone = true;
	}
    gInitMutex.Unlock();
  }
}

//=======================================================================
//function : DefineProtocol
//purpose  : 
//=======================================================================

const Handle(IGESData_Protocol) & IGESControl_Controller::DefineProtocol ()
{
  static Handle(IGESData_FileProtocol) gIGESProto;
  if (gIGESProto.IsNull()) {
    //const Handle(IGESData_Protocol) &aIGESProto1 = IGESSolid::Protocol();
    const Handle(IGESData_Protocol) &aIGESProto2 = IGESAppli::Protocol();
    gIGESProto = new IGESData_FileProtocol;
    //gIGESProto->Add(aIGESProto1);
    gIGESProto->Add(aIGESProto2);
  }
  return gIGESProto;
}
