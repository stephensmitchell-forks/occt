// Created on: 2001-03-13
// Created by: Peter KURNEV
// Copyright (c) 2001-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

#include <BOPTools_PaveFiller.ixx>

#include <TColStd_ListOfInteger.hxx>
#include <TColStd_ListIteratorOfListOfInteger.hxx>
#include <TColStd_IndexedMapOfInteger.hxx>

#include <gp_Pnt.hxx>
#include <Geom_Curve.hxx>

#include <TopoDS_Edge.hxx>
#include <BRep_Builder.hxx>

#include <BooleanOperations_ShapesDataStructure.hxx>

#include <IntTools_Curve.hxx>
#include <IntTools_Tools.hxx>

#include <BOPTools_Tools.hxx>
#include <BOPTools_SequenceOfCurves.hxx>
#include <BOPTools_Curve.hxx>
#include <BOPTools_ListOfPaveBlock.hxx>
#include <BOPTools_ListIteratorOfListOfPaveBlock.hxx>
#include <BOPTools_Pave.hxx>
#include <BOPTools_ListOfCommonBlock.hxx>
#include <BOPTools_ListIteratorOfListOfCommonBlock.hxx>
#include <BOPTools_CommonBlock.hxx>

//modified by NIZNHY-PKV Tue Aug 28 09:45:19 2012ft
//=======================================================================
//function : TreatTechnoVertices
//purpose  : 
//=======================================================================
void BOPTools_PaveFiller::TreatTechnoVertices(BOPTools_SSInterference& aFFi) 
{
  Standard_Boolean bFlag, bSplitted;
  Standard_Integer nF1, nF2, aBid, j, aNbCurves, aNbVT, aNbLPB, nE, nVT, iFlag;
  Standard_Integer nF, nE1, nE2, k;
  Standard_Real aT;
  TColStd_IndexedMapOfInteger aMVT;
  
  TColStd_ListIteratorOfListOfInteger aItLI;
  BOPTools_Pave aPave;
  BOPTools_ListOfPaveBlock aLPB, aLPBS;
  BOPTools_ListIteratorOfListOfPaveBlock aItLPB, aItLPB1;
  //
  nF1=aFFi.Index1();
  nF2=aFFi.Index2();
  BOPTools_SequenceOfCurves& aBCs=aFFi.Curves();
  //
  aNbCurves=aBCs.Length();
  for (j=1; j<=aNbCurves; j++) {
    BOPTools_Curve& aBC=aBCs(j);
    //Handle(Geom_Curve) aC3D= aBC.Curve().Curve(); //DEB
    TColStd_ListOfInteger& aLVT=aBC.TechnoVertices();
    aItLI.Initialize(aLVT);
    for (; aItLI.More(); aItLI.Next()) {
      nVT=aItLI.Value();
      aMVT.Add(nVT);
    }
  }  
  //
  aNbVT=aMVT.Extent();
  if (!aNbVT) {// nothing to do
    return; 
  }
  //------------------------------
  //
  aBid=0;
  SplitsInFace (aBid, nF1, nF2, aLPB);
  SplitsInFace (aBid, nF2, nF1, aLPB);
  SplitsOnFace (aBid, nF1, nF2, aLPB);
  //
  aNbLPB=aLPB.Extent();
  if (!aNbLPB) {// nothing to do 
    return; 
  }
  //
  for (k=1; k<=aNbVT; ++k) {
    nVT=aMVT(k);
    const TopoDS_Vertex& aV=*((TopoDS_Vertex*)&(myDS->Shape(nVT)));
    //
    // 1.Collect Pave Blocks (pretenders) to split by nVT => aLPBS
    aLPBS.Clear();
    aItLPB.Initialize(aLPB);
    for (; aItLPB.More(); aItLPB.Next()) {
      const BOPTools_PaveBlock& aPB=aItLPB.Value();
      nE=aPB.OriginalEdge();
      const TopoDS_Edge& aE=*((TopoDS_Edge*)&(myDS->Shape(nE)));
      //
      iFlag=myContext->ComputeVE(aV, aE, aT);
      if (iFlag) {
	continue;
      }
      // 
      aPave.SetIndex(nVT);
      aPave.SetParam(aT);
      //
      BOPTools_ListOfPaveBlock& aLPBE=mySplitShapesPool(myDS->RefEdge(nE)); 
      aItLPB1.Initialize(aLPBE);
      for (; aItLPB1.More(); aItLPB1.Next()) {
	const BOPTools_PaveBlock& aPBE=aItLPB1.Value();
	if (aPBE.IsInBlock(aPave)) {
	  aLPBS.Append(aPBE);
	}
      }
    }//for (; aItLPB.More(); aItLPB.Next()) {
    //
    // 2.Split Pave Blocks [aLPBS] by the vertex [nVT]
    BOPTools_ListIteratorOfListOfCommonBlock aItCB1, aItCB2;
    BOPTools_PaveBlock aPB11, aPB12, aPB21, aPB22, aPB1, aPB2;
    //
    aItLPB.Initialize(aLPBS);
    for (; aItLPB.More(); aItLPB.Next()) { 
      const BOPTools_PaveBlock& aPBS=aItLPB.Value();
      nE=aPBS.OriginalEdge();
      //
      BOPTools_ListOfCommonBlock& aLCB1=myCommonBlockPool(myDS->RefEdge(nE));
      aItCB1.Initialize(aLCB1);
      for (; aItCB1.More(); aItCB1.Next()) {
	BOPTools_CommonBlock& aCB1=aItCB1.Value();
	aPB1=aCB1.PaveBlock1(nE);
	nE1=aPB1.OriginalEdge();
	//
	if (aPB1.IsEqual(aPBS)) {
	  //
	  SplitPaveBlock(aPB1, nVT, aPB11, aPB12);
	  //
	  nF=aCB1.Face();
	  if (nF) {
	    BOPTools_CommonBlock aCBa, aCBb;
	    //
	    aLCB1.Remove(aItCB1);
	    //
	    aCBa.SetPaveBlock1(aPB11);
	    aCBa.SetFace(nF);
	    aCBb.SetPaveBlock2(aPB12);
	    aCBb.SetFace(nF);
	    //
	    aLCB1.Append(aCBa);
	    aLCB1.Append(aCBb);
	    //
	    break; 
	  }// if (nF) { 
	  else { // 1
	    BOPTools_CommonBlock aCBa, aCBb;
	    //
	    aPB2=aCB1.PaveBlock2(nE);
	    nE2=aPB2.OriginalEdge();
	    //
	    SplitPaveBlock(aPB2, nVT, aPB21, aPB22);
	    //
	    //aLCB1.Remove(aItCB1);
	    //
	    iFlag=0;
	    BOPTools_ListOfCommonBlock& aLCB2=myCommonBlockPool(myDS->RefEdge(nE2));
	    aItCB2.Initialize(aLCB2);
	    for (; aItCB2.More(); aItCB2.Next()) {
	      BOPTools_CommonBlock& aCB2=aItCB2.Value();
	      BOPTools_PaveBlock& aPBx=aCB2.PaveBlock1(nE2);
	      if (aPBx.IsEqual(aPB2)) {
		aLCB2.Remove(aItCB2);
		aLCB1.Remove(aItCB1);
		//
		bFlag=CheckCoincidence(aPB11, aPB21);
		if (bFlag) {
		  aCBa.SetPaveBlock1(aPB11);
		  aCBa.SetPaveBlock2(aPB21);
		  aCBb.SetPaveBlock1(aPB12);
		  aCBb.SetPaveBlock2(aPB22);
		}
		else {
		  aCBa.SetPaveBlock1(aPB11);
		  aCBa.SetPaveBlock2(aPB22);
		  aCBb.SetPaveBlock1(aPB12);
		  aCBb.SetPaveBlock2(aPB21);
		}
		//
		aLCB1.Append(aCBa);
		aLCB1.Append(aCBb);
		aLCB2.Append(aCBa);
		aLCB2.Append(aCBb);
		//
		iFlag=1;
		break;
	      }//if (aPB1.IsEqual(aPB2)) {
	    }//for (; aItCB2.More(); aItCB2.Next()) {
	  }//else { // 1
	}//if (aPB1.IsEqual(aPBS)) {
	if (iFlag) {
	  break;
	}
      }//for (; aItCB.More(); aItCB.Next()) {
    }//for (; aItLPB.More(); aItLPB.Next()) {
  }//for (k=1; k<=aNbVT; ++k) {
}
//=======================================================================
//function : SplitPaveBlock
//purpose  : 
//=======================================================================
void BOPTools_PaveFiller::SplitPaveBlock(const BOPTools_PaveBlock& aPBE,
					 const Standard_Integer nVT,
					 BOPTools_PaveBlock& aPB1,
					 BOPTools_PaveBlock& aPB2)
{
  Standard_Integer nE, iFlag;
  Standard_Real aT;
  BOPTools_Pave aPave;
  BOPTools_ListIteratorOfListOfPaveBlock aItLPB;
  //
  const TopoDS_Vertex& aVT=*((TopoDS_Vertex*)&(myDS->Shape(nVT)));
  //
  nE=aPBE.OriginalEdge();
  const TopoDS_Edge& aE=*((TopoDS_Edge*)&(myDS->Shape(nE)));
  //
  BOPTools_ListOfPaveBlock& aLPB=mySplitShapesPool(myDS->RefEdge(nE)); 
  //
  aItLPB.Initialize(aLPB);
  for (; aItLPB.More(); aItLPB.Next()) {
    const BOPTools_PaveBlock& aPB=aItLPB.Value();
    if (aPB.IsEqual(aPBE)) {
      aLPB.Remove(aItLPB);
      break;
    }
  }
  //
  iFlag=myContext->ComputeVE(aVT, aE, aT);
  //
  aPave.SetIndex(nVT);
  aPave.SetParam(aT);
  //
  aPB1.SetOriginalEdge(nE);
  aPB1.SetPave1(aPBE.Pave1());
  aPB1.SetPave2(aPave);
  //
  aPB2.SetOriginalEdge(nE);
  aPB2.SetPave1(aPave);
  aPB2.SetPave2(aPBE.Pave2());
  //
  aLPB.Append(aPB1);
  aLPB.Append(aPB2);
}

//=======================================================================
//function : CheckCoincidence
//purpose  : 
//=======================================================================
Standard_Boolean 
  BOPTools_PaveFiller::CheckCoincidence(const BOPTools_PaveBlock& aPB1,
					const BOPTools_PaveBlock& aPB2)
					
{
  Standard_Boolean bRet;
  Standard_Integer nV11, nV12, nV21, nV22, nE1, nE2, iFlag;
  Standard_Real aT11, aT12, aT21, aT22, aTM, aTN, aTolE1;
  gp_Pnt aPM;
  TopoDS_Vertex aVM;
  BRep_Builder aBB;
  //
  const BOPTools_Pave& aPave11=aPB1.Pave1();
  nV11=aPave11.Index();
  aT11=aPave11.Param();
  // 
  const BOPTools_Pave& aPave12=aPB1.Pave2();
  aT12=aPave12.Param();
  nV12=aPave12.Index();
  //
  const BOPTools_Pave& aPave21=aPB2.Pave1();
  nV21=aPave21.Index();
  aT21=aPave21.Param();
  //
  const BOPTools_Pave& aPave22=aPB2.Pave2();
  nV22=aPave22.Index();
  aT22=aPave22.Param();
  //
  bRet=((nV11==nV21 && nV12==nV22) || (nV11==nV22 && nV12==nV21));
  if (!bRet) {
    return bRet;
  }
  //
  nE1=aPB1.OriginalEdge();
  const TopoDS_Edge& aE1=*((TopoDS_Edge*)&(myDS->Shape(nE1)));
  aTolE1=BRep_Tool::Tolerance(aE1);
  //
  nE2=aPB2.OriginalEdge();
  const TopoDS_Edge& aE2=*((TopoDS_Edge*)&(myDS->Shape(nE2)));
  // 
  aTM=IntTools_Tools::IntermediatePoint (aT11, aT12);
  BOPTools_Tools::PointOnEdge(aE1, aTM, aPM);
  //
  aBB.MakeVertex (aVM, aPM, aTolE1);
  //
  iFlag=myContext->ComputeVE(aVM, aE2, aTN); 
  if (iFlag) {
    bRet=!bRet; //False
  }
  //
  if (aT21 <aT22) {
    bRet=(aTN>aT21 && aTN<aT22);
  }
  else {
    bRet=(aTN>aT22 && aTN<aT21);
  }
  return bRet;
} 
