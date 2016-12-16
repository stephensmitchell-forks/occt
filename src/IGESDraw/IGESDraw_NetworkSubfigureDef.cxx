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

#include <IGESDraw_ConnectPoint.hxx>
#include <IGESDraw_NetworkSubfigureDef.hxx>
#include <IGESGraph_TextDisplayTemplate.hxx>
#include <TCollection_HAsciiString.hxx>
#include <IGESFile_Reader.hxx>
#include <IGESData_IGESWriter.hxx>
#include <Interface_EntityIterator.hxx>
#include <IGESData_DirChecker.hxx>
#include <Message_Messenger.hxx>
#include <IGESData_IGESDumper.hxx>
#include <IGESData_Dump.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESDraw_NetworkSubfigureDef,IGESData_IGESEntity)

Standard_Integer IGESDraw_NetworkSubfigureDef::NbEntities () const
{
  return theEntities->Length();
}

const Handle(IGESData_IGESEntity) & IGESDraw_NetworkSubfigureDef::Entity (const Standard_Integer Index) const
{
  return theEntities->Value(Index);
  // if Index is out of bound HArray1 will raise OutOfRange exception
}

Standard_Integer IGESDraw_NetworkSubfigureDef::NbPointEntities () const
{
  return (thePointEntities.IsNull() ? 0 : thePointEntities->Length());
}

Standard_Boolean IGESDraw_NetworkSubfigureDef::HasPointEntity (const Standard_Integer Index) const
{
  if (thePointEntities.IsNull()) return Standard_False;
  return (! thePointEntities->Value(Index).IsNull());
  // if Index is out of bound HArray1 will raise OutOfRange exception
}

const Handle(IGESDraw_ConnectPoint) & IGESDraw_NetworkSubfigureDef::PointEntity (const Standard_Integer Index) const
{
  return thePointEntities->Value(Index);
  // if Index is out of bound HArray1 will raise OutOfRange exception
}

void IGESDraw_NetworkSubfigureDef::OwnRead (IGESFile_Reader &PR)
{
  PR.ReadInteger(theDepth,"Depth Of Subfigure");
  PR.ReadText(theName,"Subfigure Name");

  Standard_Integer tempNbEntities1 = 0;
  PR.ReadInteger(tempNbEntities1,"Number Of Child Entities");
  if (tempNbEntities1 > 0)
  {
    theEntities = new IGESData_HArray1OfIGESEntity (1, tempNbEntities1);
    for (Standard_Integer i = 1; i <= tempNbEntities1; i++)
      PR.ReadPointer(theEntities->ChangeValue(i),"Child Entity");
  }
  else if (tempNbEntities1 < 0)
    PR.AddFail("Number Of Child Entities : Not Positive");

  PR.ReadInteger(theTypeFlag,"Type Flag");

  if (PR.ReadText(theDesignator,"Primary Reference Designator") == IGESFile_Reader::ParamEmpty)
    PR.AddWarning("Primary Reference Designator : Null");

  PR.ReadPointer(theDesignatorTemplate,"Primary Reference Designator",Standard_True);

  Standard_Integer tempNbEntities2 = 0;
  PR.ReadInteger(tempNbEntities2,"Number Of Connect Points");
  if (tempNbEntities2 > 0)
  {
    thePointEntities = new IGESDraw_HArray1OfConnectPoint (1, tempNbEntities2);
    for (Standard_Integer i = 1; i <= tempNbEntities2; i++)
      PR.ReadPointer(thePointEntities->ChangeValue(i),"Associated Connect Point Entity",Standard_True);
  }
  else if (tempNbEntities2 < 0)
    PR.AddFail("Number Of Connect Points : Less Than Zero");
}

void IGESDraw_NetworkSubfigureDef::OwnWrite (IGESData_IGESWriter &IW) const
{
  IW.Send(theDepth);
  IW.Send(theName);
  Standard_Integer I, up = NbEntities();
  IW.Send(up);
  for (I = 1; I <= up; I++)
    IW.Send(Entity(I));
  IW.Send(theTypeFlag);
  IW.Send(theDesignator);
  IW.Send(theDesignatorTemplate);
  up = NbPointEntities();
  IW.Send(up);
  for (I = 1; I <= up; I++)
    IW.Send(PointEntity(I));
}

void IGESDraw_NetworkSubfigureDef::OwnShared (Interface_EntityIterator &iter) const
{
  Standard_Integer I, up = NbEntities();
  for (I = 1; I <= up; I++)
    iter.GetOneItem(Entity(I));
  up = NbPointEntities();
  for (I = 1; I <= up; I++)
    iter.GetOneItem(PointEntity(I));
}

IGESData_DirChecker IGESDraw_NetworkSubfigureDef::DirChecker () const
{
  IGESData_DirChecker DC(320, 0);
  DC.Structure(IGESData_DefVoid);
  DC.LineFont(IGESData_DefAny);
  DC.LineWeight(IGESData_DefValue);
  DC.BlankStatusIgnored();
  DC.UseFlagRequired(2);
  DC.GraphicsIgnored(1);
  return DC;
}

void IGESDraw_NetworkSubfigureDef::OwnCheck (const Interface_ShareTool &, Handle(Interface_Check) &ach) const
{
  if ((theTypeFlag < 0) || (theTypeFlag > 2))
    ach->AddFail("TypeFlag has Invalid value");
  if (theDesignator.IsNull())
    ach->AddFail("Primary Reference Designator : not defined");
}

void IGESDraw_NetworkSubfigureDef::OwnDump (const IGESData_IGESDumper &dumper, const Handle(Message_Messenger) &S, const Standard_Integer level) const
{
  const Standard_Integer tempSubLevel = (level <= 4) ? 0 : 1;
  S << "IGESDraw_NetworkSubfigureDef" << endl;
  S << "Depth Of Subfigure(Nesting)  : " << theDepth << endl;
  S << "Name Of Subfigure            : ";
  IGESData_DumpString(S,theName);
  S << endl << "Associated Entities          : ";
  IGESData_DumpEntities(S,dumper,level,1,NbEntities(),Entity);
  S << endl << "Type Flag : " << theTypeFlag << endl;
  S << "Primary Reference Designator : ";
  IGESData_DumpString(S,theDesignator);
  S << endl << "Text Display Template Entity : ";
  dumper.Dump(theDesignatorTemplate,S,tempSubLevel);
  S << endl << "Connect Point Entities       : ";
  IGESData_DumpEntities(S,dumper,level,1,NbPointEntities(),PointEntity);
  S << endl;
}
