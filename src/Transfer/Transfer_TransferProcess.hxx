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
          class TheStart,
          class TheMapHasher,
          class ListHandle,
          class Iterator>
class Transfer_TransferProcess : public MMgt_TShared
{
private:
  Handle(Standard_Transient) myNullTrans;
protected:
  //! to handle or not exception raisings
  Standard_Boolean myToHandleErr; 
  //! trace theLevel (exceptions,errors, etc...)
  Standard_Integer myTrace;
  //! theMessenger for sending messages
  Handle(Message_Messenger) myMessenger;

  Standard_Integer myLevel;
  //! theLevel of root at each time (not allways 1)
  Standard_Integer myRootLevel;
  //! Flag for Root Management set or not
  Standard_Boolean myToManageRoot;
  //! indices of roots in the map
  TColStd_IndexedMapOfInteger myRoots;

  //! Last Starting Object Bound
  TheStart myLastObj;
  //! Its attached Binder (noted to optimize)
  Handle(Transfer_Binder) myLastBinder;
  //! And Index
  Standard_Integer  myIndex;
  ActorHandle myActor;
  TransferMap myMap;
  //! Progress indicator
  Handle(Message_ProgressIndicator) myProgress;
  
public:

  //=======================================================================
  //function : Transfer_TransferProcess
  //purpose  : 
  //=======================================================================

  Standard_EXPORT Transfer_TransferProcess (const Standard_Integer theNb)
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

  Standard_EXPORT Transfer_TransferProcess (const Handle(Message_Messenger)& theMessenger,
                                            const Standard_Integer theNb)
  : myMap (theNb)
  {
    myToHandleErr  = Standard_True;
    myToManageRoot = Standard_False;
    myLevel = 0;     myRootLevel  = 0;
    SetMessenger (theMessenger);
    myTrace = 1;
    myIndex = 0;
  }

  Standard_EXPORT  void Clear ()
  {
    myLevel = 0;     myRootLevel  = 0;
    myRoots.Clear();
    myMap.Clear();
    myIndex = 0;  myLastObj.Nullify();  myLastBinder.Nullify();
  }

  Standard_EXPORT void Clean ()
  {
    Standard_Integer anI, aNb = NbMapped();
    Standard_Integer aJ,unb = 0;
    for (anI = 1; anI <= aNb; anI ++)
    {
      if (myMap(anI).IsNull()) unb ++;
    }
    if (unb == 0) return;

    TColStd_Array1OfInteger unbs (1,aNb);  unbs.Init(0);
    Transfer_TransferMap newmap (aNb*2);
    for (anI = 1; anI <= aNb; anI ++)
    {
      TheStart ent = Mapped(anI);
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

  Standard_EXPORT void Resize (const Standard_Integer theNb)
  {
    if (theNb > myMap.NbBuckets())
      myMap.ReSize(theNb);
  }

  //=======================================================================
  //function : SetActor
  //purpose  : 
  //=======================================================================

  Standard_EXPORT void  SetActor(const ActorHandle& theActor)
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

  Standard_EXPORT ActorHandle Actor () const 
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

  Standard_EXPORT Handle(Transfer_Binder) Find (const TheStart& theStart) const
  {
    if (myLastObj == theStart) {
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
    Handle(Transfer_Binder)  nulbinder;
    return nulbinder;
  }

  //=======================================================================
  //function : IsBound
  //purpose  : 
  //=======================================================================

  Standard_EXPORT Standard_Boolean IsBound(const TheStart& theStart) const
  {
    Handle(Transfer_Binder) theBinder = Find(theStart); //,categ); skl
    if (theBinder.IsNull()) return Standard_False;
    return theBinder->HasResult();
  }

  //=======================================================================
  //function : IsAlreadyUsed
  //purpose  : 
  //=======================================================================

  Standard_EXPORT Standard_Boolean IsAlreadyUsed(const TheStart& theStart) const
  //							 const Standard_Integer categ) const
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

  Standard_EXPORT Handle(Transfer_Binder) FindAndMask(const TheStart& theStart)
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

  Standard_EXPORT void Bind (const TheStart& theStart,
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

  Standard_EXPORT void Rebind (const TheStart& theStart,
                               const Handle(Transfer_Binder)& theBinder)
  {
    Bind(theStart,theBinder);
  }

  //=======================================================================
  //function : Unbind
  //purpose  : 
  //=======================================================================

  Standard_EXPORT Standard_Boolean Unbind (const TheStart& theStart)
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

  Standard_EXPORT Handle(Transfer_Binder) FindElseBind(const TheStart& theStart)
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

  Standard_EXPORT void SetMessenger (const Handle(Message_Messenger)& theMessenger)
  {
    if ( theMessenger.IsNull() )
      myMessenger = Message::DefaultMessenger();
    else   
      myMessenger = theMessenger;
  }

  //=======================================================================
  //function : Messenger
  //purpose  : 
  //=======================================================================

  Standard_EXPORT Handle(Message_Messenger) Messenger () const
  {
    return myMessenger;
  }

  //=======================================================================
  //function : SetTraceLevel
  //purpose  : 
  //=======================================================================

  Standard_EXPORT void SetTraceLevel (const Standard_Integer tracelev)
  {
    myTrace = tracelev;
  }

  //=======================================================================
  //function : TraceLevel
  //purpose  : 
  //=======================================================================

  Standard_EXPORT Standard_Integer TraceLevel () const
  {
    return myTrace;
  }

  //=======================================================================
  //function : SendFail
  //purpose  : 
  //=======================================================================

  Standard_EXPORT void  SendFail (const TheStart& theStart,
                                  const Message_Msg& theMsg)
  {
    AddFail(theStart,theMsg);
  }


  //=======================================================================
  //function : SendWarning
  //purpose  : 
  //=======================================================================

  Standard_EXPORT void  SendWarning (const TheStart& theStart,
                                     const Message_Msg& theMsg)
  {
    AddWarning(theStart,theMsg);
  }


  //=======================================================================
  //function : SendMsg
  //purpose  : 
  //=======================================================================

  Standard_EXPORT void  SendMsg (const TheStart& theStart,
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

  Standard_EXPORT void AddFail (const TheStart& theStart,
                                const Standard_CString mess,
                                const Standard_CString orig)
  {
    Handle(Transfer_Binder) theBinder = FindAndMask(theStart);
    if (theBinder.IsNull()) {
      theBinder = new Transfer_VoidBinder;
      Bind (theStart,theBinder);
    }
    theBinder->AddFail (mess,orig);
    if (myTrace > 0) {
      StartTrace (theBinder,theStart,myLevel,1);
      myMessenger << "    --> Fail : " << mess;
      if (orig[0] != '\0'&&myTrace>2) myMessenger << " [from: " << orig << "]";
      myMessenger << endl;
    }
  }


  //=======================================================================
  //function : AddError
  //purpose  : 
  //=======================================================================

  Standard_EXPORT void AddError(const TheStart& theStart,
                                const Standard_CString mess,
                                const Standard_CString orig)
  {
    AddFail (theStart,mess,orig);
  }

  //=======================================================================
  //function : AddFail
  //purpose  : 
  //=======================================================================

  Standard_EXPORT void AddFail (const TheStart& theStart,
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

  Standard_EXPORT void AddWarning (const TheStart& theStart,
                                   const Standard_CString mess,
                                   const Standard_CString orig)
  {
    Handle(Transfer_Binder) aBinder = FindAndMask(theStart);
    if (aBinder.IsNull()) {
      aBinder = new Transfer_VoidBinder;
      Bind (theStart,aBinder);
    }
    aBinder->AddWarning(mess,orig);
    if (myTrace > 1) {
      StartTrace (aBinder,theStart,myLevel,2);
      myMessenger << "    --> Warning : " << mess;
      if (orig[0] != '\0'&&myTrace>2) myMessenger << " [from: " << orig << "]";
      myMessenger << endl;
    }
  }

  //=======================================================================
  //function : AddWarning
  //purpose  : 
  //=======================================================================

  Standard_EXPORT void AddWarning (const TheStart& theStart,
                                   const Message_Msg& theMsg)
  {
    if (theMsg.IsEdited()) AddWarning (theStart,TCollection_AsciiString(theMsg.Value()).ToCString(),
             TCollection_AsciiString(theMsg.Original()).ToCString());
    else AddWarning (theStart,TCollection_AsciiString(theMsg.Value()).ToCString());
  }

  //=======================================================================
  //function : Mend
  //purpose  : 
  //=======================================================================

  Standard_EXPORT void Mend (const TheStart& theStart,
                             const Standard_CString pref)
  {
    Handle(Transfer_Binder) aBinder = FindAndMask(theStart);
    if (aBinder.IsNull()) return;
    Handle(Interface_Check) ach =  aBinder->CCheck();
    ach->Mend (pref);
  }

  //=======================================================================
  //function : Check
  //purpose  : 
  //=======================================================================

  Standard_EXPORT Handle(Interface_Check) Check(const TheStart& theStart) const
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

  Standard_EXPORT void BindTransient (const TheStart& theStart,
                                      const Handle(Standard_Transient)& res)
  {
    if (res.IsNull()) return;
    Handle(Transfer_Binder) aFormer = Find(theStart);//,categ);skl
    Handle(Transfer_SimpleBinderOfTransient) aBinder =
      Handle(Transfer_SimpleBinderOfTransient)::DownCast(aFormer);
    if (!aBinder.IsNull()) {
      if (aBinder->Status() == Transfer_StatusVoid) { aBinder->SetResult(res); return; }
    }
    aBinder = new Transfer_SimpleBinderOfTransient;
    aBinder->SetResult (res);
    if (aFormer.IsNull()) Bind(theStart,aBinder);
    else Rebind(theStart,aBinder);
  }

  //=======================================================================
  //function : FindTransient
  //purpose  : 
  //=======================================================================

  Standard_EXPORT const Handle(Standard_Transient)& FindTransient
                                                    (const TheStart& theStart) const
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

  Standard_EXPORT void BindMultiple(const TheStart& theStart)
  {
    Handle(Transfer_Binder) aBinder = FindAndMask (theStart);
    if (!aBinder.IsNull()) {
      if (!aBinder->IsKind(STANDARD_TYPE(Transfer_MultipleBinder))) {
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

  Standard_EXPORT void AddMultiple(const TheStart& theStart,
                                   const Handle(Standard_Transient)& res)
  {
    Handle(Transfer_Binder) aBinder = FindAndMask(theStart);
    Handle(Transfer_MultipleBinder) multr =
      Handle(Transfer_MultipleBinder)::DownCast(aBinder);
    if (multr.IsNull()) {
      StartTrace (aBinder,theStart,myLevel,4);
      if (aBinder.IsNull()) Transfer_TransferFailure::Raise
        ("TransferProcess : AddMultiple, nothing bound");
      else                 Transfer_TransferFailure::Raise
        ("TransferProcess : AddMultiple, Binder not a MultipleBinder");
    }
    multr->AddResult(res);
  }

  //=======================================================================
  //function : FindTypedTransient
  //purpose  : 
  //=======================================================================

  Standard_EXPORT Standard_Boolean FindTypedTransient
           (const TheStart& theStart,
            const Handle(Standard_Type)& atype,
            Handle(Standard_Transient)& val) const
  {
    return GetTypedTransient (Find(theStart),atype,val);
  }


  //=======================================================================
  //function : GetTypedTransient
  //purpose  : 
  //=======================================================================

  Standard_EXPORT Standard_Boolean GetTypedTransient
         (const Handle(Transfer_Binder)& aBinder,
          const Handle(Standard_Type)& atype,
          Handle(Standard_Transient)& val) const
  {
    return Transfer_SimpleBinderOfTransient::GetTypedResult(aBinder,atype,val);
  }


  //  ##    ##    ##    ##    ##    Acces Atomique    ##    ##    ##    ##    ##
  //    (ne gere pas le scope mais donne acces aux categories)

  //=======================================================================
  //function : NbMapped
  //purpose  : 
  //=======================================================================

  Standard_EXPORT Standard_Integer NbMapped () const
  {
    return myMap.Extent();
  }

  //=======================================================================
  //function : Mapped
  //purpose  : 
  //=======================================================================

  Standard_EXPORT const TheStart& Mapped(const Standard_Integer num) const
  {
    return myMap.FindKey(num);
  }

  //=======================================================================
  //function : MapIndex
  //purpose  : 
  //=======================================================================

  Standard_EXPORT Standard_Integer MapIndex(const TheStart& theStart) const
  {
    return myMap.FindIndex(theStart);
  }

  //=======================================================================
  //function : MapItem
  //purpose  : 
  //=======================================================================

  Standard_EXPORT Handle(Transfer_Binder) MapItem(const Standard_Integer num) const
  {
    Handle(Transfer_Binder) aBinder = myMap.FindFromIndex(num);
    return aBinder;
  }


  //  ########################################################################
  //  ....                         ROOT MANAGEMENT                        ....

  //=======================================================================
  //function : SetRoot
  //purpose  : 
  //=======================================================================

  Standard_EXPORT void SetRoot (const TheStart& theStart)
  {
    Standard_Integer index = MapIndex(theStart);
    if (index == 0) {
      return;
    }

    myRoots.Add(index);
    if (myTrace > 2) StartTrace (MapItem(index),theStart,myLevel,3);
  }

  //=======================================================================
  //function : SetRootManagement
  //purpose  : 
  //=======================================================================

  Standard_EXPORT void SetRootManagement(const Standard_Boolean stat)
  {
    myToManageRoot = stat;
  }

  //=======================================================================
  //function : NbRoots
  //purpose  : 
  //=======================================================================

  Standard_EXPORT Standard_Integer NbRoots () const
  {
    return myRoots.Extent();
  }

  //=======================================================================
  //function : Root
  //purpose  : 
  //=======================================================================

  Standard_EXPORT const TheStart& Root(const Standard_Integer num) const
  {
    Standard_Integer anInd = 0;
    if (num > 0 && num <= myRoots.Extent()) anInd = myRoots.FindKey(num);
    return myMap.FindKey (anInd);
  }
  
  //=======================================================================
  //function : RootItem
  //purpose  : 
  //=======================================================================

  Standard_EXPORT Handle(Transfer_Binder) RootItem(const Standard_Integer num) const
  {
    Standard_Integer anInd = 0;
    if (num > 0 && num <= myRoots.Extent()) anInd = myRoots.FindKey(num);
    return myMap.FindFromIndex(anInd);//->Search(categ);skl
  }
  
  //=======================================================================
  //function : RootIndex
  //purpose  : 
  //=======================================================================

  Standard_EXPORT Standard_Integer RootIndex(const TheStart& theStart) const
  {
    Standard_Integer index = MapIndex(theStart);
    if (index == 0) return 0;
    return myRoots.FindIndex(index);
  }

  //=======================================================================
  //function : NestingLevel
  //purpose  : 
  //=======================================================================

  Standard_EXPORT Standard_Integer NestingLevel () const
  {
    return myLevel;
  }

  //=======================================================================
  //function : ResetNestingLevel
  //purpose  : 
  //=======================================================================

  Standard_EXPORT void ResetNestingLevel ()
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
  //  ....                    AUTOMATISMES DE TRANSFERT                   ....

  //  ##    ##    ##    ##    ##  Fonctions de Base  ##    ##    ##    ##    ##


  //=======================================================================
  //function : Recognize
  //purpose  : 
  //=======================================================================

  Standard_EXPORT Standard_Boolean Recognize(const TheStart& theStart) const
  {
    ActorHandle aActor = myActor;
    // It scans up to have a next result
    while (!aActor.IsNull())
    {
      if (aActor->Recognize (theStart))
        return Standard_True;
      aActor = aActor->Next();
    }
    return Standard_False;
  }

  //  #########################################################################
  //  ....                      Error Handling + Trace                     ....
  
  //=======================================================================
  //function : SetErrorHandle
  //purpose  : 
  //=======================================================================

  Standard_EXPORT void SetErrorHandle(const Standard_Boolean isError)
  {
    myToHandleErr = isError;
  }

  //=======================================================================
  //function : ErrorHandle
  //purpose  : 
  //=======================================================================

  Standard_EXPORT Standard_Boolean ErrorHandle() const
  {
    return myToHandleErr;
  }

  //=======================================================================
  //function : StartTrace
  //purpose  : 
  //=======================================================================

  Standard_EXPORT void StartTrace(const Handle(Transfer_Binder)& theBinder,
                                  const TheStart& theStart,
                                  const Standard_Integer theLevel,
                                  const Standard_Integer theMode) const 
  {
    // ###  Fail (Roots:50)  --  Start theStart->DynamicType()
    // ###  Fail (Roots:50)  --  Start id:#label.. Type:theStart->DynamicType()
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
  //old    if (isused) sout << "  --    (Already Used in another Transfer)";
    }
    myMessenger << endl;
  }


  //=======================================================================
  //function : PrintTrace
  //purpose  : 
  //=======================================================================

  Standard_EXPORT void PrintTrace(const TheStart& theStart,
                                  const Handle(Message_Messenger)& theMessenger) const
  {
    if (!theStart.IsNull())
      theMessenger <<" Type:" << theStart->DynamicType()->Name();
  }

  //=======================================================================
  //function : IsLooping
  //purpose  : 
  //=======================================================================

  Standard_EXPORT Standard_Boolean  IsLooping (const Standard_Integer theLevel) const 
  {
    return theLevel > NbMapped();
  }

  //  #########################################################################
  //  ....                            RESULTS                            ....

  //=======================================================================
  //function : RootResult
  //purpose  : 
  //=======================================================================

  Standard_EXPORT Iterator RootResult (const Standard_Boolean isWithStart = Standard_False) const
  {
    Iterator iter(isWithStart);
    Standard_Integer aMax = myRoots.Extent();
    for (Standard_Integer aJ = 1; aJ <= aMax; aJ ++) {
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

  Standard_EXPORT Iterator CompleteResult (const Standard_Boolean isWithStart = Standard_False) const
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

  Standard_EXPORT Iterator AbnormalResult() const
  {
    Iterator iter(Standard_True);
    Standard_Integer aMax = NbMapped();
    for (Standard_Integer anI = 1; anI <= aMax; anI ++) {
      Handle(Transfer_Binder) aBinder = MapItem(anI);
      if (aBinder.IsNull()) continue;
      Transfer_StatusExec aStatExec = aBinder->StatusExec();
      if (aStatExec != Transfer_StatusInitial && aStatExec != Transfer_StatusDone)
        iter.Add (aBinder,Mapped(anI));  // on note les cas "pas normaux"
    }
    return iter;
  }

  //=======================================================================
  //function : CheckList
  //purpose  : 
  //=======================================================================

  Standard_EXPORT Interface_CheckIterator CheckList (const Standard_Boolean isErrOnly) const
  {
    Interface_CheckIterator list;
    Standard_Integer num, aMax = NbMapped();
    for (Standard_Integer anI = 1; anI <= aMax; anI ++) {
      Handle(Transfer_Binder) aBinder = MapItem(anI);
      if (aBinder.IsNull()) continue;
      Transfer_StatusExec aStatExec = aBinder->StatusExec();
      Handle(Interface_Check) aCheck = aBinder->Check();
      if (aStatExec != Transfer_StatusInitial && aStatExec != Transfer_StatusDone &&
    !aCheck->HasFailed())
        aCheck->AddFail("Transfer in Abnormal Status (!= Initial or Done)");
      if (!aCheck->HasFailed() && (isErrOnly || aCheck->NbWarnings() == 0)) continue;
      const TheStart& ent = Mapped(anI);
      num = CheckNum(ent);
      if (num == 0) num = anI;
      aCheck->SetEntity(ent);
      list.Add(aCheck,num);
    }
    return list;
  }

  //  #########################################################################
  //  ....                       RESULTS BY ENTITY                      ....

  //=======================================================================
  //function : ResultOne
  //purpose  : 
  //=======================================================================

  Standard_EXPORT Iterator ResultOne (const TheStart& theStart,
                                      const Standard_Integer theLevel,
                                      const Standard_Boolean isWithStart = Standard_False) const
  {
    Iterator iter(isWithStart);
    Standard_Integer aMax = NbMapped();
    Standard_Integer anInd = MapIndex (theStart);
    if (anInd == 0) return iter;
    Standard_Integer i1 = (theLevel == 0 ? anInd : 1);
    Standard_Integer i2 = (theLevel == 0 ? anInd : aMax);
    Handle(TColStd_HArray1OfInteger) map = new TColStd_HArray1OfInteger (i1,i2,0);
    for (Standard_Integer anI = i1; anI <= i2; anI ++) {
      anInd = map->Value(anI);
      if (anInd == 0) continue;
      Handle(Transfer_Binder) aBinder = MapItem(anI);
      if (aBinder.IsNull()) continue;
      if (isWithStart) iter.Add (aBinder,Mapped(anInd));
      else iter.Add (aBinder);
    }
    return iter;
  }

  //=======================================================================
  //function : CheckListOne
  //purpose  : 
  //=======================================================================

  Standard_EXPORT Interface_CheckIterator CheckListOne
          (const TheStart& theStart,const Standard_Integer theLevel,
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
      const TheStart& anEnt = Mapped(anInd);
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

  Standard_EXPORT Standard_Boolean IsCheckListEmpty
    (const TheStart& theStart, const Standard_Integer theLevel,
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

  Standard_EXPORT void RemoveResult (const TheStart& theStart,
                                     const Standard_Integer theLevel,
                                     const Standard_Boolean toCompute = Standard_True)
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

  Standard_EXPORT Standard_Integer CheckNum(const TheStart& ) const
  {
    return 0;
  }

  //=======================================================================
  //function : SetProgress
  //purpose  : Sets Progress indicator
  //=======================================================================

  Standard_EXPORT void SetProgress(const Handle(Message_ProgressIndicator)& theProgress)
  {
    myProgress = theProgress;
  }

  //=======================================================================
  //function : GetProgress
  //purpose  : Returns Progress indicator
  //=======================================================================

  Handle(Message_ProgressIndicator) GetProgress() const
  {
    return myProgress;
  }
};
#endif