// Copyright (c) 1994-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#ifndef _Geom_Interpolate_HeaderFile
#define _Geom_Interpolate_HeaderFile

#include <gp_Vec2d.hxx>
#include <gp_Vec.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColStd_Array1OfReal.hxx>


//! This enumeration represent boundary condition type for interpolation algorithm.
enum GeomInterpolate_BCType
{
  GeomInterpolate_CLAMPED,
  GeomInterpolate_NATURAL
};

//!  This class contains static method which are used in interpolation algorithms:
//! GeomAPI_Interpolate
//! Geom2dAPI_Interpolate
class Geom_Interpolate
{
public:

  Standard_EXPORT static gp_Vec2d ComputeLagrangeTangent2d(const TColgp_Array1OfPnt2d &thePointsArray,
                                                           const TColStd_Array1OfReal &theParametersArray,
                                                           const Standard_Integer      theIdx);

  Standard_EXPORT static gp_Vec ComputeLagrangeTangent(const TColgp_Array1OfPnt   &thePointsArray,
                                                       const TColStd_Array1OfReal &theParametersArray,
                                                       const Standard_Integer     theIdx);
};

#endif
