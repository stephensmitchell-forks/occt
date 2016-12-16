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

#include <IGESAppli_LevelToPWBLayerMap.hxx>
#include <Standard_DimensionMismatch.hxx>
#include <Standard_OutOfRange.hxx>
#include <TCollection_HAsciiString.hxx>
#include <IGESFile_Reader.hxx>
#include <IGESData_IGESWriter.hxx>
#include <Message_Messenger.hxx>
#include <IGESData_DirChecker.hxx>
#include <IGESData_IGESDumper.hxx>
#include <IGESData_Dump.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESAppli_LevelToPWBLayerMap,IGESData_IGESEntity)

Standard_Integer IGESAppli_LevelToPWBLayerMap::NbLevelToLayerDefs () const
{
  return myExchangeFileLevelNumber->Length();
}

Standard_Integer IGESAppli_LevelToPWBLayerMap::ExchangeFileLevelNumber (const Standard_Integer Index) const
{
  return myExchangeFileLevelNumber->Value(Index);
}

const Handle(TCollection_HAsciiString) & IGESAppli_LevelToPWBLayerMap::NativeLevel (const Standard_Integer Index) const
{
  return myNativeLevel->Value(Index);
}

Standard_Integer IGESAppli_LevelToPWBLayerMap::PhysicalLayerNumber (const Standard_Integer Index) const
{
  return myPhysicalLayerNumber->Value(Index);
}

const Handle(TCollection_HAsciiString) & IGESAppli_LevelToPWBLayerMap::ExchangeFileLevelIdent (const Standard_Integer Index) const
{
  return myExchangeFileLevelIdent->Value(Index);
}

void IGESAppli_LevelToPWBLayerMap::OwnRead (IGESFile_Reader &theReader)
{
  theReader.ReadInteger(myNbPropertyValues,"Number of property values");
  Standard_Integer num = 0;
  theReader.ReadInteger(num,"Number of definitions");
  if (num > 0)
  {
    myExchangeFileLevelNumber = new TColStd_HArray1OfInteger(1, num);
    myNativeLevel = new Interface_HArray1OfHAsciiString(1, num);
    myPhysicalLayerNumber = new TColStd_HArray1OfInteger(1, num);
    myExchangeFileLevelIdent = new Interface_HArray1OfHAsciiString(1, num);

    for ( Standard_Integer i = 1; i <= num; i++ )
    {
      theReader.ReadInteger(myExchangeFileLevelNumber->ChangeValue(i),"Exchange File Level Number");
      theReader.ReadText(myNativeLevel->ChangeValue(i),"Native Level Identification");
      theReader.ReadInteger(myPhysicalLayerNumber->ChangeValue(i),"Physical Layer Number");
      theReader.ReadText(myExchangeFileLevelIdent->ChangeValue(i),"Exchange File Level Identification");
    }
  }
  else theReader.AddFail("Number of definitions: Not Positive");
}

void IGESAppli_LevelToPWBLayerMap::OwnWrite (IGESData_IGESWriter &IW) const
{
  IW.Send(myNbPropertyValues);
  const Standard_Integer num = myExchangeFileLevelNumber->Length();
  IW.Send(num);
  for ( Standard_Integer i = 1; i <= num; i++ )
  {
    IW.Send(myExchangeFileLevelNumber->Value(i));
    IW.Send(myNativeLevel->Value(i));
    IW.Send(myPhysicalLayerNumber->Value(i));
    IW.Send(myExchangeFileLevelIdent->Value(i));
  }
}

IGESData_DirChecker IGESAppli_LevelToPWBLayerMap::DirChecker () const
{
  IGESData_DirChecker DC(406, 24);
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

void IGESAppli_LevelToPWBLayerMap::OwnDump (const IGESData_IGESDumper &, const Handle(Message_Messenger) &S, const Standard_Integer level) const
{
  S << "IGESAppli_LevelToPWBLayerMap" << endl;
  S << "Number of property values : " << myNbPropertyValues << endl;
  S << "Exchange File Level Number : " << endl;
  S << "Native Level Identification : " << endl;
  S << "Physical Layer Number : " << endl;
  S << "Exchange File Level Identification : ";
  const Standard_Integer num = myExchangeFileLevelNumber->Length();
  IGESData_DumpStrings(S,-level,1,num,ExchangeFileLevelIdent);
  S << endl;
  if (level > 4)
    for ( Standard_Integer i = 1; i <= num; i++ )
    {
      S << "[" << i << "]: " << endl;
      S << "Exchange File Level Number : "
        << myExchangeFileLevelNumber->Value(i) << endl;
      S << "Native Level Identification : ";
      IGESData_DumpString(S,myNativeLevel->Value(i));
      S << endl;
      S << "Physical Layer Number : " << myPhysicalLayerNumber->Value(i) << endl;
      S << "Exchange File Level Identification : ";
      IGESData_DumpString(S,myExchangeFileLevelIdent->Value(i));
      S << endl;
    }
}
