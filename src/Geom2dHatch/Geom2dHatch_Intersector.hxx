// Created on: 1994-03-23
// Created by: Jean Marc LACHAUME
// Copyright (c) 1994-1999 Matra Datavision
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

#ifndef _Geom2dHatch_Intersector_HeaderFile
#define _Geom2dHatch_Intersector_HeaderFile

#include <Geom2dInt_GInter.hxx>

class Geom2dAdaptor_Curve;
class gp_Lin2d;
class TopClass_GeomEdge;

class Geom2dHatch_Intersector  : public Geom2dInt_GInter
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Creates an intersector.
    Geom2dHatch_Intersector(const Standard_Real Confusion, const Standard_Real Tangency);
  
  //! Returns the confusion tolerance of the
  //! intersector.
    Standard_Real ConfusionTolerance() const;
  
  //! Sets the confusion tolerance of the intersector.
    void SetConfusionTolerance (const Standard_Real Confusion);
  
  //! Returns the tangency tolerance of the
  //! intersector.
    Standard_Real TangencyTolerance() const;
  
  //! Sets the tangency tolerance of the intersector.
    void SetTangencyTolerance (const Standard_Real Tangency);
  
  //! Intersects the curves C1 and C2.
  //! The results are retreived by the usual methods
  //! described in IntRes2d_Intersection.
  //! Creates an intersector.
    void Intersect (const Geom2dAdaptor_Curve& C1, const Geom2dAdaptor_Curve& C2);
  
  Standard_EXPORT Geom2dHatch_Intersector();
  
  //! Performs the intersection between the 2d line
  //! segment (<theLin>, <thePar>) and the Curve <theGE>.
  //! The line segment is the part of the 2d line <theLin> of
  //! parameter range [0, <thePar>] (thePar is positive and can be
  //! RealLast()). theTol2D is the Tolerance on the segment.
  //! The order is relevant, the first argument is the
  //! segment, the second one is the Edge.
  Standard_EXPORT void Perform(const gp_Lin2d& theLin,
                               const Standard_Real thePar,
                               const Standard_Real theTol2D,
                               const TopClass_GeomEdge& theGE);
  
protected:
  
private:
  Standard_Real myConfusionTolerance;
  Standard_Real myTangencyTolerance;


};


#include <Geom2dHatch_Intersector.lxx>





#endif // _Geom2dHatch_Intersector_HeaderFile
