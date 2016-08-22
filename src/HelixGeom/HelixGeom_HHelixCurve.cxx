// File:	HelixGeom_HHelixCurve.cxx


#include <HelixGeom_HelixCurve.hxx>
#include <HelixGeom_HHelixCurve.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(HelixGeom_HHelixCurve,HelixGeom_GHHelixCurve)

//=======================================================================
//function : HelixGeom_HHelixCurve
//purpose  : 
//=======================================================================
HelixGeom_HHelixCurve::HelixGeom_HHelixCurve()
{
}
//=======================================================================
//function : HelixGeom_HHelixCurve// Lastly modified by :
// +---------------------------------------------------------------------------+
// !       ifv ! Creation                                !$Date: 2007-10-19 18:21:42 $! %V%-%L%!
// +---------------------------------------------------------------------------+


//purpose  : 
//=======================================================================
HelixGeom_HHelixCurve::HelixGeom_HHelixCurve(const HelixGeom_HelixCurve& aC)
: HelixGeom_GHHelixCurve(aC)
{
}
//=======================================================================
//function : HelixGeom_HHelixCurve
//purpose  : 
//=======================================================================
HelixGeom_HHelixCurve::HelixGeom_HHelixCurve(const Standard_Real aT1,
					 const Standard_Real aT2,
					 const Standard_Real aPitch,
					 const Standard_Real aRStart,
					 const Standard_Real aTaperAngle,
					 const Standard_Boolean aIsCW)
{
  ChangeCurve().Load(aT1, aT2, aPitch, aRStart, aTaperAngle, aIsCW);
}


