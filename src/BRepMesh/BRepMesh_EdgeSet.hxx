// Created on: 2015-08-28
// Created by: Danila ULYANOV
// Copyright (c) 2015 OPEN CASCADE SAS
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

#ifndef _BRepMesh_EdgeSet_Header
#define _BRepMesh_EdgeSet_Header

#include <BVH_PrimitiveSet.hxx>
#include <BVH_Types.hxx>

//! Class representing set of boundary edges.
template<class T, int N>
class BRepMesh_EdgeSet : public BVH_PrimitiveSet<T, N>
{
public:

  typedef typename BVH::VectorType<T, N>::Type BVH_VecNt;
  typedef typename BVH::ArrayType<T, N>::Type BVH_ArrayNt;

public:

  //! Creates empty edge set.
  BRepMesh_EdgeSet (const BVH_ArrayNt& theTriangulationVertices);

  //! Releases resources of edge set.
  virtual ~BRepMesh_EdgeSet();

private:

  BRepMesh_EdgeSet (const BRepMesh_EdgeSet& theOther);

  BRepMesh_EdgeSet& operator= (const BRepMesh_EdgeSet& theOther);

public:

  //! Array of indices of edge vertices.
  BVH_Array4i Elements;

private:

  const BVH_ArrayNt& Vertices;

public:

  //! Returns index of edge which is closest to the given one.
  Standard_Integer ClosestEdge (const BVH_VecNt& theEdgeV1,
                                const BVH_VecNt& theEdgeV2,
                                T* theDist = NULL);

  //! Returns index of edge which is closest to the given one (accounting for edge orientation).
  Standard_Integer CoherentEdge (const BVH_VecNt& theEdgeV1,
                                 const BVH_VecNt& theEdgeV2,
                                 T* theDist = NULL);

  //! Returns the distance between to edges.
  static T EdgeToEdgeDistance (const BVH_VecNt& theE1V1,
                               const BVH_VecNt& theE1V2,
                               const BVH_VecNt& theE2V1,
                               const BVH_VecNt& theE2V2);

  //! Returns the distance between to edges.
  static T EdgeToBoxDistance (const BVH_VecNt& theEV1,
                              const BVH_VecNt& theEV2,
                              const BVH_VecNt& theMinPoint,
                              const BVH_VecNt& theMaxPoint);

  //! Returns total number of edges.
  virtual Standard_Integer Size() const;

  //! Returns AABB of entire set of objects.
  using BVH_PrimitiveSet<T, N>::Box;

  //! Returns AABB of the given edge.
  virtual BVH_Box<T, N> Box (const Standard_Integer theIndex) const;

  //! Returns centroid position along the given axis.
  virtual T Center (const Standard_Integer theIndex,
                    const Standard_Integer theAxis) const;

  //! Performs transposing the two given edges in the set.
  virtual void Swap (const Standard_Integer theIndex1,
                     const Standard_Integer theIndex2);

};

#include <BRepMesh_EdgeSet.lxx>
#endif // _BRepMesh_EdgeSet_Header
