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

#include <IGESAppli_PartNumber.hxx>
#include <TCollection_HAsciiString.hxx>
#include <IGESFile_Reader.hxx>
#include <IGESData_IGESWriter.hxx>
#include <Message_Messenger.hxx>
#include <IGESData_DirChecker.hxx>
#include <IGESData_IGESDumper.hxx>
#include <IGESData_Dump.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESAppli_PartNumber,IGESData_IGESEntity)

void IGESAppli_PartNumber::OwnRead (IGESFile_Reader &theReader)
{
  Standard_Integer aNbPropertyValues = 4;
  if (theReader.ReadInteger(aNbPropertyValues,"Number of property values") == IGESFile_Reader::ParamError || aNbPropertyValues != 4)
    theReader.AddFail("Number of property values != 4");
  theReader.ReadText(myGenericNumber,"Generic Number or Name");
  theReader.ReadText(myMilitaryNumber,"Military Number or Name");
  theReader.ReadText(myVendorNumber,"Vendor Number or Name");
  theReader.ReadText(myInternalNumber,"Internal Number or Name");
}

void IGESAppli_PartNumber::OwnWrite (IGESData_IGESWriter &IW) const
{
  IW.Send(4);
  IW.Send(myGenericNumber);
  IW.Send(myMilitaryNumber);
  IW.Send(myVendorNumber);
  IW.Send(myInternalNumber);
}

IGESData_DirChecker IGESAppli_PartNumber::DirChecker () const
{
  IGESData_DirChecker DC(406, 9);
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

void IGESAppli_PartNumber::OwnDump (const IGESData_IGESDumper &, const Handle(Message_Messenger) &S, const Standard_Integer) const
{
  S << "IGESAppli_PartNumber" << endl;
  S << "Number of property values : 4" << endl;
  S << "Generic  Number or Name : ";
  IGESData_DumpString(S,myGenericNumber);
  S << endl;
  S << "Military Number or Name : ";
  IGESData_DumpString(S,myMilitaryNumber);
  S << endl;
  S << "Vendor   Number or Name : ";
  IGESData_DumpString(S,myVendorNumber);
  S << endl;
  S << "Internal Number or Name : ";
  IGESData_DumpString(S,myInternalNumber);
  S << endl;
}
