// File:	HelixGeom_BuilderApproxCurve.cdl

#ifndef _HelixGeom_BuilderApproxCurve_HeaderFile
#define _HelixGeom_BuilderApproxCurve_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Integer.hxx>
#include <Standard_Real.hxx>
#include <GeomAbs_Shape.hxx>
#include <TColGeom_SequenceOfCurve.hxx>

//! Root class for algorithm of building helix curves
class HelixGeom_BuilderApproxCurve 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Sets  aprroximation  parameters
  Standard_EXPORT void SetApproxParameters (const GeomAbs_Shape aCont, const Standard_Integer aMaxDegree, const Standard_Integer aMaxSeg);
  
  //! Gets  aprroximation  parameters
  Standard_EXPORT void ApproxParameters (GeomAbs_Shape& aCont, Standard_Integer& aMaxDegree, Standard_Integer& aMaxSeg) const;
  
  //! Sets  aprroximation  tolerance
  Standard_EXPORT void SetTolerance (const Standard_Real aTolerance);
  
  //! Gets  aprroximation  tolerance
  Standard_EXPORT Standard_Real Tolerance() const;
  
  //! Gets actual tolerance reached by  approximation  algorithm
  Standard_EXPORT Standard_Real ToleranceReached() const;
  
  //! Gets sequence of Bspline  curves  representing  helix  coins.
  Standard_EXPORT const TColGeom_SequenceOfCurve& Curves() const;
  
  //! Returns  error  status  of  algorithm
  Standard_EXPORT Standard_Integer ErrorStatus() const;
  
  //! Returns  warning  status  of  algorithm
  Standard_EXPORT Standard_Integer WarningStatus() const;
  
  //! Performs  calculations.
  //! Must  be  redefined.
  Standard_EXPORT virtual void Perform() = 0;




protected:

  
  //! Sets  default  values  of  aprroximation  parameters
  Standard_EXPORT HelixGeom_BuilderApproxCurve();
Standard_EXPORT virtual ~HelixGeom_BuilderApproxCurve();


  Standard_Integer myErrorStatus;
  Standard_Integer myWarningStatus;
  Standard_Real myTolerance;
  GeomAbs_Shape myCont;
  Standard_Integer myMaxDegree;
  Standard_Integer myMaxSeg;
  Standard_Real myTolReached;
  TColGeom_SequenceOfCurve myCurves;


private:





};







#endif // _HelixGeom_BuilderApproxCurve_HeaderFile
