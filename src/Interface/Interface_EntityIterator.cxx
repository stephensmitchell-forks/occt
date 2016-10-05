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


#include <Interface_EntityIterator.hxx>
#include <Standard_NoMoreObject.hxx>
#include <Standard_NoSuchObject.hxx>

Interface_EntityIterator::Interface_EntityIterator (const Handle(TColStd_HSequenceOfTransient)& list)
{
  thecurr = 0;
  thelist = list;
}

void Interface_EntityIterator::AddList (const Handle(TColStd_HSequenceOfTransient)& list)
{
  if (thelist.IsNull()) thelist = new TColStd_HSequenceOfTransient();
  thecurr = 0;
  thelist->Append(list);
}

void Interface_EntityIterator::AddItem (const Handle(Standard_Transient)& anentity)
{
  if (anentity.IsNull()) return;
  if (thelist.IsNull()) thelist = new TColStd_HSequenceOfTransient();
  thecurr = 0;
  thelist->Append(anentity);
}

void Interface_EntityIterator::Reset ()
{
  thecurr = 0;
  thelist = new TColStd_HSequenceOfTransient();
}

// .... Fonctionnalites de tri prealable a l'iteration ....

//  Facon "bete" : supprimer les termes qui ne conviennent pas : lent !
//  Mieux vaut refaire une autre sequence a cote

void Interface_EntityIterator::SelectType (const Handle(Standard_Type)& atype, const Standard_Boolean keep)
{
  if (thelist.IsNull()) return;
  Standard_Integer i, n = thelist->Length();
  Handle(TColStd_HSequenceOfTransient) nlist = new TColStd_HSequenceOfTransient();
  for (i = 1 ; i <= n ; i ++) {
    if (thelist->Value(i)->IsKind(atype) == keep) nlist->Append(thelist->Value(i));
  }
  thelist = nlist;
}

//  ....  Iteration proprement dite  ....

Standard_Integer Interface_EntityIterator::NbEntities () const
{
  return thelist.IsNull()? 0 : thelist->Length();
}

Standard_Integer Interface_EntityIterator::NbTyped (const Handle(Standard_Type)& atype) const
{
  Standard_Integer res = 0;
  if (thelist.IsNull()) return res;
  Standard_Integer i, n = thelist->Length();
  for (i = 1 ; i <= n ; i ++) {
    if (thelist->Value(i)->IsKind(atype)) res ++;
  }
  return res;
}

Interface_EntityIterator Interface_EntityIterator::Typed (const Handle(Standard_Type)& atype) const
{
  Interface_EntityIterator res;
  if (thelist.IsNull()) return res;
  Standard_Integer i, n = thelist->Length();
  for (i = 1 ; i <= n ; i ++) {
    if (thelist->Value(i)->IsKind(atype)) res.AddItem (thelist->Value(i));
  }
  return res;
}

void Interface_EntityIterator::Start () const
{
  thecurr = 1;
}

Standard_Boolean Interface_EntityIterator::More () const
{
  if (thecurr == 0) Start();  // preparation de l iteration
  return thelist.IsNull()? Standard_False : (thecurr <= thelist->Length());
}

const Handle(Standard_Transient)& Interface_EntityIterator::Value () const
{
  //  NbEntity pas const (on ne sait pas comment il est implemente apres tout)
  if (thelist.IsNull())
    Standard_NoSuchObject::Raise("Interface_EntityIterator");
  if (thecurr < 1 || thecurr > thelist->Length())
    Standard_NoSuchObject::Raise("Interface_EntityIterator");
  return thelist->Value(thecurr);
}

Handle(TColStd_HSequenceOfTransient) Interface_EntityIterator::Content () const
{
  return (thelist.IsNull()? new TColStd_HSequenceOfTransient() : thelist);
}
