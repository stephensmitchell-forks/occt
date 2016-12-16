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

#include <IGESAppli_FlowLineSpec.hxx>
#include <Standard_DimensionMismatch.hxx>
#include <Standard_OutOfRange.hxx>
#include <TCollection_HAsciiString.hxx>
#include <IGESFile_Reader.hxx>
#include <IGESData_IGESWriter.hxx>
#include <Message_Messenger.hxx>
#include <IGESData_DirChecker.hxx>
#include <IGESData_IGESDumper.hxx>
#include <IGESData_Dump.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESAppli_FlowLineSpec,IGESData_IGESEntity)

Standard_Integer IGESAppli_FlowLineSpec::NbPropertyValues () const
{
  return myNameAndModifiers->Length();
}

const Handle(TCollection_HAsciiString) & IGESAppli_FlowLineSpec::FlowLineName () const
{
  return myNameAndModifiers->Value(1);
}

const Handle(TCollection_HAsciiString) & IGESAppli_FlowLineSpec::Modifier (const Standard_Integer Index) const
{
  return myNameAndModifiers->Value(Index);
}

void IGESAppli_FlowLineSpec::OwnRead (IGESFile_Reader &theReader)
{
  Standard_Integer num = 0;
  theReader.ReadInteger(num,"Number of property values");
  if (num > 0)
  {
    myNameAndModifiers = new Interface_HArray1OfHAsciiString(1, num);
    theReader.ReadTexts(myNameAndModifiers,num,"Name and Modifiers");
  }
  else theReader.AddFail("Number of property values: Not Positive");
}

void IGESAppli_FlowLineSpec::OwnWrite (IGESData_IGESWriter &IW) const
{
  const Standard_Integer num = myNameAndModifiers->Length();
  IW.Send(num);
  for ( Standard_Integer i = 1; i <= num; i++ )
    IW.Send(myNameAndModifiers->Value(i));
}

IGESData_DirChecker IGESAppli_FlowLineSpec::DirChecker () const
{
  IGESData_DirChecker DC(406, 14);
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

void IGESAppli_FlowLineSpec::OwnDump (const IGESData_IGESDumper &, const Handle(Message_Messenger) &S, const Standard_Integer level) const
{
  S << "IGESAppli_FlowLineSpec" << endl;
  S << "Name and Modifiers : ";
  IGESData_DumpStrings(S,level,1,NbPropertyValues(),Modifier);
  S << endl;
}
