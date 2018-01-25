// Created on:
// Created by:
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

#include <Draw_Report.hxx>

#include <Draw_Display.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Draw_Report,Draw_Drawable3D)

//=======================================================================
//function : Copy
//purpose  :
//=======================================================================
Handle(Draw_Drawable3D)  Draw_Report::Copy()const 
{
  Handle(Draw_Report) aReport = new Draw_Report (myReport);
  return aReport;
}

//=======================================================================
//function : Dump
//purpose  :
//=======================================================================
void  Draw_Report::Dump (Standard_OStream& S)const 
{
  (void)S;
  //S << myReport;
}


//=======================================================================
//function : Whatis
//purpose  : 
//=======================================================================
void  Draw_Report::Whatis (Draw_Interpretor& S)const 
{
  S << "Draw_Report";
}
