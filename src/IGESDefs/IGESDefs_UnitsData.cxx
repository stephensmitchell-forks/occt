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

#include <IGESDefs_UnitsData.hxx>
#include <Standard_DimensionMismatch.hxx>
#include <Standard_OutOfRange.hxx>
#include <TCollection_HAsciiString.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <IGESFile_Reader.hxx>
#include <IGESData_IGESWriter.hxx>
#include <IGESData_DirChecker.hxx>
#include <Message_Messenger.hxx>
#include <IGESData_IGESDumper.hxx>
#include <IGESData_Dump.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESDefs_UnitsData,IGESData_IGESEntity)

Standard_Integer IGESDefs_UnitsData::NbUnits () const
{
  return theUnitTypes->Length();
}

const Handle(TCollection_HAsciiString) & IGESDefs_UnitsData::UnitType (const Standard_Integer UnitNum) const
{
  return theUnitTypes->Value(UnitNum);
}

const Handle(TCollection_HAsciiString) & IGESDefs_UnitsData::UnitValue (const Standard_Integer UnitNum) const
{
  return theUnitValues->Value(UnitNum);
}

Standard_Real IGESDefs_UnitsData::ScaleFactor (const Standard_Integer UnitNum) const
{
  return theUnitScales->Value(UnitNum);
}

void IGESDefs_UnitsData::OwnRead (IGESFile_Reader &PR)
{ 
  Standard_Integer nbval = 0;
  PR.ReadInteger(nbval,"Number of Units");
  if (nbval > 0)
  {
    theUnitTypes = new Interface_HArray1OfHAsciiString(1, nbval);
    theUnitValues = new Interface_HArray1OfHAsciiString(1, nbval);
    theUnitScales = new TColStd_HArray1OfReal(1, nbval);

    for (Standard_Integer i = 1; i <= nbval; i++)
    {
      PR.ReadText(theUnitTypes->ChangeValue(i),"Type of Unit");
      PR.ReadText(theUnitValues->ChangeValue(i),"Value of Unit");
      PR.ReadReal(theUnitScales->ChangeValue(i),"Scale of Unit");
    }
  }
  else PR.AddFail("Number of Units: Less than or Equal or zero");
}

void IGESDefs_UnitsData::OwnWrite (IGESData_IGESWriter& IW) const
{ 
  Standard_Integer upper = NbUnits();
  IW.Send(upper);

  for (Standard_Integer i = 1; i <= upper; i++)
  {
    IW.Send(UnitType(i));
    IW.Send(UnitValue(i));
    IW.Send(ScaleFactor(i));
  }
}

IGESData_DirChecker IGESDefs_UnitsData::DirChecker () const
{ 
  IGESData_DirChecker DC (316, 0);
  DC.Structure(IGESData_DefVoid);
  DC.LineFont(IGESData_DefVoid);
  DC.LineWeight(IGESData_DefVoid);
  DC.Color(IGESData_DefVoid);
  DC.BlankStatusIgnored();
  DC.SubordinateStatusRequired(0);
  DC.UseFlagRequired(2);
  DC.HierarchyStatusIgnored();
  return DC;
}

void IGESDefs_UnitsData::OwnDump (const IGESData_IGESDumper &, const Handle(Message_Messenger) &S, const Standard_Integer level) const
{ 
  S << "IGESDefs_UnitsData" << endl;
  S << "Number of Units : " << NbUnits() << endl;
  S << "Type of Unit : " << endl;
  S << "Value of Unit : " << endl;
  S << "Scale Factor : " << endl;
  IGESData_DumpStrings(S,-level,1,NbUnits(),UnitType);
  S << endl;
  if (level > 4)
  {
    S << "Details of the Units" << endl;
    Standard_Integer upper = NbUnits();
    for (Standard_Integer i = 1; i <= upper; i++)
	{
      S << "[" << i << "] Type  : ";
      IGESData_DumpString(S,UnitType(i));
      S << endl;
      S << "     Value : ";
      IGESData_DumpString(S,UnitValue(i));
      S << endl;
      S << "     ScaleFactor: " << ScaleFactor(i) << endl;
	}
  }
  S << endl;
}
