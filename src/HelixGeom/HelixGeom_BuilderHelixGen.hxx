// File:	HelixGeom_BuilderHelixGen.cdl

#ifndef _HelixGeom_BuilderHelixGen_HeaderFile
#define _HelixGeom_BuilderHelixGen_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Real.hxx>
#include <Standard_Boolean.hxx>
#include <HelixGeom_BuilderApproxCurve.hxx>


//! Root  class  for  algorithms  of  building  helix  curves
class HelixGeom_BuilderHelixGen  : public HelixGeom_BuilderApproxCurve
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Sets  parameters  for  building  helix  curves
  Standard_EXPORT void SetCurveParameters (const Standard_Real aT1, const Standard_Real aT2, const Standard_Real aPitch, const Standard_Real aRStart, const Standard_Real aTaperAngle, const Standard_Boolean bIsClockwise);
  
  //! Gets  parameters  for  building  helix  curves
  Standard_EXPORT void CurveParameters (Standard_Real& aT1, Standard_Real& aT2, Standard_Real& aPitch, Standard_Real& aRStart, Standard_Real& aTaperAngle, Standard_Boolean& bIsClockwise) const;




protected:

  
  //! Sets  default  parameters
  Standard_EXPORT HelixGeom_BuilderHelixGen();
Standard_EXPORT virtual ~HelixGeom_BuilderHelixGen();


  Standard_Real myT1;
  Standard_Real myT2;
  Standard_Real myPitch;
  Standard_Real myRStart;
  Standard_Real myTaperAngle;
  Standard_Boolean myIsClockWise;


private:





};







#endif // _HelixGeom_BuilderHelixGen_HeaderFile
