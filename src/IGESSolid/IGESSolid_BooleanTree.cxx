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

#include <IGESSolid_BooleanTree.hxx>
#include <Standard_DimensionError.hxx>
#include <Standard_OutOfRange.hxx>
#include <Interface_EntityIterator.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESSolid_BooleanTree,IGESData_IGESEntity)

void IGESSolid_BooleanTree::Init
  (const Handle(IGESData_HArray1OfIGESEntity)& operands,
   const Handle(TColStd_HArray1OfInteger)& operations)
{
  if (operands->Lower()  != 1 || operations->Lower() != 1 ||
      operands->Length() != operations->Length())
    Standard_DimensionError::Raise("IGESSolid_BooleanTree : Init");

  theOperations = operations;
  theOperands   = operands;
  InitTypeAndForm(180,0);
}

Standard_Integer IGESSolid_BooleanTree::Length () const
{
  return theOperands->Length();
}

Standard_Boolean IGESSolid_BooleanTree::IsOperand (const Standard_Integer Index) const
{
  return (!theOperands->Value(Index).IsNull());
}

Handle(IGESData_IGESEntity) IGESSolid_BooleanTree::Operand (const Standard_Integer Index) const
{
  return theOperands->Value(Index);
}

Standard_Integer IGESSolid_BooleanTree::Operation (const Standard_Integer Index) const
{
  if (theOperands->Value(Index).IsNull())
    return theOperations->Value(Index);
  else
    return 0;      // It is not an operation. (operations can be : 1-2-3)
}

void IGESSolid_BooleanTree::OwnShared(Interface_EntityIterator &theIter) const
{
  const Standard_Integer length = theOperands->Length();
  for (Standard_Integer i = 1; i <= length; i++)
  {
    if (!theOperands->Value(i).IsNull())
      theIter.GetOneItem(theOperands->Value(i));
  }
}

void IGESSolid_BooleanTree::OwnCheck (const Interface_ShareTool &, const Handle(Interface_Check) &theCheck) const
{
  const Standard_Integer length = Length();
  if (length <= 2)
    theCheck->AddFail("Length of post-order notation : Less than three");
  else {
    if (!IsOperand(1)) theCheck->AddFail("First Item is not an Operand");
    if (!IsOperand(2)) theCheck->AddFail("Second Item is not an Operand");
    if ( IsOperand(length)) theCheck->AddFail("Last Item is not an Operation");
  }
  for (Standard_Integer i = 1; i <= length; i ++) {
    if (!Operand(i).IsNull()) continue;
    if (Operation(i) < 1 || Operation(i) > 3) {
      char mess[80];
      sprintf(mess,"Item no. %d Incorrect",i);
      theCheck->AddFail(mess);
    }
  }
}
