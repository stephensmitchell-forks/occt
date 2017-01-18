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

static void LocalMapShapesAndAncestors(const TopoDS_Shape& S,
                                       const TopAbs_ShapeEnum TS,
                                       const TopAbs_ShapeEnum TA,
                                       const Standard_Boolean /*CheckOrientation*/,
                                       TopTools_IndexedDataMapOfShapeListOfShape& M);

static Standard_Real SquareDistanceOnPeriodic(const gp_Pnt2d &theP1,
  const gp_Pnt2d &theP2,
  const Standard_Real theUPeriod,
  const Standard_Real theVPeriod);

static const Standard_Real Probing_Start = 0.123;
static const Standard_Real Probing_End = 0.7;
static const Standard_Real Probing_Step = 0.2111;

//=======================================================================
//function : BRepClass_FaceExplorer
//purpose  : 
//=======================================================================
BRepClass_FaceExplorer::BRepClass_FaceExplorer(const TopoDS_Face& F) :
       myFace(F)
{
  myFace.Orientation(TopAbs_FORWARD);
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

    TopTools_IndexedDataMapOfShapeListOfShape aMapVE;
    TopExp_Explorer anExp(aWir, TopAbs_EDGE);
    TopTools_MapOfShape aMapS;
    gp_Pnt2d aPFirst, aPLast;
    TopAbs_Orientation anOriFirst = TopAbs_EXTERNAL, anOriLast = TopAbs_EXTERNAL;
    LocalMapShapesAndAncestors(aWir, TopAbs_VERTEX, TopAbs_EDGE,
      Standard_True, aMapVE);

    for (Standard_Integer i = 1; i <= aMapVE.Extent(); i++)
    {
      const TopTools_ListOfShape &aLE = aMapVE.FindFromIndex(i);
      if (aLE.Extent() != 2)
      {
        // simply add edges to the list.

        TopTools_ListIteratorOfListOfShape anItr1(aLE);
        for (; anItr1.More(); anItr1.Next())
        {
          const TopoDS_Edge &anE = TopoDS::Edge(anItr1.Value());

          if (!aMapS.Add(anE))
            continue;

          SimpleAdd(anE);
        }

        continue;
      }

      // Here, aLE.Extent() == 2.

      const TopoDS_Vertex &aVertCurr = TopoDS::Vertex(aMapVE.FindKey(i));

      TopTools_ListIteratorOfListOfShape anItr1(aLE);
      TopTools_ListIteratorOfListOfShape anItr2 = anItr1;

      Standard_Boolean isClosed = Standard_False;
      for (; anItr1.More(); anItr1.Next())
      {
        const TopoDS_Edge &anE1 = TopoDS::Edge(anItr1.Value());

        if (isClosed)
        {
          if (aMapS.Add(anE1))
            SimpleAdd(anE1);

          continue;
        }

        if (BRep_Tool::IsClosed(anE1, myFace) || BRep_Tool::IsClosed(anE1))
        {
          // Algorithm of pseudo-edges creation is quite difficult
          // for these cases. Therefore, we are ignoring them at present.
          // Maybe it will be implemented in the future (in case of necessity).

          if (aMapS.Add(anE1))
            SimpleAdd(anE1);

          isClosed = Standard_True;
          continue;
        }

        if (!AddEdgeToList(anE1, aVertCurr, aMapS.Add(anE1), aPFirst, anOriFirst))
          continue;

        if ((anOriFirst != TopAbs_FORWARD) && (anOriFirst != TopAbs_REVERSED))
          continue;

        anItr2 = anItr1;
        for (anItr2.Next(); anItr2.More(); anItr2.Next())
        {
          const TopoDS_Edge &anE2 = TopoDS::Edge(anItr2.Value());

          if (BRep_Tool::IsClosed(anE2, myFace) || BRep_Tool::IsClosed(anE2))
          {
            // Algorithm of pseudo-edges creation is quite difficult
            // for these cases. Therefore, we are ignoring them at present.
            // Maybe they will be implemented in the future (in case of necessity).

            isClosed = Standard_True;
            break;
          }

          if (!AddEdgeToList(anE2, aVertCurr, aMapS.Add(anE2), aPLast, anOriLast))
            continue;

          if ((anOriLast != TopAbs_FORWARD) && (anOriLast != TopAbs_REVERSED))
            continue;

          if (SquareDistanceOnPeriodic(aPFirst, aPLast, aUPeriod, aVPeriod) <
            Precision::SquarePConfusion())
          {
            continue;
          }

          AddPseudo(aPFirst.XY(), aPLast.XY(), aVertCurr, anOriLast);
        }
      }
    }
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

  // If it is seam
  if (!BRep_Tool::IsClosed(theEdge, myFace))
    return;

  const TopClass_GeomEdge aGE1(TopoDS::Edge(theEdge.Reversed()), myFace);
  anEList.Append(aGE1);
}

//=======================================================================
//function : AddPseudo
//purpose  : 
//=======================================================================
void BRepClass_FaceExplorer::AddPseudo(const gp_XY& thePFirst,
                                       const gp_XY& thePLast,
                                       const TopoDS_Vertex &theV,
                                       const TopAbs_Orientation theOri)
{
  const gp_XY aMVec = thePLast - thePFirst;
  gp_Lin2d aLin(thePFirst, gp_Dir2d(aMVec));
  const Standard_Real aFPar = 0.0;
  const Standard_Real aLPar = aMVec.Modulus();
  const Handle(Geom2d_Curve) aPseudoEdge = new Geom2d_Line(aLin);

  // Null 3D-curve
  Handle(Geom_Curve) aC3d;

  // Pseudo-edge. Therefore, parameter of the vertex is not important value.
  TopClass_GeomVertex aV(theV, aFPar);

  TopClass_GeomEdge aGE(aC3d, aPseudoEdge, BRep_Tool::Surface(myFace),
                                              aFPar, aLPar, theOri, aV, aV);

  NCollection_List<TopClass_GeomEdge>& anEList = myWExplorer.ChangeValue();
  anEList.Append(aGE);
}

//=======================================================================
//function : AddEdgeToList
//purpose  : 
//=======================================================================
Standard_Boolean
        BRepClass_FaceExplorer::AddEdgeToList(const TopoDS_Edge &theEdge,
                                              const TopoDS_Vertex &theCurrVertex,
                                              const Standard_Boolean theHasToBeAdded,
                                              gp_Pnt2d &thePnt,
                                              TopAbs_Orientation &theOri)
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

  if (!aVFirst.IsNull() && aVFirst.IsSame(theCurrVertex))
  {
    aC1->D0((anOri == TopAbs_FORWARD) ? aFirst : aLast, thePnt);
    theOri = TopAbs_FORWARD;// aVFirst.Orientation();
  }
  else if (!aVLast.IsNull() && aVLast.IsSame(theCurrVertex))
  {
    aC1->D0((anOri == TopAbs_FORWARD) ? aLast : aFirst, thePnt);
    theOri = TopAbs_REVERSED;//aVLast.Orientation();
  }
  else
  {
    return Standard_False;
  }

  if (!theHasToBeAdded)
  {
    return Standard_True;
  }

  SimpleAdd(theEdge);

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
//function : LocalMapShapesAndAncestors
//purpose  : This function is taken from the fix #26682. It should be deleted
//            after its integration.
//           CheckOrientation - if it is TRUE then INTERNAL/EXTERNAL elements
//            will be ignored.
//=======================================================================
void LocalMapShapesAndAncestors(const TopoDS_Shape& S,
                                const TopAbs_ShapeEnum TS,
                                const TopAbs_ShapeEnum TA,
                                const Standard_Boolean /*CheckOrientation*/,
                                TopTools_IndexedDataMapOfShapeListOfShape& theM)
{
  TopTools_ListOfShape empty;

  // visit ancestors
  TopExp_Explorer exa(S, TA);
  for (; exa.More(); exa.Next())
  {
    // visit shapes
    const TopoDS_Shape& anc = exa.Current();

    if ((anc.Orientation() != TopAbs_FORWARD) &&
        (anc.Orientation() != TopAbs_REVERSED))
      continue;

    TopExp_Explorer exs(anc, TS);
    for (; exs.More(); exs.Next())
    {
      const TopoDS_Shape &aS1 = exs.Current();

      if ((aS1.Orientation() != TopAbs_FORWARD) &&
          (aS1.Orientation() != TopAbs_REVERSED))
        continue;

      Standard_Integer index = theM.FindIndex(aS1);
      if (index == 0)
      {
        index = theM.Add(aS1, empty);
      }
      TopTools_ListOfShape& aList = theM(index);
      // check if anc already exists in a list
      TopTools_ListIteratorOfListOfShape it(aList);
      for (; it.More(); it.Next())
        if (anc.IsSame(it.Value()))
          break;
      if (!it.More())
        aList.Append(anc);
    }
  }

  // visit shapes not under ancestors
  TopExp_Explorer ex(S, TS, TA);
  for (; ex.More(); ex.Next())
  {
    const TopoDS_Shape &aS1 = ex.Current();
    if ((aS1.Orientation() != TopAbs_FORWARD) &&
        (aS1.Orientation() != TopAbs_REVERSED))
      continue;

    Standard_Integer index = theM.FindIndex(aS1);
    if (index == 0)
      theM.Add(aS1, empty);
  }
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