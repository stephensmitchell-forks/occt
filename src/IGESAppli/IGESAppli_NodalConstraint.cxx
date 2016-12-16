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
#include <IGESDefs_TabularData.hxx>
#include <Standard_DimensionMismatch.hxx>
#include <Standard_OutOfRange.hxx>
#include <Interface_EntityIterator.hxx>
#include <IGESFile_Reader.hxx>
#include <IGESData_IGESWriter.hxx>
#include <Message_Messenger.hxx>
#include <IGESData_DirChecker.hxx>
#include <IGESData_IGESDumper.hxx>
#include <IGESData_Dump.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESAppli_NodalConstraint,IGESData_IGESEntity)

Standard_Integer IGESAppli_NodalConstraint::NbCases () const
{
  return myTabularDataProps->Length();
}

const Handle(IGESDefs_TabularData) & IGESAppli_NodalConstraint::TabularData (const Standard_Integer Index) const
{
  return myTabularDataProps->Value(Index);
}

void IGESAppli_NodalConstraint::OwnRead (IGESFile_Reader &theReader)
{
  Standard_Integer num = 0;
  theReader.ReadInteger(num,"Number of cases");
  if (num > 0) myTabularDataProps = new IGESDefs_HArray1OfTabularData(1, num);
  else theReader.AddFail("Number of cases: Not Positive");

  theReader.ReadInteger(myType,"Type of Constraint");
  theReader.ReadPointer(myNode,"Node");

  for ( Standard_Integer i = 1; i <= num; i++ )
    theReader.ReadPointer(myTabularDataProps->ChangeValue(i),"Tabular Data Property");
}

void IGESAppli_NodalConstraint::OwnWrite (IGESData_IGESWriter &IW) const
{
  const Standard_Integer num = myTabularDataProps->Length();
  IW.Send(num);
  IW.Send(myType);
  IW.Send(myNode);
  for ( Standard_Integer i = 1; i <= num; i++ )
    IW.Send(myTabularDataProps->Value(i));
}

void IGESAppli_NodalConstraint::OwnShared(Interface_EntityIterator &theIter) const
{
  theIter.GetOneItem(myNode);
  const Standard_Integer num = myTabularDataProps->Length();
  for ( Standard_Integer i = 1; i <= num; i++ )
    theIter.GetOneItem(myTabularDataProps->Value(i));
}

IGESData_DirChecker IGESAppli_NodalConstraint::DirChecker () const
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

void IGESAppli_NodalConstraint::OwnCheck (const Interface_ShareTool &, const Handle(Interface_Check) &theCheck) const
{
  if ((myType != 1) && (myType != 2))
    theCheck->AddFail("Type of Constraint != 1,2");
}

void IGESAppli_NodalConstraint::OwnDump (const IGESData_IGESDumper &dumper, const Handle(Message_Messenger) &S, const Standard_Integer level) const
{
  Standard_Integer sublevel = (level > 4) ? 1 : 0;
  S << "IGESAppli_NodalConstraint" << endl;
  S << "Type of Constraint : " << myType << endl;
  S << "Node : ";
  dumper.Dump(myNode,S,sublevel);
  S << endl;
  S << "Tabular Data Properties : ";
  IGESData_DumpEntities(S,dumper,level,1,NbCases(),TabularData);
  S << endl;
}
