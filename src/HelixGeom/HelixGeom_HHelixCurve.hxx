// File:	HelixGeom_HHelixCurve.cdl

#ifndef _HelixGeom_HHelixCurve_HeaderFile
#define _HelixGeom_HHelixCurve_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <HelixGeom_GHHelixCurve.hxx>
#include <Standard_Real.hxx>
#include <Standard_Boolean.hxx>
class HelixGeom_HelixCurve;


class HelixGeom_HHelixCurve;
DEFINE_STANDARD_HANDLE(HelixGeom_HHelixCurve, HelixGeom_GHHelixCurve)

//! HAdaptor class for helix  curve
class HelixGeom_HHelixCurve : public HelixGeom_GHHelixCurve
{

public:

  
  //! Empty  constructor
  Standard_EXPORT HelixGeom_HHelixCurve();
  
  //! Constructor by  corresponding  adaptor  curve
  Standard_EXPORT HelixGeom_HHelixCurve(const HelixGeom_HelixCurve& aC);
  
  //! Constructor by parameters
  Standard_EXPORT HelixGeom_HHelixCurve(const Standard_Real aT1, const Standard_Real aT2, const Standard_Real aPitch, const Standard_Real aRStart, const Standard_Real aTaperAngle, const Standard_Boolean aIsCW);




  DEFINE_STANDARD_RTTIEXT(HelixGeom_HHelixCurve,HelixGeom_GHHelixCurve)

protected:




private:




};







#endif // _HelixGeom_HHelixCurve_HeaderFile
