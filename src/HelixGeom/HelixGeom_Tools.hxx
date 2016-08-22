// File:	HelixGeom_Tools.cdl

#ifndef _HelixGeom_Tools_HeaderFile
#define _HelixGeom_Tools_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Integer.hxx>
#include <Standard_Real.hxx>
#include <Standard_Boolean.hxx>
#include <GeomAbs_Shape.hxx>
class Geom_BSplineCurve;
class Adaptor3d_HCurve;


//! Approximation  algorithms  for  bulding  helix  curves
class HelixGeom_Tools 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Bulding  helix  curves
  Standard_EXPORT static Standard_Integer ApprHelix (const Standard_Real aT1, const Standard_Real aT2, const Standard_Real aPitch, const Standard_Real aRStart, const Standard_Real aTaperAngle, const Standard_Boolean aIsCW, const Standard_Real aTol, Handle(Geom_BSplineCurve)& theBSpl, Standard_Real& theMaxError);
  
  //! Reaprroximation  of  adaptor  curve
  Standard_EXPORT static Standard_Integer ApprCurve3D (Handle(Adaptor3d_HCurve)& theHC, const Standard_Real theTol, const GeomAbs_Shape theCont, const Standard_Integer theMaxSeg, const Standard_Integer theMaxDeg, Handle(Geom_BSplineCurve)& theBSpl, Standard_Real& theMaxError);




protected:





private:





};







#endif // _HelixGeom_Tools_HeaderFile
