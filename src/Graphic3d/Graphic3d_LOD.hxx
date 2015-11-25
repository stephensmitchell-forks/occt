// Created on: 2015-10-29
// Created by: Varvara POSKONINA
// Copyright (c) 2005-2014 OPEN CASCADE SAS
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

#ifndef _Graphic3d_LOD_Header
#define _Graphic3d_LOD_Header

#include <Graphic3d_Camera.hxx>
#include <Graphic3d_SequenceOfGroup.hxx>
#include <Graphic3d_Structure.hxx>
#include <Standard.hxx>
#include <Standard_Macro.hxx>
#include <Standard_Transient.hxx>

struct Graphic3d_RangeOfLOD
{
public:
  Graphic3d_RangeOfLOD (const Standard_Real theFrom, const Standard_Real theTo)
    : myTo (theTo),
      myFrom (theFrom)
  {
    Standard_ASSERT_RAISE (theFrom < theTo,
      "The upper boundary of the interval must be greater than lower one!");
  }

  Standard_Boolean IsIn (const Standard_Real theVal) const
  {
    return (myFrom < theVal) && (theVal < myTo);
  }

  Standard_Boolean IsLess (const Standard_Real theVal) const
  {
    return myFrom < theVal;
  }

  Standard_Boolean IsGreater (const Standard_Real theVal) const
  {
    return myTo < theVal;
  }

  bool operator < (const Graphic3d_RangeOfLOD& theOther) const
  {
    return myFrom < theOther.myFrom;
  }

private:
  Standard_Real myFrom;
  Standard_Real myTo;
};

class Graphic3d_LOD : public Standard_Transient
{
public:
  Standard_EXPORT virtual ~Graphic3d_LOD();

  Standard_EXPORT void SetRange (const Standard_Real theFrom, const Standard_Real theTo);

  Standard_EXPORT const Graphic3d_RangeOfLOD& GetRange() const
  {
    return myRange;
  }

  Standard_EXPORT virtual Handle(Graphic3d_Group) NewGroup (const Handle(Graphic3d_Structure)& /*theParentStruct*/)
  {
    return NULL;
  };

  const Graphic3d_SequenceOfGroup& GetDrawGroups() const
  {
    return myGroups;
  }

  DEFINE_STANDARD_RTTI (Graphic3d_LOD, Standard_Transient)

protected:
  Standard_EXPORT Graphic3d_LOD() : myRange (-DBL_MAX, DBL_MAX) {};

protected:
  Graphic3d_SequenceOfGroup myGroups;
  Graphic3d_RangeOfLOD      myRange;
};

DEFINE_STANDARD_HANDLE (Graphic3d_LOD, Standard_Transient)

#endif // _Graphic3d_LOD_Header
