// Created on:
// Created by:
// Copyright (c)  2017 OPEN CASCADE SAS
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

#ifndef _Draw_Report_HeaderFile
#define _Draw_Report_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Message_Report.hxx>
#include <Draw_Drawable3D.hxx>
#include <Standard_OStream.hxx>
#include <Draw_Interpretor.hxx>

#include <Draw_Display.hxx>

class Draw_Report;
DEFINE_STANDARD_HANDLE(Draw_Report, Draw_Drawable3D)

//! To store message report.
class Draw_Report : public Draw_Drawable3D
{

public:

  Standard_EXPORT Draw_Report (const Handle(Message_Report)& theReport) : myReport (theReport) {}
  
  Standard_EXPORT const Handle(Message_Report)& GetReport() const { return myReport; }
  
  Standard_EXPORT void SetReport (const Handle(Message_Report)& theReport) { myReport = theReport; }
  
  //! Does nothhing.
  Standard_EXPORT void DrawOn (Draw_Display& dis) const Standard_OVERRIDE { (void)dis; }
  
  //! For variable copy.
  Standard_EXPORT virtual Handle(Draw_Drawable3D) Copy() const Standard_OVERRIDE;
  
  //! For variable dump.
  Standard_EXPORT virtual void Dump (Standard_OStream& S) const Standard_OVERRIDE;
  
  //! For variable whatis command. Set  as a result  the
  //! type of the variable.
  Standard_EXPORT virtual void Whatis (Draw_Interpretor& I) const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(Draw_Report,Draw_Drawable3D)

private:
  Handle(Message_Report) myReport;
};







#endif // _Draw_Report_HeaderFile
