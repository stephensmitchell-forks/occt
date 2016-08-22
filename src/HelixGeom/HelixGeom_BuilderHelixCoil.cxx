// File:	HelixGeom_BuilderHelixCoil.cxx


#include <Geom_BSplineCurve.hxx>
#include <HelixGeom_BuilderHelixCoil.hxx>
#include <HelixGeom_HelixCurve.hxx>
#include <HelixGeom_HHelixCurve.hxx>
#include <HelixGeom_Tools.hxx>

//=======================================================================
//function : HelixGeom_BuilderHelixCoil
//purpose  : 
//=======================================================================
HelixGeom_BuilderHelixCoil::HelixGeom_BuilderHelixCoil()
:
  HelixGeom_BuilderHelixGen()
{
  myT1=0.;
  myT2=2.*M_PI;
  myPitch=1.;
  myRStart=1.;
  myTaperAngle=0.;
  myIsClockWise=Standard_True;
}
//=======================================================================
//function : ~
//purpose  : 
//=======================================================================
HelixGeom_BuilderHelixCoil::~HelixGeom_BuilderHelixCoil()
{
}
//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================
void  HelixGeom_BuilderHelixCoil::Perform()
{
  myErrorStatus=0;
  myWarningStatus=0;
  //
  Standard_Integer iErr;
  HelixGeom_HelixCurve aAdaptor;
  Handle(HelixGeom_HHelixCurve) aHAdaptor;
  Handle(Geom_BSplineCurve)aBC;
  //
  myCurves.Clear();
  //
  aAdaptor.Load(myT1, myT2, myPitch, myRStart, myTaperAngle, myIsClockWise);
  aHAdaptor=new HelixGeom_HHelixCurve(aAdaptor);
  //
  iErr=HelixGeom_Tools::ApprCurve3D(aHAdaptor, 
				  myTolerance, myCont, myMaxSeg, myMaxDegree, 
				  aBC, myTolReached);
  if(iErr) {
    myErrorStatus=2;
  }
  else {
    myCurves.Append(aBC);
  }
}



