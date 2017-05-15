// File:	HelixGeom_HelixCurve.cdl

#ifndef _HelixGeom_HelixCurve_HeaderFile
#define _HelixGeom_HelixCurve_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Real.hxx>
#include <Standard_Boolean.hxx>
#include <Adaptor3d_Curve.hxx>
#include <GeomAbs_Shape.hxx>
#include <Standard_Integer.hxx>
class Standard_ConstructionError;
class Standard_OutOfRange;
class Standard_DomainError;
class gp_Pnt;
class gp_Vec;


//! Adaptor  class  for  calculation  helix  curve
class HelixGeom_HelixCurve  : public Adaptor3d_Curve
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Adaptor  class  for  calculation  helix  curve
  //! implementation  of  analytical  expressions
  Standard_EXPORT HelixGeom_HelixCurve();
  
  //! Sets  default  values  for  parameters
  Standard_EXPORT void Load();
  
  //! Sets  helix  parameters
  Standard_EXPORT void Load (const Standard_Real aT1, const Standard_Real aT2, const Standard_Real aPitch, const Standard_Real aRStart, const Standard_Real aTaperAngle, const Standard_Boolean aIsCW);
  
  //! Gets  first  parameter
  Standard_EXPORT virtual Standard_Real FirstParameter() const Standard_OVERRIDE;
  
  //! Gets  last  parameter
  Standard_EXPORT virtual Standard_Real LastParameter() const Standard_OVERRIDE;
  
  //! Gets  continuity
  Standard_EXPORT virtual GeomAbs_Shape Continuity() const Standard_OVERRIDE;
  
  //! Gets number  of  intervals
  Standard_EXPORT virtual Standard_Integer NbIntervals (const GeomAbs_Shape S) const Standard_OVERRIDE;
  
  //! Gets  parametric  intervals
  Standard_EXPORT virtual void Intervals (TColStd_Array1OfReal& T, const GeomAbs_Shape S) const Standard_OVERRIDE;
  
  //! Gets  parametric  resolution
  Standard_EXPORT virtual Standard_Real Resolution (const Standard_Real R3d) const Standard_OVERRIDE;
  
  //! Returns  False
  Standard_EXPORT virtual Standard_Boolean IsClosed() const Standard_OVERRIDE;
  
  //! Returns  False
  Standard_EXPORT virtual Standard_Boolean IsPeriodic() const Standard_OVERRIDE;
  
  //! Returns  2*PI
  Standard_EXPORT virtual Standard_Real Period() const Standard_OVERRIDE;
  
  //! Gets  curve  point  for  parameter  U
  Standard_EXPORT virtual gp_Pnt Value (const Standard_Real U) const Standard_OVERRIDE;
  
  //! Gets  curve  point  for  parameter  U
  Standard_EXPORT virtual void D0 (const Standard_Real U, gp_Pnt& P) const Standard_OVERRIDE;
  
  //! Gets  curve  point  and  first  derivatives
  //! for  parameter  U
  Standard_EXPORT virtual void D1 (const Standard_Real U, gp_Pnt& P, gp_Vec& V1) const Standard_OVERRIDE;
  
  //! Gets  curve  point,  first and  second derivatives
  //! for  parameter  U
  Standard_EXPORT virtual void D2 (const Standard_Real U, gp_Pnt& P, gp_Vec& V1, gp_Vec& V2) const Standard_OVERRIDE;
  
  //! Gets  curve  derivative  of  demanded  order
  //! for  parameter  U
  Standard_EXPORT virtual gp_Vec DN (const Standard_Real U, const Standard_Integer N) const Standard_OVERRIDE;




protected:



  Standard_Real myFirst;
  Standard_Real myLast;
  Standard_Real myPitch;
  Standard_Real myRStart;
  Standard_Real myTaperAngle;
  Standard_Boolean myIsClockWise;
  Standard_Real myC1;
  Standard_Real myTgBeta;
  Standard_Real myTolAngle;


private:





};







#endif // _HelixGeom_HelixCurve_HeaderFile
