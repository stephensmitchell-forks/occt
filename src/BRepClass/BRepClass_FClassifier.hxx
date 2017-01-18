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

#ifndef _BRepClass_FClassifier_HeaderFile
#define _BRepClass_FClassifier_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <IntRes2d_Position.hxx>
#include <TopClass_GeomEdge.hxx>

class BRepClass_FaceExplorer;

class BRepClass_FClassifier
{
public:

  DEFINE_STANDARD_ALLOC
    
  //! Empty constructor, undefined algorithm.
  Standard_EXPORT BRepClass_FClassifier();

  //! Creates an algorithm to classify the point <P> 
  //! relatively to the face <F> with 3D-tolerance <theTol3D>
  //! (is used only for check ON-status).
  Standard_EXPORT BRepClass_FClassifier(BRepClass_FaceExplorer& F,
                                        const gp_Pnt2d& P,
                                        const Standard_Real theTol3D);

  //! Classify the point <P> 
  //! relatively to the face <F> with 3D-tolerance <theTol3D>
  //! (is used only for check ON-status).
  Standard_EXPORT void Perform(BRepClass_FaceExplorer& F,
                               const gp_Pnt2d& P,
                               const Standard_Real theTol3D);

  //! Returns the result of the classification.
  Standard_EXPORT TopAbs_State State() const;

  //! Returns   the    Edge  used   to    determine  the
  //! classification. When the State is ON  this  is the
  //! Edge containing the point.
  //! This method is used only for compatibility with TopOpeBRep
  //! packages. It should be deleted after this package will be eliminated.
  const TopClass_GeomEdge& Edge() const
  {
    return myEdge;
  }

  //! Returns the parameter on Edge() used to determine  the
  //! classification.
  //! This method is used only for compatibility with TopOpeBRep
  //! packages. It should be deleted after this package will be eliminated.
  Standard_EXPORT Standard_Real EdgeParameter() const
  {
    return myEdgeParameter;
  }
  
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


#endif // _BRepClass_FClassifier_HeaderFile
