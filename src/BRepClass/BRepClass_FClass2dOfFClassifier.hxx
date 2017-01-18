// Created on: 1992-11-18
// Created by: Remi LEQUETTE
// Copyright (c) 1992-1999 Matra Datavision
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

#ifndef _BRepClass_FClass2dOfFClassifier_HeaderFile
#define _BRepClass_FClass2dOfFClassifier_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <BRepClass_Intersector.hxx>
#include <TopAbs_State.hxx>
#include <TopTrans_CurveTransition.hxx>

class gp_Lin2d;
class TopClass_GeomEdge;

class BRepClass_FClass2dOfFClassifier 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Creates an undefined classifier.
  Standard_EXPORT BRepClass_FClass2dOfFClassifier();
  
  //! Starts  a  classification process.   The  point to
  //! classify is the origin of  the  line <L>.  <P>  is
  //! the original length of the segment on <L>  used to
  //! compute  intersections.
  Standard_EXPORT void Reset (const gp_Lin2d& L, const Standard_Real P);
  
  //! Updates  the classification process with  the edge
  //! <theE> from the boundary.
  //! theTol3D is entered for compatibility with Geom2dHatch* classes.
  Standard_EXPORT void Compare(const TopClass_GeomEdge& theE,
                               const Standard_Real theTol3D);
  
  //! Returns the current value of the parameter.
    Standard_Real Parameter() const;
  
  //! Returns the intersecting algorithm.
    BRepClass_Intersector& Intersector();
  
  //! Returns  0  if  the   last  compared   edge had no
  //! relevant intersection.  Else returns  the index of
  //! this   intersection  in the    last   intersection
  //! algorithm.
    Standard_Integer ClosestIntersection() const;
  
  //! Returns the current state of the point.
    TopAbs_State State() const;
  
  //! Returns the Standard_True if the closest intersection point
  //! represents head or end of the edge. Returns Standard_False
  //! otherwise.
    Standard_Boolean IsHeadOrEnd() const;

protected:

private:

  Standard_Boolean myFirstCompare;
  Standard_Boolean myFirstTrans;
  gp_Lin2d myLin;
  Standard_Real myParam;

  TopTrans_CurveTransition myTrans;
  BRepClass_Intersector myIntersector;
  Standard_Integer myClosest;
  TopAbs_State myState;
  Standard_Boolean myIsHeadOrEnd;
};

#define TheIntersector BRepClass_Intersector
#define TheIntersector_hxx <BRepClass_Intersector.hxx>
#define TopClass_Classifier2d BRepClass_FClass2dOfFClassifier
#define TopClass_Classifier2d_hxx <BRepClass_FClass2dOfFClassifier.hxx>

#include <TopClass_Classifier2d.lxx>

#undef TheIntersector
#undef TheIntersector_hxx
#undef TopClass_Classifier2d
#undef TopClass_Classifier2d_hxx

#endif // _BRepClass_FClass2dOfFClassifier_HeaderFile
