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

//=======================================================================
// function : Destructor
// purpose  :
//=======================================================================
Graphic3d_LOD::~Graphic3d_LOD()
{
  myGroups.Clear();
  myParent.Nullify();
}

//=======================================================================
// function : SetParent
// purpose  :
//=======================================================================
void Graphic3d_LOD::SetParent (const Handle(Graphic3d_Structure)& theParent)
{
  myParent = theParent;
}

//=======================================================================
// function : SetRange
// purpose  :
//=======================================================================
void Graphic3d_LOD::SetRange (const Standard_Real /*theFrom*/, const Standard_Real /*theTo*/)
{
  return;
}

//=======================================================================
// function : NewGroup
// purpose  :
//=======================================================================
Handle(Graphic3d_Group) Graphic3d_LOD::NewGroup()
{
  return NULL;
}

//=======================================================================
// function : ComputeMetrics
// purpose  :
//=======================================================================
Standard_Real Graphic3d_LOD::ComputeMetrics (const Handle(Graphic3d_Camera)& theCamera) const
{
  if (myParent.IsNull())
    return std::numeric_limits<Standard_Real>::max();

  Graphic3d_BndBox4f aBndBox = myParent->CStructure()->BoundingBox();
  const Graphic3d_Vec4 aCenter = aBndBox.Center();
  const gp_Pnt aGpCenter = gp_Pnt (aCenter.x(), aCenter.y(), aCenter.z());
  return (theCamera->Eye().Distance (aGpCenter)) / theCamera->Scale();
}
