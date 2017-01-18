// Created on: 1995-03-22
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

#ifndef _BRepTopAdaptor_FClass2d_HeaderFile
#define _BRepTopAdaptor_FClass2d_HeaderFile

#include <BRepTopAdaptor_SeqOfPtr.hxx>
#include <TColStd_SequenceOfInteger.hxx>
#include <TopoDS_Face.hxx>
class CSLib_Class2d;

//! Checks the position of some 2D-point relatively 
//! to the face.
//! Returns status ON if the point is in the at least of one
//! edge of the face.
//! Returns status IN if the point is included to the face.
//! Otherwise, returns status OUT.

class BRepTopAdaptor_FClass2d 
{
public:

  DEFINE_STANDARD_ALLOC

  //! Empty constructor
  Standard_EXPORT BRepTopAdaptor_FClass2d();

  //! Initializes algorithm by the face F
  //! and tolerance Tol
  Standard_EXPORT BRepTopAdaptor_FClass2d(const TopoDS_Face& F, const Standard_Real Tol);
  
  //! Initializes algorithm by the face F
  //! and tolerance Tol
  Standard_EXPORT void Init(const TopoDS_Face& F, const Standard_Real Tol);

  //! Classifies an infinite 2d point relatively of the Face.
  Standard_EXPORT TopAbs_State PerformInfinitePoint() const;
  
  //! Returns state of the 2d point Puv.
  //! If RecadreOnPeriodic is true (default value),
  //! the point is adjusted to period for a periodical surface before classification.
  Standard_EXPORT TopAbs_State
    Perform(const gp_Pnt2d& Puv,
            const Standard_Boolean RecadreOnPeriodic = Standard_True) const;
  
  //! Destructor
  Standard_EXPORT void Destroy();
  ~BRepTopAdaptor_FClass2d()
  {
    Destroy();
  }
  
  //! Test a point with +- an offset (Tol) and returns
  //! On if some points are OUT an some are IN
  //! (Caution: Internal use . see the code for more details)
  Standard_EXPORT TopAbs_State
    TestOnRestriction (const gp_Pnt2d& Puv,
                       const Standard_Real Tol,
                       const Standard_Boolean RecadreOnPeriodic = Standard_True) const;

  //! Returns TRUE if material of the Face
  //! outside of its outer wire. 
  Standard_Boolean IsHole() const
  {
    return (PerformInfinitePoint() == TopAbs_IN);
  }


protected:
  const BRepTopAdaptor_FClass2d& operator= (const BRepTopAdaptor_FClass2d&) const;




private:

  NCollection_Sequence<CSLib_Class2d*> TabClass;
  NCollection_Sequence<Standard_Integer> TabOrien;
  TopoDS_Face Face;
  Standard_Real Toluv;
  Standard_Real Umin;
  Standard_Real Vmin;
  Standard_Real Umax;
  Standard_Real Vmax;


};







#endif // _BRepTopAdaptor_FClass2d_HeaderFile
