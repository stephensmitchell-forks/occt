// File:	HelixGeom_BuilderHelix.cxx


#include <Geom_Curve.hxx>
#include <gp_Ax2.hxx>
#include <gp_Ax3.hxx>
#include <gp_Trsf.hxx>
#include <HelixGeom_BuilderHelix.hxx>
#include <HelixGeom_BuilderHelixCoil.hxx>
#include <TColGeom_SequenceOfCurve.hxx>

//=======================================================================
//function : 
//purpose  : 
//=======================================================================
HelixGeom_BuilderHelix::HelixGeom_BuilderHelix()
:
  HelixGeom_BuilderHelixGen()
{
}
//=======================================================================
//function : ~
//purpose  : 
//=======================================================================
HelixGeom_BuilderHelix::~HelixGeom_BuilderHelix()
{
}
//=======================================================================
//function : SetPosition
//purpose  : 
//=======================================================================
void HelixGeom_BuilderHelix::SetPosition(const gp_Ax2& aAx2)
{
  myPosition=aAx2;
}
//=======================================================================
//function : Position
//purpose  : 
//=======================================================================
const gp_Ax2& HelixGeom_BuilderHelix::Position()const
{
  return myPosition;
}
//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================
void HelixGeom_BuilderHelix::Perform()
{
  myErrorStatus=0;
  myWarningStatus=0;
  //
  Standard_Integer iErr, aN, i, aNbC;
  Standard_Real aTwoPI, dT, aT1x, aT2x, aTR;
  //HelixGeom_HelixCurve aHC;
  HelixGeom_BuilderHelixCoil aBHC;
  //
  myCurves.Clear();
  myTolReached=-1.;
  aTwoPI=2.*M_PI;
  //
  aBHC.SetTolerance(myTolerance);
  aBHC.SetApproxParameters(myCont, myMaxDegree, myMaxSeg);
  //
  dT=myT2-myT1;
  aN=(Standard_Integer)(dT/aTwoPI);
  //
  if (!aN) {
    aBHC.SetCurveParameters(myT1, myT2, myPitch, myRStart, myTaperAngle, myIsClockWise);
    aBHC.Perform();
    iErr=aBHC.ErrorStatus();
    if (iErr) {
      myErrorStatus=2;
      return;
    }
    const TColGeom_SequenceOfCurve& aSC=aBHC.Curves();
    const Handle(Geom_Curve)& aC=aSC(1);
    myCurves.Append(aC);
    myTolReached=aBHC.ToleranceReached();
  }
  //
  else {
    Standard_Boolean bIsCylindrical;
    Standard_Real aTolAngle;
    //
    aTolAngle=1.e-4;
    bIsCylindrical=fabs(myTaperAngle)<aTolAngle;
    //
    aT1x=myT1;
    aT2x=myT1+aTwoPI;
    for (i=1; i<=aN; ++i) {
      //
      if (i>1 && bIsCylindrical) {
	Handle(Geom_Curve) aCi;
	gp_Pnt aP1, aPi;
	//
	const Handle(Geom_Curve)& aC1=myCurves(1);
	aC1->D0(aC1->FirstParameter(), aP1);
	aPi.SetCoord(aP1.X(), aP1.Y(), aP1.Z()+(i-1)*myPitch);
	aCi=Handle(Geom_Curve)::DownCast(aC1->Translated(aP1, aPi));
	//
	myCurves.Append(aCi);
	//
	aT1x=aT2x;
	aT2x=aT1x+aTwoPI;
	//
	continue;
      }
	
      aBHC.SetCurveParameters(aT1x, aT2x, myPitch, myRStart, myTaperAngle, myIsClockWise);
      //
      aBHC.Perform();
      iErr=aBHC.ErrorStatus();
      if (iErr) {
	myErrorStatus=2;
	return;
      }
      //
      const TColGeom_SequenceOfCurve& aSC=aBHC.Curves();
      const Handle(Geom_Curve)& aC=aSC(1);
      myCurves.Append(aC);
      aTR=aBHC.ToleranceReached();
      if(aTR>myTolReached) {
	myTolReached=aTR;
      }
      //
      aT1x=aT2x;
      aT2x=aT1x+aTwoPI;
    } // for (i=1; i<=aN; ++i) {
    //
    aT2x=myT2;
    Standard_Real eps = 1.e-7*aTwoPI;
    if (fabs (aT2x-aT1x)>eps) {
      aBHC.SetCurveParameters(aT1x, aT2x, myPitch, myRStart, myTaperAngle, myIsClockWise);
      aBHC.Perform();
      iErr=aBHC.ErrorStatus();
      if (iErr) {
	myErrorStatus=2;
	return;
      }
      //
      const TColGeom_SequenceOfCurve& aSC=aBHC.Curves();
      const Handle(Geom_Curve)& aC=aSC(1);
      myCurves.Append(aC);
      aTR=aBHC.ToleranceReached();
      if(aTR>myTolReached) {
	myTolReached=aTR;
      }
    }
  }
  //
  // Transformation
  gp_Trsf aTrsf;
  gp_Ax3 aAx3, aAx3x(myPosition);
  //
  aTrsf.SetDisplacement(aAx3, aAx3x);
  //
  aNbC=myCurves.Length();
  for (i=1; i<=aNbC; ++i) {
    Handle(Geom_Curve)& aC=myCurves(i);
    aC->Transform(aTrsf);
  }
  //
}

