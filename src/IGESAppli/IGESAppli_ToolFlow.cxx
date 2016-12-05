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

#include <IGESAppli_Flow.hxx>
#include <IGESAppli_ToolFlow.hxx>
#include <IGESData_DirChecker.hxx>
#include <IGESData_Dump.hxx>
#include <IGESData_HArray1OfIGESEntity.hxx>
#include <IGESData_IGESDumper.hxx>
#include <IGESData_IGESEntity.hxx>
#include <IGESData_IGESReaderData.hxx>
#include <IGESData_IGESWriter.hxx>
#include <IGESData_ParamCursor.hxx>
#include <IGESData_ParamReader.hxx>
#include <IGESDraw_ConnectPoint.hxx>
#include <IGESDraw_HArray1OfConnectPoint.hxx>
#include <IGESGraph_HArray1OfTextDisplayTemplate.hxx>
#include <IGESGraph_TextDisplayTemplate.hxx>
#include <Interface_Check.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_HArray1OfHAsciiString.hxx>
#include <Interface_Macros.hxx>
#include <Interface_ShareTool.hxx>
#include <Message_Messenger.hxx>
#include <Standard_DomainError.hxx>
#include <TCollection_HAsciiString.hxx>


void  IGESAppli_ToolFlow::ReadOwnParams
  (const Handle(IGESAppli_Flow)& ent,
   const Handle(IGESData_IGESReaderData)& IR, IGESData_ParamReader& PR) const
{
  //Standard_Boolean st; //szv#4:S4163:12Mar99 not needed
  Standard_Integer tempNbContextFlags;
  Standard_Integer tempTypeOfFlow;
  Standard_Integer tempFunctionFlag;
  Standard_Integer i, nf,nc,nj,nn,nt,np;
  Handle(IGESData_HArray1OfIGESEntity) tempFlowAssocs;
  Handle(IGESDraw_HArray1OfConnectPoint) tempConnectPoints;
  Handle(IGESData_HArray1OfIGESEntity) tempJoins;
  Handle(Interface_HArray1OfHAsciiString) tempFlowNames;
  Handle(IGESGraph_HArray1OfTextDisplayTemplate) tempTextDisplayTemplates;
  Handle(IGESData_HArray1OfIGESEntity) tempContFlowAssocs;

  //szv#4:S4163:12Mar99 `st=` not needed
  if (PR.DefinedElseSkip())
    PR.ReadInteger(PR.Current(), "Number of Context Flags", tempNbContextFlags);
  else
    tempNbContextFlags = 2;

  //szv#4:S4163:12Mar99 moved in if
  if (!PR.ReadInteger(PR.Current(), "Number of Flow Associativities", nf)) nf = 0;
  if (nf > 0)  tempFlowAssocs     =  new IGESData_HArray1OfIGESEntity(1, nf);
  else  PR.AddFail ("Number of Flow Associativities: Not Positive");

  if (!PR.ReadInteger(PR.Current(), "Number of Connect Points", nc)) nc = 0;
  if (nc > 0)  tempConnectPoints  = new IGESDraw_HArray1OfConnectPoint(1, nc);
  else  PR.AddFail("Number of Connect Points: Not Positive");

  if (!PR.ReadInteger(PR.Current(), "Number of Joins", nj)) nj = 0;
  if (nj > 0)  tempJoins          = new IGESData_HArray1OfIGESEntity(1, nj);
  else  PR.AddFail("Number of Joins: Not Positive");

  if (!PR.ReadInteger(PR.Current(), "Number of Flow Names", nn)) nn = 0;
  if (nn > 0)  tempFlowNames      = new Interface_HArray1OfHAsciiString(1, nn);
  else  PR.AddFail("Number of Flow Names: Not Positive");

  if (!PR.ReadInteger(PR.Current(), "Number of Text Displays", nt)) nt = 0;
  if (nt > 0)  tempTextDisplayTemplates =
    new IGESGraph_HArray1OfTextDisplayTemplate(1, nt);
  else PR.AddFail("Number of Text Displays: Not Positive");

  if (!PR.ReadInteger(PR.Current(), "Number of Continuation Flows", np)) np = 0;
  if (np > 0)  tempContFlowAssocs = new IGESData_HArray1OfIGESEntity(1, np);
  else PR.AddFail("Number of Continuation Flows Not Positive");

  if (PR.DefinedElseSkip())
    PR.ReadInteger(PR.Current(), "Type of Flow", tempTypeOfFlow);
  else
    tempTypeOfFlow = 0;

  if (PR.DefinedElseSkip())
    PR.ReadInteger(PR.Current(), "Function Flag", tempFunctionFlag);
  else
    tempFunctionFlag = 0;

  for ( i = 1; i <= nf; i++ ) {
    Handle(IGESData_IGESEntity) tempEntity;
    if (PR.ReadEntity(IR, PR.Current(), "Flow Associativity", tempEntity))
      tempFlowAssocs->SetValue(i, tempEntity);
  }

  for ( i = 1; i <= nc; i++ ) {
    Handle(IGESDraw_ConnectPoint) tempEntity;
    if (PR.ReadEntity(IR, PR.Current(), "Connect Point", STANDARD_TYPE(IGESDraw_ConnectPoint), tempEntity))
      tempConnectPoints->SetValue(i, tempEntity);
  }

  for ( i = 1; i <= nj; i++ ) {
    Handle(IGESData_IGESEntity) tempEntity;
    if (PR.ReadEntity(IR, PR.Current(), "Join", tempEntity))
      tempJoins->SetValue(i, tempEntity);
  }

  for ( i = 1; i <= nn; i++ ) {
    Handle(TCollection_HAsciiString) tempString;
    if (PR.ReadText(PR.Current(), "Flow Name", tempString))
      tempFlowNames->SetValue(i, tempString);
  }

  for ( i = 1; i <= nt; i++ ) {
    Handle(IGESGraph_TextDisplayTemplate) tempEntity;
    if (PR.ReadEntity(IR, PR.Current(), "Text Display Template",
		      STANDARD_TYPE(IGESGraph_TextDisplayTemplate), tempEntity))
      tempTextDisplayTemplates->SetValue(i, tempEntity);
  }

  for ( i = 1; i <= np; i++ ) {
    Handle(IGESData_IGESEntity) tempEntity;
    if (PR.ReadEntity(IR, PR.Current(), "Continuation Flow Associativities", tempEntity))
      tempContFlowAssocs->SetValue(i, tempEntity);
  }

  DirChecker(ent).CheckTypeAndForm(PR.CCheck(),ent);
  ent->Init(tempNbContextFlags, tempTypeOfFlow, tempFunctionFlag,
	    tempFlowAssocs, tempConnectPoints, tempJoins, tempFlowNames,
	    tempTextDisplayTemplates, tempContFlowAssocs);
}

void  IGESAppli_ToolFlow::WriteOwnParams
  (const Handle(IGESAppli_Flow)& ent, IGESData_IGESWriter& IW) const
{
  Standard_Integer i, num;
  IW.Send(ent->NbContextFlags());
  IW.Send(ent->NbFlowAssociativities());
  IW.Send(ent->NbConnectPoints());
  IW.Send(ent->NbJoins());
  IW.Send(ent->NbFlowNames());
  IW.Send(ent->NbTextDisplayTemplates());
  IW.Send(ent->NbContFlowAssociativities());
  IW.Send(ent->TypeOfFlow());
  IW.Send(ent->FunctionFlag());
  for ( num = ent->NbFlowAssociativities(), i = 1; i <= num; i++ )
    IW.Send(ent->FlowAssociativity(i));
  for ( num = ent->NbConnectPoints(), i = 1; i <= num; i++ )
    IW.Send(ent->ConnectPoint(i));
  for ( num = ent->NbJoins(), i = 1; i <= num; i++ )
    IW.Send(ent->Join(i));
  for ( num = ent->NbFlowNames(), i = 1; i <= num; i++ )
    IW.Send(ent->FlowName(i));
  for ( num = ent->NbTextDisplayTemplates(), i = 1; i <= num; i++ )
    IW.Send(ent->TextDisplayTemplate(i));
  for ( num = ent->NbContFlowAssociativities(), i = 1; i <= num; i++ )
    IW.Send(ent->ContFlowAssociativity(i));
}

Standard_Boolean  IGESAppli_ToolFlow::OwnCorrect
  (const Handle(IGESAppli_Flow)& ent) const
{
  return ent->OwnCorrect();    // nbcontextflags = 2
}

IGESData_DirChecker  IGESAppli_ToolFlow::DirChecker
  (const Handle(IGESAppli_Flow)& /* ent */ ) const
{
  IGESData_DirChecker DC(402, 18);
  DC.Structure(IGESData_DefVoid);
  DC.GraphicsIgnored();
  DC.LineFont(IGESData_DefVoid);
  DC.LineWeight(IGESData_DefVoid);
  DC.Color(IGESData_DefVoid);
  DC.BlankStatusIgnored();
  DC.UseFlagRequired(3);
  DC.HierarchyStatusIgnored();
  return DC;
}

void  IGESAppli_ToolFlow::OwnDump
  (const Handle(IGESAppli_Flow)& ent, const IGESData_IGESDumper& dumper,
   const Handle(Message_Messenger)& S, const Standard_Integer level) const
{
  S << "IGESAppli_Flow" << endl;
  S << "Number of Context Flags : " << ent->NbContextFlags() << endl;
  Standard_Integer tf = ent->TypeOfFlow();
  S << "Type of Flow : " << tf;
  if      (tf == 1) S << " (logical)" << endl;
  else if (tf == 2) S << " (physical)" << endl;
  else              S << " (not specified)" << endl;
  tf = ent->FunctionFlag();
  S << "Function Flag : " << tf;
  if      (tf == 1) S << " (electrical signal)" << endl;
  else if (tf == 2) S << " (fluid flow path)" << endl;
  else              S << " (not specified)" << endl;
  S << "Flow Associativities : ";
  IGESData_DumpEntities(S,dumper ,level,1, ent->NbFlowAssociativities(),
			ent->FlowAssociativity);
  S << endl << "Connect Points : ";
  IGESData_DumpEntities(S,dumper ,level,1, ent->NbConnectPoints(),
			ent->ConnectPoint);
  S << endl << "Joins : ";
  IGESData_DumpEntities(S,dumper ,level,1, ent->NbJoins(),ent->Join);
  S << endl << "Flow Names : ";
  IGESData_DumpStrings(S ,level,1, ent->NbFlowNames(),ent->FlowName);
  S << endl << "Text Display Templates : ";
  IGESData_DumpEntities(S,dumper ,level,1, ent->NbTextDisplayTemplates(),
			ent->TextDisplayTemplate);
  S << endl << "Continuation Flow Associativities : ";
  IGESData_DumpEntities(S,dumper ,level,1, ent->NbContFlowAssociativities(),
			ent->ContFlowAssociativity);
  S << endl;
}

