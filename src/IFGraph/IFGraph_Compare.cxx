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


#include <IFGraph_AllShared.hxx>
#include <IFGraph_Compare.hxx>
#include <Interface_EntityIterator.hxx>

void IFGraph_Compare::GetFromEntity (const Handle(Standard_Transient)& ent, const Standard_Boolean first)
{
  IFGraph_AllShared iter(thegraph.Model(),ent);
  GetFromIter(iter,first);
}

void IFGraph_Compare::GetFromIter (const Interface_EntityIterator& iter, const Standard_Boolean first)
{
  Standard_Integer stat = 2;
  if (first) stat = 1;
  thegraph.GetFromIter(iter,stat,3,Standard_False);
}

void IFGraph_Compare::Merge ()
{
  thegraph.ChangeStatus (2,1);
  thegraph.ChangeStatus (3,1);
}

void IFGraph_Compare::RemoveSecond ()
{
  thegraph.ChangeStatus (3,1);
  thegraph.RemoveStatus (2);
}

void  IFGraph_Compare::KeepCommon ()
{
  thegraph.RemoveStatus (1);
  thegraph.RemoveStatus (2);
  thegraph.ChangeStatus (3,1);
}

void  IFGraph_Compare::Evaluate ()
{
  Reset();  GetFromGraph(thegraph);  // Evaluation deja faite par le graphe
}
