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
#include <IGESAppli_Node.hxx>
#include <IGESData_TransfEntity.hxx>
#include <IGESGeom_TransformationMatrix.hxx>
#include <Interface_Macros.hxx>
#include <Interface_EntityIterator.hxx>
#include <IGESFile_Reader.hxx>
#include <IGESData_IGESWriter.hxx>
#include <Message_Messenger.hxx>
#include <IGESData_DirChecker.hxx>
#include <IGESData_IGESDumper.hxx>
#include <IGESData_Dump.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESAppli_Node,IGESData_IGESEntity)

Standard_Integer IGESAppli_Node::SystemType () const
{
  return (mySystem.IsNull()? 0 : (mySystem->FormNumber() - 9)); // 0 Global Cartesien, 1 Cartesien, 2 Cylind. 3 Spher.
}

gp_Pnt IGESAppli_Node::TransformedNodalCoord () const
{
  gp_XYZ tempCoord = myCoord;
  if (!mySystem.IsNull()) mySystem->Value().Transforms(tempCoord);
  return gp_Pnt(tempCoord);
}

void IGESAppli_Node::OwnRead (IGESFile_Reader &theReader)
{
  theReader.ReadXYZ(myCoord,"Coordinates of Node (XYZ)");
  theReader.ReadPointer(mySystem,"Transformation Matrix",Standard_True);
}

void IGESAppli_Node::OwnWrite (IGESData_IGESWriter &IW) const
{
  IW.Send(myCoord.X());
  IW.Send(myCoord.Y());
  IW.Send(myCoord.Z());
  IW.Send(mySystem);
}

void IGESAppli_Node::OwnShared(Interface_EntityIterator &theIter) const
{
  theIter.GetOneItem(mySystem);
}

IGESData_DirChecker IGESAppli_Node::DirChecker () const
{
  IGESData_DirChecker DC(134,0);  //Form no = 0 & Type = 134
  DC.Structure(IGESData_DefVoid);
  DC.LineFont(IGESData_DefVoid);
  DC.LineWeight(IGESData_DefVoid);
  DC.Color(IGESData_DefAny);
  DC.UseFlagRequired(04);
  return DC;
}

void IGESAppli_Node::OwnCheck (const Interface_ShareTool &, const Handle(Interface_Check) &theCheck) const
{
  if (!HasSubScriptNumber())
    theCheck->AddFail("SubScript Number expected (for Node Number) not present");
  if (!HasTransf())
    theCheck->AddFail("Transformation Matrix expected, not present");
  if (!mySystem.IsNull())
    if (mySystem->FormNumber() < 10)
      theCheck->AddFail("System : Incorrect FormNumber (not 10-11-12)");
}

void IGESAppli_Node::OwnDump (const IGESData_IGESDumper &dumper, const Handle(Message_Messenger) &S, const Standard_Integer level) const
{
  S << "IGESAppli_Node" << endl;
  S << "Nodal Coords : 1st " << myCoord.X() << "  2nd : " << myCoord.Y() << "  3rd : " << myCoord.Z() <<endl;
  S << "Nodal Displacement Coordinate System : ";
  if (!mySystem.IsNull())
    dumper.Dump(mySystem,S,level);
  else
    S << "Global Cartesian Coordinate System (default)";
  S << endl;
}
