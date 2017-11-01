// Created by: Eugeny MALTCHIKOV
// Copyright (c) 2017 OPEN CASCADE SAS
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

#include <IntTools_OBB.hxx>

#include <Bnd_B3d.hxx>

// =======================================================================
// function : IsOut
// purpose  : 
// =======================================================================
Standard_Boolean IntTools_OBB::IsOut(const IntTools_OBB& theOther) const
{
  if (IsVoid() || theOther.IsVoid())
    return Standard_False;

  if (myIsAABox && theOther.myIsAABox)
  {
    // Both boxes are axis aligned - PMax points should be defined in this case.
    // Use Bnd_B3d::IsOut method to check if the boxes are interfering
    Bnd_B3d myB3d(myCenter, gp_Vec(myPMin, myCenter).XYZ());
    Bnd_B3d otherB3d(theOther.myCenter, gp_Vec(theOther.myPMin, theOther.myCenter).XYZ());
    return myB3d.IsOut(otherB3d);
  }

  // According to the Separating Axis Theorem for Oriented Bounding Boxes
  // it is necessary to check the 15 separating axes:
  // - 6 axes of the boxes;
  // - 9 cross products of the axes of the boxes.
  // If any of these axes is valid, the boxes do not interfere.

  // The algorithm is following:
  // 1. For the analyzed axis L:
  // 2. Half of boxes vertices are projected onto the axis.
  // 3. The projection segments for each box are computed.
  //    The length of the segment will be:
  //    Abs((HDimX*DirX).Dot(L)) + Abs((HDimY*DirY).Dot(L)) + Abs((HDimZ*DirZ).Dot(L))
  // 3. It is necessary to check if the distance between centers of the boxes
  //    projected onto the axis is more than the sum of the lengths
  //    of the projection segments:
  //    isOut = (theCenter2 - theCenter1).Dot(L) > (Length1 + Length2);


  // Precomputed difference between centers
  gp_XYZ D = theOther.myCenter - myCenter;

  // Check the axes of the this box, i.e. L is one of myAxes
  // Since the Dot product of two of these directions is null, it could be skipped:
  // myXDirection.Dot(myYDirection) = 0

  for (Standard_Integer i = 0; i < 3; ++i)
  {
    // Length of the first segment
    Standard_Real aLSegm1 = myHDims[i];

    // Length of the second segment
    Standard_Real aLSegm2 = 0;
    for (Standard_Integer j = 0; j < 3; ++j)
      aLSegm2 += Abs(theOther.myAxesMultiplied[j].Dot(myAxes[i]));

    // Distance between projected centers
    Standard_Real aDistCC = Abs(D.Dot(myAxes[i]));

    if (aDistCC > aLSegm1 + aLSegm2)
      return Standard_True;
  }

  // Check the axes of the Other box, i.e. L is one of theOther.myAxes

  for (Standard_Integer i = 0; i < 3; ++i)
  {
    // Length of the first segment
    Standard_Real aLSegm1 = 0.;
    for (Standard_Integer j = 0; j < 3; ++j)
      aLSegm1 += Abs(myAxesMultiplied[j].Dot(theOther.myAxes[i]));

    // Length of the second segment
    Standard_Real aLSegm2 = theOther.myHDims[i];

    // Distance between projected centers
    Standard_Real aDistCC = Abs(D.Dot(theOther.myAxes[i]));

    if (aDistCC > aLSegm1 + aLSegm2)
      return Standard_True;
  }

  // Check the axes produced by the cross products
  for (Standard_Integer i = 0; i < 3; ++i)
  {
    for (Standard_Integer j = 0; j < 3; ++j)
    {
      // Separating axis
      gp_XYZ L = myAxes[i].Crossed(theOther.myAxes[j]);

      // Length of the first segment
      Standard_Real aLSegm1 = 0.;
      for (Standard_Integer k = 0; k < 3; ++k)
        aLSegm1 += Abs(myAxesMultiplied[k].Dot(L));

      // Length of the second segment
      Standard_Real aLSegm2 = 0.;
      for (Standard_Integer k = 0; k < 3; ++k)
        aLSegm2 += Abs(theOther.myAxesMultiplied[k].Dot(L));

      // Distance between projected centers
      Standard_Real aDistCC = Abs(D.Dot(L));

      if (aDistCC > aLSegm1 + aLSegm2)
        return Standard_True;
    }
  }

  return Standard_False;
}
