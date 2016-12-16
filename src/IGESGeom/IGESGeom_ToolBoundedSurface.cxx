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
#include <IGESGeom_Boundary.hxx>
#include <IGESGeom_BoundedSurface.hxx>
#include <IGESGeom_HArray1OfBoundary.hxx>
#include <IGESGeom_ToolBoundedSurface.hxx>
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

void IGESGeom_ToolBoundedSurface::ReadOwnParams(const Handle(IGESGeom_BoundedSurface)& ent,
                                                const Handle(IGESData_IGESReaderData)& IR,
                                                IGESData_ParamReader& PR) const
{
  // MGE 31/07/98

  //Standard_Boolean st; //szv#4:S4163:12Mar99 not needed
  Standard_Integer num, i;
  Standard_Integer tempType;
  Handle(IGESData_IGESEntity) tempSurface;
  Handle(IGESGeom_HArray1OfBoundary) tempBounds;
  IGESData_Status aStatus;

  if (!PR.ReadInteger(tempType)) {
    Message_Msg Msg165("XTSEP_165");
    PR.SendFail(Msg165);
  }
  if (!PR.ReadEntity(IR, aStatus, tempSurface)) {
    Message_Msg Msg166("XTSEP_166");
    switch(aStatus) {
    case IGESData_ReferenceError: {  
      Message_Msg Msg216 ("IGES_216");
      Msg166.Arg(Msg216.Value());
      PR.SendFail(Msg166);
      break; }
    case IGESData_EntityError: {
      Message_Msg Msg217 ("IGES_217");
      Msg166.Arg(Msg217.Value());
      PR.SendFail(Msg166);
      break; }
    default:{
    }
    }
  }

  if (PR.ReadInteger(num) && (num > 0)) {
    tempBounds = new IGESGeom_HArray1OfBoundary(1, num);
  }
  else{ 
    Message_Msg Msg167("XTSEP_167");
    PR.SendFail(Msg167);
  }

  if (!tempBounds.IsNull()){
    for ( i = 1; i <= num; i++ )
      {
	Handle(IGESData_IGESEntity) tempEnt;
	if (PR.ReadEntity(IR, aStatus, tempEnt))
	  tempBounds->SetValue(i, Handle(IGESGeom_Boundary)::DownCast (tempEnt));
	else{
	  Message_Msg Msg168("XTSEP_168");
	  switch(aStatus) {
	  case IGESData_ReferenceError: {  
	    Message_Msg Msg216 ("IGES_216");
	    Msg168.Arg(Msg216.Value());
	    PR.SendFail(Msg168);
	    break; }
	  case IGESData_EntityError: {
	    Message_Msg Msg217 ("IGES_217");
	    Msg168.Arg(Msg217.Value());
	    PR.SendFail(Msg168);
	    break; }
	  default:{
	  }
	  }
	}
      }
  }
    //sln 28.09.2001, BUC61004, If(tempBounds.IsNull()) function ent->Init is not called in order to avoid exception
  if(!tempBounds.IsNull())
    {
      DirChecker(ent).CheckTypeAndForm(PR.CCheck(),ent);
      ent->Init(tempType, tempSurface, tempBounds);
    }
}


//=======================================================================
//function : WriteOwnParams
//purpose  : 
//=======================================================================

void IGESGeom_ToolBoundedSurface::WriteOwnParams(const Handle(IGESGeom_BoundedSurface)& ent,
                                                 IGESData_IGESWriter& IW)  const
{
  Standard_Integer i, num;
  IW.Send(ent->RepresentationType());
  IW.Send(ent->Surface());
  IW.Send(ent->NbBoundaries());
  for ( num = ent->NbBoundaries(), i = 1; i <= num; i++ )
    IW.Send(ent->Boundary(i));
}


//=======================================================================
//function : OwnShared
//purpose  : 
//=======================================================================

void IGESGeom_ToolBoundedSurface::OwnShared(const Handle(IGESGeom_BoundedSurface)& ent,
                                            Interface_EntityIterator& iter) const
{
  Standard_Integer i, num;
  iter.GetOneItem(ent->Surface());
  for ( num = ent->NbBoundaries(), i = 1; i <= num; i++ )
    iter.GetOneItem(ent->Boundary(i));
}


//=======================================================================
//function : DirChecker
//purpose  : 
//=======================================================================

IGESData_DirChecker IGESGeom_ToolBoundedSurface::DirChecker
  (const Handle(IGESGeom_BoundedSurface)& /* ent */ )  const
{
  IGESData_DirChecker DC(143,0);
  DC.Structure(IGESData_DefVoid);
  DC.GraphicsIgnored();
  DC.LineFont(IGESData_DefAny);
//  DC.LineWeight(IGESData_DefValue);
  DC.Color(IGESData_DefAny);
  DC.UseFlagRequired(0);
  DC.HierarchyStatusIgnored();
  return DC;
}


//=======================================================================
//function : OwnCheck
//purpose  : 
//=======================================================================

void IGESGeom_ToolBoundedSurface::OwnCheck(const Handle(IGESGeom_BoundedSurface)& ent,
                                           const Interface_ShareTool&,
                                           Handle(Interface_Check)& ach)  const
{
  // MGE 31/07/98
  // Building of messages
  //========================================
  //Message_Msg Msg165("XTSEP_165");
  //========================================

  if ((ent->RepresentationType() != 0) && (ent->RepresentationType() != 1)) {
    Message_Msg Msg165("XTSEP_165");
    ach->SendFail(Msg165);
  }
}


//=======================================================================
//function : OwnDump
//purpose  : 
//=======================================================================

void IGESGeom_ToolBoundedSurface::OwnDump(const Handle(IGESGeom_BoundedSurface)& ent,
                                          const IGESData_IGESDumper& dumper,
                                          const Handle(Message_Messenger)& S,
                                          const Standard_Integer level)  const
{
  Standard_Integer sublevel = (level > 4) ? 1 : 0;
  S << "IGESGeom_BoundedSurface" << endl;
  S << "Representation Type   : " << ent->RepresentationType() << endl;
  S << "Surface to be Bounded : ";
  dumper.Dump(ent->Surface(),S, sublevel);
  S << endl;
  S << "Boundary Entities     : ";
  IGESData_DumpEntities(S,dumper ,level,1, ent->NbBoundaries(),ent->Boundary);
  S << endl;
}
