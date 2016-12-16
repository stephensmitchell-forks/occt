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

#include <IGESGraph_Color.hxx>
#include <TCollection_HAsciiString.hxx>
#include <IGESFile_Reader.hxx>
#include <IGESData_IGESWriter.hxx>
#include <IGESData_DirChecker.hxx>
#include <Message_Messenger.hxx>
#include <IGESData_IGESDumper.hxx>
#include <IGESData_Dump.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESGraph_Color,IGESData_ColorEntity)

void IGESGraph_Color::HLSPercentage (Standard_Real& Hue, Standard_Real& Lightness, Standard_Real& Saturation) const
{
  Hue        = ((1.0 / (2.0 * M_PI)) * (ATan(((2 * myRed) - myGreen - myBlue) / (Sqrt(3) * (myGreen - myBlue)))));
  Lightness  = ((1.0 / 3.0) * (myRed + myGreen + myBlue));
  Saturation = (Sqrt((myRed * myRed  ) + (myGreen * myGreen) + (myBlue * myBlue ) - (myRed * myGreen) - (myRed * myBlue ) - (myBlue * myGreen)));
}

void IGESGraph_Color::OwnRead (IGESFile_Reader &PR)
{
  PR.ReadReal(myRed,"RED as % Of Full Intensity");
  PR.ReadReal(myGreen,"GREEN as % Of Full Intensity");
  PR.ReadReal(myBlue,"BLUE as % Of Full Intensity");
  PR.ReadText(myColorName,"Color Name");
}

void IGESGraph_Color::OwnWrite (IGESData_IGESWriter& IW)  const
{
  IW.Send(myRed);
  IW.Send(myGreen);
  IW.Send(myBlue);
//  ATTENTION  place a reserver (Null) silya des pointeurs additionnels
  if (!myColorName.IsNull())
    IW.Send(myColorName);
  else IW.SendVoid();    // placekeeper to be reserved for additional pointers
}

IGESData_DirChecker IGESGraph_Color::DirChecker ()  const
{
  IGESData_DirChecker DC(314, 0);
  DC.Structure(IGESData_DefVoid);
  DC.LineFont(IGESData_DefVoid);
  DC.LineWeight(IGESData_DefVoid);
  DC.Color(IGESData_DefAny);
  DC.BlankStatusIgnored();
  DC.SubordinateStatusRequired(0);
  DC.UseFlagRequired(2);
  DC.HierarchyStatusIgnored();
  return DC;
}

void IGESGraph_Color::OwnDump (const IGESData_IGESDumper &, const Handle(Message_Messenger) &S, const Standard_Integer) const
{
  S << "IGESGraph_Color" << endl;
  S << "Red   (in % Of Full Intensity) : " << myRed   << endl;
  S << "Green (in % Of Full Intensity) : " << myGreen << endl;
  S << "Blue  (in % Of Full Intensity) : " << myBlue  << endl;
  S << "Color Name : ";
  IGESData_DumpString(S,myColorName);
  S << endl;
}
