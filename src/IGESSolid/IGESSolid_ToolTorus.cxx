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
#include <IGESSolid_ToolTorus.hxx>
#include <IGESSolid_Torus.hxx>
#include <Interface_Check.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_ShareTool.hxx>
#include <Message_Messenger.hxx>
#include <Standard_DomainError.hxx>


void  IGESSolid_ToolTorus::ReadOwnParams
  (const Handle(IGESSolid_Torus)& ent,
   const Handle(IGESData_IGESReaderData)& /* IR */, IGESData_ParamReader& PR) const
{
  Standard_Real r1, r2;
  Standard_Real tempreal;
  gp_XYZ tempPoint, tempAxis;

  PR.ReadReal(r1,"Radius of revolution");
  PR.ReadReal(r2,"Radius of disc");

  if (PR.DefinedElseSkip())
    {
      if (PR.ReadReal(tempreal,"Center Point (X)"))
	tempPoint.SetX(tempreal);
    }
  else  tempPoint.SetX(0.0);

  if (PR.DefinedElseSkip())
    {
      if (PR.ReadReal(tempreal,"Center Point (Y)"))
	tempPoint.SetY(tempreal);
    }
  else  tempPoint.SetY(0.0);

  if (PR.DefinedElseSkip())
    {
      if (PR.ReadReal(tempreal,"Center Point (Z)"))
	tempPoint.SetZ(tempreal);
    }
  else  tempPoint.SetZ(0.0);

  if (PR.DefinedElseSkip())
    {
      if (PR.ReadReal(tempreal,"Axis direction (I)"))
	tempAxis.SetX(tempreal);
    }
  else  tempAxis.SetX(0.0);

  if (PR.DefinedElseSkip())
    {
      if (PR.ReadReal(tempreal,"Axis direction (J)"))
	tempAxis.SetY(tempreal);
    }
  else  tempAxis.SetY(0.0);

  if (PR.DefinedElseSkip())
    {
      if (PR.ReadReal(tempreal,"Axis direction (K)"))
	tempAxis.SetZ(tempreal);
    }
  else  tempAxis.SetZ(1.0);

  DirChecker(ent).CheckTypeAndForm(PR.CCheck(),ent);
  ent->Init (r1, r2, tempPoint, tempAxis);
  Standard_Real eps = 1.E-05;
  if (!tempAxis.IsEqual(ent->Axis().XYZ(),eps))
    PR.AddWarning("Axis poorly unitary, normalized");
}

void  IGESSolid_ToolTorus::WriteOwnParams
  (const Handle(IGESSolid_Torus)& ent, IGESData_IGESWriter& IW) const
{
  IW.Send(ent->MajorRadius());
  IW.Send(ent->DiscRadius());
  IW.Send(ent->AxisPoint().X());
  IW.Send(ent->AxisPoint().Y());
  IW.Send(ent->AxisPoint().Z());
  IW.Send(ent->Axis().X());
  IW.Send(ent->Axis().Y());
  IW.Send(ent->Axis().Z());
}

IGESData_DirChecker  IGESSolid_ToolTorus::DirChecker
  (const Handle(IGESSolid_Torus)& /* ent */ ) const
{
  IGESData_DirChecker DC(160, 0);

  DC.Structure  (IGESData_DefVoid);
  DC.LineFont   (IGESData_DefAny);
  DC.Color      (IGESData_DefAny);

  DC.UseFlagRequired (0);
  DC.HierarchyStatusIgnored ();
  return DC;
}

void  IGESSolid_ToolTorus::OwnDump
  (const Handle(IGESSolid_Torus)& ent, const IGESData_IGESDumper& /* dumper */,
   const Handle(Message_Messenger)& S, const Standard_Integer level) const
{
  S << "IGESSolid_Torus" << endl;

  S << "Radius of revolution : " << ent->MajorRadius() << "  ";
  S << "Radius of the disc   : " << ent->DiscRadius()  << endl;
  S << "Center Point   : ";
  IGESData_DumpXYZL(S,level, ent->AxisPoint(), ent->Location());
  S << endl << "Axis direction : ";
  IGESData_DumpXYZL(S,level, ent->Axis(), ent->VectorLocation());
  S << endl;
}
