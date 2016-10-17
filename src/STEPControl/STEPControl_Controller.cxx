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

//:j4 gka 16.03.99 S4134
//    gka 05.04.99 S4136: parameters definitions changed

#include <Standard_Mutex.hxx>
#include <Standard_Version.hxx>
#include <OSD_OpenFile.hxx>
#include <Message_Messenger.hxx>
#include <Interface_Macros.hxx>
#include <Interface_Static.hxx>
#include <Interface_CheckIterator.hxx>
#include <Interface_ReportEntity.hxx>
#include <XSAlgo.hxx>
#include <RWStepAP214.hxx>
#include <StepFile_Read.hxx>
#include <StepData_StepDumper.hxx>
#include <StepData_StepWriter.hxx>
#include <StepAP214.hxx>
#include <StepAP214_Protocol.hxx>
#include <STEPControl_ActorRead.hxx>
#include <STEPControl_ActorWrite.hxx>
#include <STEPControl_Controller.hxx>
#include <StepData_StepModel.hxx>
#include <APIHeaderSection_MakeHeader.hxx>

IMPLEMENT_STANDARD_RTTIEXT(STEPControl_Controller,XSControl_Controller)

/*class STEPControl_FloatFormat
{
  Standard_Boolean thezerosup;
  TCollection_AsciiString themainform;
  TCollection_AsciiString theformrange;
  Standard_Real therangemin;
  Standard_Real therangemax;
 public:
  STEPControl_FloatFormat ()
  : thezerosup (Standard_True) , themainform ("%E") ,
    theformrange ("%f") , therangemin (0.1) , therangemax (1000.)
  {}
  //! Sets the Floatting Formats of StepWriter to the recorded parameters
  void Perform (StepData_StepWriter& writer) const
  {
    writer.FloatWriter().SetFormat (themainform.ToCString());
    writer.FloatWriter().SetZeroSuppress (thezerosup);
    if (theformrange.Length() > 0) writer.FloatWriter().SetFormatForRange (theformrange.ToCString(), therangemin, therangemax);
  }
  //! Returns specific Label : for instance,
  //! "Float Format [ZeroSuppress] %E [, in range R1-R2 %f]"
  TCollection_AsciiString Label () const
  {
    TCollection_AsciiString lab("Float Format ");
    if (thezerosup) lab.AssignCat(" ZeroSuppress");
    lab.AssignCat (themainform);
    if (theformrange.Length() > 0) {
      char mess[30];
      Sprintf(mess,", in range %f %f %s",
	      therangemin,therangemax,theformrange.ToCString());
      lab.AssignCat(mess);
    }
    return lab;
  }
};
static const STEPControl_FloatFormat gFloatFormat;*/

//=======================================================================
//function : Constructor
//purpose  : 
//=======================================================================

STEPControl_Controller::STEPControl_Controller ()
: XSControl_Controller ("STEP")
{
  SetDumpLevels (1,2);
  SetDumpHelp (0,"#id + Step Type");
  SetDumpHelp (1,"Entity as in file");
  SetDumpHelp (2,"Entity + shareds (level 1) as in file");

  myAdaptorProtocol = StepAP214::Protocol();
  myAdaptorRead     = new STEPControl_ActorRead;  // par ex pour Recognize

  TraceStatic ("read.surfacecurve.mode",5);
}

//=======================================================================
//function : ReadFile
//purpose  : 
//=======================================================================

Standard_Integer STEPControl_Controller::ReadFile (const Standard_CString theFileName, Handle(Interface_InterfaceModel)& theModel) const
{
  DeclareAndCast(StepData_Protocol,stepro,myAdaptorProtocol);
  if (stepro.IsNull()) return 1;

  Handle(StepData_StepModel) stepmodel  = new StepData_StepModel;
  theModel = stepmodel;

  StepFile_ReadTrace (0);
  char *pName=(char *)theFileName;
  return StepFile_Read (pName,stepmodel,stepro);
}

//=======================================================================
//function : WriteFile
//purpose  : 
//=======================================================================

Standard_Boolean STEPControl_Controller::WriteFile (const Standard_CString theFileName, const Handle(Interface_InterfaceModel)& theModel, Interface_CheckIterator& theChecks) const
{
  //  Preparation
  DeclareAndCast(StepData_StepModel,stepmodel,theModel);
  DeclareAndCast(StepData_Protocol,stepro,myAdaptorProtocol);
  if (stepmodel.IsNull() || stepro.IsNull()) return Standard_False;

  ofstream fout;
  OSD_OpenStream(fout,theFileName,ios::out|ios::trunc);

  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  if (!fout || !fout.rdbuf()->is_open()) {
    theChecks.CCheck(0)->AddFail("Step File could not be created");
    sout<<" Step File could not be created : " << theFileName << endl;
    return 0;
  }
  sout << " Step File Name : "<<theFileName;

  StepData_StepWriter SW(stepmodel);
  sout<<"("<<stepmodel->NbEntities()<<" ents) ";

//  File Modifiers
  //gFloatFormat.Perform(SW);
//   (impressions de mise au point)
  //sout << " .. FileMod.1 " << gFloatFormat.Label();
  //sout << " (all model)";

//  Envoi
  SW.SendModel(stepro);
  Interface_CheckIterator chl = SW.CheckList();
  for (chl.Start(); chl.More(); chl.Next())
    theChecks.CCheck(chl.Number())->GetMessages(chl.Value());
  sout<<" Write ";
  Standard_Boolean isGood = SW.Print(fout);
  sout<<" Done"<<endl;
      
  errno = 0;
  fout.close();
  isGood = fout.good() && isGood && !errno;
  if(errno)
    sout << strerror(errno) << endl;
  return isGood;  
}

//=======================================================================
//function : DumpEntity
//purpose  : 
//=======================================================================

void STEPControl_Controller::DumpEntity
  (const Handle(Interface_InterfaceModel)& model,
   const Handle(Interface_Protocol)& protocol,
   const Handle(Standard_Transient)& entity,
   const Handle(Message_Messenger)& S, const Standard_Integer level) const
{
  Standard_Integer nument = model->Number(entity);
  if (nument <= 0 || nument > model->NbEntities()) return;
  Standard_Boolean iserr = model->IsRedefinedContent(nument);
  Handle(Standard_Transient) ent, con;  ent = entity;
  S<<" --- (STEP) Entity ";  model->Print(entity,S);
  if (iserr) con = model->ReportEntity(nument)->Content();
  if (entity.IsNull()) {  S<<" Null"<<endl; return;  }

//  On attaque le dump : d abord cas de l Erreur
  S << " Type cdl : " << entity->DynamicType()->Name() << endl;
  if (iserr)
    S<<" ***  NOT WELL LOADED : CONTENT FROM FILE  ***"<<endl;
  else if (model->IsUnknownEntity(nument))
    S<<" ***  UNKNOWN TYPE  ***"<<endl;

  StepData_StepDumper dump(GetCasted(StepData_StepModel,model),
                           GetCasted(StepData_Protocol,protocol),1);
  dump.Dump(S,ent,level);
}

//=======================================================================
//function : NewModel
//purpose  : 
//=======================================================================

Handle(Interface_InterfaceModel) STEPControl_Controller::NewModel () const
{
  APIHeaderSection_MakeHeader head;
  return head.NewModel(StepAP214::Protocol());
}
  
//=======================================================================
//function : NewActorWrite
//purpose  : 
//=======================================================================

Handle(Transfer_ActorOfFinderProcess) STEPControl_Controller::NewActorWrite() const
{
  return new STEPControl_ActorWrite;
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void STEPControl_Controller::Init ()
{
  static Standard_Mutex gInitMutex;
  static volatile bool gInitDone = false;
  if (!gInitDone)
  {
    gInitMutex.Lock();
    if (!gInitDone)
    {
      RWStepAP214::Init();

      Interface_Static::Init ("step","write.step.product.name",'t',"Open CASCADE STEP translator " OCC_VERSION_STRING);
      Interface_Static::Init ("step","write.step.assembly",'e',"");
      Interface_Static::Init ("step","write.step.assembly",'&',"enum 0");
      Interface_Static::Init ("step","write.step.assembly",'&',"eval Off");
      Interface_Static::Init ("step","write.step.assembly",'&',"eval On");
      Interface_Static::Init ("step","write.step.assembly",'&',"eval Auto");
      Interface_Static::SetCVal("write.step.assembly","Auto");

      Interface_Static::Init("step","step.angleunit.mode", 'e',"");
      Interface_Static::Init("step","step.angleunit.mode", '&',"enum 0");
      Interface_Static::Init("step","step.angleunit.mode", '&',"eval File");
      Interface_Static::Init("step","step.angleunit.mode", '&',"eval Rad");
      Interface_Static::Init("step","step.angleunit.mode", '&',"eval Deg");
      Interface_Static::SetCVal("step.angleunit.mode","File");

      Interface_Static::Init("step","write.step.schema", 'e',"");
      Interface_Static::Init("step","write.step.schema",'&',"enum 1");
      Interface_Static::Init("step","write.step.schema",'&',"eval AP214CD");
      Interface_Static::Init("step","write.step.schema",'&',"eval AP214DIS");
      Interface_Static::Init("step","write.step.schema",'&',"eval AP203");
      Interface_Static::Init("step","write.step.schema",'&',"eval AP214IS");
      Interface_Static::Init("step","write.step.schema",'&',"eval AP242DIS");
      Interface_Static::SetCVal("write.step.schema","AP214IS");

      // Type of Product Definition for reading
      // Note: the numbers should be consistent with function FindShapeReprType()
      // in STEPControl_ActorRead.cxx
      Interface_Static::Init("step","read.step.shape.repr",'e',"");
      Interface_Static::Init("step","read.step.shape.repr",'&',"enum 1");
      Interface_Static::Init("step","read.step.shape.repr",'&',"eval All");   // 1
      Interface_Static::Init("step","read.step.shape.repr",'&',"eval ABSR");  // 2
      Interface_Static::Init("step","read.step.shape.repr",'&',"eval MSSR");  // 3
      Interface_Static::Init("step","read.step.shape.repr",'&',"eval GBSSR"); // 4
      Interface_Static::Init("step","read.step.shape.repr",'&',"eval FBSR");  // 5
      Interface_Static::Init("step","read.step.shape.repr",'&',"eval EBWSR"); // 6
      Interface_Static::Init("step","read.step.shape.repr",'&',"eval GBWSR"); // 7
      Interface_Static::SetCVal("read.step.shape.repr","All");

      // Mode for reading shapes attached to main SDR by SRR
      // (hybrid model representation in AP203 since 1998)
      Interface_Static::Init("step","read.step.shape.relationship",'e',"");
      Interface_Static::Init("step","read.step.shape.relationship",'&',"enum 0");
      Interface_Static::Init("step","read.step.shape.relationship",'&',"eval OFF");
      Interface_Static::Init("step","read.step.shape.relationship",'&',"eval ON");
      Interface_Static::SetCVal("read.step.shape.relationship","ON");

      // Mode for reading shapes attached to Product by ShapeAspect 
      // (hybrid model representation in AP203 before 1998)
      Interface_Static::Init("step","read.step.shape.aspect",'e',"");
      Interface_Static::Init("step","read.step.shape.aspect",'&',"enum 0");
      Interface_Static::Init("step","read.step.shape.aspect",'&',"eval OFF");
      Interface_Static::Init("step","read.step.shape.aspect",'&',"eval ON");
      Interface_Static::SetCVal("read.step.shape.aspect","ON");

      // Mode for reading SDR and ShapeRepr if it is necessary
      Interface_Static::Init("step","read.step.product.mode",'e',"");
      Interface_Static::Init("step","read.step.product.mode",'&',"enum 0");
      Interface_Static::Init("step","read.step.product.mode",'&',"eval OFF");
      Interface_Static::Init("step","read.step.product.mode",'&',"eval ON");
      Interface_Static::SetCVal("read.step.product.mode","ON");

      // Order of reading ShapeDefinitionRepresentation in ProductDefinition
      Interface_Static::Init("step","read.step.product.context",'e',"");
      Interface_Static::Init("step","read.step.product.context",'&',"enum 1");
      Interface_Static::Init("step","read.step.product.context",'&',"eval all");     // 1
      Interface_Static::Init("step","read.step.product.context",'&',"eval design");  // 2
      Interface_Static::Init("step","read.step.product.context",'&',"eval analysis");// 3
      Interface_Static::SetCVal("read.step.product.context","all");

      // What we try to read in ProductDefinition
      Interface_Static::Init("step","read.step.assembly.level",'e',"");
      Interface_Static::Init("step","read.step.assembly.level",'&',"enum 1");
      Interface_Static::Init("step","read.step.assembly.level",'&',"eval all");      // 1
      Interface_Static::Init("step","read.step.assembly.level",'&',"eval assembly"); // 2
      Interface_Static::Init("step","read.step.assembly.level",'&',"eval structure");// 3
      Interface_Static::Init("step","read.step.assembly.level",'&',"eval shape");    // 4
      Interface_Static::SetCVal("read.step.assembly.level","all");

      // unit: supposed to be cascade unit (target unit for reading)
      Interface_Static::Init("step","write.step.unit", 'e',"");
      Interface_Static::Init("step","write.step.unit",'&',"enum 1");
      Interface_Static::Init("step","write.step.unit",'&',"eval INCH");  // 1
      Interface_Static::Init("step","write.step.unit",'&',"eval MM");    // 2
      Interface_Static::Init("step","write.step.unit",'&',"eval ??");    // 3
      Interface_Static::Init("step","write.step.unit",'&',"eval FT");    // 4
      Interface_Static::Init("step","write.step.unit",'&',"eval MI");    // 5
      Interface_Static::Init("step","write.step.unit",'&',"eval M");     // 6
      Interface_Static::Init("step","write.step.unit",'&',"eval KM");    // 7
      Interface_Static::Init("step","write.step.unit",'&',"eval MIL");   // 8
      Interface_Static::Init("step","write.step.unit",'&',"eval UM");    // 9
      Interface_Static::Init("step","write.step.unit",'&',"eval CM");    //10
      Interface_Static::Init("step","write.step.unit",'&',"eval UIN");   //11
      Interface_Static::SetCVal ("write.step.unit","MM");

      // Non-manifold topology reading: OFF by default (ssv; 26.11.2010)
      Interface_Static::Init ("step","read.step.nonmanifold",'e',"");
      Interface_Static::Init ("step","read.step.nonmanifold",'&',"enum 0");
      Interface_Static::Init ("step","read.step.nonmanifold",'&',"eval Off");
      Interface_Static::Init ("step","read.step.nonmanifold",'&',"eval On");
      Interface_Static::SetIVal("read.step.nonmanifold",0);

      // Non-manifold topology writing: OFF by default (ssv; 26.11.2010)
      Interface_Static::Init ("step","write.step.nonmanifold",'e',"");
      Interface_Static::Init ("step","write.step.nonmanifold",'&',"enum 0");
      Interface_Static::Init ("step","write.step.nonmanifold",'&',"eval Off");
      Interface_Static::Init ("step","write.step.nonmanifold",'&',"eval On");
      Interface_Static::SetIVal("write.step.nonmanifold",0);

      // I-Deas-like STEP processing: OFF by default (ssv; 22.11.2010)
      Interface_Static::Init ("step","read.step.ideas",'e',"");
      Interface_Static::Init ("step","read.step.ideas",'&',"enum 0");
      Interface_Static::Init ("step","read.step.ideas",'&',"eval Off");
      Interface_Static::Init ("step","read.step.ideas",'&',"eval On");
      Interface_Static::SetIVal("read.step.ideas",0);

      //Parameter to write all free vertices in one SDR (name and style of vertex are lost) (default)
      //or each vertex in its own SDR (name and style of vertex are exported). (ika; 21.07.2014)
      Interface_Static::Init ("step","write.step.vertex.mode",'e',"");
      Interface_Static::Init ("step","write.step.vertex.mode",'&',"enum 0");
      Interface_Static::Init ("step","write.step.vertex.mode",'&',"eval One Compound");
      Interface_Static::Init ("step","write.step.vertex.mode",'&',"eval Single Vertex");
      Interface_Static::SetIVal("write.step.vertex.mode",0);
  
      // abv 15.11.00: ShapeProcessing
      Interface_Static::Init ("XSTEP","write.step.resource.name",'t',"STEP");
      Interface_Static::Init ("XSTEP","read.step.resource.name",'t',"STEP");
      Interface_Static::Init ("XSTEP","write.step.sequence",'t',"ToSTEP");
      Interface_Static::Init ("XSTEP","read.step.sequence",'t',"FromSTEP");

      // ika 28.07.16: Paremeter to read all top level solids and shells,
      // should be used only in case of invalid shape_representation without links to shapes.
      Interface_Static::Init("step", "read.step.all.shapes", 'e', "");
      Interface_Static::Init("step", "read.step.all.shapes", '&', "enum 0");
      Interface_Static::Init("step", "read.step.all.shapes", '&', "eval Off");
      Interface_Static::Init("step", "read.step.all.shapes", '&', "eval On");
      Interface_Static::SetIVal("read.step.all.shapes", 0);

      XSAlgo::Init();

      Handle(STEPControl_Controller) aController = new STEPControl_Controller;
      aController->AutoRecord();

      gInitDone = true;
	}
    gInitMutex.Unlock();
  }
}
