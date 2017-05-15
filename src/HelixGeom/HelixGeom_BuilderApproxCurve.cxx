// File:	HelixGeom_BuilderApproxCurve.cxx


#include <HelixGeom_BuilderApproxCurve.hxx>
#include <TColGeom_SequenceOfCurve.hxx>

//=======================================================================
//function : 
//purpose  : 
//=======================================================================
HelixGeom_BuilderApproxCurve::HelixGeom_BuilderApproxCurve()
{
  myTolerance=0.0001;
  myCont=GeomAbs_C2;
  myMaxDegree=8;
  myMaxSeg=150;
  //
  myTolReached=99.;
}
//=======================================================================
//function : ~
//purpose  : 
//=======================================================================
HelixGeom_BuilderApproxCurve::~HelixGeom_BuilderApproxCurve()
{
}
//=======================================================================
//function : SetApproxParameters
//purpose  : 
//=======================================================================
void HelixGeom_BuilderApproxCurve::SetApproxParameters(const GeomAbs_Shape aCont,
						     const Standard_Integer aMaxDegree,
						     const Standard_Integer aMaxSeg)
{
  
  myCont=aCont;
  myMaxDegree=aMaxDegree;
  myMaxSeg=aMaxSeg;
}
//=======================================================================
//function : ApproxParameters
//purpose  : 
//=======================================================================
void HelixGeom_BuilderApproxCurve::ApproxParameters(GeomAbs_Shape& aCont,
						  Standard_Integer& aMaxDegree,
						  Standard_Integer& aMaxSeg)const
{
  aCont=myCont;
  aMaxDegree=myMaxDegree;
  aMaxSeg=myMaxSeg;
}
//=======================================================================
//function : SetTolerance
//purpose  : 
//=======================================================================
void HelixGeom_BuilderApproxCurve::SetTolerance(const Standard_Real aTolerance)
{
  myTolerance=aTolerance;
}
//=======================================================================
//function : Tolerance
//purpose  : 
//=======================================================================
Standard_Real HelixGeom_BuilderApproxCurve::Tolerance()const
{
  return myTolerance;
}
//=======================================================================
//function : ToleranceReached
//purpose  : 
//=======================================================================
Standard_Real HelixGeom_BuilderApproxCurve::ToleranceReached()const
{
  return myTolReached;
}

//=======================================================================
//function : Curves
//purpose  : 
//=======================================================================
const TColGeom_SequenceOfCurve&  HelixGeom_BuilderApproxCurve::Curves()const
{
  return myCurves;
}

//=======================================================================   
//function : ErrorStatus       
//purpose  :                  
//======================================================================= 
Standard_Integer  HelixGeom_BuilderApproxCurve::ErrorStatus()const   
{                                                    
  return myErrorStatus;                  
}      
                        
//=======================================================================    
//function : WarningStatus    
//purpose  :                 
//=======================================================================
Standard_Integer  HelixGeom_BuilderApproxCurve::WarningStatus()const
{                            
  return myWarningStatus;     
}
