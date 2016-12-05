// Created by: CKY / Contract Toubro-Larsen
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

//--------------------------------------------------------------------
//--------------------------------------------------------------------

#include <IGESAppli_PWBDrilledHole.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESAppli_PWBDrilledHole,IGESData_IGESEntity)

void IGESAppli_PWBDrilledHole::Init
  (const Standard_Integer nbPropVal,
   const Standard_Real aDrillDia, const Standard_Real aFinishDia,
   const Standard_Integer aCode)
{
  theNbPropertyValues = nbPropVal;
  theDrillDiameter    = aDrillDia;
  theFinishDiameter   = aFinishDia;
  theFunctionCode     = aCode;
  InitTypeAndForm(406,26);
}

void IGESAppli_PWBDrilledHole::OwnCheck (const Interface_ShareTool &, const Handle(Interface_Check) &theCheck) const
{
  if (NbPropertyValues() != 3)
    theCheck->AddFail("Number of property values != 3");
  if ( (FunctionCode() < 1) ||
      ((FunctionCode() > 5) && (FunctionCode() < 5001)) ||
      (FunctionCode() > 9999))
    theCheck->AddFail("Drilled Hole Function Code != 1-5,5001-9999");
}
