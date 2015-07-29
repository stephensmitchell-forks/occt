// Created on: 1993-03-08
// Created by: Remi LEQUETTE
// Copyright (c) 1993-1999 Matra Datavision
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


#include <Precision.hxx>

//Binary representation of double numbers: expands it to MANTISSA, EXPONENT and SIGN-bit.
struct stDBLType
{
  unsigned int manl:32;
  unsigned int manh:20;
  unsigned int exp:11;
  unsigned int sign:1;
};


//=======================================================================
//function : IsIllegal
//purpose  : Returns True if R is infinite, NAN or another number
//            which is not supported by OCCT.
//            Fragment of fpclassify(...) function is used, which is taken
//            from here: http://www.jbox.dk/sanos/source/lib/math.c.html
//=======================================================================
Standard_Boolean Precision::IsIllegal (const Standard_Real R)
{
  stDBLType* aDoubleBits = (struct stDBLType*) &R;
  if (aDoubleBits->exp == 0)
  {
    if (aDoubleBits->manh != 0 || aDoubleBits->manl != 0)
    {//_FPCLASS_ND (-DBL_MIN/2.0) or _FPCLASS_PD (+DBL_MIN/2.0)
      return Standard_True;
    }
  }

  if (aDoubleBits->exp == 0x7ff)
  {//_FPCLASS_NINF (-1/0), _FPCLASS_PINF (+1/0), _FPCLASS_QNAN or _FPCLASS_SNAN
    return Standard_True;
  }

  const Standard_Real anINf = 2.0*Infinite();

  if(R >= anINf)
    return Standard_True;

  if(R <= -anINf)
    return Standard_True;

  return Standard_False;
}
