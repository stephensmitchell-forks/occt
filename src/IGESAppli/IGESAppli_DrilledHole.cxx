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

#include <IGESAppli_DrilledHole.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESAppli_DrilledHole,IGESData_IGESEntity)

void IGESAppli_DrilledHole::Init
  (const Standard_Integer nbPropVal,
   const Standard_Real    aSize,    const Standard_Real    anotherSize,
   const Standard_Integer aPlating, const Standard_Integer aLayer,
   const Standard_Integer anotherLayer)
{
  theNbPropertyValues = nbPropVal;
  theDrillDiaSize     = aSize;
  theFinishDiaSize    = anotherSize;
  thePlatingFlag      = aPlating;
  theNbLowerLayer     = aLayer;
  theNbHigherLayer    = anotherLayer;
  InitTypeAndForm(406,6);
}

void IGESAppli_DrilledHole::OwnCheck (const Interface_ShareTool &, const Handle(Interface_Check) &theCheck) const
{
  if (SubordinateStatus() != 0)
    if (DefLevel() != IGESData_DefOne &&
	    DefLevel() != IGESData_DefSeveral)
      theCheck->AddFail("Level type : Not value/reference");
  if (NbPropertyValues() != 5)
    theCheck->AddFail("Number of Property Values != 5");
}
