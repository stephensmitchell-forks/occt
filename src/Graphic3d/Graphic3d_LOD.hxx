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

#ifndef _Graphic3d_LOD_Header
#define _Graphic3d_LOD_Header

#include <Graphic3d_Camera.hxx>
#include <Graphic3d_SequenceOfGroup.hxx>
#include <Graphic3d_Structure.hxx>
#include <Standard.hxx>
#include <Standard_Macro.hxx>
#include <Standard_Transient.hxx>

class Graphic3d_LOD : public Standard_Transient
{
public:
  Standard_EXPORT Graphic3d_LOD() {};
  Standard_EXPORT virtual ~Graphic3d_LOD();

  Standard_EXPORT virtual void SetRange (const Standard_Real theFrom, const Standard_Real theTo);
  Standard_EXPORT virtual Handle(Graphic3d_Group) NewGroup();

  Standard_EXPORT Standard_Real ComputeMetrics (const Handle(Graphic3d_Camera)& theCamera) const;
  Standard_EXPORT void SetParent (const Handle(Graphic3d_Structure)& theParent);

  DEFINE_STANDARD_RTTI (Graphic3d_LOD, Standard_Transient)

protected:
  Graphic3d_SequenceOfGroup   myGroups;
  Handle(Graphic3d_Structure) myParent;
};

DEFINE_STANDARD_HANDLE (Graphic3d_LOD, Standard_Transient)

#endif // _Graphic3d_LOD_Header
