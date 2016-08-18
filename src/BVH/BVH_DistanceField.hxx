// Created on: 2014-09-06
// Created by: Denis BOGOLEPOV
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

#ifndef _BVH_DistanceField_Header
#define _BVH_DistanceField_Header

#include <map>
#include <set>

#include <BVH_Geometry.hxx>
#include <BVH_ImplicitFunction.hxx>

template<class T, int N> class BVH_ParallelDistanceFieldBuilder;

//! Tool object for building 3D distance field from the set of BVH triangulations.
//! Distance field is a scalar field that measures the distance from a given point
//! to some object, including optional information about the inside and outside of
//! the structure. Distance fields are used as alternative surface representations
//! (like polygons or NURBS).
template<class T, int N>
class BVH_DistanceField : public BVH_ImplicitFunction<T, N>
{
  friend class BVH_ParallelDistanceFieldBuilder<T, N>;

public:

  typedef typename BVH::VectorType<T, N>::Type BVH_VecNt;

public:

  //! Creates empty 3D distance field.
  BVH_DistanceField (const Standard_Integer theMaximumSize,
                     const Standard_Boolean theComputeSign);

  //! Releases resources of 3D distance field.
  virtual ~BVH_DistanceField();

  //! Calculates distance at the given point.
  virtual T Calculate (const BVH_VecNt& thePoint);

  //! Builds 3D distance field from BVH geometry.
  Standard_Boolean Build (BVH_Triangulation<T, N>* theMesh);

public:

  //! Returns packed voxel data.
  const T* PackedData() const
  {
    return myVoxelData;
  }

  //! Returns distance value for the given voxel.
  T& Voxel (const Standard_Integer theX,
            const Standard_Integer theY,
            const Standard_Integer theZ)
  {
    const Standard_Integer aX = Max (Min (myDimensionX - 1, theX), 0);
    const Standard_Integer aY = Max (Min (myDimensionY - 1, theY), 0);
    const Standard_Integer aZ = Max (Min (myDimensionZ - 1, theZ), 0);

    return myVoxelData[aX + (aY + aZ * myDimensionY) * myDimensionX];
  }

  //! Returns distance value for the given voxel.
  T Voxel (const Standard_Integer theX,
           const Standard_Integer theY,
           const Standard_Integer theZ) const
  {
    const Standard_Integer aX = Max (Min (myDimensionX - 1, theX), 0);
    const Standard_Integer aY = Max (Min (myDimensionY - 1, theY), 0);
    const Standard_Integer aZ = Max (Min (myDimensionZ - 1, theZ), 0);

    return myVoxelData[aX + (aY + aZ * myDimensionY) * myDimensionX];
  }

  //! Returns size of voxel grid in X dimension.
  Standard_Integer DimensionX() const
  {
    return myDimensionX;
  }

  //! Returns size of voxel grid in Y dimension.
  Standard_Integer DimensionY() const
  {
    return myDimensionY;
  }

  //! Returns size of voxel grid in Z dimension.
  Standard_Integer DimensionZ() const
  {
    return myDimensionZ;
  }

  //! Returns size of single voxel.
  const BVH_VecNt& VoxelSize() const
  {
    return myVoxelSize;
  }

public:

  //! Type of vertex mesh feature.
  static const Standard_Integer VERT_ID = -2;

  //! Type of face (triangle) mesh feature.
  static const Standard_Integer FACE_ID = -1;

  //! Feature (vertex, edge, face) of the mesh.
  typedef std::pair<Standard_Integer,
                    Standard_Integer> Feature;

protected:

  //! Compare functor for symmetrical pairs of integers.
  struct SymmetricalPairComparator
  {
    bool operator() (const Feature& thePair1,
                     const Feature& thePair2) const
    {

      const Standard_Integer aMin1 = std::min (thePair1.first, thePair1.second);
      const Standard_Integer aMax1 = std::max (thePair1.first, thePair1.second);
      const Standard_Integer aMin2 = std::min (thePair2.first, thePair2.second);
      const Standard_Integer aMax2 = std::max (thePair2.first, thePair2.second);

      return (aMin1 < aMin2) || (aMin1 == aMin2 && aMax1 < aMax2);
    }

    bool equal (const Feature& thePair1,
                const Feature& thePair2) const
    {
      return std::min (thePair1.first, thePair1.second) == std::min (thePair2.first, thePair2.second)
          && std::max (thePair1.first, thePair1.second) == std::max (thePair2.first, thePair2.second);
    }
  };

  //! Associates normal with each mesh feature (vertex, edge, face).
  std::map<Feature, BVH_VecNt, SymmetricalPairComparator> myFeatureNormalMap;

  //! Computes the normal to the given triangle feature (vertex, edge, face).
  BVH_VecNt normalToFeature (const Feature& theFeature);

protected:

  //! Builds 3D distance field for the given Z slices.
  void buildSlices (Standard_Integer theStartZ, Standard_Integer theFinalZ);

  //! Computes squared distance from the point to BVH triangulation.
  T squareDistanceToMesh (const BVH_VecNt& thePnt, Standard_Boolean& theIsOutside);

  //! Returns the closest feature and direction to it from the given point.
  std::pair<BVH_VecNt, Feature> directToTrg (const BVH_VecNt& thePnt, const Standard_Integer theTrg);

protected:

  //! Array of voxels.
  T* myVoxelData;

  //! Size of single voxel.
  BVH_VecNt myVoxelSize;

  //! Size of voxel grid in X dimension.
  Standard_Integer myDimensionX;

  //! Size of voxel grid in Y dimension.
  Standard_Integer myDimensionY;

  //! Size of voxel grid in Z dimension.
  Standard_Integer myDimensionZ;

  //! Size of voxel grid in maximum dimension.
  Standard_Integer myMaximumSize;

  //! Enables/disables signing of distance field.
  Standard_Boolean myComputeSign;

  //! Triangulation to compute signed distance field.
  BVH_Triangulation<T, N>* myMesh;

};

#include <BVH_DistanceField.lxx>

#endif // _BVH_DistanceField_Header
