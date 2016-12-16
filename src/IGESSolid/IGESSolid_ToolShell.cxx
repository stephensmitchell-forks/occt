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
#include <IGESData_IGESReaderData.hxx>
#include <IGESData_IGESWriter.hxx>
#include <IGESData_ParamCursor.hxx>
#include <IGESData_ParamReader.hxx>
#include <IGESData_Status.hxx>
#include <IGESSolid.hxx>
#include <IGESSolid_Face.hxx>
#include <IGESSolid_HArray1OfFace.hxx>
#include <IGESSolid_Shell.hxx>
#include <IGESSolid_ToolShell.hxx>
#include <Interface_Check.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_Macros.hxx>
#include <Interface_ShareTool.hxx>
#include <Message_Messenger.hxx>
#include <Message_Msg.hxx>
#include <Standard_DomainError.hxx>
#include <TColStd_HArray1OfInteger.hxx>


//=======================================================================
//function : ReadOwnParams
//purpose  : 
//=======================================================================

void IGESSolid_ToolShell::ReadOwnParams(const Handle(IGESSolid_Shell)& ent,
                                        const Handle(IGESData_IGESReaderData)& IR,
                                        IGESData_ParamReader& PR) const
{
  Standard_Integer nbfaces=0;
  Handle(IGESSolid_HArray1OfFace) tempFaces;
  Handle(TColStd_HArray1OfInteger) tempOrientation;

  Standard_Boolean sb = PR.ReadInteger(nbfaces);
  if (sb && nbfaces > 0 ) {
    Message_Msg Msg180("XSTEP_180");
    
    Standard_Boolean abool;
    Handle(IGESSolid_Face) aface;
    tempFaces = new IGESSolid_HArray1OfFace(1, nbfaces);
    tempOrientation = new TColStd_HArray1OfInteger(1, nbfaces);
    IGESData_Status aStatus;
    for (Standard_Integer i=1; i<=nbfaces; i++) {
      if (PR.ReadEntity(IR, aStatus, STANDARD_TYPE(IGESSolid_Face), aface))
	tempFaces->SetValue(i, aface);
      else{
	Message_Msg Msg201("XSTEP_201");
	switch(aStatus) {
	case IGESData_ReferenceError: {  
	  Message_Msg Msg216 ("IGES_216");
	  Msg201.Arg(Msg216.Value());
	  PR.SendFail(Msg201);
	  break; }
	case IGESData_EntityError: {
	  Message_Msg Msg217 ("IGES_217");
	  Msg201.Arg(Msg217.Value());
	  PR.SendFail(Msg201);
	  break; }
	case IGESData_TypeError: {
	  Message_Msg Msg218 ("IGES_218");
	  Msg201.Arg(Msg218.Value());
	  PR.SendFail(Msg201);
	  break; }
	default:{
	}
	}
      }
      if (PR.ReadBoolean(Msg180, abool))
	tempOrientation->SetValue(i, (abool ? 1 : 0) );
    }
  }
  else {
    Message_Msg Msg200("XSTEP_200");
    PR.SendFail(Msg200);
  }
  
  DirChecker(ent).CheckTypeAndForm(PR.CCheck(),ent);
  ent->Init (tempFaces, tempOrientation);
}


//=======================================================================
//function : WriteOwnParams
//purpose  : 
//=======================================================================

void IGESSolid_ToolShell::WriteOwnParams(const Handle(IGESSolid_Shell)& ent,
                                         IGESData_IGESWriter& IW) const
{
  Standard_Integer i, nbfaces = ent->NbFaces();

  IW.Send(nbfaces);
  for (i = 1; i <= nbfaces; i ++)
    {
      IW.Send(ent->Face(i));
      IW.SendBoolean(ent->Orientation(i));
    }
}


//=======================================================================
//function : DirChecker
//purpose  : 
//=======================================================================

IGESData_DirChecker IGESSolid_ToolShell::DirChecker
  (const Handle(IGESSolid_Shell)& /* ent */ ) const
{
  IGESData_DirChecker DC(514, 1,2);

  DC.Structure  (IGESData_DefVoid);
  DC.LineFont   (IGESData_DefVoid);
  DC.LineWeight (IGESData_DefVoid);
  DC.Color      (IGESData_DefVoid);

  DC.SubordinateStatusRequired(1);
  return DC;
}


//=======================================================================
//function : OwnDump
//purpose  : 
//=======================================================================

void IGESSolid_ToolShell::OwnDump(const Handle(IGESSolid_Shell)& ent,
                                  const IGESData_IGESDumper& dumper,
                                  const Handle(Message_Messenger)& S,
                                  const Standard_Integer level) const
{
  S << "IGESSolid_Shell" << endl;
  Standard_Integer upper = ent->NbFaces();
  Standard_Integer sublevel = (level <= 4) ? 0 : 1;

  S << "Faces : " << endl << "Orientation flags : ";
  IGESData_DumpEntities(S,dumper,-level,1, ent->NbFaces(),ent->Face);
  S << endl;
  if (level > 4)
    {
      S << "[" << endl;
      for (Standard_Integer i = 1; i <= upper; i ++)
	{
          S << "[" << i << "]:  ";
          S << "Face : ";
          dumper.Dump (ent->Face(i),S, sublevel);
          S << "  - Orientation flag : ";
          if (ent->Orientation(i)) S << "True" << endl;
          else                     S << "False" << endl;
	}
    }
  S << endl;
}
