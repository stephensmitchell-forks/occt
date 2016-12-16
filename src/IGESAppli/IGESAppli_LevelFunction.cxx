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

#include <IGESAppli_LevelFunction.hxx>
#include <TCollection_HAsciiString.hxx>
#include <IGESFile_Reader.hxx>
#include <IGESData_IGESWriter.hxx>
#include <Message_Messenger.hxx>
#include <IGESData_DirChecker.hxx>
#include <IGESData_IGESDumper.hxx>
#include <IGESData_Dump.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESAppli_LevelFunction,IGESData_IGESEntity)

void IGESAppli_LevelFunction::OwnRead (IGESFile_Reader &theReader)
{
  Standard_Integer aNbPropertyValues = 2;
  if (theReader.ReadInteger(aNbPropertyValues,"No. of Property values")  == IGESFile_Reader::ParamError || aNbPropertyValues != 2)
    theReader.AddFail("Number of Property Values != 2");

  myFuncDescripCode = 0;
  theReader.ReadInteger(myFuncDescripCode,"Function description code");

  theReader.ReadText(myFuncDescrip,"Function description");
}

void IGESAppli_LevelFunction::OwnWrite (IGESData_IGESWriter &IW) const
{
  IW.Send(2);
  IW.Send(myFuncDescripCode);
  if (myFuncDescrip.IsNull()) IW.SendVoid();
  else IW.Send(myFuncDescrip);
}

IGESData_DirChecker IGESAppli_LevelFunction::DirChecker () const
{
  IGESData_DirChecker DC(406,3);  //Form no = 3 & Type = 406
  DC.Structure(IGESData_DefVoid);
  DC.GraphicsIgnored();
  DC.BlankStatusIgnored();
  DC.UseFlagIgnored();
  DC.HierarchyStatusIgnored();
  DC.SubordinateStatusRequired(00);
  return DC;
}

void IGESAppli_LevelFunction::OwnDump (const IGESData_IGESDumper &, const Handle(Message_Messenger) &S, const Standard_Integer) const
{
  S << "IGESAppli_LevelFunction" << endl;
  S << "Number of property values : 2" << endl;
  S << "Function Description code : " << myFuncDescripCode << endl;
  S << "Function Description : ";
  IGESData_DumpString(S,myFuncDescrip);
  S << endl;
}
