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
  (const Standard_Integer nb) : Transfer_TransferProcess (nb)  
{  
  thetrroots = new TColStd_HSequenceOfTransient;
}


//=======================================================================
//function : SetModel
//purpose  : 
//=======================================================================

void Transfer_TransientProcess::SetModel
  (const Handle(Interface_InterfaceModel)& model)
{
  themodel = model;
}


//=======================================================================
//function : Model
//purpose  : 
//=======================================================================

Handle(Interface_InterfaceModel) Transfer_TransientProcess::Model () const
{
  return themodel;
}


void Transfer_TransientProcess::SetGraph(const Handle(Interface_HGraph)& HG)
{
  thegraph = HG;
  if (!thegraph.IsNull())
    SetModel(thegraph->Graph().Model());
  else
    themodel.Nullify();
}


//=======================================================================
//function : HasGraph
//purpose  : 
//=======================================================================

Standard_Boolean Transfer_TransientProcess::HasGraph () const
{
  return !thegraph.IsNull();
}


//=======================================================================
//function : HGraph
//purpose  : 
//=======================================================================

Handle(Interface_HGraph) Transfer_TransientProcess::HGraph () const
{
  return thegraph;
}


//=======================================================================
//function : Graph
//purpose  : 
//=======================================================================

const Interface_Graph& Transfer_TransientProcess::Graph () const
{
  return thegraph->Graph();
}


//=======================================================================
//function : SetContext
//purpose  : 
//=======================================================================

void Transfer_TransientProcess::SetContext(const Standard_CString name,
                                           const Handle(Standard_Transient)& ctx)
{
  if (thectx.IsNull()) thectx = new Dico_DictionaryOfTransient;
  thectx->SetItem (name,ctx);
}


//=======================================================================
//function : GetContext
//purpose  : 
//=======================================================================

Standard_Boolean Transfer_TransientProcess::GetContext
  (const Standard_CString name, const Handle(Standard_Type)& type,
   Handle(Standard_Transient)& ctx) const
{
  if (thectx.IsNull()) return Standard_False;
  if (!thectx->GetItem (name,ctx)) ctx.Nullify();
  if (ctx.IsNull()) return Standard_False;
  if (type.IsNull()) return Standard_True;
  if (!ctx->IsKind(type)) ctx.Nullify();
  return !ctx.IsNull();
}


//=======================================================================
//function : Context
//purpose  : 
//=======================================================================

Handle(Dico_DictionaryOfTransient)& Transfer_TransientProcess::Context ()
{
  return thectx;
}


//=======================================================================
//function : PrintTrace
//purpose  : 
//=======================================================================

void Transfer_TransientProcess::PrintTrace
  (const Handle(Standard_Transient)& start, const Handle(Message_Messenger)& S) const
{
  if (!start.IsNull()) {
    S << "Entity ";
    if (!themodel.IsNull()) {
      themodel->Print (start,S,1);
//      S<<"id ";  themodel->Print (start,S);
//      S<<" Type:"<<themodel->TypeName (start);
    }
    else S<<" Type:"<< Interface_InterfaceModel::ClassName(start->DynamicType()->Name());
 //  << start (handle)  ??
  }
}


//=======================================================================
//function : CheckNum
//purpose  : 
//=======================================================================

Standard_Integer Transfer_TransientProcess::CheckNum
  (const Handle(Standard_Transient)& start) const
{
  return (themodel.IsNull() ? 0 : themodel->Number(start));
}


//=======================================================================
//function : TypedSharings
//purpose  : 
//=======================================================================

Interface_EntityIterator Transfer_TransientProcess::TypedSharings
  (const Handle(Standard_Transient)& start,
   const Handle(Standard_Type)& type) const
{
  Interface_EntityIterator iter;
  if (thegraph.IsNull()) return iter;
  return thegraph->Graph().TypedSharings (start,type);
}


//=======================================================================
//function : IsDataLoaded
//purpose  : 
//=======================================================================

Standard_Boolean Transfer_TransientProcess::IsDataLoaded
  (const Handle(Standard_Transient)& start) const
{
  if (themodel.IsNull()) return Standard_True;
  Standard_Integer num = themodel->Number(start);
  if (num == 0) return Standard_True;
  if (themodel->IsUnknownEntity(num)) return Standard_False;
  return !themodel->IsRedefinedContent(num);
}


//=======================================================================
//function : IsDataFail
//purpose  : 
//=======================================================================

Standard_Boolean Transfer_TransientProcess::IsDataFail
  (const Handle(Standard_Transient)& start) const
{
  if (themodel.IsNull()) return Standard_False;
  Standard_Integer num = themodel->Number(start);
  if (num == 0) return Standard_False;
  if (themodel->IsErrorEntity(num)) return Standard_True;
  const Handle(Interface_Check) ach = themodel->Check(num,Standard_False); // semantic
  return ach->HasFailed();
}


//=======================================================================
//function : PrintStats
//purpose  : 
//=======================================================================

void Transfer_TransientProcess::PrintStats(const Standard_Integer /*mode*/,
                                           const Handle(Message_Messenger)& S) const
{
  S<<"\n*******************************************************************\n";
//  if (mode == 1) {    //  Statistiques de base
    S << "********                 Basic Statistics                  ********"<<endl;

    Handle(Interface_InterfaceModel) model = Model();
    if (model.IsNull())      S<<"****        Model unknown"<<endl;
    else
      S<<"****        Nb Entities         : "<<model->NbEntities()<<endl;

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
	if (ach->NbWarnings() > 0) nbw ++;
	if (binder->HasResult())  nbr ++;
      }
    }
    if (nbr > nbroots)
      S<<"****      ( Itermediate Results : "<<nbr-nbroots<<" )\n";
    if (nbe > 0)
      S<<"****                  Errors on : "<<Interface_MSG::Blanks(nbe,4)<<nbe<<" Entities\n";
    if (nbw > 0)
      S<<"****                Warnings on : "<<Interface_MSG::Blanks(nbw,4)<<nbw<<" Entities\n";
    S<<"*******************************************************************";
//  }
  S<<endl;
}


//=======================================================================
//function : RootsForTransfer
//purpose  : 
//=======================================================================

Handle(TColStd_HSequenceOfTransient) Transfer_TransientProcess::RootsForTransfer() 
{
  return thetrroots;
}

//=======================================================================
//function : TransferProduct
//purpose  : 
//=======================================================================
  Handle(Transfer_Binder) Transfer_TransientProcess::TransferProduct (const Handle(Standard_Transient)& start)
  {
    thelevel ++;             // decrement and if == 0, root transfer
    Handle(Transfer_Binder) binder;
    Handle(Transfer_ActorOfTransientProcess) actor = theactor;
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

  Handle(Transfer_Binder) Transfer_TransientProcess::Transferring (const Handle(Standard_Transient)& start)
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

  Standard_Boolean Transfer_TransientProcess::Transfer(const Handle(Standard_Transient)& start)
  {
    Handle(Transfer_Binder) binder = Transferring(start);
    return (!binder.IsNull());
  }

