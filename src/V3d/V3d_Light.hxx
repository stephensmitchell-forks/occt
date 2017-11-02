// Created on: 1992-01-17
// Created by: GG 
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

#ifndef _V3d_Light_HeaderFile
#define _V3d_Light_HeaderFile

#include <Graphic3d_CLight.hxx>
#include <V3d_TypeOfLight.hxx>
#include <V3d_View.hxx>

class Graphic3d_Structure;
class V3d_Viewer;

//! Defines services on Light type objects..
//! (base class for AmbientLight and PositionLight)
class V3d_Light : public Graphic3d_CLight
{
  DEFINE_STANDARD_RTTIEXT(V3d_Light, Graphic3d_CLight)
public:

  //! Returns the Type of the Light
  V3d_TypeOfLight Type() const { return (V3d_TypeOfLight)myType; }

  //! Returns TRUE when a light representation is displayed
  Standard_EXPORT Standard_Boolean IsDisplayed() const;

protected:

  Standard_EXPORT V3d_Light (Graphic3d_TypeOfLightSource theType,
                             const Handle(V3d_Viewer)& theViewer);

  //! Returns the symmetric point coordinates of "aPoint"
  //! on the sphere of center "Center" and radius "Radius".
  //! VX,VY,VZ is the project vector of view.
  Standard_EXPORT static void SymetricPointOnSphere (const Handle(V3d_View)& aView,
                                                     const gp_Pnt& Center,
                                                     const gp_Pnt& aPoint,
                                                     const Standard_Real Radius,
                                                     Standard_Real& X, Standard_Real& Y, Standard_Real& Z,
                                                     Standard_Real& VX, Standard_Real& VY, Standard_Real& VZ);

protected:

  Handle(Graphic3d_Structure) myGraphicStructure;
  Handle(Graphic3d_Structure) myGraphicStructure1;

};

DEFINE_STANDARD_HANDLE(V3d_Light, Graphic3d_CLight)

#endif // _V3d_Light_HeaderFile
