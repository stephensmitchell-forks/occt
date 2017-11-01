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

#ifndef _IntTools_OBB_HeaderFile
#define _IntTools_OBB_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>
#include <Standard_Real.hxx>
#include <Standard_Boolean.hxx>

#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>
#include <gp_XYZ.hxx>

//! The class describes the Oriented Bounding Box (OBB).
//! It is defined by a center of the box, the axes and the halves
//! of its three dimensions.
//! The OBB are used as a second step, after intersection of the
//! axes aligned boxes, for filtering the time-consuming interfering pairs
//! in Boolean Operations.
class IntTools_OBB
{
public:

  DEFINE_STANDARD_ALLOC

  //! Empty constructor
  IntTools_OBB()
  :
  myIsAABox(Standard_False)
  {
     myHDims[0] = 0.;
     myHDims[1] = 0.;
     myHDims[2] = 0.;
  }

  //! Constructor taking all defining parameters
  IntTools_OBB(const gp_Pnt& theCenter,
               const gp_Dir& theXDirection,
               const gp_Dir& theYDirection,
               const gp_Dir& theZDirection,
               const Standard_Real theHXSize,
               const Standard_Real theHYSize,
               const Standard_Real theHZSize)
  :
  myCenter (theCenter.XYZ()),
  myIsAABox(Standard_False)
  {
    myAxes[0] = theXDirection.XYZ();
    myAxes[1] = theYDirection.XYZ();
    myAxes[2] = theZDirection.XYZ();

    myHDims[0] = theHXSize;
    myHDims[1] = theHYSize;
    myHDims[2] = theHZSize;

    myAxesMultiplied[0] = myAxes[0].Multiplied(myHDims[0]);
    myAxesMultiplied[1] = myAxes[1].Multiplied(myHDims[1]);
    myAxesMultiplied[2] = myAxes[2].Multiplied(myHDims[2]);
  }

  //! Sets the center of OBB
  void SetCenter(const gp_Pnt& theCenter)
  {
    myCenter = theCenter.XYZ();
  }

  //! Returns the center of OBB
  const gp_XYZ& Center() const
  {
    return myCenter;
  }

  //! Sets the X component of OBB - direction and size
  void SetXComponent(const gp_Dir& theXDirection,
                     const Standard_Real theHXSize)
  {
    myAxes[0] = theXDirection.XYZ();
    myHDims[0] = theHXSize;

    myAxesMultiplied[0] = myAxes[0].Multiplied(myHDims[0]);
  }

  //! Returns the X Direction of OBB
  const gp_XYZ& XDirection() const
  {
    return myAxes[0];
  }

  //! Returns the X Dimension of OBB
  Standard_Real HXSize() const
  {
    return myHDims[0];
  }

  //! Sets the Y component of OBB - direction and size
  void SetYComponent(const gp_Dir& theYDirection,
                     const Standard_Real theHYSize)
  {
    myAxes[1] = theYDirection.XYZ();
    myHDims[1] = theHYSize;

    myAxesMultiplied[1] = myAxes[1].Multiplied(myHDims[1]);
  }

  //! Returns the Y Direction of OBB
  const gp_XYZ& YDirection() const
  {
    return myAxes[1];
  }

  //! Returns the Y Dimension of OBB
  Standard_Real HYSize() const
  {
    return myHDims[1];
  }

  //! Sets the Z component of OBB - direction and size
  void SetZComponent(const gp_Dir& theZDirection,
                     const Standard_Real theHZSize)
  {
    myAxes[2] = theZDirection.XYZ();
    myHDims[2] = theHZSize;

    myAxesMultiplied[2] = myAxes[2].Multiplied(myHDims[2]);
  }

  //! Returns the Z Direction of OBB
  const gp_XYZ& ZDirection() const
  {
    return myAxes[2];
  }

  //! Returns the Z Dimension of OBB
  Standard_Real HZSize() const
  {
    return myHDims[2];
  }

  //! Checks if the box is empty.
  Standard_Boolean IsVoid() const
  {
    return (((myHDims[0] + myHDims[1] + myHDims[2]) < gp::Resolution()) && !myIsAABox);
  }

  //! Sets the flag for axes aligned box
  void SetAABox(const Standard_Boolean& theFlag)
  {
    myIsAABox = theFlag;
  }

  //! Returns TRUE if the box is axes aligned
  Standard_Boolean IsAABox() const
  {
    return myIsAABox;
  }

  //! Sets the min point
  void SetMinPoint(const gp_Pnt& theMin)
  {
    myPMin = theMin.XYZ();
  }

  //! Returns the min point of the box
  const gp_XYZ& MinPoint() const
  {
    return myPMin;
  }

  //! Check if the box do not interfere the other box.
  Standard_EXPORT Standard_Boolean IsOut(const IntTools_OBB& theOther) const;

private:

  // OBB definition
  gp_XYZ myCenter;            //! Center of the OBB
  gp_XYZ myAxes[3];           //! The axes of the box
  Standard_Real myHDims[3];   //! Half-size dimensions of the OBB

  gp_XYZ myAxesMultiplied[3]; //! Multiplied axes to avoid their recalculation in IsOut method

  Standard_Boolean myIsAABox; //! To be set if the OBB is axis aligned box;
  gp_XYZ myPMin;              //! Min point of the box.
};

#endif
