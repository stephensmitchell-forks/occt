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

#include <IGESGraph_LineFontDefPattern.hxx>
#include <TCollection_HAsciiString.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <IGESFile_Reader.hxx>
#include <IGESData_IGESWriter.hxx>
#include <IGESData_DirChecker.hxx>
#include <Message_Messenger.hxx>
#include <IGESData_IGESDumper.hxx>
#include <IGESData_Dump.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESGraph_LineFontDefPattern,IGESData_LineFontEntity)

Standard_Integer IGESGraph_LineFontDefPattern::NbSegments () const
{
  return mySegmentLengths->Length();
}

Standard_Real IGESGraph_LineFontDefPattern::Length (const Standard_Integer Index) const
{
  return mySegmentLengths->Value(Index);
}

Standard_Boolean IGESGraph_LineFontDefPattern::IsVisible (const Standard_Integer Index) const
{
  const Standard_Integer nbSegs = mySegmentLengths->Length();
  if (Index <= 0 || Index > nbSegs)
    return Standard_False;

  // Get the Character out of String, which contains the required BIT
  char tempStr[2];
  const Standard_Integer length = myDisplayPattern->Length();
  tempStr[0] = myDisplayPattern->Value(length - ((nbSegs - Index) / 4));
  tempStr[1] = 0;
  const Standard_Integer tempVal = (Standard_Integer) strtol(tempStr, (char **)NULL, 16);
  // Now get the BIT out of tempVal
  Standard_Integer mask = 0x01;
  mask <<= ((nbSegs - Index) % 4);
  return ((tempVal & mask) != 0);
}

void IGESGraph_LineFontDefPattern::OwnRead (IGESFile_Reader &PR)
{
  Standard_Integer aNbSeg = 0;
  PR.ReadInteger(aNbSeg,"Number of Visible-Blank Segments");
  if (aNbSeg > 0)
  {
    mySegmentLengths = new TColStd_HArray1OfReal(1, aNbSeg);
    for (Standard_Integer i = 1; i <= aNbSeg; i++)
      PR.ReadReal(mySegmentLengths->ChangeValue(i),"Length of Segment");
  }
  else PR.AddFail("Number of Visible-Blank Segments : Not Positive");

  PR.ReadText(myDisplayPattern,"Visible-Blank Display Pattern");
}

void IGESGraph_LineFontDefPattern::OwnWrite (IGESData_IGESWriter& IW) const
{
  const Standard_Integer nb = mySegmentLengths->Length();
  IW.Send(nb);
  for (Standard_Integer i = 1; i <= nb; i++)
    IW.Send(mySegmentLengths->Value(i));
  IW.Send(myDisplayPattern);
}

IGESData_DirChecker IGESGraph_LineFontDefPattern::DirChecker () const
{
  IGESData_DirChecker DC(304, 2);
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

void IGESGraph_LineFontDefPattern::OwnCheck (const Interface_ShareTool &, Handle(Interface_Check) &ach) const
{
  if (RankLineFont() == 0)
    ach->AddWarning("Line Font Rank is zero");
  else if (RankLineFont() < 1 || RankLineFont() > 5)
    ach->AddWarning("Invalid Value As Line Font Rank(Valid Range 1 to 5)");
}

void IGESGraph_LineFontDefPattern::OwnDump (const IGESData_IGESDumper &, const Handle(Message_Messenger) &S, const Standard_Integer level) const
{
  S << "IGESGraph_LineFontDefPattern" << endl;
  S << "Visible-Blank Segments : ";
  const Standard_Integer nb = mySegmentLengths->Length();
  IGESData_DumpVals(S,level,1,nb,Length);
  S <<endl << "Display Pattern : ";
  IGESData_DumpString(S,myDisplayPattern);
  S << endl;
  if (level > 4) {
    S << " -> Which Segments are Visible (the others are Blank) : " << endl;
    for (Standard_Integer i = 1; i <= nb; i++) {
      if (IsVisible(i)) S << "  " << i;
    }
    S << endl;
  }
}
