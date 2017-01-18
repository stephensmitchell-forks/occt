// Created on: 1993-05-28
// Created by: Modelistation
// Copyright (c) 1993-1999 Matra Datavision
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

#ifndef _BRepClass_FaceClassifier_HeaderFile
#define _BRepClass_FaceClassifier_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <BRepClass_FaceExplorer.hxx>
#include <BRepClass_FClassifier.hxx>
#include <Standard_Real.hxx>

class gp_Pnt2d;
class TopoDS_Face;
class gp_Pnt;


//! Provides Constructors with a Face.
class BRepClass_FaceClassifier  : public BRepClass_FClassifier
{
public:
  DEFINE_STANDARD_ALLOC
  
  //! Empty constructor, undefined algorithm.
  Standard_EXPORT BRepClass_FaceClassifier();
  
  //! Creates an algorithm to classify the point P relatively to
  //! the face <F> with 3D-tolerance <theTol3D> (used only for check ON-status).
  Standard_EXPORT 
    BRepClass_FaceClassifier(const TopoDS_Face& F,
                             const gp_Pnt2d& P,
                             const Standard_Real theTol3D);
  
  //! Creates an algorithm to classify the point P relatively to
  //! the face <F> with 3D-tolerance <theTol3D> (used only for check ON-status).
  Standard_EXPORT BRepClass_FaceClassifier(const TopoDS_Face& F,
                                           const gp_Pnt& P,
                                           const Standard_Real theTol3D);

  //! Classify the Point P with 3D-tolerance <theTol3D> (used only for check ON-status)
  //! relatively to the face <F>.
  Standard_EXPORT void Perform(const TopoDS_Face& F,
                               const gp_Pnt2d& P,
                               const Standard_Real theTol3D);
  
  //! Classifies the point P relatively to
  //! the face <F> with 3D-tolerance <theTol3D> (used only for check ON-status).
  Standard_EXPORT void Perform(const TopoDS_Face& F,
                               const gp_Pnt& P,
                               const Standard_Real theTol3D);
  
protected:
  

private:

  BRepClass_FaceExplorer myFEx;
};

#endif // _BRepClass_FaceClassifier_HeaderFile
