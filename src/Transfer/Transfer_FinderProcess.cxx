// Copyright (c) 1999-2012 OPEN CASCADE SAS
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

#include <Transfer_FinderProcess.hxx>
#include <Interface_MSG.hxx>
#include <Transfer_Binder.hxx>
#include <Interface_Check.hxx>
#include <Interface_InterfaceModel.hxx>
#include <Transfer_TransientMapper.hxx>
#include <Standard_Transient.hxx>
#include <Transfer_Finder.hxx>
#include <Message_Messenger.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_TYPE(Transfer_FinderProcess)
IMPLEMENT_STANDARD_SUPERTYPE_ARRAY()
  STANDARD_TYPE(MMgt_TShared),
  STANDARD_TYPE(Standard_Transient),

IMPLEMENT_STANDARD_SUPERTYPE_ARRAY_END()
IMPLEMENT_STANDARD_TYPE_END(Transfer_FinderProcess)


IMPLEMENT_DOWNCAST(Transfer_FinderProcess,Standard_Transient)
IMPLEMENT_STANDARD_RTTI(Transfer_FinderProcess)

Transfer_FinderProcess::Transfer_FinderProcess (const Standard_Integer nb)
    : Transfer_TransferProcess (nb)    {  }
 
    void Transfer_FinderProcess::SetModel
  (const Handle(Interface_InterfaceModel)& model)
      {  themodel = model;  }
 
    Handle(Interface_InterfaceModel)  Transfer_FinderProcess::Model () const
      {  return themodel;  }
 

    Standard_Integer  Transfer_FinderProcess::NextMappedWithAttribute
  (const Standard_CString name, const Standard_Integer num0) const
{
  Standard_Integer num, nb = NbMapped();
  for (num = num0+1; num <= nb; num ++) {
    Handle(Transfer_Finder) fnd = Mapped (num);
    if (fnd.IsNull()) continue;
    if (!fnd->Attribute(name).IsNull()) return num;
  }
  return 0;
}

    Handle(Transfer_TransientMapper)  Transfer_FinderProcess::TransientMapper
  (const Handle(Standard_Transient)& obj) const
{
  Handle(Transfer_TransientMapper) mapper = new Transfer_TransientMapper(obj);
  Standard_Integer index = MapIndex (mapper);
  if (index == 0) return mapper;
  return Handle(Transfer_TransientMapper)::DownCast(Mapped(index));
}


  void  Transfer_FinderProcess::PrintTrace (const Handle(Transfer_Finder)& start,
                                            const Handle(Message_Messenger)& S) const
  {
    if (!start.IsNull()) S<<" Type:"<<start->ValueTypeName();
  }

  void  Transfer_FinderProcess::PrintStats (const Standard_Integer mode,
                                          const Handle(Message_Messenger)& S) const
  {
    S<<"\n*******************************************************************\n";
    if (mode == 1) {    //  Statistiques de base
      S << "********                 Basic Statistics                  ********"<<endl;

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
          if (ach->NbWarnings() > 0) nbw  ++;
          if (binder->HasResult())  nbr ++;
        }
      }
      if (nbr > nbroots)
        S<<"****      ( Itermediate Results : "<<nbr-nbroots<<" )\n";
      if (nbe > 0)
        S<<"****                  Errors on :"<<Interface_MSG::Blanks(nbe,4)<<nbe<<" Entities\n";
      if (nbw > 0)
        S<<"****                Warnings on : "<<Interface_MSG::Blanks(nbw,4)<<nbw<<" Entities\n";
      S<<"*******************************************************************";
    }
    S<<endl;
  }

//=======================================================================
//function : TransferProduct
//purpose  : 
//=======================================================================

  Handle(Transfer_Binder) Transfer_FinderProcess::TransferProduct (const Handle(Transfer_Finder)& start)
  {
    thelevel ++;             // decrement and if == 0, root transfer
    Handle(Transfer_Binder) binder;
    Handle(Transfer_ActorOfFinderProcess) actor = theactor;
    while (!actor.IsNull())
    {
      if (actor->Recognize (start)) binder = actor->Transferring(start,this);
      else binder.Nullify();
      if (!binder.IsNull()) break;
      actor = actor->Next();
    }
    if (binder.IsNull()) {
      if (thelevel > 0) thelevel --;
      return binder;
    }
    // Managing the root level (.. a close look ..)
    if (therootl == 0 && binder->StatusExec() == Transfer_StatusDone)
      therootl = thelevel - 1;

    if (thelevel > 0) thelevel --;
    return binder;
  }

    //=======================================================================
  //function : Transferring
  //purpose  : 
  //=======================================================================

  Handle(Transfer_Binder) Transfer_FinderProcess::Transferring (const Handle(Transfer_Finder)& start)
  {
    Handle(Transfer_Binder) former = FindAndMask(start);

    // Use more: note "AlreadyUsed" so result can not be changed
    if (!former.IsNull()) {
      if (former->HasResult()) {
        former->SetAlreadyUsed();
        return former;
      }

      // Initial state: perhaps already done ... or infeasible
      Transfer_StatusExec statex = former->StatusExec();
      switch (statex) {
        case Transfer_StatusInitial :               // Transfer is prepared to do
          break;
        case Transfer_StatusDone :                  // Transfer was already done
          themessenger << " .. and Transfer done" << endl;
          return former;
        case Transfer_StatusRun :
          former->SetStatusExec(Transfer_StatusLoop);
          return former;
        case Transfer_StatusError :
          if (thetrace) {
            themessenger << "                  *** Transfer in Error Status  :" << endl;
            StartTrace (former, start, thelevel,0);
          }
          else StartTrace (former, start,thelevel,4);
          Transfer_TransferFailure::Raise
            ("TransferProcess : Transfer in Error Status");
        case Transfer_StatusLoop :                  // The loop is closed ...
          if (thetrace) {
            themessenger << "                  *** Transfer  Head of Dead Loop  :" << endl;
            StartTrace (former, start, thelevel,0);
          }
          else StartTrace (former, start,thelevel,4);
          Transfer_TransferDeadLoop::Raise
            ("TransferProcess : Transfer at Head of a Dead Loop");
      }
#ifdef TRANSLOG
      cout << "Transfer,level "<<thelevel<<Message_Flush;
#endif
      former->SetStatusExec(Transfer_StatusRun);
    }
#ifdef TRANSLOG
    cout << " GO .." << endl;
#endif

    Handle(Transfer_Binder) binder;
    Standard_Boolean newbind = Standard_False;
    if (theerrh) {
      // Transfer under protection exceptions (for notification actually)
      Standard_Integer oldlev = thelevel;
      try {
        OCC_CATCH_SIGNALS
          binder = TransferProduct(start);
      }
      catch (Transfer_TransferDeadLoop) {
        if (binder.IsNull()) {
          themessenger << "                  *** Dead Loop with no Result" << endl;
          if (thetrace) StartTrace (binder, start, thelevel-1,0);
          binder = new Transfer_VoidBinder;
          Bind (start,binder);  newbind = Standard_True;
        } else if (binder->StatusExec() == Transfer_StatusLoop) {
          if (thetrace) {
            themessenger << "                  *** Dead Loop : Finding head of Loop :" << endl;
            StartTrace (binder, start, thelevel-1,0);
          }
          else StartTrace (binder, start,thelevel-1,4);
          Transfer_TransferFailure::Raise("TransferProcess : Head of Dead Loop");
        } else {
          if (thetrace) {
            themessenger << "                  *** Dead Loop : Actor in Loop :" << endl;
            StartTrace (binder, start, thelevel-1,0);
          }
        }
        binder->AddFail("Transfer in dead Loop");
        thelevel = oldlev;
      }
      catch (Standard_Failure) {
        if (binder.IsNull()) {
          themessenger << "                  *** Exception Raised with no Result" << endl;
          binder = new Transfer_VoidBinder;
          Bind (start,binder);  newbind = Standard_True;
        }
        binder->AddFail("Transfer stopped by exception raising");
        if (thetrace) {
          themessenger << "    *** Raised : " << Standard_Failure::Caught()->GetMessageString() << endl;
          StartTrace (binder, start, thelevel-1,4);
        }
        thelevel = oldlev;
      }
    }

    else  binder = TransferProduct(start);

    //    Conclusion : Noter dans la Map  

    if (!newbind && !binder.IsNull()) {
      if (former.IsNull()) {
        if (!IsBound(start)) Bind(start,binder);     // result = 0 category
        else {                                       // gka TRJ9 for writing SDR for solid
          Rebind(start,binder); // test_pattern.sat
        }
      }
      else Rebind(start,binder);
#ifdef TRANSLOG
      cout << " ... OK" << endl;
#endif
    }
    else
    {
      //= by ABV: 5 Oct 97: nothing generated, but former can be in run state - drop it
      //= ASK: may be set it to StatusInitial ?
      if ( ! former.IsNull() ) former->SetStatusExec ( Transfer_StatusDone );
      Handle(Transfer_Binder)     nulbinder;
      return nulbinder;
    }

    //  Manage Roots (if planned)
    if (therootl >= thelevel) {
      therootl = 0;
      if (therootm && binder->Status() != Transfer_StatusVoid) {
        SetRoot (start);
      }
    }
    return thelastbnd;
  }

  Standard_Boolean Transfer_FinderProcess::Transfer(const Handle(Transfer_Finder)& start)
  {
    Handle(Transfer_Binder) binder = Transferring(start);
    return (!binder.IsNull());
  }
