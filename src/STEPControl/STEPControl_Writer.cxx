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


#include <Interface_InterfaceModel.hxx>
#include <Interface_Macros.hxx>
#include <Interface_Static.hxx>
#include <Message_ProgressIndicator.hxx>
#include <STEPControl_ActorWrite.hxx>
#include <STEPControl_Controller.hxx>
#include <STEPControl_Writer.hxx>
#include <StepData_StepModel.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS_Shape.hxx>
#include <Transfer_FinderProcess.hxx>
#include <XSControl_WorkSession.hxx>

//=======================================================================
//function : STEPControl_Writer
//purpose  : 
//=======================================================================

STEPControl_Writer::STEPControl_Writer ()
: XSControl_Writer(new XSControl_WorkSession)
{
  STEPControl_Controller::Init();
  myWS->SelectNorm("STEP");
  myWS->NewModel();
}


//=======================================================================
//function : STEPControl_Writer

//purpose  : 
//=======================================================================

STEPControl_Writer::STEPControl_Writer (const Handle(XSControl_WorkSession)& theWS, const Standard_Boolean FromScratch)
: XSControl_Writer(theWS)
{
  STEPControl_Controller::Init();
  myWS->InitTransferReader(0);
  myWS->SelectNorm("STEP");
  Handle(StepData_StepModel) model = Model(FromScratch);
}


//=======================================================================
//function : SetWS

//purpose  : 
//=======================================================================

void STEPControl_Writer::SetWS(const Handle(XSControl_WorkSession)& WS, const Standard_Boolean scratch)
{
  myWS = WS;
  myWS->InitTransferReader(0);
  myWS->SelectNorm("STEP");
  Handle(StepData_StepModel) model = Model(scratch);
}


//=======================================================================
//function : Model
//purpose  : 
//=======================================================================

Handle(StepData_StepModel) STEPControl_Writer::Model (const Standard_Boolean newone) const
{
  DeclareAndCast(StepData_StepModel,model,myWS->Model());
  if (newone || model.IsNull())
    model = GetCasted(StepData_StepModel,myWS->NewModel());
  return model;
}


//=======================================================================
//function : Transfer
//purpose  : 
//=======================================================================

Interface_ReturnStatus STEPControl_Writer::Transfer (const TopoDS_Shape& theShape, const STEPControl_StepModelType theMode, const Standard_Boolean theCompGraph)
{
  Standard_Integer mws = -1;
  switch (theMode)
  {
    case STEPControl_AsIs :                   mws = 0;  break;
    case STEPControl_FacetedBrep :            mws = 1;  break;
    case STEPControl_ShellBasedSurfaceModel : mws = 2;  break;
    case STEPControl_ManifoldSolidBrep :      mws = 3;  break;
    case STEPControl_GeometricCurveSet :      mws = 4;  break;
    default : return Interface_RetError;
  }

  const Handle(Transfer_FinderProcess) &aWP = myWS->WriterProcess();

  // for progress indicator.
  const Handle(Message_ProgressIndicator) &progress = aWP->GetProgress();
  if ( !progress.IsNull() )
  {
    Standard_Integer nbfaces=0;
    for( TopExp_Explorer exp(theShape, TopAbs_FACE); exp.More(); exp.Next()) nbfaces++;
    progress->SetScale ( "Face", 0, nbfaces, 1 );
    progress->Show();
  }

  Handle(STEPControl_ActorWrite) anActor = Handle(STEPControl_ActorWrite)::DownCast(aWP->GetActor());
  if (anActor.IsNull())
  {
    const Handle(XSControl_Controller) &aController = myWS->NormAdaptor();
    if (aController.IsNull()) return Interface_RetError;

    anActor = Handle(STEPControl_ActorWrite)::DownCast(aController->NewActorWrite());
    aWP->SetActor(anActor);
  }

  anActor->SetTolerance(myTolerance);

  anActor->SetTransferMode(mws);

  anActor->SetGroupMode(Interface_Static::IVal("write.step.assembly"));

  return XSControl_Writer::TransferShape(theShape,theCompGraph);
}
