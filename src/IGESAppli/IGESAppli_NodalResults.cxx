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

#include <IGESAppli_NodalResults.hxx>
#include <IGESAppli_Node.hxx>
#include <IGESDimen_GeneralNote.hxx>
#include <Standard_DimensionMismatch.hxx>
#include <Standard_OutOfRange.hxx>
#include <Interface_EntityIterator.hxx>
#include <TColStd_HArray1OfReal.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESAppli_NodalResults,IGESData_IGESEntity)

// Data : Col -> // Nodes.  Row : Data per Node

void IGESAppli_NodalResults::Init
  (const Handle(IGESDimen_GeneralNote)&    aNote,
   const Standard_Integer aNumber, const Standard_Real aTime,
   const Handle(TColStd_HArray1OfInteger)& allNodeIdentifiers,
   const Handle(IGESAppli_HArray1OfNode)&  allNodes,
   const Handle(TColStd_HArray2OfReal)&    allData)
{
  if (allNodes->Lower()   != 1 || allNodeIdentifiers->Lower() != 1 ||
      allNodes->Length()  != allNodeIdentifiers->Length() ||
      allData->LowerCol() != 1 || allData->LowerRow() != 1 ||
      allNodes->Length()  != allData->UpperRow() )
    Standard_DimensionMismatch::Raise("IGESAppli_NodalResults : Init");
  theNote            = aNote;
  theSubCaseNum      = aNumber;
  theTime            = aTime;
  theNodeIdentifiers = allNodeIdentifiers;
  theNodes           = allNodes;
  theData            = allData;
  InitTypeAndForm(146,FormNumber());
// FormNumber -> Type of the Results
}

void IGESAppli_NodalResults::SetFormNumber (const Standard_Integer form)
{
  if (form < 0 || form > 34) Standard_OutOfRange::Raise
    ("IGESAppli_NodalResults : SetFormNumber");
  InitTypeAndForm(146,form);
}

const Handle(IGESAppli_Node) & IGESAppli_NodalResults::Node (const Standard_Integer Index) const
{
  return theNodes->Value(Index);
}

Standard_Integer IGESAppli_NodalResults::NbNodes () const
{
  return theNodes->Length();
}

Standard_Integer IGESAppli_NodalResults::NbData () const
{
  return theData->RowLength();
}

Standard_Integer IGESAppli_NodalResults::NodeIdentifier (const Standard_Integer Index) const
{
  return theNodeIdentifiers->Value(Index);
}

Standard_Real IGESAppli_NodalResults::Data (const Standard_Integer NodeNum, const Standard_Integer DataNum) const
{
  return theData->Value(NodeNum,DataNum);
}

void IGESAppli_NodalResults::OwnShared(Interface_EntityIterator &theIter) const
{
  theIter.GetOneItem(Note());
  const Standard_Integer nbnodes = NbNodes();
  for (Standard_Integer i = 1; i <= nbnodes; i++)
    theIter.GetOneItem(Node(i));
}

void IGESAppli_NodalResults::OwnCheck (const Interface_ShareTool &, const Handle(Interface_Check) &theCheck) const
{
  const Standard_Integer FormNum = FormNumber();
  const Standard_Integer nv = NbData();
  Standard_Boolean OK = Standard_True;
  switch (FormNum) {
    case  0 : if (nv <  0) OK = Standard_False;  break;
    case  1 : if (nv != 1) OK = Standard_False;  break;
    case  2 : if (nv != 1) OK = Standard_False;  break;
    case  3 : if (nv != 3) OK = Standard_False;  break;
    case  4 : if (nv != 6) OK = Standard_False;  break;
    case  5 : if (nv != 3) OK = Standard_False;  break;
    case  6 : if (nv != 3) OK = Standard_False;  break;
    case  7 : if (nv != 3) OK = Standard_False;  break;
    case  8 : if (nv != 3) OK = Standard_False;  break;
    case  9 : if (nv != 3) OK = Standard_False;  break;
    case 10 : if (nv != 1) OK = Standard_False;  break;
    case 11 : if (nv != 1) OK = Standard_False;  break;
    case 12 : if (nv != 3) OK = Standard_False;  break;
    case 13 : if (nv != 1) OK = Standard_False;  break;
    case 14 : if (nv != 1) OK = Standard_False;  break;
    case 15 : if (nv != 3) OK = Standard_False;  break;
    case 16 : if (nv != 1) OK = Standard_False;  break;
    case 17 : if (nv != 3) OK = Standard_False;  break;
    case 18 : if (nv != 3) OK = Standard_False;  break;
    case 19 : if (nv != 3) OK = Standard_False;  break;
    case 20 : if (nv != 3) OK = Standard_False;  break;
    case 21 : if (nv != 3) OK = Standard_False;  break;
    case 22 : if (nv != 3) OK = Standard_False;  break;
    case 23 : if (nv != 6) OK = Standard_False;  break;
    case 24 : if (nv != 6) OK = Standard_False;  break;
    case 25 : if (nv != 6) OK = Standard_False;  break;
    case 26 : if (nv != 6) OK = Standard_False;  break;
    case 27 : if (nv != 6) OK = Standard_False;  break;
    case 28 : if (nv != 6) OK = Standard_False;  break;
    case 29 : if (nv != 9) OK = Standard_False;  break;
    case 30 : if (nv != 9) OK = Standard_False;  break;
    case 31 : if (nv != 9) OK = Standard_False;  break;
    case 32 : if (nv != 9) OK = Standard_False;  break;
    case 33 : if (nv != 9) OK = Standard_False;  break;
    case 34 : if (nv != 9) OK = Standard_False;  break;
    default : theCheck->AddFail("Incorrect Form Number");    break;
  }
  if (!OK) theCheck->AddFail("Incorrect count of real values in array V for FEM node");
}
