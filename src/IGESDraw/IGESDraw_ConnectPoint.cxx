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

#include <gp_GTrsf.hxx>
#include <gp_Pnt.hxx>
#include <IGESDraw_ConnectPoint.hxx>
#include <IGESGraph_TextDisplayTemplate.hxx>
#include <TCollection_HAsciiString.hxx>
#include <IGESFile_Reader.hxx>
#include <IGESData_IGESWriter.hxx>
#include <Interface_EntityIterator.hxx>
#include <IGESData_DirChecker.hxx>
#include <Message_Messenger.hxx>
#include <IGESData_IGESDumper.hxx>
#include <IGESData_Dump.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESDraw_ConnectPoint,IGESData_IGESEntity)

gp_Pnt IGESDraw_ConnectPoint::Point () const
{
  gp_Pnt tempPoint(thePoint);
  return tempPoint;
}

gp_Pnt IGESDraw_ConnectPoint::TransformedPoint () const
{
  gp_XYZ tempPoint = thePoint;
  if (HasTransf()) Location().Transforms(tempPoint);
  gp_Pnt tempRes(tempPoint);

  return (tempRes);
}

void IGESDraw_ConnectPoint::OwnRead (IGESFile_Reader &PR)
{
  Handle(IGESData_IGESEntity) tempOwnerSubfigure;

  PR.ReadXYZ(thePoint,"Connect Point Coordinate");
  PR.ReadPointer(theDisplaySymbol,"Display Symbol Geometry Entity",Standard_True);
  PR.ReadInteger(theTypeFlag,"Type Flag");
  PR.ReadInteger(theFunctionFlag,"Function Flag");
  PR.ReadText(theFunctionIdentifier,"Function Identifier");
  PR.ReadPointer(theIdentifierTemplate,"Text Display Identifier Template",Standard_True);
  PR.ReadText(theFunctionName,"Connect Point Function Name");
  PR.ReadPointer(theFunctionTemplate,"Text Display Function Template",Standard_True);
  PR.ReadInteger(thePointIdentifier,"Unique Connect Point Identifier");
  PR.ReadInteger(theFunctionCode,"Connect Point Function Code");

  theSwapFlag = 0;  // default
  PR.ReadInteger(theSwapFlag,"Swap Flag");

  PR.ReadPointer(theOwnerSubfigure,"Owner Network Subfigure Entity",Standard_True);
}

void IGESDraw_ConnectPoint::OwnWrite (IGESData_IGESWriter &IW) const
{
  IW.Send(thePoint.X());
  IW.Send(thePoint.Y());
  IW.Send(thePoint.Z());
  IW.Send(theDisplaySymbol);
  IW.Send(theTypeFlag);
  IW.Send(theFunctionFlag);
  IW.Send(theFunctionIdentifier);
  IW.Send(theIdentifierTemplate);
  IW.Send(theFunctionName);
  IW.Send(theFunctionTemplate);
  IW.Send(thePointIdentifier);
  IW.Send(theFunctionCode);
  IW.SendBoolean(SwapFlag());
  IW.Send(theOwnerSubfigure);
}

void IGESDraw_ConnectPoint::OwnShared (Interface_EntityIterator &theIter) const
{
  theIter.GetOneItem(theDisplaySymbol);
  theIter.GetOneItem(theIdentifierTemplate);
  theIter.GetOneItem(theFunctionTemplate);
  theIter.GetOneItem(theOwnerSubfigure);
}

IGESData_DirChecker IGESDraw_ConnectPoint::DirChecker () const
{
  IGESData_DirChecker DC(132, 0);
  DC.Structure(IGESData_DefVoid);
  DC.Color(IGESData_DefAny);
  DC.UseFlagRequired(4);
  if (!theDisplaySymbol.IsNull()) {
    DC.LineFont(IGESData_DefAny);
    DC.LineWeight(IGESData_DefAny);
  }
  else {
    // Note : If ent->DisplaySymbol() is NULL Handle; ignore Line Font, Weight
    //        and Hierarchy Status
    DC.LineFont(IGESData_DefVoid);
    DC.LineWeight(IGESData_DefVoid);
    DC.HierarchyStatusIgnored();
  }
  return DC;
}

void IGESDraw_ConnectPoint::OwnCheck (const Interface_ShareTool &, Handle(Interface_Check) &ach) const
{
  if ((theTypeFlag <   0) || (theTypeFlag > 9999)  ||
      ((theTypeFlag >   2) && (theTypeFlag <  101)) ||
      ((theTypeFlag > 104) && (theTypeFlag <  201)) ||
      ((theTypeFlag > 203) && (theTypeFlag < 5001)))
    ach->AddFail("TypeFlag has Invalid value");

  if ((theFunctionFlag < 0) || (theFunctionFlag > 2))
    ach->AddFail("FunctionFlag has Invalid value");

  if ((theFunctionCode <  0) || (theFunctionCode > 9999)  ||
      ((theFunctionCode > 49) && (theFunctionCode <   98)) ||
      ((theFunctionCode > 99) && (theFunctionCode < 5001)))
    ach->AddFail("FunctionCode has Invalid value");

  if ((theSwapFlag != 0) && (theSwapFlag != 1))
    ach->AddFail("SwapFlag has Invalid value");
}

void IGESDraw_ConnectPoint::OwnDump (const IGESData_IGESDumper &dumper, const Handle(Message_Messenger) &S, const Standard_Integer level) const
{
  const Standard_Integer tempSubLevel = (level <= 4) ? 0 : 1;
  S << "IGESDraw_ConnectPoint" << endl;
  S << "Connection Point Coordinate : ";
  IGESData_DumpXYZL(S,level,thePoint,Location());
  S << "Display Symbol Geometry Entity : ";
  dumper.Dump(theDisplaySymbol,S,tempSubLevel);
  S << endl;
  S << "Type Flag : "     << theTypeFlag << "  "
    << "Function Flag : " << theFunctionFlag << endl;
  S << "Function Identifier : ";
  IGESData_DumpString(S,theFunctionIdentifier);
  S << endl << "Text Display Template Entity for CID : ";
  dumper.Dump(theIdentifierTemplate,S,tempSubLevel);
  S << endl << "Function Name : ";
  IGESData_DumpString(S,theFunctionName);
  S << endl << "Text Display Template Entity for CFN : ";
  dumper.Dump(theFunctionTemplate,S,tempSubLevel);
  S << endl;
  S << "Point Identifier : " << thePointIdentifier << endl
    << "Function Code : "    << theFunctionCode
    << "Swap Flag : "        << ( SwapFlag() ? "True" : "False" ) << endl;
  S << "Owner Subfigure Entity : ";
  dumper.Dump(theOwnerSubfigure,S,tempSubLevel);
  S << endl;
}
