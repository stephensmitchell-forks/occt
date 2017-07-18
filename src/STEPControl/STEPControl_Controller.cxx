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

#include <APIHeaderSection_EditHeader.hxx>
#include <APIHeaderSection_MakeHeader.hxx>
#include <HeaderSection.hxx>
#include <IFSelect_EditForm.hxx>
#include <IFSelect_ParamEditor.hxx>
#include <IFSelect_SelectModelRoots.hxx>
#include <IFSelect_SelectSignature.hxx>
#include <IFSelect_SignAncestor.hxx>
#include <IFSelect_SignCounter.hxx>
#include <Interface_InterfaceModel.hxx>
#include <Interface_Macros.hxx>
#include <RWHeaderSection.hxx>
#include <RWStepAP214.hxx>
#include <ShapeExtend.hxx>
#include <Standard_Type.hxx>
#include <Standard_Version.hxx>
#include <STEPControl_ActorRead.hxx>
#include <STEPControl_ActorWrite.hxx>
#include <STEPControl_Controller.hxx>
#include <StepData_FileProtocol.hxx>
#include <StepData_StepModel.hxx>
#include <STEPEdit.hxx>
#include <STEPEdit_EditContext.hxx>
#include <STEPEdit_EditSDR.hxx>
#include <StepSelect_StepType.hxx>
#include <StepSelect_WorkLibrary.hxx>
#include <STEPSelections_SelectAssembly.hxx>
#include <STEPSelections_SelectDerived.hxx>
#include <STEPSelections_SelectFaces.hxx>
#include <STEPSelections_SelectForTransfer.hxx>
#include <STEPSelections_SelectGSCurves.hxx>
#include <STEPSelections_SelectInstances.hxx>
#include <TCollection_HAsciiString.hxx>
#include <TopoDS_Shape.hxx>
#include <Transfer_ActorOfTransientProcess.hxx>
#include <Transfer_FinderProcess.hxx>
#include <XSAlgo.hxx>
#include <XSControl_WorkSession.hxx>
#include <XSAlgo_AlgoContainer.hxx>

IMPLEMENT_STANDARD_RTTIEXT(STEPControl_Controller,XSControl_Controller)

//  Pour NewModel et Write : definition de produit (temporaire ...)
STEPControl_Controller::STEPControl_Controller()
  : XSControl_Controller("STEP", "step")
{
  RWHeaderSection::Init();  RWStepAP214::Init();

  Handle(STEPControl_ActorWrite) ActWrite = new STEPControl_ActorWrite;
  myAdaptorWrite = ActWrite;

  Handle(StepSelect_WorkLibrary) swl = new StepSelect_WorkLibrary;
  swl->SetDumpLabel(1);
  myAdaptorLibrary  = swl;
  myAdaptorProtocol = STEPEdit::Protocol();
  myAdaptorRead     = new STEPControl_ActorRead;  // par ex pour Recognize

  SetModeWrite (0,4);
  SetModeWriteHelp (0,"As Is");
  SetModeWriteHelp (1,"Faceted Brep");
  SetModeWriteHelp (2,"Shell Based");
  SetModeWriteHelp (3,"Manifold Solid");
  SetModeWriteHelp (4,"Wireframe");

  //   ---  SELECTIONS, SIGNATURES, COMPTEURS, EDITEURS

  DeclareAndCast(IFSelect_Selection,xmr,SessionItem("xst-model-roots"));
  if (!xmr.IsNull()) {
    Handle(IFSelect_Signature) sty = STEPEdit::SignType();
    AddSessionItem (sty,"step-type");
    Handle(IFSelect_SignCounter) tys = new IFSelect_SignCounter(sty,Standard_False,Standard_True);
    AddSessionItem (tys,"step-types");

    //szv:mySignType = sty;
    
    //pdn S4133 18.02.99
    AddSessionItem (new IFSelect_SignAncestor(),"xst-derived");

    Handle(STEPSelections_SelectDerived) stdvar = new STEPSelections_SelectDerived();
    stdvar->SetProtocol(STEPEdit::Protocol());
    AddSessionItem (stdvar,"step-derived");
    
    Handle(IFSelect_SelectSignature) selsdr = STEPEdit::NewSelectSDR();
    selsdr->SetInput (xmr);
    AddSessionItem (selsdr,"step-shape-def-repr");

    AddSessionItem (STEPEdit::NewSelectPlacedItem(),"step-placed-items");
    // input deja pret avec ModelAll
    AddSessionItem (STEPEdit::NewSelectShapeRepr(),"step-shape-repr");
  }
  
  //pdn
  Handle(STEPSelections_SelectFaces) stfaces = new STEPSelections_SelectFaces;
  stfaces->SetInput (xmr);
  AddSessionItem (stfaces,"step-faces");
  
  Handle(STEPSelections_SelectInstances) stinst = new STEPSelections_SelectInstances;
  AddSessionItem (stinst,"step-instances");
  
  Handle(STEPSelections_SelectGSCurves) stcurves = new STEPSelections_SelectGSCurves;
  stcurves->SetInput (xmr);
  AddSessionItem (stcurves,"step-GS-curves");
  
  Handle(STEPSelections_SelectAssembly) assembly = new STEPSelections_SelectAssembly;
  assembly->SetInput (xmr);
  AddSessionItem (assembly,"step-assembly");
  
  Handle(APIHeaderSection_EditHeader) edhead = new APIHeaderSection_EditHeader;
  Handle(IFSelect_EditForm) edheadf = new IFSelect_EditForm (edhead,Standard_False,Standard_True,"Step Header");
  AddSessionItem (edhead,"step-header-edit");
  AddSessionItem (edheadf,"step-header");

  Handle(STEPEdit_EditContext) edctx = new STEPEdit_EditContext;
  Handle(IFSelect_EditForm) edctxf = new IFSelect_EditForm (edctx,Standard_False,Standard_True,"STEP Product Definition Context");
  AddSessionItem (edctx,"step-context-edit");
  AddSessionItem (edctxf,"step-context");


  Handle(STEPEdit_EditSDR) edsdr = new STEPEdit_EditSDR;
  Handle(IFSelect_EditForm) edsdrf = new IFSelect_EditForm (edsdr,Standard_False,Standard_True,"STEP Product Data (SDR)");
  AddSessionItem (edsdr,"step-SDR-edit");
  AddSessionItem (edsdrf,"step-SDR-data");

  // initialization of Standard Shape Healing
  ShapeExtend::Init();
}

Handle(Interface_InterfaceModel)  STEPControl_Controller::NewModel () const
{
  return STEPEdit::NewModel();
}

//  ####    PROVISOIRE ???   ####

IFSelect_ReturnStatus  STEPControl_Controller::TransferWriteShape
  (const TopoDS_Shape& shape,
   const Handle(Transfer_FinderProcess)& FP,
   const Handle(Interface_InterfaceModel)& model,
   const Standard_Integer modeshape) const
{
  if (modeshape < 0 || modeshape > 4) return IFSelect_RetError;
  if (model.IsNull()) return IFSelect_RetError;
  Handle(STEPControl_ActorWrite) ActWrite =
    Handle(STEPControl_ActorWrite)::DownCast(myAdaptorWrite);
//    A PRESENT ON PASSE PAR LE PROFILE
  if (!ActWrite.IsNull())
  {
    ActWrite->SetGroupMode("write.step.assembly");
  }

  return XSControl_Controller::TransferWriteShape (shape,FP,model,modeshape);
}

Standard_Boolean STEPControl_Controller::Init(const Handle(XSControl_WorkSession)& theWS)
{
  Handle(Interface_InterfaceModel) aModel = theWS->Model();
  if (aModel.IsNull()) return Standard_False;

  TraceNotStatic(aModel->GetParam("read.surfacecurve.mode"), 5);
  TraceNotStatic(aModel->GetParam("read.precision.mode"), 5);
  TraceNotStatic(aModel->GetParam("read.precision.val"), 5);
  TraceNotStatic(aModel->GetParam("write.precision.mode"), 6);
  TraceNotStatic(aModel->GetParam("write.precision.val"), 6);

  DeclareAndCast(STEPControl_ActorRead, aReadActor, myAdaptorRead);
  if (!aReadActor.IsNull()) aReadActor->SetModel(aModel);

  DeclareAndCast(STEPControl_ActorWrite, aWriteActor, myAdaptorWrite);
  if (!aWriteActor.IsNull()) aWriteActor->SetModel(aModel);

  return Standard_True;
}

//=======================================================================
//function : Customise
//purpose  : 
//=======================================================================

void STEPControl_Controller::Customise(Handle(XSControl_WorkSession)& WS) 
{
  XSControl_Controller::Customise(WS);

  Handle(IFSelect_SelectModelRoots) slr;
  Handle(Standard_Transient) slr1 = WS->NamedItem("xst-model-roots");
  if(!slr1.IsNull())
    slr = Handle(IFSelect_SelectModelRoots)::DownCast(slr1);
  else  {
    slr = new IFSelect_SelectModelRoots;
    WS->AddNamedItem ("xst-model-roots",slr);
  }

  Handle(STEPSelections_SelectForTransfer) st1= new STEPSelections_SelectForTransfer;
  st1->SetReader (WS->TransferReader());
  WS->AddNamedItem ("xst-transferrable-roots",st1);

  if (!slr.IsNull()) {
    Handle(IFSelect_Signature) sty = STEPEdit::SignType();
    WS->AddNamedItem ("step-type",sty);
    
    Handle(IFSelect_SignCounter) tys = new IFSelect_SignCounter(sty,Standard_False,Standard_True);
    WS->AddNamedItem ("step-types",tys);

	//szv:mySignType = sty;
    WS->SetSignType( sty );
    
    //pdn S4133 18.02.99
    WS->AddNamedItem ("xst-derived",new IFSelect_SignAncestor());
    Handle(STEPSelections_SelectDerived) stdvar = new STEPSelections_SelectDerived();
    stdvar->SetProtocol(STEPEdit::Protocol());
    WS->AddNamedItem ("step-derived",stdvar);
    
    Handle(IFSelect_SelectSignature) selsdr = STEPEdit::NewSelectSDR();
    selsdr->SetInput (slr);
    WS->AddNamedItem ("step-shape-def-repr",selsdr);
    Handle(IFSelect_SelectSignature) selrrs = STEPEdit::NewSelectPlacedItem();
    WS->AddNamedItem ("step-placed-items",selrrs);
    Handle(IFSelect_SelectSignature) selsr = STEPEdit::NewSelectShapeRepr();
    // input deja pret avec ModelAll
    WS->AddNamedItem ("step-shape-repr",selsr);
  }
  
  //pdn
  Handle(STEPSelections_SelectFaces) stfaces = new STEPSelections_SelectFaces;
  stfaces->SetInput (slr);
  WS->AddNamedItem ("step-faces",stfaces);
  
  Handle(STEPSelections_SelectInstances) stinst = new STEPSelections_SelectInstances;
  WS->AddNamedItem ("step-instances",stinst);
  
  Handle(STEPSelections_SelectGSCurves) stcurves = new STEPSelections_SelectGSCurves;
  stcurves->SetInput (slr);
  WS->AddNamedItem ("step-GS-curves",stcurves);
  
  Handle(STEPSelections_SelectAssembly) assembly = new STEPSelections_SelectAssembly;
  assembly->SetInput (slr);
  WS->AddNamedItem ("step-assembly",assembly);
  
  Handle(APIHeaderSection_EditHeader) edhead = new APIHeaderSection_EditHeader;
  Handle(IFSelect_EditForm) edheadf = new IFSelect_EditForm (edhead,Standard_False,Standard_True,"Step Header");
  WS->AddNamedItem ("step-header-edit",edhead);
  WS->AddNamedItem ("step-header",edheadf);

  Handle(STEPEdit_EditContext) edctx = new STEPEdit_EditContext;
  Handle(IFSelect_EditForm) edctxf = new IFSelect_EditForm (edctx,Standard_False,Standard_True,"STEP Product Definition Context");
  WS->AddNamedItem ("step-context-edit",edctx);
  WS->AddNamedItem ("step-context",edctxf);


  Handle(STEPEdit_EditSDR) edsdr = new STEPEdit_EditSDR;
  Handle(IFSelect_EditForm) edsdrf = new IFSelect_EditForm (edsdr,Standard_False,Standard_True,"STEP Product Data (SDR)");
  WS->AddNamedItem ("step-SDR-edit",edsdr);
  WS->AddNamedItem ("step-SDR-data",edsdrf);

  
  
}
