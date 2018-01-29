// Created by: Peter KURNEV
// Copyright (c) 2010-2014 OPEN CASCADE SAS
// Copyright (c) 2007-2010 CEA/DEN, EDF R&D, OPEN CASCADE
// Copyright (c) 2003-2007 OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN, CEDRAT,
//                         EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
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


#include <BOPAlgo_Builder.hxx>
#include <BOPAlgo_PaveFiller.hxx>
#include <BOPAlgo_Alerts.hxx>
#include <BOPTools_AlgoTools.hxx>
#include <BRep_Builder.hxx>
#include <IntTools_Context.hxx>
#include <Message.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <TopoDS_AlertWithShape.hxx>
#include <TopoDS_Compound.hxx>
#include <BRep_Builder.hxx>

#include <BOPDS_ShapeInfo.hxx>
#include <BOPDS_DS.hxx>

#include <BOPTools_AlgoTools.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>


//=======================================================================
//function : 
//purpose  : 
//=======================================================================
BOPAlgo_Builder::BOPAlgo_Builder()
:
  BOPAlgo_BuilderShape(),
  myArguments(myAllocator),
  myMapFence(100, myAllocator),
  myPaveFiller(NULL),
  myDS(NULL),
  myEntryPoint(0),
  myImages(100, myAllocator),
  myShapesSD(100, myAllocator),
  myOrigins(100, myAllocator),
  myNonDestructive(Standard_False),
  myGlue(BOPAlgo_GlueOff),
  myCheckInverted(Standard_True)
{
}
//=======================================================================
//function : 
//purpose  : 
//=======================================================================
BOPAlgo_Builder::BOPAlgo_Builder
  (const Handle(NCollection_BaseAllocator)& theAllocator)
:
  BOPAlgo_BuilderShape(theAllocator),
  myArguments(myAllocator),
  myMapFence(100, myAllocator),
  myPaveFiller(NULL),
  myDS(NULL),
  myEntryPoint(0),
  myImages(100, myAllocator), 
  myShapesSD(100, myAllocator),
  myOrigins(100, myAllocator),
  myNonDestructive(Standard_False),
  myGlue(BOPAlgo_GlueOff),
  myCheckInverted(Standard_True)
{
}
//=======================================================================
//function : ~
//purpose  : 
//=======================================================================
BOPAlgo_Builder::~BOPAlgo_Builder()
{
  if (myEntryPoint==1) {
    if (myPaveFiller) {
      delete myPaveFiller;
      myPaveFiller=NULL;
    }
  }
}
//=======================================================================
//function : Clear
//purpose  : 
//=======================================================================
void BOPAlgo_Builder::Clear()
{
  BOPAlgo_Algo::Clear();
  myArguments.Clear();
  myMapFence.Clear();
  myImages.Clear();
  myShapesSD.Clear();
  myOrigins.Clear();
}
//=======================================================================
//function : AddArgument
//purpose  : 
//=======================================================================
void BOPAlgo_Builder::AddArgument(const TopoDS_Shape& theShape)
{
  if (myMapFence.Add(theShape)) {
    myArguments.Append(theShape);
  }
}
//=======================================================================
//function : SetArguments
//purpose  : 
//=======================================================================
void BOPAlgo_Builder::SetArguments(const TopTools_ListOfShape& theShapes)
{
  TopTools_ListIteratorOfListOfShape aIt;
  //
  myArguments.Clear();
  //
  aIt.Initialize(theShapes);
  for (; aIt.More(); aIt.Next()) {
    const TopoDS_Shape& aS = aIt.Value();
    AddArgument(aS);
  }
}
//=======================================================================
// function: CheckData
// purpose: 
//=======================================================================
void BOPAlgo_Builder::CheckData()
{
  Standard_Integer aNb = myArguments.Extent();
  if (aNb<2) {
    AddError (new BOPAlgo_AlertTooFewArguments); // too few arguments to process
    return;
  }
  //
  CheckFiller();
}
//=======================================================================
// function: CheckFiller
// purpose: 
//=======================================================================
void BOPAlgo_Builder::CheckFiller()
{
  if (!myPaveFiller) {
    AddError (new BOPAlgo_AlertNoFiller);
    return;
  }
  GetReport()->Merge (myPaveFiller->GetReport(), GetLastInfo());
}

//=======================================================================
//function : Prepare
//purpose  : 
//=======================================================================
void BOPAlgo_Builder::Prepare()
{
  BRep_Builder aBB;
  TopoDS_Compound aC;
  //
  // 1. myShape is empty compound
  aBB.MakeCompound(aC);
  myShape=aC;
  myFlagHistory=Standard_True;
}
//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================
void BOPAlgo_Builder::Perform()
{
  GetReport()->Clear();
  //
  if (myEntryPoint==1) {
    if (myPaveFiller) {
      delete myPaveFiller;
      myPaveFiller=NULL;
    }
  }
  //
  Handle(NCollection_BaseAllocator) aAllocator=
    NCollection_BaseAllocator::CommonBaseAllocator();
  //
  BOPAlgo_PaveFiller* pPF=new BOPAlgo_PaveFiller(aAllocator);
  //
  pPF->SetArguments(myArguments);
  pPF->SetRunParallel(myRunParallel);
  pPF->SetProgressIndicator(myProgressIndicator);
  pPF->SetFuzzyValue(myFuzzyValue);
  pPF->SetNonDestructive(myNonDestructive);
  pPF->SetGlue(myGlue);
  pPF->SetUseOBB(myUseOBB);
  //
  pPF->Perform();
  //
  myEntryPoint=1;
  PerformInternal(*pPF);
}
//=======================================================================
//function : PerformWithFiller
//purpose  : 
//=======================================================================
void BOPAlgo_Builder::PerformWithFiller(const BOPAlgo_PaveFiller& theFiller)
{
  GetReport()->Clear();
  myEntryPoint=0;
  myNonDestructive = theFiller.NonDestructive();
  myFuzzyValue = theFiller.FuzzyValue();
  myGlue = theFiller.Glue();
  myUseOBB = theFiller.UseOBB();
  PerformInternal(theFiller);
}
//=======================================================================
//function : PerformInternal
//purpose  : 
//=======================================================================
void BOPAlgo_Builder::PerformInternal(const BOPAlgo_PaveFiller& theFiller)
{
  GetReport()->Clear();
  //
  GetReport()->SetStoreElapsedTime (Standard_True);
  Message::Add_report_info ("BOPAlgo_Builder::PerformInternal", myReport);
  Handle(Message_Alert) aPerformInternalAlert = GetLastInfo();

  Message::Add_report_info ("Arguments", myReport, aPerformInternalAlert);
  Handle(Message_Alert) anArgumentsAlert = GetLastInfo();
  TopTools_ListIteratorOfListOfShape aIt (Arguments());
  for (; aIt.More(); aIt.Next()) {
    const TopoDS_Shape& aShape = aIt.Value();
    TopoDS_AlertWithShape::Add_report_info (aShape, "", myReport, anArgumentsAlert);
  }
  Message::Add_report_info ("PerformInternal1", myReport, aPerformInternalAlert);
  try {
    OCC_CATCH_SIGNALS
    PerformInternal1(theFiller);

    const TopoDS_Shape& aShape = Shape();
    TopoDS_AlertWithShape::Add_report_info (aShape, "BOPAlgo_BuilderShape::Shape()", myReport, aPerformInternalAlert);
  }
  //
  catch (Standard_Failure) {
    AddError (new BOPAlgo_AlertBuilderFailed);
  }
}
//=======================================================================
//function : PerformInternal1
//purpose  : 
//=======================================================================
void BOPAlgo_Builder::PerformInternal1(const BOPAlgo_PaveFiller& theFiller)
{
  myPaveFiller=(BOPAlgo_PaveFiller*)&theFiller;
  myDS=myPaveFiller->PDS();
  myContext=myPaveFiller->Context();
  myFuzzyValue = myPaveFiller->FuzzyValue();
  myNonDestructive = myPaveFiller->NonDestructive();
  //
  // 1. CheckData
  Handle(Message_Alert) aFunctionAlert = GetLastInfo();
  Message::Add_report_info ("CheckData", myReport, aFunctionAlert);
  CheckData();
  if (HasErrors()) {
    return;
  }
  //
  // 2. Prepare
  Message::Add_report_info ("Prepare", myReport, aFunctionAlert);
  Prepare();
  if (HasErrors()) {
    return;
  }
  //
  // 3. Fill Images
  // 3.1 Vertice
  Message::Add_report_info ("FillImagesVertices", myReport, aFunctionAlert);
  FillImagesVertices();
  if (HasErrors()) {
    return;
  }
  //
  Message::Add_report_info ("BuildResult(TopAbs_VERTEX)", myReport, aFunctionAlert);
  BuildResult(TopAbs_VERTEX);
  if (HasErrors()) {
    return;
  }
  // 3.2 Edges
  Message::Add_report_info ("FillImagesEdges", myReport, aFunctionAlert);
  FillImagesEdges();
  if (HasErrors()) {
    return;
  }
  //
  Message::Add_report_info ("BuildResult(TopAbs_EDGE)", myReport, aFunctionAlert);
  BuildResult(TopAbs_EDGE);
  if (HasErrors()) {
    return;
  }
  //
  // 3.3 Wires
  Message::Add_report_info ("FillImagesContainers(TopAbs_WIRE)", myReport, aFunctionAlert);
  FillImagesContainers(TopAbs_WIRE);
  if (HasErrors()) {
    return;
  }
  //
  Message::Add_report_info ("BuildResult(TopAbs_WIRE)", myReport, aFunctionAlert);
  BuildResult(TopAbs_WIRE);
  if (HasErrors()) {
    return;
  }
  
  // 3.4 Faces
  Message::Add_report_info ("FillImagesFaces", myReport, aFunctionAlert);
  FillImagesFaces();
  if (HasErrors()) {
    return;
  }
  //
  Message::Add_report_info ("BuildResult(TopAbs_FACE", myReport, aFunctionAlert);
  BuildResult(TopAbs_FACE);
  if (HasErrors()) {
    return;
  }
  // 3.5 Shells
  Message::Add_report_info ("FillImagesContainers(TopAbs_SHELL)", myReport, aFunctionAlert);
  FillImagesContainers(TopAbs_SHELL);
  if (HasErrors()) {
    return;
  }
  
  Message::Add_report_info ("BuildResult(TopAbs_SHELL)", myReport, aFunctionAlert);
  BuildResult(TopAbs_SHELL);
  if (HasErrors()) {
    return;
  }
  // 3.6 Solids
  Message::Add_report_info ("FillImagesSolids", myReport, aFunctionAlert);
  FillImagesSolids();
  if (HasErrors()) {
    return;
  }
  
  Message::Add_report_info ("BuildResult(TopAbs_SOLID)", myReport, aFunctionAlert);
  BuildResult(TopAbs_SOLID);
  if (HasErrors()) {
    return;
  }
  // 3.7 CompSolids
  Message::Add_report_info ("FillImagesContainers(TopAbs_COMPSOLID)", myReport, aFunctionAlert);
  FillImagesContainers(TopAbs_COMPSOLID);
  if (HasErrors()) {
    return;
  }
  
  Message::Add_report_info ("BuildResult(TopAbs_COMPSOLID)", myReport, aFunctionAlert);
  BuildResult(TopAbs_COMPSOLID);
  if (HasErrors()) {
    return;
  }
  
  // 3.8 Compounds
  Message::Add_report_info ("FillImagesCompounds", myReport, aFunctionAlert);
  FillImagesCompounds();
  if (HasErrors()) {
    return;
  }
  
  Message::Add_report_info ("BuildResult(TopAbs_COMPOUND)", myReport, aFunctionAlert);
  BuildResult(TopAbs_COMPOUND);
  if (HasErrors()) {
    return;
  }
  //
  // 4.History
  Message::Add_report_info ("PrepareHistory", myReport, aFunctionAlert);
  PrepareHistory();
  //
  //
  // 5 Post-treatment 
  Message::Add_report_info ("PostTreat", myReport, aFunctionAlert);
  PostTreat();
  
}
//=======================================================================
//function : PostTreat
//purpose  : 
//=======================================================================
void BOPAlgo_Builder::PostTreat()
{
  Standard_Integer i, aNbS;
  TopAbs_ShapeEnum aType;
  TopTools_IndexedMapOfShape aMA;
  if (myPaveFiller->NonDestructive()) {
    // MapToAvoid
    aNbS=myDS->NbSourceShapes();
    for (i=0; i<aNbS; ++i) {
      const BOPDS_ShapeInfo& aSI=myDS->ShapeInfo(i);
      aType=aSI.ShapeType();
      if (aType==TopAbs_VERTEX ||
          aType==TopAbs_EDGE||
          aType==TopAbs_FACE) {
        const TopoDS_Shape& aS=aSI.Shape();
        aMA.Add(aS);
      }
    }
  }
  //
  BOPTools_AlgoTools::CorrectTolerances(myShape, aMA, 0.05, myRunParallel);
  BOPTools_AlgoTools::CorrectShapeTolerances(myShape, aMA, myRunParallel);
}
