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

#include <gp_Pnt2d.hxx>
#include <gp_Vec2d.hxx>
#include <IGESGraph_UniformRectGrid.hxx>
#include <IGESFile_Reader.hxx>
#include <IGESData_IGESWriter.hxx>
#include <IGESData_DirChecker.hxx>
#include <Message_Messenger.hxx>
#include <IGESData_IGESDumper.hxx>
#include <IGESData_Dump.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESGraph_UniformRectGrid,IGESData_IGESEntity)

gp_Pnt2d IGESGraph_UniformRectGrid::GridPoint () const
{
  return ( gp_Pnt2d(theGridPoint) );
}

gp_Vec2d IGESGraph_UniformRectGrid::GridSpacing () const
{
  return ( gp_Vec2d(theGridSpacing) );
}

void IGESGraph_UniformRectGrid::OwnRead (IGESFile_Reader &PR)
{ 
  Standard_Integer aNbPropertyValues = 0;
  PR.ReadInteger(aNbPropertyValues,"No. of property values");
  if (aNbPropertyValues != 9)
    PR.AddFail("No. of Property values : Value is not 9");

  PR.ReadInteger(isItFinite,"Finite/infinite grid flag");
  PR.ReadInteger(isItLine,"Line/point grid flag");
  PR.ReadInteger(isItWeighted,"Weighted/unweighted grid flag");
  PR.ReadXY(theGridPoint,"Grid point coordinates");
  PR.ReadXY(theGridSpacing,"Grid Spacing coordinates");

  theNbPointsX = 0;
  if (isItFinite)
    PR.ReadInteger(theNbPointsX,"No. of points/lines in X direction");

  theNbPointsY = 0;
  if (isItFinite)
    PR.ReadInteger(theNbPointsY,"No. of points/lines in Y direction");
}

void IGESGraph_UniformRectGrid::OwnWrite (IGESData_IGESWriter &IW) const
{
  IW.Send(9);
  IW.SendBoolean(isItFinite != 0);
  IW.SendBoolean(isItLine != 0);
  IW.SendBoolean(isItWeighted != 0);
  IW.Send(theGridPoint.X());
  IW.Send(theGridPoint.Y());
  IW.Send(theGridSpacing.X());
  IW.Send(theGridSpacing.Y());
  IW.Send(theNbPointsX);  // ?? even if not IsFinite ??
  IW.Send(theNbPointsY);
}

IGESData_DirChecker IGESGraph_UniformRectGrid::DirChecker () const
{ 
  IGESData_DirChecker DC (406, 22);
  DC.Structure(IGESData_DefVoid);
  DC.LineFont(IGESData_DefVoid);
  DC.LineWeight(IGESData_DefVoid);
  DC.Color(IGESData_DefVoid);
  DC.BlankStatusIgnored();
  DC.UseFlagIgnored();
  DC.HierarchyStatusIgnored();
  return DC;
}

void IGESGraph_UniformRectGrid::OwnDump (const IGESData_IGESDumper &, const Handle(Message_Messenger) &S, const Standard_Integer) const
{
  S << "IGESGraph_UniformRectGrid" << endl;
  S << "No. of property values : 9" << endl;
  S << "Grid         : " << ( isItFinite ? "Finite" : "Infinite");
  S << "  -  Composed of " << ( isItLine ? "Lines" : "Points");
  S << "  -  " << ( isItWeighted ? "Weighted" : "Unweighted") << endl;
  S << "Grid Point   : ";
  IGESData_DumpXY(S,theGridPoint);
  S << "  Grid Spacing : ";
  IGESData_DumpXY(S,theGridSpacing);  S << endl;
  if (isItFinite)
    S << "No. of points/lines in direction :  X : " << theNbPointsX << "  -  Y : " << theNbPointsY << endl;
}
