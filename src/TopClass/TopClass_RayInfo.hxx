// Created on: 2017-02-17
// Created by: Nikolai BUKHALOV
// Copyright (c) 1999-2017 OPEN CASCADE SAS
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

#ifndef _TopClass_RayInfo_HeaderFile
#define _TopClass_RayInfo_HeaderFile

#include <Standard_Type.hxx>

#include <gp_Lin2d.hxx>
#include <TopAbs_State.hxx>

//! Describes classification ray, which
//! joins the classification point with
//! some point in the edge of the 
//! classification face. 
class TopClass_RayInfo
{
public:
  //! Default constructor
  TopClass_RayInfo() :myParam(RealLast()), myClassifState(TopAbs_UNKNOWN), myWeight(0)
  {
    myLParamArrs[0] = RealLast();
    myLParamArrs[1] = RealLast();
  }

  //! Sets line of the ray
  void SetLin(const gp_Lin2d& theL)
  {
    myLin = theL;
  }

  //! Sets parameter on the edge correspond to the intersection
  //! point between the ray and the edge.
  void SetParam(const Standard_Real theParam)
  {
    myParam = theParam;
  }

  //! Sets classification state of the ray.
  //! This state will be applied as classification result
  //! if the ray is valid.
  void SetState(const TopAbs_State theState)
  {
    myClassifState = theState;
  }

  //! Returns line of the ray
  const gp_Lin2d& GetLine() const
  {
    return myLin;
  }

  //! Returns parameter on the edge correspond to the intersection
  //! point between the ray and the edge.
  Standard_Real GetParameter() const
  {
    return myParam;
  }

  //! Returns classification state of the ray.
  TopAbs_State GetState() const
  {
    return myClassifState;
  }

  //! Returns the weight of the ray.
  //! Allows determining to what extent this ray
  //! is valid.
  unsigned int GetWeight() const
  {
    return myWeight;
  }

  //! Increments myWeight.
  //! ATTENTION !!! It is impossible to decrease
  //! this value.
  void AddPenalty(const unsigned int theWeight = 1)
  {
    if (myWeight < (myMaximalWeight - theWeight))
    {
      myWeight += theWeight;
    }
    else
    {
      myWeight = myMaximalWeight;
    }
  }

  //! Fills myLParamArrs array
  void AddParamOnLine(const Standard_Real thePrm)
  {
    if (thePrm >= myLParamArrs[1])
      return;

    if (thePrm < myLParamArrs[0])
    {
      myLParamArrs[1] = myLParamArrs[0];
      myLParamArrs[0] = thePrm;
    }
    else
    {
      myLParamArrs[1] = thePrm;
    }
  }

  //! Method for provide sort
  bool operator<(const TopClass_RayInfo& theOther) const
  {
    return myWeight < theOther.myWeight;
  }

  //! Returns thickness of the section of the face by the ray.
  //! If the section contains several disconnected regions
  //! nearest region to the classification point will be taken.
  Standard_Real MinDeltaInterval() const
  {
    if (myLParamArrs[0] == RealLast())
      return RealLast();

    return (myLParamArrs[1] - myLParamArrs[0]);
  }

private:
  //! Ray-direction
  gp_Lin2d myLin;

  //! Parameter on myLin determined
  //! by intersection point of myLin
  //! and the edge.
  Standard_Real myParam;

  //! Classification state of the ray.
  //! This state will be applied as classification result
  //! if the ray is valid.
  TopAbs_State myClassifState;

  //! 0 if the segment is valid.
  //! It is incremented
  //! with every observed abnormality.
  unsigned int myWeight;

  //! Maximal value for unsigned int type
  static const unsigned int myMaximalWeight = UINT_MAX;

  //! Stores parameters on the myLin correspond to the intersection
  //! point between the ray and the edge(s).
  //! If there are many intersection points then two nearest
  //! to the classification point will be taken.
  //! This array is always sorted in ascending order.
  Standard_Real myLParamArrs[2];
};

#endif