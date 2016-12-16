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

#include <gp_Pnt.hxx>
#include <gp_XYZ.hxx>
#include <IGESData_DirChecker.hxx>
#include <IGESData_Dump.hxx>
#include <IGESData_IGESDumper.hxx>
#include <IGESData_IGESReaderData.hxx>
#include <IGESData_IGESWriter.hxx>
#include <IGESData_ParamCursor.hxx>
#include <IGESData_ParamReader.hxx>
#include <IGESSolid_ToolVertexList.hxx>
#include <IGESSolid_VertexList.hxx>
#include <Interface_Check.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_Macros.hxx>
#include <Interface_ShareTool.hxx>
#include <Message_Messenger.hxx>
#include <Message_Msg.hxx>
#include <Standard_DomainError.hxx>
#include <TColgp_HArray1OfXYZ.hxx>


//=======================================================================
//function : ReadOwnParams
//purpose  : 
//=======================================================================

void IGESSolid_ToolVertexList::ReadOwnParams(const Handle(IGESSolid_VertexList)& ent,
                                             const Handle(IGESData_IGESReaderData)& /* IR */,
                                             IGESData_ParamReader& PR) const
{
  Standard_Integer nbitems = 0;
  Handle(TColgp_HArray1OfXYZ) tempVertices;

  Standard_Boolean sb = PR.ReadInteger(nbitems);
  if (sb && (nbitems > 0)) {
    
    tempVertices = new TColgp_HArray1OfXYZ(1, nbitems);
    
    for (Standard_Integer i = 1; i <= nbitems; i++)
      {
	PR.ReadXYZ(tempVertices->ChangeValue(i));
      }
  }
  else {
    Message_Msg Msg182("XSTEP_182");
    PR.SendFail(Msg182);
  }

  DirChecker(ent).CheckTypeAndForm(PR.CCheck(),ent);
  if (nbitems > 0) ent->Init (tempVertices);
}


//=======================================================================
//function : WriteOwnParams
//purpose  : 
//=======================================================================

void IGESSolid_ToolVertexList::WriteOwnParams(const Handle(IGESSolid_VertexList)& ent,
                                              IGESData_IGESWriter& IW) const
{
  Standard_Integer nbitems = ent->NbVertices();
  Standard_Integer i;

  IW.Send(nbitems);
  for (i = 1; i <= nbitems; i ++)
    {
      IW.Send(ent->Vertex(i).X());
      IW.Send(ent->Vertex(i).Y());
      IW.Send(ent->Vertex(i).Z());
    }
}


//=======================================================================
//function : DirChecker
//purpose  : 
//=======================================================================

IGESData_DirChecker IGESSolid_ToolVertexList::DirChecker
  (const Handle(IGESSolid_VertexList)& /* ent */ ) const
{
  IGESData_DirChecker DC(502, 1);

  DC.Structure  (IGESData_DefVoid);
  DC.LineFont   (IGESData_DefVoid);
  DC.LineWeight (IGESData_DefVoid);
  DC.Color      (IGESData_DefAny);

  DC.SubordinateStatusRequired (1);
  DC.HierarchyStatusIgnored ();
  return DC;
}


//=======================================================================
//function : OwnDump
//purpose  : 
//=======================================================================

void IGESSolid_ToolVertexList::OwnDump(const Handle(IGESSolid_VertexList)& ent,
                                       const IGESData_IGESDumper& /* dumper */,
                                       const Handle(Message_Messenger)& S,
                                       const Standard_Integer level) const
{
  S << "IGESSolid_VertexList" << endl;

  S << "Vertices : ";
  IGESData_DumpListXYZL(S ,level,1, ent->NbVertices(),ent->Vertex,ent->Location());
  S << endl;
}
