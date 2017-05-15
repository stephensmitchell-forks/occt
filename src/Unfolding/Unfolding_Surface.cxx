// File:	Unfolding_Surface.cxx
// Created:	Tue Jul 22 13:12:24 2008
// Author:	Sergey KHROMOV
//		<skv@dimox>


#include <Unfolding_Surface.hxx>
#include <Unfolding.hxx>
#include <Unfolding_HArray2OfPoint.hxx>
#include <Unfolding_Point.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepTools.hxx>
#include <BRepTools_WireExplorer.hxx>
#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <ElCLib.hxx>
#include <ElSLib.hxx>
#include <GccAna_Circ2d3Tan.hxx>
#include <Geom_BezierSurface.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_Circle.hxx>
#include <Geom_ConicalSurface.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_Line.hxx>
#include <Geom_Plane.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_SphericalSurface.hxx>
#include <Geom_Surface.hxx>
#include <Geom_SurfaceOfLinearExtrusion.hxx>
#include <Geom_SurfaceOfRevolution.hxx>
#include <Geom_ToroidalSurface.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <Geom2d_Curve.hxx>
#include <gp_Circ.hxx>
#include <IntAna2d_AnaIntersection.hxx>
#include <TColgp_Array2OfPnt.hxx>
#include <TColgp_SequenceOfXY.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Wire.hxx>
#include <TopTools_DataMapOfShapeInteger.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopTools_ListOfShape.hxx>


static Standard_Integer aLimitNumber = 1000;

static void AdjustToDeflection(      Standard_Integer     &theNbPointsU, 
			             Standard_Integer     &theNbPointsV,
			       const Handle(Geom_Surface) &theSurface, 
			       const Standard_Real         theUMin,
			       const Standard_Real         theUMax,
			       const Standard_Real         theVMin,
			       const Standard_Real         theVMax,
			       const Standard_Real         theDeflection);

//=======================================================================
//function : MakeCirc
//purpose  : This method constructs a circle that passes through 3 points.
//           theNorm is a normal to the plane that is formed by theP1, theP2
//           and theP3. Three points should not be on the same line and should
//           not coincide. All these conditions are not checked. Their respect
//           is under responsibility of the caller.
//=======================================================================

static Standard_Boolean MakeCirc(const gp_Pnt  &theP1,
				 const gp_Pnt  &theP2,
				 const gp_Pnt  &theP3,
				 const gp_Dir  &theNorm,
				       gp_Circ &theCircle)
{
  gp_Dir        aX(theP3.XYZ().Subtracted(theP1.XYZ()));
  gp_Ax3        aPlnAxis(theP1, theNorm, aX);
  Standard_Real aU;
  Standard_Real aV;
  gp_Pnt2d      aP1(0., 0.);

  ElSLib::PlaneParameters(aPlnAxis, theP2, aU, aV);

  gp_Pnt2d aP2(aU, aV);

  ElSLib::PlaneParameters(aPlnAxis, theP3, aU, aV);

  gp_Pnt2d aP3(aU, aV);

  GccAna_Circ2d3Tan aCircTan(aP1, aP2, aP3, Precision::Confusion());

  if (!aCircTan.IsDone() || aCircTan.NbSolutions() != 1)
    return Standard_False;

  gp_Circ2d aCirc2d = aCircTan.ThisSolution(1);

  // Convert 2d circle to 3d.
  const gp_Pnt2d &a2dLoc  = aCirc2d.Location();
  gp_Pnt          a3dLoc  = ElSLib::PlaneValue(a2dLoc.X(), a2dLoc.Y(), aPlnAxis);
  gp_Dir          a3dX(theP1.XYZ().Subtracted(a3dLoc.XYZ()));
  gp_Ax2          aCircAxis(a3dLoc, theNorm, a3dX);
  Standard_Real   aRadius = aCirc2d.Radius();

  theCircle.SetPosition(aCircAxis);
  theCircle.SetRadius(aRadius);

  return Standard_True;
}

//=======================================================================
//function : CrvDistance
//purpose  : Returns the approximate curvelinear distance between two
//           points on surface. The distance is approximated by length of
//           the circular arc started at thePoint1 and ended at thePoint2
//           that is passed through the middle in UV space point.
//=======================================================================

static Standard_Real CrvDistance(const Unfolding_Point      &thePoint1,
				 const Unfolding_Point      &thePoint2,
				 const Handle(Geom_Surface) &theSurface,
				 const Standard_Real        &theTolerance)
{
  const gp_XYZ  &aP1      = thePoint1.GetPointOnSurface();
  const gp_XYZ  &aP2      = thePoint2.GetPointOnSurface();
  gp_XYZ         aP12     = aP2.Subtracted(aP1);
  Standard_Real  aLinDist = aP12.Modulus();

  if (aLinDist <= theTolerance)
    return aLinDist;

  // Get middle point.
  Standard_Real aUMid;
  Standard_Real aVMid;
  Standard_Real aU;
  Standard_Real aV;
  gp_Pnt        aPntMid;

  thePoint1.GetParameters(aUMid, aVMid);
  thePoint2.GetParameters(aU, aV);

  aUMid = 0.5*(aUMid + aU);
  aVMid = 0.5*(aVMid + aV);

  theSurface->D0(aUMid, aVMid, aPntMid);

  const gp_XYZ &aPMid = aPntMid.XYZ();

  // Compute distance from aPMid to aP12.
  aP12.Divide(aLinDist);

  gp_XYZ aDistCoord(aPMid.Subtracted(aP1));

  aDistCoord.Cross(aP12);

  Standard_Real aDist = aDistCoord.Modulus();

  if (aDist < 0.01*theTolerance)
    return aLinDist;

  // Find a distance along an arc.
  gp_Pnt  aPnt1(aP1);
  gp_Pnt  aPnt2(aP2);
  gp_Dir  aNorm(aDistCoord);
  gp_Circ aCirc;

  if (!MakeCirc(aPnt1, aPntMid, aPnt2, aNorm, aCirc))
    return 0.;

  const gp_Ax2  &anAxis      = aCirc.Position();
  Standard_Real  anAngle     = ElCLib::CircleParameter(anAxis, aPnt2);
  Standard_Real  aCircParMid = ElCLib::CircleParameter(anAxis, aPntMid);
  Standard_Real  aRadius     = aCirc.Radius();

  if (anAngle < aCircParMid)
    anAngle = 2.*M_PI - anAngle;

  Standard_Real aCrvDist = anAngle*aRadius;

  return aCrvDist;
}

//=======================================================================
//function : GetCell
//purpose  : Returns the indices of a cell thePoint belongs to. The cell (i, j)
//           is defined between grid's points (i, j) - (i + 1, j) -
//           (i + 1, j + 1) - (i, j + 1). If the point is outside the grid,
//           its index is equal to 0 in case if corresponding index is lower
//           then the lower bound or NbPoints if it is greater then the upper
//           bound.
//=======================================================================

static void GetCell(const gp_XY                            &thePoint,
		    const Handle(Unfolding_HArray2OfPoint) &theGrid,
		    const Standard_Real                    &theTolerance,
		          Standard_Integer                 &theI,
		          Standard_Integer                 &theJ)
{
  Standard_Integer       iStart = theGrid->LowerRow();
  Standard_Integer       jStart = theGrid->LowerCol();
  Standard_Integer       iEnd   = theGrid->UpperRow();
  Standard_Integer       jEnd   = theGrid->UpperCol();
  const Unfolding_Point &aPff   = theGrid->Value(iStart, jStart);
  const Unfolding_Point &aPll   = theGrid->Value(iEnd,   jEnd);
  Standard_Real          aU     = thePoint.X();

  // Compute theI.
  if (aU < aPff.GetU() - theTolerance)
    theI = iStart - 1;
  else if (aU >= aPll.GetU() - theTolerance)
    theI = iEnd;
  else {
    Standard_Real aStep = (aPll.GetU() - aPff.GetU())/(iEnd - iStart + 1);

    theI = Standard_Integer(Round((aU - aPff.GetU())/aStep)) + iStart;

    if (theI < iStart)
      theI = iStart;

    if (theI > iEnd)
      theI = iEnd;

    while (theI >= iStart &&
	   aU < theGrid->Value(theI, jStart).GetU() - theTolerance)
      theI--;

    while (theI < iEnd &&
	   aU >= theGrid->Value(theI + 1, jStart).GetU() - theTolerance)
      theI++;
  }

  // Compute theI.
  Standard_Real aV = thePoint.Y();

  if (aV < aPff.GetV() - theTolerance)
    theJ = jStart - 1;
  else if (aV >= aPll.GetV() - theTolerance)
    theJ = jEnd;
  else {
    Standard_Real aStep = (aPll.GetV() - aPff.GetV())/(jEnd - jStart + 1);

    theJ = Standard_Integer(Round((aV - aPff.GetV())/aStep)) + jStart;

    if (theJ < jStart)
      theJ = jStart;

    if (theJ > jEnd)
      theJ = jEnd;

    while (theJ >= jStart &&
	   aV < theGrid->Value(iStart, theJ).GetV() - theTolerance)
      theJ--;

    while (theJ < jEnd &&
	   aV >= theGrid->Value(iStart, theJ + 1).GetV() - theTolerance)
      theJ++;
  }
}

//=======================================================================
//function : Empty constructor
//purpose  : This method constructs and returns the curve that is passed
//           through three points. It can be either a line if these three
//           points lie on the same line or a circle. In case of failure
//           it returns a null handle.
//=======================================================================

static Handle(Geom_Curve) MakeCurve(const gp_Pnt        &thePFirst,
				    const gp_Pnt        &thePMid,
				    const gp_Pnt        &thePLast,
				    const gp_Pln        &thePlane,
				    const Standard_Real  theTolerance)
{
  Handle(Geom_Curve) aCurve;
  gp_Vec             aDFL(thePLast.XYZ().Subtracted(thePFirst.XYZ()));
  Standard_Real      aLinDist = aDFL.Magnitude();

  if (aLinDist > theTolerance) {
    // compute distance between thePMid and the segment [thePFirst, thePLast].
    gp_XYZ aDPMidFirst = thePMid.XYZ();

    aDFL.Divide(aLinDist);
    aDPMidFirst.Subtract(thePFirst.XYZ());
    aDPMidFirst.Cross(aDFL.XYZ());

    Standard_Real aDist = aDPMidFirst.Modulus();

    // Line criteria like in the CrvDistance function.
    if (aDist < 0.01*theTolerance) {
      aCurve = new Geom_Line(thePFirst, gp_Dir(aDFL));
    } else {
      // Compute a circle
      gp_Circ aCirc;

      if (!MakeCirc(thePFirst, thePMid, thePLast,
		    thePlane.Position().Direction(), aCirc))
	    return aCurve;

      aCurve = new Geom_Circle(aCirc);

      // Trim the curve by the circle parameters.
      const gp_Ax2  &anAxis      = aCirc.Position();
      Standard_Real  aCircPar1   = 0.; // By construction
      Standard_Real  aCircPar2   = ElCLib::CircleParameter(anAxis, thePLast);
      Standard_Real  aCircParMid = ElCLib::CircleParameter(anAxis, thePMid);

      if (aCircParMid > aCircPar2)
	aCurve = new Geom_TrimmedCurve(aCurve, aCircPar1,
				       aCircPar2, Standard_False);
      else
	aCurve = new Geom_TrimmedCurve(aCurve, aCircPar1, aCircPar2);
    }
  }

  return aCurve;
}

//=======================================================================
// function : Analyse
// purpose  : Computes the numbers of samples along U and V direction using
//            the array of poles.
//=======================================================================

static void Analyse(const TColgp_Array2OfPnt &array2,
		          Standard_Integer   &theNbSamplesU,
		          Standard_Integer   &theNbSamplesV)
{
  gp_Vec Vi,Vip1;
  Standard_Integer sh,nbch,i,j;
  const Standard_Integer nbup = array2.UpperRow() - array2.LowerRow() + 1;
  const Standard_Integer nbvp = array2.UpperCol() - array2.LowerCol() + 1;

  sh = 1;
  nbch = 0;
  if(nbvp>2) {

    for(i = array2.LowerRow(); i <= array2.UpperRow(); i++) {
      const gp_Pnt& A=array2.Value(i,1);
      const gp_Pnt& B=array2.Value(i,2);
      const gp_Pnt& C=array2.Value(i,3);
      Vi.SetCoord(C.X()-B.X()-B.X()+A.X(),
		  C.Y()-B.Y()-B.Y()+A.Y(),
		  C.Z()-B.Z()-B.Z()+A.Z());
      Standard_Integer locnbch=0;

      for(j = array2.LowerCol() + 2; j < array2.UpperCol();j++) {  //-- essai
	const gp_Pnt& Ax=array2.Value(i,j-1);
	const gp_Pnt& Bx=array2.Value(i,j);
	const gp_Pnt& Cx=array2.Value(i,j+1);
	Vip1.SetCoord(Cx.X()-Bx.X()-Bx.X()+Ax.X(),
		      Cx.Y()-Bx.Y()-Bx.Y()+Ax.Y(),
		      Cx.Z()-Bx.Z()-Bx.Z()+Ax.Z());
	Standard_Real pd = Vi.Dot(Vip1);
	Vi=Vip1;
	if(pd>1.0e-7 || pd<-1.0e-7) {
	  if(pd>0) {  if(sh==-1) {   sh=1; locnbch++; 	}  }
	  else { 	if(sh==1) {  sh=-1; locnbch++; 	}  }
	}
      }
      if(locnbch>nbch) {
	nbch=locnbch;
      }
    }
  }
  theNbSamplesV = nbch+5;


  nbch=0;
  if(nbup > 2) {
    for(j = array2.LowerCol(); j <= array2.UpperCol(); j++) {
      const gp_Pnt& A=array2.Value(array2.LowerRow(),     j);
      const gp_Pnt& B=array2.Value(array2.LowerRow() + 1, j);
      const gp_Pnt& C=array2.Value(array2.LowerRow() + 2, j);
      Vi.SetCoord(C.X()-B.X()-B.X()+A.X(),
		  C.Y()-B.Y()-B.Y()+A.Y(),
		  C.Z()-B.Z()-B.Z()+A.Z());
      Standard_Integer locnbch=0;

      for(i = array2.LowerRow() + 2; i < array2.UpperRow(); i++) {  //-- essai
	const gp_Pnt& Ax=array2.Value(i-1,j);
	const gp_Pnt& Bx=array2.Value(i,j);
	const gp_Pnt& Cx=array2.Value(i+1,j);
	Vip1.SetCoord(Cx.X()-Bx.X()-Bx.X()+Ax.X(),
		    Cx.Y()-Bx.Y()-Bx.Y()+Ax.Y(),
		    Cx.Z()-Bx.Z()-Bx.Z()+Ax.Z());
	Standard_Real pd = Vi.Dot(Vip1);
	Vi=Vip1;
	if(pd>1.0e-7 || pd<-1.0e-7) {
	  if(pd>0) {  if(sh==-1) {   sh=1; locnbch++; 	}  }
	  else { 	if(sh==1) {  sh=-1; locnbch++; 	}  }
	}
      }
      if(locnbch>nbch) nbch=locnbch;
    }
  }
  theNbSamplesU = nbch+5;
}

//=======================================================================
//function : Empty constructor
//purpose  : 
//=======================================================================

Unfolding_Surface::Unfolding_Surface()
     : myTolContour(0.001),
       myTolCurvature(0.001),
       myDeflection(0.001)
{
  myDataContainer = new Unfolding_FaceDataContainer;
  Reset();
}

//=======================================================================
//function : Constructor
//purpose  : 
//=======================================================================

Unfolding_Surface::Unfolding_Surface(const TopoDS_Face   &theFace,
				     const gp_Pln        &thePlane,
				     const Standard_Real  theContourTolerance,
				     const Standard_Real  theCurvatureTolerance,
				     const Standard_Real  theDeflection)
     : myPlane(thePlane),
       myTolContour(theContourTolerance),
       myTolCurvature(theCurvatureTolerance),
       myDeflection(theDeflection)
{
  myDataContainer = new Unfolding_FaceDataContainer;
  myDataContainer->SetFace(theFace);
  Reset();
}

//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================

Standard_Boolean Unfolding_Surface::Perform
                  (const TopTools_DataMapOfShapeInteger &theMapEdgeNbSamples)
{
  // Check if the operation is already performed and there is nothing to do.
  if (myErrorStatus != Unfolding_NotDone)
    return (myErrorStatus == Unfolding_Done);

  // Check input data validity.
  if (myDataContainer->GetFace().IsNull() || 
      ( myTolContour <= RealEpsilon() ) ||
      ( myTolCurvature <= RealEpsilon() ) ||
      ( myDeflection <= RealEpsilon() ) ) {
    myErrorStatus = Unfolding_InvalidInput;
    return Standard_False;
  }

  // Initialize the grid on the surface.
  Standard_Boolean isOK = InitGrid();

  if (isOK) {
    // Perform unfolding
    isOK = Unfolding();

    if (isOK) {
      // Construct a planar face.
      isOK = BuildPlanarFace(theMapEdgeNbSamples);
    }
  }

  return isOK;
}

//=======================================================================
//function : InitGrid
//purpose  : 
//=======================================================================

Standard_Boolean Unfolding_Surface::InitGrid()
{
  // Get Bounding box of the face.
  Standard_Real      aUMin;
  Standard_Real      aUMax;
  Standard_Real      aVMin;
  Standard_Real      aVMax;
  const TopoDS_Face &aFace = myDataContainer->GetFace();

  BRepTools::UVBounds(aFace, aUMin, aUMax, aVMin, aVMax);

  // Compute number of samples.
  Standard_Integer aNbPntsU;
  Standard_Integer aNbPntsV;

  NbPoints(aUMin, aUMax, aVMin, aVMax, aNbPntsU, aNbPntsV);


  // Construct a grid.
  myGrid = new Unfolding_HArray2OfPoint(1, aNbPntsU, 1, aNbPntsV);

  // Fill in the grid.
  Handle(Geom_Surface) aSurface = BRep_Tool::Surface(aFace);
  Standard_Real        aUStep   = (aUMax - aUMin)/(aNbPntsU - 1);
  Standard_Real        aVStep   = (aVMax - aVMin)/(aNbPntsV - 1);
  Standard_Real        aUPar;
  Standard_Real        aVPar;
  Standard_Integer     i;
  Standard_Integer     j;
  gp_Pnt               aPnt;
  gp_Vec               aD1U;
  gp_Vec               aD1V;
  gp_Vec               aD2U;
  gp_Vec               aD2V;
  gp_Vec               aD2UV;
  gp_Vec               aNorm;
  Standard_Real        aL;
  Standard_Real        aM;
  Standard_Real        aN;
  Standard_Real        aMaxCurvature = 0.;

  for (aUPar = aUMin, i = 1; i <= aNbPntsU; i++) {
    for (aVPar = aVMin, j = 1; j <= aNbPntsV; j++) {
      aSurface->D2(aUPar, aVPar, aPnt, aD1U, aD1V, aD2U, aD2V, aD2UV);

      // Check Gauss curvature
      aNorm = aD1U.Crossed(aD1V);
      aNorm.Normalize();
      aL    = aD2U.Dot(aNorm);
      aN    = aD2V.Dot(aNorm);
      aM    = aD2UV.Dot(aNorm);
      const Standard_Real aCurvature = Abs(aL*aN - aM*aM);
      aMaxCurvature = ( aMaxCurvature < aCurvature ) ? aCurvature : aMaxCurvature;

      if (aCurvature > myTolCurvature) {
	myDataContainer->SetMaxGaussCurvature(aMaxCurvature);
	myErrorStatus = Unfolding_InvalidSurface;
	return Standard_False;
      }

      // Initialize the point.
      Unfolding_Point &aPoint = myGrid->ChangeValue(i, j);

      aPoint.SetParameters(aUPar, aVPar);
      aPoint.SetPointOnSurface(aPnt.XYZ());

      aVPar = (j == aNbPntsV - 1) ? aVMax : aVPar + aVStep;
    }

    aUPar = (i == aNbPntsU - 1) ? aUMax : aUPar + aUStep;
  }

  myDataContainer->SetMaxGaussCurvature(aMaxCurvature);

  // Initialize points by angles in DU and DV directions.
  Standard_Real aTolConf = 0.01*Max(myTolContour,myDeflection);
  gp_XYZ        aDX;
  gp_XYZ        aDY;

  for (i = 1; i <= aNbPntsU; i++) {
    for (j = 1; j <= aNbPntsV; j++) {
      Unfolding_Point &aP = myGrid->ChangeValue(i, j);

      if (i == aNbPntsU || j == aNbPntsV) {
	aP.SetAngle(0.);
      } else {
	const Unfolding_Point &aPX = myGrid->Value(i + 1, j);

	aDX = aPX.GetPointOnSurface();
	aDX.Subtract(aP.GetPointOnSurface());

	if (aDX.Modulus() <= aTolConf) {
	  aP.SetAngle(0.);
	} else {
	  const Unfolding_Point &aPY = myGrid->Value(i, j + 1);

	  aDY = aPY.GetPointOnSurface();
	  aDY.Subtract(aP.GetPointOnSurface());

	  if (aDY.Modulus() <= aTolConf) {
	    aP.SetAngle(0.);
	  } else {
	    aP.SetAngle(gp_Vec(aDX).Angle(gp_Vec(aDY)));
	  }
	}
      }
    }
  }

  return Standard_True;
}

//=======================================================================
//function : NbPoints
//purpose  : 
//=======================================================================

void Unfolding_Surface::NbPoints(const Standard_Real     theUMin,
				 const Standard_Real     theUMax,
				 const Standard_Real     theVMin,
				 const Standard_Real     theVMax,
				       Standard_Integer &theNbPointsU,
				       Standard_Integer &theNbPointsV) const
{
  Handle(Geom_Surface)  aSurface =
                            BRep_Tool::Surface(myDataContainer->GetFace());
  Handle(Standard_Type) aType    = aSurface->DynamicType();

  while (aType == STANDARD_TYPE(Geom_RectangularTrimmedSurface)) {
    Handle(Geom_RectangularTrimmedSurface) aTrSurf =
          Handle(Geom_RectangularTrimmedSurface)::DownCast(aSurface);
    aSurface = aTrSurf->BasisSurface();
    aType    = aSurface->DynamicType();
  }

  if        (aType == STANDARD_TYPE(Geom_Plane)) {
    // PLANE
    theNbPointsU = 11;
    theNbPointsV = 11;
  } else if (aType == STANDARD_TYPE(Geom_CylindricalSurface)) {
    // CYLINDER
    Handle(Geom_CylindricalSurface) aCyl        =
          Handle(Geom_CylindricalSurface)::DownCast(aSurface);
    Standard_Real                   aRadius     = aCyl->Radius();
    Standard_Real                   aMaxAngle   = M_PI *0.5;

    if(aRadius > myDeflection) {
      aMaxAngle = ACos(1. - myDeflection/aRadius)*2.;
    }
    if(aMaxAngle > Precision::Angular())
      theNbPointsU = Standard_Integer((theUMax - theUMin)/aMaxAngle);
    else
      theNbPointsU = (Standard_Integer)(theUMax - theUMin);

    theNbPointsV  = (Standard_Integer)(theVMax - theVMin);
    theNbPointsV /= 10;

    if (theNbPointsU < 3)
      theNbPointsU = 3;

    if (theNbPointsV < 3)
      theNbPointsV = 3;
  } else if (aType == STANDARD_TYPE(Geom_ConicalSurface)) {
    // CONE
    Handle(Geom_ConicalSurface) aConeSurf =
          Handle(Geom_ConicalSurface)::DownCast(aSurface);
    gp_Cone                     aCone     = aConeSurf->Cone();
    gp_Circ                     aCircle   = ElSLib::ConeVIso(aCone.Position(),
							     aCone.RefRadius(),
							     aCone.SemiAngle(),
							     theVMin);
    Standard_Real               aRadius   = aCircle.Radius();
    aCircle = ElSLib::ConeVIso(aCone.Position(), aCone.RefRadius(),
			       aCone.SemiAngle(), theVMax);

    if(aRadius < aCircle.Radius())
      aRadius = aCircle.Radius();

    Standard_Real aMaxAngle   = M_PI*0.5;

    if(aRadius > myDeflection)
      aMaxAngle = ACos(1. - myDeflection/aRadius)*2.;

    if(aMaxAngle > Precision::Angular())
      theNbPointsU = Standard_Integer((theUMax - theUMin)/aMaxAngle);
    else
      theNbPointsU = (Standard_Integer)(theUMax - theUMin);

    theNbPointsV  = (Standard_Integer)(theVMax - theVMin);
    theNbPointsV /= 10;

    if (theNbPointsU < 11)
      theNbPointsU = 11;

    if (theNbPointsV < 11)
      theNbPointsV = 11;
  } else if (aType == STANDARD_TYPE(Geom_SphericalSurface) ||
	     aType == STANDARD_TYPE(Geom_ToroidalSurface)) {
    // SPHERE or TORUS
    gp_Circ       aCircle;
    Standard_Real aRadius1;
    Standard_Real aRadius2;

    if (aType == STANDARD_TYPE(Geom_SphericalSurface)) {
      // SPHERE
      Handle(Geom_SphericalSurface) aSphSurf =
	Handle(Geom_SphericalSurface)::DownCast(aSurface);
      gp_Sphere aSphere = aSphSurf->Sphere();

      aRadius1 = aSphere.Radius();
      aRadius2 = aSphere.Radius();
    } else {
      Handle(Geom_ToroidalSurface) aTorusSurf =
	Handle(Geom_ToroidalSurface)::DownCast(aSurface);
      // TORUS
      gp_Torus aTorus = aTorusSurf->Torus();

      aCircle  = ElSLib::TorusUIso(aTorus.Position(), aTorus.MajorRadius(),
				  aTorus.MinorRadius(), theUMin);
      aRadius2 = aCircle.Radius();
      aCircle  = ElSLib::TorusUIso(aTorus.Position(), aTorus.MajorRadius(),
				   aTorus.MinorRadius(), theUMax);
      aRadius2 = (aRadius2 < aCircle.Radius()) ? aCircle.Radius() : aRadius2;
      aCircle  = ElSLib::TorusVIso(aTorus.Position(), aTorus.MajorRadius(),
				   aTorus.MinorRadius(), theVMin);
      aRadius1 = aCircle.Radius();
      aCircle  = ElSLib::TorusVIso(aTorus.Position(), aTorus.MajorRadius(),
				   aTorus.MinorRadius(), theVMax);
      aRadius1 = (aRadius1 < aCircle.Radius()) ? aCircle.Radius() : aRadius1;
    }

    Standard_Real aMaxAngle = M_PI*0.5;

    if(aRadius1 > myDeflection)
      aMaxAngle = ACos(1. - myDeflection/aRadius1)*2.;

    if(aMaxAngle > Precision::Angular())
      theNbPointsU = Standard_Integer((theUMax - theUMin)/aMaxAngle);
    else
      theNbPointsU = (Standard_Integer)(theUMax - theUMin);

    aMaxAngle = M_PI*0.5;

    if(aRadius2 > myDeflection)
      aMaxAngle = ACos(1. - myDeflection/aRadius2)*2.;

    if(aMaxAngle > Precision::Angular())
      theNbPointsV = Standard_Integer((theVMax - theVMin)/aMaxAngle);
    else
      theNbPointsV = (Standard_Integer)(theVMax - theVMin);

    if (theNbPointsU < 11)
      theNbPointsU = 11;

    if (theNbPointsV < 11)
      theNbPointsV = 11;
  } else if (aType == STANDARD_TYPE(Geom_BezierSurface)) {
    Handle(Geom_BezierSurface) aBezier =
          Handle(Geom_BezierSurface)::DownCast(aSurface);
    // BEZIER
    theNbPointsU = 3 + aBezier->NbUPoles();
    theNbPointsV = 3 + aBezier->NbVPoles();

    if(theNbPointsU > 11 || theNbPointsV > 11) {
      TColgp_Array2OfPnt array2(1, aBezier->NbUPoles(), 1, aBezier->NbVPoles());
      aBezier->Poles(array2);
      Analyse(array2, theNbPointsU, theNbPointsV);
    }
    AdjustToDeflection( theNbPointsU, theNbPointsV, aSurface, 
		        theUMin, theUMax, theVMin, theVMax, myDeflection );

    if(theNbPointsU < 11)
      theNbPointsU = 11;

    if(theNbPointsV < 11)
      theNbPointsV = 11;
  } else if (aType == STANDARD_TYPE(Geom_BSplineSurface)) {
    // BSPLINE
    Handle(Geom_BSplineSurface) aBSpline =
          Handle(Geom_BSplineSurface)::DownCast(aSurface);

    theNbPointsU  = aBSpline->NbUKnots();
    theNbPointsU *= aBSpline->UDegree();
    theNbPointsV  = aBSpline->NbVKnots();
    theNbPointsV *= aBSpline->VDegree();

    if(theNbPointsU > 11 || theNbPointsU > 11) {
      TColgp_Array2OfPnt array2(1, aBSpline->NbUPoles(),
				1, aBSpline->NbVPoles());
      aBSpline->Poles(array2);
      Analyse(array2, theNbPointsU, theNbPointsV);
    }
    AdjustToDeflection( theNbPointsU, theNbPointsV, aSurface, 
		        theUMin, theUMax, theVMin, theVMax, myDeflection );

    if(theNbPointsU < 11)
      theNbPointsU = 11;

    if(theNbPointsV < 11)
      theNbPointsV = 11;
  } else if (aType == STANDARD_TYPE(Geom_SurfaceOfLinearExtrusion)) {
    // EXTRUSION
    theNbPointsU  = 15;
    theNbPointsV  = (Standard_Integer)(theVMax - theVMin);
    theNbPointsV /= 10;

    AdjustToDeflection( theNbPointsU, theNbPointsV, aSurface, 
		       theUMin, theUMax, theVMin, theVMax, myDeflection );

    if(theNbPointsV < 15)
      theNbPointsV = 15;
  } else if (aType == STANDARD_TYPE(Geom_SurfaceOfRevolution)) {
    // REVOLUTION
    theNbPointsU = 15;
    theNbPointsV = 15;

    AdjustToDeflection( theNbPointsU, theNbPointsV, aSurface, 
		        theUMin, theUMax, theVMin, theVMax, myDeflection );
  } else {
    // OTHER
    theNbPointsU = 11;
    theNbPointsV = 11;
    AdjustToDeflection( theNbPointsU, theNbPointsV, aSurface, 
		       theUMin, theUMax, theVMin, theVMax, myDeflection );
  }

  // Check if the number of points exceeds the maximum number of points.
  const Standard_Integer aMaxNbPoints = aLimitNumber;

  if(theNbPointsU > aMaxNbPoints) {
    if ( theNbPointsU < theNbPointsV ) {
      theNbPointsU = aMaxNbPoints;
    }
    else {
      Standard_Integer alim = aMaxNbPoints * aMaxNbPoints / theNbPointsV;
      theNbPointsU = ( theNbPointsU > alim ) ? alim : theNbPointsU;
    }
  }

  if(theNbPointsV > aMaxNbPoints) {
    if ( theNbPointsV < theNbPointsU ) {
      theNbPointsV = aMaxNbPoints;
    }
    else {
      Standard_Integer alim = aMaxNbPoints * aMaxNbPoints / theNbPointsU;
      theNbPointsV = ( theNbPointsV > alim ) ? alim : theNbPointsV;
    }
  }
}

//=======================================================================
//function : Unfolding
//purpose  : 
//=======================================================================

Standard_Boolean Unfolding_Surface::Unfolding()
{
  Handle(Geom_Surface) aSurface = BRep_Tool::Surface(myDataContainer->GetFace());
  Standard_Integer     iStart   = myGrid->LowerRow();
  Standard_Integer     jStart   = myGrid->LowerCol();
  Standard_Integer     iEnd     = myGrid->UpperRow();
  Standard_Integer     jEnd     = myGrid->UpperCol();
  Standard_Integer     i;
  Standard_Integer     j;

  // Initialize the array of 4 points on a plane. For a couple of indices (i, j)
  // the points on a plane are initialized as follows:
  // aPonPln[0] -> (i,     j + 1)
  // aPonPln[1] -> (i,     j)
  // aPonPln[2] -> (i + 1, j)
  // aPonPln[3] -> (i + 1, j + 1)
  gp_XY         aPonPln[4];
  Standard_Real aCrvDist;
  Standard_Real aCrvDist2;
  gp_XY         aDirX;
  gp_XY         aDirY;
  Standard_Real aTolConf = 0.01*Max(myTolContour, myDeflection);
  Standard_Real aNorm;
  Standard_Real aDistortionArea = 0.;

  for (j = jStart; j < jEnd; j++) {
    // Step 1: Get or compute aPonPln[1].
    Unfolding_Point &aP1 = myGrid->ChangeValue(iStart, j);

    // Add the first point aPonPln[1].
    if (j == jStart)
      aP1.SetPointOnPlane(gp_XY(0., 0.));

    // Get aPonPln[1].
    aPonPln[1] = aP1.GetPointOnPlane();

    // Step 2: Compute aPonPln[0].
    Unfolding_Point &aP2 = myGrid->ChangeValue(iStart, j + 1);

    // Compute a curvelinear distance.
    aCrvDist = CrvDistance(aP1, aP2, aSurface, myTolContour);

    if (aCrvDist <= aTolConf) {
      aPonPln[0] = aPonPln[1];
    } else {
      // Compute the Y direction.
      if (j == jStart) {
	aDirY.SetCoord(0., 1.);
      } else {
	Unfolding_Point &aP21 = myGrid->ChangeValue(iStart + 1, j);

	aDirX = aP21.GetPointOnPlane().Subtracted(aPonPln[1]);
	aNorm = aDirX.Modulus();

	if (aNorm <= aTolConf) {
	  // Invalid case.
	  myErrorStatus = Unfolding_InvalidSurface;
	  return Standard_False;
	}

	// Compute the Y direction.
	aDirX.Divide(aNorm);

	gp_Vec2d aVecX(aDirX);
	aVecX.Rotate(aP1.GetAngle());
	aDirY = aVecX.XY();
      }

      // Compute point on plane.
      aPonPln[0] = aPonPln[1].Added(aDirY.Multiplied(aCrvDist));
    }

    aP2.SetPointOnPlane(aPonPln[0]);

    for (i = iStart; i < iEnd; i++) {
      Unfolding_Point &aP11 = myGrid->ChangeValue(i,     j);
      Unfolding_Point &aP21 = myGrid->ChangeValue(i + 1, j);
      Unfolding_Point &aP22 = myGrid->ChangeValue(i + 1, j + 1);
      Unfolding_Point &aP12 = myGrid->ChangeValue(i,     j + 1);

      // Step 3: Compute aPonPln[2].
      aCrvDist = CrvDistance(aP11, aP21, aSurface, myTolContour);

      if (aCrvDist <= aTolConf) {
	if (j > jStart) {
	  // Invalid case.
	  myErrorStatus = Unfolding_InvalidSurface;
	  return Standard_False;
	}
	aPonPln[2] = aPonPln[1];
      } else {
	aDirY = aPonPln[0].Subtracted(aPonPln[1]);
	aNorm = aDirY.Modulus();

	if (aNorm <= aTolConf) {
	  if (i == iStart) {
	    aDirX.SetCoord(1., 0.);
	  } else {
	    // Invalid case.
	    myErrorStatus = Unfolding_InvalidSurface;
	    return Standard_False;
	  }
	} else {
	  // Compute the X direction.
	  aDirY.Divide(aNorm);

	  gp_Vec2d aVecY(aDirY);

	  aVecY.Rotate(-aP11.GetAngle());
	  aDirX  = aVecY.XY();
	}

	aPonPln[2] = aPonPln[1].Added(aDirX.Multiplied(aCrvDist));
      }

      if (j == jStart) {
	aP21.SetPointOnPlane(aPonPln[2]);
	aCrvDist2 = CrvDistance(aP21, aP22, aSurface, myTolContour);
      } else {
	aCrvDist2 = CrvDistance(aP21, aP22, aSurface, myTolContour);
	Standard_Real aDif = aPonPln[2].Subtracted(aP21.GetPointOnPlane()).Modulus();
// 	aDistortionArea += aDif * aCrvDist * 0.5;
	Standard_Real area = aDif * aCrvDist * 0.5;
	if ( aCrvDist > gp::Resolution() ) {
	  Standard_Real aSinVal = 1. - (0.25*aDif*aDif / (aCrvDist*aCrvDist));
	  if ( aSinVal > 0. )
	    aSinVal = sqrt( aSinVal );
	  area *= aSinVal;
	}
	aDistortionArea += area;
// 	if (!aPonPln[2].IsEqual(aP21.GetPointOnPlane(), myTolContour)) {
	if ( aDif > ( aCrvDist2 + aCrvDist ) ) {
	  // The surface cannot be unfolded.
	  myErrorStatus = Unfolding_InvalidSurface;
	  return Standard_False;
	}

	// Keep old point
	aPonPln[2] = aP21.GetPointOnPlane();
      }

      // Step 4: Compute aPonPln[3].
      gp_XY         aDiag  = aPonPln[2].Subtracted(aPonPln[0]);
      Standard_Real aDDiag = aDiag.Modulus();

      if (aDDiag <= aTolConf) {
	// Invalid case.
	myErrorStatus = Unfolding_InvalidSurface;
	return Standard_False;
      }

      aCrvDist = CrvDistance(aP12, aP22, aSurface, myTolContour);

      if (aCrvDist <= aTolConf) {
	if (j < jEnd - 1) {
	  // Invalid case.
	  myErrorStatus = Unfolding_InvalidSurface;
	  return Standard_False;
	}
	aPonPln[3] = aPonPln[0];
      } else {

	if (aCrvDist2 <= aTolConf) {
	  if (i < iEnd - 1) {
	    // Invalid case.
	    myErrorStatus = Unfolding_InvalidSurface;
	    return Standard_False;
	  }
	  aPonPln[3] = aPonPln[2];
	} else {
	  if (aCrvDist + aCrvDist2 <= aDDiag + aTolConf) {
	    // Invalid case.
	    myErrorStatus = Unfolding_InvalidSurface;
	    return Standard_False;
	  }

	  // Construct two circles
	  gp_Dir2d  aX(1., 0.);
	  gp_Ax22d  aCirc1Axis = gp_Ax22d(gp_Pnt2d(aPonPln[0]), aX);
	  gp_Ax22d  aCirc2Axis = gp_Ax22d(gp_Pnt2d(aPonPln[2]), aX);
	  gp_Circ2d aCirc1(aCirc1Axis, aCrvDist);
	  gp_Circ2d aCirc2(aCirc2Axis, aCrvDist2);

	  // Analytic intersection of two circles.
	  IntAna2d_AnaIntersection anInter(aCirc1, aCirc2);

	  if (!anInter.IsDone() || anInter.NbPoints() != 2) {
	    // Invalid case.
	    myErrorStatus = Unfolding_InvalidSurface;
	    return Standard_False;
	  }

	  gp_Pnt2d anIntP = anInter.Point(1).Value();
	  gp_XY    aD30(anIntP.XY().Subtracted(aPonPln[0]));

	  if (aD30.Crossed(aDiag) > 0.) {
	    anIntP = anInter.Point(2).Value();
	    aD30   = anIntP.XY().Subtracted(aPonPln[0]);

	    if (aD30.Crossed(aDiag) > 0.) {
	      // Invalid case.
	      myErrorStatus = Unfolding_InvalidSurface;
	      return Standard_False;
	    }
	  }

	  aPonPln[3] = anIntP.XY();
	}
      }

      aP22.SetPointOnPlane(aPonPln[3]);

      // Prepare points for next iterations.
      aPonPln[0] = aPonPln[3];
      aPonPln[1] = aPonPln[2];
    }
  }

  myDataContainer->SetDistortionArea(aDistortionArea);

  return Standard_True;
}

//=======================================================================
//function : BuildPlanarFace
//purpose  : 
//=======================================================================

Standard_Boolean Unfolding_Surface::BuildPlanarFace
                (const TopTools_DataMapOfShapeInteger &theMapEdgeNbSamples)
{
  Handle(Geom_Surface) aPlane   = new Geom_Plane(myPlane);
  BRep_Builder         aBuilder;
  TopoDS_Face          aPlanarFace;
  Standard_Real        aTopoTol = Precision::Confusion();

  aBuilder.MakeFace(aPlanarFace, aPlane, aTopoTol);

  // Build wires.
  TopExp_Explorer anExp(myDataContainer->GetFace(), TopAbs_WIRE);
  TopoDS_Wire     aWire;

  for (; anExp.More(); anExp.Next()) {
    TopoDS_Wire aPlanarWire;

    aWire = TopoDS::Wire(anExp.Current());

    if (!BuildPlanarWire(aWire, theMapEdgeNbSamples, aPlanarWire)) {
      myErrorStatus = Unfolding_Failure;

      return Standard_False;
    }

    aBuilder.Add(aPlanarFace, aPlanarWire);
  }

  myDataContainer->SetUnfoldedFace(aPlanarFace);
  myErrorStatus = Unfolding_Done;

  return Standard_True;
}

//=======================================================================
//function : BuildPlanarWire
//purpose  : 
//=======================================================================

Standard_Boolean Unfolding_Surface::BuildPlanarWire
                      (const TopoDS_Wire                    &theWire,
		       const TopTools_DataMapOfShapeInteger &theMapEdgeNbSamples,
			     TopoDS_Wire                    &thePlanarWire)
{
  BRep_Builder           aBuilder;
  BRepTools_WireExplorer aWExp(theWire, myDataContainer->GetFace());
  TopoDS_Vertex          aV[2];
  TopoDS_Vertex          aVEmpty;
  TopoDS_Vertex          aVFirst[2];
  gp_XY                  aBndPnt[2] = { gp_XY(0., 0.), gp_XY(0., 0.) };
  gp_XY                  aPntFirst;

  aBuilder.MakeWire(thePlanarWire);

  for (; aWExp.More();) {
    TopoDS_Edge anEdge = aWExp.Current();

    aWExp.Next();
    // Skip degenerated edges as they cannot be unfolded.
    if (BRep_Tool::Degenerated(anEdge))
      continue;

    aV[0] = TopExp::LastVertex(anEdge, Standard_True);

    if (!aVFirst[0].IsNull() && !aWExp.More() && aVFirst[0].IsSame(aV[0])) {
      // Close the edge
      aVEmpty    = aVFirst[1];
      aBndPnt[1] = aPntFirst;
    }

    // Perform unfolding
    TopTools_ListOfShape anEdges;

    if (!UnfoldEdge(anEdge, aV[1], aVEmpty, theMapEdgeNbSamples,
		    aBndPnt[0], aBndPnt[1], anEdges))
      return Standard_False;
    myDataContainer->SetEdgesForEdge(anEdge, anEdges);

    if (anEdges.IsEmpty())
      return Standard_False;

    // Build result.
    TopTools_ListIteratorOfListOfShape anIter(anEdges);

    for (; anIter.More(); anIter.Next()) {
      const TopoDS_Shape &aPlnEdge = anIter.Value();

      aBuilder.Add(thePlanarWire, aPlnEdge);
    }

    if (aVFirst[1].IsNull()) {
      aVFirst[0] = TopExp::FirstVertex(anEdge, Standard_True);
      aVFirst[1] = TopExp::FirstVertex(TopoDS::Edge(anEdges.First()));
      aPntFirst  = aBndPnt[0];
    }

    aV[1]      = TopExp::LastVertex(TopoDS::Edge(anEdges.Last()));
    aBndPnt[0] = aBndPnt[1];
  }

  thePlanarWire.Closed(Standard_True);

  return Standard_True;
}

//=======================================================================
//function : UnfoldEdge
//purpose  : 
//=======================================================================

Standard_Boolean Unfolding_Surface::UnfoldEdge
                         (const TopoDS_Edge                    &theEdge,
			  const TopoDS_Vertex                  &theStartVtx,
			  const TopoDS_Vertex                  &theEndVtx,
			  const TopTools_DataMapOfShapeInteger &theMapEdgeNbSamples,
			        gp_XY                          &theStartPnt,
			        gp_XY                          &theEndPnt,
				TopTools_ListOfShape           &thePlnEdges) const
{
  Standard_Boolean      isReversed = theEdge.Orientation() == TopAbs_REVERSED;
  Standard_Real         aTol       = BRep_Tool::Tolerance(theEdge);
  Standard_Real         aParam[2];
  const TopoDS_Face    &aFace      = myDataContainer->GetFace();
  Handle(Geom2d_Curve)  a2dCurve   = BRep_Tool::CurveOnSurface
                                      (theEdge, aFace, aParam[0], aParam[1]);
  Standard_Integer     aNbPoints;

  if (theMapEdgeNbSamples.IsBound(theEdge)) {
    // Get already computed sampling.
    aNbPoints = theMapEdgeNbSamples.Find(theEdge);
  } else {
    // Compute sampling
    TopTools_ListOfShape aFaces;

    aFaces.Append(aFace);
    aNbPoints = Unfolding::NbSamples(theEdge, aFaces, myTolContour);
  }

  // Make sampling with middle points.
  aNbPoints *= 2;
  aNbPoints--;

  Standard_Real       aCurPar = aParam[0];
  Standard_Real       aStep   = (aParam[1] - aParam[0])/(aNbPoints - 1);
  Standard_Integer    i       = 1;
  gp_Pnt2d            aPUV;
  gp_XY               aPOnP;
  TColgp_SequenceOfXY aSeqPnts;
  Standard_Integer    aNbIter = aNbPoints;

  if (!theStartVtx.IsNull()) {
    // Do not compute the first point.
    if (isReversed)
      aNbIter--;
    else {
      aCurPar += aStep;
      i++;
    }
  }

  if (!theEndVtx.IsNull()) {
    // Do not compute the last point.
    if (isReversed) {
      aCurPar += aStep;
      i++;
    } else
      aNbIter--;
  }

  for (; i <= aNbIter; i++) {
    a2dCurve->D0(aCurPar, aPUV);

    if (!ComputePointOnPlane(aPUV.XY(), aTol, aPOnP))
      return Standard_False;

    if (isReversed)
      aSeqPnts.Prepend(aPOnP);
    else
      aSeqPnts.Append(aPOnP);

    aCurPar = (i == aNbPoints - 1) ? aParam[1] : aCurPar + aStep;
  }

  // Prepend theStartPnt to the sequence of points if it was computed before.
  if (!theStartVtx.IsNull())
    aSeqPnts.Prepend(theStartPnt);
  else
    theStartPnt = aSeqPnts.First();

  // Append theEndPoint to the sequence of points if it was computed before.
  if (!theEndVtx.IsNull())
    aSeqPnts.Append(theEndPnt);
  else
    theEndPnt = aSeqPnts.Last();

  // Compute edges.
  if (aSeqPnts.IsEmpty())
    return Standard_False;

  TopoDS_Vertex    aV[2];
  Standard_Boolean isClosed = Standard_False;

  TopExp::Vertices(theEdge, aV[0], aV[1], Standard_True);

  if (aV[0].IsSame(aV[1])) {
    gp_Pnt2d aP2d[2];

    a2dCurve->D0(aParam[0], aP2d[0]);
    a2dCurve->D0(aParam[1], aP2d[1]);
    isClosed = aP2d[0].IsEqual(aP2d[1], aTol);
  }

  // Check if the first point coincides with theStartVtx.
  Handle(Geom_Surface)  aPlane  = new Geom_Plane(myPlane);
  const gp_XY          &aFirst  = aSeqPnts.First();
  const gp_XY          &aLast   = aSeqPnts.Last();
  gp_Pnt                aPFirst;
  gp_Pnt                aPLast;

  aPlane->D0(aFirst.X(), aFirst.Y(), aPFirst);
  aPlane->D0(aLast.X(), aLast.Y(), aPLast);

  if (!theStartVtx.IsNull()) {
    gp_Pnt        aPStartVtx = BRep_Tool::Pnt(theStartVtx);
    Standard_Real aVtxTol    = BRep_Tool::Tolerance(theStartVtx);
    Standard_Real aTolConf   = Max(aVtxTol, myTolContour);

    if (!aPFirst.IsEqual(aPStartVtx, aTolConf))
      return Standard_False;

    if (isClosed && !aPLast.IsEqual(aPStartVtx, aTolConf))
      return Standard_False;
  }

  // Check if the last point coincides with theEndVtx.
  if (!theEndVtx.IsNull()) {
    gp_Pnt        aPEndVtx = BRep_Tool::Pnt(theEndVtx);
    Standard_Real aVtxTol  = BRep_Tool::Tolerance(theEndVtx);
    Standard_Real aTolConf = Max(aVtxTol, myTolContour);

    if (!aPLast.IsEqual(aPEndVtx, aTolConf))
      return Standard_False;
  }

  // Construct edges.
  gp_Pnt             aPOnPln[2];
  gp_Pnt             aPMid;
  Standard_Real      aTopoTol = Precision::Confusion();
  BRep_Builder       aBuilder;
  Handle(Geom_Curve) aCurve;

  for (i = 1; i <= aNbPoints; i += 2) {
    const gp_XY &aP = aSeqPnts.Value(i);

    aPlane->D0(aP.X(), aP.Y(), aPOnPln[1]);

    // Get or compute aV[1].
    if (i == 1 && !theStartVtx.IsNull()) {
      aV[1] = theStartVtx;
    } else if (i == aNbPoints) {
      if (isClosed && !theStartVtx.IsNull())
	aV[1] = theStartVtx;
      else if(!theEndVtx.IsNull())
	aV[1] = theEndVtx;
      else
	aBuilder.MakeVertex(aV[1], aPOnPln[1], aTopoTol);
    } else
      aBuilder.MakeVertex(aV[1], aPOnPln[1], aTopoTol);

    if (i == 1) {
      aV[0] = aV[1];
      continue;
    }

    // Compute a curve
    const gp_XY &aPm1 = aSeqPnts.Value(i - 1);

    aPOnPln[0] = BRep_Tool::Pnt(aV[0]);
    aPlane->D0(aPm1.X(), aPm1.Y(), aPMid);

    aCurve = MakeCurve(aPOnPln[0], aPMid, aPOnPln[1], myPlane, myTolContour);

    if (aCurve.IsNull())
      return Standard_False;

    // Construct the edge.
    BRepBuilderAPI_MakeEdge aMkEdge(aCurve, aV[0], aV[1]);

    if (!aMkEdge.IsDone())
      return Standard_False;

    const TopoDS_Edge &anEdge = aMkEdge.Edge();

    thePlnEdges.Append(anEdge);
    aV[0] = aV[1];
  }

  return Standard_True;
}

//=======================================================================
//function : ComputePointOnPlane
//purpose  : 
//=======================================================================

Standard_Boolean Unfolding_Surface::ComputePointOnPlane
                   (const gp_XY         &thePoint,
		    const Standard_Real  theEdgeTol,
		          gp_XY         &thePointOnPlane) const
{
  Standard_Integer i;
  Standard_Integer j;
  Standard_Integer iStart = myGrid->LowerRow();
  Standard_Integer jStart = myGrid->LowerCol();
  Standard_Integer iEnd   = myGrid->UpperRow();
  Standard_Integer jEnd   = myGrid->UpperCol();

  GetCell(thePoint, myGrid, theEdgeTol, i, j);

  if (i < iStart)
    i++;

  if (j < jStart)
    j++;

  const Unfolding_Point &aP11     = myGrid->Value(i, j);
  gp_Pnt                 aPOnSurf;
  gp_XYZ                 aDP;
  Handle(Geom_Surface)   aSurface =
                            BRep_Tool::Surface(myDataContainer->GetFace());

  aSurface->D0(thePoint.X(), thePoint.Y(), aPOnSurf);
  aDP = aPOnSurf.XYZ();
  aDP.Subtract(aP11.GetPointOnSurface());

  if (aDP.Modulus() < myTolContour) {
    thePointOnPlane = aP11.GetPointOnPlane();
    return Standard_True;
  }

  gp_XY  aDP2d = thePoint.Subtracted(gp_XY(aP11.GetU(), aP11.GetV()));
  gp_XYZ aDIso;
  gp_XY  aDIso2d;
  gp_XY  aDIsoSurf;

  // Get direction along V.
  if (j == jEnd) {
    const Unfolding_Point &aP1M1 = myGrid->Value(i, j - 1);
    aDIso     = aP11.GetPointOnSurface() - aP1M1.GetPointOnSurface();
    aDIso2d   = aP11.GetPointOnPlane()   - aP1M1.GetPointOnPlane();
    aDIsoSurf.SetCoord(aP11.GetU() - aP1M1.GetU(), aP11.GetV() - aP1M1.GetV());
  } else {
    const Unfolding_Point &aP12 = myGrid->Value(i, j + 1);
    aDIso    = aP12.GetPointOnSurface() - aP11.GetPointOnSurface();
    aDIso2d  = aP12.GetPointOnPlane()   - aP11.GetPointOnPlane();
    aDIsoSurf.SetCoord(aP12.GetU() - aP11.GetU(), aP12.GetV() - aP11.GetV());
  }

  if (aDIso.Modulus() <= myTolContour) {
    // Get direction along U.
    if (i == iEnd) {
      const Unfolding_Point &aPM11 = myGrid->Value(i - 1, j);
      aDIso    = aP11.GetPointOnSurface() - aPM11.GetPointOnSurface();
      aDIso2d  = aP11.GetPointOnPlane()   - aPM11.GetPointOnPlane();
      aDIsoSurf.SetCoord(aP11.GetU() - aPM11.GetU(), aP11.GetV() - aPM11.GetV());
    } else {
      const Unfolding_Point &aP21 = myGrid->Value(i + 1, j);
      aDIso    = aP21.GetPointOnSurface() - aP11.GetPointOnSurface();
      aDIso2d  = aP21.GetPointOnPlane()   - aP11.GetPointOnPlane();
      aDIsoSurf.SetCoord(aP21.GetU() - aP11.GetU(), aP21.GetV() - aP11.GetV());
    }
  }

  if (aDIso.Modulus() <= myTolContour)
    return Standard_False;

  // Compute angle between aDP and aDIso
  gp_Vec        aVecIso(aDIso);
  Standard_Real anAngle = aVecIso.Angle(gp_Vec(aDP));

  if (aDP2d.Crossed(aDIsoSurf) < 0.)
    anAngle = -anAngle;

  // Rotate DIso2d by anAngle.
  gp_Dir2d aPDir(aDIso2d);

  aPDir.Rotate(-anAngle);

  Unfolding_Point aPoint;
  Standard_Real   aCrvDist;

  aPoint.SetPointOnSurface(aPOnSurf.XYZ());
  aPoint.SetParameters(thePoint.X(), thePoint.Y());
  aCrvDist = CrvDistance(aP11, aPoint, aSurface, myTolContour);

  // Compute point on plane
  thePointOnPlane  = aP11.GetPointOnPlane();
  thePointOnPlane += aPDir.XY().Multiplied(aCrvDist);

  return Standard_True;
}

static void AdjustToDeflection(      Standard_Integer     &theNbPointsU, 
			             Standard_Integer     &theNbPointsV,
			       const Handle(Geom_Surface) &theSurface, 
			       const Standard_Real         theUMin,
			       const Standard_Real         theUMax,
			       const Standard_Real         theVMin,
			       const Standard_Real         theVMax,
			       const Standard_Real         theDeflection)
{
  if ( theNbPointsU < 2 )
    theNbPointsU = 2;

  if ( theNbPointsV < 2 )
    theNbPointsV = 2;
  Standard_Real        aUStep   = (theUMax - theUMin)/(theNbPointsU - 1);
  Standard_Real        aVStep   = (theVMax - theVMin)/(theNbPointsV - 1);
  Standard_Real        aUPar;
  Standard_Real        aVPar;
  Standard_Integer     i;
  Standard_Integer     j;
  TColgp_Array2OfPnt   aMesh(1, theNbPointsU, 1, theNbPointsV);
  gp_Pnt               aPnt;
  

  for (aUPar = theUMin, i = 1; i <= theNbPointsU; i++) {
    for (aVPar = theVMin, j = 1; j <= theNbPointsV; j++) {
      aPnt = theSurface->Value(aUPar, aVPar);
      aMesh.SetValue(i, j, aPnt);
      aVPar = (j == theNbPointsV - 1) ? theVMax : aVPar + aVStep;
    }
    aUPar = (i == theNbPointsU - 1) ? theUMax : aUPar + aUStep;
  }
  Standard_Real aMaxDist = 0.;
  Standard_Real aMaxDifU = 0.;
  Standard_Real aMaxDifV = 0.;

  for (aUPar = theUMin + aUStep, i = 2; i <= theNbPointsU; i++) {
    for (aVPar = theVMin + aVStep, j = 2; j <= theNbPointsV; j++) {
      aPnt = theSurface->Value(aUPar-aUStep*0.5, aVPar-aVStep*0.5);
      gp_XYZ xyz = aMesh.Value(i,j).XYZ();
      xyz += aMesh.Value(i-1,j).XYZ();
      xyz += aMesh.Value(i,j-1).XYZ();
      xyz += aMesh.Value(i-1,j-1).XYZ();
      xyz /= 4.;
      Standard_Real aVal = aMesh.Value(i,j).SquareDistance(aMesh.Value(i-1,j));
      if ( aMaxDifU < aVal )
	aMaxDifU = aVal;

      aVal = aMesh.Value(i,j-1).SquareDistance(aMesh.Value(i-1,j-1));
      if ( aMaxDifU < aVal )
	aMaxDifU = aVal;

      aVal = aMesh.Value(i,j).SquareDistance(aMesh.Value(i,j-1));
      if ( aMaxDifV < aVal )
	aMaxDifV = aVal;

      aVal = aMesh.Value(i-1,j).SquareDistance(aMesh.Value(i-1,j-1));
      if ( aMaxDifV < aVal )
	aMaxDifV = aVal;

      Standard_Real aDist = aPnt.SquareDistance(gp_Pnt(xyz));
      if ( aMaxDist < aDist )
	aMaxDist = aDist;

      aVPar = (j == theNbPointsV - 1) ? theVMax : aVPar + aVStep;
    }
    aUPar = (i == theNbPointsU - 1) ? theUMax : aUPar + aUStep;
  }
  aMaxDist = sqrt( aMaxDist );

  if ( aMaxDist > theDeflection ) {
    aMaxDifU = sqrt( aMaxDifU );
    aMaxDifV = sqrt( aMaxDifV );

    Standard_Real anewdifu = theDeflection * aMaxDifU / aMaxDist;
    Standard_Real anewdifv = theDeflection * aMaxDifV / aMaxDist;

    Standard_Real aVal = ( theUMax - theUMin ) / anewdifu;
    if ( aVal > aLimitNumber )
      aVal = aLimitNumber;
    theNbPointsU = ( theNbPointsU < aVal ) ? Standard_Integer (aVal) : theNbPointsU;

    aVal = ( theVMax - theVMin ) / anewdifv;
    if ( aVal > aLimitNumber )
      aVal = aLimitNumber;
    theNbPointsV = ( theNbPointsV < aVal ) ? Standard_Integer (aVal) : theNbPointsV;
  }
}
