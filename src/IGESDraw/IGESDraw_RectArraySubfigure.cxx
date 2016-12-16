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
#include <gp_XYZ.hxx>
#include <IGESDraw_RectArraySubfigure.hxx>
#include <IGESFile_Reader.hxx>
#include <IGESData_IGESWriter.hxx>
#include <Interface_EntityIterator.hxx>
#include <IGESData_DirChecker.hxx>
#include <Message_Messenger.hxx>
#include <IGESData_IGESDumper.hxx>
#include <IGESData_Dump.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESDraw_RectArraySubfigure,IGESData_IGESEntity)

gp_Pnt IGESDraw_RectArraySubfigure::LowerLeftCorner () const
{
  gp_Pnt tempLowerLeftCorner(theLowerLeftCorner);
  return tempLowerLeftCorner;
}

gp_Pnt IGESDraw_RectArraySubfigure::TransformedLowerLeftCorner () const
{
  gp_XYZ tempLowerLeftCorner = theLowerLeftCorner;
  if (HasTransf()) Location().Transforms(tempLowerLeftCorner);
  gp_Pnt tempRes(tempLowerLeftCorner);

  return (tempRes);
}

Standard_Integer IGESDraw_RectArraySubfigure::ListCount () const
{
  return ( thePositions.IsNull() ? 0 : thePositions->Length() );
  // Return 0 if HArray1 thePositions is NULL Handle
}

Standard_Boolean IGESDraw_RectArraySubfigure::PositionNum (const Standard_Integer Index) const
{
  // Method : If thePositions array length is Zero return theDoDontFlag;
  //          else Search Index in to the Array. If 'Index' found in the
  //          array return theDoDontFlag else return !theDoDontFlag.

  if (thePositions.IsNull())   return theDoDontFlag;

  Standard_Integer I;
  Standard_Integer up  = thePositions->Upper();
  for (I = 1; I <= up; I++) {
    if (thePositions->Value(I) == Index)   return theDoDontFlag;
  }
  return (! theDoDontFlag);
}

Standard_Integer IGESDraw_RectArraySubfigure::ListPosition (const Standard_Integer Index) const
{
  return thePositions->Value(Index);
  // raise OutOfRange from Standard if Index is out-of-bound
  // Exception NoSuchObject will be raised if thePositions == Null Handle
}

void IGESDraw_RectArraySubfigure::OwnRead (IGESFile_Reader &PR)
{
  PR.ReadPointer(theBaseEntity,"Base Entity");

  theScaleFactor = 1.0;      // Setting to default value of 1.0
  PR.ReadReal(theScaleFactor,"Scale Factor");

  PR.ReadXYZ(theLowerLeftCorner,"Lower Left Coordinate Of Array");
  PR.ReadInteger(theNbColumns,"Number Of Columns");
  PR.ReadInteger(theNbRows,"Number Of Rows");
  PR.ReadReal(theColumnSeparation,"Horizontal Distance Between Columns");
  PR.ReadReal(theRowSeparation,"Vertical Distance Between Rows");
  PR.ReadReal(theRotationAngle,"Rotation Angle");

  Standard_Integer tempListCount = 0;
  PR.ReadInteger(tempListCount,"DO-DONT List Count");
  if (tempListCount > 0)
    thePositions = new TColStd_HArray1OfInteger (1, tempListCount);
  else if (tempListCount < 0)
    PR.AddFail("DO-DONT List Count : Less than Zero");

  Standard_Integer tempDoDontFlag = 0;
  PR.ReadInteger(tempDoDontFlag,"DO-DONT Flag");
  theDoDontFlag = (tempDoDontFlag != 0);

  if (tempListCount > 0) {
    for (Standard_Integer i = 1; i <= tempListCount; i++)
      PR.ReadInteger(thePositions->ChangeValue(i),"Number Of Position To Process");
  }
}

void IGESDraw_RectArraySubfigure::OwnWrite (IGESData_IGESWriter &IW) const
{
  IW.Send(theBaseEntity);
  IW.Send(theScaleFactor);
  IW.Send(theLowerLeftCorner.X());
  IW.Send(theLowerLeftCorner.Y());
  IW.Send(theLowerLeftCorner.Z());
  IW.Send(theNbColumns);
  IW.Send(theNbRows);
  IW.Send(theColumnSeparation);
  IW.Send(theRowSeparation);
  IW.Send(theRotationAngle);
  const Standard_Integer up = ListCount();
  IW.Send(up);
  IW.SendBoolean(DoDontFlag());
  for (Standard_Integer i = 1; i <= up; i++)
    IW.Send(ListPosition(i));
}

void IGESDraw_RectArraySubfigure::OwnShared (Interface_EntityIterator &iter) const
{
  iter.GetOneItem(theBaseEntity);
}

IGESData_DirChecker IGESDraw_RectArraySubfigure::DirChecker () const
{
  IGESData_DirChecker DC(412, 0);
  DC.Structure(IGESData_DefVoid);
  DC.LineFont(IGESData_DefAny);
  DC.LineWeight(IGESData_DefValue);
  DC.Color(IGESData_DefAny);
  DC.GraphicsIgnored(1);
  return DC;
}

void IGESDraw_RectArraySubfigure::OwnDump (const IGESData_IGESDumper &dumper, const Handle(Message_Messenger) &S, const Standard_Integer level) const
{
  const Standard_Integer tempSubLevel = (level <= 4) ? 0 : 1;
  S << "IGESDraw_RectArraySubfigure" << endl;
  S << "Base Entity : ";
  dumper.Dump(theBaseEntity,S,tempSubLevel);
  S << endl;
  S << "Scale Factor : " << theScaleFactor << "  ";
  S << "Lower Left Corner Of Array : ";
  IGESData_DumpXYZL(S,level,theLowerLeftCorner,gp_GTrsf()); // no location
  S << "Number Of Columns : " << theNbColumns   << "  ";
  S << "Number Of Rows    : " << theNbRows      << endl;
  S << "Horizontal Distance Between Columns : " << theColumnSeparation << endl;
  S << "Vertical Distance Between Rows      : " << theRowSeparation << endl;
  S << "Rotation Angle (in radians)         : " << theRotationAngle << endl;
  S << "Do-Dont Flag : " << (theDoDontFlag? "(1)Dont  " : "(0)Do  ");
  S << "Do-Dont List : ";
  IGESData_DumpVals(S,level,1,ListCount(),ListPosition);
  S << endl;
}
