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
#include <Bnd_Box2d.hxx>

typedef NCollection_UBTree<Standard_Integer, Bnd_Box2d> BRepTopAdaptor_FClass2dTree;
typedef NCollection_UBTreeFiller <Standard_Integer, Bnd_Box2d> BRepTopAdaptor_FClass2dTreeFiller;

class BRepTopAdaptor_FClass2dSel : public NCollection_UBTree <Standard_Integer, Bnd_Box2d>::Selector
{
public:
  BRepTopAdaptor_FClass2dSel(const TColgp_SequenceOfPnt2d& theSeqOfPnts,
                             const Bnd_Box2d& theBox,
                             const Standard_Integer theCurIdx) : 
                             mySeqOfPnts(theSeqOfPnts),
                             myBox(theBox),
                             myCurrentIndex(theCurIdx)
  {}

  Standard_Boolean Reject(const Bnd_Box2d& theBox) const
  {
    return theBox.IsOut(myBox);
  }

#define AcceptSegment {myStop = Standard_True; return Standard_True; }
  Standard_Boolean Accept(const Standard_Integer& theIndex)
  {
    if (myCurrentIndex == theIndex)
      return Standard_False;

    const Standard_Integer aNbPntsM1 = mySeqOfPnts.Length()-1;

    if ((myCurrentIndex == 1) && (theIndex == aNbPntsM1))
      return Standard_False;

    if ((myCurrentIndex == aNbPntsM1) && (theIndex == 1))
      return Standard_False;

    if (Abs(theIndex - myCurrentIndex) == 1)
      return Standard_False;

    const gp_XY &aP1 = mySeqOfPnts(myCurrentIndex).XY();
    const gp_XY &aP2 = mySeqOfPnts(myCurrentIndex + 1).XY();
    const gp_XY &aP3 = mySeqOfPnts(theIndex).XY();
    const gp_XY &aP4 = mySeqOfPnts(theIndex + 1).XY();

    const gp_XY aV12 = aP2 - aP1;
    const gp_XY aV13 = aP3 - aP1;
    const gp_XY aV34 = aP3 - aP4;

    const Standard_Real aDet0 = aV12.Crossed(aV34);
    const Standard_Real aDet1 = aV13.Crossed(aV34);
    const Standard_Real aDet2 = aV12.Crossed(aV13);

    if (Abs(aDet0) < Precision::PConfusion())
    {
      if ((Abs(aDet1) > Precision::PConfusion()) || (Abs(aDet2) > Precision::PConfusion()))
        return Standard_False;

      const Standard_Real aSqMP12 = aV12.SquareModulus();
      const Standard_Real aTP3 = aV13.Dot(aV12);
      if ((0.0 <= aTP3) && (aTP3 <= aSqMP12))
        AcceptSegment;

      const Standard_Real aTP4 = (aP4 - aP1).Dot(aV12);
      if ((0.0 <= aTP4) && (aTP4 <= aSqMP12))
        AcceptSegment;

      const Standard_Real aSqMP34 = aV34.SquareModulus();
      const Standard_Real aTP1 = aV13.Reversed().Dot(aV34);
      if ((0.0 <= aTP1) && (aTP1 <= aSqMP34))
        AcceptSegment;

      const Standard_Real aTP2 = (aP2 - aP3).Dot(aV34);
      if ((0.0 <= aTP2) && (aTP2 <= aSqMP34))
        AcceptSegment;

      return Standard_False;
    }

    const Standard_Real aT1 = aDet1 / aDet0,
                        aT2 = aDet2 / aDet0;

    if ((aT1 < 0.0) || (aT2 < 0.0))
      return Standard_False;

    if ((aT1 > 1.0) || (aT2 > 1.0))
      return Standard_False;

    AcceptSegment;
  }
#undef AcceptSegment

protected:
  BRepTopAdaptor_FClass2dSel(BRepTopAdaptor_FClass2dSel&);
  const BRepTopAdaptor_FClass2dSel& operator=(const BRepTopAdaptor_FClass2dSel&);

private:
  const TColgp_SequenceOfPnt2d& mySeqOfPnts;
  Bnd_Box2d myBox;
  Standard_Integer myCurrentIndex;
};


//=======================================================================
//function : Default constructor
//purpose  : 
//=======================================================================
BRepTopAdaptor_FClass2d::BRepTopAdaptor_FClass2d() : myTolU(Precision::PConfusion()),
                                                     myTolV(Precision::PConfusion()),
                                                     myTol3D(Precision::Confusion()),
                                                     Umin(RealLast()),
                                                     Vmin(RealLast()),
                                                     Umax(-Umin),
                                                     Vmax(-Vmin)
{
}

//=======================================================================
//function : Constructor
//purpose  : 
//=======================================================================
BRepTopAdaptor_FClass2d::BRepTopAdaptor_FClass2d(const TopoDS_Face& aFace,
                                                 const Standard_Real theTol3D):
                                                 Face(aFace),
                                                 myTolU(Precision::PConfusion()),
                                                 myTolV(Precision::PConfusion()),
                                                 myTol3D(theTol3D),
                                                 Umin(RealLast()), Vmin(RealLast()),
                                                 Umax(-Umin), Vmax(-Vmin)
{
  Init(Face, theTol3D);
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================
void BRepTopAdaptor_FClass2d::Init(const TopoDS_Face& aFace,
                                   const Standard_Real theTol3D)
{
  Standard_Boolean WireIsNotEmpty, Ancienpnt3dinitialise, degenerated;
  Standard_Integer nbpnts, firstpoint, NbEdges;
  Standard_Integer iX, aNbs1, nbs, Avant, BadWire;
  Standard_Real u, du, Tole, Tol, pfbid, plbid;
  Standard_Real FlecheU, FlecheV, TolVertex1, TolVertex;
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
  //TColStd_DataMapOfIntegerInteger anIndexMap;
  TColgp_SequenceOfVec2d          aD1Prev;
  TColgp_SequenceOfVec2d          aD1Next;
  
  BRepAdaptor_Surface anAS(aFace, Standard_False);

  Destroy();

  aPrCf = Precision::Confusion();
  aPrCf2 = aPrCf*aPrCf;
  //
  Face = aFace;
  Face.Orientation(TopAbs_FORWARD);
  myTol3D = theTol3D;
  myTolU = anAS.UResolution(theTol3D);
  myTolV = anAS.VResolution(theTol3D);
  //
  Tole = 0.;
  Tol = 0.;
  Umin = Vmin = RealLast();
  Umax = Vmax = -Umin;
  BadWire = 0;

  //if face has several wires and one of them is bad,
  //it is necessary to process all of them for correct
  //calculation of Umin, Umax, Vmin, Vmax - ifv, 23.08.06 
  //
  aExpF.Init(Face, TopAbs_WIRE);
  for (; aExpF.More(); aExpF.Next()) {
    const TopoDS_Wire& aW = *((TopoDS_Wire*)&aExpF.Current());
    //
    nbpnts = 0;
    firstpoint = 1;
    FlecheU = myTolU;
    FlecheV = myTolV;
    TolVertex1 = 0.;
    TolVertex = 0.;
    WireIsNotEmpty = Standard_False;
    Ancienpnt3dinitialise = Standard_False;
    Ancienpnt3d.SetCoord(0., 0., 0.);
    //
    SeqPnt2d.Clear();
    //anIndexMap.Clear();
    aD1Prev.Clear();
    aD1Next.Clear();
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
    gp_Pnt2d aPrevPoint, aFirstEPoint;
    Standard_Boolean isFirst = Standard_True;

    //
    aWExp.Init(aW, Face);
    for (; aWExp.More(); aWExp.Next()) {
      NbEdges--;
      edge = aWExp.Current();
      Or = edge.Orientation();
      if (!(Or == TopAbs_FORWARD || Or == TopAbs_REVERSED)) {
        continue;
      }
      //
      aC2D = BRep_Tool::CurveOnSurface(edge, Face, pfbid, plbid);
      if (aC2D.IsNull()) {
        return;
      }
      //
      BRepAdaptor_Curve2d C(edge, Face);

      if (isFirst)
      {
        C.D0(C.FirstParameter(), aFirstEPoint);
        C.D0(C.LastParameter(), aPrevPoint);

        if (edge.Orientation() == TopAbs_REVERSED)
          std::swap(aFirstEPoint, aPrevPoint);
      }
      else
      {
        gp_Pnt2d aP1(C.Value((Or == TopAbs_REVERSED) ? C.LastParameter() : C.FirstParameter()));
        if (aP1.SquareDistance(aPrevPoint) > Precision::PConfusion())
        {
          //Wire is not closed in 2D-space
          BadWire = 1;
        }

        C.D0((Or == TopAbs_REVERSED) ? C.FirstParameter() : C.LastParameter(), aPrevPoint);
      }

      isFirst = Standard_False;

      BRepAdaptor_Curve C3d;
      //------------------------------------------
      degenerated = Standard_False;
      if (BRep_Tool::Degenerated(edge) ||
        BRep_Tool::IsClosed(edge, Face)) {
        degenerated = Standard_True;
      }
      //
      TopExp::Vertices(edge, Va, Vb);
      //
      TolVertex1 = 0.;
      TolVertex = 0.;
      if (Va.IsNull()) {
        degenerated = Standard_True;
      }
      else {
        TolVertex1 = BRep_Tool::Tolerance(Va);
      }
      if (Vb.IsNull()){
        degenerated = Standard_True;
      }
      else {
        TolVertex = BRep_Tool::Tolerance(Vb);
      }
      // 
      if (TolVertex<TolVertex1) {
        TolVertex = TolVertex1;
      }
      //
      //-- Verification of cases when forgotten to code degenereted
      if (!degenerated) {
        // check that whole curve is located in vicinity of its middle point
        // (within sphere of Precision::Confusion() diameter)
        C3d.Initialize(edge, Face);
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
      Tole = BRep_Tool::Tolerance(edge);
      if (Tole>Tol) {
        Tol = Tole;
      }
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
      aNbs1 = nbs + 1;
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
        if (P2d.X()<Umin) Umin = P2d.X();
        if (P2d.X()>Umax) Umax = P2d.X();
        if (P2d.Y()<Vmin) Vmin = P2d.Y();
        if (P2d.Y()>Vmax) Vmax = P2d.Y();
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

          if(aDirVec.SquareModulus() > 1.0e-16)
          {
            const gp_Lin2d aLin(aPpf, gp_Dir2d(aDirVec));
            const Standard_Real aUmid = ElCLib::Parameter(aLin, aPpm);
            const gp_Pnt2d aPp = ElCLib::Value(aUmid, aLin);
            const Standard_Real aDU = Abs(aPp.X() - aPpm.X());
            const Standard_Real aDV = Abs(aPp.Y() - aPpm.Y());
            if (aDU>FlecheU)
            {
              FlecheU = aDU;
            }

            if (aDV>FlecheV)
            {
              FlecheV = aDV;
            }
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
      if (firstpoint == 1) firstpoint = 2;
      WireIsNotEmpty = Standard_True;
      // Append the derivative of the first parameter.
      Standard_Real aU = aPrms(1);
      gp_Pnt2d      aP;
      gp_Vec2d      aV;

      C.D1(aU, aP, aV);

      if (Or == TopAbs_REVERSED)
        aV.Reverse();

      aD1Next.Append(aV);

      // Append the derivative of the last parameter.
      aU = aPrms(aNbs1);
      C.D1(aU, aP, aV);

      if (Or == TopAbs_REVERSED)
        aV.Reverse();

      if (NbEdges > 0)
        aD1Prev.Append(aV);
      else
        aD1Prev.Prepend(aV);

      // Fill the map anIndexMap.
      //if (Avant > 0)
      //  anIndexMap.Bind(Avant, aD1Next.Length());
      //else
      //  anIndexMap.Bind(1, aD1Next.Length());
    } //for(;aWExp.More(); aWExp.Next()) {
    // ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
    
    BRepTopAdaptor_FClass2dTree aBBTree;
    BRepTopAdaptor_FClass2dTreeFiller aTreeFiller(aBBTree);
    NCollection_Array1<Bnd_Box2d> anArrBoxes(1, nbpnts - 1);
    //
    if (NbEdges || (aPrevPoint.SquareDistance(aFirstEPoint) > Precision::SquarePConfusion()))
    {
      // 1. TopExp_Explorer and BRepTools_WireExplorer returns differ number of edges
      // 2. Wire is not closed in 2D-space
      TColgp_Array1OfPnt2d PClass(1, 2);
      gp_Pnt2d anInitPnt(0., 0.);
      //
      PClass.Init(anInitPnt);
      TabClass.Append(new CSLib_Class2d(PClass,
                                        FlecheU,
                                        FlecheV,
                                        Umin, Vmin, Umax, Vmax));
      BadWire = 1;
      TabOrien.Append(-1);
    }
    //
    else if (WireIsNotEmpty) {
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
        Standard_Integer im2 = nbpnts - 2;
        Standard_Integer im1 = nbpnts - 1;
        Standard_Integer im0 = 1;
        Standard_Integer ii;
        Standard_Real    anArea = 0.0;
        //
        PClass(im2) = SeqPnt2d.Value(im2);
        PClass(im1) = SeqPnt2d.Value(im1);
        PClass(nbpnts) = SeqPnt2d.Value(nbpnts);
        
        anArrBoxes(im2).Add(PClass(im2));
        anArrBoxes(im2).Add(PClass(im1));

        anArrBoxes(im1).Add(PClass(im1));
        anArrBoxes(im1).Add(PClass(nbpnts));

        for (ii = 1; ii<nbpnts; ii++, im0++, im1++, im2++)
        {
          if (im2 >= nbpnts) im2 = 1;
          if (im1 >= nbpnts) im1 = 1;
          PClass(ii) = SeqPnt2d.Value(ii);

          gp_Vec2d A(PClass(1/*im2*/), PClass(im1));
          gp_Vec2d B(PClass(im1), PClass(im0));
          
          anArrBoxes(ii).Add(PClass(ii));
          anArrBoxes(ii).Add(SeqPnt2d.Value(ii + 1));

          aTreeFiller.Add(ii, anArrBoxes(ii));

          // In order to avoid FPE-signal
          // E.g. if aSqMA == aSqMB = 1.0e200.
          //const Standard_Real aSqMA = A.SquareMagnitude(),
          //                    aSqMB = B.SquareMagnitude();
          //if (Precision::IsInfinite(aSqMA) ||
          //    Precision::IsInfinite(aSqMB) ||
          //    (aSqMA*aSqMB  > 1e-32))
          {
            Standard_Real aTmpArea = A.Crossed(B);
            //  
            //if (anIndexMap.IsBound(im1))
            {
              //Standard_Integer  anInd = anIndexMap.Find(im1);
              //const gp_Vec2d   &aVPrev = aD1Prev.Value(anInd);
              //const gp_Vec2d   &aVNext = aD1Next.Value(anInd);

              //if (aVPrev.SquareMagnitude() * aVNext.SquareMagnitude() > 1e-32)
              //{
              //  Standard_Real aDerivAngle, aAbsDA, aProduct, aPA;
              //  //ifv 23.08.06
              //  aPA = Precision::Angular();
              //  aDerivAngle = aVPrev.Angle(aVNext);
              //  aAbsDA = Abs(aDerivAngle);
              //  if (aAbsDA <= aPA)
              //  {
              //    aDerivAngle = 0.;
              //  }
              //  //
              //  aProduct = aDerivAngle * a;
              //  //
              //  if ((M_PI-aAbsDA) <= aPA) {
              //    if (aProduct > 0.) {
              //      aProduct = -aProduct;
              //    }
              //  }
              //  //ifv 23.08.06 : if edges continuity > G1, |aDerivAngle| ~0,
              //  //but can has wrong sign and causes condition aDerivAngle * a < 0.
              //  //that is wrong in such situation
              //  if (iFlag && aProduct < 0.)
              //  {
              //    iFlag = 0;
              //    // Bad case.
              //    anArea = 0.;
              //  }
              //}
            }

            anArea += aTmpArea;
          }
        }//for(ii=1; ii<nbpnts; ii++,im0++,im1++,im2++) { 

        {
          const Standard_Real aToler = Max(myTol3D, Precision::Confusion());
          if (Abs(anArea) < aToler*aToler)
          {
            // anArea = |A|*|B|*sin(Angle) < Tol*Tol
            anArea = 0.;
            BadWire = 1;
          }
        }
        //
        FlecheU = Max(myTolU, FlecheU);
        FlecheV = Max(myTolV, FlecheV);

        if (!BadWire)
        {
          TabClass.Append(new CSLib_Class2d(PClass,
                                            FlecheU, FlecheV,
                                            Umin, Vmin, Umax, Vmax));

          TabOrien.Append((anArea > 0.0) ? 1 : 0);
        }
      }
      
      if (BadWire)
      {
        TabOrien.Append(-1);
        TColgp_Array1OfPnt2d PPClass(1, 2);
        PPClass.Init(anInitPnt);
        TabClass.Append(new CSLib_Class2d(PPClass,
                                          FlecheU,
                                          FlecheV,
                                          Umin, Vmin, Umax, Vmax));
      }
    }// else if(WireIsNotEmpty)

    if (BadWire)
      continue;

    // 2. Shake the tree filler
    aTreeFiller.Fill();

    for (Standard_Integer aVecIdx = anArrBoxes.Lower();
         aVecIdx <= anArrBoxes.Upper(); aVecIdx++)
    {
      const Bnd_Box2d& aBox = anArrBoxes(aVecIdx);
      BRepTopAdaptor_FClass2dSel aSelector(SeqPnt2d, aBox, aVecIdx);
      if (aBBTree.Select(aSelector))
      {
        BadWire = Standard_True;
        break;
      }
    }
  } // for(; aExpF.More();  aExpF.Next()) {

  if ((TabClass.Length() > 0) && (BadWire))
  {
    //-- if an error on a wire was detected : all TabOrien set to -1
    TabOrien(1) = -1;
  }
}

//=======================================================================
//function : PerformInfinitePoint
//purpose  : 
//=======================================================================
TopAbs_State BRepTopAdaptor_FClass2d::PerformInfinitePoint() const
{
  if (Umax == -RealLast() || Vmax == -RealLast() ||
      Umin == RealLast() || Vmin == RealLast())
  {
    return TopAbs_IN;
  }

  const Standard_Real aMaxTol = Min(10.0, BRep_Tool::MaxTolerance(Face, TopAbs_VERTEX));
  const BRepAdaptor_Surface aS(Face, Standard_False);
  gp_Pnt2d aP;

  // For periodic surfaces, Umax - Umin (or Vmax - Vmin)
  // can be equal to period. In this case, shift on 
  // k*(Umax - Umin), where k is an integer number,
  // will not take any effect in 3D-space. Therefore, we apply
  // that k is not integer.
  const Standard_Real aDeltaU = 1.5*(Umax - Umin),
                      aDeltaV = 1.5*(Vmax - Vmin);

  if (aDeltaU < aDeltaV)
  {
    aP.SetCoord((Umin - Max(aDeltaU + 2.0*aS.UResolution(aMaxTol), 1.0)),
                                                              0.5*(Vmax + Vmin));

    //In order to avoid work with infinite-located curves (e.g. in
    //half-spaces)
    if (Precision::IsInfinite(Vmax) && Precision::IsInfinite(Vmin))
    {
      aP.SetY(0.0);
    }
    else if (Precision::IsInfinite(Vmax))
    {
      aP.SetY(Vmin - 100.0*aS.VResolution(aMaxTol));
    }
    else if (Precision::IsInfinite(Vmin))
    {
      aP.SetY(Vmax + 100.0*aS.VResolution(aMaxTol));
    }
  }
  else
  {
    aP.SetCoord(0.5*(Umax + Umin),
                      (Vmin - Max(aDeltaV + 2.0*aS.VResolution(aMaxTol), 1.0)));

    //In order to avoid work with infinite-located curves (e.g. in
    //half-spaces)
    if (Precision::IsInfinite(Umax) && Precision::IsInfinite(Umin))
    {
      aP.SetX(0.0);
    }
    else if (Precision::IsInfinite(Umax))
    {
      aP.SetX(Umin - 100.0*aS.UResolution(aMaxTol));
    }
    else if (Precision::IsInfinite(Umin))
    {
      aP.SetX(Umax + 100.0*aS.UResolution(aMaxTol));
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
        BRepTopAdaptor_FClass2d::Perform(const gp_Pnt2d& _Puv,
                                         const Standard_Boolean theIsReqToAdjust) const
{ 
  Standard_Integer nbtabclass = TabClass.Length();
  if (nbtabclass == 0)
  {
    return TopAbs_IN;
  }

  //-- U1 is the First Param and U2 is in this case U1+Period
  Standard_Real u = _Puv.X();
  Standard_Real v = _Puv.Y();
  Standard_Real uu = u;
  Standard_Real vv = v;
  TopAbs_State aStatus = TopAbs_UNKNOWN;

  const BRepAdaptor_Surface anAS(Face, Standard_False);

  const Standard_Boolean IsUPer = anAS.IsUPeriodic();
  const Standard_Boolean IsVPer = anAS.IsVPeriodic();
  const Standard_Real    uperiod = IsUPer ? anAS.UPeriod() : 0.0;
  const Standard_Real    vperiod = IsVPer ? anAS.VPeriod() : 0.0;

  Standard_Boolean urecadre, vrecadre, bUseClassifier;
  Standard_Integer dedans = 1;
  //
  urecadre = Standard_False;
  vrecadre = Standard_False;
  //
  if (theIsReqToAdjust) {
    Standard_Real du, dv;
    if (IsUPer) {
      GeomInt::AdjustPeriodic(uu, Umin, Umax, uperiod, uu, du);
    }// if (IsUPer) {
    //
    if (IsVPer) {
      GeomInt::AdjustPeriodic(vv, Vmin, Vmax, vperiod, vv, dv);
    }//if (IsVPer) {
  }
  //
  for (;;) {
    dedans = 1;
    gp_Pnt2d Puv(u, v);
    bUseClassifier = (TabOrien(1) == -1);
    if (!bUseClassifier) {
      Standard_Integer n, cur, TabOrien_n;
      for (n = 1; n <= nbtabclass; n++) {
        cur = TabClass(n)->Classify(Puv);
        TabOrien_n = TabOrien(n);

        if (cur == 1) {
          if (TabOrien_n == 0) {
            dedans = -1;
            break;
          }
        }
        else if (cur == -1) {
          if (TabOrien_n == 1) {
            dedans = -1;
            break;
          }
        }
        else {
          dedans = 0;
          break;
        }
      } // for(n=1; n<=nbtabclass; n++)

      if (dedans == 0) {
        bUseClassifier = Standard_True;
      }
      else {
        aStatus = (dedans == 1) ? TopAbs_IN : TopAbs_OUT;
      }
    } // if(TabOrien(1)!=-1) {
    //compute state of the point using face classifier
    if (bUseClassifier) {
      myClassifier.Perform(Face, Puv, myTol3D);
      aStatus = myClassifier.State();
    }

    if (!theIsReqToAdjust || (!IsUPer && !IsVPer))
      return aStatus;
    
    if (aStatus == TopAbs_IN || aStatus == TopAbs_ON)
      return aStatus;
    
    if (!urecadre){
      u = uu;
      urecadre = Standard_True;
    }
    else {
      if (IsUPer){
        u += uperiod;
      }
    }

    if (u > Umax || !IsUPer) {
      if (!vrecadre){
        v = vv;
        vrecadre = Standard_True;
      }
      else {
        if (IsVPer){
          v += vperiod;
        }
      }

      u = uu;
      
      if (v > Vmax || !IsVPer) {
        return aStatus;
      }
    }
  } //while (1)
}

//=======================================================================
//function : TestOnRestriction
//purpose  : 
//=======================================================================
TopAbs_State BRepTopAdaptor_FClass2d::TestOnRestriction(const gp_Pnt2d& theP2D,
                                                        const Standard_Real theTol3D,
                                                        const Standard_Boolean theIsReqToAdjust) const
{ 
  Standard_Integer nbtabclass = TabClass.Length();
  if (nbtabclass == 0)
  {
    return TopAbs_IN;
  }

  //-- U1 is the First Param and U2 in this case is U1+Period
  Standard_Real u = theP2D.X();
  Standard_Real v = theP2D.Y();
  Standard_Real uu = u, vv = v;
  TopAbs_State aStatus = TopAbs_UNKNOWN;
  const BRepAdaptor_Surface anAS(Face, Standard_False);
  const Standard_Boolean IsUPer = anAS.IsUPeriodic();
  const Standard_Boolean IsVPer = anAS.IsVPeriodic();
  const Standard_Real    uperiod = IsUPer ? anAS.UPeriod() : 0.0;
  const Standard_Real    vperiod = IsVPer ? anAS.VPeriod() : 0.0;
  const Standard_Real aURes = anAS.UResolution(theTol3D),
                      aVRes = anAS.VResolution(theTol3D);
  Standard_Boolean urecadre = Standard_False, vrecadre = Standard_False;
  Standard_Integer dedans = 1;

  if (theIsReqToAdjust) {
    Standard_Real du, dv;
    if (IsUPer) {
      GeomInt::AdjustPeriodic(uu, Umin, Umax, uperiod, uu, du);
    }// if (IsUPer) {
    //
    if (IsVPer) {
      GeomInt::AdjustPeriodic(vv, Vmin, Vmax, vperiod, vv, dv);
    }//if (IsVPer) {
  }
  //
  for (;;) {
    dedans = 1;
    gp_Pnt2d Puv(u, v);

    if (TabOrien(1) != -1) {
      for (Standard_Integer n = 1; n <= nbtabclass; n++) {
        Standard_Integer cur = TabClass(n)->Classify(Puv, aURes, aVRes);
        if (cur == 1) {
          if (TabOrien(n) == 0) {
            dedans = -1;
            break;
          }
        }
        else if (cur == -1) {
          if (TabOrien(n) == 1) {
            dedans = -1;
            break;
          }
        }
        else {
          dedans = 0;
          break;
        }
      }
      if (dedans == 0) {
        aStatus = TopAbs_ON;
      }
      if (dedans == 1) {
        aStatus = TopAbs_IN;
      }
      if (dedans == -1) {
        aStatus = TopAbs_OUT;
      }
    }
    else {  //-- TabOrien(1)=-1  Wrong  Wire 
      BRepClass_FaceClassifier aClassifier;
      aClassifier.Perform(Face, Puv, theTol3D);
      aStatus = aClassifier.State();
    }

    if (!theIsReqToAdjust || (!IsUPer && !IsVPer))
      return aStatus;
    if (aStatus == TopAbs_IN || aStatus == TopAbs_ON)
      return aStatus;

    if (!urecadre)
    {
      u = uu;
      urecadre = Standard_True;
    }
    else
      if (IsUPer)
        u += uperiod;
    if (u > Umax || !IsUPer)
    {
      if (!vrecadre)
      {
        v = vv;
        vrecadre = Standard_True;
      }
      else
        if (IsVPer)
          v += vperiod;

      u = uu;

      if (v > Vmax || !IsVPer)
        return aStatus;
    }
  } //for (;;)
}

//=======================================================================
//function : Destroy
//purpose  : 
//=======================================================================
void BRepTopAdaptor_FClass2d::Destroy()
{
  Standard_Integer nbtabclass = TabClass.Length();
  for (Standard_Integer d = 1; d <= nbtabclass; d++)
  {
    if (TabClass(d))
    {
      delete TabClass(d);
    }
  }

  TabClass.Clear();
  TabOrien.Clear();
}