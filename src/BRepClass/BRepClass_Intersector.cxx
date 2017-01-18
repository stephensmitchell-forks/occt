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

#include <Adaptor3d_HSurface.hxx>
#include <ElCLib.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2dInt_GInter.hxx>
#include <Geom2dLProp_CLProps2d.hxx>
#include <TopClass_GeomEdge.hxx>

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
                                     const Standard_Real /*theTol3D*/,
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

  // There is no point in intersecting geometrical curves. Here,
  // intersection of line with polygonal-representation of the anAdC2d
  // will be enough. After we determine the nearest (to the
  // classified point) edge, only this edge needs to be GEOMETRICALY
  // intersected with the line (e.g. in order to define correct transition).

  Handle(Geom2d_Line) GL = new Geom2d_Line(theLin);
  Geom2dAdaptor_Curve CGA(GL);
  Geom2dInt_GInter Inter(CGA, DL, anAdC2d, DE,
                         Precision::PConfusion(),
                         Precision::PIntersection());
  //
  SetValues(Inter);
}