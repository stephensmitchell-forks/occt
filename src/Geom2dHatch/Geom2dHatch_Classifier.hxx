// Created on: 1994-02-03
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

#ifndef _Geom2dHatch_Classifier_HeaderFile
#define _Geom2dHatch_Classifier_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <IntRes2d_Position.hxx>
#include <TopAbs_State.hxx>
#include <TopClass_GeomEdge.hxx>

class Geom2dHatch_Elements;
class gp_Pnt2d;

class Geom2dHatch_Classifier 
{
public:

  DEFINE_STANDARD_ALLOC


  //! Empty constructor, undefined algorithm.
  Standard_EXPORT Geom2dHatch_Classifier();

  //! Creates an algorithm to classify the Point  P with
  //! Tolerance <T> on the face described by <F>.
  Standard_EXPORT Geom2dHatch_Classifier(Geom2dHatch_Elements& F,
                                         const gp_Pnt2d& P,
                                         const Standard_Real Tol);

  //! Classify the Point P with 2D-tolerance <Tol2D> relatively to
  //! the contour set by the 2D-curves in <F>.
  Standard_EXPORT void Perform(Geom2dHatch_Elements& F,
                               const gp_Pnt2d& P,
                               const Standard_Real Tol2D);

  //! Returns the result of the classification.
  Standard_EXPORT TopAbs_State State() const;

protected:


private:

  //! ATTENTION!!!
  //! Fields myEdge and myEdgeParameter
  //! should be eliminated after removing TopOpeBRep*
  //! packages.

  TopClass_GeomEdge myEdge;
  Standard_Real myEdgeParameter;
  TopAbs_State myState;
};

#endif // _Geom2dHatch_Classifier_HeaderFile
