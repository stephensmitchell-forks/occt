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

#include <IGESAppli_PinNumber.hxx>
#include <TCollection_HAsciiString.hxx>
#include <IGESFile_Reader.hxx>
#include <IGESData_IGESWriter.hxx>
#include <Message_Messenger.hxx>
#include <IGESData_DirChecker.hxx>
#include <IGESData_IGESDumper.hxx>
#include <IGESData_Dump.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESAppli_PinNumber,IGESData_IGESEntity)

void IGESAppli_PinNumber::OwnRead (IGESFile_Reader &theReader)
{
  Standard_Integer aNbPropertyValues = 1;
  if (theReader.ReadInteger(aNbPropertyValues,"Number of property values") == IGESFile_Reader::ParamError || aNbPropertyValues != 1)
    theReader.AddFail("Number of Property Values != 1");
  theReader.ReadText(myPinNumber,"PinNumber");
}

void IGESAppli_PinNumber::OwnWrite (IGESData_IGESWriter &IW) const
{
  IW.Send(1);
  IW.Send(myPinNumber);
}

IGESData_DirChecker IGESAppli_PinNumber::DirChecker () const
{
  IGESData_DirChecker DC(406,8);  //Form no = 8 & Type = 406
  DC.Structure(IGESData_DefVoid);
  DC.GraphicsIgnored();
  DC.BlankStatusIgnored();
  DC.UseFlagIgnored();
  DC.HierarchyStatusIgnored();
  return DC;
}

void IGESAppli_PinNumber::OwnCheck (const Interface_ShareTool &, const Handle(Interface_Check) &theCheck) const
{
  if (SubordinateStatus() != 0)
    if (DefLevel() != IGESData_DefOne &&
	    DefLevel() != IGESData_DefSeveral)
      theCheck->AddFail("Level type: Incorrect");
}

Standard_Boolean IGESAppli_PinNumber::OwnCorrect ()
{
  if (SubordinateStatus() != 0) {
    Handle(IGESData_LevelListEntity) nulevel;
    InitLevel(nulevel,0);
    return Standard_True;
  }
  return Standard_False;
}

void IGESAppli_PinNumber::OwnDump (const IGESData_IGESDumper &, const Handle(Message_Messenger) &S, const Standard_Integer) const
{
  S << "IGESAppli_PinNumber" << endl;
  S << "Number of Property Values : 1" << endl;
  S << "PinNumber : ";
  IGESData_DumpString(S,myPinNumber);
  S << endl;
}
