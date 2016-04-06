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
#include <OpenGl_LOD.hxx>
#include <OpenGl_LODManager.hxx>

#include <algorithm>

IMPLEMENT_STANDARD_RTTIEXT (OpenGl_LODManager, Graphic3d_LODManager)

namespace
{
  // Comparison operator for sorting LODs
  class CompareLODS
  {
  public:
   
    CompareLODS (const Graphic3d_MapOfLODs& theLODs)
      : myLODs (theLODs) {}

    Standard_Boolean operator() (const Standard_Integer theLeftIdx, const Standard_Integer theRightIdx) const
    {
      return myLODs.FindKey (theLeftIdx)->GetRange() < myLODs.FindKey (theRightIdx)->GetRange();
    }

  private:
    void operator = (const CompareLODS&);

  private:
    const Graphic3d_MapOfLODs& myLODs;
  };
}

//=======================================================================
// function : Creation
// purpose  :
//=======================================================================
OpenGl_LODManager::OpenGl_LODManager (const Handle(Graphic3d_Structure)& theParentStructure)
  : Graphic3d_LODManager (theParentStructure)
{
  Standard_ASSERT_RAISE (!theParentStructure.IsNull(),
    "Parent structure of LOD manager is null!");
}

//=======================================================================
// function : AddNewLOD
// purpose  :
//=======================================================================
Handle(Graphic3d_LOD) OpenGl_LODManager::AddNewLOD()
{
  Handle(Graphic3d_LOD) aNewLOD = new OpenGl_LOD();
  myLODs.Add (aNewLOD);
  myIsToSortLods = Standard_True;
  return aNewLOD;
}

//=======================================================================
// function : sortLODs
// purpose  :
//=======================================================================
void OpenGl_LODManager::sortLODs()
{
  Standard_Integer aIndArrSize = myLODs.Extent();
  myLodIndexes.Nullify();
  myLodIndexes = new TColStd_HArray1OfInteger (0, aIndArrSize - 1);
  for (Standard_Integer aIdx = 0; aIdx < aIndArrSize; ++aIdx)
  {
    myLodIndexes->SetValue (aIdx, aIdx + 1);
  }
  TColStd_Array1OfInteger& anIdxArr = myLodIndexes->ChangeArray1();
  std::sort (anIdxArr.begin(), anIdxArr.end(), CompareLODS (myLODs));
  myIsToSortLods = Standard_False;
}
