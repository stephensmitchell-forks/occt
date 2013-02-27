// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

#include <BRepLib_ToleranceRule.hxx>

#include <Bnd_Box.hxx>
#include <BRep_CurveRepresentation.hxx>
#include <BRep_ListIteratorOfListOfCurveRepresentation.hxx>
#include <BRepBndLib.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Surface.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom2d_Curve.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <gp_Pnt.hxx>
#include <BRep_Tool.hxx>
#include <Precision.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Vertex.hxx>

//=======================================================================
//function : ProperTolerances
//purpose  :
//=======================================================================
void BRepLib_ToleranceRule::ProperTolerances(const TopoDS_Shape & theS,
  NCollection_DataMap<TopoDS_Shape, Standard_Real,
    TopTools_ShapeMapHasher> & theTs,
  Standard_Boolean theMinimizeTolerances)
{
  // Process faces.
  for (TopExp_Explorer aFIt(theS, TopAbs_FACE); aFIt.More(); aFIt.Next())
  {
    const TopoDS_Face & aF = TopoDS::Face(aFIt.Current());
    Standard_Real aFT = Precision::Confusion() * 4;
    if (!theMinimizeTolerances)
    {
      aFT = BRep_Tool::Tolerance(aF);
    }
    else
    {
      TopLoc_Location aL;
      Handle_Geom_Surface aS = BRep_Tool::Surface(aF, aL);
      if (!aS.IsNull())
      {
        if (theTs.IsBound(aF))
        {
          continue;
        }
        if (aS->DynamicType() == STANDARD_TYPE(Geom_RectangularTrimmedSurface))
        {
          Handle_Geom_RectangularTrimmedSurface aTS =
            Handle_Geom_RectangularTrimmedSurface::DownCast(aS);
          aS = aTS->BasisSurface();
        }
        //
        switch (GeomAdaptor_Surface(aS).GetType())
        {
          case GeomAbs_Plane:
          case GeomAbs_Cylinder:
          case GeomAbs_Cone:
          {
            aFT = Precision::Confusion();
            break;
          }
          case GeomAbs_Sphere:
          case GeomAbs_Torus:
          {
            aFT = Precision::Confusion() * 2;
          }
        }
        //
        Bnd_Box aBox;
        BRepBndLib::Add(aF, aBox);
        if (!aBox.IsWhole())
        {
          Standard_Real aXmin, aYmin, aZmin, aXmax, aYmax, aZmax;
          aBox.Get(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);
          Standard_Real dMax = 1;
          if (!aBox.IsOpenXmin() && !aBox.IsOpenXmax())
          {
            dMax  = aXmax - aXmin;
          }
          if (!aBox.IsOpenYmin() && !aBox.IsOpenYmax())
          {
            aYmin = aYmax - aYmin;
          }
          if (!aBox.IsOpenZmin() && !aBox.IsOpenZmax())
          {
            aZmin = aZmax - aZmin;
          }
          if (dMax < aYmin)
          {
            dMax = aYmin;
          }
          if (dMax < aZmin)
          {
            dMax = aZmin;
          }
          aFT = aFT * dMax;
          // Do not process tolerances > 1.
          if (1 < aFT) aFT = 0.99;
        }
      }
      theTs.Bind(aF, aFT);
    }
    // Process face edges.
    for (TopExp_Explorer anEIt(aF, TopAbs_EDGE); anEIt.More(); anEIt.Next())
    {
      const TopoDS_Edge & anE = TopoDS::Edge(anEIt.Current());
      Standard_Real anET = Max(aFT, BRep_Tool::Tolerance(anE));
      if (!theTs.IsBound(anE))
      {
        theTs.Bind(anE, anET);
      }
      else
      {
        Standard_Real & aT = theTs(anE);
        anET = Max(anET, aT);
        aT = anET;
      }
      // Process face edge vertices.
      for (TopExp_Explorer aVIt(anE, TopAbs_VERTEX); aVIt.More(); aVIt.Next())
      {
        const TopoDS_Vertex & aV = TopoDS::Vertex(aVIt.Current());
        Standard_Real aVT = anET;
        if (!theMinimizeTolerances)
        {
          aVT = Max(anET, BRep_Tool::Tolerance(aV));
        }
        //
        if (BRep_Tool::SameRange(anE))
        {
          Bnd_Box aBox;
          aBox.Add(BRep_Tool::Pnt(aV));
          Standard_Real aPar = BRep_Tool::Parameter(aV, anE);
          Handle_BRep_TEdge & anED = (Handle_BRep_TEdge &)anE.TShape();
          const TopLoc_Location & anEL = anE.Location();
          for (BRep_ListIteratorOfListOfCurveRepresentation
            aRIt(anED->Curves()); aRIt.More(); aRIt.Next())
          {
	          // For each CurveRepresentation, check the provided parameter
	          const Handle_BRep_CurveRepresentation & aR = aRIt.Value();
	          const TopLoc_Location & aRL = aR->Location();
	          TopLoc_Location aL = anEL * aRL;
	          if (aR->IsCurve3D())
	          {
	            const Handle_Geom_Curve & aC = aR->Curve3D();
	            if (!aC.IsNull())
	            {
	              // edge non degenerated
	              gp_Pnt aPnt = aC->Value(aPar);
	              aPnt.Transform(aL.Transformation());
	              aBox.Add(aPnt);
	            }
	          }
	          else if (aR->IsCurveOnSurface())
	          {
	            const Handle_Geom_Surface & aS = aR->Surface();
	            const Handle_Geom2d_Curve & aC = aR->PCurve();
	            const Handle_Geom2d_Curve * aC2 = 0;
	            if (aR->IsCurveOnClosedSurface())
	            {
	              aC2 = &(aR->PCurve2());
	            }
	            gp_Pnt2d a2DP = aC->Value(aPar);
	            gp_Pnt a3DP = aS->Value(a2DP.X(), a2DP.Y());
	            a3DP.Transform(aL.Transformation());
	            aBox.Add(a3DP);
	            if (aC2 != 0)
	            {
	              a2DP = (*aC2)->Value(aPar);
	              a3DP = aS->Value(a2DP.X(), a2DP.Y());
	              a3DP.Transform(aL.Transformation());
	              aBox.Add(a3DP);
	            }
	          }
          }
          //
          Standard_Real aXmin, aYmin, aZmin, aXmax, aYmax, aZmax;
          aBox.Get(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);
          aXmax -= aXmin; aYmax -= aYmin; aZmax -= aZmin;
          Standard_Real aT =
            sqrt(aXmax * aXmax + aYmax * aYmax + aZmax * aZmax);
          aT += 2 * Epsilon(aT);
          aVT = Max(aVT, aT);
        }
        //
        if (!theTs.IsBound(aV))
        {
          theTs.Bind(aV, aVT);
        }
        else
        {
          Standard_Real & aT = theTs(aV);
          aT = Max(aVT, aT);
        }
      }
    }
    // Process face vertices.
    for (TopExp_Explorer aVIt(aF, TopAbs_VERTEX, TopAbs_EDGE);
      aVIt.More(); aVIt.Next())
    {
      const TopoDS_Vertex & aV = TopoDS::Vertex(aVIt.Current());
      Standard_Real aVT = Max(aFT, BRep_Tool::Tolerance(aV));
      if (!theTs.IsBound(aV))
      {
        theTs.Bind(aV, aVT);
      }
      else
      {
        Standard_Real & aT = theTs(aV);
        aT = Max(aVT, aT);
      }
    }
  }
  // Process edges without faces.
  for (TopExp_Explorer anEIt(theS, TopAbs_EDGE, TopAbs_FACE);
    anEIt.More(); anEIt.Next())
  {
    const TopoDS_Edge & anE = TopoDS::Edge(anEIt.Current());
    Standard_Real anET = BRep_Tool::Tolerance(anE);
    // Process edge vertices.
    for (TopExp_Explorer aVIt(anE, TopAbs_VERTEX); aVIt.More(); aVIt.Next())
    {
      const TopoDS_Vertex & aV = TopoDS::Vertex(aVIt.Current());
      Standard_Real aVT = Max(anET, BRep_Tool::Tolerance(aV));
      if (!theTs.IsBound(aV))
      {
        theTs.Bind(aV, aVT);
      }
      else
      {
        Standard_Real & aT = theTs(aV);
        aT = Max(aVT, aT);
      }
    }
  }
}
