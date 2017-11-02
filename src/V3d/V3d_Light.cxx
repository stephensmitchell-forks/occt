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

#include <V3d_Light.hxx>

#include <Graphic3d_Structure.hxx>
#include <V3d.hxx>
#include <V3d_View.hxx>
#include <V3d_Viewer.hxx>

IMPLEMENT_STANDARD_RTTIEXT(V3d_Light, Graphic3d_CLight)

// =======================================================================
// function : V3d_Light
// purpose  :
// =======================================================================
V3d_Light::V3d_Light (Graphic3d_TypeOfLightSource theType,
                      const Handle(V3d_Viewer)& theViewer) /// TODO V3d_Light should NOT store V3d_Viewer, or it will end-up with cyclic dependencies (active lights are stored in V3d_Viewer/V3d_View and in OpenGl_View)
: Graphic3d_CLight (theType)
{
  if (!theViewer.IsNull())
  {
    theViewer->AddLight (this);
  }
}

// =======================================================================
// function : SymetricPointOnSphere
// purpose  :
// =======================================================================
void V3d_Light::SymetricPointOnSphere (const Handle(V3d_View)& aView,
                                       const gp_Pnt& Center,
                                       const gp_Pnt& aPoint,
                                       const Standard_Real Rayon,
                                       Standard_Real& X, Standard_Real& Y, Standard_Real& Z,
                                       Standard_Real& VX, Standard_Real& VY, Standard_Real& VZ )
{
  Standard_Real X0,Y0,Z0,XP,YP,ZP;
  Standard_Real PXP,PYP,DeltaX,DeltaY,DeltaZ;
  Standard_Real A,B,C,Delta,Lambda;
  Standard_Integer IPX,IPY;

  Center.Coord(X0,Y0,Z0);
  aPoint.Coord(XP,YP,ZP);
  aView->Project(XP,YP,ZP,PXP,PYP);
  aView->Convert(PXP,PYP,IPX,IPY);
  aView->ProjReferenceAxe(IPX,IPY,X,Y,Z,VX,VY,VZ);
  DeltaX = X0 - XP;
  DeltaY = Y0 - YP;
  DeltaZ = Z0 - ZP;

//      The point of intersection of straight lines defined by :
//      - Straight line passing by the point of projection and the eye
//        if this is a perspective, parralel to the normal of the view 
//        if this is an axonometric view.
//        position in the view is parallel to the normal of the view
//      - The distance position of the target camera is equal to the radius.

  A = VX*VX + VY*VY + VZ*VZ ;
  B = -2. * (VX*DeltaX + VY*DeltaY + VZ*DeltaZ);
  C = DeltaX*DeltaX + DeltaY*DeltaY + DeltaZ*DeltaZ 
    - Rayon*Rayon ;
  Delta = B*B - 4.*A*C;
  if ( Delta >= 0 ) {
    Lambda = (-B + Sqrt(Delta))/(2.*A);
    if ( Lambda >= -0.0001 && Lambda <= 0.0001 ) 
      Lambda = (-B - Sqrt(Delta))/(2.*A);
    X = XP + Lambda*VX;
    Y = YP + Lambda*VY;
    Z = ZP + Lambda*VZ;
  }
  else {
    X = XP; Y = YP; Z = ZP;
  }
}

// =======================================================================
// function : IsDisplayed
// purpose  :
// =======================================================================
Standard_Boolean V3d_Light::IsDisplayed() const
{
  if (myGraphicStructure.IsNull())
  {
    return Standard_False;
  }

  return myGraphicStructure->IsDisplayed();
}
