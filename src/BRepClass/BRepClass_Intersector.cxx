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

static
      void RefineTolerance(const TopClass_GeomEdge& theGE,
                           const Standard_Real theParam,
                           Standard_Real& theTol2D);


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
                                     const Standard_Real theTol2D, 
                                     const TopClass_GeomEdge& theGE)
{
  if (theGE.IsNull())
  {
    done = Standard_False; // !IsDone()
    return;
  }
  Standard_Real aTolZ = theTol2D;
  const gp_Pnt2d &aPnt2D = theLin.Location();
  const Standard_Real aParF = theGE.FirstParameter();
  const Standard_Real aParL = theGE.LastParameter();

  {
    // Case of "ON": direct check of belonging to edge
    // taking into account the tolerance
    gp_Pnt2d aPOnC;
    Standard_Real aParam = aParF;
    const Standard_Real aDist = theGE.Distance2D(aPnt2D, aPOnC, aParam);
    if ((0.0 <= aDist) && (aDist <= aTolZ))
    {
      //
      RefineTolerance(theGE, aParam, aTolZ);
      //
      if (aDist <= aTolZ)
      {
        IntRes2d_Transition tr_on_lin(IntRes2d_Head);
        IntRes2d_Position pos_on_curve = IntRes2d_Middle;
        if (Abs(aParF-aParam) <= Precision::Confusion())
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
  Standard_Real toldeb = 1.e-5, tolfin = 1.e-5;

  IntRes2d_Domain DL;
  //
  if (thePar != RealLast()) {
    DL.SetValues(aPnt2D, 0., aTolZ, ElCLib::Value(thePar, theLin), thePar, aTolZ);
  }
  else { 
    DL.SetValues(aPnt2D, 0., aTolZ, Standard_True);
  }

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
//function : RefineTolerance
//purpose  : 
//=======================================================================
void RefineTolerance(const TopClass_GeomEdge& theGE,
                     const Standard_Real theParam,
                     Standard_Real& theTol2D)
{
  

  const Geom2dAdaptor_Curve &anAC = theGE.Get2DCurve();
  const GeomAdaptor_Surface &anAS = theGE.GetSurface();

  const GeomAbs_SurfaceType aTypeS = anAS.GetType();
  if (aTypeS == GeomAbs_Cylinder) {
    Standard_Real aURes, aVRes, aTolX;
    gp_Pnt2d aP2D;
    gp_Vec2d aV2D;
    //
    aURes = anAS.UResolution(theTol2D);
    aVRes = anAS.VResolution(theTol2D);
    //
    anAC.D1(theParam, aP2D, aV2D);
    gp_Dir2d aD2D(aV2D);
    //
    aTolX = aURes*aD2D.Y() + aVRes*aD2D.X();
    if (aTolX<0.) {
      aTolX = -aTolX;
    }
    //
    if (aTolX < Precision::Confusion()) {
      aTolX = Precision::Confusion();
    }
    //
    if (aTolX < theTol2D)
    {
      theTol2D = aTolX;
    }
  }
}
