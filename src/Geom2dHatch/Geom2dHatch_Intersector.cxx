// Created on: 1994-03-23
// Created by: Jean Marc LACHAUME
// Copyright (c) 1994-1999 Matra Datavision
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


#include <ElCLib.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <Geom2dHatch_Intersector.hxx>
#include <Geom2dLProp_CLProps2d.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Lin2d.hxx>
#include <Precision.hxx>
#include <TopClass_GeomEdge.hxx>

//=======================================================================
//function : Geom2dHatch_Intersector
//purpose  : 
//=======================================================================
Geom2dHatch_Intersector::Geom2dHatch_Intersector() :
myConfusionTolerance(0.0),
myTangencyTolerance(0.0)
{
}

//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================

void  Geom2dHatch_Intersector::Perform(const gp_Lin2d& theLin, 
                                       const Standard_Real thePar, 
                                       const Standard_Real theTol2D,
                                       const TopClass_GeomEdge& theGE)
{
  const gp_Pnt2d &aPnt2D = theLin.Location();
  IntRes2d_Domain DL;
  if(thePar != RealLast())
    DL.SetValues(aPnt2D, 0., theTol2D, ElCLib::Value(thePar, theLin), thePar, theTol2D);
  else
    DL.SetValues(aPnt2D, 0., theTol2D, Standard_True);

  const Geom2dAdaptor_Curve &anAC = theGE.Get2DCurve();

  IntRes2d_Domain DE(anAC.Value(anAC.FirstParameter()),
                     anAC.FirstParameter(), Precision::PIntersection(),
                     anAC.Value(anAC.LastParameter()),
                     anAC.LastParameter(), Precision::PIntersection());

  Handle(Geom2d_Line) GL = new Geom2d_Line(theLin);
  Geom2dAdaptor_Curve CGA(GL);

  Geom2dInt_GInter Inter(CGA,
                         DL,
                         anAC,
                         DE,
                         Precision::PConfusion(),
                         Precision::PIntersection());
  this->SetValues(Inter);
}
