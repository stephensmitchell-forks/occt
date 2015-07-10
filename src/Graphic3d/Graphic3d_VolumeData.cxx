// Created on: 2014-12-24
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

#include <Graphic3d_VolumeData.hxx>

IMPLEMENT_STANDARD_HANDLE (Graphic3d_VolumeData, Standard_Transient)
IMPLEMENT_STANDARD_RTTIEXT(Graphic3d_VolumeData, Standard_Transient)

//========================================================================
//function : Graphic3d_VolumeData
//purpose  : 
//========================================================================
Graphic3d_VolumeData::Graphic3d_VolumeData()
: myNbVoxX (1),
  myNbVoxY (1),
  myNbVoxZ (1),
  myState  (1)
{
  //
}

//========================================================================
//function : Graphic3d_VolumeData
//purpose  : 
//========================================================================
Graphic3d_VolumeData::Graphic3d_VolumeData (const BVH_Box3d&       theBounds,
                                            const Standard_Integer theNbVoxX,
                                            const Standard_Integer theNbVoxY,
                                            const Standard_Integer theNbVoxZ)
: myState (1)
{
  init (theBounds,
        theNbVoxX,
        theNbVoxY,
        theNbVoxZ);
}

//========================================================================
//function : ~Graphic3d_VolumeData
//purpose  : 
//========================================================================
Graphic3d_VolumeData::~Graphic3d_VolumeData()
{
  //
}

//========================================================================
//function : init
//purpose  : 
//========================================================================
void Graphic3d_VolumeData::init (const BVH_Box3d&       theBounds,
                                 const Standard_Integer theNbVoxX,
                                 const Standard_Integer theNbVoxY,
                                 const Standard_Integer theNbVoxZ)
{
  myBounds = theBounds;

  myNbVoxX = theNbVoxX;
  myNbVoxY = theNbVoxY;
  myNbVoxZ = theNbVoxZ;

  myVoxSize = myBounds.Size() * Graphic3d_Vec3d (
    1.0 / static_cast<Standard_Real> (theNbVoxX),
    1.0 / static_cast<Standard_Real> (theNbVoxY),
    1.0 / static_cast<Standard_Real> (theNbVoxZ));
}
