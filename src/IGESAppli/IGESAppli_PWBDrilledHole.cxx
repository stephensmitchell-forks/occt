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
#include <IGESFile_Reader.hxx>
#include <IGESData_IGESWriter.hxx>
#include <Message_Messenger.hxx>
#include <IGESData_DirChecker.hxx>
#include <IGESData_IGESDumper.hxx>
#include <IGESData_Dump.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESAppli_PWBDrilledHole,IGESData_IGESEntity)

void IGESAppli_PWBDrilledHole::OwnRead (IGESFile_Reader &theReader)
{
  Standard_Integer aNbPropertyValues = 3;
  if (theReader.ReadInteger(aNbPropertyValues,"Number of property values") == IGESFile_Reader::ParamError || aNbPropertyValues != 3)
    theReader.AddFail("Number of property values != 3");
  theReader.ReadReal(myDrillDiameter,"Drill Diameter Size");
  theReader.ReadReal(myFinishDiameter,"Finish Diameter Size");
  theReader.ReadInteger(myFunctionCode,"Drilled Hole Function Code");
}

void IGESAppli_PWBDrilledHole::OwnWrite (IGESData_IGESWriter &IW) const
{
  IW.Send(3);
  IW.Send(myDrillDiameter);
  IW.Send(myFinishDiameter);
  IW.Send(myFunctionCode);
}

IGESData_DirChecker IGESAppli_PWBDrilledHole::DirChecker () const
{
  IGESData_DirChecker DC(406, 26);
  DC.Structure(IGESData_DefVoid);
  DC.GraphicsIgnored();
  DC.LineFont(IGESData_DefVoid);
  DC.LineWeight(IGESData_DefVoid);
  DC.Color(IGESData_DefVoid);
  DC.BlankStatusIgnored();
  DC.UseFlagIgnored();
  DC.HierarchyStatusIgnored();
  return DC;
}

void IGESAppli_PWBDrilledHole::OwnCheck (const Interface_ShareTool &, const Handle(Interface_Check) &theCheck) const
{
  if ( (myFunctionCode < 1) ||
      ((myFunctionCode > 5) && (myFunctionCode < 5001)) ||
      (myFunctionCode > 9999))
    theCheck->AddFail("Drilled Hole Function Code != 1-5,5001-9999");
}

void IGESAppli_PWBDrilledHole::OwnDump (const IGESData_IGESDumper &, const Handle(Message_Messenger) &S, const Standard_Integer) const
{
  S << "IGESAppli_PWBDrilledHole" << endl;
  S << "Number of property values : 3" << endl;
  S << "Drill Diameter Size  : " << myDrillDiameter << endl;
  S << "Finish Diameter Size : " << myFinishDiameter << endl;
  S << "Drilled Hole Function Code : " << myFunctionCode << endl;
}
