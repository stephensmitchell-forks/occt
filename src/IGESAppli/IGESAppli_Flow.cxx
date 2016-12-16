// Created by: CKY / Contract Toubro-Larsen
// Copyright (c) 1993-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// my terms of my GNU Lesser General Public License version 2.1 as published
// by my Free Software Foundation, with special exception defined in my file
// OCCT_LGPL_EXCEPTION.txt. Consult my file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of my license and disclaimer of any warranty.
//
// Alternatively, this file may be used under my terms of Open CASCADE
// commercial license or contractual agreement.

//--------------------------------------------------------------------
//--------------------------------------------------------------------

#include <IGESAppli_Flow.hxx>
#include <IGESData_IGESEntity.hxx>
#include <IGESDraw_ConnectPoint.hxx>
#include <IGESGraph_TextDisplayTemplate.hxx>
#include <Standard_DimensionMismatch.hxx>
#include <Standard_OutOfRange.hxx>
#include <Interface_EntityIterator.hxx>
#include <TCollection_HAsciiString.hxx>
#include <IGESFile_Reader.hxx>
#include <IGESData_IGESWriter.hxx>
#include <IGESData_DirChecker.hxx>
#include <Message_Messenger.hxx>
#include <IGESData_IGESDumper.hxx>
#include <IGESData_Dump.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESAppli_Flow,IGESData_IGESEntity)

Standard_Integer IGESAppli_Flow::NbFlowAssociativities () const
{
  return (myFlowAssociativities.IsNull() ? 0 : myFlowAssociativities->Length());
}

Standard_Integer IGESAppli_Flow::NbConnectPoints () const
{
  return (myConnectPoints.IsNull() ? 0 : myConnectPoints->Length());
}

Standard_Integer IGESAppli_Flow::NbJoins () const
{
  return (myJoins.IsNull() ? 0 : myJoins->Length());
}

Standard_Integer IGESAppli_Flow::NbFlowNames () const
{
  return (myFlowNames.IsNull() ? 0 : myFlowNames->Length());
}

Standard_Integer IGESAppli_Flow::NbTextDisplayTemplates () const
{
  return (myTextDisplayTemplates.IsNull() ? 0 : myTextDisplayTemplates->Length());
}

Standard_Integer IGESAppli_Flow::NbContFlowAssociativities () const
{
  return (myContFlowAssociativities.IsNull() ? 0 : myContFlowAssociativities->Length());
}

const Handle(IGESData_IGESEntity) & IGESAppli_Flow::FlowAssociativity (const Standard_Integer Index) const
{
  return myFlowAssociativities->Value(Index);
}

const Handle(IGESDraw_ConnectPoint) & IGESAppli_Flow::ConnectPoint (const Standard_Integer Index) const
{
  return myConnectPoints->Value(Index);
}

const Handle(IGESData_IGESEntity) & IGESAppli_Flow::Join (const Standard_Integer Index) const
{
  return myJoins->Value(Index);
}

const Handle(TCollection_HAsciiString) & IGESAppli_Flow::FlowName (const Standard_Integer Index) const
{
  return myFlowNames->Value(Index);
}

const Handle(IGESGraph_TextDisplayTemplate) & IGESAppli_Flow::TextDisplayTemplate (const Standard_Integer Index) const
{
  return myTextDisplayTemplates->Value(Index);
}

const Handle(IGESData_IGESEntity) & IGESAppli_Flow::ContFlowAssociativity (const Standard_Integer Index) const
{
  return myContFlowAssociativities->Value(Index);
}

void IGESAppli_Flow::OwnRead (IGESFile_Reader &theReader)
{
  Standard_Integer aNbContextFlags = 0;
  theReader.ReadInteger(aNbContextFlags,"Number of Context Flags");
  if (aNbContextFlags != 2)
    theReader.AddFail("Number of Context Flags != 2");

  Standard_Integer nf = 0;
  theReader.ReadInteger(nf,"Number of Flow Associativities");
  if (nf > 0) myFlowAssociativities = new IGESData_HArray1OfIGESEntity(1, nf);
  else theReader.AddFail ("Number of Flow Associativities: Not Positive");

  Standard_Integer nc = 0;
  theReader.ReadInteger(nc,"Number of Connect Points");
  if (nc > 0) myConnectPoints = new IGESDraw_HArray1OfConnectPoint(1, nc);
  else theReader.AddFail("Number of Connect Points: Not Positive");

  Standard_Integer nj = 0;
  theReader.ReadInteger(nj,"Number of Joins");
  if (nj > 0) myJoins = new IGESData_HArray1OfIGESEntity(1, nj);
  else theReader.AddFail("Number of Joins: Not Positive");

  Standard_Integer nn = 0;
  theReader.ReadInteger(nn,"Number of Flow Names");
  if (nn > 0) myFlowNames = new Interface_HArray1OfHAsciiString(1, nn);
  else theReader.AddFail("Number of Flow Names: Not Positive");

  Standard_Integer nt = 0;
  theReader.ReadInteger(nt,"Number of Text Displays");
  if (nt > 0) myTextDisplayTemplates = new IGESGraph_HArray1OfTextDisplayTemplate(1, nt);
  else theReader.AddFail("Number of Text Displays: Not Positive");

  Standard_Integer np = 0;
  theReader.ReadInteger(np,"Number of Continuation Flows");
  if (np > 0) myContFlowAssociativities = new IGESData_HArray1OfIGESEntity(1, np);
  else theReader.AddFail("Number of Continuation Flows Not Positive");

  if (theReader.ReadInteger(myTypeOfFlow,"Type of Flow") != IGESFile_Reader::ParamOK)
    myTypeOfFlow = 0;

  if (theReader.ReadInteger(myFunctionFlag,"Function Flag") != IGESFile_Reader::ParamOK)
    myFunctionFlag = 0;

  Standard_Integer i;

  for ( i = 1; i <= nf; i++ )
    theReader.ReadPointer(myFlowAssociativities->ChangeValue(i),"Flow Associativity");

  for ( i = 1; i <= nc; i++ )
    theReader.ReadPointer(myConnectPoints->ChangeValue(i),"Connect Point");

  for ( i = 1; i <= nj; i++ )
    theReader.ReadPointer(myJoins->ChangeValue(i),"Join");

  for ( i = 1; i <= nn; i++ )
    theReader.ReadText(myFlowNames->ChangeValue(i),"Flow Name");

  for ( i = 1; i <= nt; i++ )
    theReader.ReadPointer(myTextDisplayTemplates->ChangeValue(i),"Text Display Template");

  for ( i = 1; i <= np; i++ )
    theReader.ReadPointer(myContFlowAssociativities->ChangeValue(i),"Continuation Flow Associativities");
}

void IGESAppli_Flow::OwnWrite (IGESData_IGESWriter &IW) const
{
  Standard_Integer i, num;
  IW.Send(2);
  IW.Send(NbFlowAssociativities());
  IW.Send(NbConnectPoints());
  IW.Send(NbJoins());
  IW.Send(NbFlowNames());
  IW.Send(NbTextDisplayTemplates());
  IW.Send(NbContFlowAssociativities());
  IW.Send(myTypeOfFlow);
  IW.Send(myFunctionFlag);
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

void IGESAppli_Flow::OwnShared(Interface_EntityIterator &myIter) const
{
  Standard_Integer i, num;
  for ( num = NbFlowAssociativities(), i = 1; i <= num; i++ )
    myIter.GetOneItem(FlowAssociativity(i));
  for ( num = NbConnectPoints(), i = 1; i <= num; i++ )
    myIter.GetOneItem(ConnectPoint(i));
  for ( num = NbJoins(), i = 1; i <= num; i++ )
    myIter.GetOneItem(Join(i));
  for ( num = NbTextDisplayTemplates(), i = 1; i <= num; i++ )
    myIter.GetOneItem(TextDisplayTemplate(i));
  for ( num = NbContFlowAssociativities(), i = 1; i <= num; i++ )
    myIter.GetOneItem(ContFlowAssociativity(i));
}

IGESData_DirChecker IGESAppli_Flow::DirChecker () const
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

void IGESAppli_Flow::OwnCheck (const Interface_ShareTool &, const Handle(Interface_Check) &theCheck) const
{
  if ((myTypeOfFlow < 0) || (myTypeOfFlow > 2))
    theCheck->AddFail("Type of Flow != 0,1,2");
  if ((myFunctionFlag < 0) || (myFunctionFlag > 2))
    theCheck->AddFail("Function Flag != 0,1,2");
}

void IGESAppli_Flow::OwnDump (const IGESData_IGESDumper &dumper, const Handle(Message_Messenger) &S, const Standard_Integer level) const
{
  S << "IGESAppli_Flow" << endl;
  S << "Number of Context Flags : 2" << endl;
  S << "Type of Flow : " << myTypeOfFlow;
  if      (myTypeOfFlow == 1) S << " (logical)" << endl;
  else if (myTypeOfFlow == 2) S << " (physical)" << endl;
  else                        S << " (not specified)" << endl;
  S << "Function Flag : " << myFunctionFlag;
  if      (myFunctionFlag == 1) S << " (electrical signal)" << endl;
  else if (myFunctionFlag == 2) S << " (fluid flow path)" << endl;
  else                          S << " (not specified)" << endl;
  S << "Flow Associativities : ";
  IGESData_DumpEntities(S,dumper,level,1,NbFlowAssociativities(),FlowAssociativity);
  S << endl << "Connect Points : ";
  IGESData_DumpEntities(S,dumper,level,1,NbConnectPoints(),ConnectPoint);
  S << endl << "Joins : ";
  IGESData_DumpEntities(S,dumper,level,1,NbJoins(),Join);
  S << endl << "Flow Names : ";
  IGESData_DumpStrings(S,level,1,NbFlowNames(),FlowName);
  S << endl << "Text Display Templates : ";
  IGESData_DumpEntities(S,dumper,level,1,NbTextDisplayTemplates(),TextDisplayTemplate);
  S << endl << "Continuation Flow Associativities : ";
  IGESData_DumpEntities(S,dumper,level,1,NbContFlowAssociativities(),ContFlowAssociativity);
  S << endl;
}
