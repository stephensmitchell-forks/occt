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

  Transfer_FinderProcess::Transfer_FinderProcess (const Standard_Integer theNb)
    : Transfer_TransferProcess (theNb) {  }
 
  void Transfer_FinderProcess::SetModel (const Handle(Interface_InterfaceModel)& theModel)
  {
    myModel = theModel;
  }
 
  Handle(Interface_InterfaceModel)  Transfer_FinderProcess::Model () const
  {
    return myModel;
  }
 
  Standard_Integer  Transfer_FinderProcess::NextMappedWithAttribute
      (const Standard_CString theName,
       const Standard_Integer theNum0) const
  {
    Standard_Integer aNum, aNb = NbMapped();
    for (aNum = theNum0 + 1; aNum <= aNb; aNum ++)
    {
      Handle(Transfer_Finder) aFnd = Mapped (aNum);
      if (aFnd.IsNull()) continue;
      if (!aFnd->Attribute(theName).IsNull())
        return aNum;
    }
    return 0;
  }

  Handle(Transfer_TransientMapper)  Transfer_FinderProcess::TransientMapper
    (const Handle(Standard_Transient)& theObj) const
  {
    Handle(Transfer_TransientMapper) aMapper = new Transfer_TransientMapper(theObj);
    Standard_Integer anIndex = MapIndex (aMapper);
    if (anIndex == 0)
      return aMapper;
    return Handle(Transfer_TransientMapper)::DownCast(Mapped(anIndex));
  }


  void  Transfer_FinderProcess::PrintTrace (const Handle(Transfer_Finder)& theStart,
                                            const Handle(Message_Messenger)& theMessenger) const
  {
    if (!theStart.IsNull())
      theMessenger<<" Type:"<<theStart->ValueTypeName();
  }

  void  Transfer_FinderProcess::PrintStats (const Standard_Integer theMode,
                                          const Handle(Message_Messenger)& theMessenger) const
  {
    theMessenger<<"\n*******************************************************************\n";
    if (theMode == 1) {    //  Statistiques de base
      theMessenger << "********                 Basic Statistics                  ********"<<endl;

      Standard_Integer aNbr = 0, aNbe = 0, aNbw = 0;
      Standard_Integer anI, aMax = NbMapped(), aNbRoots = NbRoots();
      theMessenger << "****        Nb Final Results    : "<<aNbRoots<<endl;

      for (anI = 1; anI <= aMax; anI ++) {
        const Handle(Transfer_Binder)& aBinder = MapItem(anI);
        if (aBinder.IsNull()) continue;
        const Handle(Interface_Check) aCheck = aBinder->Check();
        Transfer_StatusExec aStatus = aBinder->StatusExec();
        if (aStatus != Transfer_StatusInitial && aStatus != Transfer_StatusDone)
          aNbe ++;
        else {
          if (aCheck->NbWarnings() > 0) aNbw  ++;
          if (aBinder->HasResult())  aNbr ++;
        }
      }
      if (aNbr > aNbRoots)
        theMessenger<<"****      ( Itermediate Results : "<<aNbr-aNbRoots<<" )\n";
      if (aNbe > 0)
        theMessenger<<"****                  Errors on :"<<Interface_MSG::Blanks(aNbe,4)<<aNbe<<" Entities\n";
      if (aNbw > 0)
        theMessenger<<"****                Warnings on : "<<Interface_MSG::Blanks(aNbw,4)<<aNbw<<" Entities\n";
      theMessenger<<"*******************************************************************";
    }
    theMessenger<<endl;
  }

//=======================================================================
//function : TransferProduct
//purpose  : 
//=======================================================================

  Handle(Transfer_Binder) Transfer_FinderProcess::TransferProduct (const Handle(Transfer_Finder)& theStart)
  {
    myLevel ++;             // decrement and if == 0, root transfer
    Handle(Transfer_Binder) aBinder;
    Handle(Transfer_ActorOfFinderProcess) anActor = myActor;
    while (!anActor.IsNull())
    {
      if (anActor->Recognize (theStart)) aBinder = anActor->Transferring(theStart,this);
      else aBinder.Nullify();
      if (!aBinder.IsNull()) break;
      anActor = anActor->Next();
    }
    if (aBinder.IsNull()) {
      if (myLevel > 0) myLevel --;
      return aBinder;
    }
    // Managing the root level (.. a close look ..)
    if (myRootLevel == 0 && aBinder->StatusExec() == Transfer_StatusDone)
      myRootLevel = myLevel - 1;

    if (myLevel > 0) myLevel --;
    return aBinder;
  }

    //=======================================================================
  //function : Transferring
  //purpose  : 
  //=======================================================================

  Handle(Transfer_Binder) Transfer_FinderProcess::Transferring (const Handle(Transfer_Finder)& theStart)
  {
    Handle(Transfer_Binder) aFormer = FindAndMask(theStart);

    // Use more: note "AlreadyUsed" so result can not be changed
    if (!aFormer.IsNull()) {
      if (aFormer->HasResult()) {
        aFormer->SetAlreadyUsed();
        return aFormer;
      }

      // Initial state: perhaps already done ... or infeasible
      Transfer_StatusExec statex = aFormer->StatusExec();
      switch (statex) {
        case Transfer_StatusInitial :               // Transfer is prepared to do
          break;
        case Transfer_StatusDone :                  // Transfer was already done
          myMessenger << " .. and Transfer done" << endl;
          return aFormer;
        case Transfer_StatusRun :
          aFormer->SetStatusExec(Transfer_StatusLoop);
          return aFormer;
        case Transfer_StatusError :
          if (myTrace) {
            myMessenger << "                  *** Transfer in Error Status  :" << endl;
            StartTrace (aFormer, theStart, myLevel,0);
          }
          else StartTrace (aFormer, theStart,myLevel,4);
          Transfer_TransferFailure::Raise
            ("TransferProcess : Transfer in Error Status");
        case Transfer_StatusLoop :                  // The loop is closed ...
          if (myTrace) {
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
    Standard_Boolean newbind = Standard_False;
    if (myToHandleErr) {
      // Transfer under protection exceptions (for notification actually)
      Standard_Integer anOldLevel = myLevel;
      try {
        OCC_CATCH_SIGNALS
          aBinder = TransferProduct(theStart);
      }
      catch (Transfer_TransferDeadLoop) {
        if (aBinder.IsNull()) {
          myMessenger << "                  *** Dead Loop with no Result" << endl;
          if (myTrace) StartTrace (aBinder, theStart, myLevel-1,0);
          aBinder = new Transfer_VoidBinder;
          Bind (theStart,aBinder);  newbind = Standard_True;
        } else if (aBinder->StatusExec() == Transfer_StatusLoop) {
          if (myTrace) {
            myMessenger << "                  *** Dead Loop : Finding head of Loop :" << endl;
            StartTrace (aBinder, theStart, myLevel-1,0);
          }
          else StartTrace (aBinder, theStart,myLevel-1,4);
          Transfer_TransferFailure::Raise("TransferProcess : Head of Dead Loop");
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
          Bind (theStart,aBinder);  newbind = Standard_True;
        }
        aBinder->AddFail("Transfer stopped by exception raising");
        if (myTrace) {
          myMessenger << "    *** Raised : " << Standard_Failure::Caught()->GetMessageString() << endl;
          StartTrace (aBinder, theStart, myLevel-1,4);
        }
        myLevel = anOldLevel;
      }
    }

    else  aBinder = TransferProduct(theStart);

    //    Conclusion : Noter dans la Map  

    if (!newbind && !aBinder.IsNull()) {
      if (aFormer.IsNull()) {
        if (!IsBound(theStart)) Bind(theStart,aBinder);     // result = 0 category
        else {                                       // gka TRJ9 for writing SDR for solid
          Rebind(theStart,aBinder); // test_pattern.sat
        }
      }
      else Rebind(theStart,aBinder);
#ifdef TRANSLOG
      cout << " ... OK" << endl;
#endif
    }
    else
    {
      //= by ABV: 5 Oct 97: nothing generated, but aFormer can be in run state - drop it
      //= ASK: may be set it to StatusInitial ?
      if ( ! aFormer.IsNull() ) aFormer->SetStatusExec ( Transfer_StatusDone );
      Handle(Transfer_Binder)     nulbinder;
      return nulbinder;
    }

    //  Manage Roots (if planned)
    if (myRootLevel >= myLevel) {
      myRootLevel = 0;
      if (myToManageRoot && aBinder->Status() != Transfer_StatusVoid) {
        SetRoot (theStart);
      }
    }
    return myLastBinder;
  }

  Standard_Boolean Transfer_FinderProcess::Transfer(const Handle(Transfer_Finder)& theStart)
  {
    Handle(Transfer_Binder) aBinder = Transferring(theStart);
    return (!aBinder.IsNull());
  }
