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

#include <IGESData_DirChecker.hxx>
#include <IGESData_IGESDumper.hxx>
#include <IGESData_IGESEntity.hxx>
#include <IGESData_IGESReaderData.hxx>
#include <IGESData_IGESWriter.hxx>
#include <IGESData_ParamCursor.hxx>
#include <IGESData_ParamReader.hxx>
#include <IGESData_Status.hxx>
#include <IGESGeom_Line.hxx>
#include <IGESGeom_SurfaceOfRevolution.hxx>
#include <IGESGeom_ToolSurfaceOfRevolution.hxx>
#include <Interface_Check.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_Macros.hxx>
#include <Interface_ShareTool.hxx>
#include <Message_Messenger.hxx>
#include <Message_Msg.hxx>
#include <Standard_DomainError.hxx>


void IGESGeom_ToolSurfaceOfRevolution::ReadOwnParams
  (const Handle(IGESGeom_SurfaceOfRevolution)& ent,
   const Handle(IGESData_IGESReaderData)& IR, IGESData_ParamReader& PR) const
{
  Handle(IGESGeom_Line) anAxis;
  Handle(IGESData_IGESEntity) aGeneratrix;
  Standard_Real aStartAngle, anEndAngle;
  IGESData_Status aStatus;

  if (!PR.ReadEntity(IR, aStatus, STANDARD_TYPE(IGESGeom_Line), anAxis)) {
    Message_Msg Msg152("XSTEP_152");
    switch(aStatus) {
    case IGESData_ReferenceError: {  
      Message_Msg Msg216 ("IGES_216");
      Msg152.Arg(Msg216.Value());
      PR.SendFail(Msg152);
      break; }
    case IGESData_EntityError: {
      Message_Msg Msg217 ("IGES_217");
      Msg152.Arg(Msg217.Value());
      PR.SendFail(Msg152);
      break; }
    case IGESData_TypeError: {
      Message_Msg Msg218 ("IGES_218");
      Msg152.Arg(Msg218.Value());
      PR.SendFail(Msg152);
      break; }
    default:{
    }
    }
  }
  if (!PR.ReadEntity(IR, aStatus, aGeneratrix)) {
    Message_Msg Msg153("XSTEP_153");
    switch(aStatus) {
    case IGESData_ReferenceError: {  
      Message_Msg Msg216 ("IGES_216");
      Msg153.Arg(Msg216.Value());
      PR.SendFail(Msg153);
      break; }
    case IGESData_EntityError: {
      Message_Msg Msg217 ("IGES_217");
      Msg153.Arg(Msg217.Value());
      PR.SendFail(Msg153);
      break; }
    default:{
    }
    }
  }
  if (!PR.ReadReal(aStartAngle)) {
    Message_Msg Msg154("XSTEP_154");
    PR.SendFail(Msg154);
  }
  if (!PR.ReadReal(anEndAngle)) {
    Message_Msg Msg155("XSTEP_155");
    PR.SendFail(Msg155);
  }
  DirChecker(ent).CheckTypeAndForm(PR.CCheck(),ent);
  ent->Init(anAxis, aGeneratrix, aStartAngle, anEndAngle);
}

void IGESGeom_ToolSurfaceOfRevolution::WriteOwnParams
  (const Handle(IGESGeom_SurfaceOfRevolution)& ent, IGESData_IGESWriter& IW)  const
{
  IW.Send(ent->AxisOfRevolution());
  IW.Send(ent->Generatrix());
  IW.Send(ent->StartAngle());
  IW.Send(ent->EndAngle());
}

void  IGESGeom_ToolSurfaceOfRevolution::OwnShared
  (const Handle(IGESGeom_SurfaceOfRevolution)& ent, Interface_EntityIterator& iter) const
{
  iter.GetOneItem(ent->AxisOfRevolution());
  iter.GetOneItem(ent->Generatrix());
}


IGESData_DirChecker IGESGeom_ToolSurfaceOfRevolution::DirChecker
  (const Handle(IGESGeom_SurfaceOfRevolution)& /*ent*/ )   const
{
  IGESData_DirChecker DC(120, 0);
  DC.Structure(IGESData_DefVoid);
  DC.LineFont(IGESData_DefAny);
//  DC.LineWeight(IGESData_DefValue);
  DC.Color(IGESData_DefAny);

  DC.HierarchyStatusIgnored ();
  return DC;
}

void IGESGeom_ToolSurfaceOfRevolution::OwnDump
  (const Handle(IGESGeom_SurfaceOfRevolution)& ent, const IGESData_IGESDumper& dumper,
   const Handle(Message_Messenger)& S, const Standard_Integer level)  const
{

  S << "IGESGeom_SurfaceOfRevolution" << endl << endl;
  Standard_Integer sublevel = (level <= 4) ? 0 : 1;

  S << "Axis Of Revolution : ";
  dumper.Dump(ent->AxisOfRevolution(),S, sublevel);
  S << endl;
  S << "Generatrix         : ";
  dumper.Dump(ent->Generatrix(),S, sublevel);
  S << endl;
  S << "Start Angle        : " << ent->StartAngle() << "  ";
  S << "End Angle   : " << ent->EndAngle() << endl;
}
