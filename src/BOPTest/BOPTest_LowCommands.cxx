// Created on: 2001-03-28
// Created by: Peter KURNEV
// Copyright (c) 2001-2014 OPEN CASCADE SAS
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


#include <BOPTest.hxx>
#include <BOPTools_AlgoTools2D.hxx>
#include <BRep_CurveRepresentation.hxx>
#include <BRep_GCurve.hxx>
#include <BRep_ListIteratorOfListOfCurveRepresentation.hxx>
#include <BRep_TEdge.hxx>
#include <BRep_Tool.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepBuilderAPI_MakePolygon.hxx>
#include <BRepClass3d_SolidClassifier.hxx>
#include <BRepClass_FaceClassifier.hxx>
#include <BRepTopAdaptor_FClass2d.hxx>
#include <CSLib_Class2d.hxx>
#include <DBRep.hxx>
#include <Draw.hxx>
#include <DrawTrSurf.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <TCollection_AsciiString.hxx>
#include <TopAbs_State.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>

#include <stdio.h>
static
  void PrintState (Draw_Interpretor& aDI,
                   const TopAbs_State& aState);
static
  Handle(Geom2d_Curve) CurveOnSurface(const TopoDS_Edge& E, 
                                      const TopoDS_Face& F,
                                      Standard_Real& First,
                                      Standard_Real& Last);
static
  Handle(Geom2d_Curve) CurveOnSurface(const TopoDS_Edge& E, 
                                      const Handle(Geom_Surface)& S,
                                      const TopLoc_Location& L,
                                      Standard_Real& First,
                                      Standard_Real& Last);

static  Standard_Integer bclassify   (Draw_Interpretor& , Standard_Integer , const char** );
static  Standard_Integer b2dclassify (Draw_Interpretor& , Standard_Integer , const char** );
static  Standard_Integer b2dclassifx (Draw_Interpretor& , Standard_Integer , const char** );
static  Standard_Integer bhaspc      (Draw_Interpretor& , Standard_Integer , const char** );
static  Standard_Integer IsHole      (Draw_Interpretor&, Standard_Integer, const char**);
static  Standard_Integer polyclassify( Draw_Interpretor&, Standard_Integer, const char** );

//=======================================================================
//function : LowCommands
//purpose  : 
//=======================================================================
  void  BOPTest::LowCommands(Draw_Interpretor& theCommands)
{
  static Standard_Boolean done = Standard_False;
  if (done) return;
  done = Standard_True;
  // Chapter's name
  const char* g = "BOPTest commands";
  theCommands.Add("bclassify"    , "use bclassify Solid Point [Tolerance=1.e-7]",
                  __FILE__, bclassify   , g);
  theCommands.Add("b2dclassify", "use b2dclassify Face Point2d "
                  "[-t Tol3D = <FaceToler>] :\n "
                  "Tol3D is 3D-tolerance used for check ON-status.",
                  __FILE__, b2dclassify , g);
  theCommands.Add("b2dclassifx"  , "use b2dclassifx Face Point2d "
                  "[-t Tol3D = <FaceToler>] [-na] :\n "
                  "Enter the command w/o arguments to obtain help.",
                  __FILE__, b2dclassifx , g);
  theCommands.Add("bhaspc"       , "use bhaspc Edge Face [do]",
                  __FILE__, bhaspc      , g);

  theCommands.Add("ishole", "Use: ishole face [-t toler3D = <FaceToler>]: "
                      "Checks if the face is hole", __FILE__, IsHole, g);

  theCommands.Add("polyclassify", "Use: polyclassify [-s] -c px py -t tolU tolV "
                  "-p px1 py1 -p px2 py2 ...:\n"
                   "Classifies the point relatively to the polygon. "
                   "Enter the command w/o arguments to obtain detail help.",
                   __FILE__, polyclassify, g );
}


//
//=======================================================================
//function : b2dclassifx
//purpose  : 
//=======================================================================
Standard_Integer b2dclassifx (Draw_Interpretor& theDI,
                              Standard_Integer  theArgNb,
                              const char**      theArgVec)
{
  if (theArgNb < 3)  {
    theDI << "Use b2dclassifx Face Point2d [-t Tol3D = <FaceToler>] [-na]:\n";
    return 1;
  }

  TopoDS_Shape aS = DBRep::Get (theArgVec[1]);
  if (aS.IsNull())  {
    theDI << " Null Shape is not allowed here\n";
    return 1;
  }
  else if (aS.ShapeType() != TopAbs_FACE)  {
    theDI << " Shape type must be FACE\n";
    return 1;
  }
  
  gp_Pnt2d aP;
  //
  DrawTrSurf::GetPoint2d (theArgVec[2], aP);
  const TopoDS_Face&  aF   = TopoDS::Face(aS);
  
  Standard_Real aTol3D = BRep_Tool::Tolerance(aF);
  Standard_Boolean isReqToAdjust = Standard_True;

  for (Standard_Integer aCurrArg = 3; aCurrArg < theArgNb; aCurrArg++)
  {
    if (theArgVec[aCurrArg][0] != '-')
      continue;

    switch (theArgVec[aCurrArg][1])
    {
    case 't' :
      aTol3D = Draw::Atof(theArgVec[++aCurrArg]);
      break;
    case 'n' :
      if(!strncmp(theArgVec[aCurrArg], "-na", 3))
        isReqToAdjust = Standard_False;
      break;
    default:
      break;
    }
  }

  BRepTopAdaptor_FClass2d aClassifier(aF, aTol3D);
  const TopAbs_State aState = aClassifier.Perform(aP, isReqToAdjust);
  PrintState (theDI, aState);
  //
  return 0;
}
//
//=======================================================================
//function : b2dclassify
//purpose  : 
//=======================================================================
Standard_Integer b2dclassify (Draw_Interpretor& theDI,
                              Standard_Integer  theArgNb,
                              const char**      theArgVec)
{
  if (theArgNb < 3)  {
    theDI << " use b2dclassify Face Point2d [-t Tol3D = <FaceToler>]\n";
    return 1;
  }

  TopoDS_Shape aS = DBRep::Get (theArgVec[1]);
  if (aS.IsNull())  {
    theDI << " Null Shape is not allowed here\n";
    return 1;
  }
  else if (aS.ShapeType() != TopAbs_FACE)  {
    theDI << " Shape type must be FACE\n";
    return 1;
  }
  //
  gp_Pnt2d aP;
  //
  DrawTrSurf::GetPoint2d (theArgVec[2], aP);
  const TopoDS_Face&  aF   = TopoDS::Face(aS);
  Standard_Real aTol3D = BRep_Tool::Tolerance(aF);

  for (Standard_Integer aCurrArg = 3; aCurrArg < theArgNb; aCurrArg++)
  {
    if (theArgVec[aCurrArg][0] != '-')
      continue;

    switch (theArgVec[aCurrArg][1])
    {
    case 't':
      aTol3D = Draw::Atof(theArgVec[++aCurrArg]);
      break;
    default:
      break;
    }
  }

  BRepClass_FaceClassifier aClassifier;
  aClassifier.Perform(aF, aP, aTol3D);
  PrintState (theDI, aClassifier.State());
  //
  return 0;
}

//=======================================================================
//function : IsHole
//purpose  : 
//=======================================================================
Standard_Integer IsHole(Draw_Interpretor& theDI,
                        Standard_Integer  theArgNb,
                        const char**      theArgVec)
{
  if (theArgNb < 2)
  {
    theDI << "Use: ishole face [-t Tol3D = <FaceToler>]\n";
    return 1;
  }

  TopoDS_Face aF = TopoDS::Face(DBRep::Get(theArgVec[1]));

  Standard_Real aTol = BRep_Tool::Tolerance(aF);
  for (Standard_Integer i = 2; i < theArgNb; i++)
  {
    if ((theArgVec[i][0] == '-') && (theArgVec[i][1] == 't'))
    {
      aTol = Draw::Atof(theArgVec[++i]);
    }
  }

  BRepTopAdaptor_FClass2d aClassifier(aF, aTol);
  if (aClassifier.PerformInfinitePoint() == TopAbs_IN)
  {
    theDI << "The face is hole\n";
  }
  else
  {
    theDI << "The face is not hole\n";
  }

  return 0;
}


//=======================================================================
//function : bclassify
//purpose  : 
//=======================================================================
Standard_Integer bclassify (Draw_Interpretor& theDI,
                            Standard_Integer  theArgNb,
                            const char**      theArgVec)
{
  if (theArgNb < 3)  {
    theDI << " use bclassify Solid Point [Tolerance=1.e-7]\n";
    return 1;
  }

  TopoDS_Shape aS = DBRep::Get (theArgVec[1]);
  if (aS.IsNull())  {
    theDI << " Null Shape is not allowed\n";
    return 1;
  }
  else if (aS.ShapeType() != TopAbs_SOLID)  {
    theDI << " Shape type must be SOLID\n";
    return 1;
  }

  gp_Pnt aP (8., 9., 10.);
  DrawTrSurf::GetPoint (theArgVec[2], aP);
  const Standard_Real aTol = (theArgNb == 4) ? 
    Draw::Atof (theArgVec[3]) : 1.e-7;

  BRepClass3d_SolidClassifier aSC (aS);
  aSC.Perform (aP,aTol);

  PrintState (theDI, aSC.State());
  return 0;
}

//=======================================================================
//function : polyclassify
//purpose  : 
//=======================================================================
Standard_Integer polyclassify(Draw_Interpretor& theDI,
                              Standard_Integer  theArgNb,
                              const char**      theArgVec)
{
  if(theArgNb < 16)
  {
    theDI << "Use: polyclassify [-s] -c px py -t tolU tolV -p px1 py1 -p px2 py2 ...\n"
    "Creates a 2D-polygon with vertices set by the \"-p\" options. "
    "Classifies the point (px, py) relatively to this polygon using UV-tolerances "
    "tolU and tolV.\n"
    "Option \"-s\" allows showing this polygon in DRAW-viewer. For that:\n"
    "  1. Create a plane strictly as \"plane pl 0 0 0 0 0 1\";\n"
    "  2. Create face with surface pl and wire wirsrc;\n"
    "  3. Look at the p-curve of this face. The classified point "
    "will be stored in variable pntsrc.\n";
    return 1;
  }

  Standard_Boolean hasToShow = Standard_False;
  gp_Pnt2d aClassifPnt;
  Standard_Real aTolU = 0.0, aTolV = 0.0;

  Standard_Real aUmin = RealLast(), aUmax = RealFirst();
  Standard_Real aVmin = RealLast(), aVmax = RealFirst();

  NCollection_Sequence<gp_Pnt2d> aPntVec;

  for(Standard_Integer i = 1; i < theArgNb; i++)
  {
    if(theArgVec[i][0] != '-')
      continue;

    switch(theArgVec[i][1])
    {
      case 's':
        hasToShow = Standard_True;
        break;

      case 'c':
      {
        const Standard_Real aX = Draw::Atof(theArgVec[++i]);
        const Standard_Real aY = Draw::Atof(theArgVec[++i]);
        aClassifPnt.SetCoord(aX, aY);
        break;
      }

      case 't':
      {
        aTolU = Draw::Atof(theArgVec[++i]);
        aTolV = Draw::Atof(theArgVec[++i]);
        break;
      }

      case 'p':
      {
        const Standard_Real aX = Draw::Atof(theArgVec[++i]);
        const Standard_Real aY = Draw::Atof(theArgVec[++i]);
        aUmin = Min(aUmin, aX);
        aUmax = Max(aUmax, aX);
        aVmin = Min(aVmin, aY);
        aVmax = Max(aVmax, aY);

        aPntVec.Append(gp_Pnt2d(aX, aY));
        break;
      }
    }
  }


  BRepBuilderAPI_MakePolygon aW;

  TColgp_Array1OfPnt2d anArrPnts(1, aPntVec.Length());
  for(Standard_Integer i = aPntVec.Lower(), j = anArrPnts.Lower(); i <= aPntVec.Upper(); i++, j++)
  {
    anArrPnts(j) = aPntVec(i);

    if(!hasToShow)
      continue;

    aW.Add(gp_Pnt(aPntVec(i).X(), aPntVec(i).Y(), 0.0));
  }

  if(hasToShow)
  {
    aW.Close();
    DBRep::Set("wirsrc", aW);
    theDI << "Wire \"wirsrc\" has been created.\n";
    DrawTrSurf::Set("pntsrc", aClassifPnt);
    theDI << "Point \"pntsrc\" has been created.\n";
  }

  CSLib_Class2d aPClass(anArrPnts, aTolU, aTolV, aUmin, aVmin, aUmax, aVmax);
  CSLib_Class2d::PolyState aState = aPClass.Classify(aClassifPnt);

  theDI << "Point pntsrc is ";
  switch(aState)
  {
    case CSLib_Class2d::PolyOut:
      theDI << "OUT\n";
      break;
    case CSLib_Class2d::PolyOn:
      theDI << "ON\n";
      break;
    case CSLib_Class2d::PolyIn:
      theDI << "IN\n";
      break;
  }

  return 0;
}

//=======================================================================
//function : bhaspc
//purpose  : 
//=======================================================================
Standard_Integer bhaspc (Draw_Interpretor& di, 
                         Standard_Integer n, 
                         const char** a)
{
  if (n<3) {
    di << " use bhaspc Edge Face [do]\n";
    return 1;
  }

  TopoDS_Shape S1 = DBRep::Get(a[1]);
  TopoDS_Shape S2 = DBRep::Get(a[2]);
  
  if (S1.IsNull() || S2.IsNull()) {
    di << " Null shapes are not allowed \n";
    return 1;
  }
  if (S1.ShapeType()!=TopAbs_EDGE || S2.ShapeType()!=TopAbs_FACE) {
    di << " Type mismatch\n";
    return 1;
  }
  //
  const TopoDS_Edge& aE=TopoDS::Edge(S1);
  const TopoDS_Face& aF=TopoDS::Face(S2);
  Standard_Real f2D, l2D; 

  Handle(Geom2d_Curve) C2D=CurveOnSurface(aE, aF, f2D, l2D);
  
  if (C2D.IsNull()) {
    di << " No 2D Curves detected\n";
  }
  else {
    di << " Ok Edge has P-Curve on this Face\n";
  }
  
  if (n==4) {
    if (!strcmp(a[3], "do")) {
      BOPTools_AlgoTools2D::BuildPCurveForEdgeOnFace(aE, aF);  
    }
  }

  return 0;
}
//=======================================================================
//function : PrintState
//purpose  :
//=======================================================================
void PrintState (Draw_Interpretor&   theDI,
                 const TopAbs_State& theState)
{
  switch (theState) {
   case TopAbs_IN:       
    theDI << "The point is IN shape\n";      
    break;
   case TopAbs_OUT:  
    theDI << "The point is OUT of shape\n";  
    break;
   case TopAbs_ON: 
    theDI << "The point is ON shape\n";      
    break;
   case TopAbs_UNKNOWN:
   default:              
    theDI << "The point is UNKNOWN shape\n"; 
    break;
  }
}
//=======================================================================
//function : CurveOnSurface
//purpose  : 
//=======================================================================
Handle(Geom2d_Curve) CurveOnSurface(const TopoDS_Edge& E, 
                                    const TopoDS_Face& F,
                                    Standard_Real& First,
                                    Standard_Real& Last)
{
  TopLoc_Location l;
  const Handle(Geom_Surface)& S = BRep_Tool::Surface(F,l);
  TopoDS_Edge aLocalEdge = E;
  if (F.Orientation() == TopAbs_REVERSED) {
    aLocalEdge.Reverse();
  }
  return CurveOnSurface(aLocalEdge,S,l,First,Last);
}

static Handle(Geom2d_Curve) nullPCurve;
//=======================================================================
//function : CurveOnSurface
//purpose  : 
//=======================================================================
Handle(Geom2d_Curve) CurveOnSurface(const TopoDS_Edge& E, 
                                    const Handle(Geom_Surface)& S,
                                    const TopLoc_Location& L,
                                    Standard_Real& First,
                                    Standard_Real& Last)
{
  TopLoc_Location l = L.Predivided(E.Location());
  Standard_Boolean Eisreversed = (E.Orientation() == TopAbs_REVERSED);

  // find the representation
  BRep_ListIteratorOfListOfCurveRepresentation itcr
    ((*((Handle(BRep_TEdge)*)&E.TShape()))->ChangeCurves());

  while (itcr.More()) {
    const Handle(BRep_CurveRepresentation)& cr = itcr.Value();
    if (cr->IsCurveOnSurface(S,l)) {
      Handle(BRep_GCurve) GC (Handle(BRep_GCurve)::DownCast (cr));
      GC->Range(First,Last);
      if (GC->IsCurveOnClosedSurface() && Eisreversed)
        return GC->PCurve2();
      else
        return GC->PCurve();
    }
    itcr.Next();
  }
  return nullPCurve;
}
