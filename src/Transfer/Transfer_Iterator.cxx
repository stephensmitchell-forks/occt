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


#include <Standard_NoSuchObject.hxx>
#include <Transfer_Binder.hxx>
#include <Transfer_Iterator.hxx>
#include <Transfer_HSequenceOfBinder.hxx>
#include <TColStd_HSequenceOfInteger.hxx>

Transfer_Iterator::Transfer_Iterator ()
{
  theitems = new Transfer_HSequenceOfBinder;
  theselect = new TColStd_HSequenceOfInteger;
  thecurr = 1;
}

void Transfer_Iterator::AddItem (const Handle(Transfer_Binder)& atr)
{
  theitems->Append(atr);
  theselect->Append(1);
}

void Transfer_Iterator::SelectItem (const Standard_Integer num, const Standard_Boolean keep)
{
  if (num < 1 || num > theselect->Length()) return;
  theselect->SetValue (num,(keep? 1 : 0));
}

//  ....                Iteration-Interrogations                ....

Standard_Integer Transfer_Iterator::Number () const
{
  Standard_Integer i, n = theselect->Length(), numb = 0;
  for (i = 1; i <= n; i ++) {
    if (theselect->Value(i) != 0) numb++;
  }
  return numb;
}

Standard_Boolean Transfer_Iterator::More ()
{
  if (thecurr > theselect->Length()) return Standard_False;
  if (theselect->Value(thecurr) == 0) Next();
  if (thecurr > theselect->Length()) return Standard_False;
  return (theselect->Value(thecurr) > 0);
}

void Transfer_Iterator::Next ()
{
  thecurr++;
  if (thecurr > theselect->Length()) return;
  if (theselect->Value(thecurr) == 0) Next();
}

const Handle(Transfer_Binder)& Transfer_Iterator::Value () const
{
  if (thecurr == 0 || thecurr > theselect->Length())
    Standard_NoSuchObject::Raise ("Transfer_Iterator::Value");
  if (theselect->Value(thecurr) == 0)
    Standard_NoSuchObject::Raise ("Transfer_Iterator::Value");
  return theitems->Value(thecurr);
}
