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

#ifndef _Graphic3d_DenseVolumeData_HeaderFile
#define _Graphic3d_DenseVolumeData_HeaderFile

#include <NCollection_Array1.hxx>
#include <NCollection_Handle.hxx>
#include <Graphic3d_VolumeData.hxx>

//! Dense volumetric data of specific type
//! (stores the value in each voxel).
template<class T>
class Graphic3d_DenseVolumeData : public Graphic3d_VolumeData
{
public:

  //! Type of voxel value.
  typedef T DataType;

  //! Type of voxel array.
  typedef NCollection_Array1<DataType> VoxelArray;

public:

  //! Creates uninitialized dense volumetric data.
  Graphic3d_DenseVolumeData();

  //! Creates dense volumetric data with the given dimensions.
  Graphic3d_DenseVolumeData (const BVH_Box3d&       theBounds,
                             const Standard_Integer theNbVoxX,
                             const Standard_Integer theNbVoxY,
                             const Standard_Integer theNbVoxZ);

  //! Creates dense volumetric data with the given dimensions and data.
  Graphic3d_DenseVolumeData (const BVH_Box3d&                      theBounds,
                             const Standard_Integer                theNbVoxX,
                             const Standard_Integer                theNbVoxY,
                             const Standard_Integer                theNbVoxZ,
                             const NCollection_Handle<VoxelArray>& theVoxels);

  //! Releases resources of dense volumetric data.
  virtual ~Graphic3d_DenseVolumeData();

public:

  //! Reinitializes dense volumetric data with the given dimensions.
  virtual void Init (const BVH_Box3d&       theBounds,
                     const Standard_Integer theNbVoxX,
                     const Standard_Integer theNbVoxY,
                     const Standard_Integer theNbVoxZ);

  //! Reinitializes dense volumetric data with the given dimensions and data.
  virtual void Init (const BVH_Box3d&                      theBounds,
                     const Standard_Integer                theNbVoxX,
                     const Standard_Integer                theNbVoxY,
                     const Standard_Integer                theNbVoxZ,
                     const NCollection_Handle<VoxelArray>& theVoxels);

  //! Returns value at the given voxel.
  virtual DataType Voxel (const Standard_Integer theIndex) const
  {
    return myVoxels->Value (theIndex);
  }

  //! Returns value at the given voxel.
  virtual DataType Voxel (const Standard_Integer theIndexX,
                          const Standard_Integer theIndexY,
                          const Standard_Integer theIndexZ) const
  {
    return myVoxels->Value (theIndexX + (theIndexY + theIndexZ * myNbVoxY) * myNbVoxX);
  }

  //! Sets value at the given voxel.
  virtual void SetVoxel (const DataType         theValue,
                         const Standard_Integer theIndex)
  {
    myVoxels->ChangeValue (theIndex) = theValue;
  }

  //! Sets value at the given voxel.
  virtual void SetVoxel (const DataType         theValue,
                         const Standard_Integer theIndexX,
                         const Standard_Integer theIndexY,
                         const Standard_Integer theIndexZ)
  {
    myVoxels->ChangeValue (theIndexX + (theIndexY + theIndexZ * myNbVoxY) * myNbVoxX) = theValue;
  }

  //! Returns raw voxel data.
  NCollection_Handle<VoxelArray>& Data()
  {
    return myVoxels;
  }

  //! Returns raw voxel data.
  const NCollection_Handle<VoxelArray>& Data() const
  {
    return myVoxels;
  }

  //! Sets raw voxel data (must correspond to dataset size).
  void SetData (const NCollection_Handle<VoxelArray>& theData);

  //! Checks if volumetric data is dense.
  virtual Standard_Boolean IsDense() const Standard_OVERRIDE
  {
    return Standard_True;
  }

protected:

  //! Array of voxels.
  NCollection_Handle<VoxelArray> myVoxels;
};

#include <Graphic3d_DenseVolumeData.lxx>

#endif // _Graphic3d_DenseVolumeData_HeaderFile
