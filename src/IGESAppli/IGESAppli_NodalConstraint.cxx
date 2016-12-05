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

IMPLEMENT_STANDARD_RTTIEXT(IGESAppli_NodalConstraint,IGESData_IGESEntity)

void IGESAppli_NodalConstraint::Init
  (const Standard_Integer aType,
   const Handle(IGESAppli_Node)& aNode,
   const Handle(IGESDefs_HArray1OfTabularData)& allTabData)
{
  if (allTabData->Lower() != 1)
    Standard_DimensionMismatch::Raise("IGESAppli_NodalConstraint : Init");
  theType = aType;
  theNode = aNode;
  theTabularDataProps = allTabData;
  InitTypeAndForm(418,0);
}

Standard_Integer IGESAppli_NodalConstraint::NbCases () const
{
  return theTabularDataProps->Length();
}

const Handle(IGESDefs_TabularData) & IGESAppli_NodalConstraint::TabularData (const Standard_Integer Index) const
{
  return theTabularDataProps->Value(Index);
}

void IGESAppli_NodalConstraint::OwnShared(Interface_EntityIterator &theIter) const
{
  theIter.GetOneItem(NodeEntity());
  const Standard_Integer num = NbCases();
  for ( Standard_Integer i = 1; i <= num; i++ )
    theIter.GetOneItem(TabularData(i));
}

void IGESAppli_NodalConstraint::OwnCheck (const Interface_ShareTool &, const Handle(Interface_Check) &theCheck) const
{
  if ((Type() != 1) && (Type() != 2))
    theCheck->AddFail("Type of Constraint != 1,2");
}
