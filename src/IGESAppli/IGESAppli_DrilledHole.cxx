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
#include <IGESFile_Reader.hxx>
#include <IGESData_IGESWriter.hxx>
#include <IGESData_DirChecker.hxx>
#include <Message_Messenger.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESAppli_DrilledHole,IGESData_IGESEntity)

void IGESAppli_DrilledHole::OwnRead (IGESFile_Reader &theReader)
{
  Standard_Integer aNbPropertyValues = 5;
  if (theReader.ReadInteger(aNbPropertyValues,"No. of Property values") == IGESFile_Reader::ParamError || aNbPropertyValues != 5)
    theReader.AddFail("Number of Property Values != 5");
  theReader.ReadReal(myDrillDiaSize,"Drill diameter size");
  theReader.ReadReal(myFinishDiaSize,"Finish diameter size");
  theReader.ReadInteger(myPlatingFlag,"Plating Flag");
  theReader.ReadInteger(myNbLowerLayer,"Lower numbered layer");
  theReader.ReadInteger(myNbHigherLayer,"Higher numbered layer");
}

void IGESAppli_DrilledHole::OwnWrite (IGESData_IGESWriter &IW) const
{
  IW.Send(5);
  IW.Send(myDrillDiaSize);
  IW.Send(myFinishDiaSize);
  IW.SendBoolean(myPlatingFlag != 0);
  IW.Send(myNbLowerLayer);
  IW.Send(myNbHigherLayer);
}


IGESData_DirChecker IGESAppli_DrilledHole::DirChecker () const
{
  IGESData_DirChecker DC(406,6);  //Form no = 6 & Type = 406
  DC.Structure(IGESData_DefVoid);
  DC.GraphicsIgnored();
  DC.BlankStatusIgnored();
  DC.UseFlagIgnored();
  DC.HierarchyStatusIgnored();
  return DC;
}

void IGESAppli_DrilledHole::OwnCheck (const Interface_ShareTool &, const Handle(Interface_Check) &theCheck) const
{
  if (SubordinateStatus() != 0)
    if (DefLevel() != IGESData_DefOne &&
	    DefLevel() != IGESData_DefSeveral)
      theCheck->AddFail("Level type : Not value/reference");
}
  
Standard_Boolean IGESAppli_DrilledHole::OwnCorrect ()
{
  if (SubordinateStatus() != 0) {
    Handle(IGESData_LevelListEntity) nulevel;
    InitLevel(nulevel,0);
    return Standard_True;
  }
  return Standard_False;
}

void IGESAppli_DrilledHole::OwnDump (const IGESData_IGESDumper &, const Handle(Message_Messenger) &S, const Standard_Integer) const
{
  S << "IGESAppli_DrilledHole" << endl;
  S << "Number of property values : 5" << endl;
  S << "Drill  diameter size : " << myDrillDiaSize << "  ";
  S << "Finish diameter size : " << myFinishDiaSize << endl;
  S << "Plating indication flag : " << (myPlatingFlag? "YES" : "NO") << "  -  ";
  S << "Lower Numbered Layer  : " << myNbLowerLayer << "  ";
  S << "Higher Numbered Layer : " << myNbHigherLayer << endl;
}
