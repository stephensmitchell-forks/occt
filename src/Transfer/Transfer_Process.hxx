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

#ifndef _Transfer_Process_HeaderFile
#define _Transfer_Process_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Type.hxx>
#include <MMgt_TShared.hxx>

#include <NCollection_IndexedDataMap.hxx>
#include <TColStd_IndexedMapOfInteger.hxx>
#include <Message.hxx>
#include <Transfer_Binder.hxx>
#include <Transfer_Hasher.hxx>

class TColStd_HSequenceOfInteger;
class TColStd_HSequenceOfTransient;
class Message_Msg;
class Message_Messenger;
class Interface_Check;
class Interface_CheckIterator;
class Interface_InterfaceModel;
class Transfer_HSequenceOfBinder;
class Transfer_Binder;


class Transfer_Process : public MMgt_TShared
{
 public:

  typedef NCollection_IndexedDataMap<Handle(Standard_Transient),Handle(Transfer_Binder),Transfer_Hasher> Transfer_MapOfProcess;

  class Actor : public MMgt_TShared
  {
   public:
  
    Actor() : themodetrans(0) {}

    //! Prerequesite for Transfer : the method Transfer is called on a starting object only if Recognize has returned True on it.
    //! This allows to define a list of Actors, each one processing a definite kind of data
    //! Transfer_Process calls Recognize on each one before calling Transfer.
    //! But even if Recognize has returned True, Transfer can reject by returning a Null Binder (afterwards rejection),
    //! the next actor is then invoked
    //!
    //! The provided default returns True, can be redefined
    Standard_EXPORT virtual Standard_Boolean Recognize (const Handle(Standard_Transient)& /*start*/) { return Standard_True; }
  
    //! Specific action of Transfer. The Result is stored in
    //! the returned Binder, or a Null Handle for "No result"
    //! (Default defined as doing nothing; should be deffered)
    //! "mutable" allows the Actor to record intermediate
    //! information, in addition to those of TransferProcess
    Standard_EXPORT virtual Handle(Transfer_Binder) Transferring (const Handle(Standard_Transient)& /*start*/, const Handle(Transfer_Process)& /*TP*/) { return NULL; }
  
    //! Returns min and max values for transfer mode
    //! Returns True if bounds are set, False else (i.e., always the same mode)
    Standard_EXPORT virtual Standard_Boolean TransferModeBounds (Standard_Integer& /*theMin*/, Standard_Integer& /*theMax*/) const { return Standard_False; }
  
    //! Returns the help string recorded for a given transfer mode
    //! Empty string if help not defined or not within bounds or if values are free
    Standard_EXPORT virtual Standard_CString TransferModeHelp (const Standard_Integer /*theMode*/) const { return ""; }

    //! Set a specific transfer mode
    void SetTransferMode (const Standard_Integer theMode) { themodetrans = theMode; }

    DEFINE_STANDARD_RTTI_INLINE(Actor,MMgt_TShared)

   protected:

    Standard_Integer themodetrans;
  };

  class Iterator
  {
   public:

    DEFINE_STANDARD_ALLOC
  
    //! Creates an empty Iterator
    //! If WithSource is True, each Binder to be iterated will
    //! be associated to its corresponding Source Object
    Standard_EXPORT Iterator(const Standard_Boolean WithSource);
  
    //! Adds a Binder to the iteration list (construction)
    //! with no corresponding Starting Object
    //! (note that Result is brought by Binder)
    Standard_EXPORT void Add (const Handle(Transfer_Binder)& theBinder);
  
    //! Adds a Binder to the iteration list, associated with
    //! its corresponding Starting Object "start"
    //! Starting Object is ignored if not required at
    //! Creation time
    Standard_EXPORT void Add (const Handle(Transfer_Binder)& theBinder, const Handle(Standard_Transient)& theSource);
  
    //! After having added all items, keeps or rejects items
    //! which are attached to starting data given by <list>
    //! <keep> = True (D) : keeps. <keep> = False : rejects
    //! Does nothing if <withstarts> was False
    Standard_EXPORT void Filter (const Handle(TColStd_HSequenceOfTransient)& theList);
  
    //! Returns count of Binders to be iterated
    Standard_EXPORT Standard_Integer Number() const;
  
    //! Clears Iteration in progress, to allow it to be restarted
    void Start() { myCurrent = 0; Next(); }
  
    //! Returns True if there are other Items to iterate
    Standard_EXPORT Standard_Boolean More();
  
    //! Sets Iteration to the next Item
    Standard_EXPORT void Next();
  
    //! Returns the current Binder
    Standard_EXPORT const Handle(Transfer_Binder)& Value() const;

    //! Returns corresponding Source Object
    Standard_EXPORT const Handle(Standard_Transient) & SourceObject() const;

   private:
  
    //! Adds a Binder to the iteration list (construction)
    Standard_EXPORT void AddItem (const Handle(Transfer_Binder)& atr);

    Standard_Integer myCurrent;
    Handle(Transfer_HSequenceOfBinder) myBinders;
    Handle(TColStd_HSequenceOfInteger) mySelected;
    Handle(TColStd_HSequenceOfTransient) mySourceObjects;
  };

 public:

  //! Sets TransferProcess at initial state. Gives an Initial size
  //! (indicative) for the Map when known (default is 10000).
  //! Sets default trace file as a printer and default trace level
  //! (see Message_TraceFile).
  Transfer_Process(const Standard_Integer nb = 10000)
  : themessenger(Message::DefaultMessenger()),
    thetrace(0),
    thelevel(0), therootl(0),
    therootm(Standard_False),
    theindex(0),
    myMap(nb)
  {}
  
  //! Sets TransferProcess at initial state. Gives an Initial size
  //! (indicative) for the Map when known (default is 10000).
  //! Sets a specified printer.
  Transfer_Process(const Handle(Message_Messenger)& messenger, const Standard_Integer nb = 10000)
  : themessenger(messenger.IsNull()? Message::DefaultMessenger() : messenger),
    thetrace(1),
    thelevel(0), therootl(0),
    therootm(Standard_False),
    theindex(0),
    myMap(nb)
  {}
  
  //! Sets an InterfaceModel, which can be used during transfer
  //! for instance if a context must be managed, it is in the Model
  void SetModel (const Handle(Interface_InterfaceModel)& theModel) { myModel = theModel; }
  
  //! Returns the Model which can be used for context
  const Handle(Interface_InterfaceModel)& Model() const { return myModel; }
  
  //! Resets a TransferProcess as ready for a completely new work.
  //! Clears general data (roots) and the Map
  void Clear()
  {
    thelevel = 0; therootl  = 0;
    theroots.Clear();
    myMap.Clear();
    theindex = 0; thelastobj.Nullify(); thelastbnd.Nullify();
  }
  
  //! Rebuilds the Map and the roots to really remove Unbound items
  //! Because Unbind keeps the entity in place, even if not bound
  //! Hence, working by checking new items is meaningless if a
  //! formerly unbound item is rebound
  Standard_EXPORT void Clean();
  
  //! Resizes the Map as required (if a new reliable value has been
  //! determined). Acts only if <nb> is greater than actual NbMapped
  void Resize (const Standard_Integer nb) { if (nb > myMap.NbBuckets()) myMap.ReSize(nb); }

  //! Defines an Actor, which is used for automatic Transfer
  void SetActor (const Handle(Actor)& theActor) { myActor = theActor; }

  //! Returns an Actor, which is used for automatic Transfer
  const Handle(Actor)& GetActor () const { return myActor; }

  //! Returns the Binder which is linked with a starting Object
  //! It can either bring a Result (Transfer done) or none (for a
  //! pre-binding).
  //! If no Binder is linked with <start>, returns a Null Handle
  //! Considers a category number, by default 0
  Standard_EXPORT Handle(Transfer_Binder) Find (const Handle(Standard_Transient) &start) const;
  
  //! Returns True if a Result (whatever its form) is Bound with a starting Object.
  //! I.e., if a Binder with a Result set, is linked with it
  Standard_EXPORT Standard_Boolean IsBound (const Handle(Standard_Transient) &start) const;
  
  //! Creates a Link a starting Object with a Binder. This Binder
  //! can either bring a Result (effective Binding) or none (it can
  //! be set later : pre-binding).
  //! Considers a category number, by default 0
  Standard_EXPORT void Bind (const Handle(Standard_Transient) &start, const Handle(Transfer_Binder)& binder);
  
  //! Removes the Binder linked with a starting object
  //! If this Binder brings a non-empty Check, it is replaced by
  //! a VoidBinder. Also removes from the list of Roots as required.
  //! Returns True if done, False if <start> was not bound
  //! Considers a category number, by default 0
  Standard_EXPORT Standard_Boolean Unbind (const Handle(Standard_Transient) &start);

  //! Sets Messenger used for outputting messages.
  void SetMessenger (const Handle(Message_Messenger)& messenger)
  { themessenger = messenger.IsNull()? Message::DefaultMessenger() : messenger; }
  
  //! Returns Messenger used for outputting messages.
  //! The returned object is guaranteed to be non-null;
  //! default is Message::Messenger().
  const Handle(Message_Messenger) & Messenger() const { return themessenger; }

  //! Sets trace level used for outputting messages:
  //! <trace> = 0 : no trace at all
  //! <trace> = 1 : handled exceptions and calls to AddError
  //! <trace> = 2 : also calls to AddWarning
  //! <trace> = 3 : also traces new Roots
  //! (uses method ErrorTrace).
  //! Default is 1 : Errors traced
  void SetTraceLevel (const Standard_Integer tracelev) { thetrace = tracelev; }

  //! Returns trace level used for outputting messages.
  Standard_Integer TraceLevel() const { return thetrace; }
  
  //! New name for AddFail (Msg)
  void SendFail (const Handle(Standard_Transient) &start, const Message_Msg& amsg) { AddFail(start,amsg); }

  //! New name for AddWarning (Msg)
  void SendWarning (const Handle(Standard_Transient) &start, const Message_Msg& amsg) { AddWarning(start,amsg); }
  
  //! Adds an Error message to a starting entity (to the check of
  //! its Binder of category 0, as a Fail)
  Standard_EXPORT void AddFail (const Handle(Standard_Transient) &start, const Standard_CString mess, const Standard_CString orig = "");
  
  //! (other name of AddFail, maintained for compatibility)
  void AddError (const Handle(Standard_Transient) &start, const Standard_CString mess, const Standard_CString orig = "")
  { AddFail (start,mess,orig); }
  
  //! Adds an Error Message to a starting entity from the definition
  //! of a Msg (Original+Value)
  Standard_EXPORT void AddFail (const Handle(Standard_Transient) &start, const Message_Msg& amsg);
  
  //! Adds a Warning message to a starting entity (to the check of
  //! its Binder of category 0)
  Standard_EXPORT void AddWarning (const Handle(Standard_Transient) &start, const Standard_CString mess, const Standard_CString orig = "");
  
  //! Adds a Warning Message to a starting entity from the definition
  //! of a Msg (Original+Value)
  Standard_EXPORT void AddWarning (const Handle(Standard_Transient) &start, const Message_Msg& amsg);
  
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
  Standard_EXPORT Handle(Interface_Check) Check (const Handle(Standard_Transient) &start) const;
  
  //! Binds a starting object with a Transient Result.
  //! Uses a SimpleBinderOfTransient to work. If there is already
  //! one but with no Result set, sets its Result.
  //! Considers a category number, by default 0
  Standard_EXPORT void BindTransient (const Handle(Standard_Transient) &start, const Handle(Standard_Transient)& res);
  
  //! Returns the Result of the Transfer of an object <start> as a
  //! Transient Result.
  //! Returns a Null Handle if there is no Transient Result
  //! Considers a category number, by default 0
  //! Warning : Supposes that Binding is done with a SimpleBinderOfTransient
  Standard_EXPORT Handle(Standard_Transient) FindTransient (const Handle(Standard_Transient) &start) const;
  
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
  Standard_Boolean FindTypedTransient (const Handle(Standard_Transient) &start, const Handle(Standard_Type)& atype, Handle(Standard_Transient)& val) const
  { return GetTypedTransient (Find(start),atype,val); }
  
  //! Safe variant for arbitrary type of argument
  template <class T> 
  Standard_Boolean FindTypedTransient (const Handle(Standard_Transient) &start, const Handle(Standard_Type)& atype, Handle(T)& val) const
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
  Standard_EXPORT Standard_Boolean GetTypedTransient (const Handle(Transfer_Binder)& binder, const Handle(Standard_Type)& atype, Handle(Standard_Transient)& val) const;

  //! Safe variant for arbitrary type of argument
  template <class T> 
  Standard_Boolean GetTypedTransient (const Handle(Transfer_Binder)& start, const Handle(Standard_Type)& atype, Handle(T)& val) const
  {
    Handle(Standard_Transient) aVal = val;
    return GetTypedTransient (start, atype, aVal) && ! (val = Handle(T)::DownCast(aVal)).IsNull();
  }

  //! Returns the maximum possible value for Map Index
  //! (no result can be bound with a value greater than it)
  Standard_Integer NbMapped() const { return myMap.Extent(); }
  
  //! Returns the Starting Object bound to an Index,
  const Handle(Standard_Transient) & Mapped (const Standard_Integer num) const { return myMap.FindKey(num); }
  
  //! Returns the Index value bound to a Starting Object, 0 if none
  Standard_Integer MapIndex (const Handle(Standard_Transient) &start) const { return myMap.FindIndex(start); }
  
  //! Returns the Binder bound to an Index
  //! Considers a category number, by default 0
  Handle(Transfer_Binder) MapItem (const Standard_Integer num) const { return myMap.FindFromIndex(num); }
  
  //! Declares <obj> (and its Result) as Root. This status will be
  //! later exploited by RootResult, see below (Result can be
  //! produced at any time)
  void SetRoot (const Handle(Standard_Transient) &start)
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
  void SetRootManagement (const Standard_Boolean stat) { therootm = stat; }

  //! Returns the count of recorded Roots
  Standard_Integer NbRoots() const { return theroots.Extent(); }
  
  //! Returns a Root Entity given its number in the list (1-NbRoots)
  Standard_EXPORT const Handle(Standard_Transient) & Root (const Standard_Integer num) const
  {
    Standard_Integer ind = 0;
    if (num > 0 && num <= theroots.Extent()) ind = theroots.FindKey(num);
    return myMap.FindKey (ind);
  }
  
  //! Returns the index in the list of roots for a starting item,
  //! or 0 if it is not recorded as a root
  Standard_EXPORT Standard_Integer RootIndex (const Handle(Standard_Transient) &start) const
  {
    Standard_Integer index = MapIndex(start);
    if (index == 0) return 0;
    return theroots.FindIndex(index);
  }
  
  //! Performs the Transfer of a Source Object.
  //! Mapping and Roots are managed : nothing is done if a Result is
  //! already Bound, an exception is raised in case of error.
  Standard_EXPORT Handle(Transfer_Binder) Transferring (const Handle(Standard_Transient) &theSource);
  
  //! Same as Transferring but does not return the Binder.
  //! Simply returns True in case of success (for user call)
  Standard_Boolean Transfer (const Handle(Standard_Transient) &theSource)
  {
    Handle(Transfer_Binder) binder = Transferring(theSource);
    return (!binder.IsNull());
  }
  
  //! Method called when trace is asked
  //! Calls PrintTrace to display information relevant for source objects (which can be redefined)
  //! <theLevel> is Nesting Level of Transfer (0 = root)
  //! <theMode> controls the way the trace is done :
  //! 0 neutral, 1 for Error, 2 for Warning message, 3 for new Root
  Standard_EXPORT void StartTrace (const Handle(Transfer_Binder)& theBinder, const Handle(Standard_Transient) &theSource, const Standard_Integer theLevel, const Standard_Integer theMode) const;
  
  //! Prints statistics on transfer
  Standard_EXPORT void PrintStats(const Handle(Message_Messenger)& S) const;

  //! Returns, as an iterator, the log of root transfer, i.e. the
  //! created objects and Binders bound to starting roots
  //! If withstart is given True, Starting Objets are also returned
  Standard_EXPORT Iterator RootResult (const Standard_Boolean withstart = Standard_False) const;
  
  //! Returns, as an Iterator, the entire log of transfer (list of
  //! created objects and Binders which can bring errors)
  //! If withstart is given True, Starting Objets are also returned
  Standard_EXPORT Iterator CompleteResult (const Standard_Boolean withstart = Standard_False) const;
  
  //! Returns Binders which are neither "Done" nor "Initial",
  //! that is Error,Loop or Run (abnormal states at end of Transfer)
  //! Starting Objects are given in correspondance in the iterator
  Standard_EXPORT Iterator AbnormalResult() const;
  
  //! Returns a CheckList as a list of Check : each one is for a
  //! starting entity which have either check (warning or fail)
  //! messages are attached, or are in abnormal state : that case
  //! gives a specific message
  //! If <erronly> is True, checks with Warnings only are ignored
  Standard_EXPORT Interface_CheckIterator CheckList (const Standard_Boolean erronly) const;
  
  //! Prints the transfer status of a transferred item, as beeing the Mapped n0 <num>
  //! Returns True when done, False else (i.e. num out of range)
  Standard_EXPORT virtual Standard_Boolean PrintTransferStatus (const Standard_Integer theNum, const Handle(Message_Messenger)& theMessenger) const = 0;
  
  //! Sets Progress indicator
  void SetProgress (const Handle(Message_ProgressIndicator)& theProgress) { myProgress = theProgress; }
  
  //! Gets Progress indicator
  const Handle(Message_ProgressIndicator) & GetProgress() const { return myProgress; }

  DEFINE_STANDARD_RTTIEXT(Transfer_Process,MMgt_TShared)

 protected:

  //! Adds an information message
  //! Trace is filled if trace level is at least 3
  Standard_EXPORT void SendMsg (const Handle(Standard_Transient) &start, const Message_Msg& amsg);
  
  //! Computes a number to be associated to a starting object in
  //! a check or a check-list
  //! By default, returns 0; can be redefined
  Standard_EXPORT virtual Standard_Integer CheckNum (const Handle(Standard_Transient) &) const { return 0; }
  
  //! Prints a short information on a source object.
  Standard_EXPORT virtual void PrintTrace (const Handle(Standard_Transient) &theSource, const Handle(Message_Messenger)& S) const;

  Handle(Interface_InterfaceModel) myModel;
  Handle(Actor) myActor;

 private:
  
  //! Same as Find but stores the last access to the map, for a
  //! faster access on next calls (as Bind does too)
  const Handle(Transfer_Binder) & FindAndMask (const Handle(Standard_Transient) &start)
  {
    if (thelastobj == start) {
      if (theindex > 0) return thelastbnd;
    }
    thelastobj = start;
    theindex   = myMap.FindIndex (start);
    if (theindex > 0) thelastbnd = myMap.FindFromIndex(theindex);
    else thelastbnd.Nullify();
    return thelastbnd;
  }
  
  Transfer_MapOfProcess myMap;

  Standard_Integer thetrace;
  Standard_Integer thelevel;
  Standard_Integer therootl;
  Standard_Boolean therootm;
  TColStd_IndexedMapOfInteger theroots;
  Handle(Standard_Transient) thelastobj;
  Handle(Transfer_Binder) thelastbnd;
  Standard_Integer theindex;

  Handle(Message_Messenger) themessenger;
  Handle(Message_ProgressIndicator) myProgress;
};

#endif // _Transfer_Process_HeaderFile
