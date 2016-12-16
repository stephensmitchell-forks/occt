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
#include <IGESFile_Reader.hxx>
#include <IGESData_IGESWriter.hxx>
#include <Message_Messenger.hxx>
#include <IGESData_DirChecker.hxx>
#include <IGESData_IGESDumper.hxx>
#include <IGESData_Dump.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESAppli_RegionRestriction,IGESData_IGESEntity)

void IGESAppli_RegionRestriction::OwnRead (IGESFile_Reader &theReader)
{
  Standard_Integer aNbPropertyValues = 3;
  if (theReader.ReadInteger(aNbPropertyValues,"No. of Property values") == IGESFile_Reader::ParamError || aNbPropertyValues != 3)
    theReader.AddFail("Number of Property Values != 3");
  theReader.ReadInteger(myElectViasRestrict,"Electrical vias restriction");
  theReader.ReadInteger(myElectCompRestrict,"Electrical components restriction");
  theReader.ReadInteger(myElectCktRestrict," Electrical circuitary restriction");
}

void IGESAppli_RegionRestriction::OwnWrite (IGESData_IGESWriter &IW) const
{
  IW.Send(3);
  IW.Send(myElectViasRestrict);
  IW.Send(myElectCompRestrict);
  IW.Send(myElectCktRestrict);
}

IGESData_DirChecker IGESAppli_RegionRestriction::DirChecker () const
{
  IGESData_DirChecker DC(406,2);  //Form no = 2 & Type = 406
  DC.Structure(IGESData_DefVoid);
  DC.GraphicsIgnored();
  DC.BlankStatusIgnored();
  DC.UseFlagIgnored();
  DC.HierarchyStatusIgnored();
  return DC;
}

void IGESAppli_RegionRestriction::OwnCheck (const Interface_ShareTool &, const Handle(Interface_Check) &theCheck) const
{
  if (SubordinateStatus() != 0)
    if (DefLevel() != IGESData_DefOne &&
	    DefLevel() != IGESData_DefSeveral)
      theCheck->AddFail("Level type: Not value/reference");

  if (myElectViasRestrict < 0 || myElectViasRestrict > 2)
    theCheck->AddFail("Incorrect value for Electrical Vias Restriction");
  if (myElectCompRestrict < 0 || myElectCompRestrict > 2)
    theCheck->AddFail("Incorrect value for Electrical Component Restriction");
  if (myElectCktRestrict < 0 || myElectCktRestrict > 2)
    theCheck->AddFail("Incorrect value for Electrical Circuit Restriction");
  //UNFINISHED
  //level ignored if this property is subordinate -- queried
}

Standard_Boolean IGESAppli_RegionRestriction::OwnCorrect ()
{
  if (SubordinateStatus() != 0) {
    Handle(IGESData_LevelListEntity) nulevel;
    InitLevel(nulevel,0);
    return Standard_True;
  }
  return Standard_False;
}

void IGESAppli_RegionRestriction::OwnDump (const IGESData_IGESDumper &, const Handle(Message_Messenger) &S, const Standard_Integer) const
{
  S << "IGESAppli_RegionRestriction" << endl;
  S << "Number of property values : 3" << endl;
  S << "Electrical vias restriction       : " << myElectViasRestrict << endl;
  S << "Electrical components restriction : " << myElectCompRestrict << endl;
  S << "Electrical circuitary restriction : " << myElectCktRestrict  << endl;
}
