// Created on: 2008-05-13
// Created by: Vladislav ROMASHKO
// Copyright (c) 2008-2014 OPEN CASCADE SAS
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

#ifndef _Voxel_Prs_HeaderFile
#define _Voxel_Prs_HeaderFile

#include <Handle_Voxel_Prs.hxx>
#include <AIS_InteractiveObject.hxx>
#include <PrsMgr_PresentationManager3d.hxx>
#include <Handle_Prs3d_Presentation.hxx>
#include <Handle_SelectMgr_Selection.hxx>
#include <Prs3d_Presentation.hxx>
#include <SelectMgr_Selection.hxx>
#include <Graphic3d_VolumeData.hxx>
#include <Graphic3d_TransferFunction.hxx>

//! Interactive object for volumetric data.
class Voxel_Prs : public AIS_InteractiveObject
{
public:

  //! Creates empty voxel presentation.
  Standard_EXPORT Voxel_Prs();

  //! Releases resources of voxel presentation.
  Standard_EXPORT ~Voxel_Prs();

public:

  //! Returns 3D volume object.
  Handle(Graphic3d_Volume) Volume() const
  {
    return myVolume;
  }

  //! Sets 3D volume object.
  void SetVolume (const Handle(Graphic3d_Volume)& theVolume)
  {
    myVolume  = theVolume;
  }

protected:

  //! Computes 3D volume presentation.
  Standard_EXPORT virtual void Compute (const Handle(PrsMgr_PresentationManager3d)& theManager,
                                        const Handle(Prs3d_Presentation)&           thePresentation,
                                        const Standard_Integer                      theMode = 0);

  //! Computes 3D volume selection.
  Standard_EXPORT void ComputeSelection (const Handle(SelectMgr_Selection)& theSelection,
                                         const Standard_Integer theMode);

protected:

  //! 3D volume object.
  Handle(Graphic3d_Volume) myVolume;

public:

  DEFINE_STANDARD_RTTI (Voxel_Prs)
};

#endif // _Voxel_Prs_HeaderFile