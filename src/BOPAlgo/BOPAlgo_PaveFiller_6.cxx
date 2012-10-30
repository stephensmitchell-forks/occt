// Created by: Peter KURNEV
// Copyright (c) 2010-2012 OPEN CASCADE SAS
// Copyright (c) 2007-2010 CEA/DEN, EDF R&D, OPEN CASCADE
// Copyright (c) 2003-2007 OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN, CEDRAT,
//                         EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
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


#include <BOPAlgo_PaveFiller.ixx>

#include <Precision.hxx>
#include <NCollection_IncAllocator.hxx>
#include <Bnd_Box.hxx>

#include <Geom_Curve.hxx>
#include <Geom2d_Curve.hxx>

#include <GeomAPI_ProjectPointOnCurve.hxx>
#include <GeomAPI_ProjectPointOnSurf.hxx>

#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Compound.hxx>

#include <TopExp_Explorer.hxx>

#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>

#include <BRepBndLib.hxx>
#include <BRepTools.hxx>

#include <BRepAdaptor_Curve.hxx>
#include <BRepAdaptor_Surface.hxx>

#include <IntTools_FaceFace.hxx>
#include <IntTools_SequenceOfCurves.hxx>
#include <IntTools_SequenceOfPntOn2Faces.hxx>
#include <IntTools_Curve.hxx>
#include <IntTools_PntOn2Faces.hxx>
#include <IntTools_Tools.hxx>

#include <IntSurf_ListOfPntOn2S.hxx>
#include <IntSurf_PntOn2S.hxx>

#include <BOPTools_AlgoTools.hxx>
#include <BOPTools_AlgoTools3D.hxx>

#include <BOPCol_MapOfInteger.hxx>
#include <BOPCol_ListOfShape.hxx>
#include <BOPCol_DataMapOfShapeInteger.hxx>
#include <BOPCol_ListOfInteger.hxx>

#include <BOPInt_Context.hxx>
#include <BOPInt_Tools.hxx>

#include <BOPDS_Interf.hxx>
#include <BOPDS_Iterator.hxx>
#include <BOPDS_Curve.hxx>
#include <BOPDS_Point.hxx>
#include <BOPDS_FaceInfo.hxx>
#include <BOPDS_Curve.hxx>
#include <BOPDS_MapOfPaveBlock.hxx>
#include <BOPDS_PaveBlock.hxx>
#include <BOPDS_VectorOfCurve.hxx>
#include <BOPDS_VectorOfPoint.hxx>
#include <BOPDS_ShapeInfo.hxx>
#include <BOPDS_PaveBlock.hxx>
#include <BOPDS_ListOfPave.hxx>
#include <BOPDS_ListOfPaveBlock.hxx>
#include <BOPDS_CoupleOfPaveBlocks.hxx>
#include <BOPDS_FaceInfo.hxx>
#include <BOPDS_CommonBlock.hxx>

#include <BOPAlgo_Tools.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <TopExp.hxx>
#include <BOPInt_ShrunkRange.hxx>


static void FindPaveBlocksByFirst(BOPDS_ListOfPaveBlock& aLPBC,
                                  BOPDS_ListOfPaveBlock& aLPB);

static Standard_Boolean IsMicroEdge(const TopoDS_Edge& aE,
                                    const Handle(BOPInt_Context)& aCtx);

//=======================================================================
//function : PerformFF
//purpose  : 
//=======================================================================
  void BOPAlgo_PaveFiller::PerformFF()
{
  Standard_Integer iSize;
  Standard_Boolean bValid;
  //
  myErrorStatus=0;
  //
  myIterator->Initialize(TopAbs_FACE, TopAbs_FACE);
  iSize=myIterator->ExpectedLength();
  if (!iSize) {
    return; 
  }
  //
  Standard_Boolean bJustAdd, bApp, bCompC2D1, bCompC2D2, bIsDone;
  Standard_Boolean bToSplit;
  Standard_Integer nF1, nF2, aNbCurves, aNbPoints, iX, i, iP, iC, aNbLP;
  Standard_Real aApproxTol, aTolR3D, aTolR2D;
  //
  BOPDS_VectorOfInterfFF& aFFs=myDS->InterfFF();
  aFFs.SetStartSize(iSize);
  aFFs.SetIncrement(iSize);
  aFFs.Init();
  //
  bApp=mySectionAttribute.Approximation();
  bCompC2D1=mySectionAttribute.PCurveOnS1();
  bCompC2D2=mySectionAttribute.PCurveOnS2();
  aApproxTol=1.e-7;
  bToSplit = Standard_False;
  //
  for (; myIterator->More(); myIterator->Next()) {
    myIterator->Value(nF1, nF2, bJustAdd);
    if(bJustAdd) {
      continue;
    }
    //
    const TopoDS_Face& aF1=(*(TopoDS_Face *)(&myDS->Shape(nF1)));
    const TopoDS_Face& aF2=(*(TopoDS_Face *)(&myDS->Shape(nF2)));
    //
    //modified by NIZHNY-EMV Thu Jan 20 16:15:59 2011
    IntTools_FaceFace aFaceFace;
    //
    IntSurf_ListOfPntOn2S aListOfPnts;
    GetListOfEFPnts(nF1, nF2, aListOfPnts);
    aNbLP = aListOfPnts.Extent();
    if (aNbLP) {
      aFaceFace.SetList(aListOfPnts);
    }
    //modified by NIZHNY-EMV Thu Jan 20 16:16:06 2011

    aFaceFace.SetParameters(bApp, bCompC2D1, bCompC2D2, aApproxTol);
    //
    aFaceFace.Perform(aF1, aF2);
    //
    bIsDone=aFaceFace.IsDone();
    if (bIsDone) {
      aTolR3D=aFaceFace.TolReached3d();
      aTolR2D=aFaceFace.TolReached2d();
      if (aTolR3D < 1.e-7){
        aTolR3D=1.e-7;
      }
      if (aTolR2D < 1.e-7){
        aTolR2D=1.e-7;
      }
      //
      aFaceFace.PrepareLines3D(bToSplit);
      //
      const IntTools_SequenceOfCurves& aCvsX=aFaceFace.Lines();
      const IntTools_SequenceOfPntOn2Faces& aPntsX=aFaceFace.Points();
      //
      aNbCurves=aCvsX.Length();
      aNbPoints=aPntsX.Length();
      //
      myDS->AddInterf(nF1, nF2);
      //
      iX=aFFs.Append()-1;
      BOPDS_InterfFF& aFF=aFFs(iX);
      aFF.SetIndices(nF1, nF2);
      //
      aFF.SetTolR3D(aTolR3D);
      aFF.SetTolR2D(aTolR2D);
      //
      // Curves, Points 
      aFF.Init(aNbCurves, aNbPoints);
      //
      // Curves
      BOPDS_VectorOfCurve& aVNC=aFF.ChangeCurves();
      for (i=1; i<=aNbCurves; ++i) {
        Bnd_Box aBox;
        //
        const IntTools_Curve& aIC=aCvsX(i);
        const Handle(Geom_Curve)& aC3D= aIC.Curve();
        bValid=BOPInt_Tools::CheckCurve(aC3D, aTolR3D, aBox);
        if (bValid) {
          iC=aVNC.Append()-1;
          BOPDS_Curve& aNC=aVNC(iC);
          aNC.SetCurve(aIC);
          aNC.SetBox(aBox);
        }
      }
      //
      // Points
      BOPDS_VectorOfPoint& aVNP=aFF.ChangePoints();
      for (i=1; i<=aNbPoints; ++i) {
        const IntTools_PntOn2Faces& aPi=aPntsX(i);
        const gp_Pnt& aP=aPi.P1().Pnt();
        //
        iP=aVNP.Append()-1;
        BOPDS_Point& aNP=aVNP(iP);
        aNP.SetPnt(aP);
      }
    //}// if (aNbCs || aNbPs)
    }// if (bIsDone) {
    else {// 904/L1
      iX=aFFs.Append()-1;
      BOPDS_InterfFF& aFF=aFFs(iX);
      aFF.SetIndices(nF1, nF2);
      aNbCurves=0;
      aNbPoints=0;
      aFF.Init(aNbCurves, aNbPoints);
    }
  }// for (; myIterator->More(); myIterator->Next()) {
}
//modified by NIZHNY-EMV Thu Mar 31 14:39:52 2011
//=======================================================================
//function : MakeBlocks
//purpose  : 
//=======================================================================

  void BOPAlgo_PaveFiller::MakeBlocks()
{
  Standard_Integer aNbFF;
  //
  myErrorStatus=0;
  //
  BOPDS_VectorOfInterfFF& aFFs=myDS->InterfFF();
  aNbFF=aFFs.Extent();
  if (!aNbFF) {
    return;
  }
  //
  Standard_Boolean bExist, bValid2D;
  Standard_Integer i, nF1, nF2, aNbC, aNbP, j, aSC;
  Standard_Integer nV1, nV2, iFlag;
  Standard_Real aTolR3D, aTolR2D, aT1, aT2;
  Handle(NCollection_IncAllocator) aAllocator;
  BOPDS_ListIteratorOfListOfPaveBlock aItLPB;
  TopoDS_Edge aES;
  //
  //-----------------------------------------------------scope f
  aAllocator=new NCollection_IncAllocator();
  //
  BOPCol_ListOfInteger aLSE(aAllocator);
  BOPCol_MapOfInteger aMVOnIn(100, aAllocator), aMF(100, aAllocator),
                      aMVStick(100,aAllocator);
  BOPDS_MapOfPaveBlock aMPBOnIn(100, aAllocator),
                       aMPBAdd(100, aAllocator);
  BOPDS_ListOfPaveBlock aLPB(aAllocator);
  BOPDS_DataMapOfShapeCoupleOfPaveBlocks aMSCPB(100, aAllocator); 
  BOPCol_DataMapOfShapeInteger aMVI(100, aAllocator);
  //modified by NIZHNY-EMV Tue Sep 27 08:26:45 2011
  BOPCol_MapOfInteger aMVB;
  BOPCol_DataMapOfIntegerListOfInteger aMInterfs;
  BOPCol_ListOfInteger aLIEF;
  iFlag = 1 + 2 + 4 + 8 + 16; // 2^0 + 2^1 + 2^2 + 2^3 + 2^4
  //modified by NIZHNY-EMV Tue Sep 27 08:26:46 2011
  //
  for (i=0; i<aNbFF; ++i) {
    BOPDS_InterfFF& aFF=aFFs(i);
    aFF.Indices(nF1, nF2);
    //
    BOPDS_VectorOfPoint& aVP=aFF.ChangePoints();
    aNbP=aVP.Extent();
    BOPDS_VectorOfCurve& aVC=aFF.ChangeCurves();
    aNbC=aVC.Extent();
    if (!aNbP && !aNbC) {
      continue;
    }
    //
    const TopoDS_Face& aF1=(*(TopoDS_Face *)(&myDS->Shape(nF1)));
    const TopoDS_Face& aF2=(*(TopoDS_Face *)(&myDS->Shape(nF2)));
    //
    aTolR3D=aFF.TolR3D();
    aTolR2D=aFF.TolR2D();
    //
    // Update face info
    if (aMF.Add(nF1)) {
      myDS->UpdateFaceInfoOn(nF1);
    }
    if (aMF.Add(nF2)) {
      myDS->UpdateFaceInfoOn(nF2);
    }
   
    BOPDS_FaceInfo& aFI1=myDS->ChangeFaceInfo(nF1);
    BOPDS_FaceInfo& aFI2=myDS->ChangeFaceInfo(nF2);
    //
    aMVOnIn.Clear();
    aMPBOnIn.Clear();
    aMVB.Clear();
    //
    myDS->VerticesOnIn(nF1, nF2, aMVOnIn, aMPBOnIn);
    myDS->SharedEdges(nF1, nF2, aLSE, aAllocator);
    
    // 1. Treat Points
    for (j=0; j<aNbP; ++j) {
      TopoDS_Vertex aV;
      BOPDS_CoupleOfPaveBlocks aCPB;
      //
      BOPDS_Point& aNP=aVP.ChangeValue(j);
      const gp_Pnt& aP=aNP.Pnt();
      //
      bExist=IsExistingVertex(aP, aTolR3D, aMVOnIn);
      if (!bExist) {
        BOPTools_AlgoTools::MakeNewVertex(aP, aTolR3D, aV);
        //
        aCPB.SetIndexInterf(i);
        aCPB.SetIndex(j);
        aMSCPB.Bind(aV, aCPB);
      }
    }

    //modified by NIZHNY-EMV Tue Sep 27 08:26:16 2011
    // 2. Treat Curves
    aMVStick.Clear();
    aMInterfs.Clear();
    GetInterfs(nF1, nF2, aMVStick, aMInterfs, iFlag);
    //modified by NIZHNY-EMV Tue Sep 27 08:26:18 2011
    //modified by NIZHNY-EMV Fri Sep 23 11:54:59 2011
    for (j=0; j<aNbC; ++j) {
      BOPDS_Curve& aNC=aVC.ChangeValue(j);
      const IntTools_Curve& aIC=aNC.Curve();
      // DEBf
      const Handle(Geom_Curve)& aC3D=aIC.Curve();
      // DEBt
      aNC.InitPaveBlock1();
      //
      //modified by NIZHNY-EMV Tue Sep 27 09:10:52 2011
      PutPaveOnCurve(aMVOnIn, aTolR3D, aNC, nF1, nF2, aMVB);
      //modified by NIZHNY-EMV Fri Feb 18 16:42:19 2011
      ProcessUnUsedVertices(nF1, nF2, aNC, aMVStick);
      //pkv/904/F7
      if (aNbC == 1) {
        aLIEF = aMInterfs(4);
        ProcessUnUsedVertices(nF1, nF2, aNC, aLIEF);
      }
      //modified by NIZHNY-EMV Tue Sep 27 09:10:59 2011
      //
      if (aIC.HasBounds()) {
        //modified by NIZHNY-EMV Wed Mar 16 14:11:36 2011
        PutBoundPaveOnCurve(aF1, aF2, aTolR3D, aNC, aMVOnIn, aMVB);
        //modified by NIZHNY-EMV Wed Mar 16 12:53:21 2011
      }
    }//for (j=0; j<aNbC; ++j) {
    //

    //modified by NIZHNY-EMV Wed Feb 15 10:08:58 2012
    // Put closing pave if needded
    for (j=0; j<aNbC; ++j) {
      BOPDS_Curve& aNC=aVC.ChangeValue(j);
      PutClosingPaveOnCurve (aNC);
    }
    //modified by NIZHNY-EMV Wed Feb 15 10:09:02 2012
    aSC = 0;
    // 3. Make section edges
    for (j=0; j<aNbC; ++j) {
      BOPDS_Curve& aNC=aVC.ChangeValue(j);
      const IntTools_Curve& aIC=aNC.Curve();
      //
      BOPDS_ListOfPaveBlock& aLPBC=aNC.ChangePaveBlocks();
      Handle(BOPDS_PaveBlock)& aPB1=aNC.ChangePaveBlock1();
      //
      aLPB.Clear();
      aPB1->Update(aLPB, Standard_False);
      //
      aItLPB.Initialize(aLPB);
      for (; aItLPB.More(); aItLPB.Next()) {
        Handle(BOPDS_PaveBlock)& aPB=aItLPB.ChangeValue();
        aPB->Indices(nV1, nV2);
        aPB->Range  (aT1, aT2);
        //modified by NIZHNY-EMV Mon Apr 09 11:36:30 2012
        if (fabs(aT1 - aT2) < Precision::PConfusion()) {
          continue;
        }
        //modified by NIZHNY-EMV Mon Apr 09 11:36:31 2012
        bExist=IsExistingPaveBlock(aPB, aNC, aTolR3D, aMPBOnIn);
        if (bExist) {
          aSC++;
          continue;
        }
        //
        bExist=IsExistingPaveBlock(aPB, aNC, aTolR3D, aLSE);
        if (bExist) {
          aSC++;
          continue;
        }
        //
        bValid2D=myContext->IsValidBlockForFaces(aT1, aT2, aIC, aF1, aF2, aTolR3D);
        if (!bValid2D) {
          continue;
        }
        //
        // Make Edge
        const TopoDS_Vertex& aV1=(*(TopoDS_Vertex *)(&myDS->Shape(nV1)));
        const TopoDS_Vertex& aV2=(*(TopoDS_Vertex *)(&myDS->Shape(nV2)));
        //
        BOPTools_AlgoTools::MakeEdge (aIC, aV1, aT1, aV2, aT2, aTolR3D, aES);
        if (mySectionAttribute.PCurveOnS1() && mySectionAttribute.PCurveOnS2()) {
          BOPTools_AlgoTools::MakePCurve(aES, aF1, aF2, aIC);
        }
        //
        if (IsMicroEdge(aES, myContext)) {
          continue;
        }
        //
        // Append the Pave Block to the Curve j
        aLPBC.Append(aPB);
        //
        // Keep info for post treatment 
        BOPDS_CoupleOfPaveBlocks aCPB;
        aCPB.SetIndexInterf(i);
        aCPB.SetIndex(j);
        aCPB.SetPaveBlock1(aPB);
        //
        aMSCPB.Bind(aES, aCPB);
        aMVI.Bind(aV1, nV1);
        aMVI.Bind(aV2, nV2);
        aSC++;
      }
      //
      aLPBC.RemoveFirst();
    }//for (j=0; j<aNbC; ++j) {
    //modified by NIZHNY-EMV Fri Dec 23 14:43:26 2011
    if (aSC) {
      ProcessExistingPaveBlocks(i, aMPBOnIn, aMVB, aMPBAdd, aMSCPB, aMVI);
    }
    //modified by NIZHNY-EMV Fri Dec 23 14:43:27 2011
  }//for (i=0; i<aNbFF; ++i) {
  // 
  // post treatment
  myErrorStatus=PostTreatFF(aMSCPB, aMVI, aAllocator);
  if (myErrorStatus) {
    return;
  }
  //
  // update face info
  for (i=0; i<aNbFF; ++i) {
    BOPDS_InterfFF& aFF=aFFs(i);
    aFF.Indices(nF1, nF2);
    //
    BOPDS_FaceInfo& aFI1=myDS->ChangeFaceInfo(nF1);
    BOPDS_FaceInfo& aFI2=myDS->ChangeFaceInfo(nF2);
    // PaveBlocksSc
    BOPDS_VectorOfCurve& aVNC=aFF.ChangeCurves();
    aNbC=aVNC.Extent();
    for (j=0; j<aNbC; ++j) {
      BOPDS_Curve& aNC=aVNC(j);
      BOPDS_ListOfPaveBlock& aLPBC=aNC.ChangePaveBlocks();
      //modified by NIZHNY-EMV Tue Dec 13 09:04:41 2011
      UpdateExistingPaveBlocks(aLPBC, nF1, nF2);
      //modified by NIZHNY-EMV Tue Dec 13 09:04:44 2011
      aItLPB.Initialize(aLPBC);
      for(; aItLPB.More(); aItLPB.Next()) {
        const Handle(BOPDS_PaveBlock)& aPB=aItLPB.Value();
        aFI1.ChangePaveBlocksSc().Add(aPB);
        aFI2.ChangePaveBlocksSc().Add(aPB);
      }
    }
    // VerticesSc
    const BOPDS_VectorOfPoint& aVNP=aFF.Points();
    aNbP=aVNP.Extent();
    for (j=0; j<aNbP; ++j) {
      const BOPDS_Point& aNP=aVNP(j);
      nV1=aNP.Index();
      aFI1.ChangeVerticesSc().Add(nV1);
      aFI2.ChangeVerticesSc().Add(nV1);
    }
  }
  //-----------------------------------------------------scope t
  aMF.Clear();
  aMVStick.Clear();
  aMPBOnIn.Clear();
  aMVOnIn.Clear();
  aAllocator.Nullify();
}

//=======================================================================
//function : PostTreatFF
//purpose  : 
//=======================================================================
  Standard_Integer BOPAlgo_PaveFiller::PostTreatFF
    (BOPDS_DataMapOfShapeCoupleOfPaveBlocks& theMSCPB,
     BOPCol_DataMapOfShapeInteger& aMVI,
     Handle(NCollection_BaseAllocator)& theAllocator)
{
  Standard_Integer iRet, aNbS;
  //
  iRet=0;
  aNbS=theMSCPB.Extent();
  if (!aNbS) {
    return iRet;
  }
  //
  Standard_Boolean bHasPaveBlocks, bOld;
  Standard_Integer iErr, nSx, nVSD, iX, iP, iC, j, nV, iV, iE;
  //modified by NIZHNY-EMV Tue Dec 13 15:47:08 2011
  Standard_Integer jx;
  Standard_Real aT;
  //modified by NIZHNY-EMV Tue Dec 13 15:47:14 2011
  Standard_Integer aNbLPBx;
  TopAbs_ShapeEnum aType;
  TopoDS_Shape aV, aE;
  BOPCol_ListIteratorOfListOfShape aItLS;
  BOPDS_DataMapIteratorOfDataMapOfShapeCoupleOfPaveBlocks aIt;
  BOPDS_ListIteratorOfListOfPaveBlock aItLPB;
  BOPDS_PDS aPDS;
  Handle(BOPDS_PaveBlock) aPB1;
  BOPDS_Pave aPave[2], aPave1[2];
  BOPDS_ShapeInfo aSI;
  //
  BOPCol_ListOfShape aLS(theAllocator);
  BOPAlgo_PaveFiller aPF(theAllocator);
  //
  BOPDS_VectorOfInterfFF& aFFs=myDS->InterfFF();
  //
  // <-DEB f
  //
  // 0
  if (aNbS==1) {
    aIt.Initialize(theMSCPB);
    for (; aIt.More(); aIt.Next()) {
      const TopoDS_Shape& aS=aIt.Key();
      const BOPDS_CoupleOfPaveBlocks &aCPB=aIt.Value();
      
      //
      aType=aS.ShapeType();
      if (aType==TopAbs_VERTEX) {
        aSI.SetShapeType(aType);
        aSI.SetShape(aS);
        iV=myDS->Append(aSI);
        //
        iX=aCPB.IndexInterf();
        iP=aCPB.Index();
        BOPDS_InterfFF& aFF=aFFs(iX); 
        BOPDS_VectorOfPoint& aVNP=aFF.ChangePoints();
        BOPDS_Point& aNP=aVNP(iP);
        aNP.SetIndex(iV);
      }
      else if (aType==TopAbs_EDGE) {
        //modified by NIZHNY-EMV Thu Dec 15 08:32:11 2011
        aPB1=aCPB.PaveBlock1();
        //
        if (aPB1->HasEdge()) {
          BOPDS_InterfFF& aFF=aFFs(aCPB.IndexInterf());
          BOPDS_VectorOfCurve& aVNC=aFF.ChangeCurves();
          BOPDS_Curve& aNC=aVNC(aCPB.Index());
          BOPDS_ListOfPaveBlock& aLPBC=aNC.ChangePaveBlocks();
          aLPBC.Clear();
        } else {
          aSI.SetShapeType(aType);
          aSI.SetShape(aS);
          iE=myDS->Append(aSI);
          //
          aPB1->SetEdge(iE);
        }
        //modified by NIZHNY-EMV Thu Dec 15 08:32:13 2011
      }
      return iRet;
    }
  }
  //
  // 1 prepare arguments
  aIt.Initialize(theMSCPB);
  for (; aIt.More(); aIt.Next()) {
    const TopoDS_Shape& aS=aIt.Key();
    aLS.Append(aS);
  }
  //
  // 2 Fuse shapes
  aPF.SetArguments(aLS);
  aPF.Perform();
  iErr=aPF.ErrorStatus();
  if (iErr) {
    iRet=1;
    return iRet;
  }
  aPDS=aPF.PDS();
  //
  aItLS.Initialize(aLS);
  for (; aItLS.More(); aItLS.Next()) {
    const TopoDS_Shape& aSx=aItLS.Value();
    nSx=aPDS->Index(aSx);
    const BOPDS_ShapeInfo& aSIx=aPDS->ShapeInfo(nSx);
    //
    aType=aSIx.ShapeType();
    //
    if (aType==TopAbs_VERTEX) {
      if (aPDS->HasShapeSD(nSx, nVSD)) {
        aV=aPDS->Shape(nVSD);
      }
      else {
        aV=aSx;
      }
      // index of new vertex in theDS -> iV
      if (!aMVI.IsBound(aV)) {
        aSI.SetShapeType(aType);
        aSI.SetShape(aV);
        iV=myDS->Append(aSI);
        //
        aMVI.Bind(aV, iV);
      }
      else {
        iV=aMVI.Find(aV);
      }
      // update FF interference
      const BOPDS_CoupleOfPaveBlocks &aCPB=theMSCPB.Find(aSx);
      iX=aCPB.IndexInterf();
      iP=aCPB.Index();
      BOPDS_InterfFF& aFF=aFFs(iX);
      BOPDS_VectorOfPoint& aVNP=aFF.ChangePoints();
      BOPDS_Point& aNP=aVNP(iP);
      aNP.SetIndex(iV);
    }//if (aType==TopAbs_VERTEX) {
    //
    else if (aType==TopAbs_EDGE) {
      bHasPaveBlocks=aPDS->HasPaveBlocks(nSx);
      if (!bHasPaveBlocks) {
        const BOPDS_CoupleOfPaveBlocks &aCPB=theMSCPB.Find(aSx);
        aPB1=aCPB.PaveBlock1();
        //
        if (aPB1->HasEdge()) {
          BOPDS_InterfFF& aFF=aFFs(aCPB.IndexInterf());
          BOPDS_VectorOfCurve& aVNC=aFF.ChangeCurves();
          BOPDS_Curve& aNC=aVNC(aCPB.Index());
          BOPDS_ListOfPaveBlock& aLPBC=aNC.ChangePaveBlocks();
          aItLPB.Initialize(aLPBC);
          for (; aItLPB.More(); aItLPB.Next()) {
            const Handle(BOPDS_PaveBlock)& aPBC=aItLPB.Value();
            if (aPBC==aPB1) {
              aLPBC.Remove(aItLPB);
              break;
            }
          } 
        } else {
          aSI.SetShapeType(aType);
          aSI.SetShape(aSx);
          iE=myDS->Append(aSI);
          //
          aPB1->SetEdge(iE);
        }
      }
      else {
        const BOPDS_CoupleOfPaveBlocks &aCPB=theMSCPB.Find(aSx);
        iX=aCPB.IndexInterf();
        iC=aCPB.Index();
        aPB1=aCPB.PaveBlock1();
        //modified by NIZHNY-EMV Tue Dec 13 15:49:29 2011
        bOld = Standard_False;
        if (aPB1->HasEdge()) {
          bOld = Standard_True;
        }
        //modified by NIZHNY-EMV Tue Dec 13 15:49:31 2011
        //
        BOPDS_InterfFF& aFF=aFFs(iX);
        BOPDS_VectorOfCurve& aVNC=aFF.ChangeCurves();
        BOPDS_Curve& aNC=aVNC(iC);
        BOPDS_ListOfPaveBlock& aLPBC=aNC.ChangePaveBlocks();
        //
        const BOPDS_ListOfPaveBlock& aLPBx=aPDS->PaveBlocks(nSx);
        aNbLPBx=aLPBx.Extent();
        //
        if (!bOld || !aNbLPBx) {
          aItLPB.Initialize(aLPBC);
          for (; aItLPB.More(); aItLPB.Next()) {
            const Handle(BOPDS_PaveBlock)& aPBC=aItLPB.Value();
            if (aPBC==aPB1) {
              aLPBC.Remove(aItLPB);
              break;
            }
          } 
        }
        //
        if (!aNbLPBx) {
          //modified by NIZHNY-EMV Mon Dec 12 15:08:01 2011
          if (bOld) {
            continue;
          }
          //modified by NIZHNY-EMV Mon Dec 12 15:08:03 2011
          aE=aSx;
          //
          if (!aMVI.IsBound(aE)) {
            aSI.SetShapeType(aType);
            aSI.SetShape(aE);
            iE=myDS->Append(aSI);
            aMVI.Bind(aE, iE);
          }
          else {
            iE=aMVI.Find(aE);
          }
          // append new PaveBlock to aLPBC
          Handle(BOPDS_PaveBlock) aPBC=new BOPDS_PaveBlock();
          //
          aPB1->SetEdge(iE);
          aLPBC.Append(aPB1);
        } // if (!aNbLPBx) {
        //
        else {
          aItLPB.Initialize(aLPBx);
          //modified by NIZHNY-EMV Mon Dec 12 15:08:01 2011
          if (bOld) {
            aPave1[0] = aPB1->Pave1();
            aPave1[1] = aPB1->Pave2();
          }
          //modified by NIZHNY-EMV Tue Dec 13 16:00:05 2011
          for (; aItLPB.More(); aItLPB.Next()) {
            const Handle(BOPDS_PaveBlock)& aPBx=aItLPB.Value();
            const Handle(BOPDS_PaveBlock)& aPBRx=aPBx->RealPaveBlock();
            //
            // update vertices of paves
            aPave[0]=aPBx->Pave1();
            aPave[1]=aPBx->Pave2();
            for (j=0; j<2; ++j) {
              //modified by NIZHNY-EMV Tue Dec 13 15:53:55 2011
              jx = 0;
              if (bOld) {
                aT = aPave[j].Parameter();
                if (aT == aPave1[0].Parameter()) {
                  jx = 1;
                } else if (aT == aPave1[1].Parameter()) {
                  jx = 2;
                }
                //
                if (jx) {
                  iV = aPave1[jx-1].Index();
                }
              } 
              if (!jx) {
                //modified by NIZHNY-EMV Tue Dec 13 15:55:35 2011
                nV=aPave[j].Index();
                aV=aPDS->Shape(nV);
                //
                if (!aMVI.IsBound(aV)) {// index of new vertex in theDS -> iV
                  aSI.SetShapeType(TopAbs_VERTEX);
                  aSI.SetShape(aV);
                  iV=myDS->Append(aSI);
                  aMVI.Bind(aV, iV);
                }
                else {
                  iV=aMVI.Find(aV);
                }
              }
              aPave[j].SetIndex(iV);
            }
            //
            // add edge
            aE=aPDS->Shape(aPBRx->Edge());
            //
            if (!aMVI.IsBound(aE)) {
              aSI.SetShapeType(aType);
              aSI.SetShape(aE);
              iE=myDS->Append(aSI);
              aMVI.Bind(aE, iE);
            }
            else {
              iE=aMVI.Find(aE);
            }
            // append new PaveBlock to aLPBC
            Handle(BOPDS_PaveBlock) aPBC=new BOPDS_PaveBlock();
            //
            aPBC->SetPave1(aPave[0]);
            aPBC->SetPave2(aPave[1]);
            aPBC->SetEdge(iE);
            if (bOld) {
              aPBC->SetOriginalEdge(aPB1->OriginalEdge());
            }
            //
            aLPBC.Append(aPBC);
          }
        }
      }
    }//else if (aType==TopAbs_EDGE)
  }//for (; aItLS.More(); aItLS.Next()) {
  return iRet;
}
//=======================================================================
//function : IsExistingVertex
//purpose  : 
//=======================================================================
  Standard_Boolean BOPAlgo_PaveFiller::IsExistingVertex
    (const gp_Pnt& aP,
     const Standard_Real theTolR3D,
     const BOPCol_MapOfInteger& aMVOnIn)const
{
  Standard_Boolean bRet;
  Standard_Integer nV, iFlag;
  Standard_Real aTolV;
  gp_Pnt aPV;
  Bnd_Box aBoxP;
  BOPCol_MapIteratorOfMapOfInteger aIt;
  //
  bRet=Standard_True;
  //
  aBoxP.Add(aP);
  aBoxP.Enlarge(theTolR3D);
  //
  aIt.Initialize(aMVOnIn);
  for (; aIt.More(); aIt.Next()) {
    Bnd_Box aBoxV;
    //
    nV=aIt.Value();
    const TopoDS_Vertex& aV=(*(TopoDS_Vertex *)(&myDS->Shape(nV)));
    aPV=BRep_Tool::Pnt(aV);
    aTolV=BRep_Tool::Tolerance(aV);
    aBoxV.Add(aP);
    aBoxV.Enlarge(aTolV);
    //
    if (!aBoxP.IsOut(aBoxV)) {
      iFlag=BOPTools_AlgoTools::ComputeVV(aV, aP, theTolR3D);
      if (!iFlag) {
        return bRet;
      }
    }
  }
  return !bRet;
}
//=======================================================================
//function : IsExistingPaveBlock
//purpose  : 
//=======================================================================
  Standard_Boolean BOPAlgo_PaveFiller::IsExistingPaveBlock
    (const Handle(BOPDS_PaveBlock)& thePB,
     const BOPDS_Curve& theNC,
     const Standard_Real theTolR3D,
     const BOPCol_ListOfInteger& theLSE)
{
  Standard_Boolean bRet=Standard_True;
  //
  if (theLSE.IsEmpty()) {
    return !bRet;
  } 
  //
  Standard_Real aT1, aT2, aTm, aTx, aTol;
  Standard_Integer nE, iFlag;
  gp_Pnt aPm;
  Bnd_Box aBoxPm;
  BOPCol_ListIteratorOfListOfInteger aItLI;
  //
  thePB->Range(aT1, aT2);
  aTm=IntTools_Tools::IntermediatePoint (aT1, aT2);
  theNC.Curve().D0(aTm, aPm);
  aBoxPm.Add(aPm);
  aBoxPm.Enlarge(theTolR3D);
  //
  aItLI.Initialize(theLSE);
  for (; aItLI.More(); aItLI.Next()) {
    nE=aItLI.Value();
    const BOPDS_ShapeInfo& aSIE=myDS->ChangeShapeInfo(nE);
    const Bnd_Box& aBoxE=aSIE.Box();
    if (!aBoxE.IsOut(aBoxPm)) {
      const TopoDS_Edge& aE=(*(TopoDS_Edge *)(&aSIE.Shape()));
      //modified by NIZHNY-EMV Tue Dec 27 14:21:07 2011
      aTol = BRep_Tool::Tolerance(aE);
      aTol = aTol > theTolR3D ? aTol : theTolR3D;
      iFlag=myContext->ComputePE(aPm, aTol, aE, aTx);
      //modified by NIZHNY-EMV Tue Dec 27 14:21:08 2011
      if (!iFlag) {
        return bRet;
      }
    }
  }
  return !bRet;
}

//modified by NIZHNY-EMV Tue Jul 26 12:47:52 2011
//=======================================================================
//function : IsExistingPaveBlock
//purpose  : 
//=======================================================================
  Standard_Boolean BOPAlgo_PaveFiller::IsExistingPaveBlock
    (const Handle(BOPDS_PaveBlock)& thePB,
     const BOPDS_Curve& theNC,
     const Standard_Real theTolR3D,
     const BOPDS_MapOfPaveBlock& theMPBOnIn)
{
  Standard_Boolean bRet;
  Standard_Real aT1, aT2, aTm, aTx;
  Standard_Integer nSp, iFlag, nV11, nV12, nV21, nV22;
  gp_Pnt aP1, aPm, aP2;
  Bnd_Box aBoxP1, aBoxPm, aBoxP2;
  BOPDS_MapIteratorOfMapOfPaveBlock aIt;
  //
  bRet=Standard_True;
  const IntTools_Curve& aIC=theNC.Curve();
  //
  thePB->Range(aT1, aT2);
  thePB->Indices(nV11, nV12);
  //first point
  aIC.D0(aT1, aP1);
  aBoxP1.Add(aP1);
  aBoxP1.Enlarge(theTolR3D);
  //intermediate point
  aTm=IntTools_Tools::IntermediatePoint (aT1, aT2);
  aIC.D0(aTm, aPm);
  aBoxPm.Add(aPm);
  aBoxPm.Enlarge(theTolR3D);
  //last point
  aIC.D0(aT2, aP2);
  aBoxP2.Add(aP2);
  aBoxP2.Enlarge(theTolR3D);
  //
  aIt.Initialize(theMPBOnIn);
  for (; aIt.More(); aIt.Next()) {
    const Handle(BOPDS_PaveBlock)& aPB=aIt.Value();
    aPB->Indices(nV21, nV22);
    nSp=aPB->Edge();
    const BOPDS_ShapeInfo& aSISp=myDS->ChangeShapeInfo(nSp);
    const Bnd_Box& aBoxSp=aSISp.Box();
    if (!aBoxSp.IsOut(aBoxP1) && !aBoxSp.IsOut(aBoxPm) && !aBoxSp.IsOut(aBoxP2)) {
      const TopoDS_Edge& aSp=(*(TopoDS_Edge *)(&aSISp.Shape()));
      iFlag=(nV11 == nV21 || nV11 == nV22) ? 0 : 
                     myContext->ComputePE(aP1, theTolR3D, aSp, aTx);
      if (!iFlag) {
        iFlag=(nV12 == nV21 || nV12 == nV22) ? 0 :
                       myContext->ComputePE(aP2, theTolR3D, aSp, aTx);
        if (!iFlag) {
          iFlag=myContext->ComputePE(aPm, theTolR3D, aSp, aTx);
          if (!iFlag) {
            return bRet;
          }
        }
      }
    }
  }
  return !bRet;
}
//modified by NIZHNY-EMV Tue Jul 26 12:47:54 2011

//=======================================================================
//function : PutBoundPaveOnCurve
//purpose  : 
//=======================================================================
  void BOPAlgo_PaveFiller::PutBoundPaveOnCurve(const TopoDS_Face& aF1,
                                               const TopoDS_Face& aF2,
                                               const Standard_Real aTolR3D,
                                               BOPDS_Curve& aNC,
                                               BOPCol_MapOfInteger& aMVOnIn,
                                               BOPCol_MapOfInteger& aMVB)
{
  Standard_Boolean bVF;
  Standard_Integer nV, iFlag, nVn, j, aNbEP;
  Standard_Real aT[2], aTmin, aTmax, aTV, aTol, aTolVnew;
  gp_Pnt aP[2];
  TopoDS_Vertex aVn;
  BOPDS_ListIteratorOfListOfPave aItLP;
  BOPDS_Pave aPn, aPMM[2];
  //
  aTolVnew = Precision::Confusion();
  //
  const IntTools_Curve& aIC=aNC.Curve();
  aIC.Bounds(aT[0], aT[1], aP[0], aP[1]);
  //
  Handle(BOPDS_PaveBlock)& aPB=aNC.ChangePaveBlock1();
  const BOPDS_ListOfPave& aLP=aPB->ExtPaves();
  //
  aNbEP=aLP.Extent();
  if (aNbEP) {
    aTmin=1.e10;
    aTmax=-aTmin;
    //
    aItLP.Initialize(aLP);
    for (; aItLP.More(); aItLP.Next()) {
      const BOPDS_Pave& aPv=aItLP.Value();
      aPv.Contents(nV, aTV);
      if (aTV<aTmin) {
        aPMM[0]=aPv;
        aTmin=aTV;
      }
      if (aTV>aTmax) {
        aPMM[1]=aPv;
        aTmax=aTV;
      }
    }
  }
  //
  for (j=0; j<2; ++j) {
    //if curve is closed, process only one bound
    if (j && aP[1].IsEqual(aP[0], aTolVnew)) {
      continue;
    }
    //
    iFlag=1;
    //
    if (aNbEP) {
      Bnd_Box aBoxP;
      //
      aBoxP.Set(aP[j]);
      //modified by NIZHNY-EMV Tue Apr 26 15:09:15 2011
      aTol = aTolR3D+Precision::Confusion();
      aBoxP.Enlarge(aTol);
      //modified by NIZHNY-EMV Tue Apr 26 15:09:17 2011
      const BOPDS_Pave& aPV=aPMM[j];
      nV=aPV.Index();
      const BOPDS_ShapeInfo& aSIV=myDS->ShapeInfo(nV);
      const TopoDS_Vertex& aV=(*(TopoDS_Vertex *)(&aSIV.Shape()));
      const Bnd_Box& aBoxV=aSIV.Box();
      if (!aBoxP.IsOut(aBoxV)){
        iFlag=BOPTools_AlgoTools::ComputeVV(aV, aP[j], aTol);
      }
    }
    if (iFlag) {
      // 900/L5
      //modified by NIZNHY-PKV Wed Jul  8 13:16:18 2009f 
      bVF=myContext->IsValidPointForFaces (aP[j], aF1, aF2, aTolR3D);
      if (!bVF) {
        continue;
      }
      //modified by NIZNHY-PKV Wed Jul  8 13:16:22 2009t
      //
      BOPDS_ShapeInfo aSIVn;
      //
      BOPTools_AlgoTools::MakeNewVertex(aP[j], aTolR3D, aVn);
      aSIVn.SetShapeType(TopAbs_VERTEX);
      aSIVn.SetShape(aVn);
      //
      nVn=myDS->Append(aSIVn);
      //
      aPn.SetIndex(nVn);
      aPn.SetParameter(aT[j]);
      aPB->AppendExtPave(aPn);
      //
      aVn=(*(TopoDS_Vertex *)(&myDS->Shape(nVn)));
      BOPTools_AlgoTools::UpdateVertex (aIC, aT[j], aVn);
      //
      aTolVnew = BRep_Tool::Tolerance(aVn);
      //
      BOPDS_ShapeInfo& aSIDS=myDS->ChangeShapeInfo(nVn);
      Bnd_Box& aBoxDS=aSIDS.ChangeBox();
      BRepBndLib::Add(aVn, aBoxDS);
      //modified by NIZHNY-EMV Wed Mar 16 13:46:01 2011
      aMVOnIn.Add(nVn);
      aMVB.Add(nVn);
      //modified by NIZHNY-EMV Tue Apr 12 10:01:51 2011
    }
  }
}

//=======================================================================
//function : PutPaveOnCurve
//purpose  : 
//=======================================================================
  void BOPAlgo_PaveFiller::PutPaveOnCurve(const BOPCol_MapOfInteger& aMVOnIn,
                                          const Standard_Real aTolR3D,
                                          BOPDS_Curve& aNC,
                                          const Standard_Integer nF1,
                                          const Standard_Integer nF2,
                                          BOPCol_MapOfInteger& aMVB)
{
  Standard_Boolean bIsVertexOnLine, bInBothFaces;
  Standard_Integer nV;
  Standard_Real aT;
  BOPDS_Pave aPave;
  //
  BOPCol_MapIteratorOfMapOfInteger aIt;
  //
  Handle(BOPDS_PaveBlock)& aPB=aNC.ChangePaveBlock1();
  const IntTools_Curve& aIC=aNC.Curve();
  const Bnd_Box& aBoxC=aNC.Box();
  //
  aIt.Initialize(aMVOnIn);
  for (; aIt.More(); aIt.Next()) {
    nV=aIt.Value();
    const BOPDS_ShapeInfo& aSIV=myDS->ShapeInfo(nV);
    const TopoDS_Vertex& aV=(*(TopoDS_Vertex *)(&aSIV.Shape()));
    const Bnd_Box& aBoxV=aSIV.Box();

    if (aBoxC.IsOut(aBoxV)){
      continue; 
    }
    //modified by NIZHNY-EMV Wed Mar 16 15:38:53 2011
    if (!myDS->IsNewShape(nV)) {
      const BOPDS_FaceInfo& aFI1 = myDS->FaceInfo(nF1);
      const BOPDS_FaceInfo& aFI2 = myDS->FaceInfo(nF2);
      //
      bInBothFaces = (aFI1.VerticesOn().Contains(nV) ||
                      aFI1.VerticesIn().Contains(nV))&&
                     (aFI2.VerticesOn().Contains(nV) ||
                      aFI2.VerticesIn().Contains(nV));
      if (!bInBothFaces) {
        continue;
      }
    }
    //modified by NIZHNY-EMV Fri Sep 23 13:20:38 2011
    bIsVertexOnLine=myContext->IsVertexOnLine(aV, aIC, aTolR3D, aT);
    if (!bIsVertexOnLine) {
      Standard_Real aTolVExt;
      BOPCol_MapOfInteger aMI;
      //
      aTolVExt = BRep_Tool::Tolerance(aV);
      //
      GetFullFaceMap(nF1, aMI);
      GetFullFaceMap(nF2, aMI);
      //
      ExtendedTolerance(nV, aMI, aTolVExt);
      bIsVertexOnLine=myContext->IsVertexOnLine(aV, aTolVExt, aIC, aTolR3D, aT);
    }   
    //
    if (bIsVertexOnLine) {
      aPave.SetIndex(nV);
      aPave.SetParameter(aT);
      //
      aPB->AppendExtPave(aPave);
      //
      BOPTools_AlgoTools::UpdateVertex (aIC, aT, aV);
      // 
      BOPDS_ShapeInfo& aSIDS=myDS->ChangeShapeInfo(nV);
      Bnd_Box& aBoxDS=aSIDS.ChangeBox();
      BRepBndLib::Add(aV, aBoxDS);
      //
      aMVB.Add(nV);
    }
    //modified by NIZHNY-EMV Fri Sep 23 13:35:49 2011
  }
}

//=======================================================================
//function : ExtendedTolerance
//purpose  : 
//=======================================================================
  Standard_Boolean BOPAlgo_PaveFiller::ExtendedTolerance(const Standard_Integer nV,
                                                         const BOPCol_MapOfInteger& aMI,
                                                         Standard_Real& aTolVExt)
{
  Standard_Boolean bFound = Standard_False;
  if (!(myDS->IsNewShape(nV))) {
    return bFound;
  }

  Standard_Integer i, k, aNbLines;
  Standard_Real aT11, aT12, aD1, aD2, aD;
  TopoDS_Vertex aV;
  gp_Pnt aPV, aP11, aP12;

  aV = (*(TopoDS_Vertex *)(&myDS->Shape(nV)));
  aPV=BRep_Tool::Pnt(aV);
  //
  BOPDS_VectorOfInterfEE& aEEs=myDS->InterfEE();
  BOPDS_VectorOfInterfEF& aEFs=myDS->InterfEF();

  for (k=0; k<2; ++k) {
    aNbLines = !k ? aEEs.Extent() : aEFs.Extent();
    for (i = 0; i < aNbLines; ++i) {
      BOPDS_Interf *aInt = !k ? (BOPDS_Interf*) (&aEEs(i)) :
                                (BOPDS_Interf*) (&aEFs(i));
      if (aInt->IndexNew() == nV) {
        if (aMI.Contains(aInt->Index1()) && aMI.Contains(aInt->Index2())) {
          const IntTools_CommonPrt& aComPrt = !k ? aEEs(i).CommonPart() :
                                                   aEFs(i).CommonPart();
          //
          const TopoDS_Edge& aE1=aComPrt.Edge1();
          aComPrt.Range1(aT11, aT12);
          BOPTools_AlgoTools::PointOnEdge(aE1, aT11, aP11);
          BOPTools_AlgoTools::PointOnEdge(aE1, aT12, aP12);
          aD1=aPV.Distance(aP11);
          aD2=aPV.Distance(aP12);
          aD=(aD1>aD2)? aD1 : aD2;
          if (aD>aTolVExt) {
            aTolVExt=aD;
          }
          return !bFound;
        }//if (aMI.Contains(aEF.Index1()) && aMI.Contains(aEF.Index2())) {
      }//if (aInt->IndexNew() == nV) {
    }//for (i = 0; i < aNbLines; ++i) {
  }//for (k=0; k<2; ++k) {

  return bFound;
}

//=======================================================================
//function : GetListOfEFPnts
//purpose  : 
//=======================================================================
  void BOPAlgo_PaveFiller::GetListOfEFPnts(const Standard_Integer nF1,
                                           const Standard_Integer nF2,
                                           IntSurf_ListOfPntOn2S& aListOfPnts)
{
  Standard_Real U1, U2, V1, V2, f, l;
  BOPCol_ListOfInteger aLIEF;
  BOPCol_ListIteratorOfListOfInteger anItLI;
  BOPDS_VectorOfInterfEF& aEFs=myDS->InterfEF();

  BOPCol_MapOfInteger aMVStick, aMVToCheck;
  BOPCol_DataMapOfIntegerListOfInteger aMInterfs;
  BOPDS_ListIteratorOfListOfPave aItLP;
  Standard_Integer iFlag;
  iFlag = 16; //EF only
  GetInterfs(nF1, nF2, aMVStick, aMInterfs, iFlag);
  if (!aMVStick.Extent()) {
    return;
  }

  aLIEF = aMInterfs(4);
  if (!aLIEF.Extent()) {
    return;
  }

  anItLI.Initialize(aLIEF);
  for (; anItLI.More(); anItLI.Next()) {
    Standard_Integer interf = anItLI.Value();
    const BOPDS_InterfEF& aEF = aEFs(interf);
    Standard_Integer nE = aEF.Index1();
    Standard_Integer nFOpposite = aEF.Index2();
    IntTools_CommonPrt aCP = aEF.CommonPart();
    Standard_Real aPar = aCP.VertexParameter1();

    const TopoDS_Edge& aE = (*(TopoDS_Edge*)(&myDS->Shape(nE)));
    const TopoDS_Face& aFOpposite = (*(TopoDS_Face*)(&myDS->Shape(nFOpposite)));
    
    Handle(Geom_Curve) aCurve = BRep_Tool::Curve(aE, f, l);
    //
    Standard_Integer nF = (nFOpposite == nF1) ? nF2 : nF1;
    const TopoDS_Face& aF = (*(TopoDS_Face*)(&myDS->Shape(nF)));
    Handle(Geom2d_Curve) aPCurve = BRep_Tool::CurveOnSurface(aE, aF, f, l);
    //
    GeomAPI_ProjectPointOnSurf& aProj = myContext->ProjPS(aFOpposite);
    //
    gp_Pnt aPoint;
    aCurve->D0(aPar, aPoint);
    IntSurf_PntOn2S aPnt;
    if(!aPCurve.IsNull()) {
      gp_Pnt2d aP2d = aPCurve->Value(aPar);
      aProj.Perform(aPoint);
      if(aProj.IsDone()) {
        aProj.LowerDistanceParameters(U1,V1);
        if (nF == nF1) {
          aPnt.SetValue(aP2d.X(),aP2d.Y(),U1,V1);
        } else {
          aPnt.SetValue(U1,V1,aP2d.X(),aP2d.Y());
        }
        aListOfPnts.Append(aPnt);
      }
    }
    else {
      GeomAPI_ProjectPointOnSurf& aProj1 = myContext->ProjPS(aF);
      aProj1.Perform(aPoint);
      aProj.Perform(aPoint);
      if(aProj1.IsDone() && aProj.IsDone()){
        aProj1.LowerDistanceParameters(U1,V1);
        aProj.LowerDistanceParameters(U2,V2);
        if (nF == nF1) {
          aPnt.SetValue(U1,V1,U2,V2);
        } else {
          aPnt.SetValue(U2,V2,U1,V1);
        }
        aListOfPnts.Append(aPnt);
      }
    }
  }
}

//modified by NIZHNY-EMV Thu Jun 16 15:09:46 2011
//=======================================================================
//function : ProcessUnUsedVertices
//purpose  : 
//=======================================================================
  void BOPAlgo_PaveFiller::ProcessUnUsedVertices(const Standard_Integer nF1,
                                                 const Standard_Integer nF2,
                                                 BOPDS_Curve& aNC,
                                                 const BOPCol_ListOfInteger& aLIEF)
{
  if (!aLIEF.Extent()) {
    return;
  }
  //
  const IntTools_Curve& aIC=aNC.Curve();
  GeomAbs_CurveType aTypeC;
  aTypeC=aIC.Type();
  if (!(aTypeC==GeomAbs_BezierCurve || aTypeC==GeomAbs_BSplineCurve)) {
    return;
  }
  //
  Standard_Integer nV, interf;
  BOPCol_MapOfInteger aMV;
  BOPCol_ListIteratorOfListOfInteger aItLI;
  //
  BOPDS_VectorOfInterfEF& aEFs=myDS->InterfEF();
  aItLI.Initialize(aLIEF);
  for (; aItLI.More(); aItLI.Next()) {
    interf = aItLI.Value();
    const BOPDS_InterfEF& aEF = aEFs(interf);
    nV = aEF.IndexNew();
    aMV.Add(nV);
  }
  //
  RemoveUsedVertices(aNC, aMV);
  if (!aMV.Extent()) {
    return;
  }
  //
  Standard_Real aDist;
  BOPDS_Pave aPave;
  
  const Handle(Geom_Curve)& aC3D=aIC.Curve();
  Handle(BOPDS_PaveBlock)& aPB = aNC.ChangePaveBlock1();

  GeomAPI_ProjectPointOnCurve& aProjPT = myContext->ProjPT(aC3D);
  
  BOPCol_MapIteratorOfMapOfInteger aItMI;
  aItMI.Initialize(aMV);
  for (; aItMI.More(); aItMI.Next()) {
    nV = aItMI.Value();
    const TopoDS_Vertex& aV = (*(TopoDS_Vertex *)(&myDS->Shape(nV)));
    gp_Pnt aPV = BRep_Tool::Pnt(aV);
    aProjPT.Perform(aPV);
    Standard_Integer aNbPoints = aProjPT.NbPoints();
    if (aNbPoints) {
      aDist = aProjPT.LowerDistance();
      PutPaveOnCurve(nV, aDist, aNC, aPB);
    }
  }
}
//modified by NIZHNY-EMV Thu Jun 16 15:09:47 2011

//modified by NIZHNY-EMV Fri Sep 23 11:24:21 2011
//=======================================================================
//function : ProcessUnUsedVertices
//purpose  : 
//=======================================================================
  void BOPAlgo_PaveFiller::ProcessUnUsedVertices(const Standard_Integer nF1,
                                                 const Standard_Integer nF2,
                                                 BOPDS_Curve& aNC,
                                                 BOPCol_MapOfInteger& aMVStick)
{
  RemoveUsedVertices(aNC, aMVStick);
  //
  if (!aMVStick.Extent()) {
    return;
  }
  //
  GeomAbs_SurfaceType aType1, aType2;
  const TopoDS_Face& aF1 = (*(TopoDS_Face*)(&myDS->Shape(nF1)));  
  const TopoDS_Face& aF2 = (*(TopoDS_Face*)(&myDS->Shape(nF2)));  
  Handle(Geom_Surface) aS1=BRep_Tool::Surface(aF1);
  Handle(Geom_Surface) aS2=BRep_Tool::Surface(aF2);
  GeomAdaptor_Surface aGAS1(aS1);
  GeomAdaptor_Surface aGAS2(aS2);
  
  //
  aType1=aGAS1.GetType();
  aType2=aGAS2.GetType();
  //
  if(aType1==GeomAbs_Torus  || aType2==GeomAbs_Torus) {
    Standard_Integer nV, m, n;
    Standard_Real aTC[2], aD, aD2, u, v, aDT2, aScPr, aDScPr;
    GeomAbs_CurveType aTypeC;
    gp_Pnt aPC[2], aPV;
    gp_Dir aDN[2];
    gp_Pnt2d aP2D;
    
    Handle(Geom2d_Curve) aC2D[2];
    //
    aDT2=2e-7;     // the rich criteria
    aDScPr=5.e-9;  // the creasing criteria
    //
    const IntTools_Curve& aIC=aNC.Curve();
    //Handle(Geom_Curve) aC3D=aIC.Curve(); //DEB
    aTypeC=aIC.Type();
    if (aTypeC==GeomAbs_BezierCurve || aTypeC==GeomAbs_BSplineCurve) {
      //
      aIC.Bounds(aTC[0], aTC[1], aPC[0], aPC[1]);
      aC2D[0]=aIC.FirstCurve2d();
      aC2D[1]=aIC.SecondCurve2d();
      if (!aC2D[0].IsNull() && !aC2D[1].IsNull()) {
        BOPCol_MapIteratorOfMapOfInteger aItMI, aItMI1;
        aItMI.Initialize(aMVStick);
        for (; aItMI.More(); aItMI.Next()) {
          nV = aItMI.Value();
          const TopoDS_Vertex& aV=*((TopoDS_Vertex*)&myDS->Shape(nV));
          aPV=BRep_Tool::Pnt(aV);
          //
          for (m=0; m<2; ++m) {
            aD2=aPC[m].SquareDistance(aPV);
            if (aD2>aDT2) {// no rich
              continue; 
            }
            //
            for (n=0; n<2; ++n) {
              Handle(Geom_Surface)& aS=(!n)? aS1 : aS2;
              aC2D[n]->D0(aTC[m], aP2D);
              aP2D.Coord(u, v);
              BOPTools_AlgoTools3D::GetNormalToSurface(aS, u, v, aDN[n]);
            }
            // 
            aScPr=aDN[0]*aDN[1];
            if (aScPr<0.) {
              aScPr=-aScPr;
            }
            aScPr=1.-aScPr;
            //
            if (aScPr>aDScPr) {
              continue;
            }
            //
            // The intersection curve aIC is vanishing curve (the crease)
            aD=sqrt(aD2);
            //
            Handle(BOPDS_PaveBlock)& aPB=aNC.ChangePaveBlock1();
            PutPaveOnCurve(nV, aD, aNC, aPB);
          }
        }//for (jVU=1; jVU=aNbVU; ++jVU) {
      }
    }//if (aTypeC==GeomAbs_BezierCurve || aTypeC==GeomAbs_BSplineCurve) {
  }//if(aType1==GeomAbs_Torus  || aType2==GeomAbs_Torus) {
}
//modified by NIZHNY-EMV Fri Sep 23 11:24:25 2011

//modified by NIZHNY-EMV Fri Sep 23 11:19:34 2011
//=======================================================================
//function : GetInterfs
//purpose  : 
//=======================================================================
  void BOPAlgo_PaveFiller::GetInterfs (const Standard_Integer nF1,
                                       const Standard_Integer nF2,
                                       BOPCol_MapOfInteger& aMVStick,
                                       BOPCol_DataMapOfIntegerListOfInteger& aMInterfs,
                                       const Standard_Integer iFlag)
{
  BOPCol_MapOfInteger aMIF1, aMIF2;
  Standard_Integer nV1, nV2, nE1, nE2, nV, nE, nF, nVNew;
  //
  BOPDS_VectorOfInterfVV& aVVs=myDS->InterfVV();
  BOPDS_VectorOfInterfVE& aVEs=myDS->InterfVE();
  BOPDS_VectorOfInterfEE& aEEs=myDS->InterfEE();
  BOPDS_VectorOfInterfVF& aVFs=myDS->InterfVF();
  BOPDS_VectorOfInterfEF& aEFs=myDS->InterfEF();
  //
  Standard_Integer aNbVVs, aNbVEs, aNbEEs, aNbVFs, aNbEFs;
  aNbVVs = aVVs.Extent();
  aNbVEs = aVEs.Extent();
  aNbEEs = aEEs.Extent();
  aNbVFs = aVFs.Extent();
  aNbEFs = aEFs.Extent();
  
  //collect indexes of all shapes from nF1 and nF2.
  GetFullFaceMap(nF1, aMIF1);
  GetFullFaceMap(nF2, aMIF2);
  //collect VV interferences
  if (iFlag & 1) {
    BOPCol_ListOfInteger aLVV;
    for(Standard_Integer aInt = 0; aInt < aNbVVs; ++aInt) {
      const BOPDS_InterfVV& aVV = aVVs(aInt);
      nV1 = aVV.Index1();
      nV2 = aVV.Index2();
      if(aMIF1.Contains(nV1) && aMIF2.Contains(nV2) ||
         aMIF1.Contains(nV2) && aMIF2.Contains(nV1)) {
        if (aVV.HasIndexNew()) {
          nVNew = aVV.IndexNew();
          aMVStick.Add(nVNew);
          aLVV.Append(aInt);
        }
      }
    }
    aMInterfs.Bind(0, aLVV);
  }
  //collect VE interferences
  if (iFlag & 2) {
    BOPCol_ListOfInteger aLVE;
    for(Standard_Integer aInt = 0; aInt < aNbVEs; ++aInt) {
      const BOPDS_InterfVE& aVE = aVEs(aInt);
      nV = aVE.Index1();
      nE = aVE.Index2();
      if(aMIF1.Contains(nV) && aMIF2.Contains(nE) ||
         aMIF1.Contains(nE) && aMIF2.Contains(nV)) {
        aMVStick.Add(nV);
        aLVE.Append(aInt);
      }
    }
    aMInterfs.Bind(1, aLVE);
  }
  //collect EE interferences
  if (iFlag & 4) {
    BOPCol_ListOfInteger aLEE;
    for(Standard_Integer aInt = 0; aInt < aNbEEs; ++aInt) {
      const BOPDS_InterfEE& aEE = aEEs(aInt);
      nE1 = aEE.Index1();
      nE2 = aEE.Index2();
      if(aMIF1.Contains(nE1) && aMIF2.Contains(nE2) ||
         aMIF1.Contains(nE2) && aMIF2.Contains(nE1)) {
        IntTools_CommonPrt aCP = aEE.CommonPart();
        if(aCP.Type() == TopAbs_VERTEX) {
          nVNew = aEE.IndexNew();
          aMVStick.Add(nVNew);
          aLEE.Append(aInt);
        }
      }
    }
    aMInterfs.Bind(2, aLEE);
  }
  //collect VF interferences
  if (iFlag & 8) {
    BOPCol_ListOfInteger aLVF;
    for(Standard_Integer aInt = 0; aInt < aNbVFs; ++aInt) {
      const BOPDS_InterfVF& aVF = aVFs(aInt);
      nV = aVF.Index1();
      nF = aVF.Index2();
      if(aMIF1.Contains(nV) && aMIF2.Contains(nF) ||
         aMIF1.Contains(nF) && aMIF2.Contains(nV)) {
        aMVStick.Add(nV);
        aLVF.Append(aInt);
      }
    }
    aMInterfs.Bind(3, aLVF);
  }
  //collect EF interferences
  if (iFlag & 16) {
    BOPCol_ListOfInteger aLEF;
    for(Standard_Integer aInt = 0; aInt < aNbEFs; ++aInt) {
      const BOPDS_InterfEF& aEF = aEFs(aInt);
      nE = aEF.Index1();
      nF = aEF.Index2();
      if(aMIF1.Contains(nE) && aMIF2.Contains(nF) ||
         aMIF1.Contains(nF) && aMIF2.Contains(nE)) {
        IntTools_CommonPrt aCP = aEF.CommonPart();
        if(aCP.Type() == TopAbs_VERTEX) {
          nVNew = aEF.IndexNew();
          aMVStick.Add(nVNew);
          aLEF.Append(aInt);
        }
      }
    }
    aMInterfs.Bind(4, aLEF);
  }
}
//modified by NIZHNY-EMV Fri Sep 23 11:19:38 2011

//modified by NIZHNY-EMV Fri Sep 23 11:45:24 2011
//=======================================================================
// function: GetFullFaceMap
// purpose: 
//=======================================================================
void BOPAlgo_PaveFiller::GetFullFaceMap(const Standard_Integer nF,
                                        BOPCol_MapOfInteger& aMI)
{
  Standard_Integer nV, nE;
  TopoDS_Edge aE;
  TopoDS_Vertex aV;
  //face
  aMI.Add(nF);
  //edges
  const TopoDS_Face& aF = (*(TopoDS_Face*)(&myDS->Shape(nF)));
  TopExp_Explorer anExp(aF, TopAbs_EDGE);
  for (; anExp.More(); anExp.Next()) {
    aE = (*(TopoDS_Edge *)(&anExp.Current()));
    nE = myDS->Index(aE);
    aMI.Add(nE);
  }
  //vertices
  TopExp_Explorer anExpV(aF, TopAbs_VERTEX);
  for (; anExpV.More(); anExpV.Next()) {
    aV = (*(TopoDS_Vertex *)(&anExpV.Current()));
    nV = myDS->Index(aV);
    aMI.Add(nV);
  }
}
//modified by NIZHNY-EMV Fri Sep 23 11:45:20 2011
//=======================================================================
// function: RemoveUsedVertices
// purpose: 
//=======================================================================
void BOPAlgo_PaveFiller::RemoveUsedVertices(BOPDS_Curve& aNC,
                                            BOPCol_MapOfInteger& aMV)
{
  if (!aMV.Extent()) {
    return;
  }
  Standard_Integer nV;
  BOPDS_Pave aPave;
  BOPDS_ListIteratorOfListOfPave aItLP;
  //
  Handle(BOPDS_PaveBlock)& aPB=aNC.ChangePaveBlock1();
  const BOPDS_ListOfPave& aLP = aPB->ExtPaves();
  aItLP.Initialize(aLP);
  for (;aItLP.More();aItLP.Next()) {
    aPave = aItLP.Value();
    nV = aPave.Index();
    aMV.Remove(nV);
  }
}
//modified by NIZHNY-EMV Fri Sep 23 12:05:10 2011
//modified by NIZHNY-EMV Fri Sep 23 13:27:31 2011
//=======================================================================
//function : PutPaveOnCurve
//purpose  : 
//=======================================================================
  void BOPAlgo_PaveFiller::PutPaveOnCurve(const Standard_Integer nV,
                                          const Standard_Real aTolR3D,
                                          const BOPDS_Curve& aNC, 
                                          Handle(BOPDS_PaveBlock)& aPB)
{
  Standard_Boolean bIsVertexOnLine;
  Standard_Real aT;
  BOPDS_Pave aPave;
  //
  const TopoDS_Vertex aV = (*(TopoDS_Vertex *)(&myDS->Shape(nV)));
  const IntTools_Curve& aIC = aNC.Curve();
  //
  bIsVertexOnLine=myContext->IsVertexOnLine(aV, aIC, aTolR3D, aT);
  if (bIsVertexOnLine) {
    aPave.SetIndex(nV);
    aPave.SetParameter(aT);
    //
    aPB->AppendExtPave(aPave);
    //
    BOPTools_AlgoTools::UpdateVertex (aIC, aT, aV);
    // 
    BOPDS_ShapeInfo& aSIDS=myDS->ChangeShapeInfo(nV);
    Bnd_Box& aBoxDS=aSIDS.ChangeBox();
    BRepBndLib::Add(aV, aBoxDS);
  }
}
//modified by NIZHNY-EMV Fri Sep 23 13:27:32 2011

//=======================================================================
//function : ProcessOldPaveBlocks
//purpose  : 
//=======================================================================
  void BOPAlgo_PaveFiller::ProcessExistingPaveBlocks
    (const Standard_Integer theInt,
     const BOPDS_MapOfPaveBlock& aMPBOnIn,
     const BOPCol_MapOfInteger& aMV,
     BOPDS_MapOfPaveBlock& aMPB,
     BOPDS_DataMapOfShapeCoupleOfPaveBlocks& aMSCPB,
     BOPCol_DataMapOfShapeInteger& aMVI)
{
  Standard_Integer nV, nV1, nV2, nE, iFlag, iC;
  Standard_Real aT;
  BOPCol_MapIteratorOfMapOfInteger aIt;
  BOPDS_MapIteratorOfMapOfPaveBlock aItPB;
  //
  BOPDS_VectorOfInterfFF& aFFs=myDS->InterfFF();
  BOPDS_InterfFF& aFF = aFFs(theInt);
  BOPDS_VectorOfCurve& aVC=aFF.ChangeCurves();
  //  
  aIt.Initialize(aMV);
  for (; aIt.More(); aIt.Next()) {
    nV = aIt.Value();
    const BOPDS_ShapeInfo& aSIV=myDS->ShapeInfo(nV);
    const Bnd_Box& aBoxV=aSIV.Box();
    const TopoDS_Vertex& aV = *(TopoDS_Vertex*)&aSIV.Shape();
    //
    aItPB.Initialize(aMPBOnIn);
    for (; aItPB.More(); aItPB.Next()) {
      const Handle(BOPDS_PaveBlock)& aPB = aItPB.Value();
      if (aPB->Pave1().Index() == nV || aPB->Pave2().Index() == nV) {
        continue;
      }
      //
      if (aMPB.Contains(aPB)) {
        continue;
      }
      nE=aPB->Edge();
      const BOPDS_ShapeInfo& aSIE=myDS->ShapeInfo(nE);
      const Bnd_Box& aBoxE=aSIE.Box();
      //
      if (!aBoxV.IsOut(aBoxE)) {
        const TopoDS_Edge& aE = *(TopoDS_Edge*)&aSIE.Shape();
        //
        iFlag=myContext->ComputeVE (aV, aE, aT);
        if (!iFlag) {
          iC=aVC.Append()-1;
          BOPDS_Curve& aNC=aVC(iC);
          BOPDS_ListOfPaveBlock& aLPBC = aNC.ChangePaveBlocks();
          //
          aMPB.Add(aPB);
          aLPBC.Append(aPB);
          //
          aPB->Indices(nV1, nV2);
          const TopoDS_Vertex& aV1=(*(TopoDS_Vertex *)(&myDS->Shape(nV1)));
          const TopoDS_Vertex& aV2=(*(TopoDS_Vertex *)(&myDS->Shape(nV2)));
          // Keep info for post treatment 
          BOPDS_CoupleOfPaveBlocks aCPB;
          aCPB.SetIndexInterf(theInt);
          aCPB.SetIndex(iC);
          aCPB.SetPaveBlock1(aPB);
          //
          aMSCPB.Bind(aE, aCPB);
          aMVI.Bind(aV1, nV1);
          aMVI.Bind(aV2, nV2);
        }
      }
    }
  }
}

//=======================================================================
//function : UpdateExistingPaveBlocks
//purpose  : 
//=======================================================================
  void BOPAlgo_PaveFiller::UpdateExistingPaveBlocks
    (BOPDS_ListOfPaveBlock& aLPBC,
     const Standard_Integer nF1,
     const Standard_Integer nF2) 
{
  if (!aLPBC.Extent()) {
    return;
  }
  //
  BOPDS_MapOfPaveBlock aMPB;
  BOPDS_MapIteratorOfMapOfPaveBlock aItMPB;
  Standard_Integer nE;
  Handle(BOPDS_PaveBlock) aPBf;
  aPBf = aLPBC.First();
  nE = aPBf->OriginalEdge();
  if (nE == -1) {
    return;
  }
  //
  Handle(BOPDS_PaveBlock) aPB, aPB1, aPB2, aPB2n;
  Handle(BOPDS_CommonBlock) aCB;
  BOPDS_ListOfPaveBlock aLPB;
  BOPDS_ListIteratorOfListOfPaveBlock aIt, aIt1, aIt2;
  //
  FindPaveBlocksByFirst(aLPBC, aLPB);
  if (aLPB.Extent() == 1) {
    return;
  }
  //
  aPBf = aLPB.First();
  if (aPBf->IsCommonBlock()) {
    const Handle(BOPDS_CommonBlock)& aCB1 = aPBf->CommonBlock();
    const BOPDS_ListOfPaveBlock& aLPB2 = aCB1->PaveBlocks();
    //
    // remove old pave blocks
    aIt2.Initialize(aLPB2);
    for (; aIt2.More(); aIt2.Next()) {
      aPB2 = aIt2.Value();
      nE = aPB2->OriginalEdge();
      //
      BOPDS_ListOfPaveBlock& aLPB1 = myDS->ChangePaveBlocks(nE);
      aIt1.Initialize(aLPB1);
      for (; aIt1.More(); aIt1.Next()) {
        aPB1 = aIt1.Value();
        if (aPB2 == aPB1) {
          aLPB1.Remove(aIt1);
          break;
        }
      }
    }
    //
    //create new pave blocks
    aIt.Initialize(aLPB);
    for (aIt.Next(); aIt.More(); aIt.Next()) {
      aPB = aIt.Value();
      //
      aCB = new BOPDS_CommonBlock;
      aIt2.Initialize(aLPB2);
      for (; aIt2.More(); aIt2.Next()) {
        aPB2 = aIt2.Value();
        nE = aPB2->OriginalEdge();
        //
        aPB2n = new BOPDS_PaveBlock;
        aPB2n->SetPave1(aPB->Pave1());
        aPB2n->SetPave2(aPB->Pave2());
        aPB2n->SetEdge(aPB->Edge());
        aPB2n->SetOriginalEdge(nE);
        aCB->AddPaveBlock(aPB2n);
        aPB2n->SetCommonBlock(aCB);
        myDS->ChangePaveBlocks(nE).Append(aPB2n);
      }
      aCB->AddFace(nF1);
      aCB->AddFace(nF2);
      myDS->SortPaveBlocks(aCB);
    }
  } 
  //
  else {
    BOPDS_ListOfPaveBlock& aLPB1 = myDS->ChangePaveBlocks(nE);
    aIt1.Initialize(aLPB1);
    for (; aIt1.More(); aIt1.Next()) {
      aPB1 = aIt1.Value();
      if (aPBf == aPB1) {
        aLPB1.Remove(aIt1);
        break;
      }
    }
    //
    aIt.Initialize(aLPB);
    for (aIt.Next(); aIt.More(); aIt.Next()) {
      aPB = aIt.Value();
      //
      if (IsCommonBlockOnFaces(aPB, nF1, nF2)) {
        aCB = new BOPDS_CommonBlock;
        aCB->AddPaveBlock(aPB);
        aCB->AddFace(nF1);
        aCB->AddFace(nF2);
        //
        aPB->SetCommonBlock(aCB);
        aMPB.Add(aPB);
      }
      aLPB1.Append(aPB);
    }
  }
  //update face info
  myDS->UpdateFaceInfoOn(nF1);
  //
  myDS->UpdateFaceInfoOn(nF2);
  //
  BOPDS_FaceInfo& aFI1 = myDS->ChangeFaceInfo(nF1);
  BOPDS_FaceInfo& aFI2 = myDS->ChangeFaceInfo(nF2);
  //
  BOPDS_MapOfPaveBlock& aMPBOn1 = aFI1.ChangePaveBlocksOn();
  BOPDS_MapOfPaveBlock& aMPBIn1 = aFI1.ChangePaveBlocksIn();
  BOPDS_MapOfPaveBlock& aMPBOn2 = aFI2.ChangePaveBlocksOn();
  BOPDS_MapOfPaveBlock& aMPBIn2 = aFI2.ChangePaveBlocksIn();
  //
  aItMPB.Initialize(aMPB);
  for(; aItMPB.More(); aItMPB.Next()) {
    const Handle(BOPDS_PaveBlock)& aPBnew = aItMPB.Value();
    if (!aMPBOn1.Contains(aPBnew)) {
      aMPBIn1.Add(aPBnew);
    }
    //
    if (!aMPBOn2.Contains(aPBnew)) {
      aMPBIn2.Add(aPBnew);
    }
  }
}

//modified by NIZHNY-EMV Wed Feb 15 10:08:44 2012
//=======================================================================
// function: PutClosingPaveOnCurve
// purpose:
//=======================================================================
  void BOPAlgo_PaveFiller::PutClosingPaveOnCurve(BOPDS_Curve& aNC)
{
  Standard_Boolean bIsClosed, bHasBounds, bAdded;
  Standard_Integer nVC, j;
  Standard_Real aT[2], aTC, dT, aTx;
  gp_Pnt aP[2] ; 
  BOPDS_Pave aPVx;
  BOPDS_ListIteratorOfListOfPave aItLP;
  //
  const IntTools_Curve& aIC=aNC.Curve();
  const Handle(Geom_Curve)& aC3D=aIC.Curve();
  if(aC3D.IsNull()) {
    return;
  }
  //
  bIsClosed=IntTools_Tools::IsClosed(aC3D);
  if (!bIsClosed) {
    return;
  }
  //
  bHasBounds=aIC.HasBounds ();
  if (!bHasBounds){
    return;
  }
  // 
  bAdded=Standard_False;
  dT=Precision::PConfusion();
  aIC.Bounds (aT[0], aT[1], aP[0], aP[1]);
  //
  Handle(BOPDS_PaveBlock)& aPB=aNC.ChangePaveBlock1();
  BOPDS_ListOfPave& aLP=aPB->ChangeExtPaves();
  //
  aItLP.Initialize(aLP);
  for (; aItLP.More() && !bAdded; aItLP.Next()) {
    const BOPDS_Pave& aPC=aItLP.Value();
    nVC=aPC.Index();
    aTC=aPC.Parameter();
    //
    for (j=0; j<2; ++j) {
      if (fabs(aTC-aT[j]) < dT) {
        aTx=(!j) ? aT[1] : aT[0];
        aPVx.SetIndex(nVC);
        aPVx.SetParameter(aTx);
        aLP.Append(aPVx);
        //
        bAdded=Standard_True;
        break;
      }
    }
  }
}
//modified by NIZHNY-EMV Wed Feb 15 10:08:34 2012

//=======================================================================
// function: IsCommonOnFaces
// purpose:
//=======================================================================
  Standard_Boolean BOPAlgo_PaveFiller::IsCommonBlockOnFaces(const Handle(BOPDS_PaveBlock)& aPB,
                                                            const Standard_Integer nF1,
                                                            const Standard_Integer nF2) 
{
  Standard_Boolean bRet;
  Standard_Integer i, j, nV1, nV2;
  Standard_Real aT1, aT2, aTm, f, l;
  gp_Pnt aP1, aP2, aPm;
  BRep_Builder aBB;
  //
  bRet = Standard_True;
  aPB->Indices(nV1, nV2);
  //
  const TopoDS_Edge& aE = *(TopoDS_Edge*)&myDS->Shape(aPB->Edge());
  Handle(Geom_Curve) aC3D = BRep_Tool::Curve(aE, aT1, aT2);
  aTm=IntTools_Tools::IntermediatePoint (aT1, aT2);
  //
  const TopoDS_Vertex& aV1 = *(TopoDS_Vertex*)&myDS->Shape(nV1);
  const TopoDS_Vertex& aV2 = *(TopoDS_Vertex*)&myDS->Shape(nV2);
  aC3D->D0(aTm, aPm);  
  //
  TopoDS_Vertex aVm = BRepBuilderAPI_MakeVertex(aPm);
  aBB.UpdateVertex(aVm, BRep_Tool::Tolerance(aE));
  //
  TopoDS_Vertex aV[3] = {aV1, aVm, aV2};
  //
  const TopoDS_Face& aF1=(*(TopoDS_Face *)(&myDS->Shape(nF1)));
  const TopoDS_Face& aF2=(*(TopoDS_Face *)(&myDS->Shape(nF2)));
  //
  for (i = 0; i < 2; ++i) {
    const TopoDS_Face& aF = (i == 0) ? aF1 : aF2;
    for (j = 0; j < 3; ++j) {
      if (bRet) {
        bRet = !myContext->ComputeVF(aV[i], aF, f, l) ? bRet : !bRet;
      }
    }
  }
  //
  return bRet;
}

//=======================================================================
//function : FindPaveBlocksByFirst
//purpose  : 
//=======================================================================
void FindPaveBlocksByFirst(BOPDS_ListOfPaveBlock& aLPBC,
                           BOPDS_ListOfPaveBlock& aLPB)
{
  Standard_Integer nE;
  BOPDS_ListIteratorOfListOfPaveBlock aIt;
  //
  const Handle(BOPDS_PaveBlock)& aPB1 = aLPBC.First();
  nE = aPB1->OriginalEdge();
  aLPB.Append(aPB1);
  //
  aLPBC.RemoveFirst();
  //
  aIt.Initialize(aLPBC);
  for (;aIt.More();) {
    const Handle(BOPDS_PaveBlock)& aPB = aIt.Value();
    if (aPB->OriginalEdge() == nE) {
      aLPB.Append(aPB);
      aLPBC.Remove(aIt);
      continue;
    }
    aIt.Next();
  }
}

//=======================================================================
//function : IsMicroEdge
//purpose  : Checks if it is possible to compute shrunk range for the edge <aE>.
//=======================================================================
Standard_Boolean IsMicroEdge(const TopoDS_Edge& aE,
                             const Handle(BOPInt_Context)& aCtx) 
{
  Standard_Boolean bRet;
  Standard_Integer iErr;
  Standard_Real aT1, aT2, aTmp;
  Handle(Geom_Curve) aC3D;
  TopoDS_Vertex aV1, aV2;
  //
  bRet=(BRep_Tool::Degenerated(aE) ||
        !BRep_Tool::IsGeometric(aE));
  if (bRet) {
    return bRet;
  }
  //
  aC3D=BRep_Tool::Curve(aE, aT1, aT2);
  TopExp::Vertices(aE, aV1, aV2);
  aT1=BRep_Tool::Parameter(aV1, aE);
  aT2=BRep_Tool::Parameter(aV2, aE);
  if (aT2<aT1) {
    aTmp=aT1;
    aT1=aT2;
    aT2=aTmp;
  }
  //
  BOPInt_ShrunkRange aSR;
  aSR.SetData(aE, aT1, aT2, aV1, aV2, aCtx);
  aSR.Perform();
  iErr=aSR.ErrorStatus();
  bRet = !(iErr==0);
  //
  return bRet;
}

// DEB f
  /*
  {
    BOPDS_DataMapIteratorOfDataMapOfShapeCoupleOfPaveBlocks aItx;
    TopoDS_Compound aCx;
    //
    BRep_Builder aBBx;
    aBBx.MakeCompound(aCx);
    //
    aItx.Initialize(theMSCPB);
    for (; aItx.More(); aItx.Next()) {
      const TopoDS_Shape& aSx=aItx.Key();
      aBBx.Add(aCx, aSx);
    }
    int a=0;
    BRepTools::Write(aCx, "cx");
  }
  */
  // DEB t
