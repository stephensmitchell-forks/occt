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
#include <Graphic3d_LODSelector.hxx>
#include <Graphic3d_LODDistanceSelector.hxx>
#include <Graphic3d_SequenceOfGroup.hxx>
#include <Graphic3d_StructurePtr.hxx>
#include <NCollection_IndexedMap.hxx>
#include <Standard.hxx>
#include <Standard_Handle.hxx>
#include <TColStd_HArray1OfInteger.hxx>

class Graphic3d_LOD;

typedef NCollection_IndexedMap<Handle(Graphic3d_LOD)> Graphic3d_MapOfLODs;

//! This class is used to manage all the LODs of one Graphic3d_Structure.
//! It provides an interface for the following actions:
//!   - addition of a new LOD;
//!   - management of current method for LOD metrics calculation;
//!   - access to LODs and its characteristics, such as quantity, range,
//!     combined bounding box.
//! The class also runs a procedure that picks the LOD that should be
//! rendered according to the current state of camera and stores choosen
//! LOD index.
class Graphic3d_LODManager : public Standard_Transient
{
public:

  //! Nullifies the pointer to current selector and clears used data maps,
  //! destroys the LODs used, so they can not be referenced to after the destruction
  //! of corresponding LOD manager
  Standard_EXPORT virtual ~Graphic3d_LODManager();

  //! Sets up a method for LOD metrics computation
  Standard_EXPORT void SetSelector (const Handle(Graphic3d_LODSelector)& theSelector)
  {
    mySelector = theSelector;
  }

  //! Returns a number of addded detail levels
  Standard_EXPORT inline Standard_Integer NbOfDetailLevels() const
  {
    return myLODs.Size();
  }

  //! For ranged LODs, sets the distance interval where the LOD will be visible.
  //! @param theLodIdx the index of LOD in sorted list
  Standard_EXPORT void SetRange (const Standard_Integer theLodIdx,
                                 const Standard_Real theFrom,
                                 const Standard_Real theTo);

  //! Returns own index of the LOD that is displayed
  Standard_EXPORT Standard_Boolean HasLodToDisplay (const Handle(Graphic3d_Camera)& theCamera);

  //! Returns graphic groups of the LOD that must be displayed according to the current
  //! camera state
  Standard_EXPORT const Graphic3d_SequenceOfGroup& GetCurrentGroups() const;

  Standard_EXPORT virtual Handle(Graphic3d_LOD) AddNewLOD() = 0;

  //! Returns combined bounding box for all LOD groups that were added to this
  //! manager
  Standard_EXPORT void GetCombinedBndBox (Graphic3d_BndBox4f& theBndBox) const;

  //! Returns false if at least one of LODs has non-empty sequence of Graphic3d_Groups
  Standard_EXPORT Standard_Boolean IsEmpty() const;

  //! Iterates through added LODs and clears up its resources.
  //! If theWithDestruction is true, all the graphic groups of each LOD will be destroyed
  //! and new group must be created for further displaying.
  //! If theWithDestruction is false, all the data of groups is reset to defauls
  //! so it is threated like empty.
  Standard_EXPORT void Clear (const Standard_Boolean theWithDestruction);

  //! The indexes are from 1 to NbOfDetailLevels(). The ID of the element corresponds
  //! to the index of the LOD in the map of LOD manager. Therefore, in case if no LOD was
  //! removed, the LODs in map are located in order of addition and IDs are identical to
  //! the addition iteration number
  Standard_EXPORT const Handle(Graphic3d_LOD)& GetLodById (const Standard_Integer theLodIdx);

  //! Checks if the given LOD is added to this manager
  Standard_EXPORT Standard_Boolean HasLod (const Handle(Graphic3d_LOD)& theLod) const;

  DEFINE_STANDARD_RTTIEXT (Graphic3d_LODManager, Standard_Transient)

protected:

  //! Creates an empty manager with invalid LOD metrics calculation method
  Standard_EXPORT Graphic3d_LODManager (const Handle(Graphic3d_Structure)& theParentStructure);

  //! Sorts LODs in the map according to its ranges to improve search for LOD to display.
  //! After the sort is done, flag myIsToSortLods will be set to false.
  Standard_EXPORT virtual void sortLODs() {};

protected:
  Graphic3d_MapOfLODs              myLODs;          //!< Map of LODs
  Handle(TColStd_HArray1OfInteger) myLodIndexes;    //!< Array of LOD indexes, for sorting purposes
  Standard_Boolean                 myIsToSortLods;  //!< Is true when LODs must be sorted before picking procedure. For example, when new LOD was added

private:
  Standard_Integer              myCurrentLODIdx;    //!< Index of current LOD that should be rendered; is -1 by default.
  Handle(Graphic3d_LODSelector) mySelector;         //!< A wrapper that performs measurement of LOD metrics
  Graphic3d_StructurePtr        myStructure;        //!< A pointer to graphic structure that is common for all LODs
  Graphic3d_WorldViewProjState  myPrevCameraState;  //!< Previous state of the camera for current LOD detection procedure
};

DEFINE_STANDARD_HANDLE (Graphic3d_LODManager, Standard_Transient)

#endif //_Graphic3d_LODManager_Header
