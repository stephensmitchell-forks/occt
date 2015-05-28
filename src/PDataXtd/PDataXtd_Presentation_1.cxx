// Created on: 1999-07-08
// Created by: srn
// Copyright (c) 1999-1999 Matra Datavision
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

#include <PDataXtd_Presentation_1.ixx>


//=======================================================================
//function : PDataXtd_Presentation_1
//purpose  : 
//=======================================================================

PDataXtd_Presentation_1::PDataXtd_Presentation_1() :
    myIsDisplayed(Standard_False),
    myTransparency(0.),
    myColor(0),
    myMaterial(0),
    myWidth(0.),
    myMode(0)
{
}


//=======================================================================
//function : SetDisplayed
//purpose  : 
//=======================================================================

void PDataXtd_Presentation_1::SetDisplayed (const Standard_Boolean B)
{
  myIsDisplayed = B ;
}

//=======================================================================
//function : IsDisplayed
//purpose  : 
//=======================================================================

Standard_Boolean PDataXtd_Presentation_1::IsDisplayed() const 
{
    return myIsDisplayed;
}


//=======================================================================
//function : SetDriverGUID
//purpose  : 
//=======================================================================
void PDataXtd_Presentation_1::SetDriverGUID( const Handle(PCollection_HExtendedString)&  guid)
{
  myDriverGUID = guid;
}

//=======================================================================
//function : GetDriverGUID
//purpose  : 
//=======================================================================
Handle(PCollection_HExtendedString) PDataXtd_Presentation_1::GetDriverGUID( void ) const
{
  return myDriverGUID;
}

//=======================================================================
//function : Color
//purpose  : 
//=======================================================================
Standard_Integer PDataXtd_Presentation_1::Color() const
{
  return myColor;
}

//=======================================================================
//function : SetColor
//purpose  : 
//=======================================================================
void PDataXtd_Presentation_1::SetColor(const Standard_Integer C) 
{
  myColor = C;
}

//=======================================================================
//function : Transparency
//purpose  : 
//=======================================================================
Standard_Real PDataXtd_Presentation_1::Transparency() const
{
  return myTransparency;
}

//=======================================================================
//function : SetTransparency
//purpose  : 
//=======================================================================
void PDataXtd_Presentation_1::SetTransparency(const Standard_Real T) 
{
  myTransparency = T;
}

//=======================================================================
//function : Material
//purpose  : 
//=======================================================================
Standard_Integer PDataXtd_Presentation_1::Material() const
{
  return myMaterial;
}

//=======================================================================
//function : SetMaterial
//purpose  : 
//=======================================================================
void PDataXtd_Presentation_1::SetMaterial(const Standard_Integer M) 
{
   myMaterial = M;
}

//=======================================================================
//function : Width 
//purpose  : 
//=======================================================================
Standard_Real PDataXtd_Presentation_1::Width() const
{
  return myWidth;  
}




//=======================================================================
//function : SetWidth
//purpose  : 
//=======================================================================

void PDataXtd_Presentation_1::SetWidth(const Standard_Real W) 
{
  myWidth = W;
}



//=======================================================================
//function : Mode
//purpose  : 
//=======================================================================

Standard_Integer PDataXtd_Presentation_1::Mode() const
{
  return myMode;
}


//=======================================================================
//function : SetMode
//purpose  : 
//=======================================================================

void PDataXtd_Presentation_1::SetMode(const Standard_Integer M) 
{
   myMode = M;
}
