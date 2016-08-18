// Created on: 2016-04-07
// Created by: Denis BOGOLEPOV
// Copyright (c) 2013-2016 OPEN CASCADE SAS
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

#ifndef _BVH_Tessellator_Header
#define _BVH_Tessellator_Header

#include <map>
#include <BVH_Triangulation.hxx>
#include <BVH_ImplicitFunction.hxx>

//! Tool class for generating tessellation of specific value
//! of 3D/4D implicit function. Tessellation is generated in
//! the specified bounding box, and its precision is defined
//! by the resolution of uniform grid used to split this box
//! into sub-volumes (voxels).
template<class T, int N>
class BVH_Tessellator
{
public:

  typedef typename BVH::VectorType<T, N>::Type BVH_VecNt;

public:

  //! Creates tessellator with the given resolution.
  BVH_Tessellator (const BVH_Box<T, N>& theBox, const Standard_Integer theMaxSlices = 128);

  //! Releases resources of the tessellator.
  virtual ~BVH_Tessellator() { }

  //! Returns number of slices in X dimension.
  Standard_Integer SlicesX() const
  {
    return mySlicesX;
  }

  //! Returns number of slices in Y dimension.
  Standard_Integer SlicesY() const
  {
    return mySlicesY;
  }

  //! Returns number of slices in Z dimension.
  Standard_Integer SlicesZ() const
  {
    return mySlicesZ;
  }

  //! Returns minimum corner of tessellation AABB.
  const BVH_VecNt& CornerMin() const
  {
    return myCornerMin;
  }

  //! Returns maximum corner of tessellation AABB.
  const BVH_VecNt& CornerMax() const
  {
    return myCornerMax;
  }

  //! Returns resulting tessellation (triangulation) of the function.
  NCollection_Handle<BVH_Triangulation<T, N> > Tessellation() const
  {
    return myTessellation;
  }

  //! Generates tessellation for the given value of implicit function.
  virtual Standard_Boolean Perform (BVH_ImplicitFunction<T, N>& theFunction, const T theValue) = 0;

protected:

  //! Minimum corner of AABB.
  BVH_VecNt myCornerMin;

  //! Maximum corner of AABB.
  BVH_VecNt myCornerMax;

  //! Size of single grid voxel.
  BVH_VecNt myVoxelSize;

  //! Number of slices in X dimension.
  Standard_Integer mySlicesX;

  //! Number of slices in Y dimension.
  Standard_Integer mySlicesY;

  //! Number of slices in Z dimension.
  Standard_Integer mySlicesZ;

  //! Resulting tessellation (triangulation) of the function.
  NCollection_Handle<BVH_Triangulation<T, N> > myTessellation;

protected:

  //! Compare functor for 3D/4D vectors.
  struct VertexComparator
  {
    bool operator() (const BVH_VecNt& thePoint1,
                     const BVH_VecNt& thePoint2) const
    {
      if (thePoint1.x() < thePoint2.x() - BVH::Precision<T>::Confusion())
      {
        return true;
      }
      if (thePoint1.x() > thePoint2.x() + BVH::Precision<T>::Confusion())
      {
        return false;
      }
      if (thePoint1.y() < thePoint2.y() - BVH::Precision<T>::Confusion())
      {
        return true;
      }
      if (thePoint1.y() > thePoint2.y() + BVH::Precision<T>::Confusion())
      {
        return false;
      }
      if (thePoint1.z() < thePoint2.z() - BVH::Precision<T>::Confusion())
      {
        return true;
      }

      return false;
    }
  };

  //! Map to store the set of indices for 3D mesh point.
  typedef std::map<BVH_VecNt, int, VertexComparator> IndexMap;

};

#include <BVH_Tessellator.lxx>

#endif // _BVH_Tessellator_Header
