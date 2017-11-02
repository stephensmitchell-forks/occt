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

#include <V3d_SpotLight.hxx>

#include <Graphic3d_ArrayOfSegments.hxx>
#include <Graphic3d_AspectLine3d.hxx>
#include <Graphic3d_Group.hxx>
#include <Graphic3d_Structure.hxx>
#include <TCollection_AsciiString.hxx>
#include <V3d.hxx>
#include <V3d_BadValue.hxx>
#include <V3d_View.hxx>
#include <V3d_Viewer.hxx>

IMPLEMENT_STANDARD_RTTIEXT(V3d_SpotLight,V3d_PositionLight)

// =======================================================================
// function : V3d_SpotLight
// purpose  :
// =======================================================================
V3d_SpotLight::V3d_SpotLight (const Handle(V3d_Viewer)& theViewer,
                              const Standard_Real theX,
                              const Standard_Real theY,
                              const Standard_Real theZ,
                              const V3d_TypeOfOrientation theDirection,
                              const Quantity_Color& theColor,
                              const Standard_Real theConstAttenuation,
                              const Standard_Real theLinearAttenuation,
                              const Standard_Real theConcentration,
                              const Standard_Real theAngle)
: V3d_PositionLight (Graphic3d_TOLS_SPOT, theViewer)
{
  gp_Dir aDir = V3d::GetProjAxis (theDirection);
  SetColor (theColor);
  SetTarget (theX + aDir.X(), theY + aDir.Y(), theZ + aDir.Z());
  SetPosition (theX, theY, theZ);
  SetDirection (aDir.X(), aDir.Y(), aDir.Z());
  SetAttenuation ((float )theConstAttenuation, (float )theLinearAttenuation);
  SetConcentration ((float )theConcentration);
  SetAngle ((float )theAngle);
}

// =======================================================================
// function : V3d_SpotLight
// purpose  :
// =======================================================================
V3d_SpotLight::V3d_SpotLight (const Handle(V3d_Viewer)& theViewer,
                              const Standard_Real theXt,
                              const Standard_Real theYt,
                              const Standard_Real theZt,
                              const Standard_Real theXp,
                              const Standard_Real theYp,
                              const Standard_Real theZp,
                              const Quantity_Color& theColor,
                              const Standard_Real theConstAttenuation,
                              const Standard_Real theLinearAttenuation,
                              const Standard_Real theConcentration,
                              const Standard_Real theAngle)
: V3d_PositionLight (Graphic3d_TOLS_SPOT, theViewer)
{
  SetColor (theColor);
  SetTarget (theXt, theYt, theZt);
  SetPosition (theXp, theYp, theZp);
  SetDirection (theXt - theXp, theYt - theYp, theZt - theZp);
  SetAttenuation ((float )theConstAttenuation, (float )theLinearAttenuation);
  SetConcentration ((float )theConcentration);
  SetAngle ((float )theAngle);
}

// =======================================================================
// function : SetDirection
// purpose  :
// =======================================================================
void V3d_SpotLight::SetDirection (V3d_TypeOfOrientation theDirection)
{
  gp_Dir aDir = V3d::GetProjAxis (theDirection);
  SetDirection (aDir.X(), aDir.Y(), aDir.Z());
}

// =======================================================================
// function : Symbol
// purpose  :
// =======================================================================
void V3d_SpotLight::Symbol (const Handle(Graphic3d_Group)& theSymbol,
                            const Handle(V3d_View)& ) const
{
  Standard_Real X,Y,Z;
  Standard_Real DX,DY,DZ;
  this->Position(X,Y,Z);
  this->Direction(DX,DY,DZ);

  V3d::ArrowOfRadius(theSymbol,X,Y,Z,-DX,-DY,-DZ,M_PI/8.,this->Radius()/15.);
}

// =======================================================================
// function : Display
// purpose  :
// =======================================================================
void V3d_SpotLight::Display (const Handle(V3d_View)& theView,
                             const V3d_TypeOfRepresentation theTPres)
{
  Standard_Real X,Y,Z,Rayon;
  Standard_Real X0,Y0,Z0,VX,VY,VZ;
  Standard_Real X1,Y1,Z1;
  Standard_Real DXRef,DYRef,DZRef,DXini,DYini,DZini;
  V3d_TypeOfRepresentation Pres;

//  Creation of a structure slight of markable elements (position of the
//  light, and the domain of lighting represented by a circle)
//  Creation of a structure snopick of non-markable elements (target, meridian and 
//  parallel).// 

  Pres = theTPres;
  Handle(V3d_Viewer) TheViewer = theView->Viewer();
  if (!myGraphicStructure.IsNull()) {
    myGraphicStructure->Disconnect(myGraphicStructure1);
    myGraphicStructure->Clear();
    myGraphicStructure1->Clear();
    if (Pres == V3d_SAMELAST) Pres = myTypeOfRepresentation;
  }
  else {
    if (Pres == V3d_SAMELAST) Pres = V3d_SIMPLE;
    Handle(Graphic3d_Structure) slight = new Graphic3d_Structure(TheViewer->StructureManager());
    myGraphicStructure = slight;
    Handle(Graphic3d_Structure) snopick = new Graphic3d_Structure(TheViewer->StructureManager()); 
    myGraphicStructure1 = snopick;
  }

  Handle(Graphic3d_Group) gradius, gExtArrow, gIntArrow;
  if (Pres == V3d_COMPLETE)
  {
    gradius   = myGraphicStructure->NewGroup();
    gExtArrow = myGraphicStructure->NewGroup();
    gIntArrow = myGraphicStructure->NewGroup();
  }
  Handle(Graphic3d_Group) glight = myGraphicStructure->NewGroup();
  Handle(Graphic3d_Group) gsphere;
  if (Pres == V3d_COMPLETE
   || Pres == V3d_PARTIAL)
  {
    gsphere = myGraphicStructure->NewGroup();
  }
  
  Handle(Graphic3d_Group) gnopick = myGraphicStructure1->NewGroup();
  
  X0 = myTarget.X();
  Y0 = myTarget.Y();
  Z0 = myTarget.Z();
  
//Display of the position of the light.

  const Quantity_Color Col1 = this->Color();
  Handle(Graphic3d_AspectLine3d) Asp1 = new Graphic3d_AspectLine3d();
  Asp1->SetColor(Col1);
  glight->SetPrimitivesAspect(Asp1);
  this->Symbol(glight,theView);
  
// Display of the reference sphere (limited by circle).

  if (Pres == V3d_COMPLETE || Pres == V3d_PARTIAL) {
    
    Rayon = this->Radius(); 
    theView->Proj(VX,VY,VZ);
    V3d::CircleInPlane(gsphere,X0,Y0,Z0,VX,VY,VZ,Rayon);

// Display of the radius of the sphere (line + text)

    if (Pres == V3d_COMPLETE) {
      this->Position(X,Y,Z);
      Handle(Graphic3d_ArrayOfSegments) aPrims = new Graphic3d_ArrayOfSegments(2);
      aPrims->AddVertex(X0,Y0,Z0);
      aPrims->AddVertex(X,Y,Z);
      gnopick->AddPrimitiveArray(aPrims);
      V3d::ArrowOfRadius(gExtArrow,X-.1*(X-X0),Y-.1*(Y-Y0),Z-.1*(Z-Z0),X-X0,Y-Y0,Z-Z0,M_PI/15.,Rayon/20.);
      V3d::ArrowOfRadius(gIntArrow,X0,Y0,Z0,X0-X,Y0-Y,Z0-Z,M_PI/15.,Rayon/20.);
      TCollection_AsciiString ValOfRadius(Rayon);
      Graphic3d_Vertex PText ( .5*(X0+X), .5*(Y0+Y), .5*(Z0+Z) );
      gradius->Text(ValOfRadius.ToCString(),PText,0.01);
    }
    
// Display of the meridian

    Quantity_Color Col2(Quantity_NOC_GREEN);
    Handle(Graphic3d_AspectLine3d) Asp2 = new Graphic3d_AspectLine3d(Col2,Aspect_TOL_SOLID,1.);
    gnopick->SetPrimitivesAspect(Asp2);
    
//    Definition of the axis of the circle
    theView->Up(DXRef,DYRef,DZRef);
    this->Position(X,Y,Z);
    DXini = X-X0; DYini = Y-Y0; DZini = Z-Z0;
    VX = DYRef*DZini - DZRef*DYini;
    VY = DZRef*DXini - DXRef*DZini;
    VZ = DXRef*DYini - DYRef*DXini;
    
    V3d::CircleInPlane(gnopick,X0,Y0,Z0,VX,VY,VZ,Rayon);

//    Display of the parallel

//    Definition of the axis of the circle
    theView->Proj(VX,VY,VZ);
    theView->Up(X1,Y1,Z1);
    DXRef = VY * Z1 - VZ * Y1;
    DYRef = VZ * X1 - VX * Z1;
    DZRef = VX * Y1 - VY * X1;
    this->Position(X,Y,Z);
    DXini = X-X0; DYini = Y-Y0; DZini = Z-Z0;
    VX = DYRef*DZini - DZRef*DYini;
    VY = DZRef*DXini - DXRef*DZini;
    VZ = DXRef*DYini - DYRef*DXini;
    
    V3d::CircleInPlane(gnopick,X0,Y0,Z0,VX,VY,VZ,Rayon);
  }
  
  myGraphicStructure->Connect(myGraphicStructure1,Graphic3d_TOC_DESCENDANT);
  myTypeOfRepresentation = Pres;
  myGraphicStructure->Display();
}
