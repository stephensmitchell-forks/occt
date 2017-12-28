// Created on: 2017-02-17
// Created by: Nikolai BUKHALOV
// Copyright (c) 1999-2017 OPEN CASCADE SAS
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

#include <TopClass_GeomEdge.hxx>

#include <BRep_Tool.hxx>
#include <Extrema_ExtPC.hxx>
#include <Extrema_ExtPC2d.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Vertex.hxx>

//=======================================================================
//function : Constructor
//purpose  : 
//=======================================================================
TopClass_GeomEdge::TopClass_GeomEdge(const Handle(Geom_Curve)& theC3D,
                                     const Handle(Geom2d_Curve)& theC2D,
                                     const Handle(Geom_Surface)& theS,
                                     const Standard_Real theFPar,
                                     const Standard_Real theLPar,
                                     const TopAbs_Orientation theOri,
                                     const TopClass_GeomVertex &theVf,
                                     const TopClass_GeomVertex &theVl,
                                     const Standard_Boolean theIsSameParameter)
  : myOri(theOri),
    myVertF(theVf),
    myVertL(theVl),
    myIsSameParameter(theIsSameParameter)
{
  Init(theC3D, theC2D, theS, theFPar, theLPar);
}

//=======================================================================
//function : Constructor
//purpose  : 
//=======================================================================
TopClass_GeomEdge::TopClass_GeomEdge(const TopoDS_Edge& theE,
                                     const Handle(Geom_Surface)& theSurf,
                                     const Handle(Geom2d_Curve)& theC2d,
                                     const Standard_Real theFPar,
                                     const Standard_Real theLPar)
  : myTopoEdge(theE)
{
  myOri = theE.Orientation();
  
  if ((myOri != TopAbs_FORWARD) && (myOri != TopAbs_REVERSED))
    return;

  TopoDS_Vertex aVf, aVl;
  TopExp::Vertices(theE, aVf, aVl, Standard_True);

  myVertF.Init(aVf, theE);
  myVertL.Init(aVl, theE);

  Standard_Real aFPar = 1.0, aLPar = -1.0;
  const Handle(Geom_Curve) aC3d = BRep_Tool::Curve(theE, aFPar, aLPar);

  myIsSameParameter = (BRep_Tool::SameParameter(theE) && BRep_Tool::SameRange(theE));

  Init(aC3d, theC2d, theSurf, theFPar, theLPar);
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================
void TopClass_GeomEdge::Init(const Handle(Geom_Curve)& theC3D,
                             const Handle(Geom2d_Curve)& theC2D,
                             const Handle(Geom_Surface)& theS,
                             const Standard_Real theFPar,
                             const Standard_Real theLPar)
{
  if (theC2D.IsNull())
    return;

  myC2D.Load(theC2D, theFPar, theLPar);
    
  if (!theC3D.IsNull())
    myC3D.Load(theC3D, theFPar, theLPar);
  else
    myIsSameParameter = Standard_False;

  if (!theS.IsNull())
    mySurf.Load(theS);
  else
    myIsSameParameter = Standard_False;
}

//=======================================================================
//function : Distance3D
//purpose  : 
//=======================================================================
Standard_Real TopClass_GeomEdge::Distance3D(const gp_Pnt &thePt,
                                            gp_Pnt *thePOnC,
                                            Standard_Real *theParOnC) const
{
  if (IsPseudo())
  {
    return -1.0;
  }

  // Default value for extrema algo.
  const Standard_Real aTolPar = 1.0e-10;

  Extrema_ExtPC anEG(thePt, myC3D, aTolPar);

  if (!anEG.IsDone() || (anEG.NbExt() == 0))
  {
    const Standard_Real aDistF = thePt.SquareDistance(myVertF.GetPnt3D());
    const Standard_Real aDistL = thePt.SquareDistance(myVertL.GetPnt3D());

    if (aDistF < aDistL)
    {
      if (thePOnC)
        thePOnC->SetXYZ(myVertF.GetPnt3D().XYZ());
      if (theParOnC)
        *theParOnC = myVertF.ParameterOnEdge();

      return Sqrt(aDistF);
    }
    else // if (aDistL <= aDistF)
    {
      if (thePOnC)
        thePOnC->SetXYZ(myVertL.GetPnt3D().XYZ());
      if (theParOnC)
        *theParOnC = myVertL.ParameterOnEdge();

      return Sqrt(aDistL);
    }
  }

  const Standard_Integer aNbExt = anEG.NbExt();
  const Extrema_POnCurv *aNearPoint = 0;
  Standard_Real aMinSqDist = RealLast();

  for (Standard_Integer i = 1; i <= aNbExt; i++)
  {
    const Standard_Real aSqDist = anEG.SquareDistance(i);

    if (aSqDist > aMinSqDist)
      continue;

    aMinSqDist = aSqDist;

    const Extrema_POnCurv &aEPt = anEG.Point(i);
    aNearPoint = &aEPt;
  }

  if (aNearPoint == 0)
  {
    return -1.0;
  }

  if (thePOnC)
    thePOnC->SetXYZ(aNearPoint->Value().XYZ());
  if (theParOnC)
    *theParOnC = aNearPoint->Parameter();

  return Sqrt(aMinSqDist);
}

//=======================================================================
//function : Distance2D
//purpose  : 
//=======================================================================
Standard_Real TopClass_GeomEdge::Distance2D(const gp_Pnt2d &thePt,
                                            gp_Pnt2d *thePOnC,
                                            Standard_Real *theParOnC) const
{
  // Default value for extrema algo.
  const Standard_Real aTolPar = 1.0e-10;

  Extrema_ExtPC2d anEG(thePt, myC2D, aTolPar);

  if (!anEG.IsDone() || (anEG.NbExt() == 0))
  {
    const Standard_Real aParF = myC2D.FirstParameter();
    const Standard_Real aParL = myC2D.LastParameter();

    const gp_Pnt2d aPtF(myC2D.Value(aParF)), aPtL(myC2D.Value(aParL));
    const Standard_Real aDistF = thePt.SquareDistance(aPtF);
    const Standard_Real aDistL = thePt.SquareDistance(aPtL);

    if (aDistF < aDistL)
    {
      if (thePOnC)
        thePOnC->SetXY(aPtF.XY());
      if (theParOnC)
        *theParOnC = aParF;

      return Sqrt(aDistF);
    }
    else // if (aDistL <= aDistF)
    {
      if (thePOnC)
        thePOnC->SetXY(aPtL.XY());
      if (theParOnC)
        *theParOnC = aParL;

      return Sqrt(aDistL);
    }
  }

  const Standard_Integer aNbExt = anEG.NbExt();
  const Extrema_POnCurv2d *aNearPoint = 0;
  Standard_Real aMinSqDist = RealLast();

  for (Standard_Integer i = 1; i <= aNbExt; i++)
  {
    const Standard_Real aSqDist = anEG.SquareDistance(i);

    if (aSqDist > aMinSqDist)
      continue;

    aMinSqDist = aSqDist;

    const Extrema_POnCurv2d &aEPt = anEG.Point(i);
    aNearPoint = &aEPt;
  }

  if (aNearPoint == 0)
  {
    return -1.0;
  }

  if (thePOnC)
    thePOnC->SetXY(aNearPoint->Value().XY());
  if (theParOnC)
    *theParOnC = aNearPoint->Parameter();

  return Sqrt(aMinSqDist);
}
