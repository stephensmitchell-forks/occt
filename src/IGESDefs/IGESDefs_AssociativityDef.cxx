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

#include <IGESBasic_HArray1OfHArray1OfInteger.hxx>
#include <IGESDefs_AssociativityDef.hxx>
#include <Standard_DimensionMismatch.hxx>
#include <Standard_OutOfRange.hxx>
#include <IGESFile_Reader.hxx>
#include <IGESData_IGESWriter.hxx>
#include <IGESData_DirChecker.hxx>
#include <Message_Messenger.hxx>
#include <IGESData_Dump.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESDefs_AssociativityDef,IGESData_IGESEntity)

Standard_Integer IGESDefs_AssociativityDef::NbClassDefs () const 
{
  return myBackPointerReqs->Length();
}

Standard_Boolean IGESDefs_AssociativityDef::IsBackPointerReq (const Standard_Integer ClassNum) const
{
  return (myBackPointerReqs->Value(ClassNum) == 1);
//  1 True  2 False
}

Standard_Integer IGESDefs_AssociativityDef::BackPointerReq (const Standard_Integer ClassNum) const
{
  return myBackPointerReqs->Value(ClassNum);
}

Standard_Boolean IGESDefs_AssociativityDef::IsOrdered (const Standard_Integer ClassNum) const
{
  return (myClassOrders->Value(ClassNum) == 1);
//  1 True  2 False
}

Standard_Integer IGESDefs_AssociativityDef::ClassOrder (const Standard_Integer ClassNum) const
{
  return myClassOrders->Value(ClassNum);
}

Standard_Integer IGESDefs_AssociativityDef::NbItemsPerClass (const Standard_Integer ClassNum) const
{
  return myNbItemsPerClass->Value(ClassNum);
}

Standard_Integer IGESDefs_AssociativityDef::Item (const Standard_Integer ClassNum, const Standard_Integer ItemNum) const
{
  return myItems->Value(ClassNum)->Value(ItemNum);
}

void IGESDefs_AssociativityDef::OwnRead (IGESFile_Reader& theReader)
{ 
  Standard_Integer nbval = 0;
  theReader.ReadInteger(nbval,"No. of Class definitions");
  if (nbval > 0)
  {
    myBackPointerReqs = new TColStd_HArray1OfInteger(1, nbval);
    myClassOrders = new TColStd_HArray1OfInteger(1, nbval);
    myNbItemsPerClass = new TColStd_HArray1OfInteger(1, nbval);
    myItems = new IGESBasic_HArray1OfHArray1OfInteger(1, nbval);

    for (Standard_Integer i = 1; i <= nbval; i++)
    {
      theReader.ReadInteger(myBackPointerReqs->ChangeValue(i),"Back Pointer Requirement");
      theReader.ReadInteger(myClassOrders->ChangeValue(i),"Ordered/Unordered Class");
	
      Standard_Integer numItem = 0;
      theReader.ReadInteger(numItem,"No. of items per entry");
      if (numItem > 0)
      {
        myNbItemsPerClass->SetValue(i, numItem);
        Handle(TColStd_HArray1OfInteger) item = new TColStd_HArray1OfInteger(1, numItem);
        for (Standard_Integer j = 1; j <= numItem; j++)
          theReader.ReadInteger(item->ChangeValue(j),"Item");
        myItems->SetValue(i, item);
      }
    }
  }
  else theReader.AddFail("No. of Class definitions: Not Positive");
}

void IGESDefs_AssociativityDef::OwnWrite (IGESData_IGESWriter& IW) const
{ 
  const Standard_Integer upper = NbClassDefs();
  IW.Send(upper); 
  for (Standard_Integer i = 1; i <= upper; i++) {
    IW.Send(BackPointerReq(i));
    IW.Send(ClassOrder(i));
    IW.Send(NbItemsPerClass(i));
    const Standard_Integer items = NbItemsPerClass(i);
    for (Standard_Integer j = 1; j <= items; j++)
      IW.Send(Item(i,j));
  }
}

IGESData_DirChecker IGESDefs_AssociativityDef::DirChecker () const
{ 
  IGESData_DirChecker DC (302, 5001, 9999);
  DC.Structure(IGESData_DefVoid);
  DC.LineFont(IGESData_DefVoid);
  DC.LineWeight(IGESData_DefVoid);
  DC.Color(IGESData_DefVoid);
  DC.BlankStatusIgnored();
  DC.SubordinateStatusRequired(0);
  DC.UseFlagRequired(2);
  DC.HierarchyStatusIgnored();
  return DC;
}

void IGESDefs_AssociativityDef::OwnDump (const IGESData_IGESDumper &, const Handle(Message_Messenger) &S, const Standard_Integer level) const
{ 
  S << "IGESDefs_AssociativityDef" << endl;
  S << "Number of Class Definitions : " << NbClassDefs() << endl;
  S << "Back Pointer Requirement  : " << endl; 
  S << "Ordered / Unordered Class : " << endl;
  S << "Number Of Items per Entry : " << endl;
  S << "Items : " << endl; 
  IGESData_DumpVals(S,-level,1,NbClassDefs(),BackPointerReq);
  S << endl;
  if (level > 4) {
    // Warning : Item is a JAGGED Array
    const Standard_Integer upper = NbClassDefs();
    for (Standard_Integer i = 1; i <= upper; i ++) {
      S << "[" << i << "]: " << endl;
      S << "Back Pointer Requirement : "  << BackPointerReq(i) << (IsBackPointerReq(i)? "  (Yes)  " : "  (No)   ");
      S << " Ordered/Unordered Class : "  << ClassOrder(i) << (IsOrdered(i)? " (Yes)" : " (No)")  << endl;
      S << "Number Of Items per Entry : " << NbItemsPerClass(i);
      if (level < 6) {
        S << " [ask level > 5 for more]" << endl;
        continue;
      }
      S << endl << " [";
      for (Standard_Integer j = 1; j <= NbItemsPerClass(i); j ++)
        S << "  " << Item(i,j);
      S << "]" << endl;
    }
  }
  S << endl;
}
