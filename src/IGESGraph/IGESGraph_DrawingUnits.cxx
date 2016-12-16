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

#include <IGESGraph_DrawingUnits.hxx>
#include <TCollection_HAsciiString.hxx>
#include <IGESFile_Reader.hxx>
#include <IGESData_IGESWriter.hxx>
#include <IGESData_DirChecker.hxx>
#include <Message_Messenger.hxx>
#include <IGESData_IGESDumper.hxx>
#include <IGESData_Dump.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESGraph_DrawingUnits,IGESData_IGESEntity)

Standard_Real IGESGraph_DrawingUnits::UnitValue () const
{
  switch (myFlag) {
    case  1 : return 0.0254;
    case  2 : return 0.001;
    case  3 : return 1.;
    case  4 : return 0.3048;
    case  5 : return 1609.27;
    case  6 : return 1.;
    case  7 : return 1000.;
    case  8 : return 0.0000254;
    case  9 : return 0.000001;
    case 10 : return 0.01;
    case 11 : return 0.0000000254;
    default : break;
  }
  return 1.;
}

void IGESGraph_DrawingUnits::OwnRead (IGESFile_Reader &PR)
{ 
  Standard_Integer aNbPropertyValues = 0;
  PR.ReadInteger(aNbPropertyValues,"No. of property values");
  if (aNbPropertyValues != 2)
    PR.AddFail("No. of Property values : Value is not 2");

  PR.ReadInteger(myFlag,"Units Flag");
  PR.ReadText(myUnit,"Units Name");
}

void IGESGraph_DrawingUnits::OwnWrite (IGESData_IGESWriter& IW) const
{ 
  IW.Send(2);
  IW.Send(myFlag);
  IW.Send(myUnit);
}

IGESData_DirChecker IGESGraph_DrawingUnits::DirChecker () const
{ 
  IGESData_DirChecker DC (406, 17);
  DC.Structure(IGESData_DefVoid);
  DC.LineFont(IGESData_DefVoid);
  DC.LineWeight(IGESData_DefVoid);
  DC.Color(IGESData_DefVoid);
  DC.BlankStatusIgnored();
  DC.UseFlagIgnored();
  DC.HierarchyStatusIgnored();
  return DC;
}

void IGESGraph_DrawingUnits::OwnCheck (const Interface_ShareTool &, Handle(Interface_Check) &ach) const
{
  // Check Flag//Unit Name
  if (myUnit.IsNull()) {
    if (myFlag == 3)
      ach->AddFail("Unit Flag = 3 (user def.) and Unit Name undefined");
  }
  else {
    Standard_CString unm  = myUnit->ToCString();
    Standard_Boolean unok = Standard_True;
    switch (myFlag) {
      case  1 : unok = (!strcmp(unm,"IN") || !strcmp(unm,"INCH"));  break;
      case  2 : unok = !strcmp(unm,"MM");   break;
      case  3 : unok = Standard_True;       break;  // nom libre
      case  4 : unok = !strcmp(unm,"FT");   break;
      case  5 : unok = !strcmp(unm,"MI");   break;
      case  6 : unok = !strcmp(unm,"M");    break;
      case  7 : unok = !strcmp(unm,"KM");   break;
      case  8 : unok = !strcmp(unm,"MIL");  break;
      case  9 : unok = !strcmp(unm,"UM");   break;
      case 10 : unok = !strcmp(unm,"CM");   break;
      case 11 : unok = !strcmp(unm,"UIN");  break;
      default : ach->AddFail("Unit Flag not in range 1 - 11"); break;
    }
    if (!unok) ach->AddFail("Unit Flag & Name not accorded");
  }
}

Standard_Boolean IGESGraph_DrawingUnits::OwnCorrect ()
{
  // ya aussi les noms : Flag a priorite sur Unit
  Handle(TCollection_HAsciiString) name;
  Standard_CString unm = (myUnit.IsNull()? "" : myUnit->ToCString());
  switch (myFlag) {
    case  1 : if (strcmp(unm,"IN") && strcmp(unm,"INCH"))
      name = new TCollection_HAsciiString ("IN");  break;
    case  2 : if (strcmp(unm,"MM"))
      name = new TCollection_HAsciiString ("MM");  break;
    case  3 : break;  // nom libre
    case  4 : if (strcmp(unm,"FT"))
      name = new TCollection_HAsciiString ("FT");  break;
    case  5 : if (strcmp(unm,"MI"))
      name = new TCollection_HAsciiString ("MI");  break;
    case  6 : if (strcmp(unm,"M"))
      name = new TCollection_HAsciiString ("M");   break;
    case  7 : if (strcmp(unm,"KM"))
      name = new TCollection_HAsciiString ("KM");  break;
    case  8 : if (strcmp(unm,"MIL"))
      name = new TCollection_HAsciiString ("MIL"); break;
    case  9 : if (strcmp(unm,"UM"))
      name = new TCollection_HAsciiString ("UM");  break;
    case 10 : if (strcmp(unm,"CM"))
      name = new TCollection_HAsciiString ("CM");  break;
    case 11 : if (strcmp(unm,"UIN"))
      name = new TCollection_HAsciiString ("UIN"); break;
    default : break;    // on ne peut rien faire ... ?
  }
  if (!name.IsNull()) {
    myUnit = name;
    return Standard_True;
  }
  return Standard_False;
}

void IGESGraph_DrawingUnits::OwnDump (const IGESData_IGESDumper &, const Handle(Message_Messenger) &S, const Standard_Integer) const
{
  S << "IGESGraph_DrawingUnits" << endl;
  S << "No. of property values : 2" << endl;
  S << "  Units Flag : " << myFlag;
  S << "  Units Name : ";
  IGESData_DumpString(S,myUnit);
  S << "  computed Value (in meters) : " << UnitValue();
  S << endl;
}
