// File:      Handle_Poly_PolyCurve2d.hxx
// Created:   25.07.10 10:47
// Author:    Alexander GRIGORIEV
// Copyright: Open Cascade 2010


#ifndef Handle_Poly_PolyCurve2d_HeaderFile
#define Handle_Poly_PolyCurve2d_HeaderFile

#include <Standard_DefineHandle.hxx>
#include <Standard_Transient.hxx>

class Poly_PolyCurve2d;

/**
 * Definition of HANDLE object using Standard_DefineHandle.hxx
 */
DEFINE_STANDARD_HANDLE (Poly_PolyCurve2d, Standard_Transient)

inline Standard_Boolean IsEqual(const Handle(Poly_PolyCurve2d)& theCurve1,
                                const Handle(Poly_PolyCurve2d)& theCurve2)
{
  return theCurve1 == theCurve2;
}

#endif
