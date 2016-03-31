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

#ifndef _OpenGl_LODManager_Header
#define _OpenGl_LODManager_Header

#include <Graphic3d_LODManager.hxx>

class OpenGl_LODManager : public Graphic3d_LODManager
{
public:
  Standard_EXPORT OpenGl_LODManager (const Handle(Graphic3d_Structure)& theParentStructure);

  Standard_EXPORT virtual ~OpenGl_LODManager() {};

  Standard_EXPORT virtual Handle(Graphic3d_LOD) AddNewLOD() Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT (OpenGl_LODManager, Graphic3d_LODManager)

protected:
  Standard_EXPORT virtual void sortLODs() Standard_OVERRIDE;
};

DEFINE_STANDARD_HANDLE (OpenGl_LODManager, Graphic3d_LODManager)

#endif // _OpenGl_LODManager_Header