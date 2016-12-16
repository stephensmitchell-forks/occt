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
#include <IGESDraw_CircArraySubfigure.hxx>
#include <IGESFile_Reader.hxx>
#include <IGESData_IGESWriter.hxx>
#include <IGESData_DirChecker.hxx>
#include <Message_Messenger.hxx>
#include <IGESData_IGESDumper.hxx>
#include <IGESData_Dump.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESDraw_CircArraySubfigure,IGESData_IGESEntity)

gp_Pnt IGESDraw_CircArraySubfigure::CenterPoint () const
{
  gp_Pnt tempCenterPoint(myCenter);
  return tempCenterPoint;
}

gp_Pnt IGESDraw_CircArraySubfigure::TransformedCenterPoint () const
{
  gp_XYZ tempCenterPoint = myCenter;
  if (HasTransf()) Location().Transforms(tempCenterPoint);
  gp_Pnt tempRes(tempCenterPoint);

  return tempRes;
}

Standard_Integer IGESDraw_CircArraySubfigure::ListCount () const
{
  return ( myPositions.IsNull() ? 0 : myPositions->Length() );
}

Standard_Boolean IGESDraw_CircArraySubfigure::PositionNum (const Standard_Integer Index) const
{
  // Method : If myPositions array length is Zero return myDoDontFlag;
  //          else Search Index in to the Array. If 'Index' found in the
  //          array return myDoDontFlag else return !myDoDontFlag.

  if (myPositions.IsNull())
    return (myDoDontFlag != 0);

  const Standard_Integer up = myPositions->Upper();
  for (Standard_Integer i = 1; i <= up; i++)
    if (myPositions->Value(i) == Index)
      return (myDoDontFlag != 0);

  return (myDoDontFlag == 0);
}

Standard_Integer IGESDraw_CircArraySubfigure::ListPosition (const Standard_Integer Index) const
{
  return myPositions->Value(Index);
  // raise OutOfRange from Standard if Index is out-of-bound
  // Exception NoSuchObject will be raised if myPositions == Null Handle
}

void IGESDraw_CircArraySubfigure::OwnRead (IGESFile_Reader &theReader)
{
  theReader.ReadPointer(myBaseEntity,"Base Entity");
  theReader.ReadInteger(myNbLocations,"Number Of Instance Locations");
  theReader.ReadXYZ(myCenter,"Imaginary Circle Center Coordinate");
  theReader.ReadReal(myRadius,"Radius Of Imaginary Circle");
  theReader.ReadReal(myStartAngle,"Start Angle in Radians");
  theReader.ReadReal(myDeltaAngle,"Delta Angle in Radians");

  Standard_Integer tempListCount = 0;
  theReader.ReadInteger(tempListCount,"DO-DONT List Count");
  if (tempListCount > 0)
    myPositions = new TColStd_HArray1OfInteger(1, tempListCount);
  else if (tempListCount < 0)
    theReader.AddFail("DO-DONT List Count : Less than Zero");

  theReader.ReadInteger(myDoDontFlag,"DO-DONT Flag");

  // Read the HArray1 only if its Length was read without any Error
  if (tempListCount > 0) {
    for (Standard_Integer i = 1; i <= tempListCount; i++)
      theReader.ReadInteger(myPositions->ChangeValue(i),"Number Of Position To Process");
  }
}

void IGESDraw_CircArraySubfigure::OwnWrite (IGESData_IGESWriter &IW) const
{
  IW.Send(myBaseEntity);
  IW.Send(myNbLocations);
  IW.Send(myCenter.X());
  IW.Send(myCenter.Y());
  IW.Send(myCenter.Z());
  IW.Send(myRadius);
  IW.Send(myStartAngle);
  IW.Send(myDeltaAngle);
  const Standard_Integer up = ListCount();
  IW.Send(up);
  IW.SendBoolean(DoDontFlag());
  // Send the HArray1 only if it is not empty (i.e. Null)
  for (Standard_Integer i = 1; i <= up; i++)
    IW.Send(ListPosition(i));
}

IGESData_DirChecker IGESDraw_CircArraySubfigure::DirChecker () const
{
  IGESData_DirChecker DC(414, 0);
  DC.Structure(IGESData_DefVoid);
  DC.LineFont(IGESData_DefAny);
  DC.LineWeight(IGESData_DefValue);
  DC.Color(IGESData_DefAny);
  DC.GraphicsIgnored(1);
  return DC;
}

void IGESDraw_CircArraySubfigure::OwnDump (const IGESData_IGESDumper &dumper, const Handle(Message_Messenger) &S, const Standard_Integer level) const
{
  const Standard_Integer tempSubLevel = (level <= 4) ? 0 : 1;
  S << "IGESDraw_CircArraySubfigure" << endl;
  S << "Base Entity : ";
  dumper.Dump(myBaseEntity,S,tempSubLevel);
  S << endl;
  S << "Total Number Of Possible Instance Locations : " << myNbLocations << endl;
  S << "Imaginary Circle. Radius : " << myRadius << "  Center : ";
  IGESData_DumpXYZL(S,level,myCenter,Location());  S << endl;
  S << "Start Angle (in radians) : " << myStartAngle << "  ";
  S << "Delta Angle (in radians) : " << myDeltaAngle << endl;
  S << "Do-Dont Flag : ";
  S << (DoDontFlag()? "Dont" : "Do") << endl;
  S << "The Do-Dont List : ";
  IGESData_DumpVals(S,level,1,ListCount(),ListPosition);
  S << endl;
}
