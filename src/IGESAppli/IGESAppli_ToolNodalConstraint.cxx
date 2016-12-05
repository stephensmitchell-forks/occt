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

#include <IGESAppli_NodalConstraint.hxx>
#include <IGESAppli_Node.hxx>
#include <IGESAppli_ToolNodalConstraint.hxx>
#include <IGESData_DirChecker.hxx>
#include <IGESData_Dump.hxx>
#include <IGESData_IGESDumper.hxx>
#include <IGESData_IGESReaderData.hxx>
#include <IGESData_IGESWriter.hxx>
#include <IGESData_ParamCursor.hxx>
#include <IGESData_ParamReader.hxx>
#include <IGESDefs_HArray1OfTabularData.hxx>
#include <IGESDefs_TabularData.hxx>
#include <Interface_Check.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_Macros.hxx>
#include <Interface_ShareTool.hxx>
#include <Message_Messenger.hxx>
#include <Standard_DomainError.hxx>


void  IGESAppli_ToolNodalConstraint::ReadOwnParams
  (const Handle(IGESAppli_NodalConstraint)& ent,
   const Handle(IGESData_IGESReaderData)& IR, IGESData_ParamReader& PR)  const
{
  //Standard_Boolean st; //szv#4:S4163:12Mar99 not needed
  Standard_Integer num, i;
  Standard_Integer tempType;
  Handle(IGESAppli_Node) tempNode;
  Handle(IGESDefs_HArray1OfTabularData) tempTabularDataProps;
  if (!PR.ReadInteger(PR.Current(), "Number of cases", num)) num = 0;
  if (num > 0)  tempTabularDataProps = new IGESDefs_HArray1OfTabularData(1, num);
  else  PR.AddFail("Number of cases: Not Positive");
  //szv#4:S4163:12Mar99 `st=` not needed
  PR.ReadInteger(PR.Current(), "Type of Constraint", tempType);
  PR.ReadEntity(IR, PR.Current(), "Node", STANDARD_TYPE(IGESAppli_Node), tempNode);

  if (!tempTabularDataProps.IsNull())
    for ( i = 1; i <= num; i++ )
      {
	Handle(IGESDefs_TabularData) tempEntity;
	//szv#4:S4163:12Mar99 moved in if
	if (PR.ReadEntity(IR, PR.Current(), "Tabular Data Property",
			  STANDARD_TYPE(IGESDefs_TabularData), tempEntity))
	  tempTabularDataProps->SetValue(i, tempEntity);
      }
  DirChecker(ent).CheckTypeAndForm(PR.CCheck(),ent);
  ent->Init (tempType, tempNode, tempTabularDataProps);
}

void  IGESAppli_ToolNodalConstraint::WriteOwnParams
  (const Handle(IGESAppli_NodalConstraint)& ent, IGESData_IGESWriter& IW) const
{
  Standard_Integer i, num;
  IW.Send(ent->NbCases());
  IW.Send(ent->Type());
  IW.Send(ent->NodeEntity());
  for ( num = ent->NbCases(), i = 1; i <= num; i++ )
    IW.Send(ent->TabularData(i));
}

IGESData_DirChecker  IGESAppli_ToolNodalConstraint::DirChecker
  (const Handle(IGESAppli_NodalConstraint)& /* ent */ ) const
{
  IGESData_DirChecker DC(418, 0);
  DC.Structure(IGESData_DefVoid);
  DC.GraphicsIgnored();
  DC.LineFont(IGESData_DefVoid);
  DC.LineWeight(IGESData_DefVoid);
  DC.Color(IGESData_DefVoid);
  DC.HierarchyStatusIgnored();
  return DC;
}

void  IGESAppli_ToolNodalConstraint::OwnDump
  (const Handle(IGESAppli_NodalConstraint)& ent, const IGESData_IGESDumper& dumper,
   const Handle(Message_Messenger)& S, const Standard_Integer level) const
{
  Standard_Integer sublevel = (level > 4) ? 1 : 0;
  S << "IGESAppli_NodalConstraint" << endl;
  S << "Type of Constraint : " << ent->Type() << endl;
  S << "Node : ";
  dumper.Dump(ent->NodeEntity(),S, sublevel);
  S << endl;
  S << "Tabular Data Properties : ";
  IGESData_DumpEntities(S,dumper ,level,1, ent->NbCases(),ent->TabularData);
  S << endl;
}
