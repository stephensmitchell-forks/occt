// Created by: Julia GERASIMOVA
// Copyright (c) 2015 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#ifndef _BlendFunc_GenChamfInv_HeaderFile
#define _BlendFunc_GenChamfInv_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>
#include <BlendFunc_Corde.hxx>
#include <Blend_FuncInv.hxx>
#include <math_Vector.hxx>
#include <Standard_Real.hxx>
class Adaptor3d_HSurface;
class Adaptor3d_HCurve;
class Adaptor2d_HCurve2d;
class math_Matrix;



//! Deferred class for a function used to compute a general chamfer on a surface's boundary
class BlendFunc_GenChamfInv  : public Blend_FuncInv
{
public:

  DEFINE_STANDARD_ALLOC

  Standard_EXPORT BlendFunc_GenChamfInv(const Handle(Adaptor3d_HSurface)& S1, const Handle(Adaptor3d_HSurface)& S2, const Handle(Adaptor3d_HCurve)& C);
  
  Standard_EXPORT void Set (const Standard_Boolean OnFirst, const Handle(Adaptor2d_HCurve2d)& COnSurf) Standard_OVERRIDE;
  
  Standard_EXPORT void GetTolerance (math_Vector& Tolerance, const Standard_Real Tol) const Standard_OVERRIDE;
  
  Standard_EXPORT void GetBounds (math_Vector& InfBound, math_Vector& SupBound) const Standard_OVERRIDE;
  
  Standard_EXPORT Standard_Boolean IsSolution (const math_Vector& Sol, const Standard_Real Tol) Standard_OVERRIDE = 0;
  
  //! returns the number of equations of the function.
  Standard_EXPORT Standard_Integer NbEquations() const Standard_OVERRIDE;
  
  //! computes the values <F> of the Functions for the
  //! variable <X>.
  //! Returns True if the computation was done successfully,
  //! False otherwise.
  Standard_EXPORT Standard_Boolean Value (const math_Vector& X, math_Vector& F) Standard_OVERRIDE = 0;
  
  //! returns the values <D> of the derivatives for the
  //! variable <X>.
  //! Returns True if the computation was done successfully,
  //! False otherwise.
  Standard_EXPORT Standard_Boolean Derivatives (const math_Vector& X, math_Matrix& D) Standard_OVERRIDE = 0;
  
  //! returns the values <F> of the functions and the derivatives
  //! <D> for the variable <X>.
  //! Returns True if the computation was done successfully,
  //! False otherwise.
  Standard_EXPORT Standard_Boolean Values (const math_Vector& X, math_Vector& F, math_Matrix& D) Standard_OVERRIDE;
  
  Standard_EXPORT virtual void Set (const Standard_Real Dist1,
                                    const Standard_Real Dist2,
                                    const Standard_Integer Choix) = 0;




protected:

  Handle(Adaptor3d_HSurface) surf1;
  Handle(Adaptor3d_HSurface) surf2;
  Handle(Adaptor3d_HCurve) curv;
  Handle(Adaptor2d_HCurve2d) csurf;
  Standard_Integer choix;
  Standard_Boolean first;


private:

  
};







#endif // _BlendFunc_GenChamfInv_HeaderFile
