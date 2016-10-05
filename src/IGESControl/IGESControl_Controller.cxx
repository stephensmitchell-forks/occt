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

#include <IGESControl_AlgoContainer.hxx>
#include <IGESControl_Controller.hxx>
#include <IGESData.hxx>
#include <IGESData_IGESModel.hxx>
#include <IGESData_Protocol.hxx>
#include <IGESData_FileProtocol.hxx>
#include <IGESControl_WorkLibrary.hxx>
#include <IGESAppli.hxx>
#include <IGESAppli_Protocol.hxx>
#include <IGESSolid.hxx>
#include <IGESSolid_Protocol.hxx>
#include <IGESToBRep.hxx>
#include <IGESToBRep_Actor.hxx>
#include <IGESControl_ActorWrite.hxx>
#include <Interface_Macros.hxx>
#include <Interface_Static.hxx>
#include <TCollection_HAsciiString.hxx>
#include <TopoDS_Shape.hxx>
#include <XSAlgo.hxx>
#include <XSControl_WorkSession.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESControl_Controller,XSControl_Controller)

//=======================================================================
//function : IGESControl_Controller
//purpose  : 
//=======================================================================

IGESControl_Controller::IGESControl_Controller (const Standard_Boolean mod)
: XSControl_Controller ((Standard_CString ) (mod ? "FNES" : "IGES") , (Standard_CString ) (mod ? "fnes" : "iges") ),
  themode (mod)
{
  static Standard_Boolean init = Standard_False;
  if (!init) {
    init = Standard_True;
    IGESSolid::Init();
    IGESAppli::Init();
  }

  //  -- STATICS

  TraceStatic ("read.iges.bspline.approxd1.mode",5);
  TraceStatic ("read.iges.bspline.continuity",5);

  TraceStatic ("write.iges.header.receiver",2);
  TraceStatic ("write.iges.header.author",2);
  TraceStatic ("write.iges.header.company",2);
  TraceStatic ("write.iges.unit",6);
  TraceStatic ("write.iges.brep.mode",6);

  myAdaptorLibrary  = new IGESControl_WorkLibrary(themode);
  myAdaptorProtocol = DefineProtocol();

  Handle(IGESToBRep_Actor) anactiges = new IGESToBRep_Actor;
  anactiges->SetContinuity(0);
  myAdaptorRead     = anactiges;

  myAdaptorWrite    = new IGESControl_ActorWrite;

  SetModeWrite (0,1);
  SetModeWriteHelp (0,"Faces");
  SetModeWriteHelp (1,"BRep");
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
    Handle(Interface_InterfaceModel) aModel = (model->IsKind(STANDARD_TYPE(IGESData_IGESModel))? model : NewModel());
    anactiges->SetModel(GetCasted(IGESData_IGESModel,aModel));
    anactiges->SetContinuity(Interface_Static::IVal("read.iges.bspline.continuity"));
  }
  return myAdaptorRead;
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void IGESControl_Controller::Init ()
{
  static Standard_Boolean inic = Standard_False;
  if (!inic) {
    inic = Standard_True;
    Handle(IGESControl_Controller) ADIGES = new IGESControl_Controller(Standard_False);
    ADIGES->AutoRecord();
    XSAlgo::Init();
    IGESToBRep::Init();
    IGESToBRep::SetAlgoContainer (new IGESControl_AlgoContainer());
  }
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

const Handle(IGESData_Protocol) & IGESControl_Controller::DefineProtocol ()
{
  static Handle(IGESData_FileProtocol) gIGESProto;
  if (gIGESProto.IsNull()) {
    Handle(IGESData_Protocol) aIGESProto1 = IGESSolid::Protocol();
    Handle(IGESData_Protocol) aIGESProto2 = IGESAppli::Protocol();
    gIGESProto  = new IGESData_FileProtocol;
    gIGESProto->Add(aIGESProto1);
    gIGESProto->Add(aIGESProto2);
  }
  return gIGESProto;
}
