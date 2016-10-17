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


#include <Dico_DictionaryOfTransient.hxx>
#include <Interface_Check.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_Graph.hxx>
#include <Interface_HGraph.hxx>
#include <Interface_InterfaceModel.hxx>
#include <Interface_MSG.hxx>
#include <Interface_Macros.hxx>
#include <Interface_Static.hxx>
#include <Message_Messenger.hxx>
#include <TColStd_HSequenceOfTransient.hxx>
#include <Transfer_Binder.hxx>
#include <Transfer_TransientProcess.hxx>
#include <Transfer_ResultFromModel.hxx>
#include <Transfer_ResultFromTransient.hxx>
#include <Transfer_SimpleBinderOfTransient.hxx>
#include <TransferBRep.hxx>
#include <TransferBRep_ShapeBinder.hxx>
#include <TransferBRep_ShapeMapper.hxx>
#include <TopoDS_HShape.hxx>
#include <ShapeFix.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Transfer_TransientProcess,Transfer_Process)

static TopoDS_Shape BinderShape (const Handle(Standard_Transient)& tr)
{
  TopoDS_Shape sh;
  DeclareAndCast(Transfer_Binder,sb,tr);
  if (!sb.IsNull()) return TransferBRep::ShapeResult(sb);
  DeclareAndCast(TransferBRep_ShapeMapper,sm,tr);
  if (!sm.IsNull()) return sm->Shape();
  DeclareAndCast(TopoDS_HShape,hs,tr);
  if (!hs.IsNull()) return hs->Shape();
  return sh;
}

//=======================================================================
//function : SetGraph
//purpose  : 
//=======================================================================

void Transfer_TransientProcess::SetGraph(const Handle(Interface_HGraph)& HG)
{
  thegraph = HG;
  if (!thegraph.IsNull())
    myModel = thegraph->Graph().Model();
  else
    myModel.Nullify();
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
//function : PrintTrace
//purpose  : 
//=======================================================================

void Transfer_TransientProcess::PrintTrace (const Handle(Standard_Transient)& theSource, const Handle(Message_Messenger)& S) const
{
  if (!theSource.IsNull()) {
    S << "Entity ";
    if (!myModel.IsNull())
      myModel->Print (theSource,S,1);
    else
      S<<" Type:"<< Interface_InterfaceModel::ClassName(theSource->DynamicType()->Name());
  }
}


//=======================================================================
//function : CheckNum
//purpose  : 
//=======================================================================

Standard_Integer Transfer_TransientProcess::CheckNum (const Handle(Standard_Transient)& start) const
{
  return (myModel.IsNull() ? 0 : myModel->Number(start));
}


//  ########################################################
//  ###########            TRANSFERT            ############
//  ########################################################


//=======================================================================
//function : BeginTransfer
//purpose  : 
//=======================================================================

Standard_Boolean Transfer_TransientProcess::BeginTransfer ()
{
  if (myModel.IsNull()) return Standard_False;
  /*c1:temporarily! if ( myActor.IsNull() && !myController.IsNull())
    myActor = myController->ActorRead(myModel);*/
  return (!myActor.IsNull());
}


//=======================================================================
//function : TransferOne
//purpose  : 
//=======================================================================

Standard_Integer Transfer_TransientProcess::TransferOne (const Handle(Standard_Transient)& ent)
{
  if (myActor.IsNull() || myModel.IsNull()) return 0;

  //c1:if (myTP.IsNull()) { if (!BeginTransfer()) return 0; }

  //c1:if (myGraph.IsNull()) myTP->SetModel(myModel);
  //c1:else                  myTP->SetGraph(myGraph);

  // Check if the entity belongs to the model
  if (myModel->Number(ent) == 0)
    return 0;

  Transfer (ent);
  SetRoot (ent);

  //  Resultat ...
  Handle(Transfer_Binder) binder = Find (ent);
  if (binder.IsNull()) return 0;

  RecordResult (ent);

  if (!binder->HasResult()) return 0;

  return 1;
}


//  ########################################################
//  ###########            RESULTATS            ############


//=======================================================================
//function : RecordResult
//purpose  : 
//=======================================================================

Standard_Boolean Transfer_TransientProcess::RecordResult (const Handle(Standard_Transient)& ent)
{
  if (myModel.IsNull()) return Standard_False;
  Standard_Integer num = myModel->Number(ent);
  if (num == 0) return Standard_False;
  Handle(TCollection_HAsciiString) lab = myModel->StringLabel(ent);

  Handle(Transfer_ResultFromModel) res = new Transfer_ResultFromModel;
  res->Fill (this,ent);

  //   Cas du resultat Shape : pour resultat principal, faire HShape ...
  Handle(Transfer_Binder) binder = res->MainResult()->Binder();
  DeclareAndCast(TransferBRep_ShapeBinder,shb,binder);
  if (!shb.IsNull()) {
    Handle(Transfer_SimpleBinderOfTransient) trb = new Transfer_SimpleBinderOfTransient;
    trb->SetResult ( new TopoDS_HShape(shb->Result()) );
    trb->Merge(binder);
    res->MainResult()->SetBinder (trb);
  }

  //c1:res->SetFileName(myFileName.ToCString());
  myResults.Bind(num,res);
  return Standard_True;
}


//=======================================================================
//function : IsRecorded
//purpose  : 
//=======================================================================

Standard_Boolean Transfer_TransientProcess::IsRecorded (const Handle(Standard_Transient)& ent) const
{
  if (myModel.IsNull()) return Standard_False;
  Standard_Integer num = myModel->Number(ent);
  if (num == 0) return Standard_False;
  if(!myResults.IsBound(num)) return Standard_False;
  return (myResults.Find(num)->DynamicType() == STANDARD_TYPE(Transfer_ResultFromModel) );
}


//=======================================================================
//function : RecordedList
//purpose  : 
//=======================================================================

Handle(TColStd_HSequenceOfTransient) Transfer_TransientProcess::RecordedList () const
{
  Handle(TColStd_HSequenceOfTransient) li = new TColStd_HSequenceOfTransient();
  if (myModel.IsNull())   return li;
  Standard_Integer i, nb = myModel->NbEntities();
  for (i = 1; i <= nb; i ++) {
    if(myResults.IsBound(i))
      if(!myResults.Find(i).IsNull()) li->Append (myModel->Value(i));
  }
  return li;
}


//=======================================================================
//function : FinalResult
//purpose  : 
//=======================================================================

Handle(Transfer_ResultFromModel) Transfer_TransientProcess::FinalResult (const Handle(Standard_Transient)& ent) const
{
  if (myModel.IsNull()) return NULL;
  Standard_Integer num = myModel->Number(ent);
  if (num == 0) return NULL;
  if(!myResults.IsBound(num)) return NULL;
  return Handle(Transfer_ResultFromModel)::DownCast(myResults.Find(num));
}


//=======================================================================
//function : ShapeResult
//purpose  : 
//=======================================================================

TopoDS_Shape Transfer_TransientProcess::ShapeResult (const Handle(Standard_Transient)& ent) const
{
  TopoDS_Shape aResult;
  Handle(Transfer_ResultFromModel) res = FinalResult(ent);
  if (res.IsNull()) return aResult;
  Handle(Transfer_ResultFromTransient) mres = res->MainResult();
  if (mres.IsNull()) return aResult;
  aResult = BinderShape (mres->Binder());
  if (aResult.IsNull()) return aResult;

  //   Ouh la vilaine verrue
  Standard_Real tolang = Interface_Static::RVal("read.encoderegularity.angle");
  if (tolang <= 0) return aResult;
  ShapeFix::EncodeRegularity (aResult,tolang);
  return aResult;
}


//=======================================================================
//function : EntityFromShapeResult
//purpose  : 
//=======================================================================

Handle(Standard_Transient) Transfer_TransientProcess::EntityFromShapeResult (const TopoDS_Shape& res, const Standard_Integer mode) const
{
  Handle(Standard_Transient) nulh, samesh, partner;
  if (res.IsNull()) return nulh;
  Standard_Integer i,j,nb;

  if (mode == 0 || mode == 1 || mode == -1) {
    //  on regarde dans le TransientProcess
    nb = (mode == 0 ? NbRoots() : NbMapped());
    for (j = 1; j <= nb; j ++) {
      i = (mode == 0 ? myModel->Number (Root(j)) : j);
      if (i == 0) continue;
      Handle(Standard_Transient) ent = Mapped(i);
      TopoDS_Shape sh = TransferBRep::ShapeResult (this,ent);
      if (!sh.IsNull()) {
        if (sh == res) return ent;
        // priorites moindre : Same (tjrs) ou Partner (mode < 0)
        if (sh.IsSame(res)) samesh = ent;
        if (mode == -1 && sh.IsPartner(res)) partner= ent;
      }
    }
    //    Ici, pas trouve de vraie egalite. Priorites moindres : Same puis Partner
    if (!samesh.IsNull())  return samesh;
    if (!partner.IsNull()) return partner;  // calcule si mode = -1
    return nulh;
  }

  //   Recherche dans myResults (racines)
  //     2 : Main only  3 : Main + one sub;  4 : all
  if (mode >= 2) {
    nb = myModel->NbEntities();
    for (i = 1; i <= nb; i ++) {
      Handle(Transfer_ResultFromModel) rec = ResultFromNumber (i);
      if (rec.IsNull()) continue;
      
      Handle(TColStd_HSequenceOfTransient) list = rec->Results (mode-2);
      Standard_Integer ir,nr = list->Length();
      for (ir = 1; ir <= nr; ir ++) {
        DeclareAndCast(Transfer_ResultFromTransient,rft,list->Value(ir));
        if (rft.IsNull()) continue;
        TopoDS_Shape sh = BinderShape (rft->Binder());
        if (!sh.IsNull() && sh == res) return rft->Start();
      }
    }
  }

  return nulh;
}


//=======================================================================
//function : CheckedList
//purpose  : 
//=======================================================================

Handle(TColStd_HSequenceOfTransient)  Transfer_TransientProcess::CheckedList
       (const Handle(Standard_Transient)& ent,
        const Interface_CheckStatus withcheck, const Standard_Boolean level) const
{
  Handle(TColStd_HSequenceOfTransient) res = new TColStd_HSequenceOfTransient();
  if (ent.IsNull()) return res;

  if (ent == myModel) {
    Standard_Integer i,nb = myModel->NbEntities();
    for (i = 1; i <= nb; i ++) {
      Handle(Transfer_ResultFromModel) rec = ResultFromNumber (i);
      if (!rec.IsNull()) res->Append (rec->CheckedList(withcheck,level));
    }
  } else if (ent->IsKind(STANDARD_TYPE(TColStd_HSequenceOfTransient))) {
    DeclareAndCast(TColStd_HSequenceOfTransient,list,ent);
    Standard_Integer i,nb = list->Length();
    for (i = 1; i <= nb; i ++) {
      Handle(Transfer_ResultFromModel) rec = FinalResult (list->Value(i));
      if (!rec.IsNull()) res->Append (rec->CheckedList(withcheck,level));
    }
  } else {
    Handle(Transfer_ResultFromModel) rec = FinalResult (ent);
    if (!rec.IsNull()) res = rec->CheckedList(withcheck,level);
  }
  return res;
}


//=======================================================================
//function : PrintTransferStatus
//purpose  : 
//=======================================================================

Standard_Boolean Transfer_TransientProcess::PrintTransferStatus(const Standard_Integer theNum,
                                                                const Handle(Message_Messenger)& theMessenger) const
{
  Handle(Transfer_Binder) binder;
  Handle(Standard_Transient) finder;
  Handle(Standard_Transient) aSource;

  if (theNum == 0) return Standard_False;

  const Handle(Interface_InterfaceModel) &model = Model();
  if (model.IsNull()) cout<<"No Model"<<endl;

  Standard_Integer ne=0, nr=0, max = NbMapped(), maxr = NbRoots();
  if (theNum > 0) {
    if (theNum > max) return Standard_False;
    ne = theNum;
    aSource = Mapped(ne);
    nr = RootIndex(finder);
  } else if (theNum < 0) {
    nr = -theNum;
    if (nr > maxr) return Standard_False;
    aSource = Root(nr);
    ne = MapIndex(aSource);
  }

  theMessenger<<"Transfer Read item n0."<<ne<<" of "<<max;
  if (nr > 0) theMessenger<<"  ** Transfer Root n0."<<ne<<endl;
  if (!model.IsNull())
  { theMessenger<<" In input Model, Entity "; model->Print(aSource,theMessenger); }
  theMessenger<<endl;

  binder = MapItem (ne);
  StartTrace (binder,aSource,0,0);

  //   ***   CHECK   ***
  if (!binder.IsNull()) {
    const Handle(Interface_Check) ch = binder->Check();
    Standard_Integer i, nbw = ch->NbWarnings(), nbf = ch->NbFails();
    if (nbw > 0) {
      theMessenger<<" - Warnings : "<<nbw<<" :\n";
      for (i = 1; i <= nbw; i ++) theMessenger<<ch->CWarning(i)<<endl;
    }
    if (nbf > 0) {
      theMessenger<<" - Fails : "<<nbf<<" :\n";
      for (i = 1; i <= nbf; i ++) theMessenger<<ch->CFail(i)<<endl;
    }
  }
  return Standard_True;
}


//=======================================================================
//function : ResultFromNumber
//purpose  : 
//=======================================================================

Handle(Transfer_ResultFromModel) Transfer_TransientProcess::ResultFromNumber (const Standard_Integer num) const
{
  if ( num<1 || num>myModel->NbEntities() ) return NULL;
  if(!myResults.IsBound(num)) return NULL;
  return Handle(Transfer_ResultFromModel)::DownCast(myResults.Find(num));
}
