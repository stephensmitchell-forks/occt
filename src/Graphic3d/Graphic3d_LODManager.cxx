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

#include <Graphic3d_LOD.hxx>
#include <Graphic3d_LODManager.hxx>

//=======================================================================
// function : Creation
// purpose  :
//=======================================================================
Graphic3d_LODManager::Graphic3d_LODManager (const Handle(Graphic3d_Structure)& theParentStructure)
  : myCurrentLODIdx (-1),
    mySelector (new Graphic3d_LODDistanceSelector())
{
  myStructure = theParentStructure.operator->();
}

//=======================================================================
// function : GetCurrentLODIdx
// purpose  :
//=======================================================================
Standard_Integer Graphic3d_LODManager::GetCurrentLODIdx (const Handle(Graphic3d_Camera)& theCamera)
{
  if (theCamera->WorldViewProjState() == myPrevCameraState && myCurrentLODIdx != -1)
    return myCurrentLODIdx;

  myPrevCameraState = theCamera->WorldViewProjState();
  const Standard_Real aMetric = mySelector->ComputeMetric (myStructure->CStructure(), theCamera);
  if (myLODs.Value (0)->GetRange().IsLess (aMetric))
    return -1;

  for (Standard_Integer aLodIdx = 1; aLodIdx < myLODs.Size(); ++aLodIdx)
  {
    if (myLODs.Value (aLodIdx)->GetRange().IsIn (aMetric))
    {
      myCurrentLODIdx = aLodIdx;
      break;
    }
  }

  if (myLODs.Value (myLODs.Size() - 1)->GetRange().IsGreater (aMetric))
    myCurrentLODIdx = myLODs.Size() - 1;

  return myCurrentLODIdx;
}

//=======================================================================
// function : SetRange
// purpose  :
//=======================================================================
void Graphic3d_LODManager::SetRange (const Standard_Integer theLodIdx,
                                     const Standard_Real theFrom,
                                     const Standard_Real theTo)
{
  myLODs.ChangeValue (theLodIdx)->SetRange (theFrom, theTo);
}

//=======================================================================
// function : GetCurrentGroups
// purpose  :
//=======================================================================
 const Graphic3d_SequenceOfGroup& Graphic3d_LODManager::GetCurrentGroups() const
 {
   return myLODs.Value (myCurrentLODIdx)->GetDrawGroups();
 }
