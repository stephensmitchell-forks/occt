// Copyright (c) 2013 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

#ifndef _Transfer_Iterator_HeaderFile
#define _Transfer_Iterator_HeaderFile

#include <Standard_NoSuchObject.hxx>
#include <Transfer_TransferIterator.hxx>
#include <Transfer_Binder.hxx>
#include <Standard_Boolean.hxx>

//! This is a TransferIterator PLUS the capability to access
//! the Starting Objects. It is given as a Result from
//! TransferProcess. It can be used when a TransferIterator
//! is required.
//! See also : TransferIterator, for more information
template <class TheStart, class List, class ListHandle, class TransferMap>
class Transfer_Iterator : public Transfer_TransferIterator
{
private:
  ListHandle  thestarts;

public:
  DEFINE_STANDARD_ALLOC
  
  Standard_EXPORT Transfer_Iterator (const Standard_Boolean withstarts)
  : Transfer_TransferIterator ()
  {  if (withstarts) thestarts = new List();  }


  Standard_EXPORT void Add (const Handle(Transfer_Binder)& binder)
  {
    if (!thestarts.IsNull()) Standard_NoSuchObject::Raise
      ("Transfer_Iterator : Add, Starting Object required not provided");
    AddItem(binder);
  }

  Standard_EXPORT  void Add (const Handle(Transfer_Binder)& binder,
                             const TheStart& start)
  {
    AddItem(binder);
    if (!thestarts.IsNull()) thestarts->Append(start);
  }

  Standard_EXPORT void Filter (const ListHandle& list,
                               const Standard_Boolean keep = Standard_True)
  {
    if (list.IsNull() || thestarts.IsNull()) return;
    Standard_Integer i, j, nb = thestarts->Length();
    if (nb == 0) return;
    Handle(Transfer_Binder) factice;
    TransferMap amap (nb);
    for (i = 1; i <= nb; i ++) {
      j = amap.Add (thestarts->Value(i),factice);
      SelectItem (j,!keep);
    }

    //  Comparaison
    nb = list->Length();
    for (i = 1; i <= nb; i ++) {
      j = amap.FindIndex (list->Value(i));
      if (j > 0) SelectItem (j,keep);
    }
  }

  Standard_EXPORT Standard_Boolean HasStarting () const
  {  return (!thestarts.IsNull());  }

  Standard_EXPORT const TheStart& Starting () const
  {

    if (thestarts.IsNull()) Standard_NoSuchObject::Raise
      ("TransferIterator : No Starting defined at all");
    return thestarts->Value(thecurr);
  }
};
#endif