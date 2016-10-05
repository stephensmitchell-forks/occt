// Created on: 1994-06-03
// Created by: Christian CAILLET
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

#ifndef _XSDRAWIGES_Activator_HeaderFile
#define _XSDRAWIGES_Activator_HeaderFile

#include <IFSelect_Activator.hxx>

//! Performs Actions specific to IGESSelect, i.e. creation of
//! IGES Selections and Dispatches, plus dumping specific to IGES
class XSDRAWIGES_Activator : public IFSelect_Activator
{
 public:
  
  Standard_EXPORT XSDRAWIGES_Activator();
  
  //! Executes a Command Line for IGESSelect
  Standard_EXPORT IFSelect_ReturnStatus Do (const Standard_Integer number, const Handle(IFSelect_SessionPilot)& pilot) Standard_OVERRIDE;
  
  //! Sends a short help message for IGESSelect commands
  Standard_EXPORT Standard_CString Help (const Standard_Integer number) const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(XSDRAWIGES_Activator,IFSelect_Activator)
};

#endif // _XSDRAWIGES_Activator_HeaderFile
