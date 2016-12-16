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
#include <IGESGraph_TextDisplayTemplate.hxx>
#include <IGESGraph_TextFontDef.hxx>
#include <IGESFile_Reader.hxx>
#include <IGESData_IGESWriter.hxx>
#include <Interface_EntityIterator.hxx>
#include <IGESData_DirChecker.hxx>
#include <Message_Messenger.hxx>
#include <IGESData_IGESDumper.hxx>
#include <IGESData_Dump.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESGraph_TextDisplayTemplate,IGESData_IGESEntity)

gp_Pnt IGESGraph_TextDisplayTemplate::StartingCorner () const
{
  return ( gp_Pnt(myCorner) );
}

gp_Pnt IGESGraph_TextDisplayTemplate::TransformedStartingCorner () const
{
  gp_XYZ TempXYZ = myCorner;
  if (HasTransf()) Location().Transforms(TempXYZ);
  return ( gp_Pnt(TempXYZ) );
}

void IGESGraph_TextDisplayTemplate::OwnRead (IGESFile_Reader &PR)
{ 
  PR.ReadReal(myBoxWidth,"Character box width");
  PR.ReadReal(myBoxHeight,"Character box height");

  myFontCode = 1;
  PR.ReadInteger(myFontCode,"Font Code");
  if (myFontCode < 0)
    PR.GetPointer(myFontEntity,-myFontCode);

  mySlantAngle = M_PI/2.0;
  PR.ReadReal(mySlantAngle,"Slant Angle");

  PR.ReadReal(myRotationAngle,"Rotation Angle");
  PR.ReadInteger(myMirrorFlag,"Mirror Flag");
  PR.ReadInteger(myRotateFlag,"Rotate Flag");
  PR.ReadXYZ(myCorner,"Lower left coordinates/Increments");
}

void IGESGraph_TextDisplayTemplate::OwnWrite (IGESData_IGESWriter &IW) const
{ 
  IW.Send(myBoxWidth);
  IW.Send(myBoxHeight);

  if (!myFontEntity.IsNull())
    IW.Send(myFontEntity, Standard_True);  // negative
  else
    IW.Send(myFontCode);

  IW.Send(mySlantAngle);
  IW.Send(myRotationAngle);
  IW.Send(myMirrorFlag);
  IW.Send(myRotateFlag);
  IW.Send(myCorner.X());
  IW.Send(myCorner.Y());
  IW.Send(myCorner.Z());
}

void IGESGraph_TextDisplayTemplate::OwnShared (Interface_EntityIterator &theIter) const
{
  if (!myFontEntity.IsNull())
    theIter.GetOneItem(myFontEntity);
}

IGESData_DirChecker IGESGraph_TextDisplayTemplate::DirChecker () const
{ 
  IGESData_DirChecker DC (312, 0, 1);
  DC.Structure(IGESData_DefVoid);
  DC.LineFont(IGESData_DefVoid);
  DC.LineWeight(IGESData_DefVoid);
  DC.Color(IGESData_DefAny);
  DC.SubordinateStatusRequired(0);
  DC.UseFlagRequired(2);
  DC.HierarchyStatusRequired(0);
  return DC;
}

void IGESGraph_TextDisplayTemplate::OwnDump (const IGESData_IGESDumper &dumper, const Handle(Message_Messenger) &S, const Standard_Integer level) const
{
  S << "IGESGraph_TextDisplayTemplate" << endl;
  S << "Character box width  : "  << myBoxWidth << "  ";
  S << "Character box height : "  << myBoxHeight << endl;
  if (!myFontEntity.IsNull())
  {
    const Standard_Integer sublevel = (level <= 4) ? 0 : 1;
    S << "Font Entity : ";
    dumper.Dump(myFontEntity,S,sublevel);
  }
  else
    S << "Font code : " << myFontCode;
  S << endl;
  S << "Slant angle    : "  << mySlantAngle << "  ";
  S << "Rotation angle : "  << myRotationAngle << endl;
  S << "Mirror flag    : "  << myMirrorFlag << "  ";
  S << "Rotate flag    : "  << myRotateFlag << endl;
  if ( myForm == 0 )
    S << "Lower Left Corner coordinates : ";
  else
    S << "Increments from coordinates : "; 
  IGESData_DumpXYZL(S,level,myCorner,Location());
  S << endl;
}
