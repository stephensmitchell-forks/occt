// Created on: 2015-04-07
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

#ifndef _BVH_ImplicitFunction_Header
#define _BVH_ImplicitFunction_Header

#include <BVH_Types.hxx>

//! Represents implicit function defined over 2D/3D space.
template<class T, int N>
class BVH_ImplicitFunction
{
public:

  typedef typename BVH::VectorType<T, N>::Type BVH_VecNt;

public:

  //! Creates new implicit function.
  BVH_ImplicitFunction() : myCornerMin (static_cast<T> (1.0)),
                           myCornerMax (static_cast<T> (1.0))
  {
    for (int aDim = 0; aDim < std::min (N, 3); ++aDim)
    {
      myCornerMin[aDim] = -std::numeric_limits<T>::max();
      myCornerMax[aDim] =  std::numeric_limits<T>::max();
    }
  }

  //! Creates new implicit function with the given domain.
  BVH_ImplicitFunction (const BVH_VecNt& theCornerMin,
                        const BVH_VecNt& theCornerMax) : myCornerMin (theCornerMin),
                                                         myCornerMax (theCornerMax)
  {
    Standard_ASSERT_INVOKE (N == 2 || N == 3 || N == 4);
  }

  //! Releases resources of implicit function.
  virtual ~BVH_ImplicitFunction() { }

  //! Returns minimum corner of function domain.
  const BVH_VecNt& CornerMin() const
  {
    return myCornerMin;
  }

  //! Returns maximum corner of function domain.
  const BVH_VecNt& CornerMax() const
  {
    return myCornerMax;
  }

  //! Calculates function value at the given point.
  virtual T Calculate (const BVH_VecNt& thePoint) = 0;

  //! Estimates surface normal at the given point.
  BVH_VecNt EstimateNormal (const BVH_VecNt& thePoint);

protected:

  //! Minimum corner of function domain.
  BVH_VecNt myCornerMin;

  //! Maximum corner of function domain.
  BVH_VecNt myCornerMax;

};

#include <BVH_ImplicitFunction.lxx>

#endif // _BVH_ImplicitFunction_Header
