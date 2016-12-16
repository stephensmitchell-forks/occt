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
#include <gp_XY.hxx>
#include <IGESData_DirChecker.hxx>
#include <IGESData_Dump.hxx>
#include <IGESData_IGESDumper.hxx>
#include <IGESData_IGESReaderData.hxx>
#include <IGESData_IGESWriter.hxx>
#include <IGESData_ParamCursor.hxx>
#include <IGESData_ParamReader.hxx>
#include <IGESDimen_LeaderArrow.hxx>
#include <IGESDimen_ToolLeaderArrow.hxx>
#include <Interface_Check.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_Macros.hxx>
#include <Interface_ShareTool.hxx>
#include <Message_Messenger.hxx>
#include <Standard_DomainError.hxx>
#include <TColgp_HArray1OfXY.hxx>


void  IGESDimen_ToolLeaderArrow::ReadOwnParams
  (const Handle(IGESDimen_LeaderArrow)& ent,
   const Handle(IGESData_IGESReaderData)& /* IR */, IGESData_ParamReader& PR) const
{ 
  Standard_Real arrowHeadHeight; 
  Standard_Real arrowHeadWidth; 
  Standard_Real zDepth; 
  gp_XY arrowHead; 
  Handle(TColgp_HArray1OfXY) segmentTails;
  Standard_Integer nbval;

  Standard_Boolean st = PR.ReadInteger(nbval,"Count of Segments");
  if (st && nbval > 0)
    segmentTails = new TColgp_HArray1OfXY(1,nbval);
  else  PR.AddFail("Count of Segments: Not Positive");

  PR.ReadReal(arrowHeadHeight,"Arrow Head Height");
  PR.ReadReal(arrowHeadWidth,"Arrow Head Width");
  PR.ReadReal(zDepth,"Z Depth");
  PR.ReadXY(arrowHead,"Arrow Head Position");

  if (! segmentTails.IsNull())
    {
      for (Standard_Integer i = 1; i <= nbval; i++)
	{
	  PR.ReadXY(segmentTails->ChangeValue(i),"Segment Co-ords.");
	}
      DirChecker(ent).CheckTypeAndForm(PR.CCheck(),ent);
      ent->Init (arrowHeadHeight, arrowHeadWidth, zDepth, arrowHead, segmentTails);
    } //segmentTails.IsNull() crash in ent->Init( ...

}

void  IGESDimen_ToolLeaderArrow::WriteOwnParams
  (const Handle(IGESDimen_LeaderArrow)& ent, IGESData_IGESWriter& IW) const 
{ 
  Standard_Integer upper = ent->NbSegments();
  IW.Send(upper);
  IW.Send(ent->ArrowHeadHeight());
  IW.Send(ent->ArrowHeadWidth());
  IW.Send(ent->ZDepth());
  IW.Send(ent->ArrowHead().X());
  IW.Send(ent->ArrowHead().Y());
  for (Standard_Integer i = 1; i <= upper; i++)
    {
      IW.Send((ent->SegmentTail(i)).X());
      IW.Send((ent->SegmentTail(i)).Y());
    }
}

IGESData_DirChecker  IGESDimen_ToolLeaderArrow::DirChecker
  (const Handle(IGESDimen_LeaderArrow)& /* ent */ ) const 
{ 
  IGESData_DirChecker DC (214, 1, 12);
  DC.Structure(IGESData_DefVoid);
  DC.LineFont(IGESData_DefAny);
  DC.LineWeight(IGESData_DefValue);
  DC.Color(IGESData_DefAny);
  DC.UseFlagRequired(1);
  DC.HierarchyStatusIgnored();
  return DC;
}

void  IGESDimen_ToolLeaderArrow::OwnDump
  (const Handle(IGESDimen_LeaderArrow)& ent, const IGESData_IGESDumper& /* dumper */,
   const Handle(Message_Messenger)& S, const Standard_Integer level) const 
{ 
  S << "IGESDimen_LeaderArrow" << endl;
  S << "Number of Segments : " << ent->NbSegments() << endl;
  S << "Arrowhead Height   : " << ent->ArrowHeadHeight() << endl;
  S << "Arrowhead Width    : " << ent->ArrowHeadWidth() << endl;
  S << "Z depth            : " << ent->ZDepth() << endl;
  S << "Arrowhead co-ords  : ";
  IGESData_DumpXYLZ(S,level,ent->ArrowHead(),ent->Location(),ent->ZDepth());
  S << endl << "Segment Tails : "; 
  IGESData_DumpListXYLZ(S ,level,1, ent->NbSegments(),ent->SegmentTail,
			ent->Location(), ent->ZDepth());
  S << endl;
}
