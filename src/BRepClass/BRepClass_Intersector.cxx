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

#include <BRepClass_Intersector.hxx>

#include <ElCLib.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2dInt_GInter.hxx>
#include <Geom2dLProp_CLProps2d.hxx>
#include <TopClass_GeomEdge.hxx>

static Standard_Boolean Check2D(const TopClass_GeomEdge& theGE,
                                const Standard_Real theCurvParam,
                                const gp_Pnt2d& thePRef,
                                const Standard_Real theTol3D,
                                gp_Pnt2d& thePOnC);

//=======================================================================
//function : BRepClass_Intersector
//purpose  : 
//=======================================================================
BRepClass_Intersector::BRepClass_Intersector()
{
}

//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================
void  BRepClass_Intersector::Perform(const gp_Lin2d& theLin, 
                                     const Standard_Real thePar, 
                                     const Standard_Real theTol3D, 
                                     const TopClass_GeomEdge& theGE)
{
  if (theGE.IsNull())
  {
    done = Standard_False; // !IsDone()
    return;
  }

  const gp_Pnt2d &aPnt2D = theLin.Location();
  const Standard_Real aParF = theGE.FirstParameter();
  const Standard_Real aParL = theGE.LastParameter();

  {
    // Case of "ON": direct check of belonging to edge
    // taking into account the tolerance
    Standard_Real aParam = RealFirst();
    const Standard_Real aDist = theGE.Distance3D(aPnt2D, 0, &aParam);
    if ((0.0 <= aDist) && (aDist <= theTol3D))
    {
      gp_Pnt2d aPOnC;
      if (Check2D(theGE, aParam, aPnt2D, theTol3D, aPOnC))
      {
        IntRes2d_Transition tr_on_lin(IntRes2d_Head);
        IntRes2d_Position pos_on_curve = IntRes2d_Middle;
        if (Abs(aParF - aParam) <= Precision::Confusion())
        {
          pos_on_curve = IntRes2d_Head;
        }
        else if (Abs(aParL - aParam) <= Precision::Confusion())
        {
          pos_on_curve = IntRes2d_End;
        }
        //
        IntRes2d_Transition tr_on_curve(pos_on_curve);
        IntRes2d_IntersectionPoint pnt_inter(aPOnC, 0., aParam,
                                              tr_on_lin, tr_on_curve,
                                              Standard_False);
        //
        Append(pnt_inter);
        done = Standard_True;
        return;
      }
    }
  }

  const Geom2dAdaptor_Curve &anAdC2d = theGE.Get2DCurve();

  //  
  gp_Pnt2d pdeb,pfin;
  anAdC2d.D0(aParF, pdeb);
  anAdC2d.D0(aParL, pfin);

  IntRes2d_Domain DL;
  if (thePar != RealLast()) {
    DL.SetValues(aPnt2D, 0., Precision::PConfusion(),
                 ElCLib::Value(thePar, theLin), thePar, Precision::PConfusion());
  }
  else { 
    DL.SetValues(aPnt2D, 0., Precision::PConfusion(), Standard_True);
  }

  const Standard_Real 
    toldeb = Min(1.0e-5, (theGE.GetOrientation() == TopAbs_FORWARD) ? 
                anAdC2d.Resolution(theGE.FirstVertex().Tolerance()) :
                anAdC2d.Resolution(theGE.LastVertex().Tolerance())),
    tolfin = Min(1.0e-5, (theGE.GetOrientation() == TopAbs_REVERSED) ? 
                anAdC2d.Resolution(theGE.FirstVertex().Tolerance()) :
                anAdC2d.Resolution(theGE.LastVertex().Tolerance()));

  IntRes2d_Domain DE(pdeb, aParF, toldeb, pfin, aParL, tolfin);
  // temporary periodic domain
  if (anAdC2d.IsPeriodic())
  {
    const Standard_Real aPeriod = anAdC2d.LastParameter() - anAdC2d.FirstParameter();
    DE.SetEquivalentParameters(anAdC2d.FirstParameter(),
                               anAdC2d.FirstParameter() + aPeriod);
  }

  Handle(Geom2d_Line) GL = new Geom2d_Line(theLin);
  Geom2dAdaptor_Curve CGA(GL);
  Geom2dInt_GInter Inter(CGA, DL, anAdC2d, DE,
                         Precision::PConfusion(),
                         Precision::PIntersection());
  //
  SetValues(Inter);
}

//=======================================================================
//function : LocalGeometry
//purpose  : 
//=======================================================================
void  BRepClass_Intersector::LocalGeometry(const Geom2dAdaptor_Curve& theAC,
                                           const Standard_Real U, 
                                           gp_Dir2d& Tang, 
                                           gp_Dir2d& Norm, 
                                           Standard_Real& C) const 
{
  const Handle(Geom2d_Curve) &aC = theAC.Curve();
  Geom2dLProp_CLProps2d Prop(aC, U,2,Precision::PConfusion());
  Prop.Tangent(Tang);
  C = Prop.Curvature();
  if (C > Precision::PConfusion())
    Prop.Normal(Norm);
  else
    Norm.SetCoord(Tang.Y(),-Tang.X());
}

//=======================================================================
//function : Check2D
//purpose  : Returns TRUE if theP and thePRef are the same points.
//           E.g. if X-coord of thePRef is equal to 0 and X-coord of theP
//            is equal to T (the period of theSurf) then, most likely,
//            theP and thePRef will be the same point in 3D-space.
//           However, these points must be considered as different.
//            It is made by this function.
//           Another example why this function is useful is described in
//            the test script "bugs modalg_6 bug28211_18" (please see
//            comment there).
//=======================================================================
Standard_Boolean Check2D(const TopClass_GeomEdge& theGE,
                         const Standard_Real theCurvParam,
                         const gp_Pnt2d& thePRef,
                         const Standard_Real theTol3D,
                         gp_Pnt2d& thePOnC)
{
  if (theGE.IsSameParameter())
  {
    const Geom2dAdaptor_Curve &aC = theGE.Get2DCurve();
    aC.D0(theCurvParam, thePOnC);
  }
  else
  {
    const Standard_Real aDist = theGE.Distance2D(thePRef, &thePOnC);
    if (aDist < 0.0)
      return Standard_False;
  }

  const Adaptor3d_Surface &anAS = theGE.GetSurface();

  if (anAS.GetType() == GeomAbs_OtherSurface)
    return Standard_False;

  const Standard_Real aDU = Abs(thePOnC.X() - thePRef.X());
  const Standard_Real aDUMax = anAS.UResolution(theTol3D);
  if (aDU > aDUMax)
    return Standard_False;

  const Standard_Real aDV = Abs(thePOnC.Y() - thePRef.Y());
  const Standard_Real aDVMax = anAS.VResolution(theTol3D);
  if (aDV > aDVMax)
    return Standard_False;

  return Standard_True;
}
