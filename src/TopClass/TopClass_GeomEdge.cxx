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
#include <Extrema_ExtPC2d.hxx>
#include <Extrema_LocateExtPC.hxx>
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
                                     const TopClass_GeomVertex &theVl)
  : myOri(theOri), myVertF(theVf), myVertL(theVl)
{
  Init(theC3D, theC2D, theS, theFPar, theLPar);
}

//=======================================================================
//function : Constructor
//purpose  : 
//=======================================================================
TopClass_GeomEdge::TopClass_GeomEdge(const TopoDS_Edge& theE,
                                     const TopoDS_Face& theF) : myTopoEdge(theE)
{
  myOri = theE.Orientation();
  
  if ((myOri != TopAbs_FORWARD) && (myOri != TopAbs_REVERSED))
    return;

  TopoDS_Vertex aVf, aVl;
  TopExp::Vertices(theE, aVf, aVl, Standard_True);

  myVertF.Init(aVf, theE);
  myVertL.Init(aVl, theE);

  Standard_Real aFPar = 1.0, aLPar = -1.0;
  const Handle(Geom_Surface) aS = BRep_Tool::Surface(theF);
  const Handle(Geom_Curve) aC3d = BRep_Tool::Curve(theE, aFPar, aLPar);
  const Handle(Geom2d_Curve) aC2d = BRep_Tool::CurveOnSurface(theE, theF, aFPar, aLPar);

  Init(aC3d, aC2d, aS, aFPar, aLPar);
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

  if (!theS.IsNull())
    mySurf.Load(theS);
}

//=======================================================================
//function : Distance3D
//purpose  : 
//=======================================================================
Standard_Real TopClass_GeomEdge::Distance3D(const gp_Pnt2d &thePt,
                                            const Standard_Real thePar0) const
{
  if (IsPseudo())
  {
    return -1.0;
  }

  const gp_Pnt aP(mySurf.Value(thePt.X(), thePt.Y()));

  Extrema_LocateExtPC anExt(aP, myC3D, thePar0, Precision::Confusion());

  if (!anExt.IsDone() || !anExt.IsMin())
  {
    return -1.0;
  }

  return Sqrt(anExt.SquareDistance());
}

//=======================================================================
//function : Distance2D
//purpose  : 
//=======================================================================
Standard_Real TopClass_GeomEdge::Distance2D(const gp_Pnt2d &thePt,
                                            gp_Pnt2d &thePOnC,
                                            Standard_Real &theParOnC) const
{
  Extrema_ExtPC2d anExtPC2d(thePt, myC2D);

  if (!anExtPC2d.IsDone() || (anExtPC2d.NbExt() == 0))
    return -1.0;

  const Standard_Integer aNbExt = anExtPC2d.NbExt();

  Standard_Real aMinSqDist = RealLast();
  Standard_Integer aMinIndex = 0;

  for (Standard_Integer i = 1; i <= aNbExt; i++)
  {
    const Standard_Real aSqDist = anExtPC2d.SquareDistance(i);

    if (aSqDist > aMinSqDist)
      continue;

    aMinSqDist = aSqDist;
    aMinIndex = i;
  }

  const Extrema_POnCurv2d &aPExt = anExtPC2d.Point(aMinIndex);

  theParOnC = aPExt.Parameter();
  thePOnC = aPExt.Value();

  return Sqrt(aMinSqDist);
}
