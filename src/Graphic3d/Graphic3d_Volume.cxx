// Created on: 2014-12-12
// Created by: Ilya Sevrikov
// Copyright (c) 2013-2014 OPEN CASCADE SAS
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

#include <Graphic3d_Volume.hxx>
#include <Graphic3d_ShaderObject.hxx>
#include <Standard_DefineHandle.hxx>

IMPLEMENT_STANDARD_HANDLE (Graphic3d_Volume, Standard_Transient)
IMPLEMENT_STANDARD_RTTIEXT (Graphic3d_Volume)

//=================================================
//function : Graphic3d_Volume
//purpose  : 
//=================================================
Graphic3d_Volume::Graphic3d_Volume()
: myNumOfSamples (1000),
  myOpacityScale (1.0f),
  myApplyShading (Standard_False),
  myTriCubFilter (Standard_False),
  myTraceShadows (Standard_False),
  myRandomShift  (Standard_False)
{
  Init();
}

//=================================================
//function : ~Graphic3d_Volume
//purpose  : 
//=================================================
Graphic3d_Volume::~Graphic3d_Volume()
{
  //
}

//=================================================
//function : Init
//purpose  :
//=================================================
Standard_Boolean Graphic3d_Volume::Init()
{
  if (myShaderProgram.IsNull())
  {
    myShaderProgram = new Graphic3d_ShaderProgram();
  }

  const TCollection_AsciiString& aShaderFolder = Graphic3d_ShaderProgram::ShadersFolder();

  if (!myShaderProgram->AttachShader (Graphic3d_ShaderObject::
    CreateFromFile (Graphic3d_TOS_VERTEX,   aShaderFolder + "/VolumeRender.vs")))
  {
    return Standard_False;
  }

  if (!myShaderProgram->AttachShader (Graphic3d_ShaderObject::
    CreateFromFile (Graphic3d_TOS_FRAGMENT, aShaderFolder + "/VolumeRender.fs")))
  {
    return Standard_False;
  }

  return Standard_True;
}

