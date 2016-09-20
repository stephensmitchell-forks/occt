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

//  TheSourceType is assumed Handle(Standard_Transient) or Handle(Transfer_Finder)
//  It should provide : "==" , .IsNull() , ->DynamicType()

#ifndef _Transfer_ProcessGen_HeaderFile
#define _Transfer_ProcessGen_HeaderFile

#include <MMgt_TShared.hxx>

#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>

#include <Message_Messenger.hxx>
#include <Message_Msg.hxx>
#include <Message.hxx>

class Interface_InterfaceModel;
#include <Interface_MSG.hxx>
#include <Interface_CheckIterator.hxx>

class Transfer_Binder;
#include <Transfer_Actor.hxx>
#include <Transfer_Iterator.hxx>
#include <Transfer_Finder.hxx>
#include <Transfer_Hasher.hxx>

#include <Transfer_VoidBinder.hxx>
#include <Transfer_SimpleBinderOfTransient.hxx>
#include <Transfer_StatusResult.hxx>
#include <Transfer_TransferFailure.hxx>
#include <Transfer_TransferDeadLoop.hxx>

#include <TColStd_IndexedMapOfInteger.hxx>
#include <TColStd_Array1OfInteger.hxx>

#include <NCollection_IndexedDataMap.hxx>


template < class TheKeyType,
           class TheListType >
class Transfer_ProcessGen : public MMgt_TShared
{
 public:

  typedef NCollection_IndexedDataMap<TheKeyType,Handle(Transfer_Binder),Transfer_Hasher> Transfer_MapOfProcess;

  class Actor : public Transfer_Actor
  {
   public:
  
    Standard_EXPORT Actor() {}

    //! Prerequesite for Transfer : the method Transfer is
    //! called on a starting object only if Recognize has
    //! returned True on it
    //! This allows to define a list of Actors, each one
    //! processing a definite kind of data
    //! TransferProcess calls Recognize on each one before
    //! calling Transfer. But even if Recognize has returned
    //! True, Transfer can reject by returning a Null Binder
    //! (afterwards rejection), the next actor is then invoked
    //!
    //! The provided default returns True, can be redefined
    Standard_EXPORT virtual Standard_Boolean Recognize (const TheKeyType& /*start*/) { return Standard_True; }
  
    //! Specific action of Transfer. The Result is stored in
    //! the returned Binder, or a Null Handle for "No result"
    //! (Default defined as doing nothing; should be deffered)
    //! "mutable" allows the Actor to record intermediate
    //! information, in addition to those of TransferProcess
    Standard_EXPORT virtual Handle(Transfer_Binder) Transferring (const TheKeyType& /*start*/, const Handle(Transfer_ProcessGen)& /*TP*/) { return NULL; }

    //! Defines a Next Actor : it can then be asked to work if
    //! <me> produces no result for a given type of Object.
    //! If Next is already set and is not "Last", calls
    //! SetNext on it. If Next defined and "Last", the new
    //! actor is added before it in the list
    Standard_EXPORT void SetNext (const Handle(Actor)& next)
    {
      if (thenext == next) return;
      if (thenext.IsNull()) thenext = next;
      else                  thenext->SetNext(next);
    }
  
    //! Returns the Actor defined as Next, or a Null Handle
    const Handle(Actor) & Next() const { return thenext; }

    DEFINE_STANDARD_RTTI_INLINE(Actor,Transfer_Actor)

   protected:

    Handle(Actor) thenext;
  };

  class Iterator : public Transfer_Iterator
  {
   public:

    DEFINE_STANDARD_ALLOC
  
    //! Creates an empty Iterator
    //! If WithSource is True, each Binder to be iterated will
    //! be associated to its corresponding Source Object
    Standard_EXPORT Iterator(const Standard_Boolean WithSource)
    { if (WithSource) mySourceObjects = new TheListType(); }
  
    //! Adds a Binder to the iteration list (construction)
    //! with no corresponding Starting Object
    //! (note that Result is brought by Binder)
    Standard_EXPORT void Add (const Handle(Transfer_Binder)& theBinder)
    {
      if (!mySourceObjects.IsNull())
        Standard_NoSuchObject::Raise("Transfer_ProcessGen::Iterator::Add, Source Object required but not provided");
      AddItem(theBinder);
    }
  
    //! Adds a Binder to the iteration list, associated with
    //! its corresponding Starting Object "start"
    //! Starting Object is ignored if not required at
    //! Creation time
    Standard_EXPORT void Add (const Handle(Transfer_Binder)& theBinder, const TheKeyType& theSource)
    {
      AddItem(theBinder);
      if (!mySourceObjects.IsNull()) mySourceObjects->Append(theSource);
    }
  
    //! After having added all items, keeps or rejects items
    //! which are attached to starting data given by <list>
    //! <keep> = True (D) : keeps. <keep> = False : rejects
    //! Does nothing if <withstarts> was False
    Standard_EXPORT void Filter (const Handle(TheListType)& theList)
    {
      if (theList.IsNull() || mySourceObjects.IsNull()) return;
      Standard_Integer i, j, nb = mySourceObjects->Length();
      if (nb == 0) return;
      Handle(Transfer_Binder) factice;
      Transfer_MapOfProcess amap (nb);
      for (i = 1; i <= nb; i ++) {
        j = amap.Add (mySourceObjects->Value(i),factice);
        SelectItem (j,Standard_False);
      }

      // Comparaison
      nb = theList->Length();
      for (i = 1; i <= nb; i ++) {
        j = amap.FindIndex (theList->Value(i));
        if (j > 0) SelectItem (j,Standard_True);
      }
    }
  
    //! Returns corresponding Source Object
    Standard_EXPORT const TheKeyType& SourceObject() const
    {
      if (mySourceObjects.IsNull())
        Standard_NoSuchObject::Raise("Transfer_ProcessGen::Iterator : No Source Objects defined at all");
      return mySourceObjects->Value(thecurr);
    }

   private:

    Handle(TheListType) mySourceObjects;
  };

 public:

  //! Sets TransferProcess at initial state. Gives an Initial size
  //! (indicative) for the Map when known (default is 10000).
  //! Sets default trace file as a printer and default trace level
  //! (see Message_TraceFile).
  Standard_EXPORT Transfer_ProcessGen(const Standard_Integer nb = 10000)
  : themap (nb)
  {
    therootm = Standard_False;
    thelevel = 0;     therootl  = 0;
    themessenger = Message::DefaultMessenger();
    thetrace = 0;
    theindex = 0;
  }
  
  //! Sets TransferProcess at initial state. Gives an Initial size
  //! (indicative) for the Map when known (default is 10000).
  //! Sets a specified printer.
  Standard_EXPORT Transfer_ProcessGen(const Handle(Message_Messenger)& messenger, const Standard_Integer nb = 10000)
  : themap (nb)
  {
    therootm = Standard_False;
    thelevel = 0;     therootl  = 0;
    SetMessenger (messenger);
    thetrace = 1;
    theindex = 0;
  }
  
  //! Sets an InterfaceModel, which can be used during transfer
  //! for instance if a context must be managed, it is in the Model
  void SetModel (const Handle(Interface_InterfaceModel)& model) { themodel = model; }
  
  //! Returns the Model which can be used for context
  const Handle(Interface_InterfaceModel)& Model() const { return themodel; }
  
  //! Resets a TransferProcess as ready for a completely new work.
  //! Clears general data (roots) and the Map
  Standard_EXPORT void Clear()
  {
    thelevel = 0;     therootl  = 0;
    theroots.Clear();
    themap.Clear();
    theindex = 0;  thelastobj.Nullify();  thelastbnd.Nullify();
  }
  
  //! Rebuilds the Map and the roots to really remove Unbound items
  //! Because Unbind keeps the entity in place, even if not bound
  //! Hence, working by checking new items is meaningless if a
  //! formerly unbound item is rebound
  Standard_EXPORT void Clean()
  {
    Standard_Integer i, nb = NbMapped();
    Standard_Integer j,unb = 0;
    for (i = 1; i <= nb; i ++) {
      if (themap(i).IsNull()) unb ++;
    }
    if (unb == 0) return;

    // Refaire la map -> decalages
    TColStd_Array1OfInteger unbs (1,nb);  unbs.Init(0);
    Transfer_TransferMap newmap (nb*2);
    for (i = 1; i <= nb; i ++) {
      TheKeyType ent = Mapped(i);
      Handle(Transfer_Binder) bnd = MapItem(i);
      if (bnd.IsNull()) continue;
      j = newmap.Add (ent,bnd);
      unbs.SetValue (i,j);
    }
    themap.Assign (newmap);

    // Update the map of roots
    TColStd_IndexedMapOfInteger aNewRoots;
    for( i=1; i<= theroots.Extent(); i++ ) {
      j = theroots.FindKey(i);
      Standard_Integer k = unbs.Value(j);
      if ( k ) aNewRoots.Add ( k );
    }
    theroots.Clear();
    theroots = aNewRoots;

    // Le reste : nettoyage
    thelastobj.Nullify();
    thelastbnd.Nullify();
    theindex = 0;
  }
  
  //! Resizes the Map as required (if a new reliable value has been
  //! determined). Acts only if <nb> is greater than actual NbMapped
  Standard_EXPORT void Resize (const Standard_Integer nb) { if (nb > themap.NbBuckets()) themap.ReSize(nb); }
  
  //! Defines an Actor, which is used for automatic Transfer
  //! If already defined, the new Actor is cumulated
  //! (see SetNext from Actor)
  Standard_EXPORT void SetActor (const Handle(Actor)& actor)
  {
    if (theactor == actor)         return;
    if (theactor.IsNull())         theactor = actor;
    else if (actor.IsNull())       theactor = actor;  // declenche RAZ
    else                           theactor->SetNext(actor);
  }

  //! Returns the Binder which is linked with a starting Object
  //! It can either bring a Result (Transfer done) or none (for a
  //! pre-binding).
  //! If no Binder is linked with <start>, returns a Null Handle
  //! Considers a category number, by default 0
  Standard_EXPORT Handle(Transfer_Binder) Find (const TheKeyType& start) const
  {
    if (thelastobj == start) {
      if (theindex > 0) return thelastbnd;
    }
    const Standard_Integer index = themap.FindIndex (start);
    if (index > 0) {
      return themap.FindFromIndex(index);
    }
    return NULL;
  }
  
  //! Returns True if a Result (whatever its form) is Bound with a starting Object.
  //! I.e., if a Binder with a Result set, is linked with it
  Standard_EXPORT Standard_Boolean IsBound (const TheKeyType& start) const
  {
    Handle(Transfer_Binder) binder = Find(start);
    return (binder.IsNull()? Standard_False : binder->HasResult());
  }
  
  //! Creates a Link a starting Object with a Binder. This Binder
  //! can either bring a Result (effective Binding) or none (it can
  //! be set later : pre-binding).
  //! Considers a category number, by default 0
  Standard_EXPORT void Bind (const TheKeyType& start, const Handle(Transfer_Binder)& binder)
  {
    if (binder.IsNull()) return;
    Handle(Transfer_Binder) former = FindAndMask(start);
    if (!former.IsNull()) {
      // On admet VoidBinder : alors on reprend son Check
      if (former->DynamicType() == STANDARD_TYPE(Transfer_VoidBinder)) {
        binder->Merge(former);
        themap(theindex) = binder;                          // Substitution
      }
      else if (former->Status() == Transfer_StatusUsed) {
        StartTrace (former,start,thelevel,4);
        Transfer_TransferFailure::Raise ("TransferProcess : Bind, already Bound");
      }
      else {
        if (thetrace > 2) StartTrace (former,start,thelevel,5);
        binder->CCheck()->GetMessages (former->Check());
      }
    }
    if (theindex == 0 || thelastbnd.IsNull()) {
      if (theindex == 0) theindex = themap.Add(start,binder);  // Nouveau
      else themap(theindex) = binder;                          // idem en fait
      thelastbnd = binder;
    }
    else {
      thelastbnd  = binder;
      themap(theindex) = binder;
    }
  }
  
  //! Removes the Binder linked with a starting object
  //! If this Binder brings a non-empty Check, it is replaced by
  //! a VoidBinder. Also removes from the list of Roots as required.
  //! Returns True if done, False if <start> was not bound
  //! Considers a category number, by default 0
  Standard_EXPORT Standard_Boolean Unbind (const TheKeyType& start)
  {
    Handle(Transfer_Binder) former = FindAndMask(start);
    if (theindex == 0) return Standard_False;
    if (former.IsNull()) return Standard_False;
    if (former->DynamicType() == STANDARD_TYPE(Transfer_VoidBinder)) return Standard_True;
    themap(theindex) = thelastbnd;
    if(theroots.Contains(theindex)) {
      TColStd_IndexedMapOfInteger aNewRoots;
      for(Standard_Integer i = 1; i <= theroots.Extent(); i++)
        if(theindex!= theroots.FindKey(i))
          aNewRoots.Add(theroots.FindKey(i));
    
      theroots.Clear();
      theroots = aNewRoots;
    }

    thelastobj.Nullify();
    thelastbnd.Nullify();
    theindex = 0;
    return Standard_True;
  }

  //! Sets Messenger used for outputting messages.
  Standard_EXPORT void SetMessenger (const Handle(Message_Messenger)& messenger)
  { themessenger = messenger.IsNull()? Message::DefaultMessenger() : messenger; }
  
  //! Returns Messenger used for outputting messages.
  //! The returned object is guaranteed to be non-null;
  //! default is Message::Messenger().
  Standard_EXPORT const Handle(Message_Messenger) & Messenger() const { return themessenger; }

  //! Sets trace level used for outputting messages:
  //! <trace> = 0 : no trace at all
  //! <trace> = 1 : handled exceptions and calls to AddError
  //! <trace> = 2 : also calls to AddWarning
  //! <trace> = 3 : also traces new Roots
  //! (uses method ErrorTrace).
  //! Default is 1 : Errors traced
  Standard_EXPORT void SetTraceLevel (const Standard_Integer tracelev) { thetrace = tracelev; }

  //! Returns trace level used for outputting messages.
  Standard_EXPORT Standard_Integer TraceLevel() const { return thetrace; }
  
  //! New name for AddFail (Msg)
  Standard_EXPORT void SendFail (const TheKeyType& start, const Message_Msg& amsg) { AddFail(start,amsg); }

  //! New name for AddWarning (Msg)
  Standard_EXPORT void SendWarning (const TheKeyType& start, const Message_Msg& amsg) { AddWarning(start,amsg); }

  //! Adds an information message
  //! Trace is filled if trace level is at least 3
  Standard_EXPORT void SendMsg (const TheKeyType& start, const Message_Msg& amsg)
  {
    Handle(Transfer_Binder) binder = FindAndMask(start);
    if (binder.IsNull()) {
      binder = new Transfer_VoidBinder;
      Bind (start,binder);
    }
    // Alimente la trace : Regle causant (user messages)
    if (thetrace > 0) {
      StartTrace (binder,start,thelevel,6);
      themessenger << amsg.Value();
      if (amsg.IsEdited()&&thetrace>2)
        themessenger << " [from: " << amsg.Original() << "]";
      themessenger << endl;
    }
  }
  
  //! Adds an Error message to a starting entity (to the check of
  //! its Binder of category 0, as a Fail)
  Standard_EXPORT void AddFail (const TheKeyType& start, const Standard_CString mess, const Standard_CString orig = "")
  {
    Handle(Transfer_Binder) binder = FindAndMask(start);
    if (binder.IsNull()) {
      binder = new Transfer_VoidBinder;
      Bind (start,binder);
    }
    binder->AddFail (mess,orig);
    if (thetrace > 0) {
      StartTrace (binder,start,thelevel,1);
      themessenger << "    --> Fail : " << mess;
      if (orig[0] != '\0'&&thetrace>2) themessenger << " [from: " << orig << "]";
      themessenger << endl;
    }
  }
  
  //! (other name of AddFail, maintained for compatibility)
  Standard_EXPORT void AddError (const TheKeyType& start, const Standard_CString mess, const Standard_CString orig = "")
  { AddFail (start,mess,orig); }
  
  //! Adds an Error Message to a starting entity from the definition
  //! of a Msg (Original+Value)
  Standard_EXPORT void AddFail (const TheKeyType& start, const Message_Msg& amsg)
  {
    if (amsg.IsEdited()) AddFail (start,TCollection_AsciiString(amsg.Value()).ToCString(), TCollection_AsciiString(amsg.Original()).ToCString());
    else AddFail (start,TCollection_AsciiString(amsg.Value()).ToCString());
  }
  
  //! Adds a Warning message to a starting entity (to the check of
  //! its Binder of category 0)
  Standard_EXPORT void AddWarning (const TheKeyType& start, const Standard_CString mess, const Standard_CString orig = "")
  {
    Handle(Transfer_Binder) binder = FindAndMask(start);
    if (binder.IsNull()) {
      binder = new Transfer_VoidBinder;
      Bind (start,binder);
    }
    binder->AddWarning(mess,orig);
    if (thetrace > 1) {
      StartTrace (binder,start,thelevel,2);
      themessenger << "    --> Warning : " << mess;
      if (orig[0] != '\0'&&thetrace>2) themessenger << " [from: " << orig << "]";
      themessenger << endl;
    }
  }
  
  //! Adds a Warning Message to a starting entity from the definition
  //! of a Msg (Original+Value)
  Standard_EXPORT void AddWarning (const TheKeyType& start, const Message_Msg& amsg)
  {
    if (amsg.IsEdited()) AddWarning (start,TCollection_AsciiString(amsg.Value()).ToCString(), TCollection_AsciiString(amsg.Original()).ToCString());
    else AddWarning (start,TCollection_AsciiString(amsg.Value()).ToCString());
  }
  
  //! Returns the Check attached to a starting entity. If <start>
  //! is unknown, returns an empty Check
  //! Adds a case name to a starting entity
  //! Adds a case value to a starting entity
  //! Returns the complete case list for an entity. Null Handle if empty
  //! In the list of mapped items (between 1 and NbMapped),
  //! searches for the first item which follows <num0>(not included)
  //! and which has an attribute named <name>
  //! Attributes are brought by Binders
  //! Hence, allows such an iteration
  //!
  //! for (num = TP->NextItemWithAttribute(name,0);
  //! num > 0;
  //! num = TP->NextItemWithAttribute(name,num) {
  //! .. process mapped item <num>
  //! }
  //! Returns the type of an Attribute attached to binders
  //! If this name gives no Attribute, returns ParamVoid
  //! If this name gives several different types, returns ParamMisc
  //! Else, returns the effective type (ParamInteger, ParamReal,
  //! ParamIdent, or ParamText)
  //! Returns the list of recorded Attribute Names, as a Dictionary
  //! of Integer : each value gives the count of items which bring
  //! this attribute name
  //! By default, considers all the attribute names
  //! If <rootname> is given, considers only the attribute names
  //! which begin by <rootname>
  Standard_EXPORT Handle(Interface_Check) Check (const TheKeyType& start) const
  {
    const Handle(Transfer_Binder)& binder = Find(start);
    return (binder.IsNull()? NULL : binder->Check());
  }
  
  //! Binds a starting object with a Transient Result.
  //! Uses a SimpleBinderOfTransient to work. If there is already
  //! one but with no Result set, sets its Result.
  //! Considers a category number, by default 0
  Standard_EXPORT void BindTransient (const TheKeyType& start, const Handle(Standard_Transient)& res)
  {
    if (res.IsNull()) return;
    Handle(Transfer_SimpleBinderOfTransient) binder = Handle(Transfer_SimpleBinderOfTransient)::DownCast(Find(start));
    // Binding sur place ?
    if (!binder.IsNull() && binder->Status() == Transfer_StatusVoid) {
      binder->SetResult(res); return;
    }
    // Sinon, refaire
    binder = new Transfer_SimpleBinderOfTransient(res);
    Bind(start,binder);
  }
  
  //! Returns the Result of the Transfer of an object <start> as a
  //! Transient Result.
  //! Returns a Null Handle if there is no Transient Result
  //! Considers a category number, by default 0
  //! Warning : Supposes that Binding is done with a SimpleBinderOfTransient
  Standard_EXPORT Handle(Standard_Transient) FindTransient (const TheKeyType& start) const
  {
    Handle(Transfer_SimpleBinderOfTransient) binder = Handle(Transfer_SimpleBinderOfTransient)::DownCast(Find(start));
    if (binder.IsNull()) return NULL;
    if (!binder->HasResult()) return NULL;
    return binder->Result();
  }
  
  //! Searches for a transient result attached to a starting object,
  //! according to its type, by criterium IsKind(atype)
  //!
  //! In case of multiple result, explores the list and gives in
  //! <val> the first transient result IsKind(atype)
  //! Returns True and fills <val> if found
  //! Else, returns False (<val> is not touched, not even nullified)
  //!
  //! This syntactic form avoids to do DownCast : if a result is
  //! found with the good type, it is loaded in <val> and can be
  //! immediately used, well initialised
  Standard_EXPORT Standard_Boolean FindTypedTransient (const TheKeyType& start, const Handle(Standard_Type)& atype, Handle(Standard_Transient)& val) const
  { return GetTypedTransient (Find(start),atype,val); }
  
  //! Safe variant for arbitrary type of argument
  template <class T> 
  Standard_Boolean FindTypedTransient (const TheKeyType& start, const Handle(Standard_Type)& atype, Handle(T)& val) const
  {
    Handle(Standard_Transient) aVal = val;
    return FindTypedTransient (start, atype, aVal) && ! (val = Handle(T)::DownCast(aVal)).IsNull();
  }

  //! Searches for a transient result recorded in a Binder, whatever
  //! this Binder is recorded or not in <me>
  //!
  //! This is strictly equivalent to the class method GetTypedResult
  //! from class SimpleBinderOfTransient, but is just lighter to call
  //!
  //! Apart from this, works as FindTypedTransient
  Standard_EXPORT Standard_Boolean GetTypedTransient (const Handle(Transfer_Binder)& binder, const Handle(Standard_Type)& atype, Handle(Standard_Transient)& val) const
  { return Transfer_SimpleBinderOfTransient::GetTypedResult(binder,atype,val); }

  //! Safe variant for arbitrary type of argument
  template <class T> 
  Standard_Boolean GetTypedTransient (const Handle(Transfer_Binder)& start, const Handle(Standard_Type)& atype, Handle(T)& val) const
  {
    Handle(Standard_Transient) aVal = val;
    return GetTypedTransient (start, atype, aVal) && ! (val = Handle(T)::DownCast(aVal)).IsNull();
  }

  //! Returns the maximum possible value for Map Index
  //! (no result can be bound with a value greater than it)
  Standard_EXPORT Standard_Integer NbMapped() const { return themap.Extent(); }
  
  //! Returns the Starting Object bound to an Index,
  Standard_EXPORT const TheKeyType& Mapped (const Standard_Integer num) const { return themap.FindKey(num); }
  
  //! Returns the Index value bound to a Starting Object, 0 if none
  Standard_EXPORT Standard_Integer MapIndex (const TheKeyType& start) const { return themap.FindIndex(start); }
  
  //! Returns the Binder bound to an Index
  //! Considers a category number, by default 0
  Standard_EXPORT Handle(Transfer_Binder) MapItem (const Standard_Integer num) const { return themap.FindFromIndex(num); }
  
  //! Declares <obj> (and its Result) as Root. This status will be
  //! later exploited by RootResult, see below (Result can be
  //! produced at any time)
  Standard_EXPORT void SetRoot (const TheKeyType& start)
  {
    Standard_Integer index = MapIndex(start);
    if (index == 0)
      return;
    theroots.Add(index);
    if (thetrace > 2) StartTrace (MapItem(index),start,thelevel,3);
  }
  
  //! Enable (if <stat> True) or Disables (if <stat> False) Root
  //! Management. If it is set, Transfers are considered as stacked
  //! (a first Transfer commands other Transfers, and so on) and
  //! the Transfers commanded by an external caller are "Root".
  //! Remark : SetRoot can be called whatever this status, on every
  //! object.
  //! Default is set to True.
  Standard_EXPORT void SetRootManagement (const Standard_Boolean stat) { therootm = stat; }
  
  //! Returns the count of recorded Roots
  Standard_EXPORT Standard_Integer NbRoots() const { return theroots.Extent(); }
  
  //! Returns a Root Entity given its number in the list (1-NbRoots)
  Standard_EXPORT const TheKeyType& Root (const Standard_Integer num) const
  {
    Standard_Integer ind = 0;
    if (num > 0 && num <= theroots.Extent()) ind = theroots.FindKey(num);
    return themap.FindKey (ind);
  }
  
  //! Returns the Binder bound with a Root Entity given its number
  //! Considers a category number, by default 0
  Standard_EXPORT Handle(Transfer_Binder) RootItem (const Standard_Integer num) const
  {
    Standard_Integer ind = 0;
    if (num > 0 && num <= theroots.Extent()) ind = theroots.FindKey(num);
    return themap.FindFromIndex(ind);
  }
  
  //! Returns the index in the list of roots for a starting item,
  //! or 0 if it is not recorded as a root
  Standard_EXPORT Standard_Integer RootIndex (const TheKeyType& start) const
  {
    Standard_Integer index = MapIndex(start);
    if (index == 0) return 0;
    return theroots.FindIndex(index);
  }
  
  //! Returns Nesting Level of Transfers (managed by methods
  //! TranscriptWith & Co). Starts to zero. If no automatic Transfer
  //! is used, it remains to zero. Zero means Root Level.
  Standard_EXPORT Standard_Integer NestingLevel() const { return thelevel; }

  //! Resets Nesting Level of Transfers to Zero (Root Level),
  //! whatever its current value.
  Standard_EXPORT void ResetNestingLevel() { thelevel = 0; }
  
  //! Tells if <start> has been recognized as good candidate for
  //! Transfer. i.e. queries the Actor and its Nexts
  Standard_EXPORT Standard_Boolean Recognize (const TheKeyType& start) const
  {
    Handle(Actor) actor = theactor;
    // On balaie les Next jusqu a avoir un Resultat
    while (!actor.IsNull()) {
      if (actor->Recognize (start)) return Standard_True;
      actor = actor->Next();
    }
    return Standard_False;
  }
  
  //! Performs the Transfer of a Source Object.
  //! Mapping and Roots are managed : nothing is done if a Result is
  //! already Bound, an exception is raised in case of error.
  Standard_EXPORT Handle(Transfer_Binder) Transferring (const TheKeyType& theSource)
  {
    // Map deja alimentee ?
    Handle(Transfer_Binder) former = FindAndMask(theSource);

    //  ....    Transfert deja effectue avec Resultat ?    ....

    //  On considere que cette nouvelle demande de Transfert correspond donc a une
    //  utilisation en plus : noter "AlreadyUsed", donc resultat non modifiable
    if (!former.IsNull()) {
      if (former->HasResult()) {
        former->SetAlreadyUsed();
        return former;
      }

      //  ....    Etat Initial : peut-etre deja fait ... ou infaisable !
      Transfer_StatusExec statex = former->StatusExec();
      switch (statex) {
        case Transfer_StatusInitial :               // Transfert prepare a faire
          break;
        case Transfer_StatusDone :                  // Transfert deja fait
          themessenger << " .. and Transfer done" << endl;
          return former;
        case Transfer_StatusRun :                   // ca y est, on boucle
          former->SetStatusExec(Transfer_StatusLoop);
          return former;
        case Transfer_StatusError :                 // pas propre, ca ...
          if (thetrace) {
            themessenger << "                  *** Transfer in Error Status  :" << endl;
            StartTrace (former, theSource, thelevel,0);
          }
          else StartTrace (former,theSource,thelevel,4);
          Transfer_TransferFailure::Raise("TransferProcess : Transfer in Error Status");
        case Transfer_StatusLoop :                  // la boucle est bouclee ...
          if (thetrace) {
            themessenger << "                  *** Transfer  Head of Dead Loop  :" << endl;
            StartTrace (former,theSource,thelevel,0);
          }
          else StartTrace (former,theSource,thelevel,4);
          Transfer_TransferDeadLoop::Raise("TransferProcess : Transfer at Head of a Dead Loop");
      }

      //  ....    OK, on peut lancer l Execution
      former->SetStatusExec(Transfer_StatusRun);
    }

    Handle(Transfer_Binder) binder;
    Standard_Boolean newbind = Standard_False;

    //   Transfert sous protection pour les exceptions (pour notification en fait)
    const Standard_Integer oldlev = thelevel;
    try {
      OCC_CATCH_SIGNALS
      //! Invokes the Actor to work (set by SetActor), and tries its Nexts if no result is produced,
      //! until a Non Null Binder is produced.
      //! But keep in mind that a Null Binder can allways be returned
      //! if a Starting Entity has not been recognized at all.
      thelevel ++;             // si decremente et == 0, transfert racine

      Handle(Actor) actor = theactor;
      // On balaie les Next jusqu a avoir un Resultat
      while (!actor.IsNull()) {
        if (actor->Recognize (theSource)) binder = actor->Transferring(theSource,this);
        else binder.Nullify();
        if (!binder.IsNull()) break;
        actor = actor->Next();
      }

      if (!binder.IsNull()) {
        // Gestion du niveau racine (.. a regarder de pres ..)
        if (therootl == 0 && binder->StatusExec() == Transfer_StatusDone)
          therootl = thelevel - 1;
      }

      if (thelevel > 0) thelevel --;
    }

    //    ...  Exceptions a Rattraper : elles ne se ressemblent pas toutes ... !
    catch (Transfer_TransferDeadLoop) {
      if (binder.IsNull()) {
        themessenger << "                  *** Dead Loop with no Result" << endl;
        if (thetrace) StartTrace (binder,theSource,thelevel-1,0);
        binder = new Transfer_VoidBinder;
        Bind (theSource,binder);  newbind = Standard_True;
      } else if (binder->StatusExec() == Transfer_StatusLoop) {
        if (thetrace) {
          themessenger << "                  *** Dead Loop : Finding head of Loop :" << endl;
          StartTrace (binder,theSource,thelevel-1,0);
        }
        else StartTrace (binder,theSource,thelevel-1,4);
        Transfer_TransferFailure::Raise("TransferProcess : Head of Dead Loop");
        // Autrement dit, on change d exception (on est sorti de la boucle)
      } else {
        if (thetrace) {
          themessenger << "                  *** Dead Loop : Actor in Loop :" << endl;
          StartTrace (binder,theSource,thelevel-1,0);
        }
      }
      binder->AddFail("Transfer in dead Loop");
      thelevel = oldlev;
    }
    catch (Standard_Failure) {
      if (binder.IsNull()) {
        themessenger << "                  *** Exception Raised with no Result" << endl;
        binder = new Transfer_VoidBinder;
        Bind (theSource,binder);  newbind = Standard_True;
      }
      binder->AddFail("Transfer stopped by exception raising");
      if (thetrace) {
        themessenger << "    *** Raised : " << Standard_Failure::Caught()->GetMessageString() << endl;
        StartTrace (binder,theSource,thelevel-1,4);
      }
      thelevel = oldlev;
    }

    //  ....                Conclusion : Noter dans la Map                ....
    if (!newbind && !binder.IsNull()) {
      Bind(theSource,binder);
    }
    else {
      //= by ABV: 5 Oct 97: nothing generated, but former can be in run state - drop it
      //= ASK: may be set it to StatusInitial ?
      if ( !former.IsNull() ) former->SetStatusExec ( Transfer_StatusDone ); //+
      return NULL;    // Binder Null ... que faire d autre ?
    }

    //  ....                Gerer les Racines (si prevu)                ....
    if (therootl >= thelevel) {
      therootl = 0;
      if (therootm && binder->Status() != Transfer_StatusVoid) {
        SetRoot (theSource);
      }
    }
    return thelastbnd;
  }
  
  //! Same as Transferring but does not return the Binder.
  //! Simply returns True in case of success (for user call)
  Standard_EXPORT Standard_Boolean Transfer (const TheKeyType& theSource)
  {
    Handle(Transfer_Binder) binder = Transferring(theSource);
    return (!binder.IsNull());
  }
  
  //! Method called when trace is asked
  //! Calls PrintTrace to display information relevant for starting
  //! objects (which can be redefined)
  //! <level> is Nesting Level of Transfer (0 = root)
  //! <mode> controls the way the trace is done :
  //! 0 neutral, 1 for Error, 2 for Warning message, 3 for new Root
  Standard_EXPORT void StartTrace (const Handle(Transfer_Binder)& binder, const TheKeyType& start, const Standard_Integer level, const Standard_Integer mode) const
  {
    // ###  Fail (Roots:50)  --  Start start->DynamicType()
    // ###  Fail (Roots:50)  --  Start id:#label.. Type:start->DynamicType()
    if (thetrace > 3) {  // Internal to be switch when searching bug (trace >= 4)
      if (mode == 1) themessenger << "  ###  Fail";
      if (mode == 2) themessenger << "  ###  Warning";
      if (mode == 3) themessenger << "  ###  New Root n0 " << theroots.Extent();
      if (mode == 4) themessenger << "  ###  Exception";
      if (mode == 5) themessenger << "  ###  Substitution";
      if (mode == 6) themessenger << "  ###  Information";
      if (level > 1)
        themessenger << " (nested)";  // " at nesting Level:"<<level;
      if (mode >= 0 && mode != 3)
        themessenger << " at " << theroots.Extent() << " Roots";
    }
    if (!start.IsNull()) PrintTrace (start,themessenger);

    if (!binder.IsNull()) {
      Handle(Transfer_Binder) bnd = binder;
      Standard_Boolean hasres = Standard_False;
      while (!bnd.IsNull()) {
        if (bnd->Status() != Transfer_StatusVoid) {
          // ---  Result Type: binder->ResultType()  ---  Binder : binder->DynamicType();
          if (!hasres)
            themessenger << "\n  ---  Result Type : ";
          else 
            themessenger << " , ";
          themessenger << bnd->ResultTypeName();
          hasres = Standard_True;
        }
        bnd = bnd->NextResult();
      }
      if (!hasres && mode > 2) {
        themessenger << "\n  ---  No Result recorded";
      }
    }
    themessenger << endl;
  }
  
  //! Prints a short information on a source object.
  Standard_EXPORT virtual void PrintTrace (const TheKeyType& theSource, const Handle(Message_Messenger)& S) const
  {
    if (!theSource.IsNull()) {
      Handle(Transfer_Finder) aFinder = Handle(Transfer_Finder)::DownCast(theSource);
      S << " Type:" << (aFinder.IsNull()? theSource->DynamicType()->Name() : aFinder->ValueTypeName());
    }
  }
  
  //! Prints statistics on transfer
  void PrintStats(const Handle(Message_Messenger)& S) const
  {
    S<<"\n*******************************************************************\n";
    S << "********                 Basic Statistics                  ********"<<endl;

    if (themodel.IsNull())
      S<<"****        Model unknown"<<endl;
    else
      S<<"****        Nb Entities         : "<<themodel->NbEntities()<<endl;

    Standard_Integer nbr = 0, nbe = 0, nbw = 0;
    Standard_Integer i, max = NbMapped(), nbroots = NbRoots();
    S << "****        Nb Final Results    : "<<nbroots<<endl;

    for (i = 1; i <= max; i ++) {
      const Handle(Transfer_Binder)& binder = MapItem(i);
      if (binder.IsNull()) continue;
      const Handle(Interface_Check) ach = binder->Check();
      Transfer_StatusExec stat = binder->StatusExec();
      if (stat != Transfer_StatusInitial && stat != Transfer_StatusDone)
        nbe ++;
      else {
        if (ach->NbWarnings() > 0) nbw++;
        if (binder->HasResult())   nbr++;
      }
    }
    if (nbr > nbroots)
      S<<"****      ( Itermediate Results : "<<nbr-nbroots<<" )\n";
    if (nbe > 0)
      S<<"****                  Errors on : "<<Interface_MSG::Blanks(nbe,4)<<nbe<<" Entities\n";
    if (nbw > 0)
      S<<"****                Warnings on : "<<Interface_MSG::Blanks(nbw,4)<<nbw<<" Entities\n";
    S<<"*******************************************************************\n";
  }

  //! Returns, as an iterator, the log of root transfer, i.e. the
  //! created objects and Binders bound to starting roots
  //! If withstart is given True, Starting Objets are also returned
  Standard_EXPORT Iterator RootResult (const Standard_Boolean withstart = Standard_False) const
  {
    Iterator iter(withstart);
    const Standard_Integer max = theroots.Extent();
    for (Standard_Integer j = 1; j <= max; j ++) {
      const Standard_Integer i = theroots.FindKey(j);
      Handle(Transfer_Binder) binder = MapItem(i);
      if (binder.IsNull()) continue;
      if (withstart) iter.Add (binder,Mapped(i));
      else iter.Add (binder);
    }
    return iter;
  }
  
  //! Returns, as an Iterator, the entire log of transfer (list of
  //! created objects and Binders which can bring errors)
  //! If withstart is given True, Starting Objets are also returned
  Standard_EXPORT Iterator CompleteResult (const Standard_Boolean withstart = Standard_False) const
  {
    Iterator iter(withstart);
    const Standard_Integer max = NbMapped();
    for (Standard_Integer i = 1; i <= max; i ++) {
      Handle(Transfer_Binder) binder = MapItem(i);
      if (binder.IsNull()) continue;
      if (withstart) iter.Add (binder,Mapped(i));
      else iter.Add (binder);
    }
    return iter;
  }
  
  //! Returns Binders which are neither "Done" nor "Initial",
  //! that is Error,Loop or Run (abnormal states at end of Transfer)
  //! Starting Objects are given in correspondance in the iterator
  Standard_EXPORT Iterator AbnormalResult() const
  {
    Iterator iter(Standard_True);
    const Standard_Integer max = NbMapped();
    for (Standard_Integer i = 1; i <= max; i ++) {
      Handle(Transfer_Binder) binder = MapItem(i);
      if (binder.IsNull()) continue;
      Transfer_StatusExec statex = binder->StatusExec();
      if (statex != Transfer_StatusInitial && statex != Transfer_StatusDone)
        iter.Add (binder,Mapped(i));  // on note les cas "pas normaux"
    }
    return iter;
  }
  
  //! Returns a CheckList as a list of Check : each one is for a
  //! starting entity which have either check (warning or fail)
  //! messages are attached, or are in abnormal state : that case
  //! gives a specific message
  //! If <erronly> is True, checks with Warnings only are ignored
  Standard_EXPORT Interface_CheckIterator CheckList (const Standard_Boolean erronly) const
  {
    Interface_CheckIterator list;
    Standard_Integer num, max = NbMapped();
    for (Standard_Integer i = 1; i <= max; i ++) {
      Handle(Transfer_Binder) binder = MapItem(i);
      if (binder.IsNull()) continue;
      Transfer_StatusExec statex = binder->StatusExec();
      Handle(Interface_Check) check = binder->Check();
      if (statex != Transfer_StatusInitial && statex != Transfer_StatusDone && !check->HasFailed())
        check->AddFail("Transfer in Abnormal Status (!= Initial or Done)");
      if (!check->HasFailed() && (erronly || check->NbWarnings() == 0)) continue;
      const TheKeyType& ent = Mapped(i);
      num = CheckNum(ent);
      if (num == 0) num = i;
      check->SetEntity(ent);
      list.Add(check,num);
    }
    return list;
  }
  
  //! Sets Progress indicator
  void SetProgress (const Handle(Message_ProgressIndicator)& theProgress) { myProgress = theProgress; }
  
  //! Gets Progress indicator
  const Handle(Message_ProgressIndicator) & GetProgress() const { return myProgress; }

  DEFINE_STANDARD_RTTI_INLINE(Transfer_ProcessGen,MMgt_TShared)

 protected:
  
  //! Computes a number to be associated to a starting object in
  //! a check or a check-list
  //! By default, returns 0; can be redefined
  Standard_EXPORT virtual Standard_Integer CheckNum (const TheKeyType&) const { return 0; }

  Handle(Interface_InterfaceModel) themodel;

 private:
  
  //! Same as Find but stores the last access to the map, for a
  //! faster access on next calls (as Bind does too)
  Standard_EXPORT const Handle(Transfer_Binder) & FindAndMask (const TheKeyType& start)
  {
    if (thelastobj == start) {
      if (theindex > 0) return thelastbnd;
    }
    thelastobj = start;
    theindex   = themap.FindIndex (start);
    if (theindex > 0) thelastbnd = themap.FindFromIndex(theindex);
    else thelastbnd.Nullify();
    return thelastbnd;
  }
  
  Standard_Integer thetrace;
  Handle(Message_Messenger) themessenger;
  Standard_Integer thelevel;
  Standard_Integer therootl;
  Standard_Boolean therootm;
  TColStd_IndexedMapOfInteger theroots;
  TheKeyType thelastobj;
  Handle(Transfer_Binder) thelastbnd;
  Standard_Integer theindex;
  Handle(Actor) theactor;
  Transfer_MapOfProcess themap;
  Handle(Message_ProgressIndicator) myProgress;
};

#endif // _Transfer_ProcessGen_HeaderFile
