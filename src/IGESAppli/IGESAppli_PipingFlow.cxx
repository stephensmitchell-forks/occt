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

#include <IGESAppli_PipingFlow.hxx>
#include <IGESData_IGESEntity.hxx>
#include <IGESDraw_ConnectPoint.hxx>
#include <IGESGraph_TextDisplayTemplate.hxx>
#include <Standard_DimensionMismatch.hxx>
#include <Standard_OutOfRange.hxx>
#include <Interface_EntityIterator.hxx>
#include <TCollection_HAsciiString.hxx>
#include <IGESFile_Reader.hxx>
#include <IGESData_IGESWriter.hxx>
#include <Message_Messenger.hxx>
#include <IGESData_DirChecker.hxx>
#include <IGESData_IGESDumper.hxx>
#include <IGESData_Dump.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESAppli_PipingFlow,IGESData_IGESEntity)

Standard_Integer IGESAppli_PipingFlow::NbFlowAssociativities () const
{
  return myFlowAssociativities->Length();
}

Standard_Integer IGESAppli_PipingFlow::NbConnectPoints () const
{
  return myConnectPoints->Length();
}

Standard_Integer IGESAppli_PipingFlow::NbJoins () const
{
  return myJoins->Length();
}

Standard_Integer IGESAppli_PipingFlow::NbFlowNames () const
{
  return myFlowNames->Length();
}

Standard_Integer IGESAppli_PipingFlow::NbTextDisplayTemplates () const
{
  return myTextDisplayTemplates->Length();
}

Standard_Integer IGESAppli_PipingFlow::NbContFlowAssociativities () const
{
  return myContFlowAssociativities->Length();
}

const Handle(IGESData_IGESEntity) & IGESAppli_PipingFlow::FlowAssociativity (const Standard_Integer Index) const
{
  return myFlowAssociativities->Value(Index);
}

const Handle(IGESDraw_ConnectPoint) & IGESAppli_PipingFlow::ConnectPoint (const Standard_Integer Index) const
{
  return myConnectPoints->Value(Index);
}

const Handle(IGESData_IGESEntity) & IGESAppli_PipingFlow::Join (const Standard_Integer Index) const
{
  return myJoins->Value(Index);
}

const Handle(TCollection_HAsciiString) & IGESAppli_PipingFlow::FlowName (const Standard_Integer Index) const
{
  return myFlowNames->Value(Index);
}

const Handle(IGESGraph_TextDisplayTemplate) & IGESAppli_PipingFlow::TextDisplayTemplate (const Standard_Integer Index) const
{
  return myTextDisplayTemplates->Value(Index);
}

const Handle(IGESData_IGESEntity) & IGESAppli_PipingFlow::ContFlowAssociativity (const Standard_Integer Index) const
{
  return myContFlowAssociativities->Value(Index);
}

void IGESAppli_PipingFlow::OwnRead (IGESFile_Reader &PR)
{
  Standard_Integer i, num;

  Standard_Integer aNbContextFlags = 1;
  if (PR.ReadInteger(aNbContextFlags,"Number of Context Flags") == IGESFile_Reader::ParamError || aNbContextFlags != 1)
    PR.AddFail("Number of Context Flags != 1");

  num = 0;
  PR.ReadInteger(num,"Number of Flow Associativities");
  if (num > 0) myFlowAssociativities = new IGESData_HArray1OfIGESEntity(1, num);
  else PR.AddFail("Number of Flow Associativities: Not Positive");

  num = 0;
  PR.ReadInteger(num,"Number of Connect Points");
  if (num > 0) myConnectPoints = new IGESDraw_HArray1OfConnectPoint(1, num);
  else PR.AddFail("Number of Connect Points: Not Positive");

  num = 0;
  PR.ReadInteger(num,"Number of Joins");
  if (num > 0) myJoins = new IGESData_HArray1OfIGESEntity(1, num);
  else PR.AddFail("Number of Joins: Not Positive");

  num = 0;
  PR.ReadInteger(num,"Number of Flow Names");
  if (num > 0) myFlowNames = new Interface_HArray1OfHAsciiString(1, num);
  else PR.AddFail("Number of Flow Names: Not Positive");

  num = 0;
  PR.ReadInteger(num,"Number of Text Displays");
  if (num > 0) myTextDisplayTemplates = new IGESGraph_HArray1OfTextDisplayTemplate(1, num);
  else PR.AddFail("Number of Text Displays: Not Positive");

  num = 0;
  PR.ReadInteger(num,"Number of Continuation Flows");
  if (num > 0) myContFlowAssociativities = new IGESData_HArray1OfIGESEntity(1, num);
  else PR.AddFail("Number of Continuation Flows: Not Positive");

  myTypeOfFlow = 0;
  PR.ReadInteger(myTypeOfFlow,"Type of Flow");

  if (!myFlowAssociativities.IsNull())
    for ( num = myFlowAssociativities->Length(), i = 1; i <= num; i++ )
      PR.ReadPointer(myFlowAssociativities->ChangeValue(i),"Flow Associativity");

  if (!myConnectPoints.IsNull())
    for ( num = myConnectPoints->Length(), i = 1; i <= num; i++ )
      PR.ReadPointer(myConnectPoints->ChangeValue(i),"Connect Point");

  if (!myJoins.IsNull())
    for ( num = myJoins->Length(), i = 1; i <= num; i++ )
      PR.ReadPointer(myJoins->ChangeValue(i),"Join");

  if (!myFlowNames.IsNull())
    for ( num = myFlowNames->Length(), i = 1; i <= num; i++ )
      PR.ReadText(myFlowNames->ChangeValue(i),"Flow Name");

  if (!myTextDisplayTemplates.IsNull())
    for ( num = myTextDisplayTemplates->Length(), i = 1; i <= num; i++ )
      PR.ReadPointer(myTextDisplayTemplates->ChangeValue(i),"Text Display Template");

  if (!myContFlowAssociativities.IsNull())
    for ( num = myContFlowAssociativities->Length(), i = 1; i <= num; i++ )
      PR.ReadPointer(myContFlowAssociativities->ChangeValue(i),"Continuation Flow Assocs");
}

void IGESAppli_PipingFlow::OwnWrite (IGESData_IGESWriter &IW) const
{
  Standard_Integer i, num;
  IW.Send(1);
  IW.Send(NbFlowAssociativities());
  IW.Send(NbConnectPoints());
  IW.Send(NbJoins());
  IW.Send(NbFlowNames());
  IW.Send(NbTextDisplayTemplates());
  IW.Send(NbContFlowAssociativities());
  IW.Send(myTypeOfFlow);
  for ( num = NbFlowAssociativities(), i = 1; i <= num; i++ )
    IW.Send(FlowAssociativity(i));
  for ( num = NbConnectPoints(), i = 1; i <= num; i++ )
    IW.Send(ConnectPoint(i));
  for ( num = NbJoins(), i = 1; i <= num; i++ )
    IW.Send(Join(i));
  for ( num = NbFlowNames(), i = 1; i <= num; i++ )
    IW.Send(FlowName(i));
  for ( num = NbTextDisplayTemplates(), i = 1; i <= num; i++ )
    IW.Send(TextDisplayTemplate(i));
  for ( num = NbContFlowAssociativities(), i = 1; i <= num; i++ )
    IW.Send(ContFlowAssociativity(i));
}

void IGESAppli_PipingFlow::OwnShared(Interface_EntityIterator &theIter) const
{
  Standard_Integer i, num;
  for ( num = NbFlowAssociativities(), i = 1; i <= num; i++ )
    theIter.GetOneItem(FlowAssociativity(i));
  for ( num = NbConnectPoints(), i = 1; i <= num; i++ )
    theIter.GetOneItem(ConnectPoint(i));
  for ( num = NbJoins(), i = 1; i <= num; i++ )
    theIter.GetOneItem(Join(i));
  for ( num = NbTextDisplayTemplates(), i = 1; i <= num; i++ )
    theIter.GetOneItem(TextDisplayTemplate(i));
  for ( num = NbContFlowAssociativities(), i = 1; i <= num; i++ )
    theIter.GetOneItem(ContFlowAssociativity(i));
}

IGESData_DirChecker IGESAppli_PipingFlow::DirChecker () const
{
  IGESData_DirChecker DC(402, 20);
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

void IGESAppli_PipingFlow::OwnCheck (const Interface_ShareTool &, const Handle(Interface_Check) &theCheck) const
{
  if ((myTypeOfFlow < 0) || (myTypeOfFlow > 2))
    theCheck->AddFail("Type of Flow != 0,1,2");
}

void IGESAppli_PipingFlow::OwnDump (const IGESData_IGESDumper &dumper, const Handle(Message_Messenger) &S, const Standard_Integer level) const
{
  S << "IGESAppli_PipingFlow" << endl;
  S << "Number of Context Flags : 1" << endl;
  S << "Type of Flow : " << TypeOfFlow() << endl;
  S << "Flow Associativities : ";
  IGESData_DumpEntities(S,dumper,level,1,NbFlowAssociativities(),FlowAssociativity);
  S << "Connect Points : ";
  IGESData_DumpEntities(S,dumper,level,1,NbConnectPoints(),ConnectPoint);
  S << "Joins : ";
  IGESData_DumpEntities(S,dumper,level,1,NbJoins(),Join);
  S << "Flow Names : ";
  IGESData_DumpStrings(S,level,1,NbFlowNames(),FlowName);
  S << "Text Display Templates : ";
  IGESData_DumpEntities(S,dumper,level,1,NbTextDisplayTemplates(),TextDisplayTemplate);
  S << "Continuation Flow Associativities : ";
  IGESData_DumpEntities(S,dumper,level,1,NbContFlowAssociativities(),ContFlowAssociativity);
  S << endl;
}
