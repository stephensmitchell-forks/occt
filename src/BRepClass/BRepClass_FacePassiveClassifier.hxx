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

#ifndef _BRepClass_FacePassiveClassifier_HeaderFile
#define _BRepClass_FacePassiveClassifier_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>
#include <gp_Lin2d.hxx>
#include <Standard_Real.hxx>
#include <TopTrans_CurveTransition.hxx>
#include <BRepClass_Intersector.hxx>
#include <Standard_Integer.hxx>
#include <TopAbs_State.hxx>
#include <TopAbs_Orientation.hxx>
class Standard_DomainError;
class TopClass_GeomEdge;
class BRepClass_Intersector;
class gp_Lin2d;



class BRepClass_FacePassiveClassifier 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Creates an undefined classifier.
  Standard_EXPORT BRepClass_FacePassiveClassifier();
  
  //! Starts  a  classification process.   The  point to
  //! classify is the origin of  the  line <L>.  <P>  is
  //! the original length of the segment on <L>  used to
  //! compute  intersections.   <Tol> is the   tolerance
  //! attached to the line segment in intersections.
  Standard_EXPORT void Reset (const gp_Lin2d& L, const Standard_Real P, const Standard_Real Tol);
  
  //! Updates  the classification process with  the edge
  //! <E> from the boundary.
  Standard_EXPORT void Compare(const TopClass_GeomEdge& E);
  
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



  Standard_Boolean myIsSet;
  Standard_Boolean myFirstCompare;
  Standard_Boolean myFirstTrans;
  gp_Lin2d myLin;
  Standard_Real myParam;
  Standard_Real myTolerance;
  TopTrans_CurveTransition myTrans;
  BRepClass_Intersector myIntersector;
  Standard_Integer myClosest;
  TopAbs_State myState;
  Standard_Boolean myIsHeadOrEnd;


};

#define TheIntersector BRepClass_Intersector
#define TheIntersector_hxx <BRepClass_Intersector.hxx>
#define TopClass_Classifier2d BRepClass_FacePassiveClassifier
#define TopClass_Classifier2d_hxx <BRepClass_FacePassiveClassifier.hxx>

#include <TopClass_Classifier2d.lxx>

#undef TheIntersector
#undef TheIntersector_hxx
#undef TopClass_Classifier2d
#undef TopClass_Classifier2d_hxx




#endif // _BRepClass_FacePassiveClassifier_HeaderFile
