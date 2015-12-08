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

#ifndef _OpenGl_LOD_Header
#define _OpenGl_LOD_Header

#include <Graphic3d_LOD.hxx>
#include <OpenGl_AspectFace.hxx>
#include <OpenGl_AspectLine.hxx>
#include <OpenGl_Structure.hxx>

class OpenGl_LOD : public Graphic3d_LOD
{
public:

  Standard_EXPORT OpenGl_LOD() : Graphic3d_LOD () {};
  Standard_EXPORT ~OpenGl_LOD() {};

  Standard_EXPORT virtual Handle(Graphic3d_Group) NewGroup (const Handle(Graphic3d_Structure)& theParentStruct) Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT (OpenGl_LOD, Graphic3d_LOD)
};

DEFINE_STANDARD_HANDLE (OpenGl_LOD, Graphic3d_LOD)

#endif // _OpenGl_LOD_Header
