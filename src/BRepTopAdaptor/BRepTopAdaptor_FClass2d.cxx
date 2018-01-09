// Created on: 1995-03-22
// Created by: Laurent BUCHARD
// Copyright (c) 1995-1999 Matra Datavision
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

#include <BRepTopAdaptor_FClass2d.hxx>

#include <BRep_Tool.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepAdaptor_Curve2d.hxx>
#include <BRepAdaptor_HSurface.hxx>
#include <BRepClass_FaceClassifier.hxx>
#include <BRepTools_WireExplorer.hxx>
#include <CSLib_Class2d.hxx>
#include <ElCLib.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2dInt_Geom2dCurveTool.hxx>
#include <GeomInt.hxx>
#include <Precision.hxx>
#include <TColgp_SequenceOfPnt2d.hxx>
#include <TColgp_SequenceOfVec2d.hxx>
#include <TColStd_DataMapOfIntegerInteger.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>

#include <NCollection_UBTree.hxx>
#include <NCollection_UBTreeFiller.hxx>
#include <Bnd_B2d.hxx>

#ifdef DEBUG_PCLASS_POLYGON
#include <DrawTrSurf.hxx>
#include <Geom2d_BSplineCurve.hxx>
#endif

typedef NCollection_UBTree<Standard_Integer, Bnd_B2d> BRepTopAdaptor_FClass2dTree;
typedef NCollection_UBTreeFiller <Standard_Integer, Bnd_B2d> BRepTopAdaptor_FClass2dTreeFiller;

inline Standard_Boolean IsInter1d(Standard_Real theU1, Standard_Real theU2,
                                  Standard_Real theW1, Standard_Real theW2)
{
  if (theU1 > theU2)  std::swap(theU1, theU2);
  if (theW1 > theW2)  std::swap(theW1, theW2);
  return Max(theU1, theW1) - Min(theU2, theW2) < Precision::PConfusion();
}

class BRepTopAdaptor_FClass2dSel : public BRepTopAdaptor_FClass2dTree::Selector
{
public:
  BRepTopAdaptor_FClass2dSel(const TColgp_Array1OfPnt2d& thePnts,
                             const Bnd_B2d& theBox,
                             const Standard_Integer theCurIdx) : 
                             myPnts(thePnts),
                             myBox(theBox),
                             myNbSeg(myPnts.Length() - 1),
                             myCurrentIndex(theCurIdx),
                             myP1(myPnts(myCurrentIndex).XY()),
                             myP2(myPnts(myCurrentIndex + 1).XY()),
                             myVec12(myP2 - myP1)
  {}

  Standard_Boolean Reject(const Bnd_B2d& theBox) const
  {
    return theBox.IsOut(myBox);
  }

  Standard_Boolean Accept(const Standard_Integer& theIndex)
  {
    // reject the same or neighboring index
    if (myCurrentIndex == theIndex)
      return Standard_False;
    if ((myCurrentIndex % myNbSeg) + 1 == theIndex || (theIndex % myNbSeg) + 1 == myCurrentIndex)
      return Standard_False;

    const gp_XY& aP3 = myPnts(theIndex).XY();
    const gp_XY& aP4 = myPnts(theIndex + 1).XY();

    if (IsInter1d(myP1.X(), myP2.X(), aP3.X(), aP4.X()) &&
        IsInter1d(myP1.Y(), myP2.Y(), aP3.Y(), aP4.Y()))
    {
      Standard_Real anAreaEps = Precision::PConfusion() * Precision::PConfusion();
      gp_XY aVec13 = aP3 - myP1;
      gp_XY aVec14 = aP4 - myP1;
      if (myVec12.Crossed(aVec13) * myVec12.Crossed(aVec14) < anAreaEps)
      {
        gp_XY aVec34 = aP4 - aP3;
        gp_XY aVec32 = myP2 - aP3;
        if (aVec13.Crossed(aVec14) * aVec34.Crossed(aVec32) < anAreaEps)
        {
          myStop = Standard_True;
          return Standard_True;
        }
      }
    }
    return Standard_False;
  }

protected:
  BRepTopAdaptor_FClass2dSel(BRepTopAdaptor_FClass2dSel&);
  const BRepTopAdaptor_FClass2dSel& operator=(const BRepTopAdaptor_FClass2dSel&);

private:
  const TColgp_Array1OfPnt2d& myPnts;
  Bnd_B2d myBox;
  Standard_Integer myNbSeg, myCurrentIndex;
  gp_XY myP1, myP2, myVec12;
};


//=======================================================================
//function : Default constructor
//purpose  : 
//=======================================================================
BRepTopAdaptor_FClass2d::BRepTopAdaptor_FClass2d() : myTol3D(Precision::Confusion()),
                                                     myUmin(RealLast()),
                                                     myVmin(RealLast()),
                                                     myUmax(-myUmin),
                                                     myVmax(-myVmin)
{
}

//=======================================================================
//function : Constructor
//purpose  : 
//=======================================================================
BRepTopAdaptor_FClass2d::BRepTopAdaptor_FClass2d(const TopoDS_Face& theFace,
                                                 const Standard_Real theTol3D):
                                                 myFace(theFace),
                                                 myTol3D(theTol3D),
                                                 myUmin(RealLast()), myVmin(RealLast()),
                                                 myUmax(-myUmin), myVmax(-myVmin)
{
  Init(myFace, theTol3D);
}

//=======================================================================
//function : ComputeDeviation
//purpose  : 
//=======================================================================
static Standard_Boolean ComputeDeviation(const gp_Pnt2d& thePf,
                                         const gp_Pnt2d& thePl,
                                         const gp_Pnt2d& thePm,
                                         Standard_Real& theDU, Standard_Real& theDV)
{
  const gp_XY aDirVec = thePl.XY() - thePf.XY();

  if (aDirVec.SquareModulus() > 1.0e-16)
  {
    const gp_Lin2d aLin(thePf, gp_Dir2d(aDirVec));
    const Standard_Real aUmid = ElCLib::Parameter(aLin, thePm);
    const gp_Pnt2d aPp = ElCLib::Value(aUmid, aLin);
    theDU = Abs(aPp.X() - thePm.X());
    theDV = Abs(aPp.Y() - thePm.Y());
    return Standard_True;
  }
  return Standard_False;
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================
void BRepTopAdaptor_FClass2d::Init(const TopoDS_Face& theFace,
                                   const Standard_Real theTol3D)
{
  Standard_Boolean WireIsEmpty, Ancienpnt3dinitialise, degenerated;
  Standard_Integer nbpnts, firstpoint, NbEdges;
  Standard_Integer iX, aNbs1, nbs, Avant, BadWire;
  Standard_Real u, du, pfbid, plbid;
  Standard_Real FlecheU, FlecheV;
  Standard_Real uFirst, uLast;
  Standard_Real aPrCf, aPrCf2;
  //
  TopoDS_Edge  edge;
  TopoDS_Vertex Va, Vb;
  TopAbs_Orientation Or;
  BRepTools_WireExplorer aWExp;
  TopExp_Explorer aExpF, aExp;
  Handle(Geom2d_Curve) aC2D;
  gp_Pnt Ancienpnt3d;
  TColgp_SequenceOfPnt2d SeqPnt2d;
  
  BRepAdaptor_Surface anAS(theFace, Standard_False);

  Destroy();

  aPrCf = Precision::Confusion();
  aPrCf2 = aPrCf*aPrCf;
  //
  myFace = theFace;
  myFace.Orientation(TopAbs_FORWARD);
  myTol3D = theTol3D;
  const Standard_Real aTolU = anAS.UResolution(theTol3D),
                      aTolV = anAS.VResolution(theTol3D);
  //const Standard_Real aMaxGapU = Max(aTolU, Precision::PConfusion());
  //const Standard_Real aMaxGapV = Max(aTolV, Precision::PConfusion());
  //
  myUmin = myVmin = RealLast();
  myUmax = myVmax = -myUmin;
  BadWire = 0;

  //if face has several wires and one of them is bad,
  //it is necessary to process all of them for correct
  //calculation of Umin, Umax, Vmin, Vmax - ifv, 23.08.06 
  //
  aExpF.Init(myFace, TopAbs_WIRE);
  for (; aExpF.More(); aExpF.Next())
  {
    const TopoDS_Wire& aW = *((TopoDS_Wire*)&aExpF.Current());
    //
    nbpnts = 0;
    firstpoint = 1;
    FlecheU = aTolU;
    FlecheV = aTolV;
    WireIsEmpty = Standard_True;
    Ancienpnt3dinitialise = Standard_False;
    Ancienpnt3d.SetCoord(0., 0., 0.);
    //
    SeqPnt2d.Clear();
    //
    // NbEdges
    NbEdges = 0;
    aExp.Init(aW, TopAbs_EDGE);
    for (; aExp.More(); aExp.Next())
    {
      const TopoDS_Edge& anE = TopoDS::Edge(aExp.Current());
      const Standard_Real aTol = BRep_Tool::Tolerance(anE);
      FlecheU = Max(FlecheU, anAS.UResolution(aTol));
      FlecheV = Max(FlecheV, anAS.VResolution(aTol));
      NbEdges++;
    }

    // First and last points of the previous edges
    // in order to detect if wire is close in 2d-space
    //gp_Pnt2d aPrevPoint, aFirstEPoint;
    //Standard_Boolean isFirst = Standard_True;

    //
    aWExp.Init(aW, myFace);
    for (; aWExp.More(); aWExp.Next()) {
      NbEdges--;
      edge = aWExp.Current();
      Or = edge.Orientation();
      if (!(Or == TopAbs_FORWARD || Or == TopAbs_REVERSED)) {
        continue;
      }
      //
      aC2D = BRep_Tool::CurveOnSurface(edge, myFace, pfbid, plbid);
      if (aC2D.IsNull()) {
        return;
      }
      //
      BRepAdaptor_Curve2d C(edge, myFace);

      //if (isFirst)
      //{
      //  C.D0(C.FirstParameter(), aFirstEPoint);
      //  C.D0(C.LastParameter(), aPrevPoint);

      //  if (edge.Orientation() == TopAbs_REVERSED)
      //    std::swap(aFirstEPoint, aPrevPoint);
      //}
      //else
      //{
      //  gp_Pnt2d aP1(C.Value((Or == TopAbs_REVERSED) ? C.LastParameter() : C.FirstParameter()));
      //  if (Abs(aP1.X() - aPrevPoint.X()) > aMaxGapU || Abs(aP1.Y() - aPrevPoint.Y()) > aMaxGapV)
      //  {
      //    //Wire is not closed in 2D-space
      //    BadWire = 1;
      //  }

      //  C.D0((Or == TopAbs_REVERSED) ? C.FirstParameter() : C.LastParameter(), aPrevPoint);
      //}

      //isFirst = Standard_False;

      BRepAdaptor_Curve C3d;
      //------------------------------------------
      degenerated = Standard_False;
      if (BRep_Tool::Degenerated(edge) ||
          BRep_Tool::IsClosed(edge, myFace))
      {
        degenerated = Standard_True;
      }
      //
      TopExp::Vertices(edge, Va, Vb);
      //
      if (Va.IsNull() || Vb.IsNull())
      {
        degenerated = Standard_True;
      }
      //
      //-- Verification of cases when forgotten to code degenereted
      if (!degenerated) {
        // check that whole curve is located in vicinity of its middle point
        // (within sphere of Precision::Confusion() diameter)
        C3d.Initialize(edge, myFace);
        gp_Pnt P3da = C3d.Value(0.5 * (pfbid + plbid));
        du = plbid - pfbid;
        const int NBSTEPS = 10;
        Standard_Real aPrec2 = 0.25 * Precision::Confusion() * Precision::Confusion();
        degenerated = Standard_True;
        for (Standard_Integer i = 0; i <= NBSTEPS; i++)
        {
          Standard_Real U = pfbid + i * du / NBSTEPS;
          gp_Pnt P3db = C3d.Value(U);
          Standard_Real aR2 = P3da.SquareDistance(P3db);
          if (aR2 > aPrec2) {
            degenerated = Standard_False;
            break;
          }
        }
      }//if(!degenerated)
      //-- ----------------------------------------
      //
      // NbSamples +> nbs
      nbs = Geom2dInt_Geom2dCurveTool::NbSamples(C);
      if (nbs > 2) {
        nbs *= 4;
      }
      du = (plbid - pfbid) / (Standard_Real)(nbs - 1);
      //
      if (Or == TopAbs_FORWARD) {
        u = pfbid;
        uFirst = pfbid;
        uLast = plbid;
      }
      else {
        u = plbid;
        uFirst = plbid;
        uLast = pfbid;
        du = -du;
      }
      //
      // aPrms
      aNbs1 = (nbs == 2 ? nbs + 1 : nbs);
      TColStd_Array1OfReal aPrms(1, aNbs1);
      //
      if (nbs == 2) {
        Standard_Real aCoef = 0.0025;
        aPrms(1) = uFirst;
        aPrms(2) = uFirst + aCoef*(uLast - uFirst);
        aPrms(3) = uLast;
      }
      else if (nbs>2) {
        aNbs1 = nbs;
        aPrms(1) = uFirst;
        for (iX = 2; iX<aNbs1; ++iX) {
          aPrms(iX) = u + (iX - 1)*du;
        }
        aPrms(aNbs1) = uLast;
      }
      //
      //-- ------------------------------------------------------------
      //-- Check distance uv between the start point of the edge
      //-- and the last point saved in SeqPnt2d
      //-- To to set the first point of the current 
      //-- afar from the last saved point
      Avant = nbpnts;
      for (iX = firstpoint; iX <= aNbs1; iX++) {
        Standard_Boolean IsRealCurve3d;
        Standard_Integer ii;
        Standard_Real aDstX;
        gp_Pnt2d P2d;
        gp_Pnt P3d;
        //
        u = aPrms(iX);
        P2d = C.Value(u);
        if(P2d.X()<myUmin) myUmin = P2d.X();
        if(P2d.X()>myUmax) myUmax = P2d.X();
        if(P2d.Y()<myVmin) myVmin = P2d.Y();
        if(P2d.Y()>myVmax) myVmax = P2d.Y();
        //
        aDstX = RealLast();
        if (degenerated == Standard_False) {
          P3d = C3d.Value(u);
          if (nbpnts>1) {
            if (Ancienpnt3dinitialise) {
              aDstX = P3d.SquareDistance(Ancienpnt3d);
            }
          }
        }
        //
        IsRealCurve3d = Standard_True;
        if (aDstX < aPrCf2)  {
          if (iX>1) {
            Standard_Real aDstX1;
            gp_Pnt MidP3d;
            //
            MidP3d = C3d.Value(0.5*(u + aPrms(iX - 1)));
            aDstX1 = P3d.SquareDistance(MidP3d);
            if (aDstX1 < aPrCf2){
              IsRealCurve3d = Standard_False;
            }
          }
        }
        //
        if (IsRealCurve3d) {
          if (degenerated == Standard_False) {
            Ancienpnt3d = P3d;
            Ancienpnt3dinitialise = Standard_True;
          }
          nbpnts++;
          SeqPnt2d.Append(P2d);
        }
        //
        ii = nbpnts;
        if (ii>(Avant + 4)) {
          const gp_Pnt2d &aPpf = SeqPnt2d(ii - 2),
                         &aPpl = SeqPnt2d(ii),
                         &aPpm = SeqPnt2d(ii - 1);
          const gp_XY aDirVec = aPpl.XY() - aPpf.XY();
          Standard_Real aDU, aDV;
          if (ComputeDeviation(aPpf, aPpl, aPpm, aDU, aDV))
          {
            if (aDU > FlecheU)
              FlecheU = aDU;
            if (aDV > FlecheV)
              FlecheV = aDV;
          }
        }
        else if (firstpoint == 2 && iX == 2)
        {
          // compute deflection of skipped beginning of the curve to the created polygon
          const gp_Pnt2d &aPpf = SeqPnt2d(ii - 1),
                         &aPpl = SeqPnt2d(ii);
          const gp_Pnt2d aPpm = C.Value(aPrms(1));
          Standard_Real aDU, aDV;
          if (ComputeDeviation(aPpf, aPpl, aPpm, aDU, aDV))
          {
            if (aDU > FlecheU)
              FlecheU = aDU;
            if (aDV > FlecheV)
              FlecheV = aDV;
          }
        }
      }// for(iX=firstpoint; iX<=aNbs1; iX++) {
      //
      if (BadWire) {
        continue; //if face has several wires and one of them is bad,
        //it is necessary to process all of them for correct
        //calculation of Umin, Umax, Vmin, Vmax - ifv, 23.08.06 
      }
      //
      if (firstpoint == 1)
        firstpoint = 2;

      WireIsEmpty = Standard_False;
    } //for(;aWExp.More(); aWExp.Next()) {
    // ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
    if (NbEdges/* || Abs(aFirstEPoint.X() - aPrevPoint.X()) > aMaxGapU ||
                   Abs(aFirstEPoint.Y() - aPrevPoint.Y()) > aMaxGapV*/)
    {
      // 1. TopExp_Explorer and BRepTools_WireExplorer returns differ number of edges
      // 2. Wire is not closed in 2D-space
      TColgp_Array1OfPnt2d PClass(1, 2);
      gp_Pnt2d anInitPnt(0., 0.);
      //
      PClass.Init(anInitPnt);
      myTabClass.Append(new CSLib_Class2d(PClass,
                                          FlecheU,
                                          FlecheV,
                                          myUmin, myVmin, myUmax, myVmax));
      BadWire = 1;
      myTabOrien.Append(-1);
    }
    //
    else if (!WireIsEmpty)
    {
      Standard_Real anArea = 0.0;
      TColgp_Array1OfPnt2d PClass(1, nbpnts);
      gp_Pnt2d anInitPnt(0., 0.);
      //
      PClass.Init(anInitPnt);
      if (nbpnts <= 3)
      {
        BadWire = 1;
      }
      
      if (!BadWire)
      {
        // Point to area computation
        const Standard_Integer im2 = nbpnts - 2;
        Standard_Integer im1 = nbpnts - 1;
        Standard_Integer im0 = 1;
        Standard_Integer ii;
        //
        PClass(im2) = SeqPnt2d.Value(im2);
        PClass(im1) = SeqPnt2d.Value(im1);
        PClass(nbpnts) = SeqPnt2d.Value(nbpnts);

        for (ii = 1; ii < nbpnts; ii++, im0++, im1++)
        {
          if (im1 >= nbpnts) im1 = 1;
          PClass(ii) = SeqPnt2d.Value(ii);

          gp_Vec2d A(PClass(1/*im2*/), PClass(im1));
          gp_Vec2d B(PClass(im1), PClass(im0));
          anArea += A.Crossed(B);
        }//for(ii=1; ii<nbpnts; ii++,im0++,im1++,im2++) { 

#ifdef DEBUG_PCLASS_POLYGON
        TColStd_Array1OfReal aKnots(1, nbpnts);
        TColStd_Array1OfInteger aMults(1, nbpnts);
        for (int i = 1; i <= nbpnts; i++)
        {
          aKnots(i) = i;
          aMults(i) = 1;
        }
        aMults(1) = aMults(nbpnts) = 2;
        Handle(Geom2d_BSplineCurve) aPol = new Geom2d_BSplineCurve(PClass, aKnots, aMults, 1);
        DrawTrSurf::Set("pol", aPol);
#endif

        {
          const Standard_Real aToler = Max(myTol3D, Precision::Confusion());
          if (Abs(anArea) < aToler*aToler)
          {
            // anArea = |A|*|B|*sin(Angle) < Tol*Tol
            anArea = 0.;
            BadWire = 1;
          }
        }

        if (!BadWire)
        {
          NCollection_Array1<Bnd_B2d> anArrBoxes(1, nbpnts - 1);
          anArrBoxes(im2).Add(PClass(im2));
          anArrBoxes(im2).Add(PClass(im1));
          anArrBoxes(im1).Add(PClass(im1));
          anArrBoxes(im1).Add(PClass(nbpnts));
          for (ii = 1; ii < nbpnts; ii++)
          {
            anArrBoxes(ii).Add(PClass(ii));
            anArrBoxes(ii).Add(PClass(ii + 1));
          }

          if (nbpnts > 100)
          {
            BRepTopAdaptor_FClass2dTree aBBTree;
            {
              BRepTopAdaptor_FClass2dTreeFiller aTreeFiller(aBBTree);
              for (ii = 1; ii < nbpnts; ii++)
                aTreeFiller.Add(ii, anArrBoxes(ii));
            }
            for (ii = 1; ii < nbpnts; ii++)
            {
              const Bnd_B2d& aBox = anArrBoxes(ii);
              BRepTopAdaptor_FClass2dSel aSelector(PClass, aBox, ii);
              if (aBBTree.Select(aSelector))
              {
                //The polygon is self-intersected
                BadWire = Standard_True;
                break;
              }
            }
          }
          else
          {
            for (ii = 1; ii < nbpnts; ii++)
            {
              const Bnd_B2d& aBox = anArrBoxes(ii);
              BRepTopAdaptor_FClass2dSel aSelector(PClass, aBox, ii);
              for (Standard_Integer j = ii + 1; j < nbpnts; j++)
              {
                const Bnd_B2d& aBoxj = anArrBoxes(j);
                if (!aSelector.Reject(aBoxj) && aSelector.Accept(j))
                {
                  //The polygon is self-intersected
                  BadWire = Standard_True;
                  break;
                }
              }
            }
          }
        } // if (!BadWire) cond.
      } // if (!BadWire) cond.

      if (!BadWire)
      {
        myTabClass.Append(new CSLib_Class2d(PClass,
                                            FlecheU, FlecheV,
                                            myUmin, myVmin, myUmax, myVmax));

        myTabOrien.Append((anArea > 0.0) ? 1 : 0);
      }
      else // if (BadWire)
      {
        myTabOrien.Append(-1);
        TColgp_Array1OfPnt2d PPClass(1, 2);
        myTabClass.Append(new CSLib_Class2d(PPClass,
                                            FlecheU,
                                            FlecheV,
                                            myUmin, myVmin, myUmax, myVmax));
      }
    }// else if(!WireIsEmpty) cond.
  } // for(; aExpF.More();  aExpF.Next()) {

  if((myTabClass.Length() > 0) && (BadWire))
  {
    //-- if an error on a wire was detected : all TabOrien set to -1
    myTabOrien(1) = -1;
  }
}

//=======================================================================
//function : PerformInfinitePoint
//purpose  : 
//=======================================================================
TopAbs_State BRepTopAdaptor_FClass2d::PerformInfinitePoint() const
{
  if(myUmax == -RealLast() || myVmax == -RealLast() ||
     myUmin == RealLast() || myVmin == RealLast())
  {
    return TopAbs_IN;
  }

  const Standard_Real aMaxTol = Min(10.0, BRep_Tool::MaxTolerance(myFace,
                                                                  TopAbs_VERTEX));
  const BRepAdaptor_Surface aS(myFace, Standard_False);
  gp_Pnt2d aP;

  // For periodic surfaces, Umax - Umin (or Vmax - Vmin)
  // can be equal to period. In this case, shift on 
  // k*(Umax - Umin), where k is an integer number,
  // will not take any effect in 3D-space. Therefore, we apply
  // that k is not integer.
  const Standard_Real aDeltaU = 1.5*(myUmax - myUmin),
                      aDeltaV = 1.5*(myVmax - myVmin);

  if (aDeltaU < aDeltaV)
  {
    aP.SetCoord((myUmin - Max(aDeltaU + 2.0*aS.UResolution(aMaxTol), 1.0)),
                                                        0.5*(myVmax + myVmin));

    //In order to avoid work with infinite-located curves (e.g. in
    //half-spaces)
    if(Precision::IsInfinite(myVmax) && Precision::IsInfinite(myVmin))
    {
      aP.SetY(0.0);
    }
    else if(Precision::IsInfinite(myVmax))
    {
      aP.SetY(myVmin - 100.0*aS.VResolution(aMaxTol));
    }
    else if(Precision::IsInfinite(myVmin))
    {
      aP.SetY(myVmax + 100.0*aS.VResolution(aMaxTol));
    }
  }
  else
  {
    aP.SetCoord(0.5*(myUmax + myUmin),
                    (myVmin - Max(aDeltaV + 2.0*aS.VResolution(aMaxTol), 1.0)));

    //In order to avoid work with infinite-located curves (e.g. in
    //half-spaces)
    if(Precision::IsInfinite(myUmax) && Precision::IsInfinite(myUmin))
    {
      aP.SetX(0.0);
    }
    else if(Precision::IsInfinite(myUmax))
    {
      aP.SetX(myUmin - 100.0*aS.UResolution(aMaxTol));
    }
    else if(Precision::IsInfinite(myUmin))
    {
      aP.SetX(myUmax + 100.0*aS.UResolution(aMaxTol));
    }
  }

#ifdef BREPTOPADAPTOR_FCLASS2D_DEBUG
  std::cout << "BRepTopAdaptor_FClass2d::PerformInfinitePoint: point " << 
                                            aP.X() << " " << aP.Y() << std::endl;
#endif

  return (Perform(aP, Standard_False));
}

//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================
TopAbs_State 
        BRepTopAdaptor_FClass2d::Perform(const gp_Pnt2d& theP2D,
                                         const Standard_Boolean theIsReqToAdjust) const
{ 
  const Standard_Integer aNbTabClass = myTabClass.Length();
  if(aNbTabClass == 0)
  {
    return TopAbs_IN;
  }

  Standard_Real aU = theP2D.X();
  Standard_Real aV = theP2D.Y();
  Standard_Real aUprev = aU;
  Standard_Real aVprev = aV;
  TopAbs_State aStatus = TopAbs_UNKNOWN;

  const BRepAdaptor_Surface anAS(myFace, Standard_False);

  const Standard_Boolean IsUPer = anAS.IsUPeriodic();
  const Standard_Boolean IsVPer = anAS.IsVPeriodic();
  const Standard_Real    aUperiod = IsUPer ? anAS.UPeriod() : 0.0;
  const Standard_Real    aVperiod = IsVPer ? anAS.VPeriod() : 0.0;

  Standard_Boolean isUAdjusted = Standard_False, 
                   isVAdjusted = Standard_False;
  Standard_Integer aTmpStatus = 1;
  //
  if (theIsReqToAdjust) {
    Standard_Real aDU, aDV;
    if (IsUPer) {
      GeomInt::AdjustPeriodic(aUprev, myUmin, myUmax, aUperiod, aUprev, aDU);
    }// if (IsUPer) {
    //
    if (IsVPer) {
      GeomInt::AdjustPeriodic(aVprev, myVmin, myVmax, aVperiod, aVprev, aDV);
    }//if (IsVPer) {
  }
  //
  for (;;) {
    aTmpStatus = 1;
    gp_Pnt2d aPuv(aU, aV);

#ifdef BREPTOPADAPTOR_FCLASS2D_CheckPolygon
    const Standard_Boolean hasToUseClassifier = Standard_False;
#else
    Standard_Boolean hasToUseClassifier = (myTabOrien(1) == -1);
#endif

    if(!hasToUseClassifier)
    {
      for(Standard_Integer aN = 1; aN <= aNbTabClass; aN++)
      {
        const Standard_Integer aPolyStatus = myTabClass(aN)->Classify(aPuv);
        const Standard_Integer aTabOri = myTabOrien(aN);
        if(aPolyStatus == 1)
        {
          if(aTabOri == 0)
          {
            aTmpStatus = -1;
            break;
          }
        }
        else if(aPolyStatus == -1)
        {
          if(aTabOri == 1)
          {
            aTmpStatus = -1;
            break;
          }
        }
        else {
          aTmpStatus = 0;
          break;
        }
      } // for(n=1; n<=nbtabclass; n++)

      if(aTmpStatus == 0)
      {
#if BREPTOPADAPTOR_FCLASS2D_CheckPolygon
        aStatus = TopAbs_ON;
#else
        hasToUseClassifier = Standard_True;
#endif
      }
      else
      {
        aStatus = (aTmpStatus == 1) ? TopAbs_IN : TopAbs_OUT;
      }
    } // if(TabOrien(1)!=-1) {
    //compute state of the point using face classifier
    if(hasToUseClassifier)
    {
      myClassifier.Perform(myFace, aPuv, myTol3D);
      aStatus = myClassifier.State();
    }

    if (!theIsReqToAdjust || (!IsUPer && !IsVPer))
      return aStatus;
    
    if (aStatus == TopAbs_IN || aStatus == TopAbs_ON)
      return aStatus;
    
    if(!isUAdjusted)
    {
      aU = aUprev;
      isUAdjusted = Standard_True;
    }
    else {
      if (IsUPer){
        aU += aUperiod;
      }
    }

    if(aU > myUmax || !IsUPer)
    {
      if(!isVAdjusted)
      {
        aV = aVprev;
        isVAdjusted = Standard_True;
      }
      else {
        if (IsVPer){
          aV += aVperiod;
        }
      }

      aU = aUprev;
      
      if(aV > myVmax || !IsVPer)
      {
        return aStatus;
      }
    }
  } //while (1)
}

//=======================================================================
//function : TestOnRestriction
//purpose  : 
//=======================================================================
TopAbs_State 
BRepTopAdaptor_FClass2d::TestOnRestriction(const gp_Pnt2d& theP2D,
                                           const Standard_Real theTol3D,
                                           const Standard_Boolean theIsReqToAdjust) const
{ 
  const Standard_Integer aNbTabClass = myTabClass.Length();
  if(aNbTabClass == 0)
  {
    return TopAbs_IN;
  }

  Standard_Real aU = theP2D.X();
  Standard_Real aV = theP2D.Y();
  Standard_Real aUprev = aU;
  Standard_Real aVprev = aV;
  TopAbs_State aStatus = TopAbs_UNKNOWN;

  const BRepAdaptor_Surface anAS(myFace, Standard_False);
  const Standard_Real aURes = anAS.UResolution(theTol3D),
                      aVRes = anAS.VResolution(theTol3D);

  const Standard_Boolean IsUPer = anAS.IsUPeriodic();
  const Standard_Boolean IsVPer = anAS.IsVPeriodic();
  const Standard_Real    aUperiod = IsUPer ? anAS.UPeriod() : 0.0;
  const Standard_Real    aVperiod = IsVPer ? anAS.VPeriod() : 0.0;

  Standard_Boolean isUAdjusted = Standard_False,
    isVAdjusted = Standard_False;
  Standard_Integer aTmpStatus = 1;
  //
  if(theIsReqToAdjust)
  {
    Standard_Real aDU, aDV;
    if(IsUPer)
    {
      GeomInt::AdjustPeriodic(aUprev, myUmin, myUmax, aUperiod, aUprev, aDU);
    }// if (IsUPer) {
    //
    if(IsVPer)
    {
      GeomInt::AdjustPeriodic(aVprev, myVmin, myVmax, aVperiod, aVprev, aDV);
    }//if (IsVPer) {
  }
  //
  for(;;)
  {
    aTmpStatus = 1;
    gp_Pnt2d aPuv(aU, aV);

#ifdef BREPTOPADAPTOR_FCLASS2D_CheckPolygon
    const Standard_Boolean bUseClassifier = Standard_False;
#else
    Standard_Boolean hasToUseClassifier = (myTabOrien(1) == -1);
#endif

    if(!hasToUseClassifier)
    {
      for(Standard_Integer aN = 1; aN <= aNbTabClass; aN++)
      {
        const Standard_Integer aPolyStatus = myTabClass(aN)->Classify(aPuv, aURes, aVRes);
        const Standard_Integer aTabOri = myTabOrien(aN);
        if(aPolyStatus == 1)
        {
          if(aTabOri == 0)
          {
            aTmpStatus = -1;
            break;
          }
        }
        else if(aPolyStatus == -1)
        {
          if(aTabOri == 1)
          {
            aTmpStatus = -1;
            break;
          }
        }
        else
        {
          aTmpStatus = 0;
          break;
        }
      } // for(n=1; n<=nbtabclass; n++)

      if(aTmpStatus == 0)
      {
#if BREPTOPADAPTOR_FCLASS2D_CheckPolygon
        aStatus = TopAbs_ON;
#else
        hasToUseClassifier = Standard_True;
#endif
      }
      else
      {
        aStatus = (aTmpStatus == 1) ? TopAbs_IN : TopAbs_OUT;
      }
    } // if(TabOrien(1)!=-1) {
    //compute state of the point using face classifier
    if(hasToUseClassifier)
    {
      myClassifier.Perform(myFace, aPuv, theTol3D);
      aStatus = myClassifier.State();
    }

    if(!theIsReqToAdjust || (!IsUPer && !IsVPer))
      return aStatus;

    if(aStatus == TopAbs_IN || aStatus == TopAbs_ON)
      return aStatus;

    if(!isUAdjusted)
    {
      aU = aUprev;
      isUAdjusted = Standard_True;
    }
    else
    {
      if(IsUPer)
      {
        aU += aUperiod;
      }
    }

    if(aU > myUmax || !IsUPer)
    {
      if(!isVAdjusted)
      {
        aV = aVprev;
        isVAdjusted = Standard_True;
      }
      else
      {
        if(IsVPer)
        {
          aV += aVperiod;
        }
      }

      aU = aUprev;

      if(aV > myVmax || !IsVPer)
      {
        return aStatus;
      }
    }
  } //while (1)
}

//=======================================================================
//function : Destroy
//purpose  : 
//=======================================================================
void BRepTopAdaptor_FClass2d::Destroy()
{
  Standard_Integer nbtabclass = myTabClass.Length();
  for (Standard_Integer d = 1; d <= nbtabclass; d++)
  {
    if(myTabClass(d))
    {
      delete myTabClass(d);
    }
  }

  myTabClass.Clear();
  myTabOrien.Clear();
}