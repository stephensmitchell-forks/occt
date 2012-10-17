// Created by: Peter KURNEV
// Copyright (c) 1999-2012 OPEN CASCADE SAS
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


#include <BOPInt_Context.ixx>

#include <Precision.hxx>

#include <Geom_Curve.hxx>
#include <Geom_BoundedCurve.hxx>
#include <GeomAPI_ProjectPointOnCurve.hxx>
#include <GeomAPI_ProjectPointOnSurf.hxx>
#include <GeomAdaptor_Curve.hxx>

#include <TopAbs_State.hxx>
#include <TopoDS.hxx>
#include <TopExp_Explorer.hxx>

#include <BRep_Tool.hxx>
#include <BRepAdaptor_Surface.hxx>

#include <IntTools_Tools.hxx>
#include <IntTools_FClass2d.hxx>
// 
#include <Extrema_LocateExtPC.hxx>

#include <Geom2d_Curve.hxx>
#include <NCollection_IncAllocator.hxx>


//=======================================================================
//function : 
//purpose  : 
//=======================================================================
  BOPInt_Context::BOPInt_Context()
:
  myAllocator(new NCollection_IncAllocator()),
  myFClass2dMap(100, myAllocator),
  myProjPSMap(100, myAllocator),
  myProjPCMap(100, myAllocator),
  mySClassMap(100, myAllocator),
  myProjPTMap(100, myAllocator),
  myCreateFlag(0)
{
}
//=======================================================================
//function : 
//purpose  : 
//=======================================================================
  BOPInt_Context::BOPInt_Context(const Handle(NCollection_BaseAllocator)& theAllocator)
:
  myAllocator(theAllocator),
  myFClass2dMap(100, myAllocator),
  myProjPSMap(100, myAllocator),
  myProjPCMap(100, myAllocator),
  mySClassMap(100, myAllocator),
  myProjPTMap(100, myAllocator),
  myCreateFlag(1)
{
}
//=======================================================================
//function : ~
//purpose  : 
//=======================================================================
  BOPInt_Context::~BOPInt_Context()
{
  Standard_Address anAdr;
  BOPCol_DataMapIteratorOfDataMapOfShapeAddress aIt;
  BOPCol_DataMapIteratorOfDataMapOfTransientAddress aIt1;
  //
  IntTools_FClass2d* pFClass2d;
  //
  aIt.Initialize(myFClass2dMap);
  for (; aIt.More(); aIt.Next()) {
    anAdr=aIt.Value();
    pFClass2d=(IntTools_FClass2d*)anAdr;
    (*pFClass2d).~IntTools_FClass2d();
    myAllocator->Free(anAdr); 
  }
  myFClass2dMap.Clear();
  //
  GeomAPI_ProjectPointOnSurf* pProjPS;
  aIt.Initialize(myProjPSMap);
  for (; aIt.More(); aIt.Next()) {
    anAdr=aIt.Value();
    pProjPS=(GeomAPI_ProjectPointOnSurf*)anAdr;
    (*pProjPS).~GeomAPI_ProjectPointOnSurf();
    myAllocator->Free(anAdr); 
  }
  myProjPSMap.Clear();
  //
  GeomAPI_ProjectPointOnCurve* pProjPC;
  aIt.Initialize(myProjPCMap);
  for (; aIt.More(); aIt.Next()) {
    anAdr=aIt.Value();
    pProjPC=(GeomAPI_ProjectPointOnCurve*)anAdr;
    (*pProjPC).~GeomAPI_ProjectPointOnCurve();
    myAllocator->Free(anAdr); 
  }
  myProjPCMap.Clear();
  //
  //
  BRepClass3d_SolidClassifier* pSC;
  aIt.Initialize(mySClassMap);
  for (; aIt.More(); aIt.Next()) {
    anAdr=aIt.Value();
    pSC=(BRepClass3d_SolidClassifier*)anAdr;
    (*pSC).~BRepClass3d_SolidClassifier();
    myAllocator->Free(anAdr); 
  }
  mySClassMap.Clear();
  //
  GeomAPI_ProjectPointOnCurve* pProjPT;
  aIt1.Initialize(myProjPTMap);
  for (; aIt1.More(); aIt1.Next()) {
    anAdr=aIt1.Value();
    pProjPT=(GeomAPI_ProjectPointOnCurve*)anAdr;
    (*pProjPT).~GeomAPI_ProjectPointOnCurve();
    myAllocator->Free(anAdr); 
  }
  myProjPTMap.Clear();
}
//=======================================================================
//function : FClass2d
//purpose  : 
//=======================================================================
  IntTools_FClass2d& BOPInt_Context::FClass2d(const TopoDS_Face& aF)
{
  Standard_Address anAdr;
  IntTools_FClass2d* pFClass2d;
  //
  if (!myFClass2dMap.IsBound(aF)) {
    Standard_Real aTolF;
    TopoDS_Face aFF;
    //
    aFF=aF;
    aFF.Orientation(TopAbs_FORWARD);
    aTolF=BRep_Tool::Tolerance(aFF);
    //
    pFClass2d=(IntTools_FClass2d*)myAllocator->Allocate(sizeof(IntTools_FClass2d));
    new (pFClass2d) IntTools_FClass2d(aFF, aTolF);
    //
    anAdr=(Standard_Address)pFClass2d;
    myFClass2dMap.Bind(aFF, anAdr);
  }
  else {
    anAdr=myFClass2dMap.Find(aF);
    pFClass2d=(IntTools_FClass2d*)anAdr;
  }
  return *pFClass2d;
}
//=======================================================================
//function : ProjPS
//purpose  : 
//=======================================================================
  GeomAPI_ProjectPointOnSurf& BOPInt_Context::ProjPS(const TopoDS_Face& aF)
{
  Standard_Address anAdr;
  GeomAPI_ProjectPointOnSurf* pProjPS;
 
  if (!myProjPSMap.IsBound(aF)) {
    Standard_Real Umin, Usup, Vmin, Vsup, anEpsT=1.e-12 ;
    BRepAdaptor_Surface aBAS;
    //
    const Handle(Geom_Surface)& aS=BRep_Tool::Surface(aF);
    aBAS.Initialize (aF, Standard_True);
    //
    Umin=aBAS.FirstUParameter();
    Usup=aBAS.LastUParameter ();
    Vmin=aBAS.FirstVParameter();
    Vsup=aBAS.LastVParameter ();
    //
    pProjPS=(GeomAPI_ProjectPointOnSurf*)myAllocator->Allocate(sizeof(GeomAPI_ProjectPointOnSurf));
    new (pProjPS) GeomAPI_ProjectPointOnSurf();
    pProjPS->Init(aS ,Umin, Usup, Vmin, Vsup, anEpsT);
    //
    anAdr=(Standard_Address)pProjPS;
    myProjPSMap.Bind(aF, anAdr);
  }
  
  else {
    anAdr=myProjPSMap.Find(aF);
    pProjPS=(GeomAPI_ProjectPointOnSurf*)anAdr;
  }
  return *pProjPS;
}
//=======================================================================
//function : ProjPC
//purpose  : 
//=======================================================================
  GeomAPI_ProjectPointOnCurve& BOPInt_Context::ProjPC(const TopoDS_Edge& aE)
{
  Standard_Address anAdr;
  GeomAPI_ProjectPointOnCurve* pProjPC;
 
  if (!myProjPCMap.IsBound(aE)) {
    Standard_Real f, l;
    //
    Handle(Geom_Curve)aC3D=BRep_Tool::Curve (aE, f, l);
    //
    pProjPC=(GeomAPI_ProjectPointOnCurve*)myAllocator->Allocate(sizeof(GeomAPI_ProjectPointOnCurve));
    new (pProjPC) GeomAPI_ProjectPointOnCurve();
    pProjPC->Init(aC3D, f, l);
    //
    anAdr=(Standard_Address)pProjPC;
    myProjPCMap.Bind(aE, anAdr);
  }
  
  else {
    anAdr=myProjPCMap.Find(aE);
    pProjPC=(GeomAPI_ProjectPointOnCurve*)anAdr;
  }
  return *pProjPC;
}

//=======================================================================
//function : ProjPT
//purpose  : 
//=======================================================================
  GeomAPI_ProjectPointOnCurve& BOPInt_Context::ProjPT(const Handle(Geom_Curve)& aC3D)

{
  Standard_Address anAdr;
  GeomAPI_ProjectPointOnCurve* pProjPT;
 
  if (!myProjPTMap.IsBound(aC3D)) {
    Standard_Real f, l;
    f=aC3D->FirstParameter();
    l=aC3D->LastParameter();
    //
    pProjPT=(GeomAPI_ProjectPointOnCurve*)myAllocator->Allocate(sizeof(GeomAPI_ProjectPointOnCurve));
    new (pProjPT) GeomAPI_ProjectPointOnCurve();
    pProjPT->Init(aC3D, f, l);
    //
    anAdr=(Standard_Address)pProjPT;
    myProjPTMap.Bind(aC3D, anAdr);
  }
  
  else {
    anAdr=myProjPTMap.Find(aC3D);
    pProjPT=(GeomAPI_ProjectPointOnCurve*)anAdr;
  }
  return *pProjPT;
}
//=======================================================================
//function : SolidClassifier
//purpose  : 
//=======================================================================
  BRepClass3d_SolidClassifier& BOPInt_Context::SolidClassifier(const TopoDS_Solid& aSolid)
{
  Standard_Address anAdr;
  BRepClass3d_SolidClassifier* pSC;
 
  if (!mySClassMap.IsBound(aSolid)) {
    //
    pSC=(BRepClass3d_SolidClassifier*)myAllocator->Allocate(sizeof(BRepClass3d_SolidClassifier));
    new (pSC) BRepClass3d_SolidClassifier(aSolid);
    //
    anAdr=(Standard_Address)pSC;
    mySClassMap.Bind(aSolid, anAdr);
  }
  
  else {
    anAdr=mySClassMap.Find(aSolid);
    pSC =(BRepClass3d_SolidClassifier*)anAdr;
  }
  return *pSC;
}
//=======================================================================
//function : ComputePE
//purpose  : 
//=======================================================================
  Standard_Integer BOPInt_Context::ComputePE(const gp_Pnt& aP1,
                                             const Standard_Real aTolP1,
                                             const TopoDS_Edge& aE2,
                                             Standard_Real& aT)
{
  if (!BRep_Tool::IsGeometric(aE2)) { 
    return -2;
  }
  Standard_Real aDist, aTolE2, aTolSum;
  Standard_Integer aNbProj;
  //
  GeomAPI_ProjectPointOnCurve& aProjector=ProjPC(aE2);
  aProjector.Perform(aP1);

  aNbProj=aProjector.NbPoints();
  if (!aNbProj) {
    return -3;
  }
  //
  aDist=aProjector.LowerDistance();
  //
  aTolE2=BRep_Tool::Tolerance(aE2);
  aTolSum=aTolP1+aTolE2;
  //
  aT=aProjector.LowerDistanceParameter();
  if (aDist > aTolSum) {
    return -4;
  }
  return 0;
}
//=======================================================================
//function : ComputeVE
//purpose  : 
//=======================================================================
  Standard_Integer BOPInt_Context::ComputeVE(const TopoDS_Vertex& aV1, 
                                             const TopoDS_Edge&   aE2,
                                             Standard_Real& aT)
{
  if (BRep_Tool::Degenerated(aE2)) {
    return -1;
  }
  if (!BRep_Tool::IsGeometric(aE2)) { 
    return -2;
  }
  Standard_Real aDist, aTolV1, aTolE2, aTolSum;
  Standard_Integer aNbProj;
  gp_Pnt aP;
  //
  aP=BRep_Tool::Pnt(aV1);
  //
  GeomAPI_ProjectPointOnCurve& aProjector=ProjPC(aE2);
  aProjector.Perform(aP);

  aNbProj=aProjector.NbPoints();
  if (!aNbProj) {
    return -3;
  }
  //
  aDist=aProjector.LowerDistance();
  //
  aTolV1=BRep_Tool::Tolerance(aV1);
  aTolE2=BRep_Tool::Tolerance(aE2);
  aTolSum=aTolV1+aTolE2;
  //
  aT=aProjector.LowerDistanceParameter();
  if (aDist > aTolSum) {
    return -4;
  }
  return 0;
}
//=======================================================================
//function : ComputeVS
//purpose  : 
//=======================================================================
  Standard_Integer BOPInt_Context::ComputeVF(const TopoDS_Vertex& aV1, 
                                             const TopoDS_Face&   aF2,
                                             Standard_Real& U,
                                             Standard_Real& V)
{
  Standard_Real aTolV1, aTolF2, aTolSum, aDist;
  gp_Pnt aP;

  aP=BRep_Tool::Pnt(aV1);
  //
  // 1. Check if the point is projectable on the surface
  GeomAPI_ProjectPointOnSurf& aProjector=ProjPS(aF2);
  aProjector.Perform(aP);
  //
  if (!aProjector.IsDone()) { // the point is not  projectable on the surface
    return -1;
  }
  //
  // 2. Check the distance between the projection point and 
  //    the original point
  aDist=aProjector.LowerDistance();

  aTolV1=BRep_Tool::Tolerance(aV1);
  aTolF2=BRep_Tool::Tolerance(aF2);
  aTolSum=aTolV1+aTolF2;
  if (aDist > aTolSum) {
    // the distance is too large
    return -2;
  }
  aProjector.LowerDistanceParameters(U, V);
  //
  gp_Pnt2d aP2d(U, V);
  Standard_Boolean pri=IsPointInFace (aF2, aP2d);
  if (!pri) {//  the point lays on the surface but out of the face 
    return -3;
  }
  return 0;
}
//=======================================================================
//function : StatePointFace
//purpose  : 
//=======================================================================
  TopAbs_State BOPInt_Context::StatePointFace(const TopoDS_Face& aF,
                                              const gp_Pnt2d& aP2d)
{
  TopAbs_State aState;
  IntTools_FClass2d& aClass2d=FClass2d(aF);
  aState=aClass2d.Perform(aP2d);
  return aState;
}
//=======================================================================
//function : IsPointInFace
//purpose  : 
//=======================================================================
  Standard_Boolean BOPInt_Context::IsPointInFace(const TopoDS_Face& aF,
                                                 const gp_Pnt2d& aP2d)
{
  TopAbs_State aState=StatePointFace(aF, aP2d);
  if (aState==TopAbs_OUT || aState==TopAbs_ON) {
    return Standard_False;
  }
  return Standard_True;
}
//=======================================================================
//function : IsPointInOnFace
//purpose  : 
//=======================================================================
  Standard_Boolean BOPInt_Context::IsPointInOnFace(const TopoDS_Face& aF,
                                                   const gp_Pnt2d& aP2d)
{ 
  TopAbs_State aState=StatePointFace(aF, aP2d);
  if (aState==TopAbs_OUT) {
    return Standard_False;
  }
  return Standard_True;
}
//=======================================================================
//function : IsValidPointForFace
//purpose  : 
//=======================================================================
  Standard_Boolean BOPInt_Context::IsValidPointForFace(const gp_Pnt& aP,
                                                       const TopoDS_Face& aF,
                                                       const Standard_Real aTol) 
{
  Standard_Boolean bFlag;
  Standard_Real Umin, myEpsT, U, V;
  myEpsT=1.e-12;

  GeomAPI_ProjectPointOnSurf& aProjector=ProjPS(aF);
  aProjector.Perform(aP);
  
  bFlag=aProjector.IsDone();
  if (bFlag) {
    
    Umin=aProjector.LowerDistance();
    //if (Umin > 1.e-3) { // it was 
    if (Umin > aTol) {
      return !bFlag; 
    }
    //
    aProjector.LowerDistanceParameters(U, V);
    gp_Pnt2d aP2D(U, V);
    //modified by NIZHNY-EMV Tue Apr 12 10:57:52 2011
    //modified by NIZNHY-PKV Wed Jul  8 14:56:39 2009f
    bFlag=IsPointInOnFace (aF, aP2D);
    //bFlag=IsPointInFace (aF, aP2D);
    //modified by NIZNHY-PKV Wed Jul  8 14:56:42 2009t
    //modified by NIZHNY-EMV Tue Apr 12 10:58:02 2011
  }
  return bFlag;
}
//=======================================================================
//function : IsValidPointForFaces
//purpose  : 
//=======================================================================
  Standard_Boolean BOPInt_Context::IsValidPointForFaces (const gp_Pnt& aP,
                                                         const TopoDS_Face& aF1,
                                                         const TopoDS_Face& aF2,
                                                         const Standard_Real aTol) 
{
  Standard_Boolean bFlag1, bFlag2;

  bFlag1=IsValidPointForFace(aP, aF1, aTol);
  if (!bFlag1) {
    return bFlag1;
  }
  bFlag2=IsValidPointForFace(aP, aF2, aTol);  
  return  bFlag2;
}
//=======================================================================
//function : IsValidBlockForFace
//purpose  : 
//=======================================================================
  Standard_Boolean BOPInt_Context::IsValidBlockForFace (const Standard_Real aT1,
                                                        const Standard_Real aT2,
                                                        const IntTools_Curve& aC, 
                                                        const TopoDS_Face& aF,
                                                        const Standard_Real aTol) 
{
  Standard_Boolean bFlag;
  Standard_Real aTInterm, aFirst, aLast;
  gp_Pnt aPInterm;

  aTInterm=IntTools_Tools::IntermediatePoint(aT1, aT2);

  Handle(Geom_Curve) aC3D=aC.Curve();
  aFirst=aC3D->FirstParameter();
  aLast =aC3D->LastParameter();
  // point 3D
  aC3D->D0(aTInterm, aPInterm);
  //
  bFlag=IsValidPointForFace (aPInterm, aF, aTol);
  return bFlag;
}
//=======================================================================
//function : IsValidBlockForFaces
//purpose  : 
//=======================================================================
  Standard_Boolean BOPInt_Context::IsValidBlockForFaces (const Standard_Real aT1,
                                                         const Standard_Real aT2,
                                                         const IntTools_Curve& aC, 
                                                         const TopoDS_Face& aF1,
                                                         const TopoDS_Face& aF2,
                                                         const Standard_Real aTol) 
{
  Standard_Boolean bFlag1, bFlag2;
  //
  Handle(Geom2d_Curve) aPC1 = aC.FirstCurve2d();
  Handle(Geom2d_Curve) aPC2 = aC.SecondCurve2d();
  if( !aPC1.IsNull() && !aPC2.IsNull() ) {
    Standard_Real aMidPar = IntTools_Tools::IntermediatePoint(aT1, aT2);
    gp_Pnt2d aPnt2D;


    aPC1->D0(aMidPar, aPnt2D);
    bFlag1 = IsPointInOnFace(aF1, aPnt2D);

    if( !bFlag1 )
      return bFlag1;

    aPC2->D0(aMidPar, aPnt2D);
    bFlag2 = IsPointInOnFace(aF2, aPnt2D);
    return bFlag2;
  }
  //

  bFlag1=IsValidBlockForFace (aT1, aT2, aC, aF1, aTol);
  if (!bFlag1) {
    return bFlag1;
  }
  bFlag2=IsValidBlockForFace (aT1, aT2, aC, aF2, aTol);
  return bFlag2;
}
//=======================================================================
//function : IsVertexOnLine
//purpose  : 
//=======================================================================
  Standard_Boolean BOPInt_Context::IsVertexOnLine (const TopoDS_Vertex& aV,
                                                   const IntTools_Curve& aC, 
                                                   const Standard_Real aTolC,
                                                   Standard_Real& aT)
{
  Standard_Boolean bRet;
  Standard_Real aTolV;
  //
  aTolV=BRep_Tool::Tolerance(aV);
  bRet=BOPInt_Context::IsVertexOnLine(aV, aTolV, aC, aTolC , aT);
  //
  return bRet;
}
//=======================================================================
//function : IsVertexOnLine
//purpose  : 
//=======================================================================
  Standard_Boolean BOPInt_Context::IsVertexOnLine (const TopoDS_Vertex& aV,
                                                   const Standard_Real aTolV,
                                                   const IntTools_Curve& aC, 
                                                   const Standard_Real aTolC,
                                                   Standard_Real& aT)
{
  Standard_Real aFirst, aLast, aDist, aTolSum;
  Standard_Integer aNbProj;
  gp_Pnt aPv; 
  
  aPv=BRep_Tool::Pnt(aV);

  Handle(Geom_Curve) aC3D=aC.Curve();
  
  
  aTolSum=aTolV+aTolC;
  //
  GeomAdaptor_Curve aGAC(aC3D);
  GeomAbs_CurveType aType=aGAC.GetType();
  if (aType==GeomAbs_BSplineCurve ||
      aType==GeomAbs_BezierCurve) {
    aTolSum=2.*aTolSum;
    if (aTolSum<1.e-5) {
      aTolSum=1.e-5;
    }
  }
  else {
    aTolSum=2.*aTolSum;//xft
    if(aTolSum < 1.e-6)
      aTolSum = 1.e-6;
  }
  //
  aFirst=aC3D->FirstParameter();
  aLast =aC3D->LastParameter();
  //
  //Checking extermities first
  if (!Precision::IsInfinite(aFirst)) {
    gp_Pnt aPCFirst=aC3D->Value(aFirst);
    aDist=aPv.Distance(aPCFirst);
    if (aDist < aTolSum) {
      aT=aFirst;
      //
      if(aDist > aTolV) {
        Extrema_LocateExtPC anExt(aPv, aGAC, aFirst, 1.e-10);
        
        if(anExt.IsDone()) {
          Extrema_POnCurv aPOncurve = anExt.Point();
          aT = aPOncurve.Parameter();
          
          if((aT > (aLast + aFirst) * 0.5) ||
             (aPv.Distance(aPOncurve.Value()) > aTolSum) ||
             (aPCFirst.Distance(aPOncurve.Value()) < Precision::Confusion()))
            aT = aFirst;
        }
      }
      //
      return Standard_True;
    }
  }
  //
  //modified by NIZNHY-PKV Thu Nov 30 14:45:52 2006f
  //if (!Precision::IsInfinite(aFirst)) {
  if (!Precision::IsInfinite(aLast)) {
  //modified by NIZNHY-PKV Thu Nov 30 14:45:55 2006t
    gp_Pnt aPCLast=aC3D->Value(aLast);
    aDist=aPv.Distance(aPCLast);
    if (aDist < aTolSum) {
      aT=aLast;
      //
      if(aDist > aTolV) {
        Extrema_LocateExtPC anExt(aPv, aGAC, aLast, 1.e-10);
        
        if(anExt.IsDone()) {
          Extrema_POnCurv aPOncurve = anExt.Point();
          aT = aPOncurve.Parameter();
          
          if((aT < (aLast + aFirst) * 0.5) ||
             (aPv.Distance(aPOncurve.Value()) > aTolSum) ||
             (aPCLast.Distance(aPOncurve.Value()) < Precision::Confusion()))
            aT = aLast;
        }
      }
      //
      return Standard_True;
    }
  }
  //
  GeomAPI_ProjectPointOnCurve& aProjector=ProjPT(aC3D);
  aProjector.Perform(aPv);
  
  aNbProj=aProjector.NbPoints();
  if (!aNbProj) {
    Handle(Geom_BoundedCurve) aBC=
      Handle(Geom_BoundedCurve)::DownCast(aC3D);
    if (!aBC.IsNull()) {
      gp_Pnt aPStart=aBC->StartPoint();
      gp_Pnt aPEnd  =aBC->EndPoint();
      
      aDist=aPv.Distance(aPStart);
      if (aDist < aTolSum) {
        aT=aFirst;
        return Standard_True;
      }
      
      aDist=aPv.Distance(aPEnd);
      if (aDist < aTolSum) {
        aT=aLast;
        return Standard_True;
      }
    }
    
    return Standard_False;
  }
  
  aDist=aProjector.LowerDistance();
  
  if (aDist > aTolSum) {
    return Standard_False;
  }

  aT=aProjector.LowerDistanceParameter();

  return Standard_True; 
}
//=======================================================================
//function : ProjectPointOnEdge
//purpose  : 
//=======================================================================
  Standard_Boolean BOPInt_Context::ProjectPointOnEdge(const gp_Pnt& aP,
                                                      const TopoDS_Edge& anEdge,
                                                      Standard_Real& aT)
{
  Standard_Integer aNbPoints;

  GeomAPI_ProjectPointOnCurve& aProjector=ProjPC(anEdge);
  aProjector.Perform(aP);

  aNbPoints=aProjector.NbPoints();
  if (aNbPoints) {
    aT=aProjector.LowerDistanceParameter();
    return Standard_True;
  }
  return Standard_False;
}

//modified by NIZHNY-EMV Tue Apr 12 09:54:56 2011
//=======================================================================
//function : StatePointFace
//purpose  : 
//=======================================================================
 TopAbs_State BOPInt_Context::StatePointFace(const TopoDS_Face& aF,
                                             const gp_Pnt& aP)
{
  Standard_Real u, v;
  TopAbs_State aState = TopAbs_OUT;
  GeomAPI_ProjectPointOnSurf& aProjector=ProjPS(aF);
  aProjector.Perform(aP);
  if (!aProjector.IsDone()) {
    return aState;
  }

  aProjector.LowerDistanceParameters(u,v);
  gp_Pnt2d aP2d(u,v);
  
  //
  aState = StatePointFace(aF, aP2d);
  return aState;
}
//modified by NIZHNY-EMV Tue Apr 12 10:01:04 2011
