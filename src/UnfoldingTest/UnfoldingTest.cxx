// File:	UnfoldingTest.cxx
// Created:	Tue Jul 22 18:35:23 2008
// Author:	Sergey KHROMOV
//		<skv@dimox>


#include <UnfoldingTest.hxx>
#include <Draw.hxx>
#include <Draw_PluginMacro.hxx>
#include <Draw_Interpretor.hxx>
#include <TopoDS_Shape.hxx>
#include <DBRep.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS.hxx>
#include <Geom_Surface.hxx>
#include <DrawTrSurf.hxx>
#include <Geom_Plane.hxx>
#include <Unfolding_Surface.hxx>
#include <Unfolding_Shell.hxx>
#include <Unfolding.hxx>
#include <TopTools_DataMapOfShapeInteger.hxx>


//=======================================================================
//function : dumpError
//purpose  : 
//=======================================================================

static void dumpError (Draw_Interpretor       &di,
		       const Standard_Integer  theStatus)
{
  di << "  Status: ";

  switch (theStatus) {
  case Unfolding_Done:
    di << "Done";
    break;
  case Unfolding_NotDone:
    di << "NotDone";
    break;
  case Unfolding_Failure:
    di << "Failure";
    break;
  case Unfolding_InvalidSurface:
    di << "InvalidSurface";
    break;
  case Unfolding_InvalidInput:
    di << "InvalidInput";
    break;
  case Unfolding_InvalidShape:
    di << "InvalidShape";
    break;
  case Unfolding_ComplexShape:
    di << "ComplexShape";
    break;
  default:
    di << "Unknown";
    break;
  }

  di << " \n";
}

//=======================================================================
//function : unfolding
//purpose  : 
//=======================================================================
static Standard_Integer unfolding (Draw_Interpretor &di,
                                   Standard_Integer  n, const char** a)
{
  if (n < 4 || n > 7) {
    di << "Usage: " << a[0] << " result face|shell plane [tol_contour] [tol_curvature] [deflection]" << "\n";
    return 1;
  }

  // Get a shape from a[2].
  TopoDS_Shape aShape = DBRep::Get(a[2]);

  if (aShape.IsNull()) {
    di << a[2] << " is not a shape!" << "\n";
    return 1;
  }

  // Check if the shape is a face.
  if (aShape.ShapeType() != TopAbs_FACE &&
      aShape.ShapeType() != TopAbs_SHELL) {
    di << a[2] << " is not a face or shell!" << "\n";
    return 1;
  }
  TopoDS_Shell aShell;
  TopoDS_Face aFace;
  if (aShape.ShapeType() == TopAbs_FACE) {
    aFace = TopoDS::Face( aShape );
    if (aFace.IsNull()) {
      di << a[2] << " is not a face!" << "\n";
      return 1;
    }
  }
  else {
    aShell = TopoDS::Shell( aShape );
  }

  // Get a surface from a[3].
  Handle(Geom_Surface) aSurface = DrawTrSurf::GetSurface(a[3]);

  if (aSurface.IsNull()) {
    di << a[3] << " is not a surface!" << "\n";
    return 1;
  }

  // Cast the surface to a plane.
  Handle(Geom_Plane) aPlane = Handle(Geom_Plane)::DownCast(aSurface);

  if (aPlane.IsNull()) {
    di << a[3] << " is not a plane!" << "\n";
    return 1;
  }

  // Get the tolerance value.
  Standard_Real aTolContour = 0.001;

  if (n >= 5) {
	  aTolContour = Draw::Atof(a[4]);
  } else {
    di << "tolerance value is not provided. The default value " << aTolContour << " is used" << "\n";
  }

  // Get the curvature tolerance value.
  Standard_Real aTolCurv = 0.001;

  if (n >= 6) {
    aTolCurv = Draw::Atof(a[5]);
  } else {
    di << "curvature value is not provided. The default value " << aTolCurv << " is used" << "\n";
  }

  // Get the curvature tolerance value.
  Standard_Real aDefl = 0.001;

  if (n == 7) {
    aDefl = Draw::Atof(a[6]);
  } else {
    di << "deflection value is not provided. The default value " << aDefl << " is used" << "\n";
  }


  // Initialization of the unfolding algorithm
  Standard_Boolean  isOK = Standard_False;
  TopoDS_Shape aResult;
  Standard_Integer aErrorStatus = 0;

  if ( !aFace.IsNull() ) {
    TopTools_DataMapOfShapeInteger anEmptyMap;
    Unfolding_Surface anUnfold(aFace, aPlane->Pln(), aTolContour, aTolCurv, aDefl);
    isOK = anUnfold.Perform(anEmptyMap);

    Handle(Unfolding_FaceDataContainer) aData = anUnfold.GetDataContainer();

    di << "Gauss curvature =" << aData->GetMaxGaussCurvature() << "\n";
    di << "Area of error =" << aData->GetDistortionArea() << "\n";
    if ( isOK )
      aResult = aData->GetUnfoldedFace();
    aErrorStatus = anUnfold.ErrorStatus();
  }
  else if ( !aShell.IsNull() ) {
    Unfolding_Shell anUnfold(aShell, aPlane->Pln(), aTolContour, aTolCurv, aDefl);
    isOK = anUnfold.Perform();

    di << "Gauss curvature =" << anUnfold.GetMaxGaussCurvature() << "\n";
    di << "Area of error =" << anUnfold.GetAreaError() << "\n";
    if ( isOK )
      aResult = anUnfold.GetResult();
    aErrorStatus = anUnfold.ErrorStatus();
  }

  if (isOK) {
    DBRep::Set(a[1], aResult);
  } else {
    di << "Unfolding failure!" << "\n";
    dumpError(di, aErrorStatus);

    return 1;
  }

  return 0;
}

//=======================================================================
//function : toshell
//purpose  : 
//=======================================================================

static Standard_Integer toshell (Draw_Interpretor &di,
				 Standard_Integer  n, const char** a)
{
  if (n != 3 && n != 4) {
    di << "Usage: " << a[0] << " result shape [tolerance]" << "\n";;
    return 1;
  }

  // Get a shape from a[2].
  TopoDS_Shape aShape = DBRep::Get(a[2]);

  if (aShape.IsNull()) {
    di << a[2] << " is not a shape!" << "\n";
    return 1;
  }

  // Get the tolerance value.
  Standard_Real aTolerance = 1.e-6;

  if (n == 4) {
    aTolerance = Draw::Atof(a[3]);
  } else {
    di << "tolerance value is not provided. The default value " << aTolerance << " is used" << "\n";
  }

  Unfolding_ErrorStatus aStatus;
  TopoDS_Shell          aResult = Unfolding::ToShell(aShape, aTolerance, aStatus);

  if (aResult.IsNull()) {
    di << "toshell failure!" << "\n";
    dumpError(di, aStatus);

    return 1;
  }

  DBRep::Set(a[1], aResult);

  return 0;
}

//=======================================================================
//function : Commands
//purpose  : 
//=======================================================================

void UnfoldingTest::Commands(Draw_Interpretor &theDI)
{
  static Standard_Boolean done = Standard_False;

  if (done)
    return;

  done = Standard_True;

  // Chapter's name
  const char* group = "Unfolding commands";

  theDI.Add("unfolding","unfolding result face|shell plane [tol_contour] [tol_curvature] [deflection]",
            __FILE__, unfolding, group);

  theDI.Add("toshell","toshell result shape [tolerance]",
            __FILE__, toshell, group);
}

//=======================================================================
//function : Factory
//purpose  : 
//=======================================================================

void UnfoldingTest::Factory(Draw_Interpretor &theDI)
{
  UnfoldingTest::Commands(theDI);
#ifdef DEB
      theDI << "Draw Plugin : Unfolding commands are loaded" << "\n";
#endif
}

// Declare entry point PLUGINFACTORY
DPLUGIN(UnfoldingTest)
