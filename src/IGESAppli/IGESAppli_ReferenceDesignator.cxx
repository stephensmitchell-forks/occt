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

#include <IGESAppli_ReferenceDesignator.hxx>
#include <IGESData_LevelListEntity.hxx>
#include <Standard_Type.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESAppli_ReferenceDesignator,IGESData_IGESEntity)

void IGESAppli_ReferenceDesignator::Init
  (const Standard_Integer nbPropVal,
   const Handle(TCollection_HAsciiString)& aText)
{
  theRefDesigText     = aText;
  theNbPropertyValues = nbPropVal;
  InitTypeAndForm(406,7);
}

void IGESAppli_ReferenceDesignator::OwnCheck (const Interface_ShareTool &, const Handle(Interface_Check) &theCheck) const
{
  if (SubordinateStatus() != 0)
    //the level is ignored if this property is subordinate
    if (DefLevel() != IGESData_DefOne &&
	    DefLevel() != IGESData_DefSeveral)
      theCheck->AddFail("Level type: Not value/reference");
  if (NbPropertyValues() != 1)
    theCheck->AddFail("Number of Property Values != 1");
  //UNFINISHED
  //the level is ignored if this property is subordinate -- queried
}
