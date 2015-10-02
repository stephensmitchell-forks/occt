// Copyright (c) 1995-1999 Matra Datavision
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

// JCV 08/01/90 Modifs suite a l'introduction des classes XY et Mat2d dans gp

#include <gp_Ax2d.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Trsf2d.hxx>
#include <gp_Vec2d.hxx>
#include <gp_XY.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_OutOfRange.hxx>

Standard_Real gp_Dir2d::Angle (const gp_Dir2d& theOther) const
{
  //Cosine and sine of Angle
  const Standard_Real aCosinus = coord.Dot   (theOther.coord);
  const Standard_Real aSinus = coord.Crossed (theOther.coord);

  //  According to Thaylor series expanding,
  //      \arccos (x_{0}+\Delta x) \approx \arccos x_{0} - \frac{\Delta x}{\sqrt{1-x_{0}^{2}}},
  //      \arcsin (x_{0}+\Delta x) \approx \arcsin x_{0} + \frac{\Delta x}{\sqrt{1-x_{0}^{2}}},

  //where @{ x_{0} \in \left [ 0,1 \right ] }@.

  //  In order to compute @{ \arccos x }@ (where @{ x \cong 1.0 }@) there are two ways:
  //1. To take reference point (@{ x_{0} }@) quite far from 1 (e.g. @{ x_{0} = 0.0 }@).
  //    It results in using too many series member.
  //2. To take reference point (@{ x_{0} \cong 1 }@). In this case,
  //      \sqrt {\frac{1}{1-x_{0}^{2}}}\rightarrow \infty.
  //Therefore, it will require to use too many series member, too.

  //  Consequently, it is difficult to compute precise value of
  //@{ \arccos x }@ and @{ \arcsin x }@ function, if @{ x \cong 1 }@.

  //  However, if @{ \sin \alpha \cong 1 }@ then @{ \cos \alpha \cong 0 }@
  //(and vice versa). In this case Angle value is better to be computed as
  //"arccos" function (from known value).

  if ((-M_SQRT1_2 < aCosinus) && (aCosinus < M_SQRT1_2))
    return Sign(acos(aCosinus), aSinus);

  const Standard_Real anArcSin = asin(aSinus);
  if(aCosinus < 0.0)
    return (Sign(M_PI, aSinus) - anArcSin);

  return anArcSin;
}

void gp_Dir2d::Mirror (const gp_Ax2d& A2)
{
  const gp_XY& XY = A2.Direction().XY();
  Standard_Real A = XY.X();
  Standard_Real B = XY.Y();
  Standard_Real X = coord.X();
  Standard_Real Y = coord.Y();
  Standard_Real M1 = 2.0 * A * B;
  Standard_Real XX = ((2.0 * A * A) - 1.0) * X + M1 * Y;
  Standard_Real YY = M1 * X + ((2.0 * B * B) - 1.0) * Y;
  coord.SetCoord(XX,YY);
}

void gp_Dir2d::Transform (const gp_Trsf2d& T)
{
  if (T.Form() == gp_Identity || T.Form() == gp_Translation)    { }
  else if (T.Form() == gp_PntMirror) { coord.Reverse(); }
  else if (T.Form() == gp_Scale) {
    if (T.ScaleFactor() < 0.0) { coord.Reverse(); }
  }
  else {
    //Apply transformation, set by the homogeneous vectorial part of T
    coord.Multiply (T.HVectorialPart());

    //After transformation, earlier normalized vector "coord" will be able to
    //be not normalized. Therefore, we must normalize it again.
    Standard_Real D = coord.Modulus();
    coord.Divide(D);
    if (T.ScaleFactor() < 0.0) { coord.Reverse(); }
  } 
}

void gp_Dir2d::Mirror (const gp_Dir2d& V)
{
  const gp_XY& XY = V.coord;
  Standard_Real A = XY.X();
  Standard_Real B = XY.Y();
  Standard_Real X = coord.X();
  Standard_Real Y = coord.Y();
  Standard_Real M1 = 2.0 * A * B;
  Standard_Real XX = ((2.0 * A * A) - 1.0) * X + M1 * Y;
  Standard_Real YY = M1 * X + ((2.0 * B * B) - 1.0) * Y;
  coord.SetCoord(XX,YY);
}

gp_Dir2d gp_Dir2d::Mirrored (const gp_Dir2d& V) const
{
  gp_Dir2d Vres = *this;
  Vres.Mirror (V);
  return Vres;
}

gp_Dir2d gp_Dir2d::Mirrored (const gp_Ax2d& A) const
{
  gp_Dir2d V = *this;
  V.Mirror (A);
  return V;
}

