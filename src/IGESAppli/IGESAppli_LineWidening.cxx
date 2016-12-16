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
#include <IGESFile_Reader.hxx>
#include <IGESData_IGESWriter.hxx>
#include <Message_Messenger.hxx>
#include <IGESData_DirChecker.hxx>
#include <IGESData_IGESDumper.hxx>
#include <IGESData_Dump.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESAppli_LineWidening,IGESData_IGESEntity)

void IGESAppli_LineWidening::OwnRead (IGESFile_Reader &theReader)
{
  Standard_Integer aNbPropertyValues = 5;
  if (theReader.ReadInteger(aNbPropertyValues,"No. of Property values") == IGESFile_Reader::ParamError || aNbPropertyValues != 5)
    theReader.AddFail("Number of Property Values != 5");
  theReader.ReadReal(myWidth,"Width of metalization");
  theReader.ReadInteger(myCorneringCode,"Cornering code");
  theReader.ReadInteger(myExtensionFlag,"Extension Flag");
  theReader.ReadInteger(myJustificationFlag,"Justification Flag");
  myExtensionValue = 0.;
  if (theReader.ReadReal(myExtensionValue,"Extension value") != IGESFile_Reader::ParamOK)
    if (myExtensionFlag == 2)
      theReader.AddFail("Extension Value not defined while Extension Flag = 2");
}

void IGESAppli_LineWidening::OwnWrite (IGESData_IGESWriter &IW) const
{
  IW.Send(5);
  IW.Send(myWidth);
  IW.Send(myCorneringCode);
  IW.Send(myExtensionFlag);
  IW.Send(myJustificationFlag);
  IW.Send(myExtensionValue);
}

IGESData_DirChecker IGESAppli_LineWidening::DirChecker () const
{
  IGESData_DirChecker DC(406,5);  //Form no = 5 & Type = 406
  DC.Structure(IGESData_DefVoid);
  DC.GraphicsIgnored();
  DC.BlankStatusIgnored();
  DC.UseFlagIgnored();
  DC.HierarchyStatusIgnored();
  return DC;
}

void IGESAppli_LineWidening::OwnCheck (const Interface_ShareTool &, const Handle(Interface_Check) &theCheck) const
{
  if (SubordinateStatus() != 0)
    if (DefLevel() == IGESData_DefOne ||
	    DefLevel() == IGESData_DefSeveral)
      theCheck->AddWarning("Level type: defined while ignored");

  if (myCorneringCode != 0 && myCorneringCode != 1)
    theCheck->AddFail("Cornering Code incorrect");
  if (myExtensionFlag < 0 || myExtensionFlag > 2)
    theCheck->AddFail("Extension Flag value incorrect");
  if (myJustificationFlag < 0 || myJustificationFlag > 2)
    theCheck->AddFail("Justification Flag value incorrect");
}

Standard_Boolean IGESAppli_LineWidening::OwnCorrect ()
{
  if (SubordinateStatus() != 0) {
    Handle(IGESData_LevelListEntity) nulevel;
    InitLevel(nulevel,0);
    return Standard_True;
  }
  return Standard_False;
}

void IGESAppli_LineWidening::OwnDump (const IGESData_IGESDumper &, const Handle(Message_Messenger) &S, const Standard_Integer) const
{
  S << "IGESAppli_LineWidening" << endl;
  S << "Number of property values : 5" << endl;
  S << "Width of metalization : " << myWidth << endl;

  S << "Cornering Code : " ;
  if      (myCorneringCode == 0) S << "0 (rounded)" << endl;
  else if (myCorneringCode == 1) S << "1 (squared)" << endl;
  else    S << "incorrect value" << endl;

  S << "Extension Flag : " ;
  if      (myExtensionFlag == 0) S << "0 (No Extension)" << endl;
  else if (myExtensionFlag == 1) S << "1 (One-half width extension)" << endl;
  else if (myExtensionFlag == 2) S << "2 (Extension set by ExtensionValue)" << endl;
  else    S << "incorrect value" << endl;

  S << "Justification Flag : " ;
  if      (myJustificationFlag == 0) S << "0 (centre justified)" << endl;
  else if (myJustificationFlag == 1) S << "1 (left justified)" << endl;
  else if (myJustificationFlag == 2) S << "2 (right justified)" << endl;
  else    S << "incorrect value" << endl;

  if (myExtensionFlag == 2)
    S << "Extension Value : " << myExtensionValue << endl;
  else
    S << "No Extension Value (Extension Flag != 2)" << endl;
}
