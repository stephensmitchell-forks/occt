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

#ifndef _Graphic3d_VolumeData_HeaderFile
#define _Graphic3d_VolumeData_HeaderFile

#include <BVH_Box.hxx>
#include <Graphic3d_Vec3.hxx>

//! Base class for voxel-based volumetric data.
class Graphic3d_VolumeData : public Standard_Transient
{
public:

  //! Creates empty volumetric data.
  Standard_EXPORT Graphic3d_VolumeData();

  //! Creates volumetric data with the given dimensions.
  Standard_EXPORT Graphic3d_VolumeData (const BVH_Box3d&       theBounds,
                                        const Standard_Integer theNbVoxX,
                                        const Standard_Integer theNbVoxY,
                                        const Standard_Integer theNbVoxZ);

  //! Release resources of volumetric data.
  Standard_EXPORT virtual ~Graphic3d_VolumeData();

public:

  //! Returns bounding box of volumetric data.
  const BVH_Box3d& Bounds() const
  {
    return myBounds;
  }

  //! Sets bounding box of volumetric data.
  void SetBounds (const BVH_Box3d& theBounds)
  {
    myBounds = theBounds;
  }

  //! Returns number of voxels in X dimension.
  Standard_Integer NbVoxelX() const
  {
    return myNbVoxX;
  }

  //! Returns number of voxels in Y dimension.
  Standard_Integer NbVoxelY() const
  {
    return myNbVoxY;
  }

  //! Returns number of voxels in Z dimension.
  Standard_Integer NbVoxelZ() const
  {
    return myNbVoxZ;
  }

  //! Returns center of the given voxel.
  Graphic3d_Vec3d VoxelCenter (const Standard_Integer theIndexX,
                               const Standard_Integer theIndexY,
                               const Standard_Integer theIndexZ) const
  {
    return myBounds.CornerMin() + myVoxSize *
      Graphic3d_Vec3d (static_cast<Standard_Real> (theIndexX + 0.5),
                       static_cast<Standard_Real> (theIndexY + 0.5),
                       static_cast<Standard_Real> (theIndexZ + 0.5));
  }

  //! Returns origin (minimum corner) of the given voxel.
  Graphic3d_Vec3d VoxelOrigin (const Standard_Integer theIndexX,
                               const Standard_Integer theIndexY,
                               const Standard_Integer theIndexZ) const
  {
    return myBounds.CornerMin() + myVoxSize *
      Graphic3d_Vec3d (static_cast<Standard_Real> (theIndexX),
                       static_cast<Standard_Real> (theIndexY),
                       static_cast<Standard_Real> (theIndexZ));
  }

  //! Checks if volumetric data is dense.
  virtual Standard_Boolean IsDense() const
  {
    return Standard_False;
  }

  //! Returns current state of volumetric data (must be updated by application).
  Standard_Size State() const
  {
    return myState;
  }

  //! Updates current state of volumetric data (affects the rendering).
  void Update()
  {
    ++myState;
  }

protected:

  //! Reinitializes volumetric data.
  Standard_EXPORT virtual void init (const BVH_Box3d&       theBounds,
                                     const Standard_Integer theNbVoxX,
                                     const Standard_Integer theNbVoxY,
                                     const Standard_Integer theNbVoxZ);

protected:

  //! Bounding box of volumetric data.
  BVH_Box3d myBounds;

  //! Number of voxels in X dimension.
  Standard_Integer myNbVoxX;
  //! Number of voxels in Y dimension.
  Standard_Integer myNbVoxY;
  //! Number of voxels in Z dimension.
  Standard_Integer myNbVoxZ;

  //! Size of single voxel (grid step).
  Graphic3d_Vec3d myVoxSize;

private:

  //! Current state of the data.
  Standard_Size myState;

public:

  DEFINE_STANDARD_RTTI (Graphic3d_VolumeData, Standard_Transient)

};

DEFINE_STANDARD_HANDLE (Graphic3d_VolumeData, Standard_Transient)

#endif // _Graphic3d_VolumeData_HeaderFile
