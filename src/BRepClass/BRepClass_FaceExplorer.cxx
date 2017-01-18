// Created on: 1992-11-19
// Created by: Remi LEQUETTE
// Copyright (c) 1992-1999 Matra Datavision
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

//  Modified by skv - Thu Jul 13 17:42:58 2006 OCC12627
//  Total rewriting of the method Segment; add the method OtherSegment.

#include <BRep_Tool.hxx>
#include <BRepClass_FaceExplorer.hxx>
#include <BRepTools.hxx>
#include <BRepTools_WireExplorer.hxx>
#include <Geom2d_Curve.hxx>
#include <gp_Lin2d.hxx>
#include <gp_Pnt2d.hxx>
#include <Precision.hxx>
#include <TopExp.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Wire.hxx>
#include <NCollection_Vector.hxx>
#include <TopTools_MapOfShape.hxx>
#include <NCollection_IncAllocator.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <TopoDS_Vertex.hxx>
#include <Geom2d_Line.hxx>
#include <ElCLib.hxx>
#include <Adaptor3d_CurveOnSurface.hxx>
#include <Geom2dAdaptor_HCurve.hxx>
#include <GeomAdaptor_HSurface.hxx>
#include <TopClass_RayInfo.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <TopClass_GeomVertex.hxx>

static Standard_Real SquareDistanceOnPeriodic(const gp_Pnt2d &theP1,
  const gp_Pnt2d &theP2,
  const Standard_Real theUPeriod,
  const Standard_Real theVPeriod);

static const Standard_Real Probing_Start = 0.123;
static const Standard_Real Probing_End = 0.7;
static const Standard_Real Probing_Step = 0.2111;

//=======================================================================
//function : BRepClass_FaceExplorer
//purpose  : BRepTools_WireExplorer works better with
//            and more reliable with FORWARD-oriented face.
//=======================================================================
BRepClass_FaceExplorer::BRepClass_FaceExplorer(const TopoDS_Face& theF) :
      myFace(TopoDS::Face(theF.Oriented(TopAbs_FORWARD)))
{
  Handle(NCollection_IncAllocator) anAlloc = new NCollection_IncAllocator;
  TopLoc_Location aLocation;
  GeomAdaptor_Surface anAdS(BRep_Tool::Surface(myFace, aLocation));

  const Standard_Real aUPeriod = anAdS.IsUPeriodic() ? anAdS.UPeriod() : 0.0;
  const Standard_Real aVPeriod = anAdS.IsVPeriodic() ? anAdS.VPeriod() : 0.0;

  //
  for (TopExp_Explorer anExpW(myFace, TopAbs_WIRE); anExpW.More(); anExpW.Next())
  {
    const TopoDS_Wire &aWir = TopoDS::Wire(anExpW.Current());
    NCollection_List<TopClass_GeomEdge> anEList(anAlloc);
    myMapWE.Add(aWir, anEList);
  }

  InitWires();

  for (; myWExplorer.More(); myWExplorer.Next())
  {
    const TopoDS_Wire &aWir = TopoDS::Wire(myWExplorer.Key());
    NCollection_List<TopClass_GeomEdge>& anEList = myWExplorer.ChangeValue();
    anEList.Clear();

    BRepTools_WireExplorer anExp(aWir, myFace);

    TopoDS_Edge anE1 = anExp.Current(), aFirstEdge, aLastEdge;

    if (anE1.IsNull())
    {
      // It is possible if aWir contains
      // only INTERNAL/EXTERNAL edges.

      continue;
    }

    aFirstEdge = anE1;
    SimpleAdd(anE1);
    anExp.Next();

    if (!anExp.More())
    {
      // The wire contains only single edge.
      // But this edge can be closed itself (e.g. circle).

      TopoDS_Vertex aVf, aVl;
      TopExp::Vertices(anE1, aVf, aVl);
      if(!aVf.IsNull() && aVf.IsSame(aVl))
        AddPseudo(anE1, anE1, aVf.IsNull() ? aVl : aVf, aUPeriod, aVPeriod);

      continue;
    }

    for (; anExp.More(); anExp.Next())
    {
      const TopoDS_Edge& anE2 = anExp.Current();
      aLastEdge = anE2;
      SimpleAdd(anE2);

      const TopoDS_Vertex &aVertCurr = anExp.CurrentVertex();
      AddPseudo(anE1, anE2, aVertCurr, aUPeriod, aVPeriod);
      anE1 = anE2;
    }

    if (aFirstEdge.IsNull() || aLastEdge.IsNull())
      continue;

    TopoDS_Vertex aVf, aVl;
    TopExp::Vertices(aFirstEdge, aVf, aVl);
    if (!aVf.IsNull() && aVf.IsSame(aVl))
    {
      // Possible reason is the NOT-CLOSED edge
      // has only single vertex and is covered by it.
      // Pseudo-edge is not added and this case is invalid.
      continue;
    }

    TopExp::Vertices(aLastEdge, aVf, aVl);
    if (!aVf.IsNull() && aVf.IsSame(aVl))
    {
      // Possible reason is the NOT-CLOSED edge
      // has only single vertex and is covered by it.
      // Pseudo-edge is not added and this case is invalid.
      continue;
    }

    if (!TopExp::CommonVertex(aFirstEdge, aLastEdge, aVf))
      continue;

    AddPseudo(aFirstEdge, aLastEdge, aVf, aUPeriod, aVPeriod);
  }

  //InitWires();
  //InitEdges();
}

//=======================================================================
//function : CheckPoint
//purpose  : 
//=======================================================================
Standard_Boolean  BRepClass_FaceExplorer::CheckPoint(gp_Pnt2d& thePoint)
{
  Standard_Real anUMin = 0.0, anUMax = 0.0, aVMin = 0.0, aVMax = 0.0;
  TopLoc_Location aLocation;
  const Handle(Geom_Surface)& aSurface = BRep_Tool::Surface(myFace, aLocation);
  aSurface->Bounds(anUMin, anUMax, aVMin, aVMax);
  if (Precision::IsInfinite(anUMin) || Precision::IsInfinite(anUMax) ||
      Precision::IsInfinite(aVMin) || Precision::IsInfinite(aVMax))
  {
    BRepTools::UVBounds(myFace, anUMin, anUMax, aVMin, aVMax);
    if (Precision::IsInfinite(anUMin) || Precision::IsInfinite(anUMax) ||
        Precision::IsInfinite(aVMin) || Precision::IsInfinite(aVMax))
    {
      return Standard_True;
    }
  }

  gp_Pnt2d aCenterPnt(( anUMin + anUMax ) / 2, ( aVMin + aVMax ) / 2);
  Standard_Real aDistance = aCenterPnt.Distance(thePoint);
  if (Precision::IsInfinite(aDistance))
  {
    thePoint.SetCoord(anUMin - ( anUMax - anUMin ),
                       aVMin - ( aVMax - aVMin ));
    return Standard_False;
  }
  else
  {
    Standard_Real anEpsilon = Epsilon(aDistance);
    if (anEpsilon > Max(anUMax - anUMin, aVMax - aVMin))
    {
      gp_Vec2d aLinVec(aCenterPnt, thePoint);
      gp_Dir2d aLinDir(aLinVec);
      thePoint = aCenterPnt.XY() + aLinDir.XY() * ( 2. * anEpsilon );
      return Standard_False;
    }
  }

  return Standard_True;
}

//=======================================================================
//function : ListOfRays
//purpose  : 
//=======================================================================
void BRepClass_FaceExplorer::ListOfRays(const gp_Pnt2d& P,
                                        NCollection_Vector<TopClass_RayInfo>& theList)
{
  const Standard_Real aTolParConf2 = Precision::SquarePConfusion();
  TopExp_Explorer     anExpF(myFace, TopAbs_EDGE);
  Standard_Real       aFPar, aLPar;

  gp_Dir2d aLinDir;

  for (; anExpF.More(); anExpF.Next())
  {
    const TopoDS_Edge &anEdge = TopoDS::Edge(anExpF.Current());
    const TopAbs_Orientation anOrientation = anEdge.Orientation();

    if ((anOrientation != TopAbs_FORWARD) &&
        (anOrientation != TopAbs_REVERSED))
    {
      continue;
    }

    const Handle(Geom2d_Curve) aC2d = BRep_Tool::CurveOnSurface(anEdge,
                                                                myFace,
                                                                aFPar,
                                                                aLPar);

    if (aC2d.IsNull())
    {
      continue;
    }

    // Treatment of infinite cases.
    if (Precision::IsNegativeInfinite(aFPar))
    {
      if (Precision::IsPositiveInfinite(aLPar))
      {
        aFPar = -1.;
        aLPar = 1.;
      }
      else
      {
        aFPar = aLPar - 1.;
      }
    }
    else if (Precision::IsPositiveInfinite(aLPar))
    {
      aLPar = aFPar + 1.;
    }

    for (Standard_Real aCurEdgePar = Probing_Start;
                       (aCurEdgePar < Probing_End);
                        aCurEdgePar += Probing_Step)
    {
      TopClass_RayInfo aRI;
      aLinDir.SetCoord(1.0, 0.0);

      const Standard_Real aParamIn = aCurEdgePar*aFPar + (1. - aCurEdgePar)*aLPar;

      gp_Pnt2d aPOnC;
      gp_Vec2d aTanVec;
      aC2d->D1(aParamIn, aPOnC, aTanVec);

      const Standard_Real aPrmTemp = aPOnC.SquareDistance(P);
      if (aPrmTemp > aTolParConf2)
      {
        aLinDir.SetXY(aPOnC.XY() - P.XY());
        Standard_Real aTanMod = aTanVec.SquareMagnitude();
        if (aTanMod < aTolParConf2)
        {
          aRI.AddPenalty(10);
        }
        else
        {
          const Standard_Real aSinA = Abs(aTanVec.Crossed(aLinDir.XY())) / Sqrt(aTanMod);
          // The line from the input point P to the current point on edge
          // is tangent to the edge curve. This condition is bad for classification.
          // Therefore, we add penalty for this ray.

          if (aSinA < 0.1)
          {
            aRI.AddPenalty(1);
          }
          else if (aSinA < 0.02)
          {
            aRI.AddPenalty(2);
          }
          else if (aSinA < 0.001)
          {
            aRI.AddPenalty(3);
          }
        }
      }
      else
      {
        aRI.AddPenalty(10);
      }

      aRI.SetLin(gp_Lin2d(P, aLinDir));
      aRI.SetParam(Sqrt(aPrmTemp));

      theList.Append(aRI);
    }// for (Standard_Real aCurEdgePar = Probing_Start;...
  }// for (Standard_Integer anEInd = 1;...
}

//=======================================================================
//function : InitWires
//purpose  : 
//=======================================================================

void  BRepClass_FaceExplorer::InitWires()
{
  myWExplorer = NCollection_IndexedDataMap<TopoDS_Wire,
                      NCollection_List<TopClass_GeomEdge>>::Iterator(myMapWE);
}

//=======================================================================
//function : SimpleAdd
//purpose  : 
//=======================================================================
void BRepClass_FaceExplorer::SimpleAdd(const TopoDS_Edge &theEdge)
{
  const TopClass_GeomEdge aGE(theEdge, myFace);

  if (aGE.IsNull())
    return;

  NCollection_List<TopClass_GeomEdge>& anEList = myWExplorer.ChangeValue();
  anEList.Append(aGE);
}

//=======================================================================
//function : AddPseudo
//purpose  : 
//=======================================================================
void BRepClass_FaceExplorer::AddPseudo(const TopoDS_Edge& theE1,
                                       const TopoDS_Edge& theE2,
                                       const TopoDS_Vertex& theV,
                                       const Standard_Real theUPeriod,
                                       const Standard_Real theVPeriod)
{
  if (BRep_Tool::Degenerated(theE1) || BRep_Tool::Degenerated(theE2))
    return;

  gp_Pnt2d aPFirst, aPLast;
  TopAbs_Orientation anOriFirst = theE1.Orientation(), anOriLast = theE2.Orientation();

  if (theE1.IsEqual(theE2))
  {
    // source Wire contains single edge only
    // with single vertex but the edge can be
    // geometrically not-closed (e.g. the part
    // of some circle). 

    // Direction of aC1 does not take into account
    // the orientation of the edge.
    Standard_Real aFirst = 1.0, aLast = -1.0;
    const Handle(Geom2d_Curve) aC1 = BRep_Tool::CurveOnSurface(theE1,
                                                               myFace,
                                                               aFirst,
                                                               aLast);

    // The algorithm below will create a pseudo-edge
    // from aPFirst to aPLast.
    // But in this case it must join end-point of curve
    // with its start-point (namely in this direction).
    aC1->D0(aFirst, aPLast);
    aC1->D0(aLast,  aPFirst);
  }
  else if (theE1.IsSame(theE2))
  {
    // This case is not processed
    return;
  }
  else
  {
    if (!PrepareEdge(theE1, theV, aPFirst, anOriFirst))
    {
      return;
    }

    if ((anOriFirst != TopAbs_FORWARD) && (anOriFirst != TopAbs_REVERSED))
      return;

    if (!PrepareEdge(theE2, theV, aPLast, anOriLast))
      return;
  }

  if ((anOriLast != TopAbs_FORWARD) && (anOriLast != TopAbs_REVERSED))
    return;

  if (SquareDistanceOnPeriodic(aPFirst, aPLast,
                               theUPeriod, theVPeriod) < Precision::SquarePConfusion())
  {
    return;
  }

  const gp_XY aMVec = aPLast.XY() - aPFirst.XY();
  gp_Lin2d aLin(aPFirst, gp_Dir2d(aMVec));
  const Standard_Real aFPar = 0.0;
  const Standard_Real aLPar = aMVec.Modulus();
  const Handle(Geom2d_Curve) aPseudoEdge = new Geom2d_Line(aLin);

  // Null 3D-curve
  Handle(Geom_Curve) aC3d;

  // Pseudo-edge. Therefore, parameter of the vertex is not important value.
  TopClass_GeomVertex aV(theV, aFPar);

  TopClass_GeomEdge aGE(aC3d, aPseudoEdge, BRep_Tool::Surface(myFace),
                                          aFPar, aLPar, anOriLast, aV, aV);

  NCollection_List<TopClass_GeomEdge>& anEList = myWExplorer.ChangeValue();
  anEList.Append(aGE);
}

//=======================================================================
//function : AddEdgeToList
//purpose  : 
//=======================================================================
Standard_Boolean BRepClass_FaceExplorer::PrepareEdge(const TopoDS_Edge& theEdge,
                                                     const TopoDS_Vertex& theV,
                                                     gp_Pnt2d& thePnt,
                                                     TopAbs_Orientation& theOri)
{
  const TopAbs_Orientation anOri = theEdge.Orientation();
  if ((anOri != TopAbs_FORWARD) && (anOri != TopAbs_REVERSED))
    return Standard_False;

  TopoDS_Vertex aVFirst, aVLast;
  TopExp::Vertices(theEdge, aVFirst, aVLast, Standard_True);

  Standard_Real aFirst = 1.0, aLast = -1.0;
  const Handle(Geom2d_Curve) aC1 = BRep_Tool::CurveOnSurface(theEdge,
                                                             myFace,
                                                             aFirst,
                                                             aLast);

  if (!aVFirst.IsNull() && aVFirst.IsSame(theV))
  {
    aC1->D0((anOri == TopAbs_FORWARD) ? aFirst : aLast, thePnt);
    theOri = TopAbs_FORWARD;// aVFirst.Orientation();
  }
  else if (!aVLast.IsNull() && aVLast.IsSame(theV))
  {
    aC1->D0((anOri == TopAbs_FORWARD) ? aLast : aFirst, thePnt);
    theOri = TopAbs_REVERSED;//aVLast.Orientation();
  }
  else
  {
    return Standard_False;
  }

  return Standard_True;
}

//=======================================================================
//function : InitEdges
//purpose  : 
//=======================================================================
void BRepClass_FaceExplorer::InitEdges()
{
  NCollection_List<TopClass_GeomEdge>& anEList = myWExplorer.ChangeValue();
  myEExplorer.Initialize(anEList);
}

//=======================================================================
//function : CurrentEdge
//purpose  : 
//=======================================================================
void BRepClass_FaceExplorer::CurrentEdge(TopClass_GeomEdge& theGE) const
{
  theGE = myEExplorer.Value();
}

//=======================================================================
//function : IsPointInVertex
//purpose  : 
//=======================================================================
Standard_Boolean BRepClass_FaceExplorer::
                    IsPointInEdgeVertex(const gp_Pnt2d &thePnt,
                                        Standard_Real* const theParameter) const
{
  TopLoc_Location aLoc;
  const Handle(Geom_Surface) aSurf = BRep_Tool::Surface(myFace, aLoc);
  const gp_Pnt aPref(aSurf->Value(thePnt.X(), thePnt.Y()).Transformed(aLoc));
  const TopClass_GeomEdge &anEdge = myEExplorer.Value();

  if (anEdge.FirstVertex().IsSame(aPref))
  {
    if (theParameter)
      *theParameter = anEdge.FirstVertex().ParameterOnEdge();

    return Standard_True;
  }

  if (anEdge.LastVertex().IsSame(aPref))
  {
    if (theParameter)
      *theParameter = anEdge.LastVertex().ParameterOnEdge();

    return Standard_True;
  }

  if (theParameter)
    *theParameter = 0.0;

  return Standard_False;
}

//=======================================================================
//function : SquareDistanceOnPeriodic
//purpose  : Computes the distance between two 2D-points in case if they
//            are in different periodic regions or in same region but near
//            to the different period boundary. I.e. (example on 1D-object
//            with period T):
//            1. Square distance between points X and (X+d+k*T)
//                (k is an integer number) is equal to d^2 
//                (instead of (d+k*T)^2).
//            2. Square distance between points X and T-Y 
//                (0 <= X <= T, 0 <= Y <= T) is equal to (X+Y)^2 instead of
//                (T-Y-X)^2.
//=======================================================================
Standard_Real SquareDistanceOnPeriodic(const gp_Pnt2d &theP1,
                                       const gp_Pnt2d &theP2,
                                       const Standard_Real theUPeriod,
                                       const Standard_Real theVPeriod)
{
  const Standard_Real aDeltaXtmp = theUPeriod > 0.0 ?
                                      fmod(Abs(theP1.X() - theP2.X()), theUPeriod) :
                                      Abs(theP1.X() - theP2.X());

  const Standard_Real aDeltaYtmp = theVPeriod > 0.0 ?
                                      fmod(Abs(theP1.Y() - theP2.Y()), theVPeriod) :
                                      Abs(theP1.Y() - theP2.Y());
  const Standard_Real aDeltaX = Min(aDeltaXtmp, Abs(aDeltaXtmp - theUPeriod));
  const Standard_Real aDeltaY = Min(aDeltaYtmp, Abs(aDeltaYtmp - theVPeriod));

  return (aDeltaX*aDeltaX + aDeltaY*aDeltaY);
}