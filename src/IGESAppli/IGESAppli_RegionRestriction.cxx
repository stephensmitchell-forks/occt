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

#include <IGESAppli_RegionRestriction.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESAppli_RegionRestriction,IGESData_IGESEntity)

void IGESAppli_RegionRestriction::Init
  (const Standard_Integer nbPropVal,  const Standard_Integer aViasRest,
   const Standard_Integer aCompoRest, const Standard_Integer aCktRest)
{
  theNbPropertyValues  = nbPropVal;
  theElectViasRestrict = aViasRest;
  theElectCompRestrict = aCompoRest;
  theElectCktRestrict  = aCktRest;
  InitTypeAndForm(406,2);
}

void IGESAppli_RegionRestriction::OwnCheck (const Interface_ShareTool &, const Handle(Interface_Check) &theCheck) const
{
  if (SubordinateStatus() != 0)
    if (DefLevel() != IGESData_DefOne &&
	    DefLevel() != IGESData_DefSeveral)
      theCheck->AddFail("Level type: Not value/reference");
  if (NbPropertyValues() != 3)
    theCheck->AddFail("Number of Property Values != 3");
  if (ElectricalViasRestriction() < 0 || ElectricalViasRestriction() > 2)
    theCheck->AddFail("Incorrect value for Electrical Vias Restriction");
  if (ElectricalComponentRestriction() < 0 || ElectricalComponentRestriction() > 2)
    theCheck->AddFail("Incorrect value for Electrical Component Restriction");
  if (ElectricalCktRestriction() < 0 || ElectricalCktRestriction() > 2)
    theCheck->AddFail("Incorrect value for Electrical Circuit Restriction");
  //UNFINISHED
  //level ignored if this property is subordinate -- queried
}
