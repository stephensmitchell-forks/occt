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
  ListHandle  myStarts;

public:
  DEFINE_STANDARD_ALLOC
  
  Standard_EXPORT Transfer_Iterator (const Standard_Boolean theWithStarts)
  : Transfer_TransferIterator ()
  {  if (theWithStarts)
       myStarts = new List();  }


  Standard_EXPORT void Add (const Handle(Transfer_Binder)& theBinder)
  {
    if (!myStarts.IsNull()) Standard_NoSuchObject::Raise
      ("Transfer_Iterator : Add, Starting Object required not provided");
    AddItem(theBinder);
  }

  Standard_EXPORT  void Add (const Handle(Transfer_Binder)& theBinder,
                             const TheStart& theStart)
  {
    AddItem (theBinder);
    if (!myStarts.IsNull())
      myStarts->Append(theStart);
  }

  Standard_EXPORT void Filter (const ListHandle& theList,
                               const Standard_Boolean toKeep = Standard_True)
  {
    if (theList.IsNull() || myStarts.IsNull())
      return;
    Standard_Integer anI, aJ, aNb = myStarts->Length();
    if (aNb == 0) return;
    Handle(Transfer_Binder) factice;
    TransferMap amap (aNb);
    for (anI = 1; anI <= aNb; anI ++) {
      aJ = amap.Add (myStarts->Value(anI),factice);
      SelectItem (aJ,!toKeep);
    }

    //  Comparaison
    aNb = theList->Length();
    for (anI = 1; anI <= aNb; anI ++) {
      aJ = amap.FindIndex (theList->Value(anI));
      if (aJ > 0) SelectItem (aJ,toKeep);
    }
  }

  Standard_EXPORT Standard_Boolean HasStarting () const
  {
    return (!myStarts.IsNull());
  }

  Standard_EXPORT const TheStart& Starting () const
  {
    if (myStarts.IsNull()) Standard_NoSuchObject::Raise
      ("TransferIterator : No Starting defined at all");
    return myStarts->Value(thecurr);
  }
};
#endif