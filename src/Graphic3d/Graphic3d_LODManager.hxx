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

#ifndef _Graphic3d_LODManager_Header
#define _Graphic3d_LODManager_Header

#include <Graphic3d_CStructure.hxx>
#include <Graphic3d_Camera.hxx>
#include <Graphic3d_LOD.hxx>
#include <Graphic3d_LODSelector.hxx>
#include <Graphic3d_LODDistanceSelector.hxx>
#include <Graphic3d_SequenceOfGroup.hxx>
#include <Graphic3d_StructurePtr.hxx>
#include <NCollection_Vector.hxx>
#include <Standard.hxx>
#include <Standard_Handle.hxx>

class Graphic3d_LODManager : public Standard_Transient
{
public:

  Standard_EXPORT virtual ~Graphic3d_LODManager() {};

  Standard_EXPORT void SetSelector (const Handle(Graphic3d_LODSelector)& theSelector)
  {
    mySelector = theSelector;
  }

  Standard_EXPORT inline Standard_Integer NbOfDetailLevels() const
  {
    return myLODs.Size();
  }

  Standard_EXPORT void SetRange (const Standard_Integer theLodIdx,
                                 const Standard_Real theFrom,
                                 const Standard_Real theTo);

  Standard_EXPORT Standard_Integer GetCurrentLODIdx (const Handle(Graphic3d_Camera)& theCamera);

  Standard_EXPORT const Graphic3d_SequenceOfGroup& GetCurrentGroups() const;

  Standard_EXPORT virtual Handle(Graphic3d_LOD) AddNewLOD() = 0;

  Standard_EXPORT void GetCombinedBndBox (Graphic3d_BndBox4f& theBndBox) const;

  DEFINE_STANDARD_RTTIEXT (Graphic3d_LODManager, Standard_Transient)

protected:
  Standard_EXPORT Graphic3d_LODManager (const Handle(Graphic3d_Structure)& theParentStructure);

  Standard_EXPORT virtual void sortLODs() {};

protected:
  NCollection_Vector<Handle(Graphic3d_LOD)> myLODs;

private:
  Standard_Integer              myCurrentLODIdx;
  Handle(Graphic3d_LODSelector) mySelector;
  Graphic3d_StructurePtr        myStructure;
  Graphic3d_WorldViewProjState  myPrevCameraState;
};

DEFINE_STANDARD_HANDLE (Graphic3d_LODManager, Standard_Transient)

#endif //_Graphic3d_LODManager_Header
