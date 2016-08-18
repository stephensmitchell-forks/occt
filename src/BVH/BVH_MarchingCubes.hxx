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

#ifndef _BVH_MarchingCubes_Header
#define _BVH_MarchingCubes_Header

#include <BVH_Tessellator.hxx>

//! Implements marching cubes (MC) tessellation algorithm.
template<class T, int N>
class BVH_MarchingCubes : public BVH_Tessellator<T, N>
{
public:

  typedef typename BVH::VectorType<T, N>::Type BVH_VecNt;

public:

  //! Creates marching cubes (MC) tessellator.
  BVH_MarchingCubes (const BVH_Box<T, N>& theBox, const Standard_Integer theMaxSlices = 128);

  //! Generates tessellation for the given value of implicit function.
  virtual Standard_Boolean Perform (BVH_ImplicitFunction<T, N>& theFunction, const T theValue);

protected:

  //! Returns minimum corner of the given voxel.
  BVH_VecNt VoxelCorner (const int theX,
                         const int theY,
                         const int theZ) const;

  //! Interpolates the intersection point based function values.
  BVH_VecNt VertexInter (const BVH_VecNt& thePnt1,
                         const BVH_VecNt& thePnt2,
                         const T          theVal1,
                         const T          theVal2) const;
};

namespace BVH
{
  //! Tool object providing pre-computed tables for MC tessellator.
  class MarchingCubeTables
  {
  public:

    //! Returns table mapping the vertices under the surface to the intersecting edges.
    Standard_EXPORT static int (&EdgeTable())[256];

    //! Returns table used for generation of triangles representing the surface within a cell.
    Standard_EXPORT static char (&TriTable())[256][16];

  private:

    //! Table that maps the vertices under the surface to the intersecting edges.
    static int myEdgeTable[256];

    //! Table used for generation of triangles representing the surface within a cell.
    static char myTriTable[256][16];
  };
}

#include <BVH_MarchingCubes.lxx>

#endif // _BVH_MarchingCubes_Header
