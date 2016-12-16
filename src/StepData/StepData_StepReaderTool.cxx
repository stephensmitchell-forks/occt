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


#include <Interface_Check.hxx>
#include <Interface_InterfaceModel.hxx>
#include <Interface_Macros.hxx>
#include <Message.hxx>
#include <Message_Messenger.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <StepData_Protocol.hxx>
#include <StepData_ReadWriteModule.hxx>
#include <StepData_StepModel.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepReaderTool.hxx>
#include <StepData_UndefinedEntity.hxx>
#include <TCollection_AsciiString.hxx>

//=======================================================================
//function : StepData_StepReaderTool
//purpose  : 
//=======================================================================
StepData_StepReaderTool::StepData_StepReaderTool
  (const Handle(StepData_StepReaderData)& reader,
   const Handle(StepData_Protocol)& protocol)
: Interface_FileReaderTool(protocol)
{
  SetData(reader,protocol);
}


//  ....         Methodes de preparations propres a StepReaderTool         ....


//=======================================================================
//function : Prepare
//purpose  : 
//=======================================================================

void StepData_StepReaderTool::Prepare ()
{
// ....            Gestion du Header : Preparation, lecture            .... //
  Standard_Integer i = 0;

// Reconnaissance des types
  DeclareAndCast(StepData_StepReaderData,stepdat,thereader);
  while ( (i = stepdat->FindNextHeaderRecord(i)) != 0) {
    Handle(Standard_Transient) ent;
    RecognizeByLib (i,ent);
    if (ent.IsNull()) ent = theproto->UnknownEntity();
    stepdat->BindEntity(i,ent);
  }

//  Reste la Resolution des references : ne concerne que les sous-listes
//  Assuree par ReaderData
  stepdat->PrepareHeader();

//   SetEntityNumbers a ete mis du cote de ReaderData, because beaucoup acces
  try {
    OCC_CATCH_SIGNALS
    stepdat->SetEntityNumbers(Standard_True);
  }
  catch(Standard_Failure) {
    const Handle(Message_Messenger) &sout = Message::DefaultMessenger();
    sout << " Exception Raised during Preparation :\n";
    sout << Standard_Failure::Caught()->GetMessageString();
    sout << "\n Now, trying to continue, but with presomption of failure\n";
  }
}


//=======================================================================
//function : Recognize
//purpose  : 
//=======================================================================

Standard_Boolean StepData_StepReaderTool::Recognize (const Standard_Integer num, Handle(Standard_Transient)& ent)
{
  return RecognizeByLib (num,ent);
}


// ....   Methodes pour la lecture du Modele (apres preparation)   .... //


//=======================================================================
//function : BeginRead
//purpose  : 
//=======================================================================

void StepData_StepReaderTool::BeginRead (const Handle(Interface_InterfaceModel)& amodel)
{
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  DeclareAndCast(StepData_StepModel,model,amodel);
  DeclareAndCast(StepData_StepReaderData,stepdat,thereader);

  model->ClearHeader();
  model->SetGlobalCheck(stepdat->GlobalCheck());
  Standard_Integer i = 0;
  while ( (i = stepdat->FindNextHeaderRecord(i)) != 0) {
    Handle(Standard_Transient) ent = stepdat->BoundEntity(i);
    Handle(Interface_Check) ach = new Interface_Check(ent);
    AnalyseRecord(i,ent,ach);
    if (ent->IsKind(STANDARD_TYPE(StepData_UndefinedEntity))) {
      TCollection_AsciiString mess("Header Entity not Recognized, StepType: ");
      mess.AssignCat(stepdat->RecordType(i));
      ach->AddWarning(mess.ToCString());
    }
    if (ach->HasFailed() || ach->HasWarnings()) {
      Handle(Interface_Check) mch = model->GlobalCheck();
      mch->GetMessages(ach); model->SetGlobalCheck(mch);
    }
    model->AddHeaderEntity(ent);
    if (ach->HasWarnings()) {
      Handle(Interface_Check) mch = model->GlobalCheck();
      Standard_Integer nbmess = ach->NbWarnings();
      sout<<nbmess<<" Warnings on Reading Header Entity N0."<<i<<":";
      if (!ent.IsNull()) sout << ent->DynamicType()->Name() << endl;
      for (Standard_Integer nf = 1; nf <= nbmess; nf ++)
	sout << ach->CWarning(nf) << "\n";
    }
    if (ach->HasFailed()) {
      Handle(Interface_Check) mch = model->GlobalCheck();
      Standard_Integer nbmess = ach->NbFails();
      sout << " Errors on Reading Header Entity N0."<<i<<":";
      if (!ent.IsNull()) sout << ent->DynamicType()->Name() << endl;
      for (Standard_Integer nf = 1; nf <= nbmess; nf ++)
	sout << ach->CFail(nf) << "\n";
    }
  }
}


//=======================================================================
//function : AnalyseRecord
//purpose  : 
//=======================================================================

Standard_Boolean StepData_StepReaderTool::AnalyseRecord
  (const Standard_Integer num,
   const Handle(Standard_Transient)& anent,
   Handle(Interface_Check)& acheck)
{
  DeclareAndCast(StepData_StepReaderData,stepdat,thereader);
  Handle(Interface_ReaderModule) imodule;
  Standard_Integer CN;
  if (therlib.Select(anent,imodule,CN))
  {
    Handle(StepData_ReadWriteModule) module =
      Handle(StepData_ReadWriteModule)::DownCast (imodule);
    module->ReadStep(CN,stepdat,num,acheck,anent);
  }
  else {
//  Pas trouve : tenter UndefinedEntity de StepData
    DeclareAndCast(StepData_UndefinedEntity,und,anent);
    if (und.IsNull()) acheck->AddFail
      ("# Entity neither Recognized nor set as UndefinedEntity from StepData #");
    else und->ReadRecord(stepdat,num,acheck);
  }
  return (!acheck->HasFailed());
}


//=======================================================================
//function : EndRead
//purpose  : 
//=======================================================================

void StepData_StepReaderTool::EndRead (const Handle(Interface_InterfaceModel)& amodel)
{
  DeclareAndCast(StepData_StepReaderData,stepdat,thereader);
  DeclareAndCast(StepData_StepModel,stepmodel,amodel);
  if (stepmodel.IsNull()) return;
  Standard_Integer i = 0;
  while ( (i = stepdat->FindNextRecord(i)) != 0) {
    stepmodel->SetIdentLabel(stepdat->BoundEntity(i),stepdat->RecordIdent(i));
  }
}
