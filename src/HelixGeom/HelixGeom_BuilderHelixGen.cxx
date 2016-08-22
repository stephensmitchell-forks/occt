// File:	HelixGeom_BuilderHelixGen.cxx


#include <HelixGeom_BuilderHelixGen.hxx>

//=======================================================================
//function : 
//purpose  : 
//=======================================================================
HelixGeom_BuilderHelixGen::HelixGeom_BuilderHelixGen()
:       
  HelixGeom_BuilderApproxCurve()
{
}
//=======================================================================
//function : ~
//purpose  : 
//=======================================================================
HelixGeom_BuilderHelixGen::~HelixGeom_BuilderHelixGen()
{
}
//=======================================================================
//function : SetCurveParameters
//purpose  : 
//=======================================================================
void HelixGeom_BuilderHelixGen::SetCurveParameters(const Standard_Real aT1,
						 const Standard_Real aT2,
						 const Standard_Real aPitch,
						 const Standard_Real aRStart,
						 const Standard_Real aTaperAngle,
						 const Standard_Boolean aIsCW)
{
  myT1=aT1;
  myT2=aT2;
  myPitch=aPitch;
  myRStart=aRStart;
  myTaperAngle=aTaperAngle;
  myIsClockWise=aIsCW;
}
//=======================================================================
//function : CurveParameters
//purpose  : 
//=======================================================================
void HelixGeom_BuilderHelixGen::CurveParameters(Standard_Real& aT1,
					      Standard_Real& aT2,
					      Standard_Real& aPitch,
					      Standard_Real& aRStart,
					      Standard_Real& aTaperAngle,
					      Standard_Boolean& aIsClockWise)const
{
  aT1=myT1;
  aT2=myT2;
  aPitch=myPitch;
  aRStart=myRStart;
  aTaperAngle=myTaperAngle;
  aIsClockWise=myIsClockWise;
}


