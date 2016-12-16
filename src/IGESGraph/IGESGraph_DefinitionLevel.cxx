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

#include <IGESGraph_DefinitionLevel.hxx>
#include <Standard_DimensionMismatch.hxx>
#include <Standard_OutOfRange.hxx>
#include <IGESFile_Reader.hxx>
#include <IGESData_IGESWriter.hxx>
#include <IGESData_DirChecker.hxx>
#include <Message_Messenger.hxx>
#include <IGESData_IGESDumper.hxx>
#include <IGESData_Dump.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESGraph_DefinitionLevel,IGESData_LevelListEntity)

Standard_Integer IGESGraph_DefinitionLevel::NbPropertyValues () const
{
  return myLevelNumbers->Length();
}

Standard_Integer IGESGraph_DefinitionLevel::NbLevelNumbers () const
{
  return myLevelNumbers->Length();
}

Standard_Integer IGESGraph_DefinitionLevel::LevelNumber (const Standard_Integer LevelIndex) const
{
  return myLevelNumbers->Value(LevelIndex);
}

void IGESGraph_DefinitionLevel::OwnRead (IGESFile_Reader &PR)
{ 
  Standard_Integer nbval = 0;
  PR.ReadInteger(nbval,"No. of Property Values");
  if (nbval > 0)
  {
    myLevelNumbers = new TColStd_HArray1OfInteger(1, nbval);
    PR.ReadInteger(myLevelNumbers->ChangeFirst(),nbval,"array levelNumbers");
  }
  else PR.AddFail("No. of Property Values : Not Positive");
}

void IGESGraph_DefinitionLevel::OwnWrite (IGESData_IGESWriter& IW) const
{ 
  const Standard_Integer Up = NbPropertyValues();
  IW.Send( Up );
  for ( Standard_Integer i = 1; i <= Up; i++)
    IW.Send(LevelNumber(i) );
}

IGESData_DirChecker IGESGraph_DefinitionLevel::DirChecker () const
{ 
  IGESData_DirChecker DC (406, 1);
  DC.Structure(IGESData_DefVoid);
  DC.LineFont(IGESData_DefVoid);
  DC.LineWeight(IGESData_DefVoid);
  DC.Color(IGESData_DefVoid);
  DC.BlankStatusIgnored();
  DC.UseFlagIgnored();
  DC.HierarchyStatusIgnored();
  return DC;
}

void IGESGraph_DefinitionLevel::OwnDump (const IGESData_IGESDumper &, const Handle(Message_Messenger) &S, const Standard_Integer level)  const
{
  S << "IGESGraph_DefinitionLevel" << endl;
  S << "Level Numbers : ";
  IGESData_DumpVals(S,level,1,NbPropertyValues(),LevelNumber);
  S << endl;
}
