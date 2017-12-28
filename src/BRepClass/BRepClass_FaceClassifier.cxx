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


#include <BRep_Builder.hxx>
#include <BRepAdaptor_HSurface.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepClass_FaceClassifier.hxx>
#include <BRepClass_FaceExplorer.hxx>
#include <BRepExtrema_DistShapeShape.hxx>
#include <BRepTools.hxx>
#include <BRepBndLib.hxx>
#include <Extrema_ExtPS.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <TopAbs_State.hxx>
#include <TopExp.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>

#if 0
Standard_Boolean Check2D(const TopoDS_Edge &theTopE,
                         const TopoDS_Face &theTopF,
                         const Standard_Real theCurvParam,
                         const gp_Pnt2d& thePRef,
                         const Standard_Real theTol3D,
                         const Standard_Boolean theIsOnClosedSurf);

Standard_Boolean Check2D(const TopoDS_Vertex &theTopV,
                         const TopoDS_Face &theTopF,
                         const gp_Pnt2d& thePRef,
                         const Standard_Real theTol3D);
#else
Standard_Boolean Check2D(const TopClass_GeomEdge& theGE,
                         const Standard_Real theCurvParam,
                         const gp_Pnt2d& thePRef,
                         const Standard_Real theTol3D,
                         gp_Pnt2d& thePOnC);

#endif
//=======================================================================
//function : BRepClass_FaceClassifier
//purpose  : 
//=======================================================================
BRepClass_FaceClassifier::BRepClass_FaceClassifier():myFEx(TopoDS_Face())
{
}

//=======================================================================
//function : BRepClass_FaceClassifier
//purpose  : 
//=======================================================================
BRepClass_FaceClassifier::BRepClass_FaceClassifier(const TopoDS_Face& theF, 
                                                   const gp_Pnt& theP, 
                                                   const Standard_Real theTol3D)
: myFEx(theF)
{
  Perform(theF, theP, theTol3D);
}
//=======================================================================
//function : BRepClass_FaceClassifier
//purpose  : 
//=======================================================================
BRepClass_FaceClassifier::
            BRepClass_FaceClassifier(const TopoDS_Face& theF, 
                                     const gp_Pnt2d& theP,
                                     const Standard_Real theTol3D) : myFEx(theF)
{
  Perform(theF, theP, theTol3D);
}

//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================
void  BRepClass_FaceClassifier::Perform(const TopoDS_Face& theF, 
                                        const gp_Pnt2d& theP, 
                                        const Standard_Real theTol3D)
{
  // First, let's check of ON-status
  
#if 0
  // Indeed, this code is more optimal than existing.
  // However, it cannot be implemented until the bug #29288
  // will be fixed.

  const Standard_Real aSqTol = theTol3D*theTol3D;
  const BRepAdaptor_Surface anAS(theF, Standard_False);
  const gp_Pnt aPt(anAS.Value(theP.X(), theP.Y()));
  TopoDS_Vertex aVert;
  BRep_Builder aBB;
  aBB.MakeVertex(aVert, aPt, Precision::Confusion());
  TopoDS_Iterator aWirExp(theF);
  for(; aWirExp.More(); aWirExp.Next())
  {
    // aWir is a wire
    const TopoDS_Shape& aWir = aWirExp.Value();
    BRepExtrema_DistShapeShape anExtr(aWir, aVert);
    if(!anExtr.IsDone() || (anExtr.NbSolution() < 1))
    {
      continue;
    }

    if(anExtr.Value() > theTol3D)
    {
      continue;
    }

    // It is necessary for periodic and quasi-periodic (e.g. sphere)
    // surfaces (tests bug28211_18, bug28211_32, bug28211_33 and
    // bug28211_35 in "tests/lowalgos/classifier/").
    // E.g. if 2D-distance between theP and the p-curve of some edge
    // is equal to T (the surface period) then 3D-point
    // corresponding to theP can be exactly in the edge.
    // So, 3D-distance is less than theTol3D however,
    // returning ON-status is wrong in this case.
    // Therefore, check in 2D-space is executed.
    
    // ATTENTION!!!
    // This check will be wrong if theTol3D covers the 2D-distance
    // of the surface. I.e. if two 3D-points of any cylinder (for example)
    // having 2D-coordinates (0, 0) and (PI, 0) can
    // be considered as same with precision theTol3D.
    // But for periodic surfaces, Check2D(...) method
    // can return FALSE even if theTol3D is big
    // (because this check is based on period value
    // instead of tolerance).

    for(Standard_Integer i = 1; i <= anExtr.NbSolution(); i++)
    {
      if(anExtr.PointOnShape1(i).SquareDistance(aPt) > aSqTol)
        continue;

      switch(anExtr.SupportTypeShape1(i))
      {
        case BRepExtrema_IsVertex:
        {
          const TopoDS_Shape aSh = anExtr.SupportOnShape1(i);
          if(Check2D(TopoDS::Vertex(aSh), theF, theP, theTol3D))
          {
            myState = TopAbs_ON;
            TopTools_IndexedDataMapOfShapeListOfShape aM;
            TopExp::MapShapesAndAncestors(theF, TopAbs_VERTEX, TopAbs_EDGE, aM);
            const TopoDS_Shape &anE = aM.FindFromKey(aSh).First();
            myEdgeParameter = BRep_Tool::Parameter(TopoDS::Vertex(aSh), TopoDS::Edge(anE));
            myEdge = TopClass_GeomEdge(TopoDS::Edge(anE), theF);
            return;
          }
        }
        break;

        case BRepExtrema_IsOnEdge:
        {
          const TopoDS_Shape aSh = anExtr.SupportOnShape1(i);
          const TopoDS_Edge &anE = TopoDS::Edge(aSh);

          //Indeed, seam-edge is included to the wire aWir twice.
          //However, BRepExtrema_DistShapeShape will return only one
          //representation of the nearest edge (at that this representation
          //is chosen randomly). Maybe the edge will be chosen whose p-curve
          //is farer from the classified point. Therefore, we must
          //check all CurveRepresentation of this edge. For that,
          //the flag isOnClosedSurface is used.
          Standard_Boolean isOnClosedSurface = BRep_Tool::IsClosed(anE, theF);

          if(isOnClosedSurface)
          {
            // See face_with_rejected_seam.brep file attached to
            // the issue #28211 (classified point has
            // (-1.0000000000000004 0.48095952536907932) coordinates).
            // This face contains the edge with two p-curves (on this face)
            // but this edge is included in the face once (i.e. indeed it is
            // not seam). It seems that the excess p-curve was not deleted
            // after splitting of the face but it must not be taken into
            // account when classifying.
            // So, we need extend check here whether this edge is really met
            // in the wire twice.

            // Take edges
            TopoDS_Iterator anItr(aWir);
            for(; anItr.More(); anItr.Next())
            {
              const TopoDS_Edge &WE = TopoDS::Edge(anItr.Value());
              if(anE.IsSame(WE))
              {
                isOnClosedSurface = !isOnClosedSurface;
              }
            }
          }

          Standard_Real aPar = 0.0;
          anExtr.ParOnEdgeS1(i, aPar);
          if(Check2D(anE, theF, aPar, theP, theTol3D, isOnClosedSurface))
          {
            myState = TopAbs_ON;
            myEdgeParameter = aPar;
            myEdge = TopClass_GeomEdge(TopoDS::Edge(aSh), theF);
            return;
          }
        }
        break;

        default:
          break;
      }
    }
  }
#else
  const Handle(Geom_Surface) aS = BRep_Tool::Surface(theF);
  gp_Pnt aP3d = aS->Value(theP.X(), theP.Y());
  TopoDS_Iterator aWirExp(theF);
  for(; aWirExp.More(); aWirExp.Next())
  {
    // aWir is a wire
    const TopoDS_Shape& aWir = aWirExp.Value();
    
    TopoDS_Iterator anEExp(aWir);
    for(; anEExp.More(); anEExp.Next())
    {
      // anE is an edge
      const TopoDS_Edge& anE = TopoDS::Edge(anEExp.Value());

      Bnd_Box aBox;
      BRepBndLib::Add(anE, aBox);
      aBox.Enlarge(theTol3D);
      if (aBox.IsOut(aP3d))
        continue;

      Standard_Real aFPar, aLPar;
      const Handle(Geom2d_Curve) aC2d = BRep_Tool::CurveOnSurface(anE, theF, aFPar, aLPar);
      const TopClass_GeomEdge aGE(anE, aS, aC2d, aFPar, aLPar);

      Standard_Real aParam = RealFirst();
      const Standard_Real aDist = aGE.Distance3D(aP3d, 0, &aParam);
      if((0.0 <= aDist) && (aDist <= theTol3D))
      {
        // It is necessary for periodic and quasi-periodic (e.g. sphere)
        // surfaces (tests bug28211_18, bug28211_32, bug28211_33 and
        // bug28211_35 in "tests/lowalgos/classifier/").
        // E.g. if 2D-distance between theP and the p-curve of some edge
        // is equal to T (the surface period) then 3D-point
        // corresponding to theP can be exactly in the edge.
        // So, 3D-distance is less than theTol3D however,
        // returning ON-status is wrong in this case.
        // Therefore, check in 2D-space is executed.

        // ATTENTION!!!
        // This check will be wrong if theTol3D covers the 2D-distance
        // of the surface. I.e. if two 3D-points of any cylinder (for example)
        // having 2D-coordinates (0, 0) and (PI, 0) can
        // be considered as same with precision theTol3D.
        // But for periodic surfaces, Check2D(...) method
        // can return FALSE even if theTol3D is big
        // (because this check is based on period value
        // instead of tolerance).

        gp_Pnt2d aPOnC;
        if(Check2D(aGE, aParam, theP, theTol3D, aPOnC))
        {
          myState = TopAbs_ON;
          myEdgeParameter = aParam;
          myEdge = aGE;
          return;
        }
      }
    }
  }
#endif

  // For information only.
  // BRepClass_FaceClassifier works wrong with small faces
  // (in 2D-space): UV-dimensions are less than 1.0e-5.
  // See tests
  //    boolean bopcommon_complex G5, boolean bopcut_complex H4,
  //    boolean bopfuse_complex F7, G1, G2,
  //    boolean boptuc_complex C2, C5, C6.

  myFEx.Init(theF);
  BRepClass_FClassifier::Perform(myFEx, theP, 0.0);
}

//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================
void  BRepClass_FaceClassifier::Perform(const TopoDS_Face& theF, 
                                        const gp_Pnt& theP, 
                                        const Standard_Real theTol3D)
{
  Standard_Integer aNbExt, aIndice, i; 
  Standard_Real aU1, aU2, aV1, aV2, aMaxDist, aD;
  gp_Pnt2d aPuv;
  Extrema_ExtPS aExtrema;
  //
  aMaxDist=RealLast();
  aIndice=0;
  //
  BRepAdaptor_Surface aSurf(theF);
  BRepTools::UVBounds(theF, aU1, aU2, aV1, aV2);
  aExtrema.Initialize(aSurf, aU1, aU2, aV1, aV2,
                        aSurf.UResolution(theTol3D), aSurf.VResolution(theTol3D));
  //
  aExtrema.Perform(theP);
  if(!aExtrema.IsDone()) {
    return;
  }
  //
  aNbExt=aExtrema.NbExt();
  if(!aNbExt) {
    return;
  }
  //
  for (i=1; i<=aNbExt; ++i) {
    aD=aExtrema.SquareDistance(i);
    if(aD < aMaxDist) { 
      aMaxDist=aD;
      aIndice=i;
    }
  }
  //
  if(aIndice) { 
    aExtrema.Point(aIndice).Parameter(aU1, aU2);
    aPuv.SetCoord(aU1, aU2);
    Perform(theF, aPuv, theTol3D);
  }
}

//=======================================================================
//function : DeltaUMax
//purpose  : Returns maximal delta of U-parameter, which is permissible
//            for returning ON-status.
//           Returns negative value in case when this delta cannot be computed.
//=======================================================================
static Standard_Real DeltaUMax(const Adaptor3d_Surface &theAS)
{
  // As it is shown in lowalgos/classifier/bug28211_18,
  // bug28211_32...bug28211_36, sometimes isolines
  // U=0 and U=T/2 (where T is the period) are the same (in fact).
  // Therefore, there is no point in returning T/2-value.
  // Here, constant T/4 is chosen.

  return theAS.IsUPeriodic() ? 0.25*theAS.UPeriod() : -1.0;
}

//=======================================================================
//function : DeltaVMax
//purpose  : Returns maximal delta of V-parameter, which is permissible
//            for returning ON-status.
//           Returns negative value in case when this delta cannot be computed.
//=======================================================================
static Standard_Real DeltaVMax(const Adaptor3d_Surface &theAS,
                               const Standard_Real theTol3D)
{
  switch(theAS.GetType())
  {
    case GeomAbs_OffsetSurface:
      //See lowalgos/classifier/bug28211_39 test case.
      return DeltaVMax(theAS.BasisSurface()->Surface(), theTol3D);
      Standard_FALLTHROUGH
    case GeomAbs_Sphere:
    case GeomAbs_SurfaceOfRevolution:
      return theAS.VResolution(theTol3D);
      Standard_FALLTHROUGH
    default:
      break;
  }

  // As it is shown in lowalgos/classifier/bug28211_18,
  // bug28211_32...bug28211_36, sometimes isolines
  // V=0 and V=T/2 (where T is the period) are the same (in fact).
  // Therefore, there is no point in returning T/2-value.
  // Here, constant T/4 is chosen.

  return theAS.IsVPeriodic() ? 0.25*theAS.VPeriod() : -1.0;
}

#if 0
//=======================================================================
//function : Check2D
//purpose  : Returns TRUE if theP and theProjPt are the same points
//            with tolerance criterion.
//=======================================================================
Standard_Boolean Check2D(const TopoDS_Face &theTopF,
                         const gp_Pnt2d& thePRef,
                         const gp_Pnt2d& theProjPt,
                         const Standard_Real theTol3D)
{
  const BRepAdaptor_Surface anAS(theTopF, Standard_False);

  if(anAS.GetType() == GeomAbs_OtherSurface)
    return Standard_False;

  const Standard_Real aDU = Abs(theProjPt.X() - thePRef.X());
  const Standard_Real aDUMax = DeltaUMax(anAS);
  if((aDUMax >= 0.0) && (aDU > aDUMax))
    return Standard_False;

  const Standard_Real aDV = Abs(theProjPt.Y() - thePRef.Y());
  const Standard_Real aDVMax = DeltaVMax(anAS, theTol3D);
  if((aDVMax >= 0.0) && (aDV > aDVMax))
    return Standard_False;

  return Standard_True;
}

//=======================================================================
//function : Check2D
//purpose  : Returns TRUE if thePRef matches the point in the p-curve
//            of theTopE on theTopF with parameter theCurvParam.
//=======================================================================
Standard_Boolean Check2D(const TopoDS_Edge &theTopE,
                         const TopoDS_Face &theTopF,
                         const Standard_Real theCurvParam,
                         const gp_Pnt2d& thePRef,
                         const Standard_Real theTol3D,
                         const Standard_Boolean theIsOnClosedSurf)
{
  gp_Pnt2d aPOnC;
  if(BRep_Tool::SameParameter(theTopE) && BRep_Tool::SameRange(theTopE))
  {
    Standard_Real aFPar, aLPar;
    const Handle(Geom2d_Curve) aC1 = BRep_Tool::CurveOnSurface(theTopE, theTopF, aFPar, aLPar);
    aC1->D0(theCurvParam, aPOnC);

    // If theTopE is a seam-edge.
    if(theIsOnClosedSurf)
    {
      const Handle(Geom2d_Curve) aC2 = BRep_Tool::CurveOnSurface(TopoDS::Edge(theTopE.Reversed()),
                                                                 theTopF, aFPar, aLPar);

      const gp_Pnt2d aPtemp(aC2->Value(theCurvParam));

      if(thePRef.SquareDistance(aPtemp) < thePRef.SquareDistance(aPOnC))
      {
        aPOnC = aPtemp;
      }
    }
  }
  else
  {
    TopClass_GeomEdge aGE(theTopE, theTopF);
    const Standard_Real aDist = aGE.Distance2D(thePRef, &aPOnC);
    if(aDist < 0.0)
      return Standard_False;
  }

  return Check2D(theTopF, thePRef, aPOnC, theTol3D);
}

//=======================================================================
//function : Check2D
//purpose  : Returns TRUE if thePRef matches the point, which is
//            2D-image of theTopV in theTopF.
//=======================================================================
Standard_Boolean Check2D(const TopoDS_Vertex &theTopV,
                         const TopoDS_Face &theTopF,
                         const gp_Pnt2d& thePRef,
                         const Standard_Real theTol3D)
{
  const gp_Pnt2d aPt = BRep_Tool::Parameters(theTopV, theTopF);
  return Check2D(theTopF, thePRef, aPt, theTol3D);
}

#else
//=======================================================================
//function : Check2D
//purpose  : Returns TRUE if theP and thePRef are the same points.
//           E.g. if X-coord of thePRef is equal to 0 and X-coord of theP
//            is equal to T (the period of theSurf) then, most likely,
//            theP and thePRef will be the same point in 3D-space.
//           However, these points must be considered as different.
//            It is made by this function.
//           Another example why this function is useful is described in
//            the test script "lowalgos/classifier/bug28211_18" (please see
//            comment there).
//=======================================================================
Standard_Boolean Check2D(const TopClass_GeomEdge& theGE,
                         const Standard_Real theCurvParam,
                         const gp_Pnt2d& thePRef,
                         const Standard_Real theTol3D,
                         gp_Pnt2d& thePOnC)
{
  if(theGE.IsSameParameter())
  {
    const Geom2dAdaptor_Curve &aC = theGE.Get2DCurve();
    aC.D0(theCurvParam, thePOnC);
  }
  else
  {
    const Standard_Real aDist = theGE.Distance2D(thePRef, &thePOnC);
    if(aDist < 0.0)
      return Standard_False;
  }

  const Adaptor3d_Surface &anAS = theGE.GetSurface();

  if(anAS.GetType() == GeomAbs_OtherSurface)
    return Standard_False;

  const Standard_Real aDU = Abs(thePOnC.X() - thePRef.X());
  const Standard_Real aDUMax = DeltaUMax(anAS);
  if((aDUMax >= 0.0) && (aDU > aDUMax))
    return Standard_False;

  const Standard_Real aDV = Abs(thePOnC.Y() - thePRef.Y());
  const Standard_Real aDVMax = DeltaVMax(anAS, theTol3D);
  if((aDVMax >= 0.0) && (aDV > aDVMax))
    return Standard_False;

  return Standard_True;
}
#endif


