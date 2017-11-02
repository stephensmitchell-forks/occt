// Copyright (c) 2017 OPEN CASCADE SAS
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

#include <Graphic3d_CLight.hxx>

#include <Standard_OutOfRange.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Graphic3d_CLight, Standard_Transient)

// =======================================================================
// function : Graphic3d_CLight
// purpose  :
// =======================================================================
Graphic3d_CLight::Graphic3d_CLight (Graphic3d_TypeOfLightSource theType)
: myPosition   (0.0,  0.0,  0.0),
  myColor      (1.0f, 1.0f, 1.0f, 1.0f),
  myDirection  (0.0f, 0.0f, 0.0f, 0.0f),
  myParams     (0.0f, 0.0f, 0.0f, 0.0f),
  mySmoothness (0.0f),
  myIntensity  (1.0f),
  myType       (theType),
  myIsHeadlight(false),
  myIsEnabled  (true)
{
  //
}

// =======================================================================
// function : SetDirection
// purpose  :
// =======================================================================
void Graphic3d_CLight::SetDirection (const Graphic3d_Vec3& theDirection)
{
  Standard_ProgramError_Raise_if (myType != Graphic3d_TOLS_SPOT
                               && myType != Graphic3d_TOLS_DIRECTIONAL,
                                  "Graphic3d_CLight::SetDirection(), incorrect light type");
  myDirection.x() = theDirection.x();
  myDirection.y() = theDirection.y();
  myDirection.z() = theDirection.z();
}

// =======================================================================
// function : SetPosition
// purpose  :
// =======================================================================
void Graphic3d_CLight::SetPosition (const Graphic3d_Vec3d& thePosition)
{
  Standard_ProgramError_Raise_if (myType != Graphic3d_TOLS_SPOT
                               && myType != Graphic3d_TOLS_POSITIONAL,
                                  "Graphic3d_CLight::SetDirection(), incorrect light type");
  myPosition = thePosition;
}

// =======================================================================
// function : SetIntensity
// purpose  :
// =======================================================================
void Graphic3d_CLight::SetIntensity (Standard_ShortReal theValue)
{
  Standard_OutOfRange_Raise_if (theValue <= 0.0f, "Graphic3d_CLight::SetIntensity(), Negative value for intensity");
  myIntensity = theValue;
}

// =======================================================================
// function : SetAngle
// purpose  :
// =======================================================================
void Graphic3d_CLight::SetAngle (Standard_ShortReal theAngle)
{
  Standard_ProgramError_Raise_if (myType != Graphic3d_TOLS_SPOT,
                                  "Graphic3d_CLight::SetAngle(), incorrect light type");
  Standard_OutOfRange_Raise_if (theAngle <= 0.0 || theAngle >= M_PI,
                                "Graphic3d_CLight::SetAngle(), bad angle");
  changeAngle() = theAngle;
}

// =======================================================================
// function : SetConstAttenuation
// purpose  :
// =======================================================================
void Graphic3d_CLight::SetConstAttenuation (Standard_ShortReal theConstAttenuation)
{
  Standard_ProgramError_Raise_if (myType != Graphic3d_TOLS_POSITIONAL
                               && myType != Graphic3d_TOLS_SPOT,
                                  "Graphic3d_CLight::SetConstAttenuation(), incorrect light type");
  Standard_OutOfRange_Raise_if (theConstAttenuation < 0.0f, "Graphic3d_CLight::SetAttenuation(), bad coefficient");
  changeConstAttenuation() = theConstAttenuation;
}

// =======================================================================
// function : SetLinearAttenuation
// purpose  :
// =======================================================================
void Graphic3d_CLight::SetLinearAttenuation (Standard_ShortReal theLinearAttenuation)
{
  Standard_ProgramError_Raise_if (myType != Graphic3d_TOLS_POSITIONAL
                               && myType != Graphic3d_TOLS_SPOT,
                                  "Graphic3d_CLight::SetLinearAttenuation(), incorrect light type");
  Standard_OutOfRange_Raise_if (theLinearAttenuation < 0.0f, "Graphic3d_CLight::SetLinearAttenuation(), bad coefficient");
  changeLinearAttenuation() = theLinearAttenuation;
}

// =======================================================================
// function : SetConcentration
// purpose  :
// =======================================================================
void Graphic3d_CLight::SetConcentration (Standard_ShortReal theConcentration)
{
  Standard_ProgramError_Raise_if (myType != Graphic3d_TOLS_SPOT, "Graphic3d_CLight::SetConcentration(), incorrect light type");
  Standard_OutOfRange_Raise_if (theConcentration < 0.0f || theConcentration > 1.0f,
                                "Graphic3d_CLight::SetConcentration(), bad coefficient");
  changeConcentration() = theConcentration;
}

// =======================================================================
// function : SetSmoothRadius
// purpose  :
// =======================================================================
void Graphic3d_CLight::SetSmoothRadius (Standard_ShortReal theValue)
{
  Standard_ProgramError_Raise_if (myType != Graphic3d_TOLS_POSITIONAL
                               && myType != Graphic3d_TOLS_SPOT,
                                  "Graphic3d_CLight::SetSmoothRadius(), incorrect light type");
  Standard_OutOfRange_Raise_if (theValue < 0.0f, "Graphic3d_CLight::SetSmoothRadius(), Bad value for smoothing radius");
  mySmoothness = theValue;
}

// =======================================================================
// function : SetSmoothAngle
// purpose  :
// =======================================================================
void Graphic3d_CLight::SetSmoothAngle (Standard_ShortReal theValue)
{
  Standard_ProgramError_Raise_if (myType != Graphic3d_TOLS_DIRECTIONAL,
                                  "Graphic3d_CLight::SetSmoothAngle(), incorrect light type");
  Standard_OutOfRange_Raise_if (theValue < 0.0f || theValue > Standard_ShortReal(M_PI / 2.0),
                                "Graphic3d_CLight::SetSmoothAngle(), Bad value for smoothing angle");
  mySmoothness = theValue;
}
