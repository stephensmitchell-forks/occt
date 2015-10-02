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

// JCV 30/08/90 Modif passage version C++ 2.0 sur Sun
// JCV 1/10/90 Changement de nom du package vgeom -> gp
// JCV 07/12/90 Modifs suite a l'introduction des classes XYZ et Mat dans gp

#define No_Standard_OutOfRange


#include <gp.hxx>
#include <gp_Ax1.hxx>
#include <gp_Ax2.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>
#include <gp_VectorWithNullMagnitude.hxx>
#include <gp_XYZ.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_OutOfRange.hxx>

Standard_Boolean gp_Vec::IsEqual( const gp_Vec& theOther,
                                  const Standard_Real theLinearTolerance,
                                  const Standard_Real theAngularTolerance) const
{
  const Standard_Real aSqLinTol = theLinearTolerance*theLinearTolerance;
  const Standard_Real aMySqNorm = SquareMagnitude();
  const Standard_Real aOtherSqNorm = theOther.SquareMagnitude();

  const Standard_Boolean  aMyCond = (aMySqNorm < aSqLinTol),
                          aOCond = (aOtherSqNorm < aSqLinTol);

  if(aMyCond && aOCond)
    return Standard_True;

  if(!aMyCond && !aOCond)
  {
    const Standard_Boolean aCond = Abs(sqrt(aMySqNorm)-sqrt(aOtherSqNorm)) < theLinearTolerance;
    return aCond && IsCoincide(theOther, theAngularTolerance);
  }

  return Standard_False;
}

Standard_Real gp_Vec::Angle (const gp_Vec& theOther) const
{
  const Standard_Real aNormProduct = sqrt(SquareMagnitude() * theOther.SquareMagnitude());
  if (aNormProduct <= gp::Resolution())
    gp_VectorWithNullMagnitude::Raise();

  //Cosine and sine of Angle
  const Standard_Real aCosinus = coord.Dot   (theOther.coord) / aNormProduct;
  const Standard_Real aSinus = coord.Crossed (theOther.coord).Modulus() / aNormProduct;

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

void gp_Vec::Mirror (const gp_Vec& V)
{
  Standard_Real D = V.coord.Modulus();
  if (D > gp::Resolution()) {
    const gp_XYZ& XYZ = V.coord;
    Standard_Real A = XYZ.X() / D;
    Standard_Real B = XYZ.Y() / D;
    Standard_Real C = XYZ.Z() / D; 
    Standard_Real M1 = 2.0 * A * B;
    Standard_Real M2 = 2.0 * A * C;
    Standard_Real M3 = 2.0 * B * C;
    Standard_Real X = coord.X();
    Standard_Real Y = coord.Y();
    Standard_Real Z = coord.Z();
    coord.SetX(((2.0 * A * A) - 1.0) * X + M1 * Y + M2 * Z);
    coord.SetY(M1 * X + ((2.0 * B * B) - 1.0) * Y + M3 * Z);
    coord.SetZ(M2 * X + M3 * Y + ((2.0 * C * C) - 1.0) * Z);
  }
}

void gp_Vec::Mirror (const gp_Ax1& A1)
{
  const gp_XYZ& V = A1.Direction().XYZ();
  Standard_Real A = V.X();
  Standard_Real B = V.Y();
  Standard_Real C = V.Z();
  Standard_Real X = coord.X();
  Standard_Real Y = coord.Y();
  Standard_Real Z = coord.Z();
  Standard_Real M1 = 2.0 * A * B;
  Standard_Real M2 = 2.0 * A * C;
  Standard_Real M3 = 2.0 * B * C;
  coord.SetX(((2.0 * A * A) - 1.0) * X + M1 * Y + M2 * Z);
  coord.SetY(M1 * X + ((2.0 * B * B) - 1.0) * Y + M3 * Z);
  coord.SetZ(M2 * X + M3 * Y + ((2.0 * C * C) - 1.0) * Z);
}

void gp_Vec::Mirror (const gp_Ax2& A2)
{
  gp_XYZ Z      = A2.Direction().XYZ();
  gp_XYZ MirXYZ = Z.Crossed (coord);
  if (MirXYZ.Modulus() <= gp::Resolution()) { coord.Reverse(); }
  else {
    Z.Cross (MirXYZ);
    Mirror (Z);
  }
}

void gp_Vec::Transform(const gp_Trsf& T)
{
  if (T.Form() == gp_Identity || T.Form() == gp_Translation) { }
  else if (T.Form() == gp_PntMirror) { coord.Reverse(); }
  else if (T.Form() == gp_Scale) { coord.Multiply (T.ScaleFactor()); }
  else { coord.Multiply (T.VectorialPart()); }
}

gp_Vec gp_Vec::Mirrored (const gp_Vec& V) const
{
  gp_Vec Vres = *this;
  Vres.Mirror (V);
  return Vres;                     
}

gp_Vec gp_Vec::Mirrored (const gp_Ax1& A1) const
{
  gp_Vec Vres = *this;
  Vres.Mirror (A1);
  return Vres;                     
}

gp_Vec gp_Vec::Mirrored (const gp_Ax2& A2) const
{
  gp_Vec Vres = *this;
  Vres.Mirror (A2);
  return Vres;                     
}
