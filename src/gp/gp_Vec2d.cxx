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

#define No_Standard_OutOfRange


#include <gp.hxx>
#include <gp_Ax2d.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Trsf2d.hxx>
#include <gp_Vec2d.hxx>
#include <gp_VectorWithNullMagnitude.hxx>
#include <gp_XY.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_OutOfRange.hxx>

Standard_Boolean gp_Vec2d::IsEqual( const gp_Vec2d& theOther, 
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

//=======================================================================
//function : Angle 
//purpose  : Computes value of the angle between two vectors. Returns
//            the number in [-PI, +PI] range.
//=======================================================================
Standard_Real gp_Vec2d::Angle (const gp_Vec2d& theOther) const
{
  const Standard_Real aNormProduct = sqrt(SquareMagnitude() * theOther.SquareMagnitude());
  if (aNormProduct <= gp::Resolution())
    gp_VectorWithNullMagnitude::Raise();

  //Cosine and sine of Angle
  const Standard_Real aCosinus = coord.Dot   (theOther.coord) / aNormProduct;
  const Standard_Real aSinus = coord.Crossed (theOther.coord) / aNormProduct;

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

void gp_Vec2d::Mirror (const gp_Ax2d& A1)
{
  const gp_XY& XY = A1.Direction().XY();
  Standard_Real X = coord.X();
  Standard_Real Y = coord.Y();
  Standard_Real A = XY.X();
  Standard_Real B = XY.Y();
  Standard_Real M1 = 2.0 * A * B;
  coord.SetX(((2.0 * A * A) - 1.) * X + M1 * Y);
  coord.SetY(M1 * X + ((2. * B * B) - 1.0) * Y);
}

gp_Vec2d gp_Vec2d::Mirrored (const gp_Ax2d& A1) const
{
  gp_Vec2d Vres = *this;
  Vres.Mirror(A1);
  return Vres;                     
}

void gp_Vec2d::Transform (const gp_Trsf2d& T)
{
  if (T.Form() == gp_Identity || T.Form() == gp_Translation) { }
  else if (T.Form() == gp_PntMirror) coord.Reverse ();
  else if (T.Form() == gp_Scale)     coord.Multiply (T.ScaleFactor ());
  else                               coord.Multiply (T.VectorialPart ());
}

void gp_Vec2d::Mirror (const gp_Vec2d& V)
{
  const Standard_Real D = V.coord.Modulus();
  if (D > gp::Resolution())
  {
    const gp_XY& XY = V.coord;
    Standard_Real X = XY.X();
    Standard_Real Y = XY.Y();
    Standard_Real A = X / D;
    Standard_Real B = Y / D;
    Standard_Real M1 = 2.0 * A * B;
    coord.SetX(((2.0 * A * A) - 1.0) * X + M1 * Y);
    coord.SetY(M1 * X + ((2.0 * B * B) - 1.0) * Y);
  }
}

gp_Vec2d gp_Vec2d::Mirrored (const gp_Vec2d& V) const
{
  gp_Vec2d Vres = *this;
  Vres.Mirror(V);
  return Vres;                     
}
