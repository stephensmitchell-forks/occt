// File:	HelixGeom.cdl

#ifndef _HelixGeom_GHHelixCurve_HeaderFile
#define _HelixGeom_GHHelixCurve_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <HelixGeom_HelixCurve.hxx>
#include <Adaptor3d_HCurve.hxx>
class Standard_OutOfRange;
class Standard_NoSuchObject;
class Standard_DomainError;
class HelixGeom_HelixCurve;
class Adaptor3d_Curve;


class HelixGeom_GHHelixCurve;
DEFINE_STANDARD_HANDLE(HelixGeom_GHHelixCurve, Adaptor3d_HCurve)


class HelixGeom_GHHelixCurve : public Adaptor3d_HCurve
{

public:

  
  Standard_EXPORT HelixGeom_GHHelixCurve();
  
  Standard_EXPORT HelixGeom_GHHelixCurve(const HelixGeom_HelixCurve& C);
  
  Standard_EXPORT void Set (const HelixGeom_HelixCurve& C);
  
  Standard_EXPORT const Adaptor3d_Curve& Curve() const;
  
  Standard_EXPORT Adaptor3d_Curve& GetCurve();
  
    HelixGeom_HelixCurve& ChangeCurve();




  DEFINE_STANDARD_RTTI_INLINE(HelixGeom_GHHelixCurve,Adaptor3d_HCurve)

protected:


  HelixGeom_HelixCurve myCurve;


private:




};

#define TheCurve HelixGeom_HelixCurve
#define TheCurve_hxx <HelixGeom_HelixCurve.hxx>
#define Adaptor3d_GenHCurve HelixGeom_GHHelixCurve
#define Adaptor3d_GenHCurve_hxx <HelixGeom_GHHelixCurve.hxx>
#define Handle_Adaptor3d_GenHCurve Handle(HelixGeom_GHHelixCurve)

#include <Adaptor3d_GenHCurve.lxx>

#undef TheCurve
#undef TheCurve_hxx
#undef Adaptor3d_GenHCurve
#undef Adaptor3d_GenHCurve_hxx
#undef Handle_Adaptor3d_GenHCurve




#endif // _HelixGeom_GHHelixCurve_HeaderFile
