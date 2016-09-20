// Created on: 1994-10-03
// Created by: Christian CAILLET
// Copyright (c) 1994-1999 Matra Datavision
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

#include <TransferBRep_ShapeMapper.hxx>
#include <TransferBRep.hxx>
#include <TopTools_ShapeMapHasher.hxx>

IMPLEMENT_STANDARD_RTTIEXT(TransferBRep_ShapeMapper,Transfer_Finder)

Standard_Integer TransferBRep_ShapeMapper::GetHashCode(const Standard_Integer theUpper) const
{
  return TopTools_ShapeMapHasher::HashCode(myShape,theUpper);
}

Standard_Boolean TransferBRep_ShapeMapper::Equates (const Handle(Transfer_Finder)& theOther) const
{
  Handle(TransferBRep_ShapeMapper) anOther = Handle(TransferBRep_ShapeMapper)::DownCast(theOther);
  if (anOther.IsNull()) return Standard_False;
  return TopTools_ShapeMapHasher::IsEqual (myShape,anOther->Shape());
}

Standard_CString TransferBRep_ShapeMapper::ValueTypeName () const
{
  return TransferBRep::TypeName(myShape);
}
