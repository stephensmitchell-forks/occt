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

#include <IGESDefs_MacroDef.hxx>
#include <Standard_DimensionMismatch.hxx>
#include <Standard_OutOfRange.hxx>
#include <TCollection_HAsciiString.hxx>
#include <Interface_HArray1OfHAsciiString.hxx>
#include <IGESFile_Reader.hxx>
#include <IGESData_IGESWriter.hxx>
#include <IGESData_DirChecker.hxx>
#include <Message_Messenger.hxx>
#include <IGESData_IGESDumper.hxx>
#include <IGESData_Dump.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESDefs_MacroDef,IGESData_IGESEntity)

Standard_Integer IGESDefs_MacroDef::NbStatements () const
{
  return myLangStatements->Length();
}

const Handle(TCollection_HAsciiString) & IGESDefs_MacroDef::LanguageStatement (const Standard_Integer StatNum) const
{
  return myLangStatements->Value(StatNum);
}

void IGESDefs_MacroDef::OwnRead (IGESFile_Reader &theReader)
{ 
  theReader.ReadText(myMACRO,"MACRO");
  theReader.ReadInteger(myEntityTypeID,"Entity Type ID");

  NCollection_Sequence<Handle(TCollection_HAsciiString)> seq;
  Handle(TCollection_HAsciiString) langStat;

  while (theReader.HasData())
    if (theReader.ReadText(langStat,"Language Statement") == IGESFile_Reader::ParamOK)
      seq.Append(langStat);

  const Standard_Integer nbval = seq.Length();
  if (nbval > 1)
  {
    myLangStatements = new Interface_HArray1OfHAsciiString(1, nbval-1);
    for (Standard_Integer i = 1; i < nbval; i++)
      myLangStatements->SetValue(i,seq.Value(i));
  }
  else  theReader.AddFail("Number of Lang. Stats. : Not Positive");

  if (nbval > 0)
    myENDMACRO = seq.Last();
}

void IGESDefs_MacroDef::OwnWrite (IGESData_IGESWriter& IW) const
{ 
  IW.Send(myMACRO);
  IW.Send(myEntityTypeID);
  const Standard_Integer upper = NbStatements();
  for (Standard_Integer i = 1; i <= upper; i++)
    IW.Send(LanguageStatement(i));
  IW.Send(myENDMACRO);
}

IGESData_DirChecker IGESDefs_MacroDef::DirChecker () const
{ 
  IGESData_DirChecker DC (306, 0);
  DC.Structure(IGESData_DefVoid);
  DC.LineFont(IGESData_DefVoid);
  DC.LineWeight(IGESData_DefVoid);
  DC.Color(IGESData_DefVoid);
  DC.BlankStatusIgnored();
  DC.SubordinateStatusRequired(0);
  DC.UseFlagRequired(2);
  DC.HierarchyStatusIgnored();
  return DC;
}

void IGESDefs_MacroDef::OwnDump (const IGESData_IGESDumper &, const Handle(Message_Messenger) &S, const Standard_Integer level) const
{ 
  S << "IGESDefs_MacroDef" << endl;
  S << "MACRO : ";
  IGESData_DumpString(S,myMACRO);
  S << endl;
  S << "Entity Type ID : " << myEntityTypeID << endl;
  S << "Language Statement : ";
  IGESData_DumpStrings(S,level,1,NbStatements(),LanguageStatement);
  S << "END MACRO : ";
  IGESData_DumpString(S,myENDMACRO);
  S << endl;
}
