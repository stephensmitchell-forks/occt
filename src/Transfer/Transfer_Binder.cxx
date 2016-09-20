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


#include <Interface_Check.hxx>
#include <Interface_IntVal.hxx>
#include <TCollection_HAsciiString.hxx>
#include <Transfer_Binder.hxx>
#include <Transfer_TransferFailure.hxx>
#include <Transfer_VoidBinder.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Transfer_Binder,MMgt_TShared)

//=======================================================================
//function : Transfer_Binder
//purpose  : 
//=======================================================================

Transfer_Binder::Transfer_Binder ()
: thestatus(Transfer_StatusVoid),
  theexecst(Transfer_StatusInitial),
  thecheck(new Interface_Check)
{
}

//=======================================================================
//function : Merge
//purpose  : 
//=======================================================================

void Transfer_Binder::Merge (const Handle(Transfer_Binder)& other)
{
  if (other.IsNull()) return;
  if ((int) theexecst < (int) other->StatusExec()) theexecst = other->StatusExec();
  thecheck->GetMessages (other->Check());
}

//=======================================================================
//function : IsMultiple
//purpose  : 
//=======================================================================

Standard_Boolean Transfer_Binder::IsMultiple () const
{
  if (thenextr.IsNull()) return Standard_False;
  if (!HasResult()) return thenextr->IsMultiple();

  Handle(Transfer_Binder) next = thenextr;
  while (!next.IsNull()) {
    if (next->HasResult()) return Standard_True;
    next = next->NextResult();
  }
  return Standard_False;
}

//=======================================================================
//function : AddResult
//purpose  : 
//=======================================================================

void Transfer_Binder::AddResult (const Handle(Transfer_Binder)& next)
{
  if (next == this || next.IsNull()) return;
  next->CutResult(this);
  if (thenextr.IsNull()) 
    thenextr = next;
  else {
    Handle(Transfer_Binder) theBinder = thenextr;
    while( theBinder != next ) { 
      if( theBinder->NextResult().IsNull() ) {
        theBinder->AddResult(next);
        return;
      }
      else
        theBinder = theBinder->NextResult();
    }
  }
}

//=======================================================================
//function : CutResult
//purpose  : 
//=======================================================================

void Transfer_Binder::CutResult (const Handle(Transfer_Binder)& next)
{
  if (thenextr.IsNull()) return;
  if (thenextr == next) thenextr.Nullify();
  else {
    Handle(Transfer_Binder) currBinder = thenextr, currNext;
    while( !( (currNext = currBinder->NextResult()) == next ) ) {
      if( currNext.IsNull() )
        return;
      currBinder = currNext;
    }
    currBinder->CutResult(next);
  }
}

//=======================================================================
//function : SetResultPresent
//purpose  : 
//=======================================================================

void Transfer_Binder::SetResultPresent ()
{
  if (thestatus == Transfer_StatusUsed)
    Transfer_TransferFailure::Raise("Transfer_Binder::SetResultPresent : Result is Already Set and Used");
  theexecst = Transfer_StatusDone;
  thestatus = Transfer_StatusDefined;
}

//  ############    Checks    ############

//=======================================================================
//function : AddFail
//purpose  : 
//=======================================================================

void Transfer_Binder::AddFail (const Standard_CString mess, const Standard_CString orig)
{
  theexecst = Transfer_StatusError;
  thecheck->AddFail(mess,orig);
}

//=======================================================================
//function : AddWarning
//purpose  : 
//=======================================================================

void Transfer_Binder::AddWarning (const Standard_CString mess, const Standard_CString orig)
{
  thecheck->AddWarning(mess,orig);
}
