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

IMPLEMENT_STANDARD_RTTIEXT(IGESAppli_PipingFlow,IGESData_IGESEntity)

void IGESAppli_PipingFlow::Init
  (const Standard_Integer nbContextFlags,
   const Standard_Integer aFlowType,
   const Handle(IGESData_HArray1OfIGESEntity)& allFlowAssocs,
   const Handle(IGESDraw_HArray1OfConnectPoint)& allConnectPoints,
   const Handle(IGESData_HArray1OfIGESEntity)& allJoins,
   const Handle(Interface_HArray1OfHAsciiString)& allFlowNames,
   const Handle(IGESGraph_HArray1OfTextDisplayTemplate)& allTextDisps,
   const Handle(IGESData_HArray1OfIGESEntity)& allContFlowAssocs)
{
  Standard_Integer num = allFlowAssocs->Length();
  if (allFlowAssocs->Lower()     != 1 ||
      allConnectPoints->Lower()  != 1 || allConnectPoints->Length()  != num ||
      allJoins->Lower()          != 1 || allJoins->Length()          != num ||
      allFlowNames->Lower()      != 1 || allFlowNames->Length()      != num ||
      allContFlowAssocs->Lower() != 1 || allContFlowAssocs->Length() != num )
    Standard_DimensionMismatch::Raise("IGESAppli_PipingFlow : Init");
  theNbContextFlags          = nbContextFlags;
  theTypeOfFlow              = aFlowType;
  theFlowAssociativities     = allFlowAssocs;
  theConnectPoints           = allConnectPoints;
  theJoins                   = allJoins;
  theFlowNames               = allFlowNames;
  theTextDisplayTemplates    = allTextDisps;
  theContFlowAssociativities = allContFlowAssocs;
  InitTypeAndForm(402,20);
}

Standard_Boolean IGESAppli_PipingFlow::OwnCorrect ()
{
  if (theNbContextFlags == 1) return Standard_False;
  theNbContextFlags = 1;
  return Standard_True;
}

Standard_Integer IGESAppli_PipingFlow::NbFlowAssociativities () const
{
  return theFlowAssociativities->Length();
}

Standard_Integer IGESAppli_PipingFlow::NbConnectPoints () const
{
  return theConnectPoints->Length();
}

Standard_Integer IGESAppli_PipingFlow::NbJoins () const
{
  return theJoins->Length();
}

Standard_Integer IGESAppli_PipingFlow::NbFlowNames () const
{
  return theFlowNames->Length();
}

Standard_Integer IGESAppli_PipingFlow::NbTextDisplayTemplates () const
{
  return theTextDisplayTemplates->Length();
}

Standard_Integer IGESAppli_PipingFlow::NbContFlowAssociativities () const
{
  return theContFlowAssociativities->Length();
}

const Handle(IGESData_IGESEntity) & IGESAppli_PipingFlow::FlowAssociativity (const Standard_Integer Index) const
{
  return theFlowAssociativities->Value(Index);
}

const Handle(IGESDraw_ConnectPoint) & IGESAppli_PipingFlow::ConnectPoint (const Standard_Integer Index) const
{
  return theConnectPoints->Value(Index);
}

const Handle(IGESData_IGESEntity) & IGESAppli_PipingFlow::Join (const Standard_Integer Index) const
{
  return theJoins->Value(Index);
}

const Handle(TCollection_HAsciiString) & IGESAppli_PipingFlow::FlowName (const Standard_Integer Index) const
{
  return theFlowNames->Value(Index);
}

const Handle(IGESGraph_TextDisplayTemplate) & IGESAppli_PipingFlow::TextDisplayTemplate (const Standard_Integer Index) const
{
  return theTextDisplayTemplates->Value(Index);
}

const Handle(IGESData_IGESEntity) & IGESAppli_PipingFlow::ContFlowAssociativity (const Standard_Integer Index) const
{
  return theContFlowAssociativities->Value(Index);
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

void IGESAppli_PipingFlow::OwnCheck (const Interface_ShareTool &, const Handle(Interface_Check) &theCheck) const
{
  if (NbContextFlags() != 1)
    theCheck->AddFail("Number of Context Flags != 1");
  if ((TypeOfFlow() < 0) || (TypeOfFlow() > 2))
    theCheck->AddFail("Type of Flow != 0,1,2");
}
