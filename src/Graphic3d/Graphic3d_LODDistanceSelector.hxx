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

#ifndef _Graphic3d_LODDistanceSelector_Header
#define _Graphic3d_LODDistanceSelector_Header

#include <Graphic3d_LODSelector.hxx>

class Graphic3d_CStructure;
class Graphic3d_Camera;

class Graphic3d_LODDistanceSelector : public Graphic3d_LODSelector
{
public:
  Standard_EXPORT Graphic3d_LODDistanceSelector() {};
  Standard_EXPORT virtual ~Graphic3d_LODDistanceSelector() {};

  Standard_EXPORT virtual Standard_Real ComputeMetric (const Handle(Graphic3d_CStructure)& theParentStructure,
                                                       const Handle(Graphic3d_Camera)& theCamera) Standard_OVERRIDE;

  DEFINE_STANDARD_RTTI (Graphic3d_LODDistanceSelector, Graphic3d_LODSelector)
};

DEFINE_STANDARD_HANDLE (Graphic3d_LODDistanceSelector, Graphic3d_LODSelector)

#endif // _Graphic3d_LODDistanceSelector_Header
