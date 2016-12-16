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
#include <IGESFile_Reader.hxx>
#include <IGESData_IGESWriter.hxx>
#include <Message_Messenger.hxx>
#include <IGESData_DirChecker.hxx>
#include <IGESData_IGESDumper.hxx>
#include <IGESData_Dump.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESAppli_NodalResults,IGESData_IGESEntity)

const Handle(IGESAppli_Node) & IGESAppli_NodalResults::Node (const Standard_Integer Index) const
{
  return myNodes->Value(Index);
}

Standard_Integer IGESAppli_NodalResults::NbNodes () const
{
  return myNodes->Length();
}

Standard_Integer IGESAppli_NodalResults::NbData () const
{
  return myData->RowLength();
}

Standard_Integer IGESAppli_NodalResults::NodeIdentifier (const Standard_Integer Index) const
{
  return myNodeIdentifiers->Value(Index);
}

Standard_Real IGESAppli_NodalResults::Data (const Standard_Integer NodeNum, const Standard_Integer DataNum) const
{
  return myData->Value(NodeNum,DataNum);
}

void IGESAppli_NodalResults::OwnRead (IGESFile_Reader &theReader)
{
  theReader.ReadPointer(myNote,"General Note describing the analysis case");
  theReader.ReadInteger(mySubCaseNum,"Subcase number");
  theReader.ReadReal(myTime,"Analysis time used");
  Standard_Integer nbval = 0;
  theReader.ReadInteger(nbval,"No. of values");
  Standard_Integer nbnodes = 0;
  theReader.ReadInteger(nbnodes,"No. of nodes");
  if (nbnodes > 0 && nbval > 0)
  {
    myData  = new TColStd_HArray2OfReal(1,nbnodes,1,nbval);
    myNodes = new IGESAppli_HArray1OfNode(1,nbnodes);
    myNodeIdentifiers = new TColStd_HArray1OfInteger(1,nbnodes);

    for (Standard_Integer i = 1; i <= nbnodes; i ++)
    {
      theReader.ReadInteger(myNodeIdentifiers->ChangeValue(i),"Node no. identifier");
      theReader.ReadPointer(myNodes->ChangeValue(i),"FEM Node");
      for (Standard_Integer j = 1; j <= nbval; j ++)
        theReader.ReadReal(myData->ChangeValue(i,j),"Value");
    }
  }
}

void IGESAppli_NodalResults::OwnWrite (IGESData_IGESWriter &IW) const
{
  const Standard_Integer nbnodes = myNodes->Length();
  Standard_Integer nbdata = myData->RowLength();
  IW.Send(myNote);
  IW.Send(mySubCaseNum);
  IW.Send(myTime);
  IW.Send(nbdata);
  IW.Send(nbnodes);
  for (Standard_Integer i = 1; i <= nbnodes; i++)
  {
    IW.Send(myNodeIdentifiers->Value(i));
    IW.Send(myNodes->Value(i));
    for (Standard_Integer j = 1; j <= nbdata; j++)
      IW.Send(myData->Value(i,j));
  }
}

void IGESAppli_NodalResults::OwnShared(Interface_EntityIterator &theIter) const
{
  theIter.GetOneItem(myNote);
  const Standard_Integer nbnodes = myNodes->Length();
  for (Standard_Integer i = 1; i <= nbnodes; i++)
    theIter.GetOneItem(myNodes->Value(i));
}

IGESData_DirChecker IGESAppli_NodalResults::DirChecker () const
{
  IGESData_DirChecker DC(146,0,34);  // Type = 146 Form No. = 0 to 34
  DC.Structure(IGESData_DefVoid);
  DC.LineFont(IGESData_DefVoid);
  DC.LineWeight(IGESData_DefVoid);
  DC.Color(IGESData_DefAny);
  DC.BlankStatusIgnored();
  DC.UseFlagRequired(03);
  DC.HierarchyStatusIgnored();
  return DC;
}

void IGESAppli_NodalResults::OwnCheck (const Interface_ShareTool &, const Handle(Interface_Check) &theCheck) const
{
  const Standard_Integer nv = NbData();
  Standard_Boolean OK = Standard_True;
  switch (myForm) {
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

void IGESAppli_NodalResults::OwnDump (const IGESData_IGESDumper &dumper, const Handle(Message_Messenger) &S, const Standard_Integer level) const
{
  S << "IGESAppli_NodalResults" << endl;

  S << "General Note : ";
  dumper.Dump(myNote,S,(level <= 4) ? 0 : 1);
  S << endl;
  S << "Analysis subcase number : " << mySubCaseNum << "  ";
  S << "Time used : " << myTime << endl;
  S << "No. of nodes : " << myNodes->Length() << "  ";
  S << "No. of values for a node : " << myData->RowLength() << endl;
  S << "Node Identifiers : " << endl;
  S << "Nodes : " << endl;
  S << "Data : ";  if (level < 6) S << " [ask level > 5]";
  S << endl;
  if (level > 4)
  {
    for (Standard_Integer i=1; i <= myNodes->Length(); i++)
    {
      S << "[" << i << "]: ";
      S << "NodeIdentifier : " << myNodeIdentifiers->Value(i) << "  ";
      S << "Node : ";
      dumper.Dump (myNodes->Value(i),S,1);
      S << endl;
      if (level < 6) continue;
      S << "Data : [ ";
      for (Standard_Integer j = 1; j <= myData->RowLength(); j ++)
        S << "  " << myData->Value(i,j);
      S << " ]" << endl;
    }
  }
}
