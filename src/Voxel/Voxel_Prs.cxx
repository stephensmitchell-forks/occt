// Created on: 2014-12-17
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

#include <Voxel_Prs.hxx>
#include <Graphic3d_AspectFillArea3d.hxx>
#include <Graphic3d_Group.hxx>
#include <Prs3d_Root.hxx>
#include <Standard_DefineHandle.hxx>

IMPLEMENT_STANDARD_HANDLE (Voxel_Prs, AIS_InteractiveObject)
IMPLEMENT_STANDARD_RTTIEXT(Voxel_Prs, AIS_InteractiveObject)

//========================================================================
//function : Voxel_Prs
//purpose  : 
//========================================================================
Voxel_Prs::Voxel_Prs() : AIS_InteractiveObject (PrsMgr_TOP_AllView)
{
  //
}

//========================================================================
//function : ~Voxel_Prs
//purpose  : 
//========================================================================
Voxel_Prs::~Voxel_Prs()
{
  //
}

//========================================================================
//function : Compute
//purpose  : 
//========================================================================
void Voxel_Prs::Compute (const Handle(PrsMgr_PresentationManager3d)& /*theMgr*/,
                         const Handle(Prs3d_Presentation)&           thePrs,
                         const Standard_Integer                      /*theMode*/)
{
  thePrs->Clear();

  Handle(Graphic3d_Group) aGroup = Prs3d_Root::CurrentGroup (thePrs);

  if (!aGroup.IsNull())
  {
    aGroup->AddVolume (myVolume);
  }
}

//========================================================================
//function : ComputeSelection
//purpose  : 
//========================================================================
void Voxel_Prs::ComputeSelection (const Handle(SelectMgr_Selection)& /*theSelection*/,
                                  const Standard_Integer             /*theMode*/)
{
  // TODO
}
