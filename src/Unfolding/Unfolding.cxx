// File:	Unfolding.cxx
// Created:	Wed Sep 17 09:16:05 2008
// Author:	Sergey KHROMOV
//		<skv@kurox>


#include <Unfolding.hxx>
#include <BRepBuilderAPI_Sewing.hxx>
#include <BRep_Tool.hxx>
#include <GCPnts_AbscissaPoint.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <Geom2d_BezierCurve.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>


//=======================================================================
//function : ToShell
//purpose  : 
//=======================================================================

TopoDS_Shell Unfolding::ToShell(const TopoDS_Shape          &theShape,
				const Standard_Real          theTolerance,
				      Unfolding_ErrorStatus &theStatus)
{
  TopoDS_Shell     aShell;
  TopAbs_ShapeEnum aType = theShape.ShapeType();

  if (aType != TopAbs_COMPOUND && aType != TopAbs_COMPSOLID &&
      aType != TopAbs_SOLID    && aType != TopAbs_SHELL) {
    theStatus = Unfolding_InvalidInput;
    return aShell;
  }

  if (aType == TopAbs_SHELL) {
    // theShape is a shell. Nothing to be done.
    theStatus = Unfolding_Done;
    aShell    = TopoDS::Shell(theShape);
    return aShell;
  }

  BRepBuilderAPI_Sewing aSewing(theTolerance);

  aSewing.Add(theShape);
  aSewing.Perform();

  const TopoDS_Shape &aShape = aSewing.SewedShape();

  if (aShape.IsNull()) {
    theStatus = Unfolding_Failure;
    return aShell;
  }

  // Explore shape to extract a shell.
  TopExp_Explorer anExp(aShape, TopAbs_SHELL);

  if (!anExp.More()) {
    theStatus = Unfolding_InvalidShape;
    return aShell;
  }

  TopoDS_Shape aSingleShell = anExp.Current();

  anExp.Next();

  if (anExp.More()) {
    theStatus = Unfolding_ComplexShape;
    return aShell;
  }

  // Check if there is no other redundant shapes that are not in the shell.
  anExp.Init(aShape, TopAbs_VERTEX, TopAbs_SHELL);

  if (anExp.More()) {
    theStatus = Unfolding_ComplexShape;
    return aShell;
  }

  // Get the single shell.
  theStatus = Unfolding_Done;
  aShell    = TopoDS::Shell(aSingleShell);

  return aShell;
}

//=======================================================================
//function : ToShell
//purpose  : 
//=======================================================================

Standard_Integer Unfolding::NbSamples(const TopoDS_Edge          &theEdge,
				      const TopTools_ListOfShape &theFaces,
				      const Standard_Real         theTolerance)
{
  Standard_Integer aNbPointsLimit = Unfolding::GetMaxNbSamples() / 2;
  Standard_Integer aMaxNbPoints = 11;
  Standard_Integer aNbPoints;
  TopTools_ListIteratorOfListOfShape anIt( theFaces );

  for ( ; anIt.More(); anIt.Next() ) {
    TopoDS_Shape aCurShape = anIt.Value();
    TopoDS_Face aFace = TopoDS::Face( aCurShape );
    aFace.Orientation(TopAbs_FORWARD);

    Standard_Integer aNbPnt = 2;
    Standard_Real         aParam[2];
    Handle(Geom2d_Curve)  aCurve = 
      BRep_Tool::CurveOnSurface(theEdge, aFace, aParam[0], aParam[1]);
    Handle(Standard_Type) aType  = aCurve->DynamicType();

    while (aType == STANDARD_TYPE(Geom2d_TrimmedCurve)) {
      Handle(Geom2d_TrimmedCurve) aTrCurve =
	Handle(Geom2d_TrimmedCurve)::DownCast(aCurve);
      aCurve = aTrCurve->BasisCurve();
      aType  = aCurve->DynamicType();
    }
    
    if        (aType == STANDARD_TYPE(Geom2d_BezierCurve)) {
      Handle(Geom2d_BezierCurve) aBezier =
	Handle(Geom2d_BezierCurve)::DownCast(aCurve);
      aNbPnt = 3 + aBezier->NbPoles();


      if (aNbPnt < 11)
	aNbPnt = 11;
    } else if (aType == STANDARD_TYPE(Geom2d_BSplineCurve)) {
      Handle(Geom2d_BSplineCurve) aBSpline =
	Handle(Geom2d_BSplineCurve)::DownCast(aCurve);
      aNbPnt  = aBSpline->NbKnots()*aBSpline->Degree();
      
      if (aNbPnt < 11)
	aNbPnt = 11;
    } else {
      aNbPnt = 11;
    }
    aMaxNbPoints = ( aMaxNbPoints < aNbPnt ) ? aNbPnt : aMaxNbPoints;

    Geom2dAdaptor_Curve aCurve1(aCurve);
    Standard_Real L = GCPnts_AbscissaPoint::Length(aCurve1, aParam[0], aParam[1]) ;
    // estimate number of points taking into account the curvature of radius myTolContour * 2.
    Standard_Real sizeR = 0.125 * L / theTolerance + 2; 

    if ( aNbPointsLimit > sizeR )
      aNbPointsLimit = (Standard_Integer) sizeR;
  }

  // Check if the number of points exceeds the maximum number of points.

  if(aMaxNbPoints > aNbPointsLimit)
    aMaxNbPoints = aNbPointsLimit;

  aNbPoints = aMaxNbPoints;

  return aNbPoints;
}
