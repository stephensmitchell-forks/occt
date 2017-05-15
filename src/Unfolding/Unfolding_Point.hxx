// File:	Unfolding_Point.cdl
// Created:	Mon Jul 28 14:56:12 2008
// Author:	Sergey KHROMOV
//		<skv@dimox>
//-Copyright:	 Matra Datavision 2008

#ifndef _Unfolding_Point_HeaderFile
#define _Unfolding_Point_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <gp_XY.hxx>
#include <gp_XYZ.hxx>
#include <Standard_Real.hxx>
class gp_XYZ;
class gp_XY;


//! This class represents a data container for a point. It contains
//! a point  on a  surface, its  U and V  parameters on  a surface,
//! corresponding point on an unfolding plane.
class Unfolding_Point 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Empty constructor.
  Standard_EXPORT Unfolding_Point();
  
  //! Sets the point on a surface.
    void SetPointOnSurface (const gp_XYZ& thePOnSurface);
  
  //! Returns the point on a surface.
    const gp_XYZ& GetPointOnSurface() const;
  
  //! Sets the U and V parameters of the point on a surface.
    void SetParameters (const Standard_Real theU, const Standard_Real theV);
  
  //! Returns the U and V parameters of the point on a surface.
    void GetParameters (Standard_Real& theU, Standard_Real& theV) const;
  
  //! Returns the U parameter of the point on a surface.
    Standard_Real GetU() const;
  
  //! Returns the V parameter of the point on a surface.
    Standard_Real GetV() const;
  
  //! Sets the angle between DU and DV directions.
    void SetAngle (const Standard_Real theAngle);
  
  //! Returns the angle between DU and DV directions.
    Standard_Real GetAngle() const;
  
  //! Sets the point on an unfolding plane.
    void SetPointOnPlane (const gp_XY& thePOnPlane);
  
  //! Returns the point on an unfolding plane.
    const gp_XY& GetPointOnPlane() const;




protected:





private:



  gp_XY myPoint2d;
  gp_XYZ myPOnSurface;
  gp_XY myPOnPlane;
  Standard_Real myAngle;


};


#include <Unfolding_Point.lxx>





#endif // _Unfolding_Point_HeaderFile
