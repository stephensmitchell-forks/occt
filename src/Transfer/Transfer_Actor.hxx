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

#ifndef _Transfer_Actor_HeaderFile
#define _Transfer_Actor_HeaderFile

#include <Standard_Boolean.hxx>
#include <MMgt_TShared.hxx>
#include <Transfer_Binder.hxx>
#include <Transfer_SimpleBinderOfTransient.hxx>
#include <Handle_Standard_Transient.hxx>
#include <Handle_Transfer_HSequenceOfFinder.hxx>

//! An Actor performs the effective Transfer of a Starting
//!           Object, piloted by a TransferProcess. That is :
//!           - It recognizes, for a Starting Object, the case to be run
//!           - Then, it performs the Transfer : the recommanded way to
//!             work is to define for each Type of Object to Transfer, a
//!             specific Transfer Tool, then to call it by the Actor
//!           - It assumes that the Result of Transfer is stored in a
//!             Binder, it creates it if the Tool does not do
//!             It can use the TransferProcess for intermediate Results
//!           - It returns the Binder which stores the Result
//!           It no Result can be produced, it returns a Null Handle
//!           It may bind this Binder to <start> but is not obliged :
//!           TransferProcess controls this point.
//!           
//!           Remark about the kind of returned Binder :
//!           - for a Transient Result,  use the method TransientResult
//!           - else, cast and query the Binder itself
//!           
//!           An Actor can be built by combination : this allows to
//!           gather several Actors, each one processing a set of cases.

template < class ActorHandle, class TheStart, class TransferProcessHandle >
class Transfer_Actor : public MMgt_TShared
{
private:
  ActorHandle myNext;
  Standard_Boolean myLast;
public:
  Standard_EXPORT Transfer_Actor () {  }

  //! Prerequesite for Transfer : the method Transfer is
  //! called on a starting object only if Recognize has
  //! returned True on it. This allows to define a list
  //! of Actors, each one processing a definite kind of data.
  //! TransferProcess calls Recognize on each one before
  //! calling Transfer. But even if Recognize has returned
  //! true, Transfer can reject by returning a Null Binder
  //! (afterwards rejection), the theNext actor is then invoked.
  //!  The provided default returns True, can be redefined.
  Standard_EXPORT   Standard_Boolean  Recognize (const TheStart& /*start*/)
  {
    return Standard_True;
  }

  //! Specific action of Transfer. The Result is stored in
  //! the returned Binder, or a Null Handle for "No result"
  //! (Default defined as doing nothing; should be deffered)
  //! "mutable" allows the Actor to record intermediate
  //! information, in addition to those of TransferProcess.
  //! In case of Error during a Transfer, any kind of
  //! exception can be raised.
  Standard_EXPORT   Handle(Transfer_Binder) Transferring (const TheStart& /*start*/,
                                                          const TransferProcessHandle& /*TP*/)
  {
    return NullResult();
  }

  //! Prepares and returns a binder for a Transient result.
  //! Returns a Null Handle if <theRes> is itself null.
  Standard_EXPORT   Handle(Transfer_SimpleBinderOfTransient) TransientResult
                   (const Handle(Standard_Transient)& theRes) const
  {
    Handle(Transfer_SimpleBinderOfTransient) binder;
    if (theRes.IsNull())
      return binder;
    binder = new Transfer_SimpleBinderOfTransient;
    binder->SetResult (theRes);
    return binder;
  }

  //! Returns a binder for no result, i.e. a null handle.
  Standard_EXPORT   Handle(Transfer_Binder) NullResult () const
  {
    Handle(Transfer_Binder) binder;
    return binder;
  }

  //! If <theMode> is true, commands an Actor to be set at the
  //! end of the list of Actors (see SetNext).
  //! If it is False (creation default), each add Actor is
  //! set at the beginning of the list.
  //! This allows to define default Actors (which are Last).
  Standard_EXPORT   void SetLast (const Standard_Boolean theMode)
  {
    myLast = theMode;
  }
  
  //! Defines a Next Actor : it can then be asked to work if
  //! <me> produces no result for a given type of Object.
  //! If Next is already set and is not "Last", calls
  //! SetNext on it. If Next defined and "Last", the new
  //! actor is added before it in the list.
  Standard_EXPORT   void SetNext (const ActorHandle& theNext)
  {
    if (myNext == theNext)
      return;
    if (myNext.IsNull())
      myNext = theNext;
    else if (myNext->IsLast())
      {
        theNext->SetNext(myNext);
        myNext = theNext;
      }
    else
      myNext->SetNext(theNext);
  }
  
  //! Returns the actor defined as Next, or a null handle.
  Standard_EXPORT   ActorHandle Next () const
  {
    return myNext;
  }

  //! Returns the Last status (see SetLast).
  Standard_EXPORT   Standard_Boolean IsLast () const
  {
    return myLast;
  }
};
#endif
