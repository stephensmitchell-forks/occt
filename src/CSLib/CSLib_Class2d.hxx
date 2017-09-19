// Created on: 1995-03-08
// Created by: Laurent BUCHARD
// Copyright (c) 1995-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#ifndef _CSLib_Class2d_HeaderFile
#define _CSLib_Class2d_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <NCollection_Array1.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
class gp_Pnt2d;


//! Performs the classification of the given 2D-point
//! relatively to the closed 2D-polygon.
//! The orientation of the polygon is NOT taken into account.
class CSLib_Class2d 
{
public:

  DEFINE_STANDARD_ALLOC

private:
  enum PolyState
  {
    PolyOut = -1,
    PolyOn = 0,
    PolyIn = 1
  };

public:
  
  //! Constructs the 2D-polygon.
  //! theTP2d is the set of the vertices (closed polygon
  //! will always be created inside of this constructor;
  //! consequently, there is no point in repeating first and
  //! last point in theTP2d).
  //! theTolu and theTolv are tolerances.
  //! theUmin, theVmin, theUmax, theVmax are
  //! UV-bounds of the polygon.
  Standard_EXPORT CSLib_Class2d(const TColgp_Array1OfPnt2d& theTP2d,
                                const Standard_Real theTolu,
                                const Standard_Real theTolv,
                                const Standard_Real theUmin,
                                const Standard_Real theVmin,
                                const Standard_Real theUmax,
                                const Standard_Real theVmax);
  
  //! Classifies theP relatively to the polygon.
  //! Uses internal tolerances (myTolU, myTolV).
  Standard_EXPORT PolyState Classify(const gp_Pnt2d& theP) const;
  
  //! Classifies theP relatively to the polygon.
  //! Uses given tolerances (theTolU, theTolV).
  Standard_EXPORT PolyState Classify(const gp_Pnt2d& theP,
                                     const Standard_Real theTolU,
                                     const Standard_Real theTolV) const;
  
protected:

  //! Classifies theP relatively to the polygon.
  //! Uses internal tolerances (myTolU, myTolV).
  //! Realizes the algorithm itself.
  Standard_EXPORT PolyState InternalClass(const Standard_Real thePx,
                                          const Standard_Real thePy,
                                          const Standard_Real theTolU,
                                          const Standard_Real theTolV) const;

  //! Assignment operator is disabled for this class.
  const CSLib_Class2d& operator= (const CSLib_Class2d& Other) const;

private:
  
  //! Stores set of vertices of the given 2D-polygons
  TColgp_Array1OfPnt2d myPnts2d;

  //! Tolerances
  Standard_Real myTolU, myTolV;

  //! UV-bounds of the polygon
  Standard_Real myUmin, myUmax, myVmin, myVmax;

};

#endif // _CSLib_Class2d_HeaderFile
