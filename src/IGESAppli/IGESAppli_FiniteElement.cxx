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

#include <IGESAppli.hxx>
#include <IGESAppli_FiniteElement.hxx>
#include <IGESAppli_Node.hxx>
#include <Standard_DimensionMismatch.hxx>
#include <Standard_OutOfRange.hxx>
#include <Interface_EntityIterator.hxx>
#include <TCollection_HAsciiString.hxx>
#include <IGESFile_Reader.hxx>
#include <IGESData_IGESWriter.hxx>
#include <IGESData_DirChecker.hxx>
#include <IGESData_IGESDumper.hxx>
#include <IGESData_Dump.hxx>
#include <Message_Messenger.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESAppli_FiniteElement,IGESData_IGESEntity)

Standard_Integer IGESAppli_FiniteElement::NbNodes () const
{
  return myNodes->Length();
}

const Handle(IGESAppli_Node) & IGESAppli_FiniteElement::Node (const Standard_Integer Index) const
{
  return myNodes->Value(Index);
}

void IGESAppli_FiniteElement::OwnRead (IGESFile_Reader &theReader)
{
  theReader.ReadInteger(myTopology,"Topology type");
  Standard_Integer nb;
  if (theReader.ReadInteger(nb,"No. of nodes defining element") && nb > 0)
  {
    myNodes = new IGESAppli_HArray1OfNode(1,nb);
    for (Standard_Integer i = 1; i <= nb; i ++)
      theReader.ReadPointer(myNodes->ChangeValue(i),"Node defining element");
  }
  theReader.ReadText(myName,"Element type name");
}

void IGESAppli_FiniteElement::OwnWrite (IGESData_IGESWriter &IW) const
{
  const Standard_Integer nb = myNodes->Length();
  IW.Send(myTopology);
  IW.Send(nb);
  for (Standard_Integer i = 1; i <= nb; i++)
    IW.Send(myNodes->Value(i));
  IW.Send(myName);
}

IGESData_DirChecker IGESAppli_FiniteElement::DirChecker () const
{
  IGESData_DirChecker DC(136,0);  //Form no = 0 & Type = 136
  DC.Structure(IGESData_DefVoid);
  DC.LineFont(IGESData_DefAny);
  DC.LineWeight(IGESData_DefVoid);
  DC.Color(IGESData_DefAny);
  DC.BlankStatusIgnored();
  DC.SubordinateStatusIgnored();
  DC.UseFlagIgnored();
  DC.HierarchyStatusIgnored();
  return DC;
}

void IGESAppli_FiniteElement::OwnShared(Interface_EntityIterator &theIter) const
{
  const Standard_Integer nb = myNodes->Length();
  for (Standard_Integer i = 1; i <= nb ; i++)
    theIter.GetOneItem(myNodes->Value(i));
}

void IGESAppli_FiniteElement::OwnDump (const IGESData_IGESDumper &dumper, const Handle(Message_Messenger) &S, const Standard_Integer level) const
{
  S << "IGESAppli_FiniteElement" << endl;
  S << "Topology type : " << myTopology << endl;
  S << "Nodes : ";
  IGESData_DumpEntities(S,dumper,level,1,NbNodes(),Node);
  S << endl << "Element Name : " << myName << endl;
}
