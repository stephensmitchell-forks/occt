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

#include <Transfer_Hasher.hxx>
#include <Transfer_Finder.hxx>
#include <TColStd_MapTransientHasher.hxx>

Standard_Integer Transfer_Hasher::HashCode (const Handle(Standard_Transient)& K, const Standard_Integer Upper)
{
  Handle(Transfer_Finder) F = Handle(Transfer_Finder)::DownCast(K);
  if (F.IsNull()) return TColStd_MapTransientHasher::HashCode(K,Upper);
  //szv_c1:return ((F->GetHashCode() - 1) % Upper) + 1;
  return F->GetHashCode(Upper);
}

Standard_Boolean Transfer_Hasher::IsEqual (const Handle(Standard_Transient)& K1, const Handle(Standard_Transient)& K2)
{
  Handle(Transfer_Finder) F1 = Handle(Transfer_Finder)::DownCast(K1);
  if (F1.IsNull()) return TColStd_MapTransientHasher::IsEqual(K1,K2);
  Handle(Transfer_Finder) F2 = Handle(Transfer_Finder)::DownCast(K2);
  if (F2.IsNull()) return Standard_False;
  return F1->Equates(F2);
}
