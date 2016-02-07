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

#include <Geom_Interpolate.hxx>

//=======================================================================
//function : computeLagrangeTangent2d
//purpose  : compute tangent of 3-point template in middle point
//           using Lagrange polynomial.
//=======================================================================
gp_Vec2d Geom_Interpolate::ComputeLagrangeTangent2d(const TColgp_Array1OfPnt2d&      thePointsArray,
                                                    const TColStd_Array1OfReal&    theParametersArray,
                                                    const Standard_Integer theIdx)
{
    gp_Vec2d aCurTangent(0.0, 0.0);

    if (theIdx == thePointsArray.Lower() ||
        theIdx == thePointsArray.Upper())
        return aCurTangent;

    // Compute tangent in the second point.
    Standard_Real t0 = theParametersArray(theIdx - 1);
    Standard_Real t1 = theParametersArray(theIdx);
    Standard_Real t2 = theParametersArray(theIdx + 1);
    gp_Pnt2d p0 = thePointsArray(theIdx - 1);
    gp_Pnt2d p1 = thePointsArray(theIdx);
    gp_Pnt2d p2 = thePointsArray(theIdx + 1);
    aCurTangent.SetXY(p0.XY() * (t1-t2)/((t0-t1)*(t0-t2))
                    + p1.XY() * (2*t1-t0-t2)/((t1-t0)*(t1-t2))
                    + p2.XY() * (t1-t0)/((t2-t0)*(t2-t1)));

    return aCurTangent;
}

//=======================================================================
//function : computeLagrangeTangent
//purpose  : compute tangent of 3-point template in middle point
//           using Lagrange polynomial.
//=======================================================================
gp_Vec Geom_Interpolate::ComputeLagrangeTangent(const TColgp_Array1OfPnt&      thePointsArray,
                                                const TColStd_Array1OfReal&    theParametersArray,
                                                const Standard_Integer theIdx)
{
    gp_Vec aCurTangent(0.0, 0.0, 0.0);

    if (theIdx == thePointsArray.Lower() ||
        theIdx == thePointsArray.Upper())
        return aCurTangent;

    // Compute tangent in the second point.
    Standard_Real t0 = theParametersArray(theIdx - 1);
    Standard_Real t1 = theParametersArray(theIdx);
    Standard_Real t2 = theParametersArray(theIdx + 1);
    gp_Pnt p0 = thePointsArray(theIdx - 1);
    gp_Pnt p1 = thePointsArray(theIdx);
    gp_Pnt p2 = thePointsArray(theIdx + 1);
    aCurTangent.SetXYZ(p0.XYZ() * (t1-t2)/((t0-t1)*(t0-t2))
                     + p1.XYZ() * (2*t1-t0-t2)/((t1-t0)*(t1-t2))
                     + p2.XYZ() * (t1-t0)/((t2-t0)*(t2-t1)));

    return aCurTangent;
}
