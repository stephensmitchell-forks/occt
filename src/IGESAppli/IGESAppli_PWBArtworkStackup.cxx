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

#include <IGESAppli_PWBArtworkStackup.hxx>
#include <Standard_DimensionMismatch.hxx>
#include <Standard_OutOfRange.hxx>
#include <TCollection_HAsciiString.hxx>
#include <IGESFile_Reader.hxx>
#include <IGESData_IGESWriter.hxx>
#include <Message_Messenger.hxx>
#include <IGESData_DirChecker.hxx>
#include <IGESData_IGESDumper.hxx>
#include <IGESData_Dump.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESAppli_PWBArtworkStackup,IGESData_IGESEntity)

Standard_Integer IGESAppli_PWBArtworkStackup::NbLevelNumbers () const
{
  return myLevelNumbers->Length();
}

Standard_Integer IGESAppli_PWBArtworkStackup::LevelNumber (const Standard_Integer Index) const
{
  return myLevelNumbers->Value(Index);
}

void IGESAppli_PWBArtworkStackup::OwnRead (IGESFile_Reader &theReader)
{
  theReader.ReadInteger(myNbPropertyValues,"Number of property values");
  theReader.ReadText(myArtworkStackupIdent,"Artwork Stackup Identification");

  Standard_Integer num = 0;
  theReader.ReadInteger(num,"Number of level numbers");
  if (num > 0)
  {
    myLevelNumbers = new TColStd_HArray1OfInteger(1, num);
    theReader.ReadInteger(myLevelNumbers->ChangeFirst(),num,"Level Numbers");
  }
  else theReader.AddFail("Number of level numbers: Not Positive");
}

void IGESAppli_PWBArtworkStackup::OwnWrite (IGESData_IGESWriter &IW) const
{
  IW.Send(myNbPropertyValues);
  IW.Send(myArtworkStackupIdent);
  const Standard_Integer num = myLevelNumbers->Length();
  IW.Send(num);
  for ( Standard_Integer i = 1; i <= num; i++ )
    IW.Send(myLevelNumbers->Value(i));
}

IGESData_DirChecker IGESAppli_PWBArtworkStackup::DirChecker () const
{
  IGESData_DirChecker DC(406, 25);
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

void IGESAppli_PWBArtworkStackup::OwnDump (const IGESData_IGESDumper &, const Handle(Message_Messenger) &S, const Standard_Integer level) const
{
  S << "IGESAppli_PWBArtworkStackup" << endl;
  S << "Number of property values : " << myNbPropertyValues << endl;
  S << "Artwork Stackup Identification : ";
  IGESData_DumpString(S,myArtworkStackupIdent);
  S << endl;
  S << "Level Numbers : ";
  IGESData_DumpVals(S,level,1,NbLevelNumbers(),LevelNumber);
  S << endl;
}
