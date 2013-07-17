// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and / or modify it
// under the terms of the GNU Lesser General Public version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#include <Transfer_TransientProcess.hxx>
#include <Interface_Check.hxx>
#include <Interface_MSG.hxx>
#include <Transfer_Binder.hxx>
#include <TColStd_HSequenceOfTransient.hxx>
#include <Standard_Type.hxx>
#include <Interface_InterfaceModel.hxx>
#include <Interface_HGraph.hxx>
#include <Dico_DictionaryOfTransient.hxx>
#include <TColStd_HSequenceOfTransient.hxx>
#include <Interface_Graph.hxx>
#include <Standard_Transient.hxx>
#include <Message_Messenger.hxx>
#include <Interface_EntityIterator.hxx>

IMPLEMENT_STANDARD_TYPE(Transfer_TransientProcess)
IMPLEMENT_STANDARD_SUPERTYPE_ARRAY()
  STANDARD_TYPE(MMgt_TShared),
  STANDARD_TYPE(Standard_Transient),

IMPLEMENT_STANDARD_SUPERTYPE_ARRAY_END()
IMPLEMENT_STANDARD_TYPE_END(Transfer_TransientProcess)


IMPLEMENT_DOWNCAST(Transfer_TransientProcess,Standard_Transient)
IMPLEMENT_STANDARD_RTTI(Transfer_TransientProcess)


//=======================================================================
//function : Transfer_TransientProcess
//purpose  : 
//=======================================================================

Transfer_TransientProcess::Transfer_TransientProcess
  (const Standard_Integer theNb)
  : Transfer_TransferProcess  < Handle(Transfer_ActorOfTransientProcess),
                                Transfer_TransferMapOfProcessForTransient,
                                Handle(Standard_Transient),
                                TColStd_MapTransientHasher,
                                Handle(TColStd_HSequenceOfTransient),
                                Transfer_IteratorOfProcessForTransient > (theNb)  
{  
  theTrRoots = new TColStd_HSequenceOfTransient;
}

//=======================================================================
//function : SetModel
//purpose  : 
//=======================================================================

void Transfer_TransientProcess::SetModel
  (const Handle(Interface_InterfaceModel)& theModel)
{
  myModel = theModel;
}


//=======================================================================
//function : Model
//purpose  : 
//=======================================================================

Handle(Interface_InterfaceModel) Transfer_TransientProcess::Model () const
{
  return myModel;
}


void Transfer_TransientProcess::SetGraph(const Handle(Interface_HGraph)& theHGraph)
{
  myGraph = theHGraph;
  if (!myGraph.IsNull())
    SetModel(myGraph->Graph().Model());
  else
    myModel.Nullify();
}


//=======================================================================
//function : HasGraph
//purpose  : 
//=======================================================================

Standard_Boolean Transfer_TransientProcess::HasGraph () const
{
  return !myGraph.IsNull();
}


//=======================================================================
//function : HGraph
//purpose  : 
//=======================================================================

Handle(Interface_HGraph) Transfer_TransientProcess::HGraph () const
{
  return myGraph;
}


//=======================================================================
//function : Graph
//purpose  : 
//=======================================================================

const Interface_Graph& Transfer_TransientProcess::Graph () const
{
  return myGraph->Graph();
}


//=======================================================================
//function : SetContext
//purpose  : 
//=======================================================================

void Transfer_TransientProcess::SetContext(const Standard_CString theName,
                                           const Handle(Standard_Transient)& theCtx)
{
  if (myCtx.IsNull()) myCtx = new Dico_DictionaryOfTransient;
  myCtx->SetItem (theName,theCtx);
}


//=======================================================================
//function : GetContext
//purpose  : 
//=======================================================================

Standard_Boolean Transfer_TransientProcess::GetContext
  (const Standard_CString theName, const Handle(Standard_Type)& theType,
   Handle(Standard_Transient)& theCtx) const
{
  if (myCtx.IsNull()) return Standard_False;
  if (!myCtx->GetItem (theName,theCtx)) theCtx.Nullify();
  if (theCtx.IsNull()) return Standard_False;
  if (theType.IsNull()) return Standard_True;
  if (!theCtx->IsKind(theType)) theCtx.Nullify();
  return !theCtx.IsNull();
}


//=======================================================================
//function : Context
//purpose  : 
//=======================================================================

Handle(Dico_DictionaryOfTransient)& Transfer_TransientProcess::Context ()
{
  return myCtx;
}


//=======================================================================
//function : PrintTrace
//purpose  : 
//=======================================================================

void Transfer_TransientProcess::PrintTrace
  (const Handle(Standard_Transient)& theStart, const Handle(Message_Messenger)& theMessenger) const
{
  if (!theStart.IsNull()) {
    theMessenger << "Entity ";
    if (!myModel.IsNull()) {
      myModel->Print (theStart,theMessenger,1);
//      theMessenger<<"id ";  myModel->Print (theStart,theMessenger);
//      theMessenger<<" Type:"<<myModel->TypeName (theStart);
    }
    else theMessenger<<" Type:"<< Interface_InterfaceModel::ClassName(theStart->DynamicType()->Name());
 //  << theStart (handle)  ??
  }
}


//=======================================================================
//function : CheckNum
//purpose  : 
//=======================================================================

Standard_Integer Transfer_TransientProcess::CheckNum
  (const Handle(Standard_Transient)& theStart) const
{
  return (myModel.IsNull() ? 0 : myModel->Number(theStart));
}


//=======================================================================
//function : TypedSharings
//purpose  : 
//=======================================================================

Interface_EntityIterator Transfer_TransientProcess::TypedSharings
  (const Handle(Standard_Transient)& theStart,
   const Handle(Standard_Type)& theType) const
{
  Interface_EntityIterator iter;
  if (myGraph.IsNull()) return iter;
  return myGraph->Graph().TypedSharings (theStart,theType);
}


//=======================================================================
//function : IsDataLoaded
//purpose  : 
//=======================================================================

Standard_Boolean Transfer_TransientProcess::IsDataLoaded
  (const Handle(Standard_Transient)& theStart) const
{
  if (myModel.IsNull()) return Standard_True;
  Standard_Integer aNum = myModel->Number(theStart);
  if (aNum == 0) return Standard_True;
  if (myModel->IsUnknownEntity(aNum)) return Standard_False;
  return !myModel->IsRedefinedContent(aNum);
}


//=======================================================================
//function : IsDataFail
//purpose  : 
//=======================================================================

Standard_Boolean Transfer_TransientProcess::IsDataFail
  (const Handle(Standard_Transient)& theStart) const
{
  if (myModel.IsNull()) return Standard_False;
  Standard_Integer aNum = myModel->Number(theStart);
  if (aNum == 0) return Standard_False;
  if (myModel->IsErrorEntity(aNum)) return Standard_True;
  const Handle(Interface_Check) aCheck = myModel->Check(aNum,Standard_False); // semantic
  return aCheck->HasFailed();
}


//=======================================================================
//function : PrintStats
//purpose  : 
//=======================================================================

void Transfer_TransientProcess::PrintStats(const Standard_Integer /*mode*/,
                                           const Handle(Message_Messenger)& theMessenger) const
{
  theMessenger<<"\n*******************************************************************\n";
//  if (mode == 1) {    //  Statistiques de base
    theMessenger << "********                 Basic Statistics                  ********"<<endl;

    Handle(Interface_InterfaceModel) aModel = Model();
    if (aModel.IsNull())      theMessenger<<"****        Model unknown"<<endl;
    else
      theMessenger<<"****        Nb Entities         : "<< aModel->NbEntities()<<endl;

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
	if (aCheck->NbWarnings() > 0) aNbw ++;
	if (aBinder->HasResult())  aNbr ++;
      }
    }
    if (aNbr > aNbRoots)
      theMessenger<<"****      ( Itermediate Results : "<<aNbr-aNbRoots<<" )\n";
    if (aNbe > 0)
      theMessenger<<"****                  Errors on : "<<Interface_MSG::Blanks(aNbe,4)<<aNbe<<" Entities\n";
    if (aNbw > 0)
      theMessenger<<"****                Warnings on : "<<Interface_MSG::Blanks(aNbw,4)<<aNbw<<" Entities\n";
    theMessenger<<"*******************************************************************";
//  }
  theMessenger<<endl;
}


//=======================================================================
//function : RootsForTransfer
//purpose  : 
//=======================================================================

Handle(TColStd_HSequenceOfTransient) Transfer_TransientProcess::RootsForTransfer() 
{
  return theTrRoots;
}

//=======================================================================
//function : TransferProduct
//purpose  : 
//=======================================================================
  Handle(Transfer_Binder) Transfer_TransientProcess::TransferProduct (const Handle(Standard_Transient)& theStart)
  {
    myLevel ++;             // decrement and if == 0, root transfer
    Handle(Transfer_Binder) aBinder;
    Handle(Transfer_ActorOfTransientProcess) anActor = myActor;
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

  Handle(Transfer_Binder) Transfer_TransientProcess::Transferring (const Handle(Standard_Transient)& theStart)
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
    Standard_Boolean isNewBind = Standard_False;
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
          Bind (theStart,aBinder);  isNewBind = Standard_True;
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

    else  aBinder = TransferProduct(theStart);

    //    Conclusion : Noter dans la Map  

    if (!isNewBind && !aBinder.IsNull()) {
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
      Handle(Transfer_Binder)     aNullBinder;
      return aNullBinder;
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

  Standard_Boolean Transfer_TransientProcess::Transfer(const Handle(Standard_Transient)& theStart)
  {
    Handle(Transfer_Binder) aBinder = Transferring(theStart);
    return (!aBinder.IsNull());
  }

