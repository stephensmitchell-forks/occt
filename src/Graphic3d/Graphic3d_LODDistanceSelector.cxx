// Created on: 2015-11-25
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

#include <Graphic3d_BndBox4f.hxx>
#include <Graphic3d_Camera.hxx>
#include <Graphic3d_CStructure.hxx>
#include <Graphic3d_LODDistanceSelector.hxx>

//=======================================================================
// function : ComputeMetric
// purpose  :
//=======================================================================
Standard_Real Graphic3d_LODDistanceSelector::ComputeMetric (const Handle(Graphic3d_CStructure)& theParentStructure,
                                                            const Handle(Graphic3d_Camera)& theCamera)
{
  const Graphic3d_BndBox4f& aBndBox = theParentStructure->BoundingBox();
  const Graphic3d_Vec4 aCenter = aBndBox.Center();
  const gp_Pnt aGpCenter = gp_Pnt (aCenter.x(), aCenter.y(), aCenter.z());
  return (theCamera->Eye().Distance (aGpCenter)) / theCamera->Scale();
}
