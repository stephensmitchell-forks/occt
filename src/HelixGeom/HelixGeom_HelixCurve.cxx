// File:	HelixGeom_HelixCurve.cxx


#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <HelixGeom_HelixCurve.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_NotImplemented.hxx>
#include <Standard_OutOfRange.hxx>
#include <TColStd_Array1OfReal.hxx>

#include <math.h>
//=======================================================================
//function : HelixGeom_HelixCurve
//purpose  : 
//=======================================================================
HelixGeom_HelixCurve::HelixGeom_HelixCurve()
{
  myFirst=0.;
  myLast=2.*M_PI;
  myPitch=1.;
  myRStart=1.;
  myTaperAngle=0.;
  myIsClockWise=Standard_True;
  //
  myC1=myPitch/myLast;
  myTgBeta=0.;
  myTolAngle=1.e-4;
}
//=======================================================================
//function : Load
//purpose  : 
//=======================================================================
void HelixGeom_HelixCurve::Load()
{
  Load(myFirst, myLast, myPitch, myRStart, myTaperAngle, myIsClockWise);
}
//=======================================================================
//function : Load
//purpose  : 
//=======================================================================
void HelixGeom_HelixCurve::Load(const Standard_Real aT1,
			      const Standard_Real aT2,
			      const Standard_Real aPitch,
			      const Standard_Real aRStart,
			      const Standard_Real aTaperAngle,
			      const Standard_Boolean aIsCW)
{
  char buf[]={"HelixGeom_HelixCurve::Load"};
  Standard_Real aTwoPI, aHalfPI;
  //
  aTwoPI=2.*M_PI;
  aHalfPI=0.5*M_PI;
  //
  myFirst=aT1;
  myLast=aT2;
  myPitch=aPitch;
  myRStart=aRStart;
  myTaperAngle=aTaperAngle;
  myIsClockWise=aIsCW;
  //
  if (aT1>=aT2) {
    Standard_ConstructionError::Raise(buf);
  }
  if (myPitch<0.) {
    Standard_ConstructionError::Raise(buf);
  }
  if (myRStart<0.) {
    Standard_ConstructionError::Raise(buf);
  }
  if (myTaperAngle<=-aHalfPI ||
      myTaperAngle>=aHalfPI) {
    Standard_ConstructionError::Raise(buf);
  }
  //
  myC1=myPitch/aTwoPI;
  if (fabs(myTaperAngle)>myTolAngle) {
    myTgBeta=tan(myTaperAngle);
  }
}
//=======================================================================
//function : FirstParameter
//purpose  : 
//=======================================================================
Standard_Real HelixGeom_HelixCurve::FirstParameter() const
{
  return myFirst;
}
//=======================================================================
//function : LastParameter
//purpose  : 
//=======================================================================
Standard_Real HelixGeom_HelixCurve::LastParameter() const
{
  return myLast;
}
//=======================================================================
//function : Continuity
//purpose  : 
//=======================================================================
GeomAbs_Shape HelixGeom_HelixCurve::Continuity() const 
{
  return GeomAbs_CN;
}
//=======================================================================
//function : NbIntervals
//purpose  : 
//=======================================================================
Standard_Integer HelixGeom_HelixCurve::NbIntervals(const GeomAbs_Shape ) const
{
  return 1;
}
//=======================================================================
//function : Intervals
//purpose  : 
//=======================================================================
void HelixGeom_HelixCurve::Intervals(TColStd_Array1OfReal& T,
				   const GeomAbs_Shape) const
{
  T(1)=myFirst;
  T(2)=myLast;
}  
//=======================================================================
//function : Resolution
//purpose  : 
//=======================================================================
Standard_Real HelixGeom_HelixCurve::Resolution(const Standard_Real) const 
{
  Standard_NotImplemented::Raise("HelixGeom_HelixCurve::Resolution");
  return 0.;
}
//=======================================================================
//function : IsClosed
//purpose  : 
//=======================================================================
Standard_Boolean HelixGeom_HelixCurve::IsClosed() const 
{
  Standard_NotImplemented::Raise("HelixGeom_HelixCurve::IsClosed");
  return Standard_False;
}
//=======================================================================
//function : IsPeriodic
//purpose  : 
//=======================================================================
Standard_Boolean HelixGeom_HelixCurve::IsPeriodic() const 
{
  Standard_NotImplemented::Raise("HelixGeom_HelixCurve::IsPeriodic");
  return Standard_False;
}
//=======================================================================
//function : Period
//purpose  : 
//=======================================================================
Standard_Real HelixGeom_HelixCurve::Period()const
{
  Standard_DomainError::Raise("HelixGeom_HelixCurve::Periodic");
  return 0.;
}
//=======================================================================
//function : Value
//purpose  : 
//=======================================================================
gp_Pnt HelixGeom_HelixCurve::Value(const Standard_Real aT) const 
{
  Standard_Real aST, aCT, aX, aY, aZ, a1;
  //
  aCT=cos(aT);
  aST=sin(aT);
  a1=myRStart+myC1*myTgBeta*aT;
  //
  aX=a1*aCT;
  aY=a1*aST;
  if (!myIsClockWise) {
    aY=-aY;
  }
  aZ=myC1*aT;
  return gp_Pnt(aX, aY, aZ); 
}
//=======================================================================
//function : D0
//purpose  : 
//=======================================================================
void HelixGeom_HelixCurve::D0(const Standard_Real aT, 
			    gp_Pnt& aP) const 
{
  aP=Value(aT);
}
//=======================================================================
//function : D1
//purpose  : 
//=======================================================================
void HelixGeom_HelixCurve::D1(const Standard_Real aT, 
			    gp_Pnt& aP, 
			    gp_Vec& aV1) const 
{
  Standard_Real aST, aCT, aX, aY, aZ, a1, a2;
  //
  aCT=cos(aT);
  aST=sin(aT);
  //
  a1=myRStart+myC1*myTgBeta*aT;
  //
  aX=a1*aCT;
  aY=a1*aST;
  if (!myIsClockWise) {
    aY=-aY;
  }
  aZ=myC1*aT;
  aP.SetCoord(aX, aY, aZ);
  //
  a1=myC1*myTgBeta;
  a2=myRStart+a1*aT;
  //
  aX=a1*aCT-a2*aST;
  aY=a1*aST+a2*aCT;
  if (!myIsClockWise) {
    aY=-aY;
  }
  aZ=myC1;
  aV1.SetCoord(aX, aY, aZ);
}
//=======================================================================
//function : D2
//purpose  : 
//=======================================================================
void HelixGeom_HelixCurve::D2(const Standard_Real aT, 
			    gp_Pnt& aP, 
			    gp_Vec& aV1, 
			    gp_Vec& aV2) const 
{
  Standard_Real aST, aCT, aX, aY, aZ, a1, a2;
  //
  aCT=cos(aT);
  aST=sin(aT);
  //
  a1=myRStart+myC1*myTgBeta*aT;
  //
  aX=a1*aCT;
  aY=a1*aST;
  if (!myIsClockWise) {
    aY=-aY;
  }
  aZ=myC1*aT;
  aP.SetCoord(aX, aY, aZ);
  //
  a1=myC1*myTgBeta;
  a2=myRStart+a1*aT;
  //
  aX=a1*aCT-a2*aST;
  aY=a1*aST+a2*aCT;
  if (!myIsClockWise) {
    aY=-aY;
  }
  aZ=myC1;
  aV1.SetCoord(aX, aY, aZ);
  //
  a1=2.*a1;
  aX=-a2*aCT-a1*aST;
  aY=-a2*aST-a1*aCT;
  if (!myIsClockWise) {
    aY=-aY;
  }
  aZ=0.;
  aV2.SetCoord(aX, aY, aZ);
}

//=======================================================================
//function : DN
//purpose  : 
//=======================================================================
gp_Vec HelixGeom_HelixCurve::DN(const Standard_Real aT, 
			      const Standard_Integer aN) const 
{
  gp_Pnt aP;
  gp_Vec aV1, aV2;
  //
  switch (aN) {
  case 1:
    D1(aT, aP, aV1);
    break;
  case 2:
    D2(aT, aP, aV1, aV2);
    break;
  default:
    Standard_NotImplemented::Raise("HelixGeom_HelixCurve::DN");
    break;
  }
  return gp_Vec(aV1);
}


