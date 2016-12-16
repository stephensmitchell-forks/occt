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

#include <IGESBasic_SubfigureDef.hxx>
#include <IGESGraph_LineFontDefTemplate.hxx>
#include <IGESFile_Reader.hxx>
#include <IGESData_IGESWriter.hxx>
#include <Interface_EntityIterator.hxx>
#include <IGESData_DirChecker.hxx>
#include <Message_Messenger.hxx>
#include <IGESData_IGESDumper.hxx>
#include <IGESData_Dump.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESGraph_LineFontDefTemplate,IGESData_LineFontEntity)

void IGESGraph_LineFontDefTemplate::OwnRead (IGESFile_Reader &PR)
{
  PR.ReadInteger(myOrientation,"Template Orientation");
  PR.ReadPointer(myTemplateEntity,"Subfigure Definition Entity for Template Display");
  PR.ReadReal(myDistance,"Distance between successive Template");
  PR.ReadReal(myScale,"Scale Factor For Subfigure");
}

void IGESGraph_LineFontDefTemplate::OwnWrite (IGESData_IGESWriter &IW) const
{
  IW.Send(myOrientation);
  IW.Send(myTemplateEntity);
  IW.Send(myDistance);
  IW.Send(myScale);
}

void IGESGraph_LineFontDefTemplate::OwnShared (Interface_EntityIterator &theIter) const
{
  theIter.GetOneItem(myTemplateEntity);
}

IGESData_DirChecker IGESGraph_LineFontDefTemplate::DirChecker () const
{
  IGESData_DirChecker DC(304, 1);
  DC.Structure(IGESData_DefVoid);
  DC.LineFont(IGESData_DefValue);
  DC.LineWeight(IGESData_DefVoid);
  DC.Color(IGESData_DefVoid);
  DC.BlankStatusIgnored();
  DC.SubordinateStatusRequired(0);
  DC.UseFlagRequired(2);
  DC.HierarchyStatusIgnored();
  return DC;
}

void IGESGraph_LineFontDefTemplate::OwnCheck (const Interface_ShareTool &, Handle(Interface_Check) &ach) const
{
  if (RankLineFont() == 0)
    ach->AddWarning("Line Font Rank is zero");
  else if ((RankLineFont() < 1) || (RankLineFont() > 5))
    ach->AddWarning("Invalid Value As Line Font Rank");
}

void IGESGraph_LineFontDefTemplate::OwnDump (const IGESData_IGESDumper &dumper, const Handle(Message_Messenger) &S, const Standard_Integer level) const
{
  Standard_Integer tempSubLevel = (level <= 4) ? 0 : 1;
  S << "IGESGraph_LineFontDefTemplate" << endl;
  S << "Orientation : " << myOrientation << endl;
  S << "Subfigure Display Entity For Template Display : ";
  dumper.Dump(myTemplateEntity,S,tempSubLevel);
  S << endl;
  S << "Length Between Successive Template Figure : " << myDistance << endl;
  S << "Scale Factor for Subfigure : " << myScale << endl;
  S << endl;
}
