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
#include <gp_XYZ.hxx>
#include <IGESData_TransfEntity.hxx>
#include <IGESDraw_View.hxx>
#include <IGESGeom_Plane.hxx>
#include <IGESFile_Reader.hxx>
#include <IGESData_IGESWriter.hxx>
#include <Interface_EntityIterator.hxx>
#include <IGESData_DirChecker.hxx>
#include <Message_Messenger.hxx>
#include <IGESData_IGESDumper.hxx>
#include <IGESData_Dump.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESDraw_View,IGESData_ViewKindEntity)

Standard_Boolean IGESDraw_View::IsSingle () const
{
  return Standard_True;
}

Standard_Integer IGESDraw_View::NbViews () const
{
  return 1;
}

Handle(IGESData_ViewKindEntity) IGESDraw_View::ViewItem (const Standard_Integer) const
{
  return Handle(IGESData_ViewKindEntity)(this);
}

gp_XYZ IGESDraw_View::ModelToView (const gp_XYZ& coords) const
{
  gp_XYZ tempCoords = coords;
  Location().Transforms(tempCoords);
  return (tempCoords);
}

void IGESDraw_View::OwnRead (IGESFile_Reader &PR)
{
  PR.ReadInteger(theViewNumber,"View Number");

  theScaleFactor = 1.0;      // Setting to default value of 1.0
  PR.ReadReal(theScaleFactor,"Scale Factor");

  PR.ReadPointer(theLeftPlane,"Left Side Of View Volume",Standard_True);
  PR.ReadPointer(theTopPlane,"Top Side Of View Volume",Standard_True);
  PR.ReadPointer(theRightPlane,"Right Side Of View Volume",Standard_True);
  PR.ReadPointer(theBottomPlane,"Bottom Side Of View Volume",Standard_True);
  PR.ReadPointer(theBackPlane,"Back Side Of View Volume",Standard_True);
  PR.ReadPointer(theFrontPlane,"Front Side Of View Volume",Standard_True);
}

void IGESDraw_View::OwnWrite (IGESData_IGESWriter &IW) const
{
  IW.Send(theViewNumber);
  IW.Send(theScaleFactor);
  IW.Send(theLeftPlane);
  IW.Send(theTopPlane);
  IW.Send(theRightPlane);
  IW.Send(theBottomPlane);
  IW.Send(theBackPlane);
  IW.Send(theFrontPlane);
}

void IGESDraw_View::OwnShared (Interface_EntityIterator &iter) const
{
  iter.GetOneItem(theLeftPlane);
  iter.GetOneItem(theTopPlane);
  iter.GetOneItem(theRightPlane);
  iter.GetOneItem(theBottomPlane);
  iter.GetOneItem(theBackPlane);
  iter.GetOneItem(theFrontPlane);
}

IGESData_DirChecker IGESDraw_View::DirChecker () const
{
  IGESData_DirChecker DC(410, 0);
  DC.Structure(IGESData_DefVoid);
  DC.LineFont(IGESData_DefVoid);
  DC.LineWeight(IGESData_DefVoid);
  DC.Color(IGESData_DefVoid);
  DC.BlankStatusIgnored();
  DC.UseFlagRequired(1);
  DC.HierarchyStatusIgnored();
  return DC;
}

void IGESDraw_View::OwnDump (const IGESData_IGESDumper &dumper, const Handle(Message_Messenger) &S, const Standard_Integer level) const
{
  const Standard_Integer tempSubLevel = (level <= 4) ? 0 : 1;
  S << "IGESDraw_View" << endl;
  S << "View Number  : " << theViewNumber  << endl;
  S << "Scale Factor : " << theScaleFactor << endl;
  S << "Left Plane Of View Volume   : ";
  dumper.Dump(theLeftPlane,S,tempSubLevel);    S << endl;
  S << "Top Plane Of View Volume    : ";
  dumper.Dump(theTopPlane,S,tempSubLevel);     S << endl;
  S << "Right Plane Of View Volume  : ";
  dumper.Dump(theRightPlane,S,tempSubLevel);   S << endl;
  S << "Bottom Plane Of View Volume : ";
  dumper.Dump(theBottomPlane,S,tempSubLevel);  S << endl;
  S << "Back Plane Of View Volume   : ";
  dumper.Dump(theBackPlane,S,tempSubLevel);    S << endl;
  S << "Front Plane Of View Volume  : ";
  dumper.Dump(theFrontPlane,S,tempSubLevel);   S << endl;
}
