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
#include <IGESData_Dump.hxx>
#include <IGESData_IGESDumper.hxx>
#include <IGESData_IGESEntity.hxx>
#include <IGESData_IGESReaderData.hxx>
#include <IGESData_IGESWriter.hxx>
#include <IGESData_ParamCursor.hxx>
#include <IGESData_ParamReader.hxx>
#include <IGESData_Status.hxx>
#include <IGESSolid_Face.hxx>
#include <IGESSolid_HArray1OfLoop.hxx>
#include <IGESSolid_Loop.hxx>
#include <IGESSolid_ToolFace.hxx>
#include <Interface_Check.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_Macros.hxx>
#include <Interface_ShareTool.hxx>
#include <Message_Messenger.hxx>
#include <Message_Msg.hxx>
#include <Standard_DomainError.hxx>


//=======================================================================
//function : ReadOwnParams
//purpose  : 
//=======================================================================

void IGESSolid_ToolFace::ReadOwnParams(const Handle(IGESSolid_Face)& ent,
                                       const Handle(IGESData_IGESReaderData)& IR,
                                       IGESData_ParamReader& PR) const
{
  // MGE 03/08/98
  // Building of messages
  //========================================
  Message_Msg Msg197("XSTEP_197");
  Message_Msg Msg198("XSTEP_198");
  //========================================

  Standard_Boolean outerLoopFlag;
  Handle(IGESData_IGESEntity) anent;
  Handle(IGESSolid_Loop) aloop;
  Handle(IGESData_IGESEntity) tempSurface;
  Standard_Integer nbloops;
  Handle(IGESSolid_HArray1OfLoop) tempLoops;
  IGESData_Status aStatus;

  if (!PR.ReadEntity(IR, aStatus, tempSurface)) {
    Message_Msg Msg196("XSTEP_196");
    switch(aStatus) {
    case IGESData_ReferenceError: {  
      Message_Msg Msg216 ("IGES_216");
      Msg196.Arg(Msg216.Value());
      PR.SendFail(Msg196);
      break; }
    case IGESData_EntityError: {
      Message_Msg Msg217 ("IGES_217");
      Msg196.Arg(Msg217.Value());
      PR.SendFail(Msg196);
      break; }
    default:{
    }
    }
  }
  Standard_Boolean st = PR.ReadInteger(nbloops);
  if(!st){
    PR.SendFail(Msg197);
  }
  if (st && nbloops > 0) tempLoops = new IGESSolid_HArray1OfLoop(1, nbloops);
  else  PR.SendFail(Msg197);

  PR.ReadBoolean(Msg198, outerLoopFlag);

  if (!tempLoops.IsNull()) {
    for (Standard_Integer i=1; i<=nbloops; i++) {
      if (PR.ReadEntity(IR, aStatus, STANDARD_TYPE(IGESSolid_Loop), aloop))
	tempLoops->SetValue(i, aloop);
      else{
	Message_Msg Msg199("XSTEP_199");
	switch(aStatus) {
	case IGESData_ReferenceError: {  
	  Message_Msg Msg216 ("IGES_216");
	  Msg199.Arg(Msg216.Value());
	  PR.SendFail(Msg199);
	  break; }
	case IGESData_EntityError: {
	  Message_Msg Msg217 ("IGES_217");
	  Msg199.Arg(Msg217.Value());
	  PR.SendFail(Msg199);
	  break; }
	case IGESData_TypeError: {
	  Message_Msg Msg218 ("IGES_218");
	  Msg199.Arg(Msg218.Value());
	  PR.SendFail(Msg199);
	  break; }
	default:{
	}
	}
      }
    }
  }
  DirChecker(ent).CheckTypeAndForm(PR.CCheck(),ent);
  ent->Init (tempSurface, outerLoopFlag, tempLoops);
}


//=======================================================================
//function : WriteOwnParams
//purpose  : 
//=======================================================================

void IGESSolid_ToolFace::WriteOwnParams(const Handle(IGESSolid_Face)& ent,
                                        IGESData_IGESWriter& IW) const
{
  Standard_Integer  upper = ent->NbLoops();
  IW.Send(ent->Surface());
  IW.Send(upper);
  IW.SendBoolean(ent->HasOuterLoop());
  for (Standard_Integer i = 1; i <= upper; i ++)
    IW.Send(ent->Loop(i));
}


//=======================================================================
//function : DirChecker
//purpose  : 
//=======================================================================

IGESData_DirChecker IGESSolid_ToolFace::DirChecker
  (const Handle(IGESSolid_Face)& /* ent */ ) const
{
  IGESData_DirChecker DC(510, 1);

  DC.Structure  (IGESData_DefVoid);
  DC.LineFont   (IGESData_DefVoid);
  DC.LineWeight (IGESData_DefVoid);
  DC.Color      (IGESData_DefAny);

  DC.SubordinateStatusRequired(1);
  return DC;
}


//=======================================================================
//function : OwnDump
//purpose  : 
//=======================================================================

void IGESSolid_ToolFace::OwnDump(const Handle(IGESSolid_Face)& ent,
                                 const IGESData_IGESDumper& dumper,
                                 const Handle(Message_Messenger)& S,
                                 const Standard_Integer level) const
{
  S << "IGESSolid_Face" << endl;

  Standard_Integer sublevel = (level <= 4) ? 0 : 1;
  S << "Surface : ";
  dumper.Dump(ent->Surface(),S, sublevel);
  S << endl;
  if (ent->HasOuterLoop())  S << "Outer loop is present (First one)" << endl;
  else                      S << "Outer loop is not present" << endl;
  S << "Loops : ";
  IGESData_DumpEntities(S,dumper ,level,1, ent->NbLoops(),ent->Loop);
  S << endl;
}

