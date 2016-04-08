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

#include <Graphic3d_LOD.hxx>

IMPLEMENT_STANDARD_RTTIEXT (Graphic3d_LOD, Standard_Transient)

//=======================================================================
// function : Destructor
// purpose  :
//=======================================================================
Graphic3d_LOD::~Graphic3d_LOD()
{
  Clear (Standard_True);
  myGroups.Clear();
}

//=======================================================================
// function : SetRange
// purpose  :
//=======================================================================
void Graphic3d_LOD::SetRange (const Standard_Real theFrom, const Standard_Real theTo)
{
    Standard_ASSERT_RAISE (theFrom < theTo,
    "The upper boundary of the interval must be greater than lower one!");

  myRange = Graphic3d_RangeOfLOD (theFrom, theTo);
}

//=======================================================================
// function : Clear
// purpose  :
//=======================================================================
void Graphic3d_LOD::Clear (const Standard_Boolean theWithDestruction)
{
  for (Graphic3d_SequenceOfGroup::Iterator aGroupIter (myGroups); aGroupIter.More(); aGroupIter.Next())
  {
    aGroupIter.ChangeValue()->Clear();
  }

  if (!theWithDestruction)
  {
    return;
  }

  while (!myGroups.IsEmpty())
  {
    Handle(Graphic3d_Group) aGroup = myGroups.First();
    aGroup->Remove();
    myGroups.Remove (1);
  }
}
