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
#include <BOPCol_MapOfShape.hxx>
#include <BOPCol_NCVector.hxx>
#include <BOPCol_Parallel.hxx>
#include <BOPDS_CommonBlock.hxx>
#include <BOPDS_Curve.hxx>
#include <BOPDS_DS.hxx>
#include <BOPDS_FaceInfo.hxx>
#include <BOPDS_Interf.hxx>
#include <BOPDS_Iterator.hxx>
#include <BOPDS_ListOfPaveBlock.hxx>
#include <BOPDS_MapOfPaveBlock.hxx>
#include <BOPDS_Pave.hxx>
#include <BOPDS_PaveBlock.hxx>
#include <BOPDS_ShapeInfo.hxx>
#include <BOPDS_VectorOfCurve.hxx>
#include <BOPDS_VectorOfFaceInfo.hxx>
#include <BOPDS_VectorOfInterfFF.hxx>
#include <BOPDS_VectorOfListOfPaveBlock.hxx>
#include <BOPTools_AlgoTools.hxx>
#include <BOPTools_AlgoTools2D.hxx>
#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepBndLib.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Plane.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_Surface.hxx>
#include <gp_Pnt.hxx>
#include <IntTools_Context.hxx>
#include <NCollection_IncAllocator.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Vertex.hxx>
#include <GeomAPI_ProjectPointOnCurve.hxx>
#include <GeomAPI_ProjectPointOnSurf.hxx>

static
  Standard_Boolean IsBasedOnPlane(const TopoDS_Face& aF);


static void UpdateVertices(const TopoDS_Edge& aE, 
                           const TopoDS_Face& aF);

//=======================================================================
//class    : BOPAlgo_SplitEdge
//purpose  : 
//=======================================================================
class BOPAlgo_SplitEdge : public BOPAlgo_Algo  {
 
 public:
  DEFINE_STANDARD_ALLOC

  BOPAlgo_SplitEdge() :
    BOPAlgo_Algo() {
    myT1=0.;
    myT2=0.;
    myTol=0.;
  }
  //
  virtual ~BOPAlgo_SplitEdge() {
  }
  //
  void SetData(const TopoDS_Edge& aE,
               const TopoDS_Vertex& aV1,
               const Standard_Real aT1,
               const TopoDS_Vertex& aV2,
               const Standard_Real aT2) {
    myE=aE;
    myV1=aV1;
    myT1=aT1;
    myV2=aV2;
    myT2=aT2;
    myESp=aE;
  }
  //
  void SetPaveBlock(const Handle(BOPDS_PaveBlock)& aPB) {
    myPB=aPB;
  }
  //
  Handle(BOPDS_PaveBlock)& PaveBlock() {
    return myPB;
  }
  //
  void SetCommonBlock(const Handle(BOPDS_CommonBlock)& aCB) {
    myCB=aCB;
  }
  //
  void SetTolerance(const Standard_Real aTol) {
    myTol = aTol;
  }
  //
  Standard_Real Tolerance() const {
    return myTol;
  }
  //
  Handle(BOPDS_CommonBlock)& CommonBlock() {
    return myCB;
  }
  //
  const TopoDS_Edge& SplitEdge() const {
    return myESp;
  }
  //
  const Bnd_Box Box() {
    return myBox;
  }
  //
  virtual void Perform () {
    BOPAlgo_Algo::UserBreak();
    BOPTools_AlgoTools::MakeSplitEdge(myE, 
                                      myV1, myT1, 
                                      myV2, myT2, 
                                      myESp);
    BRepBndLib::Add(myESp, myBox);  
  }
  //
 protected:
  // ->
  TopoDS_Edge myE;
  TopoDS_Vertex myV1;
  Standard_Real myT1;
  TopoDS_Vertex myV2;
  Standard_Real myT2;
  Standard_Real myTol;
  // <->
  Handle(BOPDS_PaveBlock) myPB;
  Handle(BOPDS_CommonBlock) myCB;
  // <-
  TopoDS_Edge myESp;
  Bnd_Box myBox;
};
//
//=======================================================================
typedef BOPCol_NCVector
  <BOPAlgo_SplitEdge> BOPAlgo_VectorOfSplitEdge; 
//
typedef BOPCol_Functor 
  <BOPAlgo_SplitEdge,
  BOPAlgo_VectorOfSplitEdge> BOPAlgo_SplitEdgeFunctor;
//
typedef BOPCol_Cnt 
  <BOPAlgo_SplitEdgeFunctor,
  BOPAlgo_VectorOfSplitEdge> BOPAlgo_SplitEdgeCnt;
//
//=======================================================================
//class    : BOPAlgo_MPC
//purpose  : 
//=======================================================================
class BOPAlgo_MPC : public BOPAlgo_Algo  {
 
 public:
  DEFINE_STANDARD_ALLOC

  BOPAlgo_MPC() : 
    BOPAlgo_Algo(),
    myFlag(Standard_False) {
  };
  //
  virtual ~BOPAlgo_MPC(){
  };
  //
  void SetEdge(const TopoDS_Edge& aE) {
    myE=aE;
  }
  //
  const TopoDS_Edge& Edge() const {
    return myE;
  }
  //
  void SetFace(const TopoDS_Face& aF) {
    myF=aF;
  }
  //
  const TopoDS_Face& Face() const {
    return myF;
  }
  //
  void SetFlag(const Standard_Boolean bFlag) {
    myFlag=bFlag;
  }
  //
  Standard_Boolean Flag() const {
    return myFlag;
  }
  //
  void SetData(const TopoDS_Edge& aEz,
               const TopoDS_Vertex& aV1,
               const Standard_Real aT1,
               const TopoDS_Vertex& aV2,
               const Standard_Real aT2) {
    myEz=aEz;
    myV1=aV1;
    myT1=aT1;
    myV2=aV2;
    myT2=aT2;
  }
  //
  void SetContext(const Handle(IntTools_Context)& aContext) {
    myContext=aContext;
  }
  //
  const Handle(IntTools_Context)& Context()const {
    return myContext;
  }
  //
  virtual void Perform() {
    Standard_Integer iErr;
    //
    iErr=1;
    if (!myEz.IsNull()) {
      TopoDS_Edge aSpz;
      //
      BOPTools_AlgoTools::MakeSplitEdge(myEz,myV1, myT1, 
                                        myV2, myT2, aSpz);
      //
      iErr=
        BOPTools_AlgoTools2D::AttachExistingPCurve(aSpz, 
                                                   myE, 
                                                   myF, 
                                                   myContext);
    }
    //
    if (iErr) { 
      BOPTools_AlgoTools2D::BuildPCurveForEdgeOnFace(myE, myF);
    }
    // 
    if (myFlag) {
      UpdateVertices(myE, myF);
    }
  }
  //
 protected:
  Standard_Boolean myFlag;
  TopoDS_Edge myE;
  TopoDS_Face myF;
  TopoDS_Edge myEz;
  TopoDS_Vertex myV1;
  Standard_Real myT1;
  TopoDS_Vertex myV2;
  Standard_Real myT2;
  //
  Handle(IntTools_Context) myContext;
};
//
//=======================================================================
typedef BOPCol_NCVector
  <BOPAlgo_MPC> BOPAlgo_VectorOfMPC; 
//
typedef BOPCol_ContextFunctor 
  <BOPAlgo_MPC,
  BOPAlgo_VectorOfMPC,
  Handle(IntTools_Context), 
  IntTools_Context> BOPAlgo_MPCFunctor;
//
typedef BOPCol_ContextCnt 
  <BOPAlgo_MPCFunctor,
  BOPAlgo_VectorOfMPC,
  Handle(IntTools_Context)> BOPAlgo_MPCCnt;
//
//=======================================================================
//class    : BOPAlgo_BPC
//purpose  : 
//=======================================================================
class BOPAlgo_BPC {
 public:
  BOPAlgo_BPC(){
  };
  //
  ~BOPAlgo_BPC(){
  };
  //
  void SetFace(const TopoDS_Face& aF) {
    myF=aF;
  }
  //
  void SetEdge(const TopoDS_Edge& aE) {
    myE=aE;
  }
  //
  void Perform() {
    BOPTools_AlgoTools2D::BuildPCurveForEdgeOnPlane (myE, myF);
  };
  //
 protected:
  TopoDS_Edge myE;
  TopoDS_Face myF;
};
//=======================================================================
typedef BOPCol_NCVector
  <BOPAlgo_BPC> BOPAlgo_VectorOfBPC; 
//
typedef BOPCol_Functor 
  <BOPAlgo_BPC,
  BOPAlgo_VectorOfBPC> BOPAlgo_BPCFunctor;
//
typedef BOPCol_Cnt 
  <BOPAlgo_BPCFunctor,
  BOPAlgo_VectorOfBPC> BOPAlgo_BPCCnt;
//
//
//=======================================================================
// function: MakeSplitEdges
// purpose: 
//=======================================================================
void BOPAlgo_PaveFiller::MakeSplitEdges()
{
  Standard_Integer aNbPBP;
  //
  myErrorStatus=0;
  //
  BOPDS_VectorOfListOfPaveBlock& aPBP=myDS->ChangePaveBlocksPool();
  aNbPBP=aPBP.Extent();
  if(!aNbPBP) {
    return;
  }
  //
  Standard_Boolean bCB, bV1, bV2;
  Standard_Integer i, nE, nV1, nV2, nSp, aNbPB, aNbVBSE, k;
  Standard_Real aT1, aT2, aTol, aTolV;
  BOPDS_ListIteratorOfListOfPaveBlock aItPB, aItPBCB;
  Handle(BOPDS_PaveBlock) aPB;
  BOPDS_MapOfPaveBlock aMPB(100);
  TopoDS_Vertex aV1, aV2;
  TopoDS_Edge aE;
  BOPAlgo_VectorOfSplitEdge aVBSE;
  //
  aTolV = Precision::Confusion();
  //
  for (i=0; i<aNbPBP; ++i) {
    BOPDS_ListOfPaveBlock& aLPB=aPBP(i);
    //
    aNbPB=aLPB.Extent();
    if (aNbPB==1) {
      aPB=aLPB.First();
      aPB->Indices(nV1, nV2);
      //
      bV1=myDS->IsNewShape(nV1);
      bV2=myDS->IsNewShape(nV2);
      //
      if (!(bV1 || bV2)) {
        if (!myDS->IsCommonBlock(aPB)) {
          nE=aPB->OriginalEdge();
          aPB->SetEdge(nE);
          continue;
        }
      }
    }
    //
    aItPB.Initialize(aLPB);
    for (; aItPB.More(); aItPB.Next()) {
      aPB=aItPB.Value();
      const Handle(BOPDS_CommonBlock)& aCB=myDS->CommonBlock(aPB);
      //
      bCB=!aCB.IsNull();
      if (bCB) {
        myDS->SortPaveBlocks(aCB);
        aPB=aCB->PaveBlock1();
        //
        aPB->Indices(nV1, nV2);
        myDS->MakeIntersectionVertex(nV1, aTolV);
        myDS->MakeIntersectionVertex(nV2, aTolV);
        myDS->UpdateCommonBlockWithSDVertices(aCB);
      }
      else {
        myDS->UpdatePaveBlockWithSDVertices(aPB);
      }
      //
      if (aMPB.Add(aPB)) {
        nE=aPB->OriginalEdge();
        aPB->Indices(nV1, nV2);
        aPB->Range(aT1, aT2);
        //
        aE=(*(TopoDS_Edge *)(&myDS->Shape(nE))); 
        aE.Orientation(TopAbs_FORWARD);
        //
        aV1=(*(TopoDS_Vertex *)(&myDS->Shape(nV1)));
        aV1.Orientation(TopAbs_FORWARD); 
        //
        aV2=(*(TopoDS_Vertex *)(&myDS->Shape(nV2)));
        aV2.Orientation(TopAbs_REVERSED); 
        //
        BOPAlgo_SplitEdge& aBSE=aVBSE.Append1();
        //
        aBSE.SetData(aE, aV1, aT1, aV2, aT2);
        aBSE.SetPaveBlock(aPB);
        if (bCB) {
          aTol = ComputeTolerance(aCB);
          aBSE.SetTolerance(aTol);
          aBSE.SetCommonBlock(aCB);
        }
        aBSE.SetProgressIndicator(myProgressIndicator);
      }
    } // for (; aItPB.More(); aItPB.Next()) {
  }  // for (i=0; i<aNbPBP; ++i) {      
  //
  aNbVBSE=aVBSE.Extent();
  //======================================================
  BOPAlgo_SplitEdgeCnt::Perform(myRunParallel, aVBSE);
  //======================================================
  //
  BOPDS_ShapeInfo aSI;
  //
  aSI.SetShapeType(TopAbs_EDGE);
  //
  for (k=0; k < aNbVBSE; ++k) {
    BOPAlgo_SplitEdge& aBSE=aVBSE(k);
    //
    const TopoDS_Edge& aSp=aBSE.SplitEdge();
    const Bnd_Box& aBox=aBSE.Box();
    //
    Handle(BOPDS_PaveBlock) aPBk=aBSE.PaveBlock();
    Handle(BOPDS_CommonBlock)& aCBk=aBSE.CommonBlock();
    //
    aSI.SetShape(aSp);
    aSI.ChangeBox()=aBox;
    //
    nSp=myDS->Append(aSI);
    //
    aTol = aBSE.Tolerance();
    myDS->UpdateEdgeTolerance(nSp, aTol);
    //
    if (!aCBk.IsNull()) {
      aCBk->SetEdge(nSp);
    }
    else {
      aPBk->SetEdge(nSp);
    }
  }
}
//=======================================================================
// function: SplitEdge
// purpose: 
//=======================================================================
Standard_Integer BOPAlgo_PaveFiller::SplitEdge(const Standard_Integer nE, 
                                               const Standard_Integer nV1,
                                               const Standard_Real aT1, 
                                               const Standard_Integer nV2, 
                                               const Standard_Real aT2)
{
  Standard_Integer nSp;
  TopoDS_Vertex aV1, aV2;
  TopoDS_Edge aE, aSp;
  BOPDS_ShapeInfo aSI;
  //
  aSI.SetShapeType(TopAbs_EDGE);
  //
  aE=(*(TopoDS_Edge *)(&myDS->Shape(nE))); 
  aE.Orientation(TopAbs_FORWARD);
  //
  aV1=(*(TopoDS_Vertex *)(&myDS->Shape(nV1)));
  aV1.Orientation(TopAbs_FORWARD); 
  //
  aV2=(*(TopoDS_Vertex *)(&myDS->Shape(nV2)));
  aV2.Orientation(TopAbs_REVERSED); 
  //
  BOPTools_AlgoTools::MakeSplitEdge(aE, aV1, aT1, aV2, aT2, aSp);  
  //
  aSI.SetShape(aSp);
  //
  Bnd_Box& aBox=aSI.ChangeBox();
  BRepBndLib::Add(aSp, aBox);
  //
  nSp=myDS->Append(aSI);
  return nSp;
}
//=======================================================================
// function: MakePCurves
// purpose: 
//=======================================================================
void BOPAlgo_PaveFiller::MakePCurves()
{
  Standard_Boolean bHasPC;
  Standard_Integer i, nF1, nF2, aNbC, k, nE, aNbFF, aNbFI, nEx;
  BOPDS_ListIteratorOfListOfPaveBlock aItLPB;
  BOPDS_MapIteratorOfMapOfPaveBlock aItMPB;
  TopoDS_Face aF1F, aF2F;
  BOPAlgo_VectorOfMPC aVMPC;
  //
  myErrorStatus=0;
  //
  // 1. Process Common Blocks 
  const BOPDS_VectorOfFaceInfo& aFIP=myDS->FaceInfoPool();
  //
  aNbFI=aFIP.Extent();
  for (i=0; i<aNbFI; ++i) {
    const BOPDS_FaceInfo& aFI=aFIP(i);
    nF1=aFI.Index();
    //
    aF1F=(*(TopoDS_Face *)(&myDS->Shape(nF1)));
    aF1F.Orientation(TopAbs_FORWARD);
    // In
    const BOPDS_IndexedMapOfPaveBlock& aMPBIn=aFI.PaveBlocksIn();
    aItMPB.Initialize(aMPBIn);
    for(; aItMPB.More(); aItMPB.Next()) {
      const Handle(BOPDS_PaveBlock)& aPB=aItMPB.Value();
      nE=aPB->Edge();
      const TopoDS_Edge& aE=(*(TopoDS_Edge *)(&myDS->Shape(nE)));
      //
      BOPAlgo_MPC& aMPC=aVMPC.Append1();
      aMPC.SetEdge(aE);
      aMPC.SetFace(aF1F);
      aMPC.SetProgressIndicator(myProgressIndicator);
    }
    //
    // On
    const BOPDS_IndexedMapOfPaveBlock& aMPBOn=aFI.PaveBlocksOn();
    aItMPB.Initialize(aMPBOn);
    for(; aItMPB.More(); aItMPB.Next()) {
      const Handle(BOPDS_PaveBlock)& aPB=aItMPB.Value();
      nE=aPB->Edge();
      const TopoDS_Edge& aE=(*(TopoDS_Edge *)(&myDS->Shape(nE)));
      bHasPC=BOPTools_AlgoTools2D::HasCurveOnSurface (aE, aF1F);
      if (bHasPC) {
        continue;
      }
      //
      Handle(BOPDS_CommonBlock) aCB=myDS->CommonBlock(aPB);
      if (aCB.IsNull()) {
        continue;
      }
      //
      const BOPDS_ListOfPaveBlock& aLPB=aCB->PaveBlocks();
      if (aLPB.Extent()<2) {
        continue;
      }
      //
      BOPAlgo_MPC& aMPC=aVMPC.Append1();
      //
      aItLPB.Initialize(aLPB);
      for(; aItLPB.More(); aItLPB.Next()) {
        const Handle(BOPDS_PaveBlock)& aPBx=aItLPB.Value();
        if (aPBx==aPB) {
          continue;
        }
        //
        nEx=aPBx->OriginalEdge();
        const TopoDS_Edge& aEx=(*(TopoDS_Edge *)(&myDS->Shape(nEx))); 
        bHasPC=BOPTools_AlgoTools2D::HasCurveOnSurface (aEx, aF1F);
        if (!bHasPC) {
          continue;
        }
        //
        Standard_Integer nV1x, nV2x;
        Standard_Real aT1x, aT2x;
        TopoDS_Vertex aV1x, aV2x;
        TopoDS_Edge aEz;
        //
        aEz=aEx;
        aEz.Orientation(TopAbs_FORWARD);
        //
        aPBx->Indices(nV1x, nV2x);
        aPBx->Range(aT1x, aT2x);
        //
        aV1x=(*(TopoDS_Vertex *)(&myDS->Shape(nV1x)));
        aV1x.Orientation(TopAbs_FORWARD); 
        //
        aV2x=(*(TopoDS_Vertex *)(&myDS->Shape(nV2x)));
        aV2x.Orientation(TopAbs_REVERSED); 
        //
        aMPC.SetData(aEz, aV1x, aT1x, aV2x, aT2x);
        //
        break;
      }
      //
      aMPC.SetEdge(aE);
      aMPC.SetFace(aF1F);
      aMPC.SetProgressIndicator(myProgressIndicator);
    }
  }// for (i=0; i<aNbFI; ++i) {
  //
  // 2. Process section edges
  Standard_Boolean bPCurveOnS[2];
  Standard_Integer m;
  TopoDS_Face aFf[2];
  //
  bPCurveOnS[0]=mySectionAttribute.PCurveOnS1();
  bPCurveOnS[1]=mySectionAttribute.PCurveOnS2();
  //
  if (bPCurveOnS[0] || bPCurveOnS[1]) {
    BOPDS_VectorOfInterfFF& aFFs=myDS->InterfFF();
    aNbFF=aFFs.Extent();
    for (i=0; i<aNbFF; ++i) {
      const BOPDS_InterfFF& aFF=aFFs(i);
      aFF.Indices(nF1, nF2);
      //
      aFf[0]=(*(TopoDS_Face *)(&myDS->Shape(nF1)));
      aFf[0].Orientation(TopAbs_FORWARD);
      //
      aFf[1]=(*(TopoDS_Face *)(&myDS->Shape(nF2)));
      aFf[1].Orientation(TopAbs_FORWARD);
      //
      const BOPDS_VectorOfCurve& aVNC=aFF.Curves();
      aNbC=aVNC.Extent();
      for (k=0; k<aNbC; ++k) {
        const BOPDS_Curve& aNC=aVNC(k);
        const BOPDS_ListOfPaveBlock& aLPB=aNC.PaveBlocks();
        aItLPB.Initialize(aLPB);
        for(; aItLPB.More(); aItLPB.Next()) {
          const Handle(BOPDS_PaveBlock)& aPB=aItLPB.Value();
          nE=aPB->Edge();
          const TopoDS_Edge& aE=(*(TopoDS_Edge *)(&myDS->Shape(nE))); 
          //
          for (m=0; m<2; ++m) {
            if (bPCurveOnS[m]) {
              BOPAlgo_MPC& aMPC=aVMPC.Append1();
              aMPC.SetEdge(aE);
              aMPC.SetFace(aFf[m]);
              aMPC.SetFlag(bPCurveOnS[m]);
              aMPC.SetProgressIndicator(myProgressIndicator);
            }
          }
        }
      }
    }// for (i=0; i<aNbFF; ++i) {
  }//if (bPCurveOnS1 || bPCurveOnS2 ) {
  //
  //======================================================
  BOPAlgo_MPCCnt::Perform(myRunParallel, aVMPC, myContext);
  //======================================================
}
//=======================================================================
// function: RefineFaceInfoOn
// purpose: 
//=======================================================================
void BOPAlgo_PaveFiller::RefineFaceInfoOn() 
{
  Standard_Integer aNbPBP;
  //
  myErrorStatus=0;
  //
  BOPDS_VectorOfListOfPaveBlock& aPBP=myDS->ChangePaveBlocksPool();
  aNbPBP=aPBP.Extent();
  if(!aNbPBP) {
    return;
  }
  //
  Standard_Boolean bV1, bV2;
  Standard_Integer i, nV1, nV2, aNbPB;
  Handle(BOPDS_PaveBlock) aPB;
  //
  for (i=0; i<aNbPBP; ++i) {
    BOPDS_ListOfPaveBlock& aLPB=aPBP(i);
    //
    aNbPB=aLPB.Extent();
    if (aNbPB==1) {
      aPB=aLPB.First();
      aPB->Indices(nV1, nV2);
      bV1=myDS->IsNewShape(nV1);
      bV2=myDS->IsNewShape(nV2);
      //
      if (!(bV1 || bV2)) {
        if (!myDS->IsCommonBlock(aPB)) {
          // the PB seems to be untouced
          aLPB.Clear();
          continue;
        }
      }//if (!(bV1 || bV2)) {
    }//if (aNbPB==1) {
  }//for (i=0; i<aNbPBP; ++i) {
  myDS->RefineFaceInfoOn();
}
//=======================================================================
//function : UpdateVertices
//purpose  : update tolerances of vertices comparing extremities of
//           3d and 2d curves
//=======================================================================
void UpdateVertices(const TopoDS_Edge& aE, 
                    const TopoDS_Face& aF)
{
  Standard_Integer j;
  Standard_Real aT[2], aUx, aVx, aTolV2, aD2, aD;
  gp_Pnt aP3D, aP3Dx;
  gp_Pnt2d aP2Dx;
  Handle(Geom_Surface) aS;
  Handle(Geom_Curve) aC3D;
  Handle(Geom2d_Curve) aC2D;
  TopoDS_Edge aEf;
  TopoDS_Vertex aV[2];
  BRep_Builder aBB;
  //
  aEf=aE;
  aEf.Orientation(TopAbs_FORWARD);
  //
  TopExp::Vertices(aEf, aV[0], aV[1]);
  //
  aS=BRep_Tool::Surface(aF);
  aC3D=BRep_Tool::Curve(aEf, aT[0], aT[1]);
  aC2D=BRep_Tool::CurveOnSurface(aEf, aF, aT[0], aT[1]);
  //
  for (j=0; j<2; ++j) {
    aTolV2=BRep_Tool::Tolerance(aV[j]);
    aTolV2=aTolV2*aTolV2;
    //
    aC3D->D0(aT[j], aP3D);
    aC2D->D0(aT[j], aP2Dx);
    aP2Dx.Coord(aUx, aVx);
    aS->D0(aUx, aVx, aP3Dx);
    aD2=aP3D.SquareDistance(aP3Dx);
    if (aD2>aTolV2) {
      aD=sqrt(aD2);
      aBB.UpdateVertex(aV[j], aD);
    }
  }
}
//=======================================================================
// function: Prepare
// purpose: 
//=======================================================================
void BOPAlgo_PaveFiller::Prepare()
{
  TopAbs_ShapeEnum aType[] = {
    TopAbs_VERTEX,
    TopAbs_EDGE,
    TopAbs_FACE
  };
  Standard_Boolean bJustAdd, bIsBasedOnPlane;
  Standard_Integer i, aNb, n1, nF;
  TopExp_Explorer aExp;
  BOPCol_MapOfShape aMF;
  BOPCol_MapIteratorOfMapOfShape aItMF;
  //
  myErrorStatus=0;
  //
  aNb=3;
  for(i=0; i<aNb; ++i) {
    myIterator->Initialize(aType[i], aType[2]);
    for (; myIterator->More(); myIterator->Next()) {
      myIterator->Value(n1, nF, bJustAdd);
      const TopoDS_Face& aF=(*(TopoDS_Face *)(&myDS->Shape(nF))); 
      //
      bIsBasedOnPlane=IsBasedOnPlane(aF);
      if (bIsBasedOnPlane) {
        aMF.Add(aF);
      }
    }
  }
  //
  if (aMF.IsEmpty()) {
    return;
  }
  //
  BOPAlgo_VectorOfBPC aVBPC;
  //
  aItMF.Initialize(aMF);
  for (; aItMF.More(); aItMF.Next()) {
    const TopoDS_Face& aF=*((TopoDS_Face *)&aItMF.Key());
    aExp.Init(aF, aType[1]);
    for (; aExp.More(); aExp.Next()) {
      const TopoDS_Edge& aE=*((TopoDS_Edge *)&aExp.Current());
      BOPAlgo_BPC& aBPC=aVBPC.Append1();
      aBPC.SetEdge(aE);
      aBPC.SetFace(aF);
    }
  }
  //
  //======================================================
  BOPAlgo_BPCCnt::Perform(myRunParallel, aVBPC);
  //======================================================
}
//=======================================================================
//function : IsBasedOnPlane
//purpose  : 
//=======================================================================
Standard_Boolean IsBasedOnPlane(const TopoDS_Face& aF)
{
  TopLoc_Location aLoc; 
  Handle(Geom_RectangularTrimmedSurface) aGRTS;
  Handle(Geom_Plane) aGP;
  
  const Handle(Geom_Surface)& aS = BRep_Tool::Surface(aF, aLoc);
  aGRTS = Handle(Geom_RectangularTrimmedSurface)::DownCast(aS);
  if(!aGRTS.IsNull()) {
    aGP = Handle(Geom_Plane)::DownCast(aGRTS->BasisSurface());
  }
  else {
    aGP = Handle(Geom_Plane)::DownCast(aS);
  }
  //
  return (!aGP.IsNull()); 
}

//=======================================================================
//function : ComputeTolerance
//purpose  : 
//=======================================================================
Standard_Real BOPAlgo_PaveFiller::ComputeTolerance(const Handle(BOPDS_CommonBlock)& theCB)
{
  Standard_Real aTolMax = 0.;
  if (theCB.IsNull()) {
    return aTolMax;
  }
  //
  const Handle(BOPDS_PaveBlock)& aPBR = theCB->PaveBlock1();
  Standard_Integer nE = aPBR->OriginalEdge();
  const TopoDS_Edge& aEOr = *(TopoDS_Edge*)&myDS->Shape(nE);
  aTolMax = BRep_Tool::Tolerance(aEOr);
  //
  const BOPDS_ListOfPaveBlock& aLPB = theCB->PaveBlocks();
  const BOPCol_ListOfInteger& aLFI = theCB->Faces();
  //
  if ((aLPB.Extent() < 2) && aLFI.IsEmpty()) {
    return aTolMax;
  }
  //
  const Standard_Integer aNbPnt = 11;
  Standard_Real aTol, aT, aT1, aT2, aDt;
  gp_Pnt aP;
  //
  const Handle(Geom_Curve)& aC3D = BRep_Tool::Curve(aEOr, aT1, aT2);
  //
  aPBR->Range(aT1, aT2);
  aDt = (aT2 - aT1) / aNbPnt;
  //
  // compute max tolerance for common blocks on edges
  if (aLPB.Extent() > 1) {
    // compute max distance between edges
    BOPDS_ListIteratorOfListOfPaveBlock aItPB;
    GeomAPI_ProjectPointOnCurve aProjPC;
    //
    aItPB.Initialize(aLPB);
    for (; aItPB.More(); aItPB.Next()) {
      const Handle(BOPDS_PaveBlock)& aPB = aItPB.Value();
      if (aPB == aPBR) {
        continue;
      }
      //
      nE = aPB->OriginalEdge();
      const TopoDS_Edge& aE = *(TopoDS_Edge*)&myDS->Shape(nE);
      //
      aTol = BRep_Tool::Tolerance(aE);
      if (aTol > aTolMax) {
        aTolMax = aTol;
      }
      //
      aProjPC = myContext->ProjPC(aE);
      //
      aT = aT1;
      while (aT <= aT2) {
        aC3D->D0(aT, aP);
        aProjPC.Perform(aP);
        if (aProjPC.NbPoints()) {
          aTol = aProjPC.LowerDistance();
          if (aTol > aTolMax) {
            aTolMax = aTol;
          }
        }
        aT += aDt;
      }
    }
  }
  //
  // compute max tolerance for common blocks on faces
  if (aLFI.Extent()) {
    Standard_Integer nF;
    GeomAPI_ProjectPointOnSurf aProjPS;
    BOPCol_ListIteratorOfListOfInteger aItLI;
    //
    aItLI.Initialize(aLFI);
    for (; aItLI.More(); aItLI.Next()) {
      nF = aItLI.Value();
      const TopoDS_Face& aF = *(TopoDS_Face*)&myDS->Shape(nF);
      //
      aTol = BRep_Tool::Tolerance(aF);
      if (aTol > aTolMax) {
        aTolMax = aTol;
      }
      //
      aProjPS = myContext->ProjPS(aF);
      //
      aT = aT1;
      while (aT <= aT2) {
        aC3D->D0(aT, aP);
        aProjPS.Perform(aP);
        if (aProjPS.NbPoints()) {
          aTol = aProjPS.LowerDistance();
          if (aTol > aTolMax) {
            aTolMax = aTol;
          }
        }
        aT += aDt;
      }
    }
  }
  //
  return aTolMax;
}
