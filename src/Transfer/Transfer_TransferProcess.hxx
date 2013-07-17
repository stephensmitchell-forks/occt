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

#ifndef _Transfer_TransferProcess_HeaderFile
#define _Transfer_TransferProcess_HeaderFile

#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <Interface_Check.hxx>
#include <Interface_CheckIterator.hxx>

#include <Message.hxx>
#include <Message_Messenger.hxx>
#include <Message_Msg.hxx>
#include <Message_ProgressIndicator.hxx>
#include <Transfer_VoidBinder.hxx>
#include <Transfer_SimpleBinderOfTransient.hxx>
#include <Transfer_MultipleBinder.hxx>
#include <Transfer_StatusResult.hxx>
#include <Transfer_TransferFailure.hxx>
#include <Transfer_TransferDeadLoop.hxx>
#include <Transfer_Binder.hxx>

#include <TCollection_HAsciiString.hxx>
#include <Dico_DictionaryOfTransient.hxx>
#include <Dico_IteratorOfDictionaryOfTransient.hxx>
#include <TColStd_MapIteratorOfMapOfInteger.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <TColStd_IndexedMapOfInteger.hxx>

    //!           This class gives the frame for doing a direct transfer : it
    //!           provides required basic tools, such as a Map, and automatisms
    //!           which simplify work, while their use is not mandatory.
    //!           
    //!           Starting Objects managed by Mapping can be Transient or any
    //!           (by two instantiations provided by the package,
    //!           TransientProcess and FinderProcess, see class Finder).
    //!           
    //!           A direct Transfer must maintain data sharing whenever possible
    //!           (that is, when described by both sides) : this is managed by a
    //!           Map. It works with intermediate Objects, the Binders. Hence it
    //!           allows a binding,  one/several-one/several, and it controls
    //!           that a Resulting Data can be used by another one only
    //!           if its creation has been ended. Also it keeps error status.
    //!           
    //!           Several results can be bound to a starting object, identified
    //!           by a category number. The default one (the only one considered
    //!           for final analysis) is zero. But mapping accesses allow to
    //!           precise a category number different from zero
    //!           
    //!           In order to work more easily in most cases, automatisms are
    //!           offered. Classically, to transfer an object, its result is
    //!           computed then bound. This applies recursively for referenced
    //!           objects. Each Transfer of a kind of object can be performed
    //!           by a specific Tool, which can use TransferProcess to work.
    //!           
    //!           
    //!           Many often, a specific Transfer can work as : "Search the good
    //!           Tool to Perform the Transfer (if not provided), run the Tool,
    //!           then Bind the Result", where "Perform" is seen as a black box.
    //!           
    //!           This way of working is assumed through automatisms which use
    //!           two types of auxiliary objects :
    //!           - An Actor acts as a Library of Transfer Actions :
    //!             it recognizes the operation to perform on a starting object,
    //!             then runs it (see class Actor)
    //!           - it produces a Binder, which is then recorded in the Map
    //!           - of course, if the Transfer has been already done, it is not
    //!             run once more : the first Result is available
    //!           - it is possible to re-evaluate the Result if required (anI.e.
    //!             if the definitive Result is known only once other Transfers
    //!             have been engaged)
    //!           - in the case of a second (or more) ask for Transfer, the
    //!             Result is considered as "AlreadyUsed", hence it may not be
    //!             changed (in the Binder itself or by Rebinding another one)
    //!           
    //!           This gives a top-down process, in which first object to be
    //!           transferred asks for another one it references to be, etc...
    //!           This allows automatic management of Roots : only first
    //!           transfers (asked by the user) can be Root Transfers.
    //!           
    //!             
    //!           Other features are : Management of Root Entities (the Entities
    //!           by which the Transfer has started, and their Results, can be
    //!           identified as Roots, by the user or by automatisms); Error
    //!           Handling (if automatisms are used); 
    //!           Returning Results, which can be specialized for
    //!           Roots, or Erroneous Data, or complete/partial (for one entity)
    //!           
    //!           Finally, TransferProcess inherits TShared : this allows to
    //!           run successive steps on the same set of data attached to a
    //!           given Transfer, by easily transmitting this set.
    
template <class ActorHandle,
          class TransferMap,
          class StartObject,
          class MapHasher,
          class ListHandle,
          class Iterator>
class Transfer_TransferProcess : public MMgt_TShared
{
private:
  Handle(Standard_Transient) myNullTrans;
protected:
  //! To handle or not exception raisings
  Standard_Boolean myToHandleErr; 
  //! Trace theLevel (exceptions,errors, etc...)
  Standard_Integer myTrace;
  //! Messenger for sending messages
  Handle(Message_Messenger) myMessenger;

  Standard_Integer myLevel;
  //! Level of root at each time (not allways 1)
  Standard_Integer myRootLevel;
  //! Flag for Root Management set or not
  Standard_Boolean myToManageRoot;
  //! Indices of roots in the map
  TColStd_IndexedMapOfInteger myRoots;

  //! Last Starting Object Bound
  StartObject myLastObj;
  //! Its attached Binder (noted to optimize)
  Handle(Transfer_Binder) myLastBinder;
  Standard_Integer  myIndex;
  ActorHandle myActor;
  TransferMap myMap;
  //! Progress indicator
  Handle(Message_ProgressIndicator) myProgress;

private:

  
  Standard_EXPORT Handle(Transfer_Binder) TransferProduct (const StartObject& start) ;
public:
  //! Sets TransferProcess at initial state. Gives an Initial size
  //! (indicative) for the Map when known (default is 10000).
  //! Sets default trace file as a printer and default trace level
  //! (see Message_TraceFile).
  Standard_EXPORT Transfer_TransferProcess (const Standard_Integer theNb = 10000);
  
  //! Sets TransferProcess at initial state. Gives an Initial size
  //! (indicative) for the Map when known (default is 10000).
  //! Sets a specified printer.
  Standard_EXPORT Transfer_TransferProcess (const Handle(Message_Messenger)& myMessenger,
                                            const Standard_Integer theNb = 10000); 
  
  //! Same as Find but stores the last access to the map, for a
  //! faster access on next calls (as Bind does too).
  //! Considers a category number, by default 0.
  Standard_EXPORT Handle(Transfer_Binder) FindAndMask (const StartObject& theStart);

  //! Resets a TransferProcess as ready for a completely new work.
  //! Clears general data (roots) and the Map.
  Standard_EXPORT  void Clear ();
  
  //! Rebuilds the Map and the roots to really remove Unbound items
  //! Because Unbind keeps the entity in place, even if not bound
  //! Hence, working by checking new items is meaningless if a
  //! formerly unbound item is rebound.
  Standard_EXPORT void Clean ();
  
  //! Resizes the Map as required (if a new reliable value has been
  //! determined). Acts only if <nb> is greater than actual NbMapped.
  Standard_EXPORT void Resize (const Standard_Integer theNb);
  
  //! Defines an Actor, which is used for automatic Transfer
  //! If already defined, the new Actor is cumulated
  //! (see SetNext from Actor).
  Standard_EXPORT void  SetActor(const ActorHandle& theActor);
  
  //! Returns the defined Actor. Returns a Null Handle if not set.
  Standard_EXPORT ActorHandle Actor () const;
  
  //! Fine Access to Map
  //! This access works on : Binder (default), or Handle Result
  //! (Transient,Finder) as privileged kinds of Result
  
  //! Returns the Binder which is linked with a starting Object
  //! It can either bring a Result (Transfer done) or none (for a
  //! pre-binding).
  //! If no Binder is linked with <start>, returns a Null Handle
  //! Considers a category number, by default 0.
  //! Warning : it is not equivalent to IsBound, which demands a Result
  Standard_EXPORT Handle(Transfer_Binder) Find (const StartObject& theStart) const;
  
  //! Returns True if a Result (whatever its form) is Bound with
  //! a starting Object. I.e., if a Binder with a Result set, is linked with it.
  //! Considers a category number, by default 0.
  Standard_EXPORT Standard_Boolean IsBound (const StartObject& theStart) const;
  
  //! Returns True if the result of the transfer of an object is
  //! already used in other ones. If it is, Rebind cannot change it.
  //! Considers a category number, by default 0.
  Standard_EXPORT Standard_Boolean IsAlreadyUsed (const StartObject& theStart) const;
    
  //! Creates a Link a starting Object with a Binder. This Binder
  //! can either bring a Result (effective Binding) or none (it can
  //! be set later : pre-binding).
  //! Considers a category number, by default 0.
  //! Error if a Binder with a Result set is already linked with
  //! this Object (Binder with StatusResult not Initial).
  Standard_EXPORT void Bind (const StartObject& theStart,
                             const Handle(Transfer_Binder)& theBinder);
  
  //! Changes the Binder linked with a starting Object for its
  //! unitary transfer. This it can be useful when the exact form
  //! of the result is known once the transfer is widely engaged.
  //! This can be done only on first transfer.
  //! Considers a category number, by default 0.
  //! Error if : - the Result is known as Already Used
  //! or if   - No  Result is bound to <start>
  //! or if   - <binder> brings No result.
  Standard_EXPORT void Rebind (const StartObject& theStart,
                               const Handle(Transfer_Binder)& theBinder);
  
  //! Removes the Binder linked with a starting object
  //! If this Binder brings a non-empty Check, it is replaced by
  //! a VoidBinder. Also removes from the list of Roots as required.
  //! Returns True if done, False if <start> was not bound.
  //! Considers a category number, by default 0.
  //! Error if : - the Result is known as Already Used.
  Standard_EXPORT Standard_Boolean Unbind (const StartObject& theStart);
  
  //! Returns a Binder for a starting entity, as follows:
  //! Tries to Find the already bound one.
  //! If none found, creates a VoidBinder and Binds it.
  Standard_EXPORT Handle(Transfer_Binder) FindElseBind (const StartObject& theStart);
  
  //! Sets Messenger used for outputting messages.
  Standard_EXPORT void SetMessenger (const Handle(Message_Messenger)& myMessenger);
  
  //! Returns Messenger used for outputting messages.
  //! The returned object is guaranteed to be non-null;
  //! default is Message::Messenger().
  Standard_EXPORT Handle(Message_Messenger) Messenger () const;
  
  //! Sets trace level used for outputting messages:
  //! <trace> = 0 : no trace at all
  //! <trace> = 1 : handled exceptions and calls to AddError
  //! <trace> = 2 : also calls to AddWarning
  //! <trace> = 3 : also traces new Roots
  //! (uses method ErrorTrace).
  //! Default is 1 : Errors traced.
  Standard_EXPORT void SetTraceLevel (const Standard_Integer myTraceLevel);
  
  //! Returns trace level used for outputting messages.
  Standard_EXPORT Standard_Integer TraceLevel () const;
  
  //! New name for AddFail(theMsg).
  Standard_EXPORT void  SendFail (const StartObject& theStart,
                                  const Message_Msg& theMsg);
  
  //! New name for AddWarning (Msg).
  Standard_EXPORT void  SendWarning (const StartObject& theStart,
                                     const Message_Msg& theMsg);
  //! Adds an information message.
  //! Trace is filled if trace level is at least 3.
  Standard_EXPORT void  SendMsg (const StartObject& theStart,
                                 const Message_Msg& theMsg);
  
  //! Adds an Error message to a starting entity (to the check of
  //! its Binder of category 0, as a Fail).
  Standard_EXPORT void AddFail (const StartObject& theStart,
                                const Standard_CString theMessage,
                                const Standard_CString theOrig = "");
  
  //! The other name of AddFail, maintained for compatibility.
  Standard_EXPORT void AddError (const StartObject& theStart,
                                 const Standard_CString theMessage,
                                 const Standard_CString theOrig = "");
                                 
  //! Adds an Error Message to a starting entity from the definition
  //! of a Msg (Original+Value).
  Standard_EXPORT void AddFail (const StartObject& theStart,
                                const Message_Msg& theMsg);
  
  //! Adds a Warning message to a starting entity (to the check of
  //! its Binder of category 0).  
  Standard_EXPORT void AddWarning (const StartObject& theStart,
                                   const Standard_CString theMessage,
                                   const Standard_CString theOrig = "");
  
  //! Adds a Warning Message to a starting entity from the definition
  //! of a Msg (Original+Value).
  Standard_EXPORT void AddWarning (const StartObject& theStart,
                                   const Message_Msg& theMsg);
                                   
  //! Mends messages bound to a starting entity, if there are some
  //! <thePref> as for Check from Interface :
  //! Default converts all fails to warnings and that's all.                                
  Standard_EXPORT void Mend (const StartObject& theStart,
                             const Standard_CString thePref = "");
  
  //! Returns the Check attached to a starting entity.
  //! If <start> is unknown, returns an empty Check.  
  Standard_EXPORT Handle(Interface_Check) Check (const StartObject& theStart) const;
  
  //! Binds a starting object with a Transient Result.
  //! Uses a SimpleBinderOfTransient to work. If there is already
  //! one but with no Result set, sets its Result.
  //! Considers a category number, by default 0.
  //! Error if a result is already bound (Binder not Initial).
  Standard_EXPORT void BindTransient (const StartObject& theStart,
                                      const Handle(Standard_Transient)& theResult);
  
  //! Returns the Result of the Transfer of an object <start> as a
  //!           Transient Result.
  //!           Returns a Null Handle if there is no Transient Result
  //!           Considers a category number, by default 0.
  //!  Warning : Supposes that Binding is done with a SimpleBinderOfTransient.
  Standard_EXPORT const Handle(Standard_Transient)& FindTransient
                                                    (const StartObject& theStart) const;

  //! Prepares an object <start> to be bound with several results.
  //! If no Binder is yet attached to <obj>, a MultipleBinder
  //! is created, empty. If a Binder is already set, it must
  //! accept Multiple Binding.
  //! Considers a category number, by default 0.
  //! Error if a UNIQUE result is already bound (if a Multiple
  //! Result is, nothing is done).
  Standard_EXPORT void BindMultiple (const StartObject& theStart);

  //! Searches for a transient result attached to a starting object,
  //! according to its type, by criterium IsKind(theType)
  //! In case of multiple result, explores the list and gives in
  //! <theVal> the first transient result IsKind(atype)
  //! Returns True and fills <theVal> if found
  //! Else, returns False (<theVal> is not touched, not even nullified)           
  //! This syntactic form avoids to do DownCast : if a result is
  //! found with the good type, it is loaded in <theVal> and can be
  //! immediately used, well initialised.
  Standard_EXPORT Standard_Boolean
  FindTypedTransient (const StartObject& theStart,
                      const Handle(Standard_Type)& theType,
                      Handle(Standard_Transient)& theVal) const;
  
  //! Searches for a transient result recorded in a Binder, whatever
  //! this Binder is recorded or not in <me>.
  //! This is strictly equivalent to the class method GetTypedResult
  //! from class SimpleBinderOfTransient, but is just lighter to call        
  //! Apart from this, works as FindTypedTransient.
  Standard_EXPORT Standard_Boolean
  GetTypedTransient (const Handle(Transfer_Binder)& theBinder,
                     const Handle(Standard_Type)& theType,
                     Handle(Standard_Transient)& theVal) const;
  
  //! Returns the maximum possible value for Map Index
  //! (no result can be bound with a value greater than it).           
  Standard_EXPORT Standard_Integer NbMapped () const;
  
  //! Returns the Starting Object bound to an Index.
  Standard_EXPORT const StartObject& Mapped (const Standard_Integer theNum) const;
  
  //! Returns the Index value bound to a Starting Object, 0 if none.
  Standard_EXPORT Standard_Integer MapIndex (const StartObject& theStart) const;
  
  //! Returns the Binder bound to an Index
  //! Considers a category number, by default 0.
  Standard_EXPORT Handle(Transfer_Binder) MapItem(const Standard_Integer theNum) const;
  
  //! Declares <obj> (and its Result) as Root. This status will be
  //! later exploited by RootResult, see below (Result can be
  //! produced at any time).
  //! Error if <obj> is not bound before calling to SetRoot.
  Standard_EXPORT void SetRoot (const StartObject& theStart);
  
  //! Enable (if <stat> True) or Disables (if <stat> False) Root
  //! Management. If it is set, Transfers are considered as stacked
  //! (a first Transfer commands other Transfers, and so on) and
  //! the Transfers commanded by an external caller are "Root".
  //! Remark : SetRoot can be called whatever this status, on every
  //! object.
  //! Default is set to True.
  Standard_EXPORT void SetRootManagement (const Standard_Boolean theStatus);
  
  //! Returns the count of recorded roots.
  Standard_EXPORT Standard_Integer NbRoots () const;
  
  //! Returns a Root Entity given its number in the list (1-NbRoots).
  Standard_EXPORT const StartObject& Root (const Standard_Integer theNum) const;
  
  //! Returns the Binder bound with a Root Entity given its number.
  //! Considers a category number, by default 0.
  Standard_EXPORT Handle(Transfer_Binder) RootItem (const Standard_Integer theNum) const;
  
  //! Returns the index in the list of roots for a starting item,
  //! or 0 if it is not recorded as a root.
  Standard_EXPORT Standard_Integer RootIndex (const StartObject& theStart) const;
  
  //! Returns Nesting Level of Transfers (managed by methods
  //! TranscriptWith & Co). Starts to zero. If no automatic Transfer
  //! is used, it remains to zero. Zero means Root Level.
  Standard_EXPORT Standard_Integer NestingLevel () const;
  
  //! Resets Nesting Level of Transfers to Zero (Root Level),
  //! whatever its current value.
  Standard_EXPORT void ResetNestingLevel ();
  
  //! Tells if <start> has been recognized as good candidate for
  //! Transfer. i.e. queries the Actor and its Nexts.
  Standard_EXPORT Standard_Boolean Recognize (const StartObject& theStart) const;
  
  //! Performs the Transfer of a Starting Object, by calling
  //! the method TransferProduct (see below).
  //! Mapping and Roots are managed : nothing is done if a Result is
  //! already Bound, an exception is raised in case of error.
  //! Error : if the Transfer falls in a loop, or asks to Rebind an
  //! Object already Bound AND Used or reads a non computed Result
  //!           
  //!  Remark : if ErrorHandle is set, no exception is raised : see
  //!  the method SetErrorHandle for more details
  Standard_EXPORT Handle(Transfer_Binder) Transferring (const StartObject& theStart);
  
  //! Same as Transferring but does not return the Binder.
  //! Simply returns True in case of success (for user call).
  Standard_EXPORT Standard_Boolean Transfer (const StartObject& theStart) ;
  
  //! Allows controls if exceptions will be handled 
  //! Transfer Operations
  //! <err> False : they are not handled with try {} catch {}
  //! <err> True  : they are
  //! Default is False: no handling performed.
  Standard_EXPORT void SetErrorHandle (const Standard_Boolean isError);
  
  //! Returns error handling flag.
  Standard_EXPORT Standard_Boolean ErrorHandle() const;
  
  //! Method called when trace is asked
  //! Calls PrintTrace to display information relevant for starting
  //! objects (which can be redefined)
  //!	<level> is Nesting Level of Transfer (0 = root)
  //!	<mode> controls the way the trace is done :
  //!	0 neutral, 1 for Error, 2 for Warning message, 3 for new Root.
  Standard_EXPORT void StartTrace (const Handle(Transfer_Binder)& theBinder,
                                   const StartObject& theStart,
                                   const Standard_Integer theLevel,
                                   const Standard_Integer theMode) const;
  
  //! Prints a short information on a starting object.
  //! By default prints its Dynamic Type. Can be redefined.  
  Standard_EXPORT void PrintTrace (const StartObject& theStart,
                                   const Handle(Message_Messenger)& myMessenger) const;
  
  //! Returns True if we are surely in a DeadLoop. Evaluation is not
  //! exact, it is a "majorant" which must be computed fast.
  //! This "majorant" is : <theLevel> greater than NbMapped.
  Standard_EXPORT Standard_Boolean IsLooping (const Standard_Integer theLevel) const;
  
  //! Returns, as an iterator, the log of root transfer, i.e. the
  //! created objects and Binders bound to starting roots.
  //! If withstart is given True, Starting Objets are also returned.
  Standard_EXPORT Iterator RootResult (const Standard_Boolean isWithStart = Standard_False) const;
  
  //! Returns, as an Iterator, the entire log of transfer (list of
  //! created objects and Binders which can bring errors).
  //! If withstart is given True, Starting Objets are also returned.
  Standard_EXPORT Iterator CompleteResult (const Standard_Boolean isWithStart = Standard_False) const;
  
  //! Returns Binders which are neither "Done" nor "Initial",
  //! that is Error,Loop or Run (abnormal states at end of Transfer)
  //! Starting Objects are given in correspondance in the iterator.
  Standard_EXPORT Iterator AbnormalResult() const;
  
  //! Returns a CheckList as a list of Check : each one is for a
  //! starting entity which have either check (warning or fail)
  //! messages are attached, or are in abnormal state : that case
  //! gives a specific message.
  //! If <erronly> is True, checks with Warnings only are ignored
  Standard_EXPORT Interface_CheckIterator CheckList (const Standard_Boolean isErrOnly) const;
  
  //! Returns, as an Iterator, the log of transfer for one object
  //! <theLevel> = 0 : this object only
  //! and if <start> is a scope owner (else, <theLevel> is ignored) :
  //! <theLevel> = 1 : object plus its immediate scoped ones
  //! <theLevel> = 2 : object plus all its scoped ones.
  Standard_EXPORT Iterator ResultOne (const StartObject& theStart,
                                      const Standard_Integer theLevel,
                                      const Standard_Boolean isWithStart = Standard_False) const;
  
  //! Returns a CheckList for one starting object
  //! <theLevel> interpreted as by ResultOne
  //! If <isErrOnly> is True, checks with Warnings only are ignored  
  Standard_EXPORT Interface_CheckIterator
  CheckListOne (const StartObject& theStart,
                const Standard_Integer theLevel,
                const Standard_Boolean isErrOnly) const;
  
  //! Returns True if no check message is attached to a starting
  //! object. <level> interpreted as by ResultOne
  //! If <erronly> is True, checks with Warnings only are ignored .
  Standard_EXPORT Standard_Boolean
  IsCheckListEmpty (const StartObject& theStart,
                    const Standard_Integer theLevel,
                    const Standard_Boolean isErrOnly) const;
  
  //! Removes Results attached to (== Unbinds) a given object and,
  //! according <level> :
  //! <level> = 0 : only it
  //! <level> = 1 : it plus its immediately owned sub-results(scope)
  //! <level> = 2 : it plus all its owned sub-results(scope).
  Standard_EXPORT void RemoveResult (const StartObject& theStart,
                                     const Standard_Integer theLevel,
                                     const Standard_Boolean toCompute = Standard_True);
  
  //! Computes a number to be associated to a starting object in
  //! a check or a check-list.
  //! By default, returns 0; can be redefined.
  Standard_EXPORT virtual Standard_Integer CheckNum(const StartObject& ) const;
  
  //! Sets Progress indicator.
  Standard_EXPORT void SetProgress (const Handle(Message_ProgressIndicator)& theProgress);
  
  //! Gets Progress indicator.
  Standard_EXPORT  Handle(Message_ProgressIndicator) GetProgress () const;

  //! Adds an item to a list of results bound to a starting object.
  //! Considers a category number, by default 0, for all results
  //! Error if no Binder accepting Multiple Binding is attached
  //! to <start> (none at all, or for a unique result)
  Standard_EXPORT void AddMultiple (const StartObject& theStart,
                                    const Handle(Standard_Transient)& theResult);
  
};

  //=======================================================================
  //function : Transfer_TransferProcess
  //purpose  : 
  //=======================================================================
  template <class ActorHandle, class TransferMap, class StartObject,
          class MapHasher, class ListHandle, class Iterator>
  Transfer_TransferProcess <ActorHandle,TransferMap,StartObject,
                            MapHasher,ListHandle,Iterator>
  ::Transfer_TransferProcess (const Standard_Integer theNb)
    : myMap (theNb)
  {
    myToHandleErr  = Standard_True;
    myToManageRoot = Standard_False;
    myLevel = 0;     myRootLevel  = 0;
    myMessenger = Message::DefaultMessenger();
    myTrace = 0;
  //  myRoots = new TColStd_HSequenceOfInteger ();
    myIndex = 0;
  }


  //=======================================================================
  //function : Transfer_TransferProcess
  //purpose  : 
  //=======================================================================

  template <class ActorHandle, class TransferMap, class StartObject,
          class MapHasher, class ListHandle, class Iterator>
  Transfer_TransferProcess <ActorHandle,TransferMap,StartObject,
                            MapHasher,ListHandle,Iterator>
  ::Transfer_TransferProcess (const Handle(Message_Messenger)& myMessenger,
                              const Standard_Integer theNb)
  : myMap (theNb)
  {
    myToHandleErr  = Standard_True;
    myToManageRoot = Standard_False;
    myLevel = 0;     myRootLevel  = 0;
    SetMessenger (myMessenger);
    myTrace = 1;
    myIndex = 0;
  }

  //=======================================================================
  //function : Clear
  //purpose  : 
  //=======================================================================
  
  template <class ActorHandle, class TransferMap, class StartObject,
            class MapHasher, class ListHandle, class Iterator>
  void
  Transfer_TransferProcess <ActorHandle,TransferMap,StartObject,
                            MapHasher,ListHandle,Iterator>
  ::Clear ()
  {
    myLevel = 0;     myRootLevel  = 0;
    myRoots.Clear();
    myMap.Clear();
    myIndex = 0;  myLastObj.Nullify();  myLastBinder.Nullify();
  }

  //=======================================================================
  //function : Clean
  //purpose  : 
  //=======================================================================
  
  template <class ActorHandle, class TransferMap, class StartObject,
            class MapHasher, class ListHandle, class Iterator>
  void
  Transfer_TransferProcess <ActorHandle,TransferMap,StartObject,
                            MapHasher,ListHandle,Iterator>
  ::Clean ()
  {
    Standard_Integer anI, aNb = NbMapped();
    Standard_Integer aJ,unb = 0;
    for (anI = 1; anI <= aNb; anI ++)
    {
      if (myMap(anI).IsNull()) unb ++;
    }
    if (unb == 0) return;

    TColStd_Array1OfInteger unbs (1,aNb);  unbs.Init(0);
    TransferMap newmap (aNb*2);
    for (anI = 1; anI <= aNb; anI ++)
    {
      StartObject ent = Mapped(anI);
      Handle(Transfer_Binder) bnd = MapItem(anI);
      if (bnd.IsNull()) continue;
      aJ = newmap.Add (ent,bnd);
      unbs.SetValue (anI,aJ);
    }
    myMap.Assign (newmap);

    // Update the list of root
    TColStd_IndexedMapOfInteger aNewRoots;
    for (anI=1; anI<= myRoots.Extent(); anI++)
    {
      aJ = myRoots.FindKey(anI);
      Standard_Integer k = unbs.Value(aJ);
      if (k) aNewRoots.Add (k);
    }
    myRoots.Clear();
    myRoots = aNewRoots;

    myLastObj.Nullify();
    myLastBinder.Nullify();
    myIndex = 0;
  }

  //=======================================================================
  //function : Resize
  //purpose  : 
  //=======================================================================

  template <class ActorHandle, class TransferMap, class StartObject,
            class MapHasher, class ListHandle, class Iterator>
  void
  Transfer_TransferProcess <ActorHandle,TransferMap,StartObject,
                            MapHasher,ListHandle,Iterator>
  ::Resize (const Standard_Integer theNb)
  {
    if (theNb > myMap.NbBuckets())
      myMap.ReSize(theNb);
  }

  //=======================================================================
  //function : SetActor
  //purpose  : 
  //=======================================================================

  template <class ActorHandle, class TransferMap, class StartObject,
            class MapHasher, class ListHandle, class Iterator>
  void
  Transfer_TransferProcess <ActorHandle,TransferMap,StartObject,
                            MapHasher,ListHandle,Iterator>
  ::SetActor(const ActorHandle& theActor)
  {
    if (myActor == theActor)         return;
    if (myActor.IsNull())         myActor = theActor;
    else if (theActor.IsNull())       myActor = theActor;  // declenche RAZ
    else if (myActor->IsLast()) { theActor->SetNext(myActor);  myActor = theActor; }
    else                           myActor->SetNext(theActor);
  }


  //=======================================================================
  //function : Actor
  //purpose  : 
  //=======================================================================

  template <class ActorHandle, class TransferMap, class StartObject,
            class MapHasher, class ListHandle, class Iterator>
  ActorHandle
  Transfer_TransferProcess <ActorHandle,TransferMap,StartObject,
                            MapHasher,ListHandle,Iterator>
  ::Actor () const
  {
    return myActor;
  }


  //  ########################################################################
  //  ....                             MAPPING                            ....

  //  ##    ##    ##       Actions Generales sur Binders       ##    ##    ##
  //  ##    ##    ##               Consultations               ##    ##    ##

  //=======================================================================
  //function : Find
  //purpose  : 
  //=======================================================================

  template <class ActorHandle, class TransferMap, class StartObject,
            class MapHasher, class ListHandle, class Iterator>
  Handle(Transfer_Binder)
  Transfer_TransferProcess <ActorHandle,TransferMap,StartObject,
                            MapHasher,ListHandle,Iterator>
  ::Find (const StartObject& theStart) const
  {
    if (myLastObj == theStart)
    {
      //if (myIndex > 0) return myLastBinder->Search(categ); //skl
      if (myIndex > 0) return myLastBinder; //skl
    }
    Standard_Integer index = myMap.FindIndex (theStart);
    if (index > 0)
    {
      const Handle(Transfer_Binder)& theBinder = myMap.FindFromIndex(index);
      //if (theBinder.IsNull()) //skl
      return theBinder;
      //return theBinder->Search(categ); //skl
    }
    Handle(Transfer_Binder)  aNullBinder;
    return aNullBinder;
  }

  //=======================================================================
  //function : IsBound
  //purpose  : 
  //=======================================================================

  template <class ActorHandle, class TransferMap, class StartObject,
            class MapHasher, class ListHandle, class Iterator>
  Standard_Boolean
  Transfer_TransferProcess <ActorHandle,TransferMap,StartObject,
                            MapHasher,ListHandle,Iterator>
  ::IsBound (const StartObject& theStart) const
  {
    Handle(Transfer_Binder) theBinder = Find(theStart); //,categ); skl
    if (theBinder.IsNull()) return Standard_False;
    return theBinder->HasResult();
  }

  //=======================================================================
  //function : IsAlreadyUsed
  //purpose  : 
  //=======================================================================

  template <class ActorHandle, class TransferMap, class StartObject,
            class MapHasher, class ListHandle, class Iterator>
  Standard_Boolean
  Transfer_TransferProcess <ActorHandle,TransferMap,StartObject,
                            MapHasher,ListHandle,Iterator>
  ::IsAlreadyUsed (const StartObject& theStart) const
  {
    Handle(Transfer_Binder) theBinder = Find(theStart);
    if (theBinder.IsNull()) {
      StartTrace (theBinder,theStart,myLevel,4);
      Transfer_TransferFailure::Raise
        ("TransferProcess : IsAlreadyUsed, transfer not done cannot be used...");
    }
    return (theBinder->Status() == Transfer_StatusUsed);
  }

  //=======================================================================
  //function : FindAndMask
  //purpose  : 
  //=======================================================================

  template <class ActorHandle, class TransferMap, class StartObject,
            class MapHasher, class ListHandle, class Iterator>
  Handle(Transfer_Binder)
  Transfer_TransferProcess <ActorHandle,TransferMap,StartObject,
                            MapHasher,ListHandle,Iterator>
  ::FindAndMask (const StartObject& theStart)
  {
    if (myLastObj == theStart)
    {
      if (myIndex > 0) return myLastBinder;
    }
    myLastObj = theStart;
    myIndex   = myMap.FindIndex (theStart);
    if (myIndex > 0) myLastBinder = myMap.FindFromIndex(myIndex);
    else myLastBinder.Nullify();
    return myLastBinder;
  }

  //=======================================================================
  //function : Bind
  //purpose  : 
  //=======================================================================

  template <class ActorHandle, class TransferMap, class StartObject,
            class MapHasher, class ListHandle, class Iterator>
  void
  Transfer_TransferProcess <ActorHandle,TransferMap,StartObject,
                            MapHasher,ListHandle,Iterator>
  ::Bind (const StartObject& theStart,
          const Handle(Transfer_Binder)& theBinder)
  {
    if (theBinder.IsNull()) return;
    Handle(Transfer_Binder) aFormer = FindAndMask(theStart);
    if (!aFormer.IsNull())
    {
      if (aFormer->DynamicType() == STANDARD_TYPE(Transfer_VoidBinder))
      {
        theBinder->Merge(aFormer);
        myMap(myIndex) = theBinder; // Substitution
      }
      else if (aFormer->Status() == Transfer_StatusUsed)
      {
        StartTrace (aFormer,theStart,myLevel,4);
        Transfer_TransferFailure::Raise
         ("TransferProcess : Bind, already Bound");
      }
      else
      {
        if (myTrace > 2) StartTrace (aFormer,theStart,myLevel,5);
        theBinder->CCheck()->GetMessages (aFormer->Check());
      }
    }
    if (myIndex == 0 || myLastBinder.IsNull())
    {
      if (myIndex == 0) myIndex = myMap.Add(theStart,theBinder);
      else myMap(myIndex) = theBinder;
      myLastBinder = theBinder;
    }
    else 
    {
      myLastBinder  = theBinder;
      myMap(myIndex) = theBinder;
    }
  }

  //=======================================================================
  //function : Rebind
  //purpose  : 
  //=======================================================================

  template <class ActorHandle, class TransferMap, class StartObject,
            class MapHasher, class ListHandle, class Iterator>
  void
  Transfer_TransferProcess <ActorHandle,TransferMap,StartObject,
                            MapHasher,ListHandle,Iterator>
  ::Rebind (const StartObject& theStart,
            const Handle(Transfer_Binder)& theBinder)
  {
    Bind(theStart,theBinder);
  }

  //=======================================================================
  //function : Unbind
  //purpose  : 
  //=======================================================================

  template <class ActorHandle, class TransferMap, class StartObject,
            class MapHasher, class ListHandle, class Iterator>
  Standard_Boolean
  Transfer_TransferProcess <ActorHandle,TransferMap,StartObject,
                            MapHasher,ListHandle,Iterator>
  ::Unbind (const StartObject& theStart)
  {
    Handle(Transfer_Binder) aFormer = FindAndMask(theStart);
    if (myIndex == 0) return Standard_False;
    if (aFormer.IsNull()) return Standard_False;
    if (aFormer->DynamicType() == STANDARD_TYPE(Transfer_VoidBinder))
      return Standard_True;
      myMap(myIndex) = myLastBinder;
    if (myRoots.Contains(myIndex))
    {
      TColStd_IndexedMapOfInteger aNewRoots;
      for (Standard_Integer anI = 1; anI <= myRoots.Extent(); anI++)
        if (myIndex!= myRoots.FindKey(anI))
          aNewRoots.Add(myRoots.FindKey(anI));
      
      myRoots.Clear();
      myRoots = aNewRoots;
    }

    myLastObj.Nullify();
    myLastBinder.Nullify();
    myIndex = 0;
    return Standard_True;
  }

  //=======================================================================
  //function : FindElseBind
  //purpose  : 
  //=======================================================================

  template <class ActorHandle, class TransferMap, class StartObject,
            class MapHasher, class ListHandle, class Iterator>
  Handle(Transfer_Binder)
  Transfer_TransferProcess <ActorHandle,TransferMap,StartObject,
                            MapHasher,ListHandle,Iterator>
  ::FindElseBind (const StartObject& theStart)
  {
    Handle(Transfer_Binder) aBinder = FindAndMask (theStart);
    if (!aBinder.IsNull()) return aBinder;
    aBinder = new Transfer_VoidBinder;
    Bind(theStart,aBinder);
    return aBinder;
  }
  
  //=======================================================================
  //function : SetMessenger
  //purpose  : 
  //=======================================================================

  template <class ActorHandle, class TransferMap, class StartObject,
            class MapHasher, class ListHandle, class Iterator>
  void
  Transfer_TransferProcess <ActorHandle,TransferMap,StartObject,
                            MapHasher,ListHandle,Iterator>
  ::SetMessenger (const Handle(Message_Messenger)& myMessenger)
  {
    if ( myMessenger.IsNull() )
      myMessenger = Message::DefaultMessenger();
    else   
      myMessenger = myMessenger;
  }

  //=======================================================================
  //function : Messenger
  //purpose  : 
  //=======================================================================

  template <class ActorHandle, class TransferMap, class StartObject,
            class MapHasher, class ListHandle, class Iterator>
  Handle(Message_Messenger)
  Transfer_TransferProcess <ActorHandle,TransferMap,StartObject,
                            MapHasher,ListHandle,Iterator>
  ::Messenger () const
  {
    return myMessenger;
  }

  //=======================================================================
  //function : SetTraceLevel
  //purpose  : 
  //=======================================================================

  template <class ActorHandle, class TransferMap, class StartObject,
            class MapHasher, class ListHandle, class Iterator>
  void
  Transfer_TransferProcess <ActorHandle,TransferMap,StartObject,
                            MapHasher,ListHandle,Iterator>
  ::SetTraceLevel (const Standard_Integer myTraceLevel)
  {
    myTrace = myTraceLevel;
  }

  //=======================================================================
  //function : TraceLevel
  //purpose  : 
  //=======================================================================

  template <class ActorHandle, class TransferMap, class StartObject,
            class MapHasher, class ListHandle, class Iterator>
  Standard_Integer
  Transfer_TransferProcess <ActorHandle,TransferMap,StartObject,
                            MapHasher,ListHandle,Iterator>
  ::TraceLevel () const
  {
    return myTrace;
  }

  //=======================================================================
  //function : SendFail
  //purpose  : 
  //=======================================================================

  template <class ActorHandle, class TransferMap, class StartObject,
            class MapHasher, class ListHandle, class Iterator>
  void
  Transfer_TransferProcess <ActorHandle,TransferMap,StartObject,
                            MapHasher,ListHandle,Iterator>
  ::SendFail (const StartObject& theStart,
              const Message_Msg& theMsg)
  {
    AddFail(theStart,theMsg);
  }

  //=======================================================================
  //function : SendWarning
  //purpose  : 
  //=======================================================================

  template <class ActorHandle, class TransferMap, class StartObject,
            class MapHasher, class ListHandle, class Iterator>
  void
  Transfer_TransferProcess <ActorHandle,TransferMap,StartObject,
                            MapHasher,ListHandle,Iterator>
  ::SendWarning (const StartObject& theStart,
                                               const Message_Msg& theMsg)
  {
    AddWarning(theStart,theMsg);
  }

  //=======================================================================
  //function : SendMsg
  //purpose  : 
  //=======================================================================

  template <class ActorHandle, class TransferMap, class StartObject,
            class MapHasher, class ListHandle, class Iterator>
  void
  Transfer_TransferProcess <ActorHandle,TransferMap,StartObject,
                            MapHasher,ListHandle,Iterator>
  ::SendMsg (const StartObject& theStart,
             const Message_Msg& theMsg)
  {
    Handle(Transfer_Binder) theBinder = FindAndMask(theStart);
    if (theBinder.IsNull()) {
      theBinder = new Transfer_VoidBinder;
      Bind (theStart,theBinder);
    }
    // Feeds the trace: Rule causing (user messages)
    if (myTrace > 0) {
      StartTrace (theBinder,theStart,myLevel,6);
      myMessenger << theMsg.Value();
      if (theMsg.IsEdited()&&myTrace>2)
        myMessenger << " [from: " << theMsg.Original() << "]";
      myMessenger << endl;
    }
  }

  //=======================================================================
  //function : AddFail
  //purpose  : 
  //=======================================================================

  template <class ActorHandle, class TransferMap, class StartObject,
            class MapHasher, class ListHandle, class Iterator>
  void
  Transfer_TransferProcess <ActorHandle,TransferMap,StartObject,
                            MapHasher,ListHandle,Iterator>
  ::AddFail (const StartObject& theStart,
                                const Standard_CString theMessage,
                                const Standard_CString theOrig)
  {
    Handle(Transfer_Binder) theBinder = FindAndMask (theStart);
    if (theBinder.IsNull())
    {
      theBinder = new Transfer_VoidBinder;
      Bind (theStart,theBinder);
    }
    theBinder->AddFail (theMessage,theOrig);
    if (myTrace > 0)
    {
      StartTrace (theBinder,theStart,myLevel,1);
      myMessenger << "    --> Fail : " << theMessage;
      if (theOrig[0] != '\0'&&myTrace>2) myMessenger << " [from: " << theOrig << "]";
      myMessenger << endl;
    }
  }


  //=======================================================================
  //function : AddError
  //purpose  : 
  //=======================================================================

  template <class ActorHandle, class TransferMap, class StartObject,
            class MapHasher, class ListHandle, class Iterator>
  void
  Transfer_TransferProcess <ActorHandle,TransferMap,StartObject,
                            MapHasher,ListHandle,Iterator>
  ::AddError(const StartObject& theStart,
             const Standard_CString theMessage,
             const Standard_CString theOrig)
  {
    AddFail (theStart,theMessage,theOrig);
  }

  //=======================================================================
  //function : AddFail
  //purpose  : 
  //=======================================================================

  template <class ActorHandle, class TransferMap, class StartObject,
            class MapHasher, class ListHandle, class Iterator>
  void
  Transfer_TransferProcess <ActorHandle,TransferMap,StartObject,
                            MapHasher,ListHandle,Iterator>
  ::AddFail (const StartObject& theStart,
             const Message_Msg& theMsg)
  {
    if (theMsg.IsEdited()) AddFail (theStart,TCollection_AsciiString(theMsg.Value()).ToCString(),
          TCollection_AsciiString(theMsg.Original()).ToCString());
    else AddFail (theStart,TCollection_AsciiString(theMsg.Value()).ToCString());
  }

  //=======================================================================
  //function : AddWarning
  //purpose  : 
  //=======================================================================

  template <class ActorHandle, class TransferMap, class StartObject,
            class MapHasher, class ListHandle, class Iterator>
  void
  Transfer_TransferProcess <ActorHandle,TransferMap,StartObject,
                            MapHasher,ListHandle,Iterator>
  ::AddWarning (const StartObject& theStart,
                const Standard_CString theMessage,
                const Standard_CString theOrig)
  {
    Handle(Transfer_Binder) aBinder = FindAndMask(theStart);
    if (aBinder.IsNull()) {
      aBinder = new Transfer_VoidBinder;
      Bind (theStart,aBinder);
    }
    aBinder->AddWarning(theMessage,theOrig);
    if (myTrace > 1) {
      StartTrace (aBinder,theStart,myLevel,2);
      myMessenger << "    --> Warning : " << theMessage;
      if (theOrig[0] != '\0'&&myTrace>2) myMessenger << " [from: " << theOrig << "]";
      myMessenger << endl;
    }
  }

  //=======================================================================
  //function : AddWarning
  //purpose  : 
  //=======================================================================

  template <class ActorHandle, class TransferMap, class StartObject,
            class MapHasher, class ListHandle, class Iterator>
  void
  Transfer_TransferProcess <ActorHandle,TransferMap,StartObject,
                            MapHasher,ListHandle,Iterator>
  ::AddWarning (const StartObject& theStart,
                const Message_Msg& theMsg)
  {
    if (theMsg.IsEdited())
      AddWarning (theStart,TCollection_AsciiString(theMsg.Value()).ToCString(),
                  TCollection_AsciiString(theMsg.Original()).ToCString());
    else
      AddWarning (theStart,TCollection_AsciiString(theMsg.Value()).ToCString());
  }

  //=======================================================================
  //function : Mend
  //purpose  : 
  //=======================================================================

  template <class ActorHandle, class TransferMap, class StartObject,
            class MapHasher, class ListHandle, class Iterator>
  void
  Transfer_TransferProcess <ActorHandle,TransferMap,StartObject,
                            MapHasher,ListHandle,Iterator>
  ::Mend (const StartObject& theStart,
          const Standard_CString thePref)
  {
    Handle(Transfer_Binder) aBinder = FindAndMask (theStart);
    if (aBinder.IsNull())
      return;
    Handle(Interface_Check) ach =  aBinder->CCheck();
    ach->Mend (thePref);
  }

  //=======================================================================
  //function : Check
  //purpose  : 
  //=======================================================================

  template <class ActorHandle, class TransferMap, class StartObject,
            class MapHasher, class ListHandle, class Iterator>
  Handle(Interface_Check)
  Transfer_TransferProcess <ActorHandle,TransferMap,StartObject,
                            MapHasher,ListHandle,Iterator>
  ::Check(const StartObject& theStart) const
  {
    const Handle(Transfer_Binder)& aBinder = Find(theStart);
    if (aBinder.IsNull()) {
      Handle(Interface_Check) aCheck;
      return aCheck;
    }
    return aBinder->Check();
  }

  //  ##    ##    ##        Actions sur Types Privilegies        ##    ##    ##
  //  ##    ##    ##          (Transient)                        ##    ##    ##

  // Bind associe un objet a un objet resultat; or la Map associe un Objet a un
  // Binder (qui designe son resultat)
  //  *Transient  travaillent avec un SimpleBinderOfTransient
  // si  deja la, on considere son resultat
  // sinon, on cree un Binder du bon type


  //=======================================================================
  //function : BindTransient
  //purpose  : 
  //=======================================================================

  template <class ActorHandle, class TransferMap, class StartObject,
            class MapHasher, class ListHandle, class Iterator>
  void
  Transfer_TransferProcess <ActorHandle,TransferMap,StartObject,
                            MapHasher,ListHandle,Iterator>
  ::BindTransient (const StartObject& theStart,
                   const Handle(Standard_Transient)& theResult)
  {
    if (theResult.IsNull()) return;
    Handle(Transfer_Binder) aFormer = Find(theStart);
    Handle(Transfer_SimpleBinderOfTransient) aBinder =
      Handle(Transfer_SimpleBinderOfTransient)::DownCast(aFormer);
    if (!aBinder.IsNull())
    {
      if (aBinder->Status() == Transfer_StatusVoid)
      {
        aBinder->SetResult(theResult); return;
      }
    }
    aBinder = new Transfer_SimpleBinderOfTransient;
    aBinder->SetResult (theResult);
    if (aFormer.IsNull())
      Bind(theStart,aBinder);
    else
      Rebind(theStart,aBinder);
  }

  //=======================================================================
  //function : FindTransient
  //purpose  : 
  //=======================================================================

  template <class ActorHandle, class TransferMap, class StartObject,
            class MapHasher, class ListHandle, class Iterator>
  const Handle(Standard_Transient)&
  Transfer_TransferProcess <ActorHandle,TransferMap,StartObject,
                            MapHasher,ListHandle,Iterator>
  ::FindTransient (const StartObject& theStart) const
  {
    Handle(Transfer_SimpleBinderOfTransient) aBinder =
      Handle(Transfer_SimpleBinderOfTransient)::DownCast(Find(theStart));
    if (aBinder.IsNull()) return myNullTrans;
    if (!aBinder->HasResult()) return myNullTrans;
    return aBinder->Result();
  }

  // Binding Multiple: D by BindMultiple declare the first (if not already done)
  // Then add the words by AddMultiple

  //=======================================================================
  //function : BindMultiple
  //purpose  : 
  //=======================================================================

  template <class ActorHandle, class TransferMap, class StartObject,
            class MapHasher, class ListHandle, class Iterator>
  void
  Transfer_TransferProcess <ActorHandle,TransferMap,StartObject,
                            MapHasher,ListHandle,Iterator>
  ::BindMultiple (const StartObject& theStart)
  {
    Handle(Transfer_Binder) aBinder = FindAndMask (theStart);
    if (!aBinder.IsNull())
    {
      if (!aBinder->IsKind(STANDARD_TYPE(Transfer_MultipleBinder)))
      {
        StartTrace (myLastBinder,theStart,myLevel,4);
        Transfer_TransferFailure::Raise ("TransferProcess : BindMultiple");
      }
    }
    else Bind(theStart,new Transfer_MultipleBinder);
  }

  //=======================================================================
  //function : AddMultiple
  //purpose  : 
  //=======================================================================

  template <class ActorHandle, class TransferMap, class StartObject,
            class MapHasher, class ListHandle, class Iterator>
  void
  Transfer_TransferProcess <ActorHandle,TransferMap,StartObject,
                            MapHasher,ListHandle,Iterator>
  ::AddMultiple (const StartObject& theStart,
                 const Handle(Standard_Transient)& theResult)
  {
    Handle(Transfer_Binder) aBinder = FindAndMask(theStart);
    Handle(Transfer_MultipleBinder) aMultBinder =
      Handle(Transfer_MultipleBinder)::DownCast(aBinder);
    if (aMultBinder.IsNull())
    {
      StartTrace (aBinder,theStart,myLevel,4);
      if (aBinder.IsNull()) Transfer_TransferFailure::Raise
        ("TransferProcess : AddMultiple, nothing bound");
      else                 Transfer_TransferFailure::Raise
        ("TransferProcess : AddMultiple, Binder not a MultipleBinder");
    }
    aMultBinder->AddResult(theResult);
  }

  //=======================================================================
  //function : FindTypedTransient
  //purpose  : 
  //=======================================================================

  template <class ActorHandle, class TransferMap, class StartObject,
            class MapHasher, class ListHandle, class Iterator>
  Standard_Boolean
  Transfer_TransferProcess <ActorHandle,TransferMap,StartObject,
                            MapHasher,ListHandle,Iterator>
  ::FindTypedTransient (const StartObject& theStart,
                        const Handle(Standard_Type)& theType,
                        Handle(Standard_Transient)& theVal) const
  {
    return GetTypedTransient (Find(theStart), theType, theVal);
  }

  //=======================================================================
  //function : GetTypedTransient
  //purpose  : 
  //=======================================================================

  template <class ActorHandle, class TransferMap, class StartObject,
            class MapHasher, class ListHandle, class Iterator>
  Standard_Boolean
  Transfer_TransferProcess <ActorHandle,TransferMap,StartObject,
                            MapHasher,ListHandle,Iterator>
  ::GetTypedTransient (const Handle(Transfer_Binder)& theBinder,
                       const Handle(Standard_Type)& theType,
                       Handle(Standard_Transient)& theVal) const
  {
    return Transfer_SimpleBinderOfTransient::GetTypedResult(theBinder, theType, theVal);
  }


  //  ##    ##    ##    ##    ##    Atomic access    ##    ##    ##    ##    ##
  //    (doesn't manage the scope but gives access to categories)

  //=======================================================================
  //function : NbMapped
  //purpose  : 
  //=======================================================================
  
  template <class ActorHandle, class TransferMap, class StartObject,
            class MapHasher, class ListHandle, class Iterator>
  Standard_Integer
  Transfer_TransferProcess <ActorHandle,TransferMap,StartObject,
                            MapHasher,ListHandle,Iterator>
  ::NbMapped () const
  {
    return myMap.Extent();
  }

  //=======================================================================
  //function : Mapped
  //purpose  : 
  //=======================================================================

  template <class ActorHandle, class TransferMap, class StartObject,
            class MapHasher, class ListHandle, class Iterator>
  const StartObject&
  Transfer_TransferProcess <ActorHandle,TransferMap,StartObject,
                            MapHasher,ListHandle,Iterator>
  ::Mapped(const Standard_Integer theNum) const
  {
    return myMap.FindKey(theNum);
  }

  //=======================================================================
  //function : MapIndex
  //purpose  : 
  //=======================================================================

  template <class ActorHandle, class TransferMap, class StartObject,
            class MapHasher, class ListHandle, class Iterator>
  Standard_Integer
  Transfer_TransferProcess <ActorHandle,TransferMap,StartObject,
                            MapHasher,ListHandle,Iterator>
  ::MapIndex(const StartObject& theStart) const
  {
    return myMap.FindIndex(theStart);
  }

  //=======================================================================
  //function : MapItem
  //purpose  : 
  //=======================================================================

  template <class ActorHandle, class TransferMap, class StartObject,
            class MapHasher, class ListHandle, class Iterator>
  Handle(Transfer_Binder)
  Transfer_TransferProcess <ActorHandle,TransferMap,StartObject,
                            MapHasher,ListHandle,Iterator>
  ::MapItem(const Standard_Integer theNum) const
  {
    Handle(Transfer_Binder) aBinder = myMap.FindFromIndex(theNum);
    return aBinder;
  }


  //  ########################################################################
  //  ....                         ROOT MANAGEMENT                        ....

  //=======================================================================
  //function : SetRoot
  //purpose  : 
  //=======================================================================

  template <class ActorHandle, class TransferMap, class StartObject,
            class MapHasher, class ListHandle, class Iterator>
  void
  Transfer_TransferProcess <ActorHandle,TransferMap,StartObject,
                            MapHasher,ListHandle,Iterator>
  ::SetRoot (const StartObject& theStart)
  {
    Standard_Integer anIndex = MapIndex(theStart);
    if (anIndex == 0)
    {
      return;
    }

    myRoots.Add(anIndex);
    if (myTrace > 2)
      StartTrace (MapItem(anIndex),theStart,myLevel,3);
  }

  //=======================================================================
  //function : SetRootManagement
  //purpose  : 
  //=======================================================================
  
  template <class ActorHandle, class TransferMap, class StartObject,
            class MapHasher, class ListHandle, class Iterator>
  void
  Transfer_TransferProcess <ActorHandle,TransferMap,StartObject,
                            MapHasher,ListHandle,Iterator>
  ::SetRootManagement (const Standard_Boolean theStatus)
  {
    myToManageRoot = theStatus;
  }

  //=======================================================================
  //function : NbRoots
  //purpose  : 
  //=======================================================================

  template <class ActorHandle, class TransferMap, class StartObject,
            class MapHasher, class ListHandle, class Iterator>
  Standard_Integer
  Transfer_TransferProcess <ActorHandle,TransferMap,StartObject,
                            MapHasher,ListHandle,Iterator>
  ::NbRoots () const
  {
    return myRoots.Extent();
  }

  //=======================================================================
  //function : Root
  //purpose  : 
  //=======================================================================

  template <class ActorHandle, class TransferMap, class StartObject,
            class MapHasher, class ListHandle, class Iterator>
  const StartObject&
  Transfer_TransferProcess <ActorHandle,TransferMap,StartObject,
                            MapHasher,ListHandle,Iterator>
  ::Root (const Standard_Integer theNum) const
  {
    Standard_Integer anInd = 0;
    if (theNum > 0 && theNum <= myRoots.Extent()) anInd = myRoots.FindKey(theNum);
    return myMap.FindKey (anInd);
  }
  
  //=======================================================================
  //function : RootItem
  //purpose  : 
  //=======================================================================

  template <class ActorHandle, class TransferMap, class StartObject,
            class MapHasher, class ListHandle, class Iterator>
  Handle(Transfer_Binder)
  Transfer_TransferProcess <ActorHandle,TransferMap,StartObject,
                            MapHasher,ListHandle,Iterator>
  ::RootItem (const Standard_Integer theNum) const
  {
    Standard_Integer anInd = 0;
    if (theNum > 0 && theNum <= myRoots.Extent()) anInd = myRoots.FindKey(theNum);
    return myMap.FindFromIndex(anInd);//->Search(categ);skl
  }
  
  //=======================================================================
  //function : RootIndex
  //purpose  : 
  //=======================================================================

  template <class ActorHandle, class TransferMap, class StartObject,
            class MapHasher, class ListHandle, class Iterator>
  Standard_Integer
  Transfer_TransferProcess <ActorHandle,TransferMap,StartObject,
                            MapHasher,ListHandle,Iterator>
  ::RootIndex (const StartObject& theStart) const
  {
    Standard_Integer anIndex = MapIndex (theStart);
    if (anIndex == 0) return 0;
    return myRoots.FindIndex (anIndex);
  }

  //=======================================================================
  //function : NestingLevel
  //purpose  : 
  //=======================================================================
  
  template <class ActorHandle, class TransferMap, class StartObject,
            class MapHasher, class ListHandle, class Iterator>
  Standard_Integer
  Transfer_TransferProcess <ActorHandle,TransferMap,StartObject,
                            MapHasher,ListHandle,Iterator>
  ::NestingLevel () const
  {
    return myLevel;
  }

  //=======================================================================
  //function : ResetNestingLevel
  //purpose  : 
  //=======================================================================
  
  template <class ActorHandle, class TransferMap, class StartObject,
            class MapHasher, class ListHandle, class Iterator>
  void
  Transfer_TransferProcess <ActorHandle,TransferMap,StartObject,
                            MapHasher,ListHandle,Iterator>
  ::ResetNestingLevel ()
  {
    myLevel = 0;
  }

  //  ########################################################################
  //  ....                        SCOPE MANAGEMENT                        ....


  //======================================================================
  //Purpose : gka TRJ9 for writing SDR for solid
  //          Check if aBinder has already been bound to the result aBinder.
  //======================================================================

  // static Standard_Boolean Contains(const Handle(Transfer_Binder)& resbinder, 
  // 				 const Handle(Transfer_Binder)& addbinder)
  // {
  //   Handle(Transfer_Binder) tmpbind = resbinder;
  //   for ( ; ! tmpbind.IsNull(); tmpbind = tmpbind->NextResult() )
  //     if ( tmpbind == addbinder ) return Standard_True;
  //   return Standard_False;
  // }

  //  ########################################################################
  //  ....                    AUTOMATION OF TRANSFER                   ....

  //  ##    ##    ##    ##    ##  Basic Functions  ##    ##    ##    ##    ##


  //=======================================================================
  //function : Recognize
  //purpose  : 
  //=======================================================================

  template <class ActorHandle, class TransferMap, class StartObject,
            class MapHasher, class ListHandle, class Iterator>
  Standard_Boolean
  Transfer_TransferProcess <ActorHandle,TransferMap,StartObject,
                            MapHasher,ListHandle,Iterator>
  ::Recognize(const StartObject& theStart) const
  {
    ActorHandle anActor = myActor;
    // It scans up to have a next result
    while (!anActor.IsNull())
    {
      if (anActor->Recognize (theStart))
        return Standard_True;
      anActor = anActor->Next();
    }
    return Standard_False;
  }
  
//=======================================================================
//function : Transferring
//purpose  : 
//=======================================================================

  template <class ActorHandle, class TransferMap, class StartObject,
            class MapHasher, class ListHandle, class Iterator>
  Handle(Transfer_Binder)
  Transfer_TransferProcess <ActorHandle,TransferMap,StartObject,
                            MapHasher,ListHandle,Iterator>
  ::Transferring (const StartObject& theStart)
  {
    Handle(Transfer_Binder) aFormer = FindAndMask(theStart);

    // Was the Transfer already done with Result?
    if (!aFormer.IsNull())
    {
      if (aFormer->HasResult())
      {
        aFormer->SetAlreadyUsed();
        return aFormer;
      }

      // Initial state: perhaps already done ... or infeasible!
      Transfer_StatusExec statex = aFormer->StatusExec();
      switch (statex)
      {
      case Transfer_StatusInitial :
        break;
      case Transfer_StatusDone :

        myMessenger << " .. and Transfer done" << endl;

        return aFormer;
      case Transfer_StatusRun :
        aFormer->SetStatusExec(Transfer_StatusLoop);
        return aFormer;
      case Transfer_StatusError :
        if (myTrace)
        {
          myMessenger << "                  *** Transfer in Error Status  :" << endl;
          StartTrace (aFormer, theStart, myLevel,0);
        }
        else StartTrace (aFormer, theStart,myLevel,4);
        Transfer_TransferFailure::Raise
          ("TransferProcess : Transfer in Error Status");
      case Transfer_StatusLoop :
        if (myTrace)
        {
          myMessenger << "                  *** Transfer  Head of Dead Loop  :" << endl;
          StartTrace (aFormer, theStart, myLevel,0);
        }
        else StartTrace (aFormer, theStart,myLevel,4);
        Transfer_TransferDeadLoop::Raise
          ("TransferProcess : Transfer at Head of a Dead Loop");
      }
#ifdef TRANSLOG
      cout << "Transfer,level "<<myLevel<<Message_Flush;
#endif
      aFormer->SetStatusExec(Transfer_StatusRun);
    }
#ifdef TRANSLOG
    cout << " GO .." << endl;
#endif

    Handle(Transfer_Binder) aBinder;
    Standard_Boolean isNewBind = Standard_False;
    if (myToHandleErr)
    {
      // Transfer under protection exceptions (for notification actually)
      Standard_Integer anOldLevel = myLevel;
      try
      {
        OCC_CATCH_SIGNALS
          aBinder = TransferProduct(theStart);
      }
      catch (Transfer_TransferDeadLoop)
      {
        if (aBinder.IsNull())
        {
          myMessenger << "                  *** Dead Loop with no Result" << endl;
          if (myTrace) StartTrace (aBinder, theStart, myLevel-1,0);
          aBinder = new Transfer_VoidBinder;
          Bind (theStart,aBinder);  isNewBind = Standard_True;
        } else if (aBinder->StatusExec() == Transfer_StatusLoop) {
          if (myTrace) {
            myMessenger << "                  *** Dead Loop : Finding head of Loop :" << endl;
            StartTrace (aBinder, theStart, myLevel-1,0);
          }
          else StartTrace (aBinder, theStart,myLevel-1,4);
          Transfer_TransferFailure::Raise("TransferProcess : Head of Dead Loop");
          // In other words, exchange of exception (it is out of the loop)
        } else {
          if (myTrace) {
            myMessenger << "                  *** Dead Loop : Actor in Loop :" << endl;
            StartTrace (aBinder, theStart, myLevel-1,0);
          }
        }
        aBinder->AddFail("Transfer in dead Loop");
        myLevel = anOldLevel;
      }
      catch (Standard_Failure) {
        if (aBinder.IsNull()) {
          myMessenger << "                  *** Exception Raised with no Result" << endl;
          aBinder = new Transfer_VoidBinder;
          Bind (theStart,aBinder);  isNewBind = Standard_True;
        }
        aBinder->AddFail("Transfer stopped by exception raising");
        if (myTrace) {
          myMessenger << "    *** Raised : " << Standard_Failure::Caught()->GetMessageString() << endl;
          StartTrace (aBinder, theStart, myLevel-1,4);
        }
        myLevel = anOldLevel;
      }
    }

    // Transfer is non protected (for example, dbx hand in case of a crash by Raise)
    else  aBinder = TransferProduct(theStart);

    // Conclusion : Save the Map

    if (!isNewBind && !aBinder.IsNull())
    {
      if (aFormer.IsNull())
      {
        // Maybe <myActor> made ​​him even Bind
        if (!IsBound(theStart))
          Bind(theStart,aBinder);     //result = 0 category
        else
        {
          // gka TRJ9 for writing SDR for solid
          Rebind(theStart,aBinder); // test_pattern.sat
        }
      }
      else Rebind(theStart,aBinder);
      // (Therefore, <myLastBinder> is <aBinder>)
#ifdef TRANSLOG
      cout << " ... OK" << endl;
#endif
    }
    else
    {
      //= by ABV: 5 Oct 97: nothing generated, but aFormer can be in run state - drop it
      //= ASK: may be set it to StatusInitial ?
      if ( ! aFormer.IsNull() ) aFormer->SetStatusExec ( Transfer_StatusDone ); //+
      Handle(Transfer_Binder)  aNullBinder;
      return aNullBinder;    // Binder Null ... que faire d autre ?
    }

    // Manage Roots (if planned)
    if (myRootLevel >= myLevel) {
      myRootLevel = 0;
      if (myToManageRoot && aBinder->Status() != Transfer_StatusVoid)
      {
        SetRoot (theStart);
      }
    }
    return myLastBinder;
  }

//=======================================================================
//function : TransferProduct
//purpose  : 
//=======================================================================

  template <class ActorHandle, class TransferMap, class StartObject,
            class MapHasher, class ListHandle, class Iterator>
  Handle(Transfer_Binder)
  Transfer_TransferProcess <ActorHandle,TransferMap,StartObject,
                            MapHasher,ListHandle,Iterator>
  ::TransferProduct (const StartObject& theStart)
{
  myLevel ++;
  Handle(Transfer_Binder) aBinder;
  ActorHandle anActor = myActor;

  while (!anActor.IsNull()) {
    if (anActor->Recognize (theStart)) aBinder = anActor->Transferring(theStart,this);
    else aBinder.Nullify();
    if (!aBinder.IsNull()) break;
    anActor = anActor->Next();
  }
  if (aBinder.IsNull())
  {

    if (myLevel > 0) myLevel --;
    return aBinder;
  }

  if (myRootLevel == 0 && aBinder->StatusExec() == Transfer_StatusDone)
    myRootLevel = myLevel - 1;

  if (myLevel > 0) myLevel --;
  return aBinder;
}


//=======================================================================
//function : Transfer
//purpose  : 
//=======================================================================

  template <class ActorHandle, class TransferMap, class StartObject,
            class MapHasher, class ListHandle, class Iterator>
  Standard_Boolean
  Transfer_TransferProcess <ActorHandle,TransferMap,StartObject,
                            MapHasher,ListHandle,Iterator>
  ::Transfer(const StartObject& theStart)
{
  Handle(Transfer_Binder) aBinder = Transferring (theStart);
  return (!aBinder.IsNull());
}

  //  #########################################################################
  //  ....                      Error Handling + Trace                     ....
  
  //=======================================================================
  //function : SetErrorHandle
  //purpose  : 
  //=======================================================================

  template <class ActorHandle, class TransferMap, class StartObject,
            class MapHasher, class ListHandle, class Iterator>
  void
  Transfer_TransferProcess <ActorHandle,TransferMap,StartObject,
                            MapHasher,ListHandle,Iterator>
  ::SetErrorHandle (const Standard_Boolean isError)
  {
    myToHandleErr = isError;
  }

  //=======================================================================
  //function : ErrorHandle
  //purpose  : 
  //=======================================================================

  template <class ActorHandle, class TransferMap, class StartObject,
            class MapHasher, class ListHandle, class Iterator>
  Standard_Boolean
  Transfer_TransferProcess <ActorHandle,TransferMap,StartObject,
                            MapHasher,ListHandle,Iterator>
  ::ErrorHandle() const
  {
    return myToHandleErr;
  }

  //=======================================================================
  //function : StartTrace
  //purpose  : 
  //=======================================================================

  template <class ActorHandle, class TransferMap, class StartObject,
            class MapHasher, class ListHandle, class Iterator>
  void
  Transfer_TransferProcess <ActorHandle,TransferMap,StartObject,
                            MapHasher,ListHandle,Iterator>
  ::StartTrace (const Handle(Transfer_Binder)& theBinder,
                const StartObject& theStart,
                const Standard_Integer theLevel,
                const Standard_Integer theMode) const 
  {
    // ###  Fail (Roots:50)  //!  Start theStart->DynamicType()
    // ###  Fail (Roots:50)  //!  Start id:#label.. Type:theStart->DynamicType()
    if (myTrace > 3) {  // Internal to be switch when searching bug (trace >= 4)
      if (theMode == 1) myMessenger << "  ###  Fail";
      if (theMode == 2) myMessenger << "  ###  Warning";
      if (theMode == 3) myMessenger << "  ###  New Root n0 " << myRoots.Extent();
      if (theMode == 4) myMessenger << "  ###  Exception";
      if (theMode == 5) myMessenger << "  ###  Substitution";
      if (theMode == 6) myMessenger << "  ###  Information";
      if (theLevel > 1)
        myMessenger << " (nested)";  // " at nesting Level:"<<theLevel;
      if (theMode >= 0 && theMode != 3)
        myMessenger << " at " << myRoots.Extent() << " Roots";
    }
    if (!theStart.IsNull()) PrintTrace (theStart,myMessenger);

    if (!theBinder.IsNull()) {   // old: if IsNull sout <<endl<< "  ---  Not Bound";
      Handle(Transfer_Binder) bnd = theBinder;
      Standard_Boolean hasres = Standard_False, isused = Standard_False;
      while (!bnd.IsNull()) {
        if (bnd->Status() != Transfer_StatusVoid) {
  // ---  Result Type: theBinder->ResultType()  ---  Binder : theBinder->DynamicType();
    if (!hasres)
      myMessenger << "\n  ---  Result Type : ";
    else 
      myMessenger << " , ";
    myMessenger << bnd->ResultTypeName();
  //  CKY 9-JAN-1999:  waiting for XSTEP Kernel message (not IGES_2075)
  /*        Message_Msg Msg2075("IGES_2075");
            Msg2075.AddString(bnd->ResultTypeName());
            Msg2075.TraceEver();  */
    hasres = Standard_True;
    if (bnd->Status() == Transfer_StatusUsed) isused = Standard_True;
        }
        bnd = bnd->NextResult();
      }
      if (!hasres && theMode > 2) {
        myMessenger << "\n  ---  No Result recorded";
  //  CKY 9-JAN-1999 : waiting for XSTEP Kernel message
  //     (not IGES_2075, no reference to specifically TopoDS_Shape)
  /*       Message_Msg Msg2075("IGES_2075");
           Msg2075.AddString("No TopoDS_Shape");
           Msg2075.TraceEver();  */
      }
  //old    if (isused) sout << "  //!    (Already Used in another Transfer)";
    }
    myMessenger << endl;
  }


  //=======================================================================
  //function : PrintTrace
  //purpose  : 
  //=======================================================================

  template <class ActorHandle, class TransferMap, class StartObject,
            class MapHasher, class ListHandle, class Iterator>
  void
  Transfer_TransferProcess <ActorHandle,TransferMap,StartObject,
                            MapHasher,ListHandle,Iterator>
  ::PrintTrace (const StartObject& theStart,
                const Handle(Message_Messenger)& myMessenger) const
  {
    if (!theStart.IsNull())
      myMessenger <<" Type:" << theStart->DynamicType()->Name();
  }

  //=======================================================================
  //function : IsLooping
  //purpose  : 
  //=======================================================================

  template <class ActorHandle, class TransferMap, class StartObject,
            class MapHasher, class ListHandle, class Iterator>
  Standard_Boolean
  Transfer_TransferProcess <ActorHandle,TransferMap,StartObject,
                            MapHasher,ListHandle,Iterator>
  ::IsLooping (const Standard_Integer theLevel) const
  {
    return theLevel > NbMapped();
  }

  //  #########################################################################
  //  ....                            RESULTS                            ....

  //=======================================================================
  //function : RootResult
  //purpose  : 
  //=======================================================================

  template <class ActorHandle, class TransferMap, class StartObject,
            class MapHasher, class ListHandle, class Iterator>
  Iterator
  Transfer_TransferProcess <ActorHandle,TransferMap,StartObject,
                            MapHasher,ListHandle,Iterator>
  ::RootResult (const Standard_Boolean isWithStart) const
  {
    Iterator iter(isWithStart);
    Standard_Integer aMax = myRoots.Extent();
    for (Standard_Integer aJ = 1; aJ <= aMax; aJ ++)
    {
      Standard_Integer anI = myRoots.FindKey(aJ);
      Handle(Transfer_Binder) aBinder = MapItem(anI);
      if (aBinder.IsNull()) continue;
      if (isWithStart) iter.Add (aBinder,Mapped(anI));
      else iter.Add (aBinder);
    }
    return iter;
  }

  //=======================================================================
  //function : CompleteResult
  //purpose  : All Results
  //=======================================================================

  template <class ActorHandle, class TransferMap, class StartObject,
            class MapHasher, class ListHandle, class Iterator>
  Iterator
  Transfer_TransferProcess <ActorHandle,TransferMap,StartObject,
                            MapHasher,ListHandle,Iterator>
  ::CompleteResult (const Standard_Boolean isWithStart) const
  {
    Iterator iter(isWithStart);
    Standard_Integer aMax = NbMapped();
    for (Standard_Integer anI = 1; anI <= aMax; anI ++) {
      Handle(Transfer_Binder) aBinder = MapItem(anI);
      if (aBinder.IsNull()) continue;
      if (isWithStart) iter.Add (aBinder,Mapped(anI));
      else iter.Add (aBinder);
    }
    return iter;
  }

  //=======================================================================
  //function : AbnormalResult
  //purpose  : Transfers a problem  
  //=======================================================================

  template <class ActorHandle, class TransferMap, class StartObject,
            class MapHasher, class ListHandle, class Iterator>
  Iterator
  Transfer_TransferProcess <ActorHandle,TransferMap,StartObject,
                            MapHasher,ListHandle,Iterator>
  ::AbnormalResult() const
  {
    Iterator anIter(Standard_True);
    Standard_Integer aMax = NbMapped();
    for (Standard_Integer anI = 1; anI <= aMax; anI ++) {
      Handle(Transfer_Binder) aBinder = MapItem(anI);
      if (aBinder.IsNull()) continue;
      Transfer_StatusExec aStatExec = aBinder->StatusExec();
      if (aStatExec != Transfer_StatusInitial && aStatExec != Transfer_StatusDone)
        anIter.Add (aBinder,Mapped(anI));  // on note les cas "pas normaux"
    }
    return anIter;
  }

  //=======================================================================
  //function : CheckList
  //purpose  : 
  //=======================================================================

  template <class ActorHandle, class TransferMap, class StartObject,
            class MapHasher, class ListHandle, class Iterator>
  Interface_CheckIterator
  Transfer_TransferProcess <ActorHandle,TransferMap,StartObject,
                            MapHasher,ListHandle,Iterator>
  ::CheckList (const Standard_Boolean isErrOnly) const
  {
    Interface_CheckIterator list;
    Standard_Integer theNum, aMax = NbMapped();
    for (Standard_Integer anI = 1; anI <= aMax; anI ++) {
      Handle(Transfer_Binder) aBinder = MapItem(anI);
      if (aBinder.IsNull()) continue;
      Transfer_StatusExec aStatExec = aBinder->StatusExec();
      Handle(Interface_Check) aCheck = aBinder->Check();
      if (aStatExec != Transfer_StatusInitial && aStatExec != Transfer_StatusDone &&
    !aCheck->HasFailed())
        aCheck->AddFail("Transfer in Abnormal Status (!= Initial or Done)");
      if (!aCheck->HasFailed() && (isErrOnly || aCheck->NbWarnings() == 0)) continue;
      const StartObject& ent = Mapped(anI);
      theNum = CheckNum(ent);
      if (theNum == 0) theNum = anI;
      aCheck->SetEntity(ent);
      list.Add(aCheck,theNum);
    }
    return list;
  }

  //  #########################################################################
  //  ....                       RESULTS BY ENTITY                      ....

  //=======================================================================
  //function : ResultOne
  //purpose  : 
  //=======================================================================

  template <class ActorHandle, class TransferMap, class StartObject,
            class MapHasher, class ListHandle, class Iterator>
  Iterator
  Transfer_TransferProcess <ActorHandle,TransferMap,StartObject,
                            MapHasher,ListHandle,Iterator>
  ::ResultOne (const StartObject& theStart,
               const Standard_Integer theLevel,
               const Standard_Boolean isWithStart) const
  {
    Iterator anIter(isWithStart);
    Standard_Integer aMax = NbMapped();
    Standard_Integer anInd = MapIndex (theStart);
    if (anInd == 0) return anIter;
    Standard_Integer i1 = (theLevel == 0 ? anInd : 1);
    Standard_Integer i2 = (theLevel == 0 ? anInd : aMax);
    Handle(TColStd_HArray1OfInteger) map = new TColStd_HArray1OfInteger (i1,i2,0);
    for (Standard_Integer anI = i1; anI <= i2; anI ++) {
      anInd = map->Value(anI);
      if (anInd == 0) continue;
      Handle(Transfer_Binder) aBinder = MapItem(anI);
      if (aBinder.IsNull()) continue;
      if (isWithStart) anIter.Add (aBinder,Mapped(anInd));
      else anIter.Add (aBinder);
    }
    return anIter;
  }

  //=======================================================================
  //function : CheckListOne
  //purpose  : 
  //=======================================================================

  template <class ActorHandle, class TransferMap, class StartObject,
            class MapHasher, class ListHandle, class Iterator>
  Interface_CheckIterator
  Transfer_TransferProcess <ActorHandle,TransferMap,StartObject,
                            MapHasher,ListHandle,Iterator>
  ::CheckListOne (const StartObject& theStart,const Standard_Integer theLevel,
                  const Standard_Boolean isErrOnly) const
  {
    Interface_CheckIterator aList;
    Standard_Integer aMax = NbMapped();
    Standard_Integer aNum, anInd = MapIndex (theStart);
    if (anInd == 0) return aList;
    Standard_Integer i1 = (theLevel == 0 ? anInd : 1);
    Standard_Integer i2 = (theLevel == 0 ? anInd : aMax);
    Handle(TColStd_HArray1OfInteger) map = new TColStd_HArray1OfInteger (i1,i2,0);

    for (Standard_Integer anI = i1; anI <= i2; anI ++) {
      anInd = map->Value(anI);
      if (anInd == 0) continue;
      Handle(Transfer_Binder) aBinder = MapItem(anInd);
      if (aBinder.IsNull()) continue;
      Transfer_StatusExec aStatExec = aBinder->StatusExec();
      Handle(Interface_Check) aCheck = aBinder->Check();
      if (aStatExec != Transfer_StatusInitial && aStatExec != Transfer_StatusDone &&
    !aCheck->HasFailed())
        aCheck->AddFail("Transfer in Abnormal Status (!= Initial or Done)");
      if (!aCheck->HasFailed() && (isErrOnly || aCheck->NbWarnings() == 0)) continue;
      const StartObject& anEnt = Mapped(anInd);
      aNum = CheckNum(anEnt);  if (aNum == 0) aNum = anInd;
      aCheck->SetEntity(anEnt);
      aList.Add(aCheck,aNum);
    }
    return aList;
  }

  //=======================================================================
  //function : IsCheckListEmpty
  //purpose  : 
  //=======================================================================
  template <class ActorHandle, class TransferMap, class StartObject,
            class MapHasher, class ListHandle, class Iterator>
  Standard_Boolean
  Transfer_TransferProcess <ActorHandle,TransferMap,StartObject,
                            MapHasher,ListHandle,Iterator>
  ::IsCheckListEmpty (const StartObject& theStart,
                      const Standard_Integer theLevel,
                      const Standard_Boolean isErrOnly) const
  {
    Standard_Integer aMax = NbMapped();
    Standard_Integer anInd = MapIndex (theStart);
    if (anInd == 0) return Standard_False;
    Standard_Integer i1 = (theLevel == 0 ? anInd : 1);
    Standard_Integer i2 = (theLevel == 0 ? anInd : aMax);
    Handle(TColStd_HArray1OfInteger) map = new TColStd_HArray1OfInteger (i1,i2,0);

    for (Standard_Integer anI = i1; anI <= i2; anI ++) {
      anInd = map->Value(anI);
      if (anInd == 0) continue;
      Handle(Transfer_Binder) aBinder = MapItem(anInd);
      if (aBinder.IsNull()) continue;

      Transfer_StatusExec aStatExec = aBinder->StatusExec();
      Handle(Interface_Check) aCheck = aBinder->Check();
      if (aStatExec != Transfer_StatusInitial && aStatExec != Transfer_StatusDone)
        return Standard_False;
      if (aCheck->HasFailed() || (!isErrOnly && aCheck->NbWarnings() > 0)) return Standard_False;
    }
    return Standard_True;
  }

  //=======================================================================
  //function : RemoveResult
  //purpose  : 
  //=======================================================================

  template <class ActorHandle, class TransferMap, class StartObject,
            class MapHasher, class ListHandle, class Iterator>
  void
  Transfer_TransferProcess <ActorHandle,TransferMap,StartObject,
                            MapHasher,ListHandle,Iterator>
  ::RemoveResult (const StartObject& theStart,
                                     const Standard_Integer theLevel,
                                     const Standard_Boolean toCompute)
  {
    Standard_Integer aMax = NbMapped();
    Standard_Integer anInd = MapIndex (theStart);
    if (anInd == 0) return;
    Standard_Integer i1 = (theLevel == 0 ? anInd : 1);
    Standard_Integer i2 = (theLevel == 0 ? anInd : aMax);
    Handle(TColStd_HArray1OfInteger) map = new TColStd_HArray1OfInteger (i1,i2,0);

    Standard_Integer anI; // svv Jan11 2000 : porting on DEC
    for (anI = i1; anI <= i2; anI ++) {
      anInd = map->Value(anI);
      if (anInd == 0) continue;
      Handle(Transfer_Binder) aBinder = MapItem(anInd);
      if (aBinder.IsNull()) continue;
    }
  }

  template <class ActorHandle, class TransferMap, class StartObject,
            class MapHasher, class ListHandle, class Iterator>
  Standard_Integer
  Transfer_TransferProcess <ActorHandle,TransferMap,StartObject,
                            MapHasher,ListHandle,Iterator>
  ::CheckNum (const StartObject& ) const
  {
    return 0;
  }

  //=======================================================================
  //function : SetProgress
  //purpose  : Sets Progress indicator
  //=======================================================================

  template <class ActorHandle, class TransferMap, class StartObject,
            class MapHasher, class ListHandle, class Iterator>
  void
  Transfer_TransferProcess <ActorHandle,TransferMap,StartObject,
                            MapHasher,ListHandle,Iterator>
  ::SetProgress (const Handle(Message_ProgressIndicator)& theProgress)
  {
    myProgress = theProgress;
  }

  //=======================================================================
  //function : GetProgress
  //purpose  : Returns Progress indicator
  //=======================================================================

  template <class ActorHandle, class TransferMap, class StartObject,
            class MapHasher, class ListHandle, class Iterator>
  Handle(Message_ProgressIndicator)
  Transfer_TransferProcess <ActorHandle,TransferMap,StartObject,
                            MapHasher,ListHandle,Iterator>
  ::GetProgress() const
  {
    return myProgress;
  }

#endif