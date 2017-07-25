// Created on: 1993-10-11
// Created by: Christophe MARION
// Copyright (c) 1993-1999 Matra Datavision
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


#include <BRep_Builder.hxx>
#include <HLRAlgo_EdgeIterator.hxx>
#include <HLRBRep.hxx>
#include <HLRBRep_Algo.hxx>
#include <HLRBRep_Data.hxx>
#include <HLRBRep_EdgeData.hxx>
#include <HLRBRep_HLRToShape.hxx>
#include <HLRBRep_ShapeBounds.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopTools_IndexedMapOfShape.hxx>

#include <BRepLib.hxx>
#include <BRepLib_MakeVertex.hxx>
#include <BRepTools.hxx>
#include <TopExp.hxx>
#include <IntTools_FClass2d.hxx>
#include <BRepTools_ReShape.hxx>
#include <Poly_MakeLoops.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <NCollection_UBTreeFiller.hxx>
#include <BndLib_Add2dCurve.hxx>
#include <Bnd_Box2d.hxx>
#include <BRepAdaptor_Curve2d.hxx>
#include <BRepGProp.hxx>
#include <GProp_GProps.hxx>
#include <NCollection_Handle.hxx>
#include <BRepBndLib.hxx>
#include <BOPTools_AlgoTools3D.hxx>
#include <IntTools_Context.hxx>
#include <BRepAlgoAPI_BuilderAlgo.hxx>
#include <NCollection_UBTree.hxx>
#include <Extrema_POnCurv2d.hxx>
#include <IntCurvesFace_ShapeIntersector.hxx>
#include <Extrema_ExtPC2d.hxx>
#include <HLRTopoBRep_OutLiner.hxx>
#include <Geom_Plane.hxx>
#include <BRepBndLib.hxx>
#include <TopTools_DataMapOfShapeReal.hxx>
#include <algorithm>
//=======================================================================
//function : HLRBRep_HLRToShape
//purpose  : 
//=======================================================================
HLRBRep_HLRToShape::HLRBRep_HLRToShape (const Handle(HLRBRep_Algo)& A) :
myAlgo(A)
{}

//=======================================================================
//function : InternalCompound
//purpose  : 
//=======================================================================

TopoDS_Shape 
HLRBRep_HLRToShape::InternalCompound (const Standard_Integer typ,
				      const Standard_Boolean visible,
				      const TopoDS_Shape& S,
                                      const Standard_Boolean In3d)
{
  Standard_Boolean added = Standard_False;
  TopoDS_Shape Result;
  Handle(HLRBRep_Data) DS = myAlgo->DataStructure();

  if (!DS.IsNull()) {
    DS->Projector().Scaled(Standard_True);
    Standard_Integer e1 = 1;
    Standard_Integer e2 = DS->NbEdges();
    Standard_Integer f1 = 1;
    Standard_Integer f2 = DS->NbFaces();
    Standard_Boolean explor = Standard_False;
//    Standard_Boolean todraw;
    if (!S.IsNull()) {
      Standard_Integer v1,v2;
      Standard_Integer index = myAlgo->Index(S);
      if (index == 0) explor = Standard_True;
      else            myAlgo->ShapeBounds(index).Bounds(v1,v2,e1,e2,f1,f2);
    }
    BRep_Builder B;
    B.MakeCompound(TopoDS::Compound(Result));
    HLRBRep_EdgeData* ed = &(DS->EDataArray().ChangeValue(e1 - 1));
    
    for (Standard_Integer ie = e1; ie <= e2; ie++) {
      ed++;
      if (ed->Selected() && !ed->Vertical()) {
	ed->Used(Standard_False);
	ed->HideCount(0);
      }
      else ed->Used(Standard_True);
    }
    if (explor) {
      TopTools_IndexedMapOfShape& Edges = DS->EdgeMap();
      TopTools_IndexedMapOfShape& Faces = DS->FaceMap();
      TopExp_Explorer Exp;
      
      for (Exp.Init (S, TopAbs_FACE);
	   Exp.More();
	   Exp.Next()) {
	Standard_Integer iface = Faces.FindIndex(Exp.Current());
	if (iface != 0)
          DrawFace(visible,typ,iface,DS,Result,added,In3d);
      }
      if (typ >= 3) {

	for (Exp.Init (S, TopAbs_EDGE, TopAbs_FACE);
	     Exp.More();
	     Exp.Next()) {
	  Standard_Integer ie = Edges.FindIndex(Exp.Current());
	  if (ie != 0) {
	    HLRBRep_EdgeData& EDataIE = DS->EDataArray().ChangeValue(ie);
	    if (!EDataIE.Used()) {
	      DrawEdge(visible,Standard_False,typ, EDataIE,Result,added,In3d);
	      EDataIE.Used(Standard_True);
	    }
	  }
	}
      }
    }
    else {

      for (Standard_Integer iface = f1; iface <= f2; iface++)
	DrawFace(visible,typ,iface,DS,Result,added,In3d);

      if (typ >= 3) {
	HLRBRep_EdgeData* EDataE11 = &(DS->EDataArray().ChangeValue(e1 - 1));
	
	for (Standard_Integer ie = e1; ie <= e2; ie++) {
          EDataE11++;
	  if (!EDataE11->Used()) {
	    DrawEdge(visible,Standard_False,typ,*EDataE11,Result,added,In3d);
	    EDataE11->Used(Standard_True);
	  }
	}
      }
    }
    DS->Projector().Scaled(Standard_False);
  }
  if (!added) Result = TopoDS_Shape();
  return Result;
}

//=======================================================================
//function : DrawFace
//purpose  : 
//=======================================================================

void 
HLRBRep_HLRToShape::DrawFace (const Standard_Boolean visible,
			      const Standard_Integer typ,
			      const Standard_Integer iface,
			      Handle(HLRBRep_Data)& DS,
			      TopoDS_Shape& Result,
			      Standard_Boolean& added,
                              const Standard_Boolean In3d) const
{
  HLRBRep_FaceIterator Itf;

  for (Itf.InitEdge(DS->FDataArray().ChangeValue(iface));
       Itf.MoreEdge();
       Itf.NextEdge()) {               
    Standard_Integer ie = Itf.Edge();
    HLRBRep_EdgeData& edf = DS->EDataArray().ChangeValue(ie);
    if (!edf.Used()) {
      Standard_Boolean todraw;
      if      (typ == 1) todraw =  Itf.IsoLine();
      else if (typ == 2) //outlines
      {
        if (In3d)
          todraw = Itf.Internal() || Itf.OutLine();
        else
          todraw =  Itf.Internal();
      }
      else if (typ == 3) todraw =  edf.Rg1Line() &&
	!edf.RgNLine() && !Itf.OutLine();
      else if (typ == 4) todraw =  edf.RgNLine() && !Itf.OutLine();
      else               todraw =
	!(Itf.IsoLine()  ||
	  Itf.Internal() ||
	  (edf.Rg1Line() && !Itf.OutLine()));

       if (todraw) {
         DrawEdge(visible,Standard_True,typ,edf,Result,added,In3d);
	edf.Used(Standard_True);
      }
      else {
	if((typ > 4 || typ == 2) && //sharp or outlines
           (edf.Rg1Line() && !Itf.OutLine()))
        {
	  Standard_Integer hc = edf.HideCount();
	  if(hc > 0) {
	    edf.Used(Standard_True);
	  }
	  else {
	    ++hc;
	    edf.HideCount(hc); //to try with another face
	  }
	}
	else {
	  edf.Used(Standard_True);
	}
      }
    }
  }
}

//=======================================================================
//function : DrawEdge
//purpose  : 
//=======================================================================

void 
HLRBRep_HLRToShape::DrawEdge (const Standard_Boolean visible,
			      const Standard_Boolean inFace,
			      const Standard_Integer typ,
			      HLRBRep_EdgeData& ed,
			      TopoDS_Shape& Result,
			      Standard_Boolean& added,
                              const Standard_Boolean In3d) const
{
  Standard_Boolean todraw = Standard_False;
  if      (inFace)   todraw = Standard_True;
  else if (typ == 3) todraw = ed.Rg1Line() && !ed.RgNLine();
  else if (typ == 4) todraw = ed.RgNLine();
  else               todraw =!ed.Rg1Line();

  if (todraw) {
    Standard_Real sta,end;
    Standard_ShortReal tolsta,tolend;
    BRep_Builder B;
    TopoDS_Edge E;
    HLRAlgo_EdgeIterator It;
    if (visible)
    {
      for (It.InitVisible(ed.Status()); It.MoreVisible(); It.NextVisible()) {
        It.Visible(sta,tolsta,end,tolend);
        if (!In3d)
          E = HLRBRep::MakeEdge(ed.Geometry(),sta,end);
        else
          E = HLRBRep::MakeEdge3d(ed.Geometry(),sta,end);
        if (!E.IsNull())
        {
          B.Add(Result,E);
          added = Standard_True;
        }
      }
    }
    else
    {
      for (It.InitHidden(ed.Status()); It.MoreHidden(); It.NextHidden()) {
        It.Hidden(sta,tolsta,end,tolend);
        if (!In3d)
          E = HLRBRep::MakeEdge(ed.Geometry(),sta,end);
        else
          E = HLRBRep::MakeEdge3d(ed.Geometry(),sta,end);
        if (!E.IsNull())
        {
          B.Add(Result,E);
          added = Standard_True;
        }
      }
    }
  }
}

//UBTree edge-point 
typedef NCollection_UBTree <Standard_Integer, Bnd_Box2d> HLRVV_BndBoxTree;

class HLRVV_BndBoxTreeSelector : public HLRVV_BndBoxTree::Selector
{
public:
  HLRVV_BndBoxTreeSelector( const TopTools_IndexedMapOfShape& theInpShapes,
    double theMinDDist) 
    : myInpShapes(theInpShapes), 
    myMinDDist(theMinDDist),
    myResId (-1),
    myCId (-1)
  {}

  Standard_Boolean Reject (const Bnd_Box2d& theBox) const
  {
    double aCurDDist = theBox.SquareDistance(myCPnt);
    if (aCurDDist > myMinDDist)
      return true;
    else
      return false;
  }

  Standard_Boolean Accept (const Standard_Integer& theObj)
  {
    //extrema pnt <-> vertex
    const TopoDS_Shape& aSh = myInpShapes(theObj);
    if (theObj == myCId)
      return Standard_False;
    gp_Pnt aCVP = BRep_Tool::Pnt(TopoDS::Vertex(aSh));
    gp_Pnt2d aCVP2d(aCVP.X(), aCVP.Y());
    double aDD = aCVP2d.SquareDistance(myCPnt);
    if (aDD < myMinDDist)
    {
      myMinDDist = aDD;
      myResId = theObj;
    }

    return Standard_True;
  }

  void Reset(double theMinDDist)
  {
    myMinDDist = theMinDDist;
    myResId = -1;
    myCId = -1; 
  }

  void SetCurrentPoint( int id ) 
  {
    gp_Pnt CP = BRep_Tool::Pnt(TopoDS::Vertex(myInpShapes(id)));
    myCPnt.SetX(CP.X());
    myCPnt.SetY(CP.Y());
    myCId = id;
  }

  double GetMinDDist() const
  {
    return myMinDDist;
  }

  int GetResVId() const
  {
    return myResId;
  }

private:
  HLRVV_BndBoxTreeSelector(const HLRVV_BndBoxTreeSelector& );
  HLRVV_BndBoxTreeSelector& operator=(const HLRVV_BndBoxTreeSelector& );

private:
  const TopTools_IndexedMapOfShape& myInpShapes;
  gp_Pnt2d myCPnt;
  double myMinDDist;
  int myResId;
  int myCId;
};

///

//UBTree edge-point 
typedef NCollection_UBTree <Standard_Integer, Bnd_Box2d> HLRVE_BndBoxTree;

class HLRVE_BndBoxTreeSelector : public HLRVE_BndBoxTree::Selector
{
public:
  HLRVE_BndBoxTreeSelector( 
    const NCollection_IndexedDataMap<TopoDS_Edge, BRepAdaptor_Curve2d, TopTools_ShapeMapHasher>& theEdgeAdapt,
    double theMinDDist) 
    : myEdgeAdapt (theEdgeAdapt), 
    myMinDDist(theMinDDist)
  {}

  Standard_Boolean Reject (const Bnd_Box2d& theBox) const
  {
    double aCurDDist = theBox.SquareDistance(myCPnt);
    if (aCurDDist > myMinDDist)
      return true;
    else
      return false;
  }

  Standard_Boolean Accept (const Standard_Integer& theObj)
  {
    const BRepAdaptor_Curve2d& adapt = myEdgeAdapt(theObj);
    Extrema_ExtPC2d aExtPC(myCPnt, adapt, adapt.FirstParameter(), adapt.LastParameter());
    if (aExtPC.IsDone() && aExtPC.NbExt() > 0 )
    {
      NCollection_Handle<Extrema_POnCurv2d> aPSol;// = aExtPC.Point(1);
      double aMinDD = Precision::Infinite(); 
      double aFp = adapt.FirstParameter();
      double aLp = adapt.LastParameter();
      for ( Standard_Integer i = 1; i <= aExtPC.NbExt(); i++) 
      {
        double aCDD = aExtPC.SquareDistance(i);
        Extrema_POnCurv2d aCurSol = aExtPC.Point(i);
        if (Abs(aCurSol.Parameter() - aFp) < Precision::Confusion() || 
          Abs(aCurSol.Parameter() - aLp) < Precision::Confusion()) //TODO it's better to use tolerance of vertex here
          continue;
        if ( aCDD < aMinDD) 
        {
          aMinDD = aCDD;
          aPSol = new Extrema_POnCurv2d(aExtPC.Point(i));
        }
      }
      if (aMinDD < myMinDDist)
      {
        myMinDDist = aMinDD;
        myEARes = adapt;
        if (!myPSolRes)
          myPSolRes = new Extrema_POnCurv2d( *aPSol );
        else
          myPSolRes->SetValues(aPSol->Parameter(), aPSol->Value());
      }
    }

    return Standard_True;
  }

  void Reset(double theMinDDist)
  {
    myMinDDist = theMinDDist;
    myPSolRes.Nullify();
  }

  void SetCurrentPoint( const gp_Pnt& thePnt ) 
  {
    myCPnt.SetX(thePnt.X());
    myCPnt.SetY(thePnt.Y());
  }

  bool GetPResult(Extrema_POnCurv2d& thePSolRes, BRepAdaptor_Curve2d& theERes) const
  {
    if (!myPSolRes.IsNull())
    {
      thePSolRes = *myPSolRes;
      theERes = myEARes;
      return true;
    }
    else
      return false;
  }

private:
  HLRVE_BndBoxTreeSelector(const HLRVE_BndBoxTreeSelector& );
  HLRVE_BndBoxTreeSelector& operator=(const HLRVE_BndBoxTreeSelector& );

private:
  const NCollection_IndexedDataMap<TopoDS_Edge, BRepAdaptor_Curve2d, TopTools_ShapeMapHasher>& myEdgeAdapt;
  gp_Pnt2d myCPnt;
  double myMinDDist;
  NCollection_Handle<Extrema_POnCurv2d> myPSolRes;
  BRepAdaptor_Curve2d myEARes;
};


//! deriatives of the edges on both ends
struct BRepFill_TangentLinkInfo
{
  gp_Dir2d myD1L;
  gp_Dir2d myD1F;
};

class BRepFill_PolyHelper : public Poly_MakeLoops2D::Helper
{
public:
  BRepFill_PolyHelper( const NCollection_DataMap<Poly_MakeLoops2D::Link, BRepFill_TangentLinkInfo>& mL2TI,
    const NCollection_DataMap<Standard_Integer, Poly_MakeLoops2D::ListOfLink>& themNode2ListOfLinks) : 
  myL2TI (mL2TI),
    mymNode2ListOfLinks (themNode2ListOfLinks)
  {

  };

  virtual const Poly_MakeLoops2D::ListOfLink& GetAdjacentLinks (Standard_Integer theNode) const
  {
    return mymNode2ListOfLinks(theNode);
  }

  virtual Standard_Boolean GetFirstTangent(const Poly_MakeLoops2D::Link& theLink, gp_Dir2d& theDir) const
  {
    if (theLink.flags & Poly_MakeLoops2D::LF_Reversed)
      theDir = -myL2TI(theLink).myD1L;
    else
      theDir = myL2TI(theLink).myD1F;
    return Standard_True;
  }

  virtual Standard_Boolean GetLastTangent(const Poly_MakeLoops2D::Link& theLink, gp_Dir2d& theDir) const
  {    
    if (theLink.flags & Poly_MakeLoops2D::LF_Reversed)
      theDir = -myL2TI(theLink).myD1F;
    else
      theDir = myL2TI(theLink).myD1L;
    return Standard_True;
  }
private:

  BRepFill_PolyHelper& operator= (const BRepFill_PolyHelper &theOther);

private:
  const NCollection_DataMap<Poly_MakeLoops2D::Link, BRepFill_TangentLinkInfo>& myL2TI;
  const NCollection_DataMap<Standard_Integer, Poly_MakeLoops2D::ListOfLink>& mymNode2ListOfLinks;

};

typedef NCollection_UBTree <Standard_Integer, Bnd_Box> HLRFL_BndBoxTree;

class HLRFL_BndBoxTreeSelector : public HLRFL_BndBoxTree::Selector
{
public:

  struct FaceParam
  {
    TopoDS_Face myF;
    double LPar;
  };

  HLRFL_BndBoxTreeSelector(const TopTools_IndexedMapOfShape& theFMap, bool theMode) 
    : HLRFL_BndBoxTreeSelector::Selector(), 
    myFMap (theFMap),
    myIntersectors(1, theFMap.Size()),
    myMode (theMode)
  {
    for (int i =1; i <= myIntersectors.Size(); i++)
      myIntersectors(i) = NULL;
  }

  ~HLRFL_BndBoxTreeSelector()
  {
    for (int i =1; i <= myIntersectors.Size(); i++)
    {
      IntCurvesFace_ShapeIntersector* an_inter = myIntersectors(i);
      if (an_inter)
        delete an_inter;
    }
  }


  Standard_Boolean Reject (const Bnd_Box& theBox) const
  {
    return theBox.IsOut (myLine);
  }

  Standard_Boolean Accept (const Standard_Integer& theObj)
  {
    if (theObj > myFMap.Extent())
      return Standard_False;
    const TopoDS_Face& F = TopoDS::Face(myFMap(theObj));
    IntCurvesFace_ShapeIntersector* an_inter = myIntersectors(theObj);
    if (!an_inter)
    {
      an_inter = new IntCurvesFace_ShapeIntersector();
      an_inter->Load(F, Precision::Confusion());
    }

    an_inter->Perform(myLine, -Precision::Infinite(), Precision::Infinite()); //TODO trim the line according to bnd box of face 
    if (an_inter->IsDone() && an_inter->NbPnt())
    {
      for (int i=1; i <= an_inter->NbPnt(); i++)
      {
        FaceParam fp;
        fp.LPar = an_inter->WParameter(i);
        fp.myF = an_inter->Face(i);
        myRParams.push_back(fp);
      }
      if (!myMode)
        myStop = 1;
    }
    return Standard_True;
    //int nbpnt = an_inter->.NbPnt();
  }
   
  void SetLine (const gp_Lin& theLine) 
  {
    myLine = theLine;
  }

  const std::vector<FaceParam>& GetRParams () const
  {
    return myRParams;
  }
  void ResetResult()
  {
    myRParams.clear();
  }

private:
  HLRFL_BndBoxTreeSelector(const HLRFL_BndBoxTreeSelector& );
  HLRFL_BndBoxTreeSelector& operator=(const HLRFL_BndBoxTreeSelector& );

private:
  const TopTools_IndexedMapOfShape& myFMap;
  gp_Lin myLine;
  NCollection_Array1<IntCurvesFace_ShapeIntersector*> myIntersectors;
  std::vector<FaceParam> myRParams;
  bool myMode; //if true => find all line-face intersections; otherwise - find any and stop the search 
};


static bool FuseVE(TopoDS_Shape& theRes)
{
  TopTools_IndexedMapOfShape allE;
  TopExp::MapShapes(theRes, TopAbs_EDGE, allE);
  TopTools_ListOfShape aLSE;
  for (int i = 1; i <= allE.Extent(); i++ )
    aLSE.Append(allE(i));

  Standard_Real aFV = 0;
  BRepAlgoAPI_BuilderAlgo aBlr;
  aBlr.SetArguments(aLSE);
  aBlr.SetRunParallel(Standard_True);
  aBlr.SetFuzzyValue(aFV);
  aBlr.Build(); 
  if (aBlr.HasErrors()) 
  {
    aBlr.DumpErrors(cout);
    return false;
  }
  theRes = aBlr.Shape();
  return true;
}

static void ProcessHVertices(TopoDS_Shape& theRes,
  NCollection_IndexedDataMap<TopoDS_Edge, BRepAdaptor_Curve2d, TopTools_ShapeMapHasher>& theEdAd,
  NCollection_DataMap<TopoDS_Edge, Bnd_Box2d, TopTools_ShapeMapHasher>& theEdBnd2d )
{
  BRepTools_ReShape reshaper;  
  ///hanging vertices => find closely located point on some edge or vertex
  TopoDS_Face aDraftPrFace;
  BRep_Builder().MakeFace(aDraftPrFace, BRepLib::Plane(), Precision::Confusion());
  TopTools_IndexedDataMapOfShapeListOfShape M;
  TopExp::MapShapesAndAncestors(theRes, TopAbs_VERTEX, TopAbs_EDGE, M);
  TopTools_IndexedMapOfShape HV; 
  for (int i = 1; i <= M.Extent(); i++)
    if (M(i).Extent() == 1)
      HV.Add(M.FindKey(i));
  //
  TopTools_IndexedMapOfShape VV, EE;
  TopExp::MapShapes(theRes, TopAbs_VERTEX, VV);
  TopExp::MapShapes(theRes, TopAbs_EDGE, EE);
  //
  //vertex-vertex extrema
  NCollection_UBTree <Standard_Integer, Bnd_Box2d> aTreeVV;
  NCollection_UBTreeFiller <Standard_Integer, Bnd_Box2d> aTreeFillerVV (aTreeVV);
  for (Standard_Integer i = 1; i <= VV.Extent(); i++)
  { 
    const TopoDS_Vertex& aV = TopoDS::Vertex(VV(i));
    Bnd_Box2d aBB;
    gp_Pnt aPV = BRep_Tool::Pnt(aV);
    aBB.Add(gp_Pnt2d(aPV.X(), aPV.Y()));
    aBB.Enlarge(BRep_Tool::Tolerance(aV));
    aTreeFillerVV.Add(i, aBB);
  }

  aTreeFillerVV.Fill();
  HLRVV_BndBoxTreeSelector aSelectorVV(VV, Precision::Infinite());  

  //vertex-edge extrema
  NCollection_UBTree <Standard_Integer, Bnd_Box2d> aTreeVE;
  NCollection_UBTreeFiller <Standard_Integer, Bnd_Box2d> aTreeFillerVE (aTreeVE);
  int nbEE = EE.Extent();
  //NCollection_Array1<BRepAdaptor_Curve2d> anEdgeAdapters(1, nbEE);
  for (Standard_Integer i = 1; i <= nbEE; i++)
  { 
    const TopoDS_Edge& anE = TopoDS::Edge(EE(i));
    Bnd_Box2d aBB;
    theEdAd.Add(anE, BRepAdaptor_Curve2d(anE, aDraftPrFace));
    BndLib_Add2dCurve::Add(*theEdAd.Seek(anE), 0, aBB);
    theEdBnd2d.Bind(anE, aBB);
    aTreeFillerVE.Add(i, aBB);
  }

  aTreeFillerVE.Fill();
  HLRVE_BndBoxTreeSelector aSelectorVE(theEdAd, Precision::Infinite());      //min dist doesn't matter here   
  //

  TopTools_IndexedMapOfShape toadd, torem;//temp
  for (int i = 1; i <= HV.Extent(); i++ )
  {
    const TopoDS_Vertex& CHV = TopoDS::Vertex(HV(i));
    gp_Pnt CHVPnt =  BRep_Tool::Pnt(CHV);
    //
    int id = VV.FindIndex(CHV);
    aSelectorVV.Reset(Precision::Infinite());
    aSelectorVV.SetCurrentPoint( id );
    /*int resVV =*/ aTreeVV.Select(aSelectorVV);
    double aMinDDistReached = aSelectorVV.GetMinDDist();
    TopoDS_Vertex aResV = TopoDS::Vertex(VV(aSelectorVV.GetResVId()));
    //          
    aSelectorVE.Reset(aMinDDistReached);
    aSelectorVE.SetCurrentPoint( CHVPnt );
    /*int resVE = */aTreeVE.Select(aSelectorVE);

    Extrema_POnCurv2d aPSolRes;
    BRepAdaptor_Curve2d aResEA;
    bool VEFound = aSelectorVE.GetPResult(aPSolRes, aResEA);
    if (VEFound)
    {
      ///if (BRep_Tool::Degenerated(E)) //can be degenerted??
      double aPar = aPSolRes.Parameter();
      const TopoDS_Edge& aResE = aResEA.Edge();
      TopoDS_Edge aDE1 = TopoDS::Edge(aResE.EmptyCopied().Oriented(TopAbs_FORWARD));
      TopoDS_Edge aDE2 = TopoDS::Edge(aResE.EmptyCopied().Oriented(TopAbs_FORWARD));

      TopoDS_Vertex aFV, aLV;
      TopExp::Vertices(aResE, aFV, aLV);
      double aFP = aResEA.FirstParameter();
      double aLP = aResEA.LastParameter();
      gp_Pnt2d aResP2d = aResEA.Value(aPar);
      gp_Pnt aResP3d(aResP2d.X(), aResP2d.Y(), 0.0);
      TopoDS_Vertex aDummyV = BRepLib_MakeVertex(aResP3d).Vertex();
      //
      NCollection_List<TopoDS_Shape> nls;
      nls.Append(aDummyV); 
      nls.Append(CHV); 
      gp_Pnt aNC;
      double aNTol;
      BRepLib::BoundingVertex(nls, aNC, aNTol);

      gp_Pnt aFVP = BRep_Tool::Pnt(aFV);
      gp_Pnt aLVP = BRep_Tool::Pnt(aLV);
      if (aFVP.Distance(aNC) < aNTol*1e10 ||
        aLVP.Distance(aNC) < aNTol*1e10)
      {
        nls.RemoveFirst();
        nls.Append(aResV); 
        BRepLib::BoundingVertex(nls, aNC, aNTol);
        BRep_Builder().UpdateVertex(aResV, aNC, aNTol);
        reshaper.Replace(CHV, aResV.Oriented(CHV.Orientation()));
        if (HV.Contains(aResV)) //if the closest vertex lays on edge => never contains in HV
          HV.RemoveKey(aResV);
      }
      else
      {
        BRep_Builder().UpdateVertex(CHV, aNC, aNTol);
        BRep_Builder().Range(aDE1, aFP, aPar);
        BRep_Builder().Add(aDE1, aFV.Oriented(TopAbs_FORWARD));
        BRep_Builder().Add(aDE1, CHV.Oriented(TopAbs_REVERSED));
        //
        BRep_Builder().Range(aDE2, aPar, aLP );
        BRep_Builder().Add(aDE2, CHV.Oriented(TopAbs_FORWARD));
        BRep_Builder().Add(aDE2, aLV.Oriented(TopAbs_REVERSED));
        //

        ///TODO temp
        torem.Add(aResE);
        toadd.Add(aDE1);
        toadd.Add(aDE2);
        //TopoDS_Compound aResCmp;
        //BRep_Builder().MakeCompound(aResCmp);
        //BRep_Builder().Add(aResCmp, aDE1);
        //BRep_Builder().Add(aResCmp, aDE2);
        //
        //TopoDS_Wire aResW;
        //BRep_Builder().MakeWire(aResW);
        //BRep_Builder().Add(aResW, aDE1);
        //BRep_Builder().Add(aResW, aDE2);
        //TopoDS_Wire aResW = BRepBuilderAPI_MakeWire(aDE1, aDE2).Wire(); //this will control proper sharing and tolerance matching between vertices
        /*{ //DEB
        TopTools_IndexedMapOfShape VVVV;
        TopExp::MapShapes(aResW, TopAbs_VERTEX, VVVV);
        //cout << VVVV.Extent() << endl;
        }*/
        //aResW.Orientation(aResE.Orientation());
        //reshape.Replace(aResE, aResCmp);
      }
    }
    else
    {
      NCollection_List<TopoDS_Shape> nls; //TODO move to ext func
      nls.Append(CHV); 
      nls.Append(aResV); 
      gp_Pnt aNC;
      double aNTol;
      BRepLib::BoundingVertex(nls, aNC, aNTol);
      BRep_Builder().UpdateVertex(aResV, aNC, aNTol);
      reshaper.Replace(CHV, aResV.Oriented(CHV.Orientation()));
      if (HV.Contains(aResV)) //if the closest vertex lays on edge => never contains in HV
        HV.RemoveKey(aResV);
    }
  }
  //DEB
  for (int i =1; i<= torem.Extent() ;i++)
    BRep_Builder().Remove(theRes, torem(i));
  //DEB
  for (int i =1; i<= toadd.Extent() ;i++)
    BRep_Builder().Add(theRes, toadd(i));
  //
  theRes = reshaper.Apply(theRes); //replace only vertices
}

static void FillNode2LLMap(Standard_Integer NewNode, const Poly_MakeLoops2D::Link& NewLink, 
  NCollection_DataMap<Standard_Integer, Poly_MakeLoops2D::ListOfLink>& mNode2ListOfLinks)
{
  if (!mNode2ListOfLinks.IsBound(NewNode))
    mNode2ListOfLinks.Bound(NewNode, Poly_MakeLoops2D::ListOfLink())->Append(NewLink);
  else
    mNode2ListOfLinks(NewNode).Append(NewLink);
}

static TopAbs_State GetStateOfSamplePoint(const TopoDS_Face& ff2, 
  const IntTools_FClass2d& fclass,
  const Bnd_Box2d& fbnd)
{     
  TopExp_Explorer exp(ff2, TopAbs_EDGE); //should contains at least one edge
  TopoDS_Edge fe2 = TopoDS::Edge(exp.Current());
  BRepAdaptor_Curve2d afe2(fe2, ff2);
  gp_Pnt2d middlepoint = afe2.Value((afe2.LastParameter() + afe2.FirstParameter()) / 2.0);
  Bnd_Box bb3d;
  BRepBndLib::Add(ff2, bb3d);
  Bnd_Box2d bb2d;
  bb2d.Set(gp_Pnt2d(bb3d.CornerMin().X(), bb3d.CornerMin().Y())); 
  bb2d.Add(gp_Pnt2d(bb3d.CornerMax().X(), bb3d.CornerMax().Y())); 
  if (bb2d.IsOut(middlepoint))
    return TopAbs_OUT;
  else
    return fclass.Perform(middlepoint);
}

bool HLRBRep_ParComp(const HLRFL_BndBoxTreeSelector::FaceParam& a, 
  const HLRFL_BndBoxTreeSelector::FaceParam& b)
{
  return a.LPar < b.LPar;
}

static TopoDS_Shape ProcessLoops(const TopoDS_Shape& theRes,
  NCollection_IndexedDataMap<TopoDS_Edge, BRepAdaptor_Curve2d, TopTools_ShapeMapHasher>& anEdAd,
  NCollection_DataMap<TopoDS_Edge, Bnd_Box2d, TopTools_ShapeMapHasher> theEdBnd2d,
  HLRAlgo_Projector& projector,
  const TopTools_ListOfShape& anOrigShapes,
  bool theMode, 
  TopTools_DataMapOfShapeShape& OrigFaceToProjFace )
{  
  NCollection_List<TopoDS_Wire> theLoops;
  NCollection_List<TopoDS_Wire> SelfLoops;
  NCollection_DataMap<TopoDS_Shape, Bnd_Box2d, TopTools_ShapeMapHasher> aShToBnd2d;
  TopoDS_Face aDraftPrFace;
  BRep_Builder().MakeFace(aDraftPrFace, BRepLib::Plane(), Precision::Confusion());
  TopTools_IndexedMapOfShape mN2V;
  TopExp::MapShapes(theRes, TopAbs_VERTEX, mN2V);
  NCollection_DataMap<Standard_Integer, Poly_MakeLoops2D::ListOfLink> mNode2ListOfLinks;
  NCollection_DataMap<Poly_MakeLoops2D::Link, BRepFill_TangentLinkInfo> mL2TI;
  //Create links for Poly_MakeLoops algo and bind them to the existing edges
  NCollection_DataMap<Poly_MakeLoops2D::Link, TopoDS_Edge> mL2E;
  Standard_Integer LastIndV = mN2V.Extent();
  TopExp_Explorer ExpE;
  ExpE.Init(theRes, TopAbs_EDGE);
  for (; ExpE.More(); ExpE.Next())
  {
    const TopoDS_Edge& E = TopoDS::Edge(ExpE.Current());
    // If edge contains only one vertex => self loop (should be non degenerated)
    TopoDS_Vertex FV, LV;
    FV = TopExp::FirstVertex(E, Standard_True); 
    LV = TopExp::LastVertex(E, Standard_True); 
    if (FV.IsSame(LV))
    {
      if (!BRep_Tool::Degenerated(E))
      {
        const TopoDS_Wire& aW = BRepLib_MakeWire(E).Wire();
        SelfLoops.Append(aW);
        Bnd_Box2d aBB;        
        BndLib_Add2dCurve::Add(BRepAdaptor_Curve2d(E, aDraftPrFace), 0, aBB);
        aShToBnd2d.Bind(aW, aBB);
      }
      continue;
    }

    const BRepAdaptor_Curve2d* anAd = anEdAd.Seek(E);
    if (!anAd)
    {
      anEdAd.Add(E, BRepAdaptor_Curve2d(E, aDraftPrFace));
      anAd = anEdAd.Seek(E);
      Bnd_Box2d aBB;
      BndLib_Add2dCurve::Add(*anEdAd.Seek(E), 0, aBB);
      theEdBnd2d.Bind(E, aBB);
    }
    //NCollection_Handle<BRepAdaptor_Curve2d> adapt = new BRepAdaptor_Curve2d(E, aDraftPrFace);

    gp_Pnt2d Pnt;
    gp_Vec2d Vec;
    gp_Dir2d D1F, D1L;
    if (anAd->Curve().IsNull())
      continue;
    TopAbs_Orientation EOri = E.Orientation();
    if (EOri == TopAbs_FORWARD)
    {
      anAd->D1(anAd->FirstParameter(), Pnt, Vec);
      if (Vec.SquareMagnitude() <= gp::Resolution())
        anAd->D1(anAd->FirstParameter() + Precision::Confusion(), Pnt, Vec);
      D1F.SetCoord(Vec.X(), Vec.Y());
      anAd->D1(anAd->LastParameter(), Pnt, Vec);
      if (Vec.SquareMagnitude() <= gp::Resolution())
        anAd->D1(anAd->LastParameter() - Precision::Confusion(), Pnt, Vec);
      D1L.SetCoord(Vec.X(), Vec.Y());
    }
    else if (EOri == TopAbs_REVERSED)
    {
      anAd->D1(anAd->LastParameter(), Pnt, Vec);
      if (Vec.SquareMagnitude() <= gp::Resolution())
        anAd->D1(anAd->LastParameter() - Precision::Confusion(), Pnt, Vec);
      D1F.SetCoord(-Vec.X(), -Vec.Y());
      anAd->D1(anAd->FirstParameter(), Pnt, Vec);
      if (Vec.SquareMagnitude() <= gp::Resolution())
        anAd->D1(anAd->FirstParameter() + Precision::Confusion(), Pnt, Vec);
      D1L.SetCoord(-Vec.X(), -Vec.Y());
    }
    else
      continue;

    Standard_Integer Node1 = mN2V.FindIndex(FV);
    Standard_Integer Node2 = mN2V.FindIndex(LV);

    Poly_MakeLoops2D::Link aLink(Node1, Node2);
    aLink.flags = Poly_MakeLoops2D::LF_Both;
    if (!mL2E.IsBound(aLink))
    {
      mL2E.Bind(aLink, E);
      FillNode2LLMap(Node1, aLink, mNode2ListOfLinks);
      FillNode2LLMap(Node2, aLink, mNode2ListOfLinks);
      BRepFill_TangentLinkInfo* Info = mL2TI.Bound(aLink, BRepFill_TangentLinkInfo());
      Info->myD1F = D1F;
      Info->myD1L = D1L;
    }
    else
    {
      //link have been met twise. This may indicate that it is has 
      //opposite direction now. Try to divide it (node1, node2) => (node1, ind) + (ind, node2)
      //so it can be passed to loop maker
      LastIndV++;
      Poly_MakeLoops2D::Link aLink1(Node1, LastIndV);
      aLink1.flags = Poly_MakeLoops2D::LF_Both;
      Poly_MakeLoops2D::Link aLink2(LastIndV, Node2);
      aLink2.flags = Poly_MakeLoops2D::LF_Both;
      mL2E.Bind(aLink1, E);
      mL2E.Bind(aLink2, TopoDS_Edge()); //indicates that one edge represented as two links

      FillNode2LLMap(Node1, aLink1, mNode2ListOfLinks);
      FillNode2LLMap(Node2, aLink2, mNode2ListOfLinks);
      FillNode2LLMap(LastIndV, aLink1, mNode2ListOfLinks);
      FillNode2LLMap(LastIndV, aLink2, mNode2ListOfLinks);

      BRepFill_TangentLinkInfo* Info1 = mL2TI.Bound(aLink1, BRepFill_TangentLinkInfo());
      Info1->myD1F = D1F;
      Info1->myD1L = gp_Dir2d(1., 1.);
      BRepFill_TangentLinkInfo* Info2 = mL2TI.Bound(aLink2, BRepFill_TangentLinkInfo());
      Info2->myD1F = gp_Dir2d(1., 1.);
      Info2->myD1L = D1L;
    }
  }

  //DEBUG: print D1F
  /*{
  NCollection_DataMap<Standard_Integer, Poly_MakeLoops2D::ListOfLink>::Iterator it(mNode2ListOfLinks);
  for (;it.More();it.Next())
  {
  int nK = it.Key();
  Poly_MakeLoops2D::ListOfLink links = it.Value();
  Poly_MakeLoops2D::ListOfLink::Iterator itl(links);
  cout << "next:" << endl;
  for (;itl.More();itl.Next())
  {
  BRepFill_TangentLinkInfo linfo = mL2TI(itl.Value());
  cout << linfo.myD1F.X() <<  "  " << linfo.myD1F.Y() << endl;
  TopoDS_Edge eee = mL2E(itl.Value());
  }
  }
  */
  //
  BRepFill_PolyHelper helper(mL2TI, mNode2ListOfLinks);
  Poly_MakeLoops2D aLoopMaker(1, &helper, NCollection_BaseAllocator::CommonBaseAllocator() );
  for (NCollection_DataMap<Poly_MakeLoops2D::Link, TopoDS_Edge>::Iterator aMapIt (mL2E); aMapIt.More(); aMapIt.Next())
    aLoopMaker.AddLink(aMapIt.Key());

  aLoopMaker.Perform();   //try to find loops
  Standard_Integer NbLoops = aLoopMaker.GetNbLoops();
  Standard_Integer NbHangs = aLoopMaker.GetNbHanging();

  cout << "NbLoops=" << NbLoops << endl;
  cout << "NbHangs=" << NbHangs << endl;

  // if (NbLoops == 0 || NbHangs != 0 )
  //   return Standard_False;
  
  theLoops.Append(SelfLoops);
  for (Standard_Integer i = 1; i <= NbLoops; i++)  //loops to wires
  {
    Poly_MakeLoops2D::Loop aLoop = aLoopMaker.GetLoop(i);
    Poly_MakeLoops2D::Loop::Iterator it(aLoop);
    BRepBuilderAPI_MakeWire aWM;
    Bnd_Box2d aWBox2d;
    TopoDS_Edge E;
    for (;it.More(); it.Next())
    {
      const Poly_MakeLoops2D::Link& Link = it.Value();
      E = mL2E(Link);
      if (Link.flags & Poly_MakeLoops2D::LF_Reversed)
        E.Reverse();
      //if null => probably this edge was passed as two links (based on the same edge); so skip this edge
      if (!E.IsNull()) 
      {
        aWM.Add(E);
        aWBox2d.Add(theEdBnd2d(E));
      }
    }
    if (aWM.IsDone())
    {
      TopoDS_Wire W = aWM.Wire();
      if (W.Closed())    
      {
        theLoops.Append(W);
        aShToBnd2d.Bind(W, aWBox2d);
      }
    }
  }

  //DEBUG
 /* Poly_MakeLoops2D::ListOfLink theHLinks;
  aLoopMaker.GetHangingLinks(theHLinks);
  Poly_MakeLoops2D::ListOfLink::Iterator itt(theHLinks);

  TopoDS_Compound cmpH;BRep_Builder().MakeCompound(cmpH);
  for (; itt.More();itt.Next())
  {
    Poly_MakeLoops2D::Link alink = itt.Value();
    const Poly_MakeLoops2D::Link& Link = itt.Value();
    TopoDS_Edge E = mL2E(Link);
    if (Link.flags & Poly_MakeLoops2D::LF_Reversed)
      E.Reverse();
    //if null => probably this edge was passed as two links (based on the same edge); so skip this edge
    if (!E.IsNull()) 
      BRep_Builder().Add(cmpH, E);
  }*/
  //

  IntTools_Context context1;// = new IntTools_Context();
  NCollection_List<TopoDS_Wire>::Iterator itL(theLoops);
  NCollection_IndexedDataMap<TopoDS_Shape, double> lf1, lf2;
  for (; itL.More(); itL.Next())
  {
    const TopoDS_Wire& aW = itL.Value();
    if (aW.IsNull())
      continue;
    TopoDS_Face ff;
    BRep_Builder().MakeFace(ff, BRepLib::Plane(), Precision::Confusion()); 
    //plane is static; pcurves of edges are already based on this plane
    BRep_Builder().Add(ff, aW);
    aShToBnd2d(aW).Enlarge(0.001);
    aShToBnd2d.Bind(ff, aShToBnd2d(aW)); //bndbox still the same
    GProp_GProps pr;
    BRepGProp::SurfaceProperties(ff, pr);
    Standard_Real CurMass = pr.Mass();
    if (Abs(CurMass) < 1e-10) //?? TODO 
      continue;
    if (CurMass >= 0)
      lf1.Add(ff, CurMass);
    else
      lf2.Add(ff, CurMass);
  }
  NCollection_IndexedDataMap<TopoDS_Face, NCollection_List<TopoDS_Face>> FaceToIntWires;
  for (int i = 1; i <= lf1.Size() && !lf2.IsEmpty(); i++)
  {
    //NCollection_Handle<BRepTopAdaptor_FClass2d> fclass;
    NCollection_Sequence<TopoDS_Face> intf;
    const TopoDS_Face& ff1 = TopoDS::Face(lf1.FindKey(i));
    for (int j=1;j<= lf2.Size();j++)
    {
      double pm = lf1.FindFromIndex(i);
      double nm = lf2.FindFromIndex(j);
      if (pm < -nm)
        continue;        
      const TopoDS_Face& ff2 = TopoDS::Face(lf2.FindKey(j));
      //if (!fclass)
      //  fclass = new BRepTopAdaptor_FClass2d(ff1, Precision::Confusion());
      IntTools_FClass2d& fclass = context1.FClass2d(ff1);
      Bnd_Box2d fbnd = aShToBnd2d(ff1);
      TopAbs_State st = GetStateOfSamplePoint(ff2, fclass, fbnd);
      if (st == TopAbs_IN)
        intf.Append(ff2);
    }
    NCollection_Map<int> intfInd;      
    for (int i = 1; i<= intf.Size();i++)
    {
      if (intfInd.Contains(i))
        continue;
      //BRepTopAdaptor_FClass2d fclassInt(intf(i), Precision::Confusion());
      IntTools_FClass2d& fclassInt = context1.FClass2d(intf(i));
      Bnd_Box2d fbndi = aShToBnd2d(intf(i)); 
      for (int j=1; j<= intf.Size();j++)
      {
        if (i==j)
          continue;
        if (intfInd.Contains(j))
          continue;
        TopAbs_State st = GetStateOfSamplePoint(intf(j), fclassInt, fbndi);
        if (st == TopAbs_OUT) //note that intf-faces are holes
          intfInd.Add(j);
      }
    }
    FaceToIntWires.Add(ff1, NCollection_List<TopoDS_Face>());
    NCollection_List<TopoDS_Face>& nl = FaceToIntWires.ChangeFromKey(ff1);

    for (int i = 1; i<= intf.Size();i++)
    {
      if (intfInd.Contains(i))
        continue;
      nl.Append(intf(i));
    }
  }

  TopoDS_Compound aResCmp;
  BRep_Builder().MakeCompound(aResCmp);

  TopTools_ListIteratorOfListOfShape itOrS(anOrigShapes);
  TopTools_IndexedMapOfShape anOrigFaces;  
  for (;itOrS.More();itOrS.Next())
    TopExp::MapShapes(itOrS.Value(), TopAbs_FACE, anOrigFaces);

  NCollection_UBTree <Standard_Integer, Bnd_Box> aTreeFL;
  NCollection_UBTreeFiller <Standard_Integer, Bnd_Box> aTreeFillerFL (aTreeFL);
  for (Standard_Integer i = 1; i <= anOrigFaces.Extent(); i++)
  { 
    const TopoDS_Face& aF = TopoDS::Face(anOrigFaces(i));
    Bnd_Box aBB;
    BRepBndLib::Add(aF, aBB);
    aTreeFillerFL.Add(i, aBB);
  }

  aTreeFillerFL.Fill();
  HLRFL_BndBoxTreeSelector aSelFL(anOrigFaces, theMode);

  for (int i = 1; i <= FaceToIntWires.Extent(); i++)
  {
    TopoDS_Face cf = FaceToIntWires.FindKey(i); 
    NCollection_List<TopoDS_Face>::Iterator it(FaceToIntWires.FindFromIndex(i));
    for (;it.More();it.Next())
    {
      TopoDS_Face ff = it.Value();
      TopoDS_Iterator it(ff);
      TopoDS_Shape int_wire = it.Value();
      BRep_Builder().Add(cf, int_wire);
    }
    gp_Pnt2d p2d;
    gp_Pnt p3d;
    TopExp_Explorer exp(cf, TopAbs_EDGE);
    Handle(IntTools_Context) context = new IntTools_Context();
    BOPTools_AlgoTools3D::PointNearEdge( TopoDS::Edge(exp.Current()), cf, p2d, p3d, context);
    gp_Lin shot_line = projector.Shoot(p2d.X(), p2d.Y());

    aSelFL.ResetResult();
    aSelFL.SetLine( shot_line );
    aTreeFL.Select(aSelFL);

    std::vector<HLRFL_BndBoxTreeSelector::FaceParam> fp = aSelFL.GetRParams();
    if (!fp.empty())
    {
      BRep_Builder().Add(aResCmp, cf);
      if (theMode)
      {
        std::sort(fp.begin(), fp.end(), HLRBRep_ParComp);
        OrigFaceToProjFace.Bind(cf, fp.front().myF);
      }
    }
  }
  //
  return aResCmp;
}

TopoDS_Shape HLRBRep_HLRToShape::CompoundOfFaces(bool theMode,
  TopTools_DataMapOfShapeShape& OrigFaceToProjFace)
{
  //
  TopTools_ListOfShape anOrigShapes;
  const HLRBRep_SeqOfShapeBounds& aSeqOfShBnds = myAlgo->SeqOfShapeBounds();
  for (int i=1; i<= aSeqOfShBnds.Size(); i++)
    anOrigShapes.Append(aSeqOfShBnds(i).Shape()->OriginalShape());
  //
  TopoDS_Compound aTotalOSh;
  BRep_Builder BB;
  BB.MakeCompound(aTotalOSh);

  TopoDS_Shape aSharpSh = InternalCompound(HLRBRep_Sharp, Standard_True, TopoDS_Shape(), Standard_False); //TODO check
  if (!aSharpSh.IsNull())
    BB.Add(aTotalOSh, aSharpSh);
  TopoDS_Shape anOutLSh = InternalCompound(HLRBRep_OutLine, Standard_True, TopoDS_Shape(), Standard_False);
  if (!anOutLSh.IsNull())
    BB.Add(aTotalOSh, anOutLSh);
  TopoDS_Shape aRg1Sh = InternalCompound(HLRBRep_Rg1Line, Standard_True, TopoDS_Shape(), Standard_False);
  if (!aRg1Sh.IsNull())
    BB.Add(aTotalOSh, aRg1Sh);

  //
  BRepLib::BuildCurves3d(aTotalOSh);
  BRepLib::SameParameter(aTotalOSh, Precision::PConfusion(),Standard_False);
  //
  if( !FuseVE (aTotalOSh))
    return TopoDS_Shape();
  //
  NCollection_DataMap<TopoDS_Edge, Bnd_Box2d, TopTools_ShapeMapHasher> theEdBnd2d;
  NCollection_IndexedDataMap<TopoDS_Edge, BRepAdaptor_Curve2d, TopTools_ShapeMapHasher> anEdAd; 
  ProcessHVertices(aTotalOSh, anEdAd, theEdBnd2d);
  //
  TopoDS_Shape aRes = ProcessLoops(aTotalOSh, anEdAd, theEdBnd2d, 
    myAlgo->Projector(), anOrigShapes, theMode, OrigFaceToProjFace);
  //
  return aRes;
}

