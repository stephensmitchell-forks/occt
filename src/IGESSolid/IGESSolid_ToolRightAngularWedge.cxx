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

#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>
#include <gp_XYZ.hxx>
#include <IGESData_DirChecker.hxx>
#include <IGESData_Dump.hxx>
#include <IGESData_IGESDumper.hxx>
#include <IGESData_IGESReaderData.hxx>
#include <IGESData_IGESWriter.hxx>
#include <IGESData_ParamCursor.hxx>
#include <IGESData_ParamReader.hxx>
#include <IGESSolid_RightAngularWedge.hxx>
#include <IGESSolid_ToolRightAngularWedge.hxx>
#include <Interface_Check.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_ShareTool.hxx>
#include <Message_Messenger.hxx>
#include <Standard_DomainError.hxx>


void  IGESSolid_ToolRightAngularWedge::ReadOwnParams
  (const Handle(IGESSolid_RightAngularWedge)& ent,
   const Handle(IGESData_IGESReaderData)& /* IR */, IGESData_ParamReader& PR) const
{
  gp_XYZ tempSize, tempCorner, tempXAxis, tempZAxis;
  Standard_Real lowX;
  Standard_Real tempreal;

  PR.ReadXYZ(tempSize,"Size of RightAngularWedge");
  PR.ReadReal(lowX,"Small X length");

  if (PR.DefinedElseSkip())
    {
      if (PR.ReadReal(tempreal,"Corner Point (X)"))
	tempCorner.SetX(tempreal);
    }
  else  tempCorner.SetX(0.0);

  if (PR.DefinedElseSkip())
    {
      if (PR.ReadReal(tempreal,"Corner Point (Y)"))
	tempCorner.SetY(tempreal);
    }
  else  tempCorner.SetY(0.0);

  if (PR.DefinedElseSkip())
    {
      if (PR.ReadReal(tempreal,"Corner Point (Z)"))
	tempCorner.SetZ(tempreal);
    }
  else  tempCorner.SetZ(0.0);

  if (PR.DefinedElseSkip())
    {
      if (PR.ReadReal(tempreal,"Local X axis (I)"))
	tempXAxis.SetX(tempreal);
    }
  else  tempXAxis.SetX(1.0);

  if (PR.DefinedElseSkip())
    {
      if (PR.ReadReal(tempreal,"Local X axis (J)"))
	tempXAxis.SetY(tempreal);
    }
  else  tempXAxis.SetY(0.0);

  if (PR.DefinedElseSkip())
    {
      if (PR.ReadReal(tempreal,"Local X axis (K)"))
	tempXAxis.SetZ(tempreal);
    }
  else  tempXAxis.SetZ(0.0);

  if (PR.DefinedElseSkip())
    {
      if (PR.ReadReal(tempreal,"Local Z axis (I)"))
	tempZAxis.SetX(tempreal);
    }
  else  tempZAxis.SetX(0.0);

  if (PR.DefinedElseSkip())
    {
      if (PR.ReadReal(tempreal,"Local Z axis (J)"))
	tempZAxis.SetY(tempreal);
    }
  else  tempZAxis.SetY(0.0);

  if (PR.DefinedElseSkip())
    {
      if (PR.ReadReal(tempreal,"Local Z axis (K)"))
	tempZAxis.SetZ(tempreal);
    }
  else  tempZAxis.SetZ(1.0);

  DirChecker(ent).CheckTypeAndForm(PR.CCheck(),ent);
  ent->Init (tempSize, lowX, tempCorner, tempXAxis, tempZAxis);
  Standard_Real eps = 1.E-05;
  if (!tempXAxis.IsEqual(ent->XAxis().XYZ(),eps)) PR.AddWarning
    ("XAxis poorly unitary, normalized");
  if (!tempZAxis.IsEqual(ent->ZAxis().XYZ(),eps)) PR.AddWarning
    ("ZAxis poorly unitary, normalized");
}

void  IGESSolid_ToolRightAngularWedge::WriteOwnParams
  (const Handle(IGESSolid_RightAngularWedge)& ent, IGESData_IGESWriter& IW) const
{
  IW.Send(ent->Size().X());
  IW.Send(ent->Size().Y());
  IW.Send(ent->Size().Z());
  IW.Send(ent->XSmallLength());
  IW.Send(ent->Corner().X());
  IW.Send(ent->Corner().Y());
  IW.Send(ent->Corner().Z());
  IW.Send(ent->XAxis().X());
  IW.Send(ent->XAxis().Y());
  IW.Send(ent->XAxis().Z());
  IW.Send(ent->ZAxis().X());
  IW.Send(ent->ZAxis().Y());
  IW.Send(ent->ZAxis().Z());
}

IGESData_DirChecker  IGESSolid_ToolRightAngularWedge::DirChecker
  (const Handle(IGESSolid_RightAngularWedge)& /* ent */ ) const
{
  IGESData_DirChecker DC(152, 0);
  DC.Structure  (IGESData_DefVoid);
  DC.LineFont   (IGESData_DefAny);
  DC.Color      (IGESData_DefAny);

  DC.UseFlagRequired (0);
  DC.HierarchyStatusIgnored ();
  return DC;
}

void  IGESSolid_ToolRightAngularWedge::OwnDump
  (const Handle(IGESSolid_RightAngularWedge)& ent, const IGESData_IGESDumper& /* dumper */,
   const Handle(Message_Messenger)& S, const Standard_Integer level) const
{
  S << "IGESSolid_RightAngularWedge" << endl;

  S << "Size   : ";
  IGESData_DumpXYZ(S, ent->Size());  S << "  ";
  S << "XSmall : " << ent->XSmallLength() << endl;
  S << "Corner : ";
  IGESData_DumpXYZL(S,level, ent->Corner(), ent->Location());
  S << endl << "XAxis : ";
  IGESData_DumpXYZL(S,level, ent->XAxis(), ent->VectorLocation());
  S << endl << "ZAxis : ";
  IGESData_DumpXYZL(S,level, ent->ZAxis(), ent->VectorLocation());
  S << endl;
}
