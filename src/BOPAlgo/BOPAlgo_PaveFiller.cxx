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


#include <BOPAlgo_PaveFiller.hxx>
#include <BOPAlgo_SectionAttribute.hxx>
#include <BOPAlgo_Alerts.hxx>
#include <BOPDS_Curve.hxx>
#include <BOPDS_DS.hxx>
#include <BOPDS_Iterator.hxx>
#include <BOPDS_PaveBlock.hxx>
#include <gp_Pnt.hxx>
#include <IntTools_Context.hxx>
#include <NCollection_BaseAllocator.hxx>
#include <Message.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>

//=======================================================================
//function : 
//purpose  : 
//=======================================================================
BOPAlgo_PaveFiller::BOPAlgo_PaveFiller()
:
  BOPAlgo_Algo()
{
  myDS = NULL;
  myIterator = NULL;
  myNonDestructive = Standard_False;
  myIsPrimary = Standard_True;
  myAvoidBuildPCurve = Standard_False;
  myGlue = BOPAlgo_GlueOff;
}
//=======================================================================
//function : 
//purpose  : 
//=======================================================================
BOPAlgo_PaveFiller::BOPAlgo_PaveFiller
  (const Handle(NCollection_BaseAllocator)& theAllocator)
:
  BOPAlgo_Algo(theAllocator)
{
  myDS = NULL;
  myIterator = NULL;
  myNonDestructive = Standard_False;
  myIsPrimary = Standard_True;
  myAvoidBuildPCurve = Standard_False;
  myGlue = BOPAlgo_GlueOff;
}
//=======================================================================
//function : ~
//purpose  : 
//=======================================================================
BOPAlgo_PaveFiller::~BOPAlgo_PaveFiller()
{
  //Clear();
}
//=======================================================================
//function : SetNonDestructive
//purpose  : 
//=======================================================================
void BOPAlgo_PaveFiller::SetNonDestructive(const Standard_Boolean bFlag)
{
  myNonDestructive=bFlag;
}
//=======================================================================
//function : NonDestructive
//purpose  : 
//=======================================================================
Standard_Boolean BOPAlgo_PaveFiller::NonDestructive()const 
{
  return myNonDestructive;
}
//=======================================================================
//function : SetGlue
//purpose  : 
//=======================================================================
void BOPAlgo_PaveFiller::SetGlue(const BOPAlgo_GlueEnum theGlue)
{
  myGlue=theGlue;
}
//=======================================================================
//function : Glue
//purpose  : 
//=======================================================================
BOPAlgo_GlueEnum BOPAlgo_PaveFiller::Glue() const 
{
  return myGlue;
}
//=======================================================================
//function : SetIsPrimary
//purpose  : 
//=======================================================================
void BOPAlgo_PaveFiller::SetIsPrimary(const Standard_Boolean bFlag)
{
  myIsPrimary=bFlag;
}
//=======================================================================
//function : IsPrimary
//purpose  : 
//=======================================================================
Standard_Boolean BOPAlgo_PaveFiller::IsPrimary()const 
{
  return myIsPrimary;
}
//=======================================================================
//function : Clear
//purpose  : 
//=======================================================================
void BOPAlgo_PaveFiller::Clear()
{
  BOPAlgo_Algo::Clear();
  if (myIterator) {
    delete myIterator;
    myIterator=NULL;
  }
  if (myDS) {
    delete myDS;
    myDS=NULL;
  }
}
//=======================================================================
//function : DS
//purpose  : 
//=======================================================================
const BOPDS_DS& BOPAlgo_PaveFiller::DS()
{
  return *myDS;
}
//=======================================================================
//function : PDS
//purpose  : 
//=======================================================================
BOPDS_PDS BOPAlgo_PaveFiller::PDS()
{
  return myDS;
}
//=======================================================================
//function : Context
//purpose  : 
//=======================================================================
const Handle(IntTools_Context)& BOPAlgo_PaveFiller::Context()
{
  return myContext;
}
//=======================================================================
//function : SectionAttribute
//purpose  : 
//=======================================================================
void BOPAlgo_PaveFiller::SetSectionAttribute
  (const BOPAlgo_SectionAttribute& theSecAttr)
{
  mySectionAttribute = theSecAttr;
}
//=======================================================================
//function : SetArguments
//purpose  : 
//=======================================================================
void BOPAlgo_PaveFiller::SetArguments(const TopTools_ListOfShape& theLS)
{
  myArguments=theLS;
}
//=======================================================================
//function : Arguments
//purpose  : 
//=======================================================================
const TopTools_ListOfShape& BOPAlgo_PaveFiller::Arguments()const
{
  return myArguments;
}
//=======================================================================
// function: Init
// purpose: 
//=======================================================================
void BOPAlgo_PaveFiller::Init()
{
  if (!myArguments.Extent()) {
    AddError (new BOPAlgo_AlertTooFewArguments);
    return;
  }
  //
  TopTools_ListIteratorOfListOfShape aIt(myArguments);
  for (; aIt.More(); aIt.Next()) {
    if (aIt.Value().IsNull()) {
      AddError (new BOPAlgo_AlertNullInputShapes);
      return;
    }
  }
  //
  // 0 Clear
  Clear();
  //
  // 1.myDS 
  myDS=new BOPDS_DS(myAllocator);
  myDS->SetArguments(myArguments);
  myDS->Init(myFuzzyValue);
  //
  // 2 myContext
  myContext=new IntTools_Context;
  //
  // 3.myIterator 
  myIterator=new BOPDS_Iterator(myAllocator);
  myIterator->SetRunParallel(myRunParallel);
  myIterator->SetDS(myDS);
  myIterator->Prepare(myContext, myUseOBB, myFuzzyValue);
  //
  // 4 NonDestructive flag
  SetNonDestructive();
}
//=======================================================================
// function: Perform
// purpose: 
//=======================================================================
void BOPAlgo_PaveFiller::Perform()
{
  try {
    OCC_CATCH_SIGNALS
    //
    PerformInternal();
    GetReport()->ApplyLastElapsedTime();
  }
  //
  catch (Standard_Failure) {
    AddError (new BOPAlgo_AlertIntersectionFailed);
  } 
}

//=======================================================================
// function: PerformInternal
// purpose: 
//=======================================================================
void BOPAlgo_PaveFiller::PerformInternal()
{
  Init();

  GetReport()->SetStoreElapsedTime (Standard_True);
  Message::Add_report_info ("BOPAlgo_PaveFiller::PerformInternal", myReport);

  Handle(Message_Alert) aFunctionAlert = GetLastInfo();
  Message::Add_report_info ("Init", myReport, aFunctionAlert);
  if (HasErrors()) {
    return; 
  }
  //
  Message::Add_report_info ("Prepare", myReport, aFunctionAlert);
  Prepare();
  if (HasErrors()) {
    return; 
  }
  // 00
  Message::Add_report_info ("PerformVV", myReport, aFunctionAlert);
  PerformVV();
  if (HasErrors()) {
    return; 
  }
  // 01
  Message::Add_report_info ("PerformVE", myReport, aFunctionAlert);
  PerformVE();
  if (HasErrors()) {
    return; 
  }
  //
  UpdatePaveBlocksWithSDVertices();
  // 11
  Message::Add_report_info ("PerformEE", myReport, aFunctionAlert);
  PerformEE();
  if (HasErrors()) {
    return; 
  }
  UpdatePaveBlocksWithSDVertices();
  // 02
  Message::Add_report_info ("PerformVF", myReport, aFunctionAlert);
  PerformVF();
  if (HasErrors()) {
    return; 
  }
  UpdatePaveBlocksWithSDVertices();
  // 12
  Message::Add_report_info ("PerformEF", myReport, aFunctionAlert);
  PerformEF();
  if (HasErrors()) {
    return; 
  }
  UpdatePaveBlocksWithSDVertices();
  UpdateInterfsWithSDVertices();

  // Force intersection of edges after increase
  // of the tolerance values of their vertices
  Message::Add_report_info ("ForceInterfEE", myReport, aFunctionAlert);
  ForceInterfEE();
  //
  // 22
  Message::Add_report_info ("PerformFF", myReport, aFunctionAlert);
  PerformFF();
  if (HasErrors()) {
    return; 
  }
  //
  UpdateBlocksWithSharedVertices();
  //
  Message::Add_report_info ("MakeSplitEdges", myReport, aFunctionAlert);
  MakeSplitEdges();
  if (HasErrors()) {
    return; 
  }
  //
  UpdatePaveBlocksWithSDVertices();
  //
  Message::Add_report_info ("MakeBlocks", myReport, aFunctionAlert);
  MakeBlocks();
  if (HasErrors()) {
    return; 
  }
  //
  Message::Add_report_info ("CheckSelfInterference", myReport, aFunctionAlert);
  CheckSelfInterference();
  //
  Message::Add_report_info ("UpdateInterfsWithSDVertices", myReport, aFunctionAlert);
  UpdateInterfsWithSDVertices();
  myDS->ReleasePaveBlocks();
  myDS->RefineFaceInfoOn();
  //
  Message::Add_report_info ("MakePCurves", myReport, aFunctionAlert);
  MakePCurves();
  if (HasErrors()) {
    return; 
  }
  //
  Message::Add_report_info ("ProcessDE", myReport, aFunctionAlert);
  ProcessDE();
  if (HasErrors()) {
    return; 
  }
}
