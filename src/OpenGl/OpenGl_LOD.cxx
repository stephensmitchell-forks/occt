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

#include <OpenGl_LOD.hxx>

//=======================================================================
// function : Construction
// purpose  :
//=======================================================================
OpenGl_LOD::OpenGl_LOD()
: Graphic3d_LOD (),
  myRange (-DBL_MAX, DBL_MAX) {}

//=======================================================================
// function : SetRange
// purpose  :
//=======================================================================
void OpenGl_LOD::SetRange (const Standard_Real theFrom, const Standard_Real theTo)
{
  Standard_ASSERT_RAISE (theFrom < theTo,
    "The upper boundary of the interval must be greater than lower one!");

  myRange = OpenGl_RangeOfLOD (theFrom, theTo);
}

//=======================================================================
// function : NewGroup
// purpose  :
//=======================================================================
Handle(Graphic3d_Group) OpenGl_LOD::NewGroup (const Handle(Graphic3d_Structure)& theParentStruct)
{
  Handle(OpenGl_Group) aGroup = new OpenGl_Group (theParentStruct);
  myGroups.Append (aGroup);
  return aGroup;
}
