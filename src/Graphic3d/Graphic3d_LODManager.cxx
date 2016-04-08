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

IMPLEMENT_STANDARD_RTTIEXT (Graphic3d_LODManager, Standard_Transient)

//=======================================================================
// function : Creation
// purpose  :
//=======================================================================
Graphic3d_LODManager::Graphic3d_LODManager (const Handle(Graphic3d_Structure)& theParentStructure)
  : myLODIndexes (NULL),
    myIsToSortLODs (Standard_False),
    myCurrentLODIdx (-1),
    mySelector (new Graphic3d_LODDistanceSelector())
{
  myStructure = theParentStructure.operator->();
}

//=======================================================================
// function : Destructor
// purpose  :
//=======================================================================
Graphic3d_LODManager::~Graphic3d_LODManager()
{
  Clear (Standard_True);

  myLODs.Clear();
  myLODIndexes.Nullify();
  mySelector.Nullify();
}

//=======================================================================
// function : HasLODToDisplay
// purpose  :
//=======================================================================
Standard_Boolean Graphic3d_LODManager::HasLODToDisplay (const Handle(Graphic3d_Camera)& theCamera)
{
  if (theCamera->WorldViewProjState() == myPrevCameraState && myCurrentLODIdx != -1)
    return Standard_True;

  if (myIsToSortLODs)
    sortLODs();

  myPrevCameraState = theCamera->WorldViewProjState();
  const Standard_Real aMetric = mySelector->ComputeMetric (myStructure->CStructure(), theCamera);
  if (myLODs.FindKey (1)->GetRange().IsLess (aMetric))
  {
    myCurrentLODIdx = -1;
    return Standard_False;
  }
  else if (myLODs.FindKey (myLODs.Extent())->GetRange().IsGreater (aMetric))
  {
    myCurrentLODIdx = myLODs.Extent();
  }
  else
  {
    for (Standard_Integer aLODIdx = 1; aLODIdx <= myLODs.Extent(); ++aLODIdx)
    {
      if (myLODs.FindKey (aLODIdx)->GetRange().IsIn (aMetric))
      {
        myCurrentLODIdx = aLODIdx;
        break;
      }
    }
  }

  return Standard_True;
}

//=======================================================================
// function : SetRange
// purpose  :
//=======================================================================
void Graphic3d_LODManager::SetRange (Standard_Integer theLODIdx,
                                     const Standard_Real theFrom,
                                     const Standard_Real theTo)
{
  if (theLODIdx < 1 || theLODIdx > myLODs.Extent())
    return;

  myLODs.FindKey (theLODIdx)->SetRange (theFrom, theTo);
  myIsToSortLODs = Standard_True;
}

//=======================================================================
// function : GetRange
// purpose  :
//=======================================================================
void Graphic3d_LODManager::GetRange (const Standard_Integer theLODIdx,
                                     Standard_Real& theFrom,
                                     Standard_Real& theTo) const
{
  if (theLODIdx < 1 || theLODIdx > myLODs.Extent())
    return;

  Graphic3d_RangeOfLOD aRange = myLODs.FindKey (theLODIdx)->GetRange();
  theFrom = aRange.From();
  theTo = aRange.To();
}

//=======================================================================
// function : GetCurrentGroups
// purpose  :
//=======================================================================
const Graphic3d_SequenceOfGroup& Graphic3d_LODManager::GetCurrentGroups() const
{
  return myLODs.FindKey (myCurrentLODIdx)->GetDrawGroups();
}

//=======================================================================
// function : GetCombinedBndBox
// purpose  :
//=======================================================================
void Graphic3d_LODManager::GetCombinedBndBox (Graphic3d_BndBox4f& theBndBox) const
{
  for (Standard_Integer aLODIdx = 1; aLODIdx <= myLODs.Extent(); ++aLODIdx)
  {
    const Graphic3d_SequenceOfGroup& aGroups = myLODs.FindKey (aLODIdx)->GetDrawGroups();
    for (Graphic3d_SequenceOfGroup::Iterator aGroupIter (aGroups); aGroupIter.More(); aGroupIter.Next())
    {
      theBndBox.Combine (aGroupIter.Value()->BoundingBox());
    }
  }
}

//=======================================================================
// function : IsEmpty
// purpose  :
//=======================================================================
Standard_Boolean Graphic3d_LODManager::IsEmpty() const
{
  for (Standard_Integer aLODIdx = 1; aLODIdx <= myLODs.Extent(); ++aLODIdx)
  {
    const Graphic3d_SequenceOfGroup& aGroups = myLODs.FindKey (aLODIdx)->GetDrawGroups();
    if (!aGroups.IsEmpty())
      return Standard_False;
  }

  return Standard_True;
}

//=======================================================================
// function : Clear
// purpose  :
//=======================================================================
void Graphic3d_LODManager::Clear (const Standard_Boolean theWithDestruction)
{
  for (Standard_Integer aLODIdx = 1; aLODIdx <= myLODs.Extent(); ++aLODIdx)
  {
    myLODs.FindKey (aLODIdx)->Clear (theWithDestruction);
  }
}

//=======================================================================
// function : GetLODById
// purpose  :
//=======================================================================
const Handle(Graphic3d_LOD)& Graphic3d_LODManager::GetLODById (const Standard_Integer theLODIdx)
{
  Standard_ASSERT_RAISE (theLODIdx > 0 || theLODIdx <= myLODs.Size(),
    "Index of LOD is out of range");

  return myLODs.FindKey (theLODIdx);
}

Standard_Boolean Graphic3d_LODManager::HasLOD (const Handle(Graphic3d_LOD)& theLOD) const
{
  return myLODs.Contains (theLOD);
}
