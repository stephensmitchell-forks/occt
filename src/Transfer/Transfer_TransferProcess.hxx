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
    //!           - it is possible to re-evaluate the Result if required (i.e.
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
  Handle(Standard_Transient) nultrans;
protected:
  //! to handle or not exception raisings
  Standard_Boolean theerrh; 
  //! trace level (exceptions,errors, etc...)
  Standard_Integer thetrace;
  //! messenger for sending messages
  Handle(Message_Messenger) themessenger;

  Standard_Integer thelevel;
  //! level of root at each time (not allways 1)
  Standard_Integer therootl;
  //! Flag for Root Management set or not
  Standard_Boolean therootm;
  //! indices of roots in the map
  TColStd_IndexedMapOfInteger theroots;

  //! Last Starting Object Bound
  TheStart thelastobj;
  //! Its attached Binder (noted to optimize)
  Handle(Transfer_Binder) thelastbnd;
  //! And Index
  Standard_Integer  theindex;
  ActorHandle theactor;
  TransferMap themap;
  //! Progress indicator
  Handle(Message_ProgressIndicator) myProgress;
  
public:

  //=======================================================================
  //function : Transfer_TransferProcess
  //purpose  : 
  //=======================================================================

  Standard_EXPORT Transfer_TransferProcess (const Standard_Integer nb)
        : themap (nb)
  {
    theerrh  = Standard_True;
    therootm = Standard_False;
    thelevel = 0;     therootl  = 0;
    themessenger = Message::DefaultMessenger();
    thetrace = 0;
  //  theroots = new TColStd_HSequenceOfInteger ();
    theindex = 0;
  }


  //=======================================================================
  //function : Transfer_TransferProcess
  //purpose  : 
  //=======================================================================

  Standard_EXPORT Transfer_TransferProcess (const Handle(Message_Messenger)& messenger,
                                            const Standard_Integer nb)
  : themap (nb)
  {
    theerrh  = Standard_True;
    therootm = Standard_False;
    thelevel = 0;     therootl  = 0;
    SetMessenger (messenger);
    thetrace = 1;
    theindex = 0;
  }

  Standard_EXPORT  void Clear ()
  {
    thelevel = 0;     therootl  = 0;
    theroots.Clear();
    themap.Clear();
    theindex = 0;  thelastobj.Nullify();  thelastbnd.Nullify();
  }

  Standard_EXPORT void Clean ()
  {
    Standard_Integer i, nb = NbMapped();
    Standard_Integer j,unb = 0;
    for (i = 1; i <= nb; i ++)
    {
      if (themap(i).IsNull()) unb ++;
    }
    if (unb == 0) return;

    TColStd_Array1OfInteger unbs (1,nb);  unbs.Init(0);
    Transfer_TransferMap newmap (nb*2);
    for (i = 1; i <= nb; i ++)
    {
      TheStart ent = Mapped(i);
      Handle(Transfer_Binder) bnd = MapItem(i);
      if (bnd.IsNull()) continue;
      j = newmap.Add (ent,bnd);
      unbs.SetValue (i,j);
    }
    themap.Assign (newmap);

    // Update the list of root
    TColStd_IndexedMapOfInteger aNewRoots;
    for (i=1; i<= theroots.Extent(); i++)
    {
      j = theroots.FindKey(i);
      Standard_Integer k = unbs.Value(j);
      if (k) aNewRoots.Add (k);
    }
    theroots.Clear();
    theroots = aNewRoots;

    thelastobj.Nullify();
    thelastbnd.Nullify();
    theindex = 0;
  }

  //=======================================================================
  //function : Resize
  //purpose  : 
  //=======================================================================

  Standard_EXPORT void Resize (const Standard_Integer nb)
  {
    if (nb > themap.NbBuckets()) themap.ReSize(nb);
  }

  //=======================================================================
  //function : SetActor
  //purpose  : 
  //=======================================================================

  Standard_EXPORT void  SetActor(const ActorHandle& actor)
  {
    if (theactor == actor)         return;
    if (theactor.IsNull())         theactor = actor;
    else if (actor.IsNull())       theactor = actor;  // declenche RAZ
    else if (theactor->IsLast()) { actor->SetNext(theactor);  theactor = actor; }
    else                           theactor->SetNext(actor);
  }


  //=======================================================================
  //function : Actor
  //purpose  : 
  //=======================================================================

  Standard_EXPORT ActorHandle Actor () const 
  {
    return theactor;
  }


  //  ########################################################################
  //  ....                             MAPPING                            ....

  //  ##    ##    ##       Actions Generales sur Binders       ##    ##    ##
  //  ##    ##    ##               Consultations               ##    ##    ##

  //=======================================================================
  //function : Find
  //purpose  : 
  //=======================================================================

  Standard_EXPORT Handle(Transfer_Binder) Find (const TheStart& start) const
  {
    if (thelastobj == start) {
      //if (theindex > 0) return thelastbnd->Search(categ); //skl
      if (theindex > 0) return thelastbnd; //skl
    }
    Standard_Integer index = themap.FindIndex (start);
    if (index > 0)
    {
      const Handle(Transfer_Binder)& binder = themap.FindFromIndex(index);
      //if (binder.IsNull()) //skl
      return binder;
      //return binder->Search(categ); //skl
    }
    Handle(Transfer_Binder)  nulbinder;
    return nulbinder;
  }

  //=======================================================================
  //function : IsBound
  //purpose  : 
  //=======================================================================

  Standard_EXPORT Standard_Boolean IsBound(const TheStart& start) const
  {
    Handle(Transfer_Binder) binder = Find(start); //,categ); skl
    if (binder.IsNull()) return Standard_False;
    return binder->HasResult();
  }

  //=======================================================================
  //function : IsAlreadyUsed
  //purpose  : 
  //=======================================================================

  Standard_EXPORT Standard_Boolean IsAlreadyUsed(const TheStart& start) const
  //							 const Standard_Integer categ) const
  {
    Handle(Transfer_Binder) binder = Find(start);
    if (binder.IsNull()) {
      StartTrace (binder,start,thelevel,4);
      Transfer_TransferFailure::Raise
        ("TransferProcess : IsAlreadyUsed, transfer not done cannot be used...");
    }
    return (binder->Status() == Transfer_StatusUsed);
  }

  //=======================================================================
  //function : FindAndMask
  //purpose  : 
  //=======================================================================

  Standard_EXPORT Handle(Transfer_Binder) FindAndMask(const TheStart& start)
  {
    if (thelastobj == start)
    {
      if (theindex > 0) return thelastbnd;
    }
    thelastobj = start;
    theindex   = themap.FindIndex (start);
    if (theindex > 0) thelastbnd = themap.FindFromIndex(theindex);
    else thelastbnd.Nullify();
    return thelastbnd;
  }

  //=======================================================================
  //function : Bind
  //purpose  : 
  //=======================================================================

  Standard_EXPORT void Bind (const TheStart& start,
                             const Handle(Transfer_Binder)& binder)
  {
    if (binder.IsNull()) return;
    Handle(Transfer_Binder) former = FindAndMask(start);
    if (!former.IsNull())
    {
      if (former->DynamicType() == STANDARD_TYPE(Transfer_VoidBinder))
      {
        binder->Merge(former);
        themap(theindex) = binder; // Substitution
      }
      else if (former->Status() == Transfer_StatusUsed)
      {
        StartTrace (former,start,thelevel,4);
        Transfer_TransferFailure::Raise
         ("TransferProcess : Bind, already Bound");
      }
      else
      {
        if (thetrace > 2) StartTrace (former,start,thelevel,5);
        binder->CCheck()->GetMessages (former->Check());
      }
    }
    if (theindex == 0 || thelastbnd.IsNull())
    {
      if (theindex == 0) theindex = themap.Add(start,binder);
      else themap(theindex) = binder;
      thelastbnd = binder;
    }
    else 
    {
      thelastbnd  = binder;
      themap(theindex) = binder;
    }
  }

  //=======================================================================
  //function : Rebind
  //purpose  : 
  //=======================================================================

  Standard_EXPORT void Rebind (const TheStart& start,
                               const Handle(Transfer_Binder)& binder)
  {
    Bind(start,binder);
  }

  //=======================================================================
  //function : Unbind
  //purpose  : 
  //=======================================================================

  Standard_EXPORT Standard_Boolean Unbind (const TheStart& start)
  {
    Handle(Transfer_Binder) former = FindAndMask(start);
    if (theindex == 0) return Standard_False;
    if (former.IsNull()) return Standard_False;
    if (former->DynamicType() == STANDARD_TYPE(Transfer_VoidBinder))
      return Standard_True;
      themap(theindex) = thelastbnd;
    if (theroots.Contains(theindex))
    {
      TColStd_IndexedMapOfInteger aNewRoots;
      for (Standard_Integer i = 1; i <= theroots.Extent(); i++)
        if (theindex!= theroots.FindKey(i))
          aNewRoots.Add(theroots.FindKey(i));
      
      theroots.Clear();
      theroots = aNewRoots;
    }

    thelastobj.Nullify();
    thelastbnd.Nullify();
    theindex = 0;
    return Standard_True;
  }

  //=======================================================================
  //function : FindElseBind
  //purpose  : 
  //=======================================================================

  Standard_EXPORT Handle(Transfer_Binder) FindElseBind(const TheStart& start)
  {
    Handle(Transfer_Binder) binder = FindAndMask (start);
    if (!binder.IsNull()) return binder;
    binder = new Transfer_VoidBinder;
    Bind(start,binder);
    return binder;
  }
  
  //=======================================================================
  //function : SetMessenger
  //purpose  : 
  //=======================================================================

  Standard_EXPORT void SetMessenger (const Handle(Message_Messenger)& messenger)
  {
    if ( messenger.IsNull() )
      themessenger = Message::DefaultMessenger();
    else   
      themessenger = messenger;
  }

  //=======================================================================
  //function : Messenger
  //purpose  : 
  //=======================================================================

  Standard_EXPORT Handle(Message_Messenger) Messenger () const
  {
    return themessenger;
  }

  //=======================================================================
  //function : SetTraceLevel
  //purpose  : 
  //=======================================================================

  Standard_EXPORT void SetTraceLevel (const Standard_Integer tracelev)
  {
    thetrace = tracelev;
  }

  //=======================================================================
  //function : TraceLevel
  //purpose  : 
  //=======================================================================

  Standard_EXPORT Standard_Integer TraceLevel () const
  {
    return thetrace;
  }

  //=======================================================================
  //function : SendFail
  //purpose  : 
  //=======================================================================

  Standard_EXPORT void  SendFail (const TheStart& start,
                                  const Message_Msg& amsg)
  {
    AddFail(start,amsg);
  }


  //=======================================================================
  //function : SendWarning
  //purpose  : 
  //=======================================================================

  Standard_EXPORT void  SendWarning(const TheStart& start,
                                    const Message_Msg& amsg)
  {
    AddWarning(start,amsg);
  }


  //=======================================================================
  //function : SendMsg
  //purpose  : 
  //=======================================================================

  Standard_EXPORT void  SendMsg (const TheStart& start,
                                           const Message_Msg& amsg)
  {
    Handle(Transfer_Binder) binder = FindAndMask(start);
    if (binder.IsNull()) {
      binder = new Transfer_VoidBinder;
      Bind (start,binder);
    }
    // Feeds the trace: Rule causing (user messages)
    if (thetrace > 0) {
      StartTrace (binder,start,thelevel,6);
      themessenger << amsg.Value();
      if (amsg.IsEdited()&&thetrace>2)
        themessenger << " [from: " << amsg.Original() << "]";
      themessenger << endl;
    }
  }

  //=======================================================================
  //function : AddFail
  //purpose  : 
  //=======================================================================

  Standard_EXPORT void AddFail (const TheStart& start,
                                const Standard_CString mess,
                                const Standard_CString orig)
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


  //=======================================================================
  //function : AddError
  //purpose  : 
  //=======================================================================

  Standard_EXPORT void AddError(const TheStart& start,
                                const Standard_CString mess,
                                const Standard_CString orig)
  {
    AddFail (start,mess,orig);
  }

  //=======================================================================
  //function : AddFail
  //purpose  : 
  //=======================================================================

  Standard_EXPORT void AddFail (const TheStart& start,
                                const Message_Msg& amsg)
  {
    if (amsg.IsEdited()) AddFail (start,TCollection_AsciiString(amsg.Value()).ToCString(),
          TCollection_AsciiString(amsg.Original()).ToCString());
    else AddFail (start,TCollection_AsciiString(amsg.Value()).ToCString());
  }

  //=======================================================================
  //function : AddWarning
  //purpose  : 
  //=======================================================================

  Standard_EXPORT void AddWarning (const TheStart& start,
                                   const Standard_CString mess,
                                   const Standard_CString orig)
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

  //=======================================================================
  //function : AddWarning
  //purpose  : 
  //=======================================================================

  Standard_EXPORT void AddWarning (const TheStart& start,
                                   const Message_Msg& amsg)
  {
    if (amsg.IsEdited()) AddWarning (start,TCollection_AsciiString(amsg.Value()).ToCString(),
             TCollection_AsciiString(amsg.Original()).ToCString());
    else AddWarning (start,TCollection_AsciiString(amsg.Value()).ToCString());
  }

  //=======================================================================
  //function : Mend
  //purpose  : 
  //=======================================================================

  Standard_EXPORT void Mend (const TheStart& start,
                             const Standard_CString pref)
  {
    Handle(Transfer_Binder) binder = FindAndMask(start);
    if (binder.IsNull()) return;
    Handle(Interface_Check) ach =  binder->CCheck();
    ach->Mend (pref);
  }

  //=======================================================================
  //function : Check
  //purpose  : 
  //=======================================================================

  Standard_EXPORT Handle(Interface_Check) Check(const TheStart& start) const
  {
    const Handle(Transfer_Binder)& binder = Find(start);
    if (binder.IsNull()) {
      Handle(Interface_Check) check;
      return check;
    }
    return binder->Check();
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

  Standard_EXPORT void BindTransient (const TheStart& start,
                                      const Handle(Standard_Transient)& res)
  {
    if (res.IsNull()) return;
    Handle(Transfer_Binder) former = Find(start);//,categ);skl
    Handle(Transfer_SimpleBinderOfTransient) binder =
      Handle(Transfer_SimpleBinderOfTransient)::DownCast(former);
    if (!binder.IsNull()) {
      if (binder->Status() == Transfer_StatusVoid) { binder->SetResult(res); return; }
    }
    binder = new Transfer_SimpleBinderOfTransient;
    binder->SetResult (res);
    if (former.IsNull()) Bind(start,binder);
    else Rebind(start,binder);
  }

  //=======================================================================
  //function : FindTransient
  //purpose  : 
  //=======================================================================

  Standard_EXPORT const Handle(Standard_Transient)& FindTransient
                                                    (const TheStart& start) const
  {
    Handle(Transfer_SimpleBinderOfTransient) binder =
      Handle(Transfer_SimpleBinderOfTransient)::DownCast(Find(start));
    if (binder.IsNull()) return nultrans;
    if (!binder->HasResult()) return nultrans;
    return binder->Result();
  }

  // Binding Multiple: D by BindMultiple declare the first (if not already done)
  // Then add the words by AddMultiple

  //=======================================================================
  //function : BindMultiple
  //purpose  : 
  //=======================================================================

  Standard_EXPORT void BindMultiple(const TheStart& start)
  {
    Handle(Transfer_Binder) binder = FindAndMask (start);
    if (!binder.IsNull()) {
      if (!binder->IsKind(STANDARD_TYPE(Transfer_MultipleBinder))) {
        StartTrace (thelastbnd,start,thelevel,4);
        Transfer_TransferFailure::Raise ("TransferProcess : BindMultiple");
      }
    }
    else Bind(start,new Transfer_MultipleBinder);
  }

  //=======================================================================
  //function : AddMultiple
  //purpose  : 
  //=======================================================================

  Standard_EXPORT void AddMultiple(const TheStart& start,
                                   const Handle(Standard_Transient)& res)
  {
    Handle(Transfer_Binder) binder = FindAndMask(start);
    Handle(Transfer_MultipleBinder) multr =
      Handle(Transfer_MultipleBinder)::DownCast(binder);
    if (multr.IsNull()) {
      StartTrace (binder,start,thelevel,4);
      if (binder.IsNull()) Transfer_TransferFailure::Raise
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
           (const TheStart& start,
            const Handle(Standard_Type)& atype,
            Handle(Standard_Transient)& val) const
  {
    return GetTypedTransient (Find(start),atype,val);
  }


  //=======================================================================
  //function : GetTypedTransient
  //purpose  : 
  //=======================================================================

  Standard_EXPORT Standard_Boolean GetTypedTransient
         (const Handle(Transfer_Binder)& binder,
          const Handle(Standard_Type)& atype,
          Handle(Standard_Transient)& val) const
  {
    return Transfer_SimpleBinderOfTransient::GetTypedResult(binder,atype,val);
  }


  //  ##    ##    ##    ##    ##    Acces Atomique    ##    ##    ##    ##    ##
  //    (ne gere pas le scope mais donne acces aux categories)

  //=======================================================================
  //function : NbMapped
  //purpose  : 
  //=======================================================================

  Standard_EXPORT Standard_Integer NbMapped () const
  {
    return themap.Extent();
  }

  //=======================================================================
  //function : Mapped
  //purpose  : 
  //=======================================================================

  Standard_EXPORT const TheStart& Mapped(const Standard_Integer num) const
  {
    return themap.FindKey(num);
  }

  //=======================================================================
  //function : MapIndex
  //purpose  : 
  //=======================================================================

  Standard_EXPORT Standard_Integer MapIndex(const TheStart& start) const
  {
    return themap.FindIndex(start);
  }

  //=======================================================================
  //function : MapItem
  //purpose  : 
  //=======================================================================

  Standard_EXPORT Handle(Transfer_Binder) MapItem(const Standard_Integer num) const
  {
    Handle(Transfer_Binder) binder = themap.FindFromIndex(num);
    return binder;
  }


  //  ########################################################################
  //  ....                         ROOT MANAGEMENT                        ....

  //=======================================================================
  //function : SetRoot
  //purpose  : 
  //=======================================================================

  Standard_EXPORT void SetRoot (const TheStart& start)
  {
    Standard_Integer index = MapIndex(start);
    if (index == 0) {
      return;
    }

    theroots.Add(index);
    if (thetrace > 2) StartTrace (MapItem(index),start,thelevel,3);
  }

  //=======================================================================
  //function : SetRootManagement
  //purpose  : 
  //=======================================================================

  Standard_EXPORT void SetRootManagement(const Standard_Boolean stat)
  {
    therootm = stat;
  }

  //=======================================================================
  //function : NbRoots
  //purpose  : 
  //=======================================================================

  Standard_EXPORT Standard_Integer NbRoots () const
  {
    return theroots.Extent();
  }

  //=======================================================================
  //function : Root
  //purpose  : 
  //=======================================================================

  Standard_EXPORT const TheStart& Root(const Standard_Integer num) const
  {
    Standard_Integer ind = 0;
    if (num > 0 && num <= theroots.Extent()) ind = theroots.FindKey(num);
    return themap.FindKey (ind);
  }
  
  //=======================================================================
  //function : RootItem
  //purpose  : 
  //=======================================================================

  Standard_EXPORT Handle(Transfer_Binder) RootItem(const Standard_Integer num) const
  {
    Standard_Integer ind = 0;
    if (num > 0 && num <= theroots.Extent()) ind = theroots.FindKey(num);
    return themap.FindFromIndex(ind);//->Search(categ);skl
  }
  
  //=======================================================================
  //function : RootIndex
  //purpose  : 
  //=======================================================================

  Standard_EXPORT Standard_Integer RootIndex(const TheStart& start) const
  {
    Standard_Integer index = MapIndex(start);
    if (index == 0) return 0;
    return theroots.FindIndex(index);
  }

  //=======================================================================
  //function : NestingLevel
  //purpose  : 
  //=======================================================================

  Standard_EXPORT Standard_Integer NestingLevel () const
  {
    return thelevel;
  }

  //=======================================================================
  //function : ResetNestingLevel
  //purpose  : 
  //=======================================================================

  Standard_EXPORT void ResetNestingLevel ()
  {
    thelevel = 0;
  }

  //  ########################################################################
  //  ....                        SCOPE MANAGEMENT                        ....


  //======================================================================
  //Purpose : gka TRJ9 for writing SDR for solid
  //          Check if binder has already been bound to the result binder.
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

  Standard_EXPORT Standard_Boolean Recognize(const TheStart& start) const
  {
    ActorHandle actor = theactor;
    // It scans up to have a next result
    while (!actor.IsNull()) {
      if (actor->Recognize (start)) return Standard_True;
      actor = actor->Next();
    }
    return Standard_False;
  }

//  //=======================================================================
//  //function : Transferring
//  //purpose  : 
//  //=======================================================================
//
//  Standard_EXPORT Handle(Transfer_Binder) Transferring (const TheStart& start)
//  {
//    Handle(Transfer_Binder) former = FindAndMask(start);
//
//    // Use more: note "AlreadyUsed" so result can not be changed
//    if (!former.IsNull()) {
//      if (former->HasResult()) {
//        former->SetAlreadyUsed();
//        return former;
//      }
//
//      // Initial state: perhaps already done ... or infeasible
//      Transfer_StatusExec statex = former->StatusExec();
//      switch (statex) {
//        case Transfer_StatusInitial :               // Transfer is prepared to do
//          break;
//        case Transfer_StatusDone :                  // Transfer was already done
//          themessenger << " .. and Transfer done" << endl;
//          return former;
//        case Transfer_StatusRun :
//          former->SetStatusExec(Transfer_StatusLoop);
//          return former;
//        case Transfer_StatusError :
//          if (thetrace) {
//            themessenger << "                  *** Transfer in Error Status  :" << endl;
//            StartTrace (former, start, thelevel,0);
//          }
//          else StartTrace (former, start,thelevel,4);
//          Transfer_TransferFailure::Raise
//            ("TransferProcess : Transfer in Error Status");
//        case Transfer_StatusLoop :                  // The loop is closed ...
//          if (thetrace) {
//            themessenger << "                  *** Transfer  Head of Dead Loop  :" << endl;
//            StartTrace (former, start, thelevel,0);
//          }
//          else StartTrace (former, start,thelevel,4);
//          Transfer_TransferDeadLoop::Raise
//            ("TransferProcess : Transfer at Head of a Dead Loop");
//      }
//#ifdef TRANSLOG
//      cout << "Transfer,level "<<thelevel<<Message_Flush;
//#endif
//      former->SetStatusExec(Transfer_StatusRun);
//    }
//#ifdef TRANSLOG
//    cout << " GO .." << endl;
//#endif
//
//    Handle(Transfer_Binder) binder;
//    Standard_Boolean newbind = Standard_False;
//    if (theerrh) {
//      // Transfer under protection exceptions (for notification actually)
//      Standard_Integer oldlev = thelevel;
//      try {
//        OCC_CATCH_SIGNALS
//          binder = TransferProduct(start);
//      }
//      catch (Transfer_TransferDeadLoop) {
//        if (binder.IsNull()) {
//          themessenger << "                  *** Dead Loop with no Result" << endl;
//          if (thetrace) StartTrace (binder, start, thelevel-1,0);
//          binder = new Transfer_VoidBinder;
//          Bind (start,binder);  newbind = Standard_True;
//        } else if (binder->StatusExec() == Transfer_StatusLoop) {
//          if (thetrace) {
//            themessenger << "                  *** Dead Loop : Finding head of Loop :" << endl;
//            StartTrace (binder, start, thelevel-1,0);
//          }
//          else StartTrace (binder, start,thelevel-1,4);
//          Transfer_TransferFailure::Raise("TransferProcess : Head of Dead Loop");
//        } else {
//          if (thetrace) {
//            themessenger << "                  *** Dead Loop : Actor in Loop :" << endl;
//            StartTrace (binder, start, thelevel-1,0);
//          }
//        }
//        binder->AddFail("Transfer in dead Loop");
//        thelevel = oldlev;
//      }
//      catch (Standard_Failure) {
//        if (binder.IsNull()) {
//          themessenger << "                  *** Exception Raised with no Result" << endl;
//          binder = new Transfer_VoidBinder;
//          Bind (start,binder);  newbind = Standard_True;
//        }
//        binder->AddFail("Transfer stopped by exception raising");
//        if (thetrace) {
//          themessenger << "    *** Raised : " << Standard_Failure::Caught()->GetMessageString() << endl;
//          StartTrace (binder, start, thelevel-1,4);
//        }
//        thelevel = oldlev;
//      }
//    }
//
//    else  binder = TransferProduct(start);
//
//    //    Conclusion : Noter dans la Map  
//
//    if (!newbind && !binder.IsNull()) {
//      if (former.IsNull()) {
//        if (!IsBound(start)) Bind(start,binder);     // result = 0 category
//        else {                                       // gka TRJ9 for writing SDR for solid
//          Rebind(start,binder); // test_pattern.sat
//        }
//      }
//      else Rebind(start,binder);
//#ifdef TRANSLOG
//      cout << " ... OK" << endl;
//#endif
//    }
//    else
//    {
//      //= by ABV: 5 Oct 97: nothing generated, but former can be in run state - drop it
//      //= ASK: may be set it to StatusInitial ?
//      if ( ! former.IsNull() ) former->SetStatusExec ( Transfer_StatusDone );
//      Handle(Transfer_Binder)     nulbinder;
//      return nulbinder;
//    }
//
//    //  Manage Roots (if planned)
//    if (therootl >= thelevel) {
//      therootl = 0;
//      if (therootm && binder->Status() != Transfer_StatusVoid) {
//        SetRoot (start);
//      }
//    }
//    return thelastbnd;
//  }

  //Standard_EXPORT Handle(Transfer_Binder) TransferProduct (const TheStart& start)
  //{
  //  thelevel ++;             // decrement and if == 0, root transfer
  //  Handle(Transfer_Binder) binder;
  //  ActorHandle actor = theactor;
  //  while (!actor.IsNull())
  //  {
  //    if (actor->Recognize (start)) binder = actor->Transferring(start,this);
  //    else binder.Nullify();
  //    if (!binder.IsNull()) break;
  //    actor = actor->Next();
  //  }
  //  if (binder.IsNull()) {
  //    if (thelevel > 0) thelevel --;
  //    return binder;
  //  }
  //  // Managing the root level (.. a close look ..)
  //  if (therootl == 0 && binder->StatusExec() == Transfer_StatusDone)
  //    therootl = thelevel - 1;

  //  if (thelevel > 0) thelevel --;
  //  return binder;
  //}

  //=======================================================================
  //function : Transfer
  //purpose  : 
  //=======================================================================

  //Standard_EXPORT Standard_Boolean Transfer(const TheStart& start)
  //{
  //  Handle(Transfer_Binder) binder = Transferring(start);
  //  return (!binder.IsNull());
  //}

  //  #########################################################################
  //  ....                      Error Handling + Trace                     ....
  
  //=======================================================================
  //function : SetErrorHandle
  //purpose  : 
  //=======================================================================

  Standard_EXPORT void SetErrorHandle(const Standard_Boolean err)
  {
    theerrh = err;
  }

  //=======================================================================
  //function : ErrorHandle
  //purpose  : 
  //=======================================================================

  Standard_EXPORT Standard_Boolean ErrorHandle() const
  {
    return theerrh;
  }

  //=======================================================================
  //function : StartTrace
  //purpose  : 
  //=======================================================================

  Standard_EXPORT void StartTrace(const Handle(Transfer_Binder)& binder,
                                  const TheStart& start,
                                  const Standard_Integer level,
                                  const Standard_Integer mode) const 
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

    if (!binder.IsNull()) {   // old: if IsNull sout <<endl<< "  ---  Not Bound";
      Handle(Transfer_Binder) bnd = binder;
      Standard_Boolean hasres = Standard_False, isused = Standard_False;
      while (!bnd.IsNull()) {
        if (bnd->Status() != Transfer_StatusVoid) {
  // ---  Result Type: binder->ResultType()  ---  Binder : binder->DynamicType();
    if (!hasres)
      themessenger << "\n  ---  Result Type : ";
    else 
      themessenger << " , ";
    themessenger << bnd->ResultTypeName();
  //  CKY 9-JAN-1999:  waiting for XSTEP Kernel message (not IGES_2075)
  /*        Message_Msg Msg2075("IGES_2075");
            Msg2075.AddString(bnd->ResultTypeName());
            Msg2075.TraceEver();  */
    hasres = Standard_True;
    if (bnd->Status() == Transfer_StatusUsed) isused = Standard_True;
        }
        bnd = bnd->NextResult();
      }
      if (!hasres && mode > 2) {
        themessenger << "\n  ---  No Result recorded";
  //  CKY 9-JAN-1999 : waiting for XSTEP Kernel message
  //     (not IGES_2075, no reference to specifically TopoDS_Shape)
  /*       Message_Msg Msg2075("IGES_2075");
           Msg2075.AddString("No TopoDS_Shape");
           Msg2075.TraceEver();  */
      }
  //old    if (isused) sout << "  --    (Already Used in another Transfer)";
    }
    themessenger << endl;
  }


  //=======================================================================
  //function : PrintTrace
  //purpose  : 
  //=======================================================================

  Standard_EXPORT void PrintTrace(const TheStart& start,
                                  const Handle(Message_Messenger)& S) const
  {
    if (!start.IsNull())   S <<" Type:" << start->DynamicType()->Name();
  }

  //=======================================================================
  //function : IsLooping
  //purpose  : 
  //=======================================================================

  Standard_EXPORT Standard_Boolean  IsLooping (const Standard_Integer alevel) const 
  {  return alevel > NbMapped();  }

  //  #########################################################################
  //  ....                            RESULTS                            ....

  //=======================================================================
  //function : RootResult
  //purpose  : 
  //=======================================================================

  Standard_EXPORT Iterator RootResult(const Standard_Boolean withstart = Standard_False) const
  {
    Iterator iter(withstart);
    Standard_Integer max = theroots.Extent();
    for (Standard_Integer j = 1; j <= max; j ++) {
      Standard_Integer i = theroots.FindKey(j);
      Handle(Transfer_Binder) binder = MapItem(i);
      if (binder.IsNull()) continue;
      if (withstart) iter.Add (binder,Mapped(i));
      else iter.Add (binder);
    }
    return iter;
  }

  //=======================================================================
  //function : CompleteResult
  //purpose  : All Results
  //=======================================================================

  Standard_EXPORT Iterator CompleteResult (const Standard_Boolean withstart = Standard_False) const
  {
    Iterator iter(withstart);
    Standard_Integer max = NbMapped();
    for (Standard_Integer i = 1; i <= max; i ++) {
      Handle(Transfer_Binder) binder = MapItem(i);
      if (binder.IsNull()) continue;
      if (withstart) iter.Add (binder,Mapped(i));
      else iter.Add (binder);
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
    Standard_Integer max = NbMapped();
    for (Standard_Integer i = 1; i <= max; i ++) {
      Handle(Transfer_Binder) binder = MapItem(i);
      if (binder.IsNull()) continue;
      Transfer_StatusExec statex = binder->StatusExec();
      if (statex != Transfer_StatusInitial && statex != Transfer_StatusDone)
        iter.Add (binder,Mapped(i));  // on note les cas "pas normaux"
    }
    return iter;
  }

  //=======================================================================
  //function : CheckList
  //purpose  : 
  //=======================================================================

  Standard_EXPORT Interface_CheckIterator CheckList (const Standard_Boolean erronly) const
  {
    Interface_CheckIterator list;
    Standard_Integer num, max = NbMapped();
    for (Standard_Integer i = 1; i <= max; i ++) {
      Handle(Transfer_Binder) binder = MapItem(i);
      if (binder.IsNull()) continue;
      Transfer_StatusExec statex = binder->StatusExec();
      Handle(Interface_Check) check = binder->Check();
      if (statex != Transfer_StatusInitial && statex != Transfer_StatusDone &&
    !check->HasFailed())
        check->AddFail("Transfer in Abnormal Status (!= Initial or Done)");
      if (!check->HasFailed() && (erronly || check->NbWarnings() == 0)) continue;
      const TheStart& ent = Mapped(i);
      num = CheckNum(ent);
      if (num == 0) num = i;
      check->SetEntity(ent);
      list.Add(check,num);
    }
    return list;
  }

  //  #########################################################################
  //  ....                       RESULTS BY ENTITY                      ....

  //=======================================================================
  //function : ResultOne
  //purpose  : 
  //=======================================================================

  Standard_EXPORT Iterator ResultOne (const TheStart& start,
                                               const Standard_Integer level,
                                               const Standard_Boolean withstart = Standard_False) const
  {
    Iterator iter(withstart);
    Standard_Integer max = NbMapped();
    Standard_Integer ind = MapIndex (start);
    if (ind == 0) return iter;
    Standard_Integer i1 = (level == 0 ? ind : 1);
    Standard_Integer i2 = (level == 0 ? ind : max);
    Handle(TColStd_HArray1OfInteger) map = new TColStd_HArray1OfInteger (i1,i2,0);
    for (Standard_Integer i = i1; i <= i2; i ++) {
      ind = map->Value(i);
      if (ind == 0) continue;
      Handle(Transfer_Binder) binder = MapItem(i);
      if (binder.IsNull()) continue;
      if (withstart) iter.Add (binder,Mapped(ind));
      else iter.Add (binder);
    }
    return iter;
  }

  //=======================================================================
  //function : CheckListOne
  //purpose  : 
  //=======================================================================

  Standard_EXPORT Interface_CheckIterator CheckListOne
          (const TheStart& start,const Standard_Integer level,
           const Standard_Boolean erronly) const
  {
    Interface_CheckIterator list;
    Standard_Integer max = NbMapped();
    Standard_Integer num, ind = MapIndex (start);
    if (ind == 0) return list;
    Standard_Integer i1 = (level == 0 ? ind : 1);
    Standard_Integer i2 = (level == 0 ? ind : max);
    Handle(TColStd_HArray1OfInteger) map = new TColStd_HArray1OfInteger (i1,i2,0);

    for (Standard_Integer i = i1; i <= i2; i ++) {
      ind = map->Value(i);
      if (ind == 0) continue;
      Handle(Transfer_Binder) binder = MapItem(ind);
      if (binder.IsNull()) continue;
      Transfer_StatusExec statex = binder->StatusExec();
      Handle(Interface_Check) check = binder->Check();
      if (statex != Transfer_StatusInitial && statex != Transfer_StatusDone &&
    !check->HasFailed())
        check->AddFail("Transfer in Abnormal Status (!= Initial or Done)");
      if (!check->HasFailed() && (erronly || check->NbWarnings() == 0)) continue;
      const TheStart& ent = Mapped(ind);
      num = CheckNum(ent);  if (num == 0) num = ind;
      check->SetEntity(ent);
      list.Add(check,num);
    }
    return list;
  }

  //=======================================================================
  //function : IsCheckListEmpty
  //purpose  : 
  //=======================================================================

  Standard_EXPORT Standard_Boolean IsCheckListEmpty
    (const TheStart& start, const Standard_Integer level,
     const Standard_Boolean erronly) const
  {
    Standard_Integer max = NbMapped();
    Standard_Integer ind = MapIndex (start);
    if (ind == 0) return Standard_False;
    Standard_Integer i1 = (level == 0 ? ind : 1);
    Standard_Integer i2 = (level == 0 ? ind : max);
    Handle(TColStd_HArray1OfInteger) map = new TColStd_HArray1OfInteger (i1,i2,0);

    for (Standard_Integer i = i1; i <= i2; i ++) {
      ind = map->Value(i);
      if (ind == 0) continue;
      Handle(Transfer_Binder) binder = MapItem(ind);
      if (binder.IsNull()) continue;

      Transfer_StatusExec statex = binder->StatusExec();
      Handle(Interface_Check) check = binder->Check();
      if (statex != Transfer_StatusInitial && statex != Transfer_StatusDone)
        return Standard_False;
      if (check->HasFailed() || (!erronly && check->NbWarnings() > 0)) return Standard_False;
    }
    return Standard_True;
  }

  //=======================================================================
  //function : RemoveResult
  //purpose  : 
  //=======================================================================

  Standard_EXPORT void RemoveResult(const TheStart& start,
                 const Standard_Integer level,
                 const Standard_Boolean compute = Standard_True)
  {
    Standard_Integer max = NbMapped();
    Standard_Integer ind = MapIndex (start);
    if (ind == 0) return;
    Standard_Integer i1 = (level == 0 ? ind : 1);
    Standard_Integer i2 = (level == 0 ? ind : max);
    Handle(TColStd_HArray1OfInteger) map = new TColStd_HArray1OfInteger (i1,i2,0);

    Standard_Integer i; // svv Jan11 2000 : porting on DEC
    for (i = i1; i <= i2; i ++) {
      ind = map->Value(i);
      if (ind == 0) continue;
      Handle(Transfer_Binder) binder = MapItem(ind);
      if (binder.IsNull()) continue;
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