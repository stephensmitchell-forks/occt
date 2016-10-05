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

#include <Interface_Macros.hxx>
#include <Interface_Static.hxx>
#include <StepAP214.hxx>
#include <StepAP214_Protocol.hxx>
#include <RWStepAP214.hxx>
#include <Standard_Version.hxx>
#include <STEPControl_ActorRead.hxx>
#include <STEPControl_ActorWrite.hxx>
#include <STEPControl_Controller.hxx>
#include <StepData_StepModel.hxx>
#include <STEPControl_WorkLibrary.hxx>
#include <TopoDS_Shape.hxx>
#include <XSAlgo.hxx>
#include <XSControl_WorkSession.hxx>
#include <APIHeaderSection_MakeHeader.hxx>

IMPLEMENT_STANDARD_RTTIEXT(STEPControl_Controller,XSControl_Controller)

//  Pour NewModel et Write : definition de produit (temporaire ...)
STEPControl_Controller::STEPControl_Controller ()
: XSControl_Controller ("STEP", "step")
{
  static Standard_Boolean init = Standard_False;
  if (!init) {
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

    init = Standard_True;
  }

  Handle(STEPControl_ActorWrite) ActWrite = new STEPControl_ActorWrite;
  ActWrite->SetGroupMode (Interface_Static::IVal("write.step.assembly"));
  myAdaptorWrite = ActWrite;

  Handle(STEPControl_WorkLibrary) swl = new STEPControl_WorkLibrary;
  swl->SetDumpLabel(1);
  myAdaptorLibrary  = swl;
  myAdaptorProtocol = StepAP214::Protocol();
  myAdaptorRead     = new STEPControl_ActorRead;  // par ex pour Recognize

  SetModeWrite (0,4);
  SetModeWriteHelp (0,"As Is");
  SetModeWriteHelp (1,"Faceted Brep");
  SetModeWriteHelp (2,"Shell Based");
  SetModeWriteHelp (3,"Manifold Solid");
  SetModeWriteHelp (4,"Wireframe");
  TraceStatic ("read.surfacecurve.mode",5);
}

Handle(Interface_InterfaceModel)  STEPControl_Controller::NewModel () const
{
  APIHeaderSection_MakeHeader head;
  return head.NewModel(StepAP214::Protocol());
}

//  ####    PROVISOIRE ???   ####

IFSelect_ReturnStatus  STEPControl_Controller::TransferWriteShape
  (const TopoDS_Shape& shape,
   const Handle(Transfer_FinderProcess)& FP,
   const Handle(Interface_InterfaceModel)& model,
   const Standard_Integer modeshape) const
{
  if (modeshape < 0 || modeshape > 4) return IFSelect_RetError;
  Handle(STEPControl_ActorWrite) ActWrite =
    Handle(STEPControl_ActorWrite)::DownCast(myAdaptorWrite);
//    A PRESENT ON PASSE PAR LE PROFILE
  if (!ActWrite.IsNull()) 
    ActWrite->SetGroupMode (Interface_Static::IVal("write.step.assembly"));

  return XSControl_Controller::TransferWriteShape (shape,FP,model,modeshape);
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void STEPControl_Controller::Init ()
{
  static Standard_Boolean inic = Standard_False;
  if (!inic) {
    inic = Standard_True;
    Handle(STEPControl_Controller) STEPCTL = new STEPControl_Controller;
    STEPCTL->AutoRecord();  // avec les noms donnes a la construction
    XSAlgo::Init();                                                                                                        
  }
}
