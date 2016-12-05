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

#include <IGESAppli_LineWidening.hxx>
#include <IGESData_LevelListEntity.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESAppli_LineWidening,IGESData_IGESEntity)

void IGESAppli_LineWidening::Init
  (const Standard_Integer nbPropVal,
   const Standard_Real    aWidth,    const Standard_Integer aCornering,
   const Standard_Integer aExtnFlag, const Standard_Integer aJustifFlag,
   const Standard_Real    aExtnVal)
{
  theNbPropertyValues  = nbPropVal;
  theWidth             = aWidth;
  theCorneringCode     = aCornering;
  theExtensionFlag     = aExtnFlag;
  theJustificationFlag = aJustifFlag;
  theExtensionValue    = aExtnVal;
  InitTypeAndForm(406,5);
}

void IGESAppli_LineWidening::OwnCheck (const Interface_ShareTool &, const Handle(Interface_Check) &theCheck) const
{
  if (SubordinateStatus() != 0)
    if (DefLevel() == IGESData_DefOne ||
	    DefLevel() == IGESData_DefSeveral)
      theCheck->AddWarning("Level type: defined while ignored");
  if (NbPropertyValues() != 5)
    theCheck->AddFail("Number of Property Values != 5");
  if (CorneringCode() != 0 && CorneringCode() != 1)
    theCheck->AddFail("Cornering Code incorrect");
  if (ExtensionFlag() < 0 || ExtensionFlag() > 2)
    theCheck->AddFail("Extension Flag value incorrect");
  if (JustificationFlag() < 0 || JustificationFlag() > 2)
    theCheck->AddFail("Justification Flag value incorrect");
}
