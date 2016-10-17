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

#include <Transfer_Process.hxx>

#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TColStd_HSequenceOfInteger.hxx>
#include <TColStd_HSequenceOfTransient.hxx>
#include <TColStd_IndexedMapOfInteger.hxx>
#include <Message_Msg.hxx>
#include <Message_Messenger.hxx>
#include <Interface_Check.hxx>
#include <Interface_CheckIterator.hxx>
#include <Interface_InterfaceModel.hxx>
#include <Interface_MSG.hxx>
#include <Transfer_Finder.hxx>
#include <Transfer_VoidBinder.hxx>
#include <Transfer_SimpleBinderOfTransient.hxx>
#include <Transfer_HSequenceOfBinder.hxx>
#include <Transfer_TransferDeadLoop.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Transfer_Process,MMgt_TShared)

//=======================================================================
// Methods of Transfer_Process::Iterator
//=======================================================================

Transfer_Process::Iterator::Iterator (const Standard_Boolean WithSource)
: myCurrent(1),
  myBinders(new Transfer_HSequenceOfBinder),
  mySelected(new TColStd_HSequenceOfInteger)
{
  if (WithSource)
    mySourceObjects = new TColStd_HSequenceOfTransient();
}
  
void Transfer_Process::Iterator::Add (const Handle(Transfer_Binder)& theBinder)
{
  if (!mySourceObjects.IsNull())
    Standard_NoSuchObject::Raise("Transfer_Process::Iterator::Add, Source Object required but not provided");
  AddItem(theBinder);
}
  
void Transfer_Process::Iterator::Add (const Handle(Transfer_Binder)& theBinder, const Handle(Standard_Transient)& theSource)
{
  AddItem(theBinder);
  if (!mySourceObjects.IsNull()) mySourceObjects->Append(theSource);
}
  
void Transfer_Process::Iterator::Filter (const Handle(TColStd_HSequenceOfTransient)& theList)
{
  if (theList.IsNull() || mySourceObjects.IsNull()) return;
  Standard_Integer i, j, nb = mySourceObjects->Length();
  if (nb == 0) return;
  Handle(Transfer_Binder) factice;
  Transfer_MapOfProcess amap (nb);
  for (i = 1; i <= nb; i ++) {
    j = amap.Add (mySourceObjects->Value(i),factice);
    mySelected->SetValue (j,0);
  }

  // Comparaison
  nb = theList->Length();
  for (i = 1; i <= nb; i ++) {
    j = amap.FindIndex (theList->Value(i));
    if (j > 0) mySelected->SetValue (j,1);
  }
}

Standard_Integer Transfer_Process::Iterator::Number () const
{
  Standard_Integer i, n = mySelected->Length(), numb = 0;
  for (i = 1; i <= n; i ++) {
    if (mySelected->Value(i) != 0) numb++;
  }
  return numb;
}

Standard_Boolean Transfer_Process::Iterator::More ()
{
  if (myCurrent > mySelected->Length()) return Standard_False;
  if (mySelected->Value(myCurrent) == 0) Next();
  if (myCurrent > mySelected->Length()) return Standard_False;
  return (mySelected->Value(myCurrent) > 0);
}

void Transfer_Process::Iterator::Next ()
{
  myCurrent++;
  if (myCurrent > mySelected->Length()) return;
  if (mySelected->Value(myCurrent) == 0) Next();
}

const Handle(Transfer_Binder)& Transfer_Process::Iterator::Value () const
{
  if (myCurrent == 0 || myCurrent > mySelected->Length())
    Standard_NoSuchObject::Raise ("Transfer_Process::Iterator::Value");
  if (mySelected->Value(myCurrent) == 0)
    Standard_NoSuchObject::Raise ("Transfer_Process::Iterator::Value");
  return myBinders->Value(myCurrent);
}

const Handle(Standard_Transient) & Transfer_Process::Iterator::SourceObject() const
{
  if (mySourceObjects.IsNull())
    Standard_NoSuchObject::Raise("Transfer_Process::Iterator : No Source Objects defined at all");
  return mySourceObjects->Value(myCurrent);
}

void Transfer_Process::Iterator::AddItem (const Handle(Transfer_Binder)& atr)
{
  myBinders->Append(atr);
  mySelected->Append(1);
}

//=======================================================================
// Methods of Transfer_Process
//=======================================================================

void Transfer_Process::Clean ()
{
  Standard_Integer i, nb = NbMapped();
  Standard_Integer j,unb = 0;
  for (i = 1; i <= nb; i ++) {
    if (myMap(i).IsNull()) unb ++;
  }
  if (unb == 0) return;

  // Refaire la map -> decalages
  TColStd_Array1OfInteger unbs (1,nb);  unbs.Init(0);
  Transfer_MapOfProcess newmap (nb*2);
  for (i = 1; i <= nb; i ++) {
    Handle(Standard_Transient) ent = Mapped(i);
    Handle(Transfer_Binder) bnd = MapItem(i);
    if (bnd.IsNull()) continue;
    j = newmap.Add (ent,bnd);
    unbs.SetValue (i,j);
  }
  myMap.Assign (newmap);

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

Handle(Transfer_Binder) Transfer_Process::Find (const Handle(Standard_Transient) &start) const
{
  if (thelastobj == start && theindex > 0) return thelastbnd;
  const Standard_Integer index = myMap.FindIndex (start);
  return (index > 0)? myMap.FindFromIndex(index) : NULL;
}
  
Standard_Boolean Transfer_Process::IsBound (const Handle(Standard_Transient) &start) const
{
  Handle(Transfer_Binder) binder = Find(start);
  return (binder.IsNull()? Standard_False : binder->HasResult());
}
  
void Transfer_Process::Bind (const Handle(Standard_Transient) &start, const Handle(Transfer_Binder)& binder)
{
  if (binder.IsNull()) return;
  Handle(Transfer_Binder) former = FindAndMask(start);
  if (!former.IsNull()) {
    // On admet VoidBinder : alors on reprend son Check
    if (former->DynamicType() == STANDARD_TYPE(Transfer_VoidBinder)) {
      binder->Merge(former);
      myMap(theindex) = binder;                          // Substitution
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
    if (theindex == 0) theindex = myMap.Add(start,binder);  // Nouveau
    else myMap(theindex) = binder;                          // idem en fait
    thelastbnd = binder;
  }
  else {
    thelastbnd  = binder;
    myMap(theindex) = binder;
  }
}
  
Standard_Boolean Transfer_Process::Unbind (const Handle(Standard_Transient) &start)
{
  Handle(Transfer_Binder) former = FindAndMask(start);
  if (theindex == 0) return Standard_False;
  if (former.IsNull()) return Standard_False;
  if (former->DynamicType() == STANDARD_TYPE(Transfer_VoidBinder)) return Standard_True;
  myMap(theindex) = thelastbnd;
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

void Transfer_Process::SendMsg (const Handle(Standard_Transient) &start, const Message_Msg& amsg)
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

void Transfer_Process::AddFail (const Handle(Standard_Transient) &start, const Standard_CString mess, const Standard_CString orig)
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
  
void Transfer_Process::AddFail (const Handle(Standard_Transient) &start, const Message_Msg& amsg)
{
  if (amsg.IsEdited()) AddFail (start,TCollection_AsciiString(amsg.Value()).ToCString(), TCollection_AsciiString(amsg.Original()).ToCString());
  else AddFail (start,TCollection_AsciiString(amsg.Value()).ToCString());
}
  
void Transfer_Process::AddWarning (const Handle(Standard_Transient) &start, const Standard_CString mess, const Standard_CString orig)
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
  
void Transfer_Process::AddWarning (const Handle(Standard_Transient) &start, const Message_Msg& amsg)
{
  if (amsg.IsEdited()) AddWarning (start,TCollection_AsciiString(amsg.Value()).ToCString(), TCollection_AsciiString(amsg.Original()).ToCString());
  else AddWarning (start,TCollection_AsciiString(amsg.Value()).ToCString());
}
  
Handle(Interface_Check) Transfer_Process::Check (const Handle(Standard_Transient) &start) const
{
  const Handle(Transfer_Binder)& binder = Find(start);
  return (binder.IsNull()? NULL : binder->Check());
}
  
void Transfer_Process::BindTransient (const Handle(Standard_Transient) &start, const Handle(Standard_Transient)& res)
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
  
Handle(Standard_Transient) Transfer_Process::FindTransient (const Handle(Standard_Transient) &start) const
{
  Handle(Transfer_SimpleBinderOfTransient) binder = Handle(Transfer_SimpleBinderOfTransient)::DownCast(Find(start));
  if (binder.IsNull()) return NULL;
  if (!binder->HasResult()) return NULL;
  return binder->Result();
}

Standard_Boolean Transfer_Process::GetTypedTransient (const Handle(Transfer_Binder)& binder, const Handle(Standard_Type)& atype, Handle(Standard_Transient)& val) const
{
  return Transfer_SimpleBinderOfTransient::GetTypedResult(binder,atype,val);
}

Handle(Transfer_Binder) Transfer_Process::Transferring (const Handle(Standard_Transient) &theSource)
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
    thelevel++;             // si decremente et == 0, transfert racine

    if (!myActor.IsNull() && myActor->Recognize(theSource))
      binder = myActor->Transferring(theSource,this);

    if (!binder.IsNull()) {
      // Gestion du niveau racine (.. a regarder de pres ..)
      if (therootl == 0 && binder->StatusExec() == Transfer_StatusDone)
        therootl = thelevel - 1;
    }

    if (thelevel > 0) thelevel--;
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
  
void Transfer_Process::StartTrace (const Handle(Transfer_Binder)& theBinder, const Handle(Standard_Transient) &theSource, const Standard_Integer theLevel, const Standard_Integer theMode) const
{
  // ###  Fail (Roots:50)  --  Start theSource->DynamicType()
  // ###  Fail (Roots:50)  --  Start id:#label.. Type:theSource->DynamicType()
  if (thetrace > 3) {  // Internal to be switch when searching bug (trace >= 4)
    if (theMode == 1) themessenger << "  ###  Fail";
    if (theMode == 2) themessenger << "  ###  Warning";
    if (theMode == 3) themessenger << "  ###  New Root n0 " << theroots.Extent();
    if (theMode == 4) themessenger << "  ###  Exception";
    if (theMode == 5) themessenger << "  ###  Substitution";
    if (theMode == 6) themessenger << "  ###  Information";
    if (theLevel > 1)
      themessenger << " (nested)";  // " at nesting Level:"<<level;
    if (theMode >= 0 && theMode != 3)
      themessenger << " at " << theroots.Extent() << " Roots";
  }

  PrintTrace (theSource,themessenger);

  if (!theBinder.IsNull()) {
    Handle(Transfer_Binder) bnd = theBinder;
    Standard_Boolean hasres = Standard_False;
    while (!bnd.IsNull()) {
      if (bnd->Status() != Transfer_StatusVoid) {
        themessenger << (hasres? " , " : "\n  ---  Result Type : ");
        themessenger << bnd->ResultTypeName();
        hasres = Standard_True;
      }
      bnd = bnd->NextResult();
    }
    if (!hasres && theMode > 2) {
      themessenger << "\n  ---  No Result recorded";
    }
  }
  themessenger << endl;
}
  
void Transfer_Process::PrintTrace (const Handle(Standard_Transient) &theSource, const Handle(Message_Messenger)& S) const
{
  if (!theSource.IsNull()) {
    Handle(Transfer_Finder) aFinder = Handle(Transfer_Finder)::DownCast(theSource);
    S << " Type:" << (aFinder.IsNull()? theSource->DynamicType()->Name() : aFinder->ValueTypeName());
  }
}
  
void Transfer_Process::PrintStats(const Handle(Message_Messenger)& S) const
{
  S<<"\n*******************************************************************\n";
  S << "********                 Basic Statistics                  ********"<<endl;

  if (myModel.IsNull())
    S<<"****        Model unknown"<<endl;
  else
    S<<"****        Nb Entities         : "<<myModel->NbEntities()<<endl;

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

Transfer_Process::Iterator Transfer_Process::RootResult (const Standard_Boolean withstart) const
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
  
Transfer_Process::Iterator Transfer_Process::CompleteResult (const Standard_Boolean withstart) const
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
  
Transfer_Process::Iterator Transfer_Process::AbnormalResult() const
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
  
Interface_CheckIterator Transfer_Process::CheckList (const Standard_Boolean erronly) const
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
    const Handle(Standard_Transient) &ent = Mapped(i);
    num = CheckNum(ent);
    if (num == 0) num = i;
    check->SetEntity(ent);
    list.Add(check,num);
  }
  return list;
}
