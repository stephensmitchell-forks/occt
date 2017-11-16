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
//#include <Extrema_ExtCC2d.hxx>
#include <Geom2dInt_GInter.hxx>
//#include <BRepExtrema_ExtCC.hxx>
//#include <ShapeFix_Shape.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <Geom_BoundedCurve.hxx>
#include <GeomLib.hxx>

#ifdef DRAW_PatchFaces
#include <DBRep.hxx>
#endif

static Standard_Boolean EdgeContains(const TopoDS_Edge&   theEdge,
                                     const TopoDS_Vertex& theVertex,
                                     const Standard_Boolean isOriented = Standard_False)
{
  Standard_Boolean (TopoDS_Shape::*pCompFunc) (const TopoDS_Shape& other) const =
    (isOriented ? &TopoDS_Shape::IsEqual : &TopoDS_Shape::IsSame);

  TopoDS_Iterator ite(theEdge);
  for (; ite.More(); ite.Next())
    if ((theVertex.*pCompFunc)(ite.Value()))
      return Standard_True;
  
  return Standard_False;
}

static void UpdateVertexTolOnFace(const TopoDS_Vertex& theVertex,
                                  const TopoDS_Edge& theEdge,
                                  const TopoDS_Face& theFace)
{
  if (!EdgeContains(theEdge, theVertex))
    return;

  Standard_Real aParam  = BRep_Tool::Parameter(theVertex, theEdge);
  BRepAdaptor_Curve2d BAcurve2d(theEdge, theFace);
  gp_Pnt2d aPnt2d = BAcurve2d.Value(aParam);

  BRepAdaptor_Surface BAsurf(theFace, Standard_False);
  gp_Pnt aPnt = BAsurf.Value(aPnt2d.X(), aPnt2d.Y());

  gp_Pnt PntVtx = BRep_Tool::Pnt(theVertex);
  Standard_Real aDist = PntVtx.Distance(aPnt);

  BRep_Builder BB;
  BB.UpdateVertex(theVertex, 1.001*aDist);
}

static void ProlongEdge(const TopoDS_Edge& theEdge,
                        const TopoDS_Face& theFace,
                        gp_Pnt&            theFirstPnt,
                        gp_Pnt&            theLastPnt)
{
  BRepAdaptor_Curve2d BAcurve2d(theEdge, theFace);
  gp_Pnt2d EndP2d [2];
  gp_Pnt2d StartP2d [2];
  gp_Vec2d StartVec [2];
  Handle(Geom2d_Curve) Ray [2], BoundLine [4];
  Geom2dAdaptor_Curve GAray [2], GAline [4];

  BAcurve2d.D1(BAcurve2d.FirstParameter(), StartP2d[0], StartVec[0]);
  StartVec[0].Reverse();
  BAcurve2d.D1(BAcurve2d.LastParameter(),  StartP2d[1], StartVec[1]);

  Handle(Geom_Surface) aSurf = BRep_Tool::Surface(theFace);
  Standard_Real Umin, Umax, Vmin, Vmax;
  aSurf->Bounds(Umin, Umax, Vmin, Vmax);
  BoundLine[0] = new Geom2d_Line(gp_Pnt2d( Umin, Vmin ),
                                 gp_Dir2d( 1., 0. ));
  BoundLine[0] = new Geom2d_TrimmedCurve(BoundLine[0], 0., Umax-Umin);
  BoundLine[1] = new Geom2d_Line(gp_Pnt2d( Umax, Vmin ),
                                 gp_Dir2d( 0., 1. ));
  BoundLine[1] = new Geom2d_TrimmedCurve(BoundLine[1], 0., Vmax-Vmin);
  BoundLine[2] = new Geom2d_Line(gp_Pnt2d( Umax, Vmax ),
                                 gp_Dir2d( -1., 0. ));
  BoundLine[2] = new Geom2d_TrimmedCurve(BoundLine[2], 0., Umax-Umin);
  BoundLine[3] = new Geom2d_Line(gp_Pnt2d( Umin, Vmax ),
                                 gp_Dir2d( 0., -1. ));
  BoundLine[3] = new Geom2d_TrimmedCurve(BoundLine[3], 0., Vmax-Vmin);

  GAline[0].Load(BoundLine[0]);
  GAline[1].Load(BoundLine[1]);
  GAline[2].Load(BoundLine[2]);
  GAline[3].Load(BoundLine[3]);

  for (Standard_Integer i = 0; i < 2; i++)
  {
    Ray[i] = new Geom2d_Line(StartP2d[i], StartVec[i]);
    //Ray[i] = new Geom2d_TrimmedCurve(Ray[i], 0., Precision::Infinite());
    GAray[i].Load(Ray[i], 0., Precision::Infinite());
    for (Standard_Integer j = 0; j < 4; j++)
    {
      Geom2dInt_GInter Inters2d(GAray[i], GAline[j],
                                Precision::PConfusion(), Precision::PConfusion());
      if (Inters2d.IsDone() && Inters2d.NbPoints() > 0)
      {
        const IntRes2d_IntersectionPoint& ip = Inters2d.Point(1);
        EndP2d[i] = ip.Value();
        break;
      }
    }
  }

  BRepAdaptor_Surface BAsurf(theFace, Standard_False);
  theFirstPnt = BAsurf.Value(EndP2d[0].X(), EndP2d[0].Y());
  theLastPnt  = BAsurf.Value(EndP2d[1].X(), EndP2d[1].Y());
}

  
static Standard_Real Project2dPointOnSegment(const TopoDS_Edge&  theEdge,
                                             const TopoDS_Face&  theFace,
                                             const Standard_Real theFirstPar,
                                             const Standard_Real theLastPar,
                                             const gp_Pnt2d&     theP2dRef,
                                             Standard_Real&      theParam,
                                             gp_Pnt2d&           theP2d)
{
  Standard_Real fpar, lpar;
  Handle(Geom2d_Curve) PCurve = BRep_Tool::CurveOnSurface(theEdge, theFace, fpar, lpar);
  Geom2dAdaptor_Curve GAcurve2d(PCurve, theFirstPar, theLastPar);
  
  Extrema_ExtPC2d Projector(theP2dRef, GAcurve2d);
  Standard_Real Param[4], dist[4], distmin;
  gp_Pnt2d Pnt2d[4];
  Param[1] = theFirstPar;
  Param[2] = theLastPar;
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

  theParam = Param[imin];
  theP2d   = Pnt2d[imin];
  distmin  = dist[imin];
  return distmin;
}

static void ProjectVertexOnAnotherEdge_2d(const TopoDS_Vertex& theVertex,
                                          const TopoDS_Edge&   theEdge1,
                                          const TopoDS_Edge&   theEdge2,
                                          const TopoDS_Face&   theFace,
                                          const gp_Pnt2d&      /*theP2dRef*/,
                                          Standard_Real&       theSqDistReached,
                                          Standard_Real&       theParamOnFirst,
                                          Standard_Real&       theParamOnSecond)
{
  theParamOnFirst = BRep_Tool::Parameter(theVertex, theEdge1);
  BRepAdaptor_Curve2d FirstBAcurve2d(theEdge1, theFace);
  BRepAdaptor_Curve2d SecondBAcurve2d(theEdge2, theFace);
  gp_Pnt2d Point2dFromEdge1 = FirstBAcurve2d.Value(theParamOnFirst);

  Extrema_ExtPC2d Projector(Point2dFromEdge1, SecondBAcurve2d);
  Standard_Real Param[4], dist[4];
  gp_Pnt2d Pnt2d[4];
  Param[1] = SecondBAcurve2d.FirstParameter();
  Param[2] = SecondBAcurve2d.LastParameter();
  Projector.TrimmedSquareDistances(dist[1], dist[2], Pnt2d[1], Pnt2d[2]);
  //dist[1] = theP2dRef.SquareDistance(Pnt2d[1]);
  //dist[2] = theP2dRef.SquareDistance(Pnt2d[2]);
  dist[3] = RealLast();
  if (Projector.IsDone())
  {
    Standard_Integer imin = 0;
    for (Standard_Integer i = 1; i <= Projector.NbExt(); i++)
    {
      //gp_Pnt2d aPnt2dOnSecondEdge = Projector.Point(i).Value();
      //Standard_Real aDist = theP2dRef.SquareDistance(aPnt2dOnSecondEdge);
      Standard_Real aDist = Projector.SquareDistance(i);
      if (aDist < dist[3])
      {
        dist[3] = aDist;
        imin = i;
      }
    }
    if (imin != 0)
    {
      Param[3] = Projector.Point(imin).Parameter();
      Pnt2d[3] = Projector.Point(imin).Value();
    }
  }

  Standard_Integer imin = 1;
  for (Standard_Integer i = 2; i <= 3; i++)
    if (dist[i] < dist[imin])
      imin = i;

  theParamOnSecond = Param[imin];
  theSqDistReached = dist[imin];
  
  //BRepAdaptor_Surface BAnewsurf(theNewFace, Standard_False);
  //thePnt        = BAnewsurf.Value(Pnt[imin].X(), Pnt[imin].Y());
  
  //thePnt2d = Pnt2d[imin];
  
  //BRepAdaptor_Curve BAcurve(theEdge);
  // BRepAdaptor_Curve BAnewcurve(theNewEdge);
  
  //thePnt = BAnewcurve.Value(theParam);
  
  //gp_Pnt PointOnEdge = BAcurve.Value(ParamOnEdge);
  
  //thePntSol.SetXYZ((thePntOnConst.XYZ() + thePnt.XYZ())/2);
  
  //theTolReached = PointOnEdge.Distance(thePnt); //sqrt(dist[imin]);
  //theTolReached = thePntSol.Distance(thePnt);
}

static Standard_Boolean IntersectIn2d(const TopoDS_Vertex& theNewVertex,
                                      const TopoDS_Edge&   theEdge1,
                                      const TopoDS_Edge&   theEdge2,
                                      const TopoDS_Face&   theFace,
                                      gp_Pnt2d&            theP2dRef,
                                      Standard_Real&       theParamOnFirst,
                                      Standard_Real&       theParamOnSecond,
                                      gp_Pnt&              thePntSol,
                                      gp_Pnt&              thePntOnFirst,
                                      gp_Pnt&              thePntOnSecond,
                                      TColStd_SequenceOfReal& theIntParamsOnFirst,
                                      TColStd_SequenceOfReal& theIntParamsOnSecond)
{
  //Current range
  BRepAdaptor_Curve BAprevcurve(theEdge1);
  BRepAdaptor_Curve BAcurcurve(theEdge2);
  BRepAdaptor_Curve2d BAprevcurve2d(theEdge1, theFace);
  BRepAdaptor_Curve2d BAcurcurve2d(theEdge2, theFace);

  //Maximum range
  /*
  Standard_Real fpar, lpar;
  Handle(Geom_Curve) Curve1 = BRep_Tool::Curve(theEdge1, fpar, lpar);
  Handle(Geom_Curve) Curve2 = BRep_Tool::Curve(theEdge2, fpar, lpar);
  Handle(Geom2d_Curve) PCurve1 = BRep_Tool::CurveOnSurface(theEdge1, theFace, fpar, lpar);
  Handle(Geom2d_Curve) PCurve2 = BRep_Tool::CurveOnSurface(theEdge2, theFace, fpar, lpar);
  GeomAdaptor_Curve BAprevcurve(Curve1);
  GeomAdaptor_Curve BAcurcurve(Curve2);
  Geom2dAdaptor_Curve BAprevcurve2d(PCurve1);
  Geom2dAdaptor_Curve BAcurcurve2d(PCurve2);
  */
  
  Standard_Integer imin = 0;
  //Standard_Real MinSumDist = RealLast();

  Standard_Real ParOnPrev[4], ParOnCur[4], DistRef[4];
  gp_Pnt2d Pnt2d[4];
  DistRef[1] = DistRef[2] = DistRef[3] = RealLast();
  
  Geom2dInt_GInter Inters2d(BAprevcurve2d, BAcurcurve2d,
                            Precision::PConfusion(), Precision::PConfusion());
  if (Inters2d.IsDone())
  {
    for (Standard_Integer i = 1; i <= Inters2d.NbPoints(); i++)
    {
      const IntRes2d_IntersectionPoint& ip = Inters2d.Point(i);
      Standard_Real ParOnFirst  = ip.ParamOnFirst();
      Standard_Real ParOnSecond = ip.ParamOnSecond();
      theIntParamsOnFirst.Append(ParOnFirst);
      theIntParamsOnSecond.Append(ParOnSecond);
      gp_Pnt2d Pint2d = ip.Value();
      //Standard_Real aDist = Pnt2dOnNewEdge.SquareDistance(Pint2d);
      Standard_Real aDist = theP2dRef.SquareDistance(Pint2d);
      if (aDist < DistRef[1])
      {
        DistRef[1] = aDist;
        imin = i;
        ParOnPrev[1] = ParOnFirst;
        ParOnCur[1]  = ParOnSecond;
      }
      //gp_Pnt Pint = BAprevcurve.Value(ip.ParamOnFirst());
    }
    for (Standard_Integer i = 1; i <= Inters2d.NbSegments(); i++)
    {
      const IntRes2d_IntersectionSegment& iseg = Inters2d.Segment(i);
      const IntRes2d_IntersectionPoint& FirstPoint = iseg.FirstPoint();
      const IntRes2d_IntersectionPoint& LastPoint = iseg.LastPoint();
      Standard_Real fparOnFirst = FirstPoint.ParamOnFirst();
      Standard_Real lparOnFirst = LastPoint.ParamOnFirst();
      Standard_Real fparOnSecond = FirstPoint.ParamOnSecond();
      Standard_Real lparOnSecond = LastPoint.ParamOnSecond();
      if (lparOnFirst < fparOnFirst)
      { Standard_Real tmp = fparOnFirst; fparOnFirst = lparOnFirst; lparOnFirst = tmp; }
      if (lparOnSecond < fparOnSecond)
      { Standard_Real tmp = fparOnSecond; fparOnSecond = lparOnSecond; lparOnSecond = tmp; }
      theIntParamsOnFirst.Append(fparOnFirst);
      theIntParamsOnFirst.Append(lparOnFirst);
      theIntParamsOnSecond.Append(fparOnSecond);
      theIntParamsOnSecond.Append(lparOnSecond);
      Standard_Real ParamOnFirst, ParamOnSecond;
      gp_Pnt2d Pnt2dOnFirst, Pnt2dOnSecond;
      Standard_Real aDist = Project2dPointOnSegment(theEdge1, theFace,
                                                    fparOnFirst, lparOnFirst,
                                                    theP2dRef,
                                                    ParamOnFirst, Pnt2dOnFirst);
      Project2dPointOnSegment(theEdge2, theFace,
                              fparOnSecond, lparOnSecond,
                              Pnt2dOnFirst,
                              ParamOnSecond, Pnt2dOnSecond);

      if (aDist < DistRef[1])
      {
        DistRef[1] = aDist;
        imin = i;
        ParOnPrev[1] = ParamOnFirst;
        ParOnCur[1]  = ParamOnSecond;
      }
    }
    /*
    if (imin != 0)
    {
      const IntRes2d_IntersectionPoint& ip = Inters2d.Point(imin);
      ParOnPrev[1] = ip.ParamOnFirst();
      ParOnCur[1]  = ip.ParamOnSecond();
      //theParamOnFirst  = ip.ParamOnFirst();
      //theParamOnSecond = ip.ParamOnSecond();
    }
    */
  }

  //Project
  if (!theNewVertex.IsNull())
  {
    if (EdgeContains(theEdge1, theNewVertex))
    {
      ProjectVertexOnAnotherEdge_2d(theNewVertex, theEdge1, theEdge2, theFace, theP2dRef,
                                    DistRef[2], ParOnPrev[2], ParOnCur[2]);
    }
    if (EdgeContains(theEdge2, theNewVertex))
    {
      ProjectVertexOnAnotherEdge_2d(theNewVertex, theEdge2, theEdge1, theFace, theP2dRef,
                                    DistRef[3], ParOnCur[3], ParOnPrev[3]);
    }
  }

  //Standard_Boolean SolutionFound = Standard_False;
  Standard_Real MinDistRef = RealLast();
  imin = 0;
  for (Standard_Integer i = 1; i <= 3; i++)
  {
    //if (!Precision::IsInfinite(DistRef[i]))
    //SolutionFound = Standard_True;
    if (DistRef[i] < MinDistRef)
    {
      MinDistRef = DistRef[i];
      imin = i;
    }
  }

  if (Precision::IsInfinite(MinDistRef))
    return Standard_False;

  theParamOnFirst  = ParOnPrev[imin];
  theParamOnSecond = ParOnCur[imin];
  thePntOnFirst    = BAprevcurve.Value(theParamOnFirst);
  thePntOnSecond   = BAcurcurve.Value(theParamOnSecond);
  //theP2dRef        = BAcurcurve2d.Value(theParamOnSecond);
  thePntSol.SetXYZ((thePntOnFirst.XYZ() + thePntOnSecond.XYZ())/2);
  return Standard_True;
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
  //aNewEdge = BRepLib_MakeEdge(aCurve, aCurve->FirstParameter(), aCurve->LastParameter()); //???
  aNewEdge = BRepLib_MakeEdge(aCurve, fpar, lpar);
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

  gp_Pnt Pnt1, Pnt2, Pnt3, Pnt4;
  ProlongEdge(aNewEdge, theNewFace1, Pnt1, Pnt2);
  ProlongEdge(aNewEdge, theNewFace2, Pnt3, Pnt4);

  BRepAdaptor_Curve BAcurve(aNewEdge);
  gp_Pnt FirstPnt = BAcurve.Value(BAcurve.FirstParameter());
  gp_Pnt LastPnt  = BAcurve.Value(BAcurve.LastParameter());

  Standard_Real DistFirst1 = FirstPnt.SquareDistance(Pnt1);
  Standard_Real DistFirst2 = FirstPnt.SquareDistance(Pnt3);
  Standard_Real DistLast1  = LastPnt.SquareDistance(Pnt2);
  Standard_Real DistLast2  = LastPnt.SquareDistance(Pnt4);

  gp_Pnt NewFirstPnt, NewLastPnt;
  NewFirstPnt = (DistFirst1 < DistFirst2)? Pnt1 : Pnt3;
  NewLastPnt  = (DistLast1  < DistLast2)?  Pnt2 : Pnt4;

  Handle(Geom_BoundedCurve) aTrCurve = new Geom_TrimmedCurve(aCurve,
                                                             BAcurve.FirstParameter(),
                                                             BAcurve.LastParameter());
  GeomLib::ExtendCurveToPoint(aTrCurve, NewFirstPnt, 1, Standard_False); //before
  GeomLib::ExtendCurveToPoint(aTrCurve, NewLastPnt,  1, Standard_True);  //after
  Handle(Geom_Curve) aCurveExt = aTrCurve;

  BB.UpdateEdge(aNewEdge, aTrCurve, Etol);
  BB.Range(aNewEdge, aTrCurve->FirstParameter(), aTrCurve->LastParameter());

  Handle(Geom_Surface) NewSurf1 = BRep_Tool::Surface(theNewFace1);
  Handle(ShapeAnalysis_Surface) SAS1 = new ShapeAnalysis_Surface(NewSurf1);
  ShapeConstruct_ProjectCurveOnSurface aToolProj;
  aToolProj.Init(SAS1, Precision::Confusion());
  Handle(Geom2d_Curve) NewPCurve1;
  aToolProj.Perform(aCurveExt,
                    aTrCurve->FirstParameter(),
                    aTrCurve->LastParameter(),
                    NewPCurve1);
  Standard_Real TolReached = SAS1->Gap();
  //BB.UpdateEdge(anEdge, NullPCurve, aFace, 0.);
  BB.UpdateEdge(aNewEdge, NewPCurve1, theNewFace1, TolReached);

  Handle(Geom_Surface) NewSurf2 = BRep_Tool::Surface(theNewFace2);
  Handle(ShapeAnalysis_Surface) SAS2 = new ShapeAnalysis_Surface(NewSurf2);
  aToolProj.Init(SAS2, Precision::Confusion());
  Handle(Geom2d_Curve) NewPCurve2;
  aToolProj.Perform(aCurveExt,
                    aTrCurve->FirstParameter(),
                    aTrCurve->LastParameter(),
                    NewPCurve2);
  TolReached = SAS2->Gap();
  BB.UpdateEdge(aNewEdge, NewPCurve2, theNewFace2, TolReached);

  Standard_Real ParamOfMaxDist;
  BOPTools_AlgoTools::ComputeTolerance(theNewFace1, aNewEdge, TolReached, ParamOfMaxDist);
  BB.UpdateEdge(aNewEdge, 1.001*TolReached);
  BOPTools_AlgoTools::ComputeTolerance(theNewFace2, aNewEdge, TolReached, ParamOfMaxDist);
  BB.UpdateEdge(aNewEdge, 1.001*TolReached);
  
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
  //Update tolerance for same parameter
  Standard_Real ParamOfMaxDist;
  BOPTools_AlgoTools::ComputeTolerance(aBoundedNewFace, anEdge, TolReached, ParamOfMaxDist);
  /*
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
  */
  BB.UpdateEdge(anEdge, TolReached);
  ///////
  
  TopoDS_Vertex V1, V2;
  TopExp::Vertices(anEdge, V1, V2);
  BB.UpdateVertex(V1, TolReached);
  BB.UpdateVertex(V2, TolReached);
}

static Standard_Real ProjectPointOnEdge(const gp_Pnt&      thePoint,
                                        const TopoDS_Edge& theEdge,
                                        Standard_Real&     theParam,
                                        gp_Pnt&            thePnt)
{
  BRepAdaptor_Curve BAcurve(theEdge);
  Extrema_ExtPC Projector(thePoint, BAcurve);
  Standard_Real Param[4], dist[4];
  gp_Pnt Pnt[4];
  Param[1] = BAcurve.FirstParameter();
  Param[2] = BAcurve.LastParameter();
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
  Standard_Real MinSqDist = dist[imin];
  return MinSqDist;
}

/*static void ProjectVertexOnNewEdge(const TopoDS_Vertex& theVertex,
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
}*/

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

static TopAbs_Orientation OrientationInEdge(const TopoDS_Vertex& theVertex,
                                            const TopoDS_Edge& theEdge)
{
  TopoDS_Vertex V1, V2;
  TopExp::Vertices(theEdge, V1, V2);
  if (theVertex.IsSame(V1))
    return TopAbs_FORWARD;
  
  return TopAbs_REVERSED;
}*/


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
//function : BRepOffsetAPI_PatchFaces
//purpose  : Empty constructor
//=======================================================================
BRepOffsetAPI_PatchFaces::BRepOffsetAPI_PatchFaces()
{}

//=======================================================================
//function : Clear
//purpose  : 
//=======================================================================
void BRepOffsetAPI_PatchFaces::Clear()
{
  myFacePatchFace.Clear();
  myFaceNewFace.Clear();
  myNewFaceBoundedFace.Clear();
  myEdgeNewEdge.Clear();
  myVertexNewVertex.Clear();
  myTangentEdges.Clear();
  mySmoothEdges.Clear();
  myEFmap.Clear();
  myVEmap.Clear();
  myVFmap.Clear();
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
//function : GetPatchFace
//purpose  : 
//=======================================================================
TopoDS_Face BRepOffsetAPI_PatchFaces::GetPatchFace(const TopoDS_Face& theFace) const
{
  const TopoDS_Shape* anInitialPatchFace = myFacePatchFace.Seek(theFace);
  if (!anInitialPatchFace)
  {
    TopoDS_Face aNullFace;
    return aNullFace;
  }

  const TopoDS_Face& aPatch = TopoDS::Face(myNewFaceBoundedFace(*anInitialPatchFace));
  return aPatch;
}

//=======================================================================
//function : Get
//purpose  : 
//=======================================================================
void BRepOffsetAPI_PatchFaces::GetAdjacentFaces(const TopoDS_Face& theFace,
                                                TopTools_ListOfShape& theNeighbors) const
{
  const TopoDS_Shape* anInitialPatchFace = myFacePatchFace.Seek(theFace);
  if (!anInitialPatchFace)
    return;

  // Fence map to avoid duplicates in the list
  TopTools_MapOfShape aMFence;
  // Find all adjacent faces in the initial solid
  TopExp_Explorer anExpV(theFace, TopAbs_VERTEX);
  for (; anExpV.More(); anExpV.Next())
  {
    const TopoDS_Shape& aVertex = anExpV.Current();
    const TopTools_ListOfShape& aLFaces = myVFmap.Find(aVertex);

    TopTools_ListIteratorOfListOfShape aItLF(aLFaces);
    for (; aItLF.More(); aItLF.Next())
    {
      const TopoDS_Shape& aFace = aItLF.Value();
      if (aFace.IsSame(theFace) || !aMFence.Add(aFace))
        continue;

      // Get face from the result solid
      const TopoDS_Shape* pNewFace = myFaceNewFace.Seek(aFace);
      if (pNewFace)
        theNeighbors.Append(myNewFaceBoundedFace(*pNewFace));
      else
        theNeighbors.Append(aFace);
    }
  }
}

//=======================================================================
//function : Build
//purpose  : 
//=======================================================================

void BRepOffsetAPI_PatchFaces::Build()
{
  TopExp::MapShapesAndUniqueAncestors(myInitialShape, TopAbs_EDGE, TopAbs_FACE, myEFmap);
  //TopTools_IndexedDataMapOfShapeListOfShape VEmap;
  TopExp::MapShapesAndUniqueAncestors(myInitialShape, TopAbs_VERTEX, TopAbs_EDGE, myVEmap);
  for (Standard_Integer i = 1; i <= myVEmap.Extent(); i++)
  {
    const TopoDS_Shape& aVertex = myVEmap.FindKey(i);
    TopTools_ListOfShape aFaceList;
    TopTools_MapOfShape aFaceMap;
    const TopTools_ListOfShape& Elist = myVEmap(i);
    TopTools_ListIteratorOfListOfShape itl(Elist);
    for (; itl.More(); itl.Next())
    {
      const TopoDS_Edge& anEdge = TopoDS::Edge(itl.Value());
      if (BRep_Tool::Degenerated(anEdge))
        continue;
      const TopTools_ListOfShape& Flist = myEFmap.FindFromKey(anEdge);
      TopTools_ListIteratorOfListOfShape itl2(Flist);
      for (; itl2.More(); itl2.Next())
      {
        const TopoDS_Shape& aFace = itl2.Value();
        if (aFaceMap.Add(aFace))
          aFaceList.Append(aFace);
      }
    }
    myVFmap.Bind(aVertex, aFaceList);
  }

#ifdef DRAW_PatchFaces
  TopoDS_Compound aComp;
  BRep_Builder().MakeCompound(aComp);
#endif
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
      if (BRep_Tool::Degenerated(anEdge))
        continue;
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
#ifdef DRAW_PatchFaces
        BRep_Builder().Add(aComp, anEdge);
#endif
        continue;
      }
                                                                          
      aNeighborFace.Orientation(TopAbs_FORWARD);
      TopoDS_Face aNewFace;
      BRepOffset_Tool::EnLargeFace(aNeighborFace, aNewFace,
                                   Standard_True,Standard_True,Standard_True,Standard_True,Standard_True,
                                   Standard_False); //not too big
      myFaceNewFace.Add(aNeighborFace, aNewFace);
#ifdef DRAW_PatchFaces
      BRep_Builder().Add(aComp, aNewFace);
#endif
    }
  }
#ifdef DRAW_PatchFaces
  DBRep::Set("nf",aComp);
#endif

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
        if (BRep_Tool::Degenerated(anEdge))
          continue;
        BRepAdaptor_Curve BAcurve(anEdge);
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
            mySmoothEdges.Add(anEdge);
            myEdgeNewEdge.Bind(anEdge.Oriented(TopAbs_FORWARD), aNewEdge);
            continue;
          }
          
          BRepLib::BuildCurves3d(aNewFace);
          BRepLib::BuildCurves3d(aNewNeighborFace);

          // consider intersection of the faces invalid
          // if the tolerance of the section edges is too big
          const Standard_Real aSecTolMax = 1.;
          
          BRepAlgoAPI_Section SecBuilder(aNewFace, aNewNeighborFace, Standard_False);
          SecBuilder.Approximation(Standard_True);
          SecBuilder.ComputePCurveOn1(Standard_True);
          SecBuilder.ComputePCurveOn2(Standard_True);
          SecBuilder.Build();
          TopoDS_Shape aSection = SecBuilder.Shape();
          Standard_Boolean Success = Standard_True;
          TopExp_Explorer ExpSec(aSection, TopAbs_EDGE);
          if (ExpSec.More())
          {
            aNewEdge = TopoDS::Edge(ExpSec.Current());
            if (BRep_Tool::Tolerance(aNewEdge) > aSecTolMax)
              Success = Standard_False;
          }
          else //no intersection
          {
            cout<<endl<<"No intersecion => smooth edge"<<endl;
            Success = Standard_False;
          }
          if (Success)
          {
            ExpSec.Next();
            if (ExpSec.More())
            {
              cout<<endl<<"More than one intersecion"<<endl;
              gp_Pnt MidPnt = BAcurve.Value((BAcurve.FirstParameter() + BAcurve.LastParameter())/2);
              Standard_Real aParam;
              gp_Pnt aPnt;
              Standard_Real MinSqDistProj = ProjectPointOnEdge(MidPnt, aNewEdge, aParam, aPnt);
              for (; ExpSec.More(); ExpSec.Next())
              {
                TopoDS_Edge aSecEdge = TopoDS::Edge(ExpSec.Current());
                if (BRep_Tool::Tolerance(aSecEdge) > aSecTolMax)
                {
                  Success = Standard_False;
                  break;
                }

                Standard_Real aSqDistProj = ProjectPointOnEdge(MidPnt, aSecEdge, aParam, aPnt);
                if (aSqDistProj < MinSqDistProj)
                {
                  MinSqDistProj = aSqDistProj;
                  aNewEdge = aSecEdge;
                }
              }
            }
          }
          if (!Success) //a smooth edge with bigger angle
          {
            if (myFacePatchFace.Contains(aFace)) //project onto patch
            {
              UpdateEdgeByProjectionOfPCurve(anEdge, aNewFace, aBoundedNewFace);

              //Remove neighbor face from <myFaceNewFace>
              myFaceNewFace.Swap(myFaceNewFace.FindIndex(aNeighborFace), myFaceNewFace.Extent());
              myFaceNewFace.RemoveLast();
          
              myTangentEdges.Add(anEdge);
              UpdatedConstEdges.Add(anEdge);
            }
            else
            {
              aNewEdge = MakeNewEdgeWithOldPcurvesOnNewSurfaces(anEdge,
                                                                aFace, aBoundedNewFace,
                                                                aNeighborFace, aBoundedNewNeighborFace);
              mySmoothEdges.Add(anEdge);
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
          //BRepAdaptor_Curve BAcurve(anEdge);
          BRepAdaptor_Curve BAnewcurve(aNewEdge);
          gp_Pnt FirstPnt, FirstNewPnt, aPnt;
          gp_Vec DirOnCurve, DirOnNewCurve;
          BAcurve.D1(BAcurve.FirstParameter(), FirstPnt, DirOnCurve);
          Standard_Real ParamOnNewEdge;
          ProjectPointOnEdge(FirstPnt, aNewEdge, ParamOnNewEdge, aPnt);
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
      if (BRep_Tool::Degenerated(FirstEdge))
      {
        wexp.Next();
        FirstEdge = wexp.Current();
      }
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
          //Standard_Real fpar, lpar;
          //BRep_Tool::Range(FirstEdge, fpar, lpar);
          //BB.Range(FirstNewEdge, fpar, lpar);
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
        if (BRep_Tool::Degenerated(CurEdge))
          continue;
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
                                                    const TopoDS_Vertex& theCurVertex,
                                                    const TopoDS_Face& theFace,
                                                    const TopoDS_Face& theNewFace,
                                                    const TopoDS_Face& theBoundedNewFace)
{
  if (thePrevEdge == theCurEdge) //only one edge in wire in fact
    return;
  
  CorrectOrientationOfSeam(thePrevEdge, theFace,
                           thePrevNewEdge, theNewFace);
  CorrectOrientationOfSeam(theCurEdge, theFace,
                           theCurNewEdge, theNewFace);
  
  BRep_Builder BB;
  BRepAdaptor_Surface BAsurf(theBoundedNewFace, Standard_False);
  Standard_Boolean IsConstVertex = Standard_False;
  
  TopoDS_Vertex CurNewVertex;
  if (myVertexNewVertex.IsBound(theCurVertex))
    CurNewVertex = TopoDS::Vertex(myVertexNewVertex(theCurVertex));
  else
  {
    IsConstVertex = (!myEdgeNewEdge.IsBound(thePrevEdge) ||
                     !myEdgeNewEdge.IsBound(theCurEdge)  ||
                     IsMoreThan3Edges(theCurVertex));
    if (IsConstVertex)
      CurNewVertex = theCurVertex;
  }

  TopAbs_Orientation OrientationOfCurVertexInPrev = theCurVertex.Orientation();
  
  if (myEdgeNewEdge.IsBound(thePrevEdge) &&
      myEdgeNewEdge.IsBound(theCurEdge)) //two new edges: intersect
  {
    Standard_Real ParamOnConst, ParamOnPrevRef, ParamOnCurRef, ParamOnPrev, ParamOnCur, TolProj;
    gp_Pnt PntOnConst, PntOnPrevRef, PntOnCurRef, PntOnPrev, PntOnCur, PntSol;
    gp_Pnt2d Pnt2dOnPrev, Pnt2dOnCur;
    ProjectVertexOnNewEdge_2d(theCurVertex,
                              thePrevEdge, theFace,
                              thePrevNewEdge, theBoundedNewFace,
                              ParamOnConst, ParamOnPrevRef,
                              PntOnConst, PntOnPrevRef, PntSol,
                              Pnt2dOnPrev, TolProj);
    ProjectVertexOnNewEdge_2d(theCurVertex,
                              theCurEdge, theFace,
                              theCurNewEdge, theBoundedNewFace,
                              ParamOnConst, ParamOnCurRef,
                              PntOnConst, PntOnCurRef, PntSol,
                              Pnt2dOnCur, TolProj);
    gp_Pnt2d P2dRef((Pnt2dOnPrev.XY() + Pnt2dOnCur.XY())/2);
    gp_Pnt   P3dRef = BAsurf.Value(P2dRef.X(), P2dRef.Y());

    //Standard_Real DistIn3d = PntOnPrev.Distance(PntOnCur);

    Standard_Real TolReached;
    gp_Pnt PntVtx = (CurNewVertex.IsNull())?
      BRep_Tool::Pnt(theCurVertex) : BRep_Tool::Pnt(CurNewVertex);
    
    gp_Pnt Pint;
    TColStd_SequenceOfReal IntParamsOnFirst, IntParamsOnSecond;
    Standard_Boolean IntersectSuccess = Standard_False;

    IntersectSuccess = IntersectIn2d(CurNewVertex,
                                     thePrevNewEdge, theCurNewEdge, theBoundedNewFace, P2dRef,
                                     ParamOnPrev, ParamOnCur, Pint, PntOnPrev, PntOnCur,
                                     IntParamsOnFirst, IntParamsOnSecond);
    //PntOnPrev = PntOnCur = Pint;

    if (IntersectSuccess)
    {
      TopoDS_Edge ThirdEdge;
      if (AreSmoothlyConnected(thePrevEdge, theCurEdge, theCurVertex, theFace, ThirdEdge))
      {
        IntersectSuccess = Standard_False;
        TopoDS_Edge NewThirdEdge = ThirdEdge;
        if (myEdgeNewEdge.IsBound(ThirdEdge))
          NewThirdEdge = TopoDS::Edge(myEdgeNewEdge(ThirdEdge));
        Standard_Real aParam;
        gp_Pnt PntOnThird, PntProjRef, PntProjInt;
        Standard_Real MinSqDistProjRef = ProjectPointOnEdge(P3dRef, NewThirdEdge, aParam, PntProjRef);
        Standard_Real MinSqDistProjInt = ProjectPointOnEdge(Pint,   NewThirdEdge, aParam, PntProjInt);
        PntOnThird = (MinSqDistProjRef < MinSqDistProjInt)? PntProjRef : PntProjInt;
        ProjectPointOnEdge(PntOnThird, thePrevNewEdge, ParamOnPrevRef, PntOnPrevRef);
        ProjectPointOnEdge(PntOnThird, theCurNewEdge, ParamOnCurRef, PntOnCurRef);
        P3dRef.SetXYZ((PntOnPrevRef.XYZ() + PntOnCurRef.XYZ())/2);
      }
    }
    
    if (!IntersectSuccess)
    {
      Pint = P3dRef; //BAsurf.Value(P2dRef.X(), P2dRef.Y());
      ParamOnPrev = ParamOnPrevRef;
      ParamOnCur  = ParamOnCurRef;
      PntOnPrev = PntOnPrevRef;
      PntOnCur  = PntOnCurRef;
      IntParamsOnFirst.Clear();
      IntParamsOnSecond.Clear();
    }
      
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
      Standard_Real DistToPrev = PntVtx.Distance(PntOnPrev);
      Standard_Real DistToCur  = PntVtx.Distance(PntOnCur);
      TolReached = Max(DistToPrev, DistToCur); //PntVtx.Distance(Pint);
    }
    BB.UpdateVertex(CurNewVertex, 1.001*TolReached);

    PutVertexToEdge(CurNewVertex, OrientationOfCurVertexInPrev,
                    thePrevNewEdge, thePrevEdge,
                    theFace, ParamOnPrev);
    
    PutVertexToEdge(CurNewVertex, TopAbs::Reverse(OrientationOfCurVertexInPrev),
                    theCurNewEdge, theCurEdge,
                    theFace, ParamOnCur);

    //Update tolerance of <CurNewVertex> by distances to inner points of intersection
    UpdateVertexTol(CurNewVertex,
                    thePrevNewEdge, thePrevEdge, OrientationOfCurVertexInPrev,
                    theCurNewEdge, theCurEdge,
                    theFace,
                    IntParamsOnFirst, IntParamsOnSecond);
    
    UpdateVertexTolOnAllFaces(CurNewVertex, theCurVertex);
  } //two new edges: intersect
  else if (myEdgeNewEdge.IsBound(thePrevEdge) ||
           myEdgeNewEdge.IsBound(theCurEdge)) //one constant edge: project point onto curve
  {
    TopoDS_Edge ConstantEdge, ModifiedEdge, NewEdge;
    TopAbs_Orientation OrForNew;
    if (myEdgeNewEdge.IsBound(thePrevEdge))
    {
      ConstantEdge = theCurEdge;
      ModifiedEdge = thePrevEdge;
      NewEdge = thePrevNewEdge;
      OrForNew = OrientationOfCurVertexInPrev;
    }
    else
    {
      ConstantEdge = thePrevEdge;
      ModifiedEdge = theCurEdge;
      NewEdge = theCurNewEdge;
      OrForNew = TopAbs::Reverse(OrientationOfCurVertexInPrev);
    }

    Standard_Real ParamOnConstEdge, ParamOnNewEdge, TolReached;
    gp_Pnt PntOnConstEdge, PntOnNewEdge, PntSol;
    gp_Pnt2d Pnt2dOnNewEdge;
    ProjectVertexOnNewEdge_2d(theCurVertex,
                              ModifiedEdge, theFace,
                              NewEdge, theBoundedNewFace,
                              ParamOnConstEdge, ParamOnNewEdge,
                              PntOnConstEdge, PntOnNewEdge, PntSol,
                              Pnt2dOnNewEdge, TolReached);

    TColStd_SequenceOfReal IntParamsOnFirst, IntParamsOnSecond;
    IntersectIn2d(CurNewVertex,
                  ConstantEdge, NewEdge, theBoundedNewFace, Pnt2dOnNewEdge,
                  ParamOnConstEdge, ParamOnNewEdge, PntSol, PntOnConstEdge, PntOnNewEdge,
                  IntParamsOnFirst, IntParamsOnSecond);

    gp_Pnt PntVtx = BRep_Tool::Pnt(CurNewVertex);
    TolReached = PntVtx.Distance(PntOnNewEdge);
    BB.UpdateVertex(CurNewVertex, 1.001*TolReached);
    
    PutVertexToEdge(CurNewVertex, OrForNew,
                    NewEdge, ModifiedEdge,
                    theFace, ParamOnNewEdge);
    
    //Update tolerance of <CurNewVertex> by distances to inner points of intersection
    UpdateVertexTol(CurNewVertex,
                    ConstantEdge, ConstantEdge, TopAbs::Reverse(OrForNew), //OrForConstant,
                    NewEdge, ModifiedEdge,
                    theFace,
                    IntParamsOnFirst, IntParamsOnSecond);

    UpdateVertexTolOnAllFaces(CurNewVertex, theCurVertex);
  } //else (one constant edge: project point onto curve)
  else //two constant edges
  {
    //nothing ?
  }
}


void BRepOffsetAPI_PatchFaces::PutVertexToEdge(const TopoDS_Vertex&     theVertex,
                                               const TopAbs_Orientation theProVertexOrientation,
                                               const TopoDS_Edge&       theEdge,
                                               const TopoDS_Edge&       theProEdge,
                                               const TopoDS_Face&       theProFace,
                                               const Standard_Real      theParamOnEdge)
{
  BRep_Builder BB;
  
  TopAbs_Orientation anOr = theProVertexOrientation;
  if (theProEdge.Orientation() == TopAbs_REVERSED)
    anOr = TopAbs::Reverse(anOr);

  if (BRepTools::IsReallyClosed(theProEdge, theProFace))
  {
    if (theEdge.Orientation() != theProEdge.Orientation())
      anOr = TopAbs::Reverse(anOr);
  }
  else
  {
    if (myEdgeNewEdge.IsBound(theProEdge) &&
        myEdgeNewEdge(theProEdge).Orientation() == TopAbs_REVERSED)
      anOr = TopAbs::Reverse(anOr);
  }

  TopoDS_Edge F_Edge = TopoDS::Edge(theEdge.Oriented(TopAbs_FORWARD));
  TopoDS_Vertex aVertWithOr = TopoDS::Vertex(theVertex.Oriented(anOr));
  Standard_Boolean EdgeContainsVertex = EdgeContains(F_Edge, aVertWithOr, Standard_True);
  
  if (!EdgeContainsVertex)
  {
    F_Edge.Free(Standard_True);

    Standard_Real aTolE = BRep_Tool::Tolerance(TopoDS::Edge(F_Edge));
    BB.UpdateVertex(aVertWithOr, aTolE);

    BB.Add(F_Edge, aVertWithOr);
  }

  // trim edge by the parameter of vertex on edge
  Standard_Real fpar, lpar;
  BRep_Tool::Range(theEdge, fpar, lpar);

  Standard_Real NewFirst = fpar, NewLast = lpar;
  if (anOr == TopAbs_FORWARD)
    NewFirst = theParamOnEdge;
  else
    NewLast = theParamOnEdge;

  BB.Range(theEdge, NewFirst, NewLast);
}

void BRepOffsetAPI_PatchFaces::ProjectVertexOnNewEdge_2d(const TopoDS_Vertex& theVertex,
                                                         const TopoDS_Edge&   theEdge,
                                                         const TopoDS_Face&   theFace,
                                                         const TopoDS_Edge&   theNewEdge,
                                                         const TopoDS_Face&   theNewFace,
                                                         Standard_Real&       theParamOnConst,
                                                         Standard_Real&       theParam,
                                                         gp_Pnt&              thePntOnConst,
                                                         gp_Pnt&              thePnt,
                                                         gp_Pnt&              thePntSol,
                                                         gp_Pnt2d&            thePnt2d,
                                                         Standard_Real&       theTolReached)
{
  Standard_Real ParamOnEdge = BRep_Tool::Parameter(theVertex, theEdge);
  theParamOnConst = ParamOnEdge;
  BRepAdaptor_Curve2d BAcurve2d(theEdge, theFace);
  BRepAdaptor_Curve2d BAnewcurve2d(theNewEdge, theNewFace);
  gp_Pnt2d Point2dFromEdge;
  //
  BRepAdaptor_Curve BAcurve(theEdge);
  gp_Pnt PointOnEdge = BAcurve.Value(ParamOnEdge);
  thePntOnConst = PointOnEdge;
  //
  if (myFacePatchFace.Contains(theFace))
  {
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
  //BRepAdaptor_Curve BAcurve(theEdge);
  BRepAdaptor_Curve BAnewcurve(theNewEdge);
  thePnt = BAnewcurve.Value(theParam);
  //gp_Pnt PointOnEdge = BAcurve.Value(ParamOnEdge);
  thePntSol.SetXYZ((thePntOnConst.XYZ() + thePnt.XYZ())/2);
  //theTolReached = PointOnEdge.Distance(thePnt); //sqrt(dist[imin]);
  theTolReached = thePntSol.Distance(thePnt);
}

Standard_Boolean BRepOffsetAPI_PatchFaces::IsMoreThan3Edges(const TopoDS_Vertex& theVertex)
{
  const TopTools_ListOfShape& Flist = myVFmap(theVertex);
  TopTools_ListIteratorOfListOfShape itl(Flist);
  for (; itl.More(); itl.Next())
  {
    const TopoDS_Shape& aFace = itl.Value();
    if (!myFaceNewFace.Contains(aFace))
      return Standard_True;
  }

  return Standard_False;
}

Standard_Boolean BRepOffsetAPI_PatchFaces::AreSmoothlyConnected(const TopoDS_Edge&   theEdge1,
                                                                const TopoDS_Edge&   theEdge2,
                                                                const TopoDS_Vertex& theVertex,
                                                                const TopoDS_Face&   theFace,
                                                                TopoDS_Edge&         theThirdEdge)
{
  TopoDS_Shape Face1, Face2;
  TopTools_ListIteratorOfListOfShape itl;
  
  const TopTools_ListOfShape& Flist1 = myEFmap.FindFromKey(theEdge1);
  for (itl.Initialize(Flist1); itl.More(); itl.Next())
  {
    const TopoDS_Shape& aFace = itl.Value();
    if (!aFace.IsSame(theFace))
    {
      Face1 = aFace;
      break;
    }
  }

  const TopTools_ListOfShape& Flist2 = myEFmap.FindFromKey(theEdge2);
  for (itl.Initialize(Flist2); itl.More(); itl.Next())
  {
    const TopoDS_Shape& aFace = itl.Value();
    if (!aFace.IsSame(theFace))
    {
      Face2 = aFace;
      break;
    }
  }

  const TopTools_ListOfShape& Elist = myVEmap.FindFromKey(theVertex);
  for (itl.Initialize(Elist); itl.More(); itl.Next())
  {
    const TopoDS_Edge& anEdge = TopoDS::Edge(itl.Value());
    if (anEdge.IsSame(theEdge1) ||
        anEdge.IsSame(theEdge2))
      continue;
    const TopTools_ListOfShape& Flist = myEFmap.FindFromKey(anEdge);
    if (Face1.IsSame(Flist.First()) && Face2.IsSame(Flist.Last()) ||
        Face1.IsSame(Flist.Last())  && Face2.IsSame(Flist.First()))
    {
      theThirdEdge = anEdge;
      break;
    }
  }

  return (mySmoothEdges.Contains(theThirdEdge));
}

void BRepOffsetAPI_PatchFaces::UpdateVertexTolOnAllFaces(const TopoDS_Vertex& theVertex,
                                                         const TopoDS_Vertex& theProVertex)
{
  const TopTools_ListOfShape& Elist = myVEmap.FindFromKey(theProVertex);
  TopTools_ListIteratorOfListOfShape ite(Elist);

  for (; ite.More(); ite.Next())
  {
    const TopoDS_Edge& anEdge = TopoDS::Edge(ite.Value());
    
    TopoDS_Edge NewEdge = anEdge;
    if (myEdgeNewEdge.IsBound(anEdge))
      NewEdge = TopoDS::Edge(myEdgeNewEdge(anEdge));

    const TopTools_ListOfShape& Flist = myEFmap.FindFromKey(anEdge);
    TopTools_ListIteratorOfListOfShape itf(Flist);
    for (; itf.More(); itf.Next())
    {
      const TopoDS_Face& aFace = TopoDS::Face(itf.Value());

      TopoDS_Face aBoundedNewFace = aFace;
      if (myFaceNewFace.Contains(aFace))
      {
        TopoDS_Shape aNewFace = myFaceNewFace.FindFromKey(aFace);
        aBoundedNewFace = TopoDS::Face(myNewFaceBoundedFace(aNewFace));
      }

      UpdateVertexTolOnFace(theVertex, NewEdge, aBoundedNewFace);
    }
  }
}

void BRepOffsetAPI_PatchFaces::UpdateVertexTol(const TopoDS_Vertex&     theVertex,
                                               const TopoDS_Edge&       theEdge1,
                                               const TopoDS_Edge&       theProEdge1,
                                               const TopAbs_Orientation theOrOfVertexForEdge1,
                                               const TopoDS_Edge&       theEdge2,
                                               const TopoDS_Edge&       theProEdge2,
                                               const TopoDS_Face&       theFace,
                                               TColStd_SequenceOfReal&  theIntParamsOnFirst,
                                               TColStd_SequenceOfReal&  theIntParamsOnSecond)
{
  BRepAdaptor_Curve BAcurve1(theEdge1);
  BRepAdaptor_Curve BAcurve2(theEdge2);
  gp_Pnt PntVtx = BRep_Tool::Pnt(theVertex);
  BRep_Builder BB;
  
  for (Standard_Integer i = 1; i <= theIntParamsOnFirst.Length(); i++)
  {
    Standard_Real aParamOnFirst  = theIntParamsOnFirst(i);
    Standard_Real aParamOnSecond = theIntParamsOnSecond(i);
    if (aParamOnFirst  <= BAcurve1.FirstParameter() ||
        aParamOnFirst  >= BAcurve1.LastParameter() ||
        aParamOnSecond <= BAcurve2.FirstParameter() ||
        aParamOnSecond >= BAcurve2.LastParameter())
      continue;
    
    gp_Pnt aPnt = BAcurve1.Value(aParamOnFirst);
    Standard_Real aDist = PntVtx.Distance(aPnt);
    BB.UpdateVertex(theVertex, 1.001*aDist);

    PutVertexToEdge(theVertex, theOrOfVertexForEdge1,
                    theEdge1, theProEdge1,
                    theFace, aParamOnFirst);
    
    PutVertexToEdge(theVertex, TopAbs::Reverse(theOrOfVertexForEdge1),
                    theEdge2, theProEdge2,
                    theFace, aParamOnSecond);
  }
}
