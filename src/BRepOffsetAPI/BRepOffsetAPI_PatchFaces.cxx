// Created on: 2012-08-06
// Created by: jgv@ROLEX
// Copyright (c) 2012-2014 OPEN CASCADE SAS
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


#include <BRepOffsetAPI_PatchFaces.hxx>
#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepAdaptor_Curve2d.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepAdaptor_HSurface.hxx>
#include <BRepTopAdaptor_TopolTool.hxx>
#include <LocalAnalysis_SurfaceContinuity.hxx>
#include <TopOpeBRepTool_TOOL.hxx>
#include <BRepLib.hxx>
#include <BRepLib_MakeVertex.hxx>
#include <BRepLib_MakeEdge.hxx>
#include <BRepTools.hxx>
#include <BRepTools_WireExplorer.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Iterator.hxx>
#include <IntTools_Context.hxx>
#include <BOPTools_AlgoTools.hxx>
#include <BRepOffset_Tool.hxx>
#include <ShapeAnalysis_Surface.hxx>
#include <ShapeConstruct_ProjectCurveOnSurface.hxx>
#include <BRepAlgoAPI_Section.hxx>
#include <Extrema_ExtPC.hxx>
#include <Extrema_ExtPC2d.hxx>
#include <Extrema_ExtCC2d.hxx>
//#include <Geom2dInt_GInter.hxx>
//#include <BRepExtrema_ExtCC.hxx>
//#include <ShapeFix_Shape.hxx>


static void IntersectIn2d(const TopoDS_Edge& theEdge1,
                          const TopoDS_Edge& theEdge2,
                          const TopoDS_Face& theFace,
                          const gp_Pnt2d&    theP2dRef,
                          Standard_Real&     theParamOnFirst,
                          Standard_Real&     theParamOnSecond,
                          gp_Pnt&            thePntSol,
                          gp_Pnt&            thePntOnSecond)
{
  BRepAdaptor_Curve BAprevcurve(theEdge1);
  BRepAdaptor_Curve BAcurcurve(theEdge2);
  BRepAdaptor_Curve2d BAprevcurve2d(theEdge1, theFace);
  BRepAdaptor_Curve2d BAcurcurve2d(theEdge2, theFace);
  //Geom2dInt_GInter Inters2d(BAprevcurve2d, BAcurcurve2d,
  //                        Precision::PConfusion(), Precision::PConfusion());
  Extrema_ExtCC2d ExtrCC(BAprevcurve2d, BAcurcurve2d);
  Standard_Real ParOnPrev[6], ParOnCur[6], dist[6], SumDist[6];
  gp_Pnt2d Pnt2d[6];
  ParOnPrev[1] = ParOnPrev[2] = BAprevcurve2d.FirstParameter();
  ParOnPrev[3] = ParOnPrev[4] = BAprevcurve2d.LastParameter();
  ParOnCur[1]  = ParOnCur[3]  = BAcurcurve2d.FirstParameter();
  ParOnCur[2]  = ParOnCur[4]  = BAcurcurve2d.LastParameter();
  
  ExtrCC.TrimmedSquareDistances(dist[1], dist[2], dist[3], dist[4],
                                Pnt2d[1], Pnt2d[2], Pnt2d[3], Pnt2d[4]);
  SumDist[1] = theP2dRef.SquareDistance(Pnt2d[1]) + theP2dRef.SquareDistance(Pnt2d[3]); //first-first
  SumDist[2] = theP2dRef.SquareDistance(Pnt2d[1]) + theP2dRef.SquareDistance(Pnt2d[4]); //first-last
  SumDist[3] = theP2dRef.SquareDistance(Pnt2d[2]) + theP2dRef.SquareDistance(Pnt2d[3]); //last-first
  SumDist[4] = theP2dRef.SquareDistance(Pnt2d[2]) + theP2dRef.SquareDistance(Pnt2d[4]); //last-last
  SumDist[5] = RealLast();
  Standard_Integer imin = 0;
  if (ExtrCC.IsDone())
  {
    for (Standard_Integer i = 1; i <= ExtrCC.NbExt(); i++)
    {
      Extrema_POnCurv2d P2d1, P2d2;
      ExtrCC.Points(i, P2d1, P2d2);
      Standard_Real aSumDist = theP2dRef.SquareDistance(P2d1.Value()) + theP2dRef.SquareDistance(P2d2.Value());
      if (aSumDist < SumDist[5])
      {
        SumDist[5] = aSumDist;
        imin = i;
      }
    }
    if (imin != 0)
    {
      Extrema_POnCurv2d P2d1min, P2d2min;
      ExtrCC.Points(imin, P2d1min, P2d2min);
      ParOnPrev[5] = P2d1min.Parameter();
      ParOnCur[5]  = P2d2min.Parameter();
    }
  }
  if (imin == 0)
  {
    imin = 1;
    for (Standard_Integer i = 2; i <= 4; i++)
      if (SumDist[i] < SumDist[imin])
        imin = i;
  }
  else
    imin = 5;
  
  theParamOnFirst  = ParOnPrev[imin];
  theParamOnSecond = ParOnCur[imin];
  gp_Pnt PntOnFirst = BAprevcurve.Value(theParamOnFirst);
  thePntOnSecond    = BAcurcurve.Value(theParamOnSecond);
  thePntSol.SetXYZ((PntOnFirst.XYZ() + thePntOnSecond.XYZ())/2);
}

static TopoDS_Edge MakeNewEdgeWithOldPcurvesOnNewSurfaces(const TopoDS_Edge& theEdge,
                                                          const TopoDS_Face& theOldFace1,
                                                          const TopoDS_Face& theNewFace1,
                                                          const TopoDS_Face& theOldFace2,
                                                          const TopoDS_Face& theNewFace2)
{
  TopoDS_Edge aNewEdge;
  Standard_Real fpar, lpar;
  BRep_Builder BB;
  
  Handle(Geom_Curve) aCurve = BRep_Tool::Curve(theEdge, fpar, lpar);
  aNewEdge = BRepLib_MakeEdge(aCurve, aCurve->FirstParameter(), aCurve->LastParameter()); //???
  TopoDS_Vertex V1, V2;
  TopExp::Vertices(aNewEdge, V1, V2);
  aNewEdge.Free(Standard_True);
  BB.Remove(aNewEdge, V1);
  BB.Remove(aNewEdge, V2);
  
  Standard_Real Etol = BRep_Tool::Tolerance(theEdge);
  Handle(Geom2d_Curve) aPCurve1 = BRep_Tool::CurveOnSurface(theEdge, theOldFace1, fpar, lpar);
  BB.UpdateEdge(aNewEdge, aPCurve1, theNewFace1, Etol);
  Handle(Geom2d_Curve) aPCurve2 = BRep_Tool::CurveOnSurface(theEdge, theOldFace2, fpar, lpar);
  BB.UpdateEdge(aNewEdge, aPCurve2, theNewFace2, Etol);

  return aNewEdge;
}

static void UpdateEdgeByProjectionOfPCurve(TopoDS_Edge& anEdge,
                                           const TopoDS_Face& aNewFace,
                                           const TopoDS_Face& aBoundedNewFace)
{
  BRep_Builder BB;
  Standard_Real fpar, lpar;
  
  Handle(Geom_Curve) aCurve = BRep_Tool::Curve(anEdge,fpar,lpar);
  
  Handle(Geom_Surface) NewSurf = BRep_Tool::Surface(aNewFace);
  Handle(ShapeAnalysis_Surface) SAS = new ShapeAnalysis_Surface(NewSurf);
  ShapeConstruct_ProjectCurveOnSurface aToolProj;
  aToolProj.Init(SAS, Precision::Confusion());
  Handle(Geom2d_Curve) NewPCurve;
  aToolProj.Perform(aCurve,fpar,lpar,NewPCurve);
  Standard_Real TolReached = SAS->Gap();
  //BB.UpdateEdge(anEdge, NullPCurve, aFace, 0.);
  BB.UpdateEdge(anEdge, NewPCurve, aBoundedNewFace, TolReached);
  //Check
  Standard_Integer NCONTROL = 23;
  BRepAdaptor_Curve BAcurve(anEdge);
  BRepAdaptor_Curve BAcurveonsu(anEdge, aBoundedNewFace);
  for (Standard_Integer i = 0; i < NCONTROL; i++)
  {
    Standard_Real prm = ((NCONTROL-1-i)*fpar + i*lpar)/(NCONTROL-1);
    gp_Pnt Pnt1 = BAcurve.Value(prm);
    gp_Pnt Pnt2 = BAcurveonsu.Value(prm);
    Standard_Real aDist = Pnt1.Distance(Pnt2);
    if (aDist > TolReached)
      TolReached = aDist;
  }
  BB.UpdateEdge(anEdge, TolReached);
  ///////
  
  TopoDS_Vertex V1, V2;
  TopExp::Vertices(anEdge, V1, V2);
  BB.UpdateVertex(V1, TolReached);
  BB.UpdateVertex(V2, TolReached);
}

static void ProjectVertexOnNewEdge(const TopoDS_Vertex& theVertex,
                                   const TopoDS_Edge&   theEdge,
                                   const TopoDS_Edge&   theNewEdge,
                                   Standard_Real&       theParam,
                                   gp_Pnt&              thePnt,
                                   Standard_Real&       theTolReached)
{
  Standard_Real ParamOnEdge = BRep_Tool::Parameter(theVertex, theEdge);
  BRepAdaptor_Curve BAcurve(theEdge);
  BRepAdaptor_Curve BAnewcurve(theNewEdge);
  gp_Pnt PointOnEdge = BAcurve.Value(ParamOnEdge);
  Extrema_ExtPC Projector(PointOnEdge, BAnewcurve);
  Standard_Real Param[4], dist[4];
  gp_Pnt Pnt[4];
  Param[1] = BAnewcurve.FirstParameter();
  Param[2] = BAnewcurve.LastParameter();
  Projector.TrimmedSquareDistances(dist[1], dist[2], Pnt[1], Pnt[2]);
  dist[3] = RealLast();
  if (Projector.IsDone() && Projector.NbExt() > 0)
  {
    Standard_Integer imin = 1;
    for (Standard_Integer i = 2; i <= Projector.NbExt(); i++)
      if (Projector.SquareDistance(i) < Projector.SquareDistance(imin))
        imin = i;
    Param[3] = Projector.Point(imin).Parameter();
    dist[3]  = Projector.SquareDistance(imin);
    Pnt[3]   = Projector.Point(imin).Value();
  }

  Standard_Integer imin = 1;
  for (Standard_Integer i = 2; i <= 3; i++)
    if (dist[i] < dist[imin])
      imin = i;

  theParam      = Param[imin];
  thePnt        = Pnt[imin];
  theTolReached = sqrt(dist[imin]);
}

static Standard_Boolean IsPCurveUiso(const Handle(Geom2d_Curve)& thePCurve,
                                     Standard_Real theFirstPar,
                                     Standard_Real theLastPar)
{
  gp_Pnt2d FirstP2d = thePCurve->Value(theFirstPar);
  gp_Pnt2d LastP2d  = thePCurve->Value(theLastPar);

  Standard_Real DeltaU = Abs(FirstP2d.X() - LastP2d.X());
  Standard_Real DeltaV = Abs(FirstP2d.Y() - LastP2d.Y());

  return (DeltaU < DeltaV);
}

static TopoDS_Edge GetSeamEdge(const TopoDS_Edge& theEdge,
                               const TopoDS_Face& theFace,
                               const TopoDS_Face& theNewFace)
{
  TopoDS_Edge aSeamEdge;

  Standard_Real fpar, lpar;
  Handle(Geom2d_Curve) aPCurve = BRep_Tool::CurveOnSurface(theEdge, theFace, fpar, lpar);
  Standard_Boolean IsUiso = IsPCurveUiso(aPCurve, fpar, lpar);

  TopExp_Explorer Explo(theNewFace, TopAbs_EDGE);
  for (; Explo.More(); Explo.Next())
  {
    const TopoDS_Edge& anEdge = TopoDS::Edge(Explo.Current());
    if (BRepTools::IsReallyClosed(anEdge, theNewFace))
    {
      Standard_Real NewFirst, NewLast;
      Handle(Geom2d_Curve) aNewPCurve = BRep_Tool::CurveOnSurface(anEdge, theNewFace,
                                                                  NewFirst, NewLast);
      Standard_Boolean IsNewUiso = IsPCurveUiso(aNewPCurve, NewFirst, NewLast);
      if (IsNewUiso == IsUiso)
      {
        aSeamEdge = anEdge;
        break;
      }
    }
  }

  TopoDS_Edge aNewEdge = TopoDS::Edge(aSeamEdge.EmptyCopied());
  return aNewEdge;
}

static Standard_Boolean IsPCurveFirst(const TopoDS_Edge& theEdge,
                                      const TopoDS_Face& theFace)
{
  Standard_Real fpar, lpar;
  Handle(Geom2d_Curve) aPCurve = BRep_Tool::CurveOnSurface(theEdge, theFace, fpar, lpar);
  TopoDS_Edge RevEdge = TopoDS::Edge(theEdge.Reversed());
  Handle(Geom2d_Curve) AnotherPCurve = BRep_Tool::CurveOnSurface(RevEdge, theFace, fpar, lpar);
  Standard_Boolean IsUiso = IsPCurveUiso(aPCurve, fpar, lpar);

  gp_Pnt2d aP2d       = aPCurve->Value(fpar);
  gp_Pnt2d AnotherP2d = AnotherPCurve->Value(fpar);

  Standard_Integer IndCoord = (IsUiso)? 1 : 2; // U or V
  
  Standard_Real aCoord       = aP2d.Coord(IndCoord);
  Standard_Real AnotherCoord = AnotherP2d.Coord(IndCoord);

  return (aCoord < AnotherCoord);
}

static void CorrectOrientationOfSeam(const TopoDS_Edge& theEdge,
                                     const TopoDS_Face& theFace,
                                     TopoDS_Edge&       theNewEdge,
                                     const TopoDS_Face& theNewFace)
{
  if (!BRepTools::IsReallyClosed(theEdge, theFace)) //only for seam
    return;

  Standard_Boolean IsFirstPCurve = IsPCurveFirst(theEdge, theFace);
  Standard_Boolean IsFirstNewPCurve = IsPCurveFirst(theNewEdge, theNewFace);
  if (IsFirstPCurve != IsFirstNewPCurve)
    theNewEdge.Reverse();
}

/*
static TopoDS_Edge GetGeneratedEdge(const TopoDS_Edge& anEdge,
                                    const TopoDS_Face& aFace,
                                    const TopoDS_Face& aNewFace)
{
  TopoDS_Edge aNewEdge;
  
  TopExp_Explorer Explo(aFace, TopAbs_EDGE);
  TopExp_Explorer ExploNew(aNewFace, TopAbs_EDGE);
  for (; Explo.More(); Explo.Next(),ExploNew.Next())
  {
    const TopoDS_Shape& EdgeInFace = Explo.Current();
    //const TopoDS_Shape& EdgeInNewFace = ExploNew.Current();
    if (anEdge == EdgeInFace)
    {
      aNewEdge = TopoDS::Edge(ExploNew.Current());
      break;
    }
  }

  TopoDS_Vertex V1, V2;
  TopExp::Vertices(aNewEdge, V1, V2);
  BRep_Builder BB;
  aNewEdge.Free(Standard_True);
  BB.Remove(aNewEdge, V1);
  BB.Remove(aNewEdge, V2);
  return aNewEdge;
}
*/

static TopAbs_Orientation OrientationInEdge(const TopoDS_Vertex& theVertex,
                                            const TopoDS_Edge& theEdge)
{
  TopoDS_Vertex V1, V2;
  TopExp::Vertices(theEdge, V1, V2);
  if (theVertex.IsSame(V1))
    return TopAbs_FORWARD;
  
  return TopAbs_REVERSED;
}

static Standard_Boolean EdgeContains(const TopoDS_Edge& theEdge,
                                     const TopoDS_Vertex& theVertex)
{
  TopoDS_Iterator ite(theEdge);
  for (; ite.More(); ite.Next())
    if (theVertex.IsSame(ite.Value()))
      return Standard_True;
  
  return Standard_False;
}


static Standard_Boolean IsTangentFaces(const TopoDS_Edge& theEdge,
                                       const TopoDS_Face& theFace1,
                                       const TopoDS_Face& theFace2,
                                       const GeomAbs_Shape Order)
{
  if (Order == GeomAbs_G1 &&
      BRep_Tool::Continuity( theEdge, theFace1, theFace2 ) != GeomAbs_C0)
    return Standard_True;

  Standard_Real TolC0 = Max(0.001, 1.5*BRep_Tool::Tolerance(theEdge));

  Standard_Real aFirst;
  Standard_Real aLast;
    
// Obtaining of pcurves of edge on two faces.
  const Handle(Geom2d_Curve) aC2d1 = BRep_Tool::CurveOnSurface
                                                (theEdge, theFace1, aFirst, aLast);
  const Handle(Geom2d_Curve) aC2d2 = BRep_Tool::CurveOnSurface
                                                (theEdge, theFace2, aFirst, aLast);
  if (aC2d1.IsNull() || aC2d2.IsNull())
    return Standard_False;

// Obtaining of two surfaces from adjacent faces.
  Handle(Geom_Surface) aSurf1 = BRep_Tool::Surface(theFace1);
  Handle(Geom_Surface) aSurf2 = BRep_Tool::Surface(theFace2);

  if (aSurf1.IsNull() || aSurf2.IsNull())
    return Standard_False;

// Computation of the number of samples on the edge.
  BRepAdaptor_Surface              aBAS1(theFace1);
  BRepAdaptor_Surface              aBAS2(theFace2);
  Handle(BRepAdaptor_HSurface)     aBAHS1      = new BRepAdaptor_HSurface(aBAS1);
  Handle(BRepAdaptor_HSurface)     aBAHS2      = new BRepAdaptor_HSurface(aBAS2);
  Handle(BRepTopAdaptor_TopolTool) aTool1      = new BRepTopAdaptor_TopolTool(aBAHS1);
  Handle(BRepTopAdaptor_TopolTool) aTool2      = new BRepTopAdaptor_TopolTool(aBAHS2);
  Standard_Integer                 aNbSamples1 =     aTool1->NbSamples();
  Standard_Integer                 aNbSamples2 =     aTool2->NbSamples();
  const Standard_Integer           aNbSamplesMax =   23;
  Standard_Integer                 aNbSamples  =     Min(aNbSamplesMax, Max(aNbSamples1, aNbSamples2));
  const Standard_Real              aTolAngle   =     M_PI/18;


// Computation of the continuity.
  Standard_Real    aPar;
  Standard_Real    aDelta = (aLast - aFirst)/(aNbSamples - 1);
  Standard_Integer i, nbNotDone = 0;

  for (i = 1, aPar = aFirst; i <= aNbSamples; i++, aPar += aDelta) {
    if (i == aNbSamples) aPar = aLast;

    LocalAnalysis_SurfaceContinuity aCont(aC2d1,  aC2d2,  aPar,
                                          aSurf1, aSurf2, Order,
                                          0.001, TolC0, aTolAngle, 0.1, 0.1);
    if (!aCont.IsDone()) 
    {
      nbNotDone++;
      continue;
    }

    if (Order == GeomAbs_G1)
    {
      if (!aCont.IsG1())
        return Standard_False;
    }
    else if (!aCont.IsG2())
      return Standard_False;
  }
  
  if (nbNotDone == aNbSamples)
    return Standard_False;

  return Standard_True;
}

//=======================================================================
//function : BRepOffsetAPI_PatchFaces
//purpose  : Constructor
//=======================================================================

BRepOffsetAPI_PatchFaces::BRepOffsetAPI_PatchFaces(const TopoDS_Shape& theShape)
{
  myInitialShape = theShape;
}

//=======================================================================
//function : SetOffsetFace
//purpose  : 
//=======================================================================
void BRepOffsetAPI_PatchFaces::AddPatchFace(const TopoDS_Face& theFace,
                                            const TopoDS_Face& thePatchFace)
{
  // Check the orientation of the patch face and make
  // it oriented the same way as original
  TopoDS_Face aFace      = TopoDS::Face(theFace.Oriented(TopAbs_FORWARD));
  TopoDS_Face aPatchFace = TopoDS::Face(thePatchFace.Oriented(TopAbs_FORWARD));
  
  Handle(IntTools_Context) aCtx = new IntTools_Context;
  Standard_Boolean bToReverse = BOPTools_AlgoTools::IsSplitToReverse(aFace, aPatchFace, aCtx);
  TopoDS_Face anOrientedPatchFace = bToReverse ? TopoDS::Face(aPatchFace.Reversed()) : aPatchFace;
  myFacePatchFace.Add(aFace, anOrientedPatchFace);
}

//=======================================================================
//function : Build
//purpose  : 
//=======================================================================

void BRepOffsetAPI_PatchFaces::Build()
{
  TopExp::MapShapesAndUniqueAncestors(myInitialShape, TopAbs_EDGE, TopAbs_FACE, myEFmap);

  //Draft filling of <myFaceNewFace>
  for (Standard_Integer i = 1; i <= myFacePatchFace.Extent(); i++)
  {
    const TopoDS_Face& aFace = TopoDS::Face(myFacePatchFace.FindKey(i));
    const TopoDS_Shape& aPatchFace = myFacePatchFace(i);
    myFaceNewFace.Add(aFace, aPatchFace);
    TopExp_Explorer Explo(aFace, TopAbs_EDGE);
    for (; Explo.More(); Explo.Next())
    {
      const TopoDS_Edge& anEdge = TopoDS::Edge(Explo.Current());
      const TopTools_ListOfShape& Lfaces = myEFmap.FindFromKey(anEdge);
      TopoDS_Face aNeighborFace = (aFace.IsSame(Lfaces.First()))?
        TopoDS::Face(Lfaces.Last()) : TopoDS::Face(Lfaces.First());
      if (myFacePatchFace.Contains(aNeighborFace))
        continue;
      if (myFaceNewFace.Contains(aNeighborFace))
        continue;
      Standard_Boolean IsTangentEdge = IsTangentFaces(anEdge, aFace, aNeighborFace, GeomAbs_G1);
      if (IsTangentEdge)
      {
        myTangentEdges.Add(anEdge);
        continue;
      }
                                                                          
      aNeighborFace.Orientation(TopAbs_FORWARD);
      TopoDS_Face aNewFace;
      BRepOffset_Tool::EnLargeFace(aNeighborFace, aNewFace,
                                   Standard_True,Standard_True,Standard_True,Standard_True,Standard_True,
                                   Standard_False); //not too big
      myFaceNewFace.Add(aNeighborFace, aNewFace);
    }
  }

  //Make draft intersection edges: draft filling of <myEdgeNewEdge>
  BRep_Builder BB;
  TopTools_MapOfShape UpdatedConstEdges;
  for (Standard_Integer i = 1; i <= myFaceNewFace.Extent(); i++)
  {
    TopoDS_Face aFace = TopoDS::Face(myFaceNewFace.FindKey(i));
    TopoDS_Face aNewFace = TopoDS::Face(myFaceNewFace(i));
    TopoDS_Face aBoundedNewFace;
    if (myNewFaceBoundedFace.IsBound(aNewFace))
      aBoundedNewFace = TopoDS::Face(myNewFaceBoundedFace(aNewFace));
    else
    {
      aBoundedNewFace = TopoDS::Face(aNewFace.EmptyCopied());
      myNewFaceBoundedFace.Bind(aNewFace, aBoundedNewFace);
    }
      
    TopoDS_Iterator itf(aFace);
    for (; itf.More(); itf.Next())
    {
      const TopoDS_Wire& aWire = TopoDS::Wire(itf.Value());
      TopoDS_Wire F_Wire = TopoDS::Wire(aWire.Oriented(TopAbs_FORWARD));
      BRepTools_WireExplorer wexp(F_Wire, aFace);
      for (; wexp.More(); wexp.Next())
      {
        TopoDS_Edge anEdge = wexp.Current();
        Standard_Boolean ToReverse = Standard_False;
        TopoDS_Edge aNewEdge;
        if (myEdgeNewEdge.IsBound(anEdge))
          //aNewEdge = TopoDS::Edge(myEdgeNewEdge(anEdge));
          continue;
        
        Handle(Geom2d_Curve) NullPCurve;
        Standard_Real fpar, lpar;
        
        const TopTools_ListOfShape& Lfaces = myEFmap.FindFromKey(anEdge);
        if (Lfaces.Extent() == 1) //seam edge
        {
          cout<<endl<<"Seam edge or degenerated edge !!!"<<endl;
          //TopoDS_Edge GeneratedEdge = GetGeneratedEdge(anEdge, aFace, aNewFace);
          TopoDS_Edge GeneratedSeamEdge = GetSeamEdge(anEdge, aFace, aNewFace);
          //myOrientedEdgeNewEdge.Bind(anEdge, GeneratedEdge);
          myEdgeNewEdge.Bind(anEdge, GeneratedSeamEdge);
          continue;
        }
        
        TopoDS_Face aNeighborFace = (aFace.IsSame(Lfaces.First()))?
          TopoDS::Face(Lfaces.Last()) : TopoDS::Face(Lfaces.First());
        
        if (myTangentEdges.Contains(anEdge)&&
            !UpdatedConstEdges.Contains(anEdge)) //project onto patch
        {
          UpdateEdgeByProjectionOfPCurve(anEdge, aNewFace, aBoundedNewFace);
          
          //aNewEdge = anEdge;
          UpdatedConstEdges.Add(anEdge);
          continue;
        }
        
        if (myFaceNewFace.Contains(aNeighborFace))//intersect
        {
          TopoDS_Face aNewNeighborFace = TopoDS::Face(myFaceNewFace.FindFromKey(aNeighborFace));
          TopoDS_Face aBoundedNewNeighborFace;
          if (myNewFaceBoundedFace.IsBound(aNewNeighborFace))
            aBoundedNewNeighborFace = TopoDS::Face(myNewFaceBoundedFace(aNewNeighborFace));
          else
          {
            aBoundedNewNeighborFace = TopoDS::Face(aNewNeighborFace.EmptyCopied());
            myNewFaceBoundedFace.Bind(aNewNeighborFace, aBoundedNewNeighborFace);
          }

          if (!myFacePatchFace.Contains(aFace) &&
              !myFacePatchFace.Contains(aNeighborFace) &&
              IsTangentFaces(anEdge, aFace, aNeighborFace, GeomAbs_G1)) //smooth edge
          {
            //make new edge with pcurves on new surfaces
            aNewEdge = MakeNewEdgeWithOldPcurvesOnNewSurfaces(anEdge,
                                                              aFace, aBoundedNewFace,
                                                              aNeighborFace, aBoundedNewNeighborFace);

            myEdgeNewEdge.Bind(anEdge.Oriented(TopAbs_FORWARD), aNewEdge);
            continue;
          }
          
          BRepLib::BuildCurves3d(aNewFace);
          BRepLib::BuildCurves3d(aNewNeighborFace);
          
          BRepAlgoAPI_Section SecBuilder(aNewFace, aNewNeighborFace, Standard_False);
          SecBuilder.Approximation(Standard_True);
          SecBuilder.ComputePCurveOn1(Standard_True);
          SecBuilder.ComputePCurveOn2(Standard_True);
          SecBuilder.Build();
          TopoDS_Shape aSection = SecBuilder.Shape();
          Standard_Boolean Success = Standard_True;
          TopExp_Explorer ExpSec(aSection, TopAbs_EDGE);
          if (ExpSec.More())
            aNewEdge = TopoDS::Edge(ExpSec.Current());
          else //no intersection
          {
            cout<<endl<<"No intersecion => smooth edge"<<endl;
            Success = Standard_False;
          }
          ExpSec.Next();
          if (ExpSec.More())
          {
            cout<<endl<<"More than one intersecion => smooth edge"<<endl;
            Success = Standard_False;
          }
          if (!Success) //a smooth edge with bigger angle
          {
            if (myFacePatchFace.Contains(aFace)) //project onto patch
            {
              UpdateEdgeByProjectionOfPCurve(anEdge, aNewFace, aBoundedNewFace);

              //Remove neighbor face from <myFaceNewFace>
              myFaceNewFace.RemoveKey(aNeighborFace);
          
              myTangentEdges.Add(anEdge);
              UpdatedConstEdges.Add(anEdge);
            }
            else
            {
              aNewEdge = MakeNewEdgeWithOldPcurvesOnNewSurfaces(anEdge,
                                                                aFace, aBoundedNewFace,
                                                                aNeighborFace, aBoundedNewNeighborFace);
              myEdgeNewEdge.Bind(anEdge.Oriented(TopAbs_FORWARD), aNewEdge);
            }
            continue;
          }
          TopoDS_Vertex V1, V2;
          TopExp::Vertices(aNewEdge, V1, V2);
          aNewEdge.Free(Standard_True);
          BB.Remove(aNewEdge, V1);
          BB.Remove(aNewEdge, V2);
          aNewEdge.Orientation(TopAbs_FORWARD);
          
          //Make pcurves on new surfaces
          Handle(Geom2d_Curve) PCurve1 = BRep_Tool::CurveOnSurface(aNewEdge, aNewFace, fpar, lpar);
          BB.UpdateEdge(aNewEdge, NullPCurve, aNewFace, 0.);
          BB.UpdateEdge(aNewEdge, PCurve1, aBoundedNewFace, 0.);
          Handle(Geom2d_Curve) PCurve2 = BRep_Tool::CurveOnSurface(aNewEdge, aNewNeighborFace, fpar, lpar);
          BB.UpdateEdge(aNewEdge, NullPCurve, aNewNeighborFace, 0.);
          BB.UpdateEdge(aNewEdge, PCurve2, aBoundedNewNeighborFace, 0.);
          
          //Check orientation of new edge
          BRepAdaptor_Curve BAcurve(anEdge);
          BRepAdaptor_Curve BAnewcurve(aNewEdge);
          gp_Pnt FirstPnt, FirstNewPnt;
          gp_Vec DirOnCurve, DirOnNewCurve;
          BAcurve.D1(BAcurve.FirstParameter(), FirstPnt, DirOnCurve);
          Standard_Real ParamOnNewEdge = BAnewcurve.FirstParameter();
          Extrema_ExtPC Projector(FirstPnt, BAnewcurve);
          if (!Projector.IsDone() || Projector.NbExt() == 0)
          {
            cout<<endl<<"Define orientation of new edge: extrema point-curve is not done"<<endl;
          }
          if (Projector.IsDone() && Projector.NbExt() > 0)
          {
            Standard_Integer indmin = 1;
            for (Standard_Integer ind = 2; ind <= Projector.NbExt(); ind++)
              if (Projector.SquareDistance(ind) < Projector.SquareDistance(indmin))
                indmin = ind;
            ParamOnNewEdge = Projector.Point(indmin).Parameter();
          }
          BAnewcurve.D1(ParamOnNewEdge, FirstNewPnt, DirOnNewCurve);
          Standard_Real ScalProd = DirOnCurve * DirOnNewCurve;
          if (ScalProd < 0.)
            ToReverse = Standard_True;
          
          myEdgeNewEdge.Bind(anEdge.Oriented(TopAbs_FORWARD),
                             (ToReverse)? aNewEdge.Oriented(TopAbs_REVERSED) : aNewEdge);
        } //intersect
        else //borders on constant face: change surface of pcurve on existing edge
        {
          Handle(Geom2d_Curve) aPCurve = BRep_Tool::CurveOnSurface(anEdge, aFace, fpar, lpar);
          //BB.UpdateEdge(anEdge, NullPCurve, aFace, 0.);
          BB.UpdateEdge(anEdge, aPCurve, aBoundedNewFace, 0.);
          
          aNewEdge = anEdge;
        }
      } //for (; wexp.More(); wexp.Next())
    } //for (; itf.More(); itf.Next())
  } //for (Standard_Integer i = 1; i <= myFaceNewFace.Extent(); i++)

  //Intersect edges and make new wires
  for (Standard_Integer i = 1; i <= myFaceNewFace.Extent(); i++)
  {
    TopoDS_Face aFace = TopoDS::Face(myFaceNewFace.FindKey(i));
    TopoDS_Face aNewFace = TopoDS::Face(myFaceNewFace(i));
    TopoDS_Face aBoundedNewFace = TopoDS::Face(myNewFaceBoundedFace(aNewFace));
    
    TopoDS_Iterator itf(aFace);
    for (; itf.More(); itf.Next())
    {
      const TopoDS_Wire& aWire = TopoDS::Wire(itf.Value());
      TopAbs_Orientation aWireOr = aWire.Orientation();
      
      TopoDS_Wire aNewWire;
      BB.MakeWire(aNewWire);
      
      TopoDS_Wire F_Wire = TopoDS::Wire(aWire.Oriented(TopAbs_FORWARD));
      BRepTools_WireExplorer wexp(F_Wire, aFace);
      TopoDS_Vertex CurVertex = wexp.CurrentVertex();
      TopoDS_Edge   FirstEdge = wexp.Current();
      TopoDS_Edge   FirstNewEdge = FirstEdge;
      if (myEdgeNewEdge.IsBound(FirstEdge))
        FirstNewEdge = TopoDS::Edge(myEdgeNewEdge(FirstEdge));
      TopoDS_Edge CurEdge, PrevEdge = FirstEdge;
      TopoDS_Edge CurNewEdge, PrevNewEdge = FirstNewEdge;
      wexp.Next();
      if (!wexp.More() && //only one edge in wire
          !myVertexNewVertex.IsBound(CurVertex))
      {
        TopoDS_Vertex CurNewVertex;
        if (myVertexNewVertex.IsBound(CurVertex))
          CurNewVertex = TopoDS::Vertex(myVertexNewVertex(CurVertex));

        if (myEdgeNewEdge.IsBound(FirstEdge))//new edge: update
        {
          Standard_Real fpar, lpar;
          BRep_Tool::Range(FirstEdge, fpar, lpar);
          BB.Range(FirstNewEdge, fpar, lpar);
          BRepAdaptor_Curve BAcurve(FirstNewEdge);
          gp_Pnt FirstPnt = BAcurve.Value(BAcurve.FirstParameter());
          if (CurNewVertex.IsNull())
            CurNewVertex = BRepLib_MakeVertex(FirstPnt);
          BB.Add(FirstNewEdge, CurNewVertex);
          BB.Add(FirstNewEdge, CurNewVertex.Oriented(TopAbs_REVERSED));
          myVertexNewVertex.Bind(CurVertex, CurNewVertex);
        }
      }
      for (; wexp.More(); wexp.Next())
      {
        CurEdge = wexp.Current();
        if (myEdgeNewEdge.IsBound(CurEdge))
          CurNewEdge = TopoDS::Edge(myEdgeNewEdge(CurEdge));
        else
          CurNewEdge = CurEdge;
        CurVertex = wexp.CurrentVertex();
        UpdateEdgesAndVertex(PrevEdge, PrevNewEdge,
                             CurEdge, CurNewEdge,
                             CurVertex,
                             aFace, aNewFace, aBoundedNewFace);
        if (!PrevEdge.IsSame(PrevNewEdge) &&
            !BRepTools::IsReallyClosed(PrevEdge, aFace) &&
            PrevEdge.Orientation() == TopAbs_REVERSED)
          PrevNewEdge.Reverse();
        BB.Add(aNewWire, PrevNewEdge);
        
        PrevEdge = CurEdge;
        PrevNewEdge = CurNewEdge;
      }
      CurEdge = FirstEdge;
      CurNewEdge = FirstNewEdge;
      CurVertex = wexp.CurrentVertex();
      UpdateEdgesAndVertex(PrevEdge, PrevNewEdge,
                           CurEdge, CurNewEdge,
                           CurVertex,
                           aFace, aNewFace, aBoundedNewFace);
      if (!PrevEdge.IsSame(PrevNewEdge) &&
          !BRepTools::IsReallyClosed(PrevEdge, aFace) &&
          PrevEdge.Orientation() == TopAbs_REVERSED)
        PrevNewEdge.Reverse();
      BB.Add(aNewWire, PrevNewEdge);

      aNewWire.Orientation(aWireOr);
      BB.Add(aBoundedNewFace, aNewWire);
    } //for (; itf.More(); itf.Next()) (iterator on face)
  }

  //Assemble resulting shape
  TopoDS_Solid aSolid;
  BB.MakeSolid(aSolid);
  TopoDS_Shell aShell;
  BB.MakeShell(aShell);
  TopExp_Explorer Explo(myInitialShape, TopAbs_FACE);
  for (; Explo.More(); Explo.Next())
  {
    const TopoDS_Shape& aFace = Explo.Current();
    TopoDS_Shape aBoundedNewFace;
    if (myFaceNewFace.Contains(aFace))
    {
      const TopoDS_Shape& aNewFace = myFaceNewFace.FindFromKey(aFace);
      aBoundedNewFace = myNewFaceBoundedFace(aNewFace);
      if (aFace.Orientation() == TopAbs_REVERSED)
        aBoundedNewFace.Reverse();
    }
    else
      aBoundedNewFace = aFace;
    BB.Add(aShell, aBoundedNewFace);
  }
  BB.Add(aSolid, aShell);

  //ShapeFix_Shape Fixer(aSolid);
  //Fixer.Perform();
  
  //myShape = Fixer.Shape();
  myShape = aSolid;
  
  Done();
}

void BRepOffsetAPI_PatchFaces::UpdateEdgesAndVertex(const TopoDS_Edge& thePrevEdge,
                                                    TopoDS_Edge&       thePrevNewEdge,
                                                    const TopoDS_Edge& theCurEdge,
                                                    TopoDS_Edge&       theCurNewEdge,
                                                    TopoDS_Vertex&     theCurVertex,
                                                    const TopoDS_Face& theFace,
                                                    const TopoDS_Face& theNewFace,
                                                    TopoDS_Face&       theBoundedNewFace)
{
  CorrectOrientationOfSeam(thePrevEdge, theFace,
                           thePrevNewEdge, theNewFace);
  CorrectOrientationOfSeam(theCurEdge, theFace,
                           theCurNewEdge, theNewFace);
  
  BRep_Builder BB;
  
  TopoDS_Vertex CurNewVertex;
  if (myVertexNewVertex.IsBound(theCurVertex))
    CurNewVertex = TopoDS::Vertex(myVertexNewVertex(theCurVertex));
  else
  {
    Standard_Boolean IsConstVertex = (!myEdgeNewEdge.IsBound(thePrevEdge) ||
                                      !myEdgeNewEdge.IsBound(theCurEdge));
    if (IsConstVertex)
      CurNewVertex = theCurVertex;
  }

  if (myEdgeNewEdge.IsBound(thePrevEdge) &&
      myEdgeNewEdge.IsBound(theCurEdge)) //two new edges: intersect
  {
    Standard_Real ParamOnPrev, ParamOnCur, TolProj;
    gp_Pnt PntOnPrev, PntOnCur;
    gp_Pnt2d Pnt2dOnPrev, Pnt2dOnCur;
    ProjectVertexOnNewEdge_2d(theCurVertex,
                              thePrevEdge, theFace,
                              thePrevNewEdge, theBoundedNewFace,
                              ParamOnPrev, PntOnPrev, Pnt2dOnPrev, TolProj);
    ProjectVertexOnNewEdge_2d(theCurVertex,
                              theCurEdge, theFace,
                              theCurNewEdge, theBoundedNewFace,
                              ParamOnCur, PntOnCur, Pnt2dOnCur, TolProj);
    gp_Pnt2d P2dRef((Pnt2dOnPrev.XY() + Pnt2dOnCur.XY())/2);

    Standard_Real TolReached;
    gp_Pnt PntVtx = (CurNewVertex.IsNull())?
      BRep_Tool::Pnt(theCurVertex) : BRep_Tool::Pnt(CurNewVertex);
    
    //TolReached = PntOnPrev.Distance(PntOnCur);
    //Standard_Real DistVtoPrev = PntVtx.Distance(PntOnPrev);
    //Standard_Real DistVtoCur = PntVtx.Distance(PntOnCur);
    //TolReached = Max(TolReached, DistVtoPrev);
    //TolReached = Max(TolReached, DistVtoCur);

    /*
    //BRepExtrema_ExtCC ExtrEE(thePrevNewEdge, theCurNewEdge);
    BRepAdaptor_Curve BAprevcurve(thePrevNewEdge);
    BRepAdaptor_Curve BAcurcurve(theCurNewEdge);
    BRepAdaptor_Curve2d BAprevcurve2d(thePrevNewEdge, theBoundedNewFace);
    BRepAdaptor_Curve2d BAcurcurve2d(theCurNewEdge, theBoundedNewFace);
    //Geom2dInt_GInter Inters2d(BAprevcurve2d, BAcurcurve2d,
    //                        Precision::PConfusion(), Precision::PConfusion());
    Extrema_ExtCC2d ExtrCC(BAprevcurve2d, BAcurcurve2d);
    Standard_Real ParOnPrev[6], ParOnCur[6], dist[6], SumDist[6];
    gp_Pnt2d Pnt2d[6];
    ParOnPrev[1] = ParOnPrev[2] = BAprevcurve2d.FirstParameter();
    ParOnPrev[3] = ParOnPrev[4] = BAprevcurve2d.LastParameter();
    ParOnCur[1]  = ParOnCur[3]  = BAcurcurve2d.FirstParameter();
    ParOnCur[2]  = ParOnCur[4]  = BAcurcurve2d.LastParameter();
    
    ExtrCC.TrimmedSquareDistances(dist[1], dist[2], dist[3], dist[4],
                                  Pnt2d[1], Pnt2d[2], Pnt2d[3], Pnt2d[4]);
    SumDist[1] = P2dRef.SquareDistance(Pnt2d[1]) + P2dRef.SquareDistance(Pnt2d[3]); //first-first
    SumDist[2] = P2dRef.SquareDistance(Pnt2d[1]) + P2dRef.SquareDistance(Pnt2d[4]); //first-last
    SumDist[3] = P2dRef.SquareDistance(Pnt2d[2]) + P2dRef.SquareDistance(Pnt2d[3]); //last-first
    SumDist[4] = P2dRef.SquareDistance(Pnt2d[2]) + P2dRef.SquareDistance(Pnt2d[4]); //last-last
    SumDist[5] = RealLast();
    Standard_Integer imin = 0;
    if (ExtrCC.IsDone())
    {
      for (Standard_Integer i = 1; i <= ExtrCC.NbExt(); i++)
      {
        Extrema_POnCurv2d P2d1, P2d2;
        ExtrCC.Points(i, P2d1, P2d2);
        Standard_Real aSumDist = P2dRef.SquareDistance(P2d1.Value()) + P2dRef.SquareDistance(P2d2.Value());
        if (aSumDist < SumDist[5])
        {
          SumDist[5] = aSumDist;
          imin = i;
        }
      }
      if (imin != 0)
      {
        Extrema_POnCurv2d P2d1min, P2d2min;
        ExtrCC.Points(imin, P2d1min, P2d2min);
        ParOnPrev[5] = P2d1min.Parameter();
        ParOnCur[5]  = P2d2min.Parameter();
      }
    }
    if (imin == 0)
    {
      imin = 1;
      for (Standard_Integer i = 2; i <= 4; i++)
        if (SumDist[i] < SumDist[imin])
          imin = i;
    }
    else
      imin = 5;

    ParamOnPrev = ParOnPrev[imin];
    ParamOnCur  = ParOnCur[imin];
    gp_Pnt SolutionOnPrev = BAprevcurve.Value(ParamOnPrev);
    gp_Pnt SolutionOnCur  = BAcurcurve.Value(ParamOnCur);
    gp_Pnt Pint((SolutionOnPrev.XYZ() + SolutionOnCur.XYZ())/2);
    //Standard_Real TolInt = PntVtx.Distance(Pint);
    //TolReached = TolInt;
    */
    gp_Pnt Pint;
    IntersectIn2d(thePrevNewEdge, theCurNewEdge, theBoundedNewFace, P2dRef,
                  ParamOnPrev, ParamOnCur, Pint, PntOnCur);
    //PntOnPrev = PntOnCur = Pint;
      
    if (CurNewVertex.IsNull())
    {
      //gp_Pnt NewPnt((PntOnPrev.XYZ() + PntOnCur.XYZ())/2);
      CurNewVertex = BRepLib_MakeVertex(Pint);
      myVertexNewVertex.Bind(theCurVertex, CurNewVertex);
      TolReached = Pint.Distance(PntOnCur);
    }
    else
    {
      PntVtx = BRep_Tool::Pnt(CurNewVertex);
      TolReached = PntVtx.Distance(PntOnCur); //PntVtx.Distance(Pint);
    }
    BB.UpdateVertex(CurNewVertex, 1.001*TolReached);
    
    if (!EdgeContains(thePrevNewEdge, CurNewVertex))
      PutVertexToEdge(CurNewVertex, theCurVertex, thePrevNewEdge, thePrevEdge, ParamOnPrev);
    
    if (!EdgeContains(theCurNewEdge, CurNewVertex))
      PutVertexToEdge(CurNewVertex, theCurVertex, theCurNewEdge, theCurEdge, ParamOnCur);
  } //two new edges: intersect
  else if (myEdgeNewEdge.IsBound(thePrevEdge) ||
           myEdgeNewEdge.IsBound(theCurEdge)) //one constant edge: project point onto curve
  {
    TopoDS_Edge ConstantEdge, ModifiedEdge, NewEdge;
    if (myEdgeNewEdge.IsBound(thePrevEdge))
    {
      ConstantEdge = theCurEdge;
      ModifiedEdge = thePrevEdge;
      NewEdge = thePrevNewEdge;
    }
    else
    {
      ConstantEdge = thePrevEdge;
      ModifiedEdge = theCurEdge;
      NewEdge = theCurNewEdge;
    }

    Standard_Real ParamOnConstEdge, ParamOnNewEdge, TolReached;
    gp_Pnt PntOnNewEdge;
    gp_Pnt2d Pnt2dOnNewEdge;
    ProjectVertexOnNewEdge_2d(theCurVertex,
                              ModifiedEdge, theFace,
                              NewEdge, theBoundedNewFace,
                              ParamOnNewEdge, PntOnNewEdge, Pnt2dOnNewEdge, TolReached);
    //BB.UpdateVertex(CurNewVertex, TolReached);

    /*
    BRepAdaptor_Curve BAconstcurve(ConstantEdge);
    BRepAdaptor_Curve2d BAconstcurve2d(ConstantEdge, theBoundedNewFace);
    BRepAdaptor_Curve2d BAnewcurve2d(NewEdge, theBoundedNewFace);
    //Geom2dInt_GInter Inters2d(BAconstcurve2d, BAnewcurve2d,
    //                          Precision::PConfusion(), Precision::PConfusion());
    Standard_Integer imin = 0;
    if (Inters2d.IsDone())
    {
      Standard_Real MinDist = RealLast();
      for (Standard_Integer iint = 1; iint <= Inters2d.NbPoints(); iint++)
      {
        const IntRes2d_IntersectionPoint& ip = Inters2d.Point(iint);
        gp_Pnt2d Pint2d = ip.Value();
        Standard_Real aDist = Pnt2dOnNewEdge.SquareDistance(Pint2d);
        if (aDist < MinDist)
        {
          MinDist = aDist;
          imin = iint;
        }
        //gp_Pnt Pint = BAprevcurve.Value(ip.ParamOnFirst());
      }
    }
    if (imin != 0)
    {
      gp_Pnt Pint = BAconstcurve.Value(Inters2d.Point(imin).ParamOnFirst());
      //Standard_Real TolInt = PntVtx.Distance(Pint);
      //TolReached = TolInt;
      PntOnNewEdge = Pint;
      //ParamOnPrev = Inters2d.Point(imin).ParamOnFirst(); //ExtrEE.ParameterOnE1(imin);
      ParamOnNewEdge  = Inters2d.Point(imin).ParamOnSecond();  //ExtrEE.ParameterOnE2(imin);
      gp_Pnt PntVtx = BRep_Tool::Pnt(CurNewVertex);
      TolReached = PntVtx.Distance(Pint);
    }
    BB.UpdateVertex(CurNewVertex, TolReached);
    */

    gp_Pnt Pint;
    IntersectIn2d(ConstantEdge, NewEdge, theBoundedNewFace, Pnt2dOnNewEdge,
                  ParamOnConstEdge, ParamOnNewEdge, Pint, PntOnNewEdge);

    gp_Pnt PntVtx = BRep_Tool::Pnt(CurNewVertex);
    TolReached = PntVtx.Distance(PntOnNewEdge);
    BB.UpdateVertex(CurNewVertex, 1.001*TolReached);
    
    if (!EdgeContains(NewEdge, CurNewVertex))
      PutVertexToEdge(CurNewVertex, theCurVertex, NewEdge, ModifiedEdge, ParamOnNewEdge);
  } //else (one constant edge: project point onto curve)
  else //two constant edges
  {
    //nothing ?
  }
}


void BRepOffsetAPI_PatchFaces::PutVertexToEdge(const TopoDS_Vertex& theVertex,
                                               const TopoDS_Vertex& theProVertex,
                                               TopoDS_Edge& theEdge,
                                               const TopoDS_Edge& theProEdge,
                                               const Standard_Real theParamOnEdge)
{
  BRep_Builder BB;
  
  TopAbs_Orientation anOr = OrientationInEdge(theProVertex, theProEdge);
  if (myEdgeNewEdge.IsBound(theProEdge) &&
      myEdgeNewEdge(theProEdge).Orientation() == TopAbs_REVERSED)
    anOr = TopAbs::Reverse(anOr);

  TopoDS_Shape F_Edge = theEdge.Oriented(TopAbs_FORWARD);
  F_Edge.Free(Standard_True);
  BB.Add(F_Edge, theVertex.Oriented(anOr));
  if (!Precision::IsInfinite(theParamOnEdge))
  {
    Standard_Real fpar, lpar;
    BRep_Tool::Range(theEdge, fpar, lpar);
    if (anOr == TopAbs_FORWARD)
      BB.Range(theEdge, theParamOnEdge, lpar);
    else
      BB.Range(theEdge, fpar, theParamOnEdge);
  }
}

void BRepOffsetAPI_PatchFaces::ProjectVertexOnNewEdge_2d(const TopoDS_Vertex& theVertex,
                                                         const TopoDS_Edge&   theEdge,
                                                         const TopoDS_Face&   theFace,
                                                         const TopoDS_Edge&   theNewEdge,
                                                         const TopoDS_Face&   theNewFace,
                                                         Standard_Real&       theParam,
                                                         gp_Pnt&              thePnt,
                                                         gp_Pnt2d&            thePnt2d,
                                                         Standard_Real&       theTolReached)
{
  Standard_Real ParamOnEdge = BRep_Tool::Parameter(theVertex, theEdge);
  BRepAdaptor_Curve2d BAcurve2d(theEdge, theFace);
  BRepAdaptor_Curve2d BAnewcurve2d(theNewEdge, theNewFace);
  gp_Pnt2d Point2dFromEdge;
  if (myFacePatchFace.Contains(theFace))
  {
    BRepAdaptor_Curve BAcurve(theEdge);
    gp_Pnt PointOnEdge = BAcurve.Value(ParamOnEdge);
    BRepAdaptor_Surface BAnewsurf(theNewFace);
    Extrema_ExtPS ProjPS(PointOnEdge, BAnewsurf,
                         Precision::PConfusion(), Precision::PConfusion());
    Standard_Integer imin = 1;
    for (Standard_Integer iext = 2; iext <= ProjPS.NbExt(); iext++)
      if (ProjPS.SquareDistance(iext) < ProjPS.SquareDistance(imin))
        imin = iext;
    Standard_Real uu, vv;
    ProjPS.Point(imin).Parameter(uu,vv);
    Point2dFromEdge.SetCoord(uu,vv);
  }
  else
  {
    Point2dFromEdge = BAcurve2d.Value(ParamOnEdge);
  }
  Extrema_ExtPC2d Projector(Point2dFromEdge, BAnewcurve2d);
  Standard_Real Param[4], dist[4];
  gp_Pnt2d Pnt2d[4];
  Param[1] = BAnewcurve2d.FirstParameter();
  Param[2] = BAnewcurve2d.LastParameter();
  Projector.TrimmedSquareDistances(dist[1], dist[2], Pnt2d[1], Pnt2d[2]);
  dist[3] = RealLast();
  if (Projector.IsDone() && Projector.NbExt() > 0)
  {
    Standard_Integer imin = 1;
    for (Standard_Integer i = 2; i <= Projector.NbExt(); i++)
      if (Projector.SquareDistance(i) < Projector.SquareDistance(imin))
        imin = i;
    Param[3] = Projector.Point(imin).Parameter();
    dist[3]  = Projector.SquareDistance(imin);
    Pnt2d[3]   = Projector.Point(imin).Value();
  }

  Standard_Integer imin = 1;
  for (Standard_Integer i = 2; i <= 3; i++)
    if (dist[i] < dist[imin])
      imin = i;

  theParam      = Param[imin];
  //BRepAdaptor_Surface BAnewsurf(theNewFace, Standard_False);
  //thePnt        = BAnewsurf.Value(Pnt[imin].X(), Pnt[imin].Y());
  thePnt2d = Pnt2d[imin];
  BRepAdaptor_Curve BAcurve(theEdge);
  BRepAdaptor_Curve BAnewcurve(theNewEdge);
  thePnt = BAnewcurve.Value(theParam);
  gp_Pnt PointOnEdge = BAcurve.Value(ParamOnEdge);
  theTolReached = PointOnEdge.Distance(thePnt); //sqrt(dist[imin]);
}
