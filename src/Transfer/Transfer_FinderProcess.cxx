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

#include <Transfer_FinderProcess.hxx>

#include <Standard_Failure.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Message_Messenger.hxx>
#include <Interface_CheckIterator.hxx>
#include <Interface_InterfaceModel.hxx>
#include <Interface_HGraph.hxx>
#include <XSControl_WorkSession.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Transfer_FinderProcess,Transfer_Process)


//=======================================================================
//function : PrintTransferStatus
//purpose  : 
//=======================================================================

Standard_Boolean Transfer_FinderProcess::PrintTransferStatus(const Standard_Integer theNum,
                                                             const Handle(Message_Messenger)& theMessenger) const
{
  Handle(Transfer_Binder) binder;
  Handle(Standard_Transient) finder;
  Handle(Standard_Transient) aDestination;

  if (theNum == 0 ) return Standard_False;

  Standard_Integer ne=0, nr=0, max = NbMapped(), maxr = NbRoots();
  if (theNum > 0) {
    if (theNum > max) return Standard_False;
    ne = theNum;
    finder = Mapped(ne);
    nr = RootIndex(finder);
  } else if (theNum < 0) {
    nr = -theNum;
    if (nr > maxr) return Standard_False;
    finder = Root(nr);
    ne = MapIndex(finder);
  }
  aDestination = FindTransient(finder);

  theMessenger<<"Transfer Write item n0."<<ne<<" of "<<max;
  if (nr > 0) theMessenger<<"  ** Transfer Root n0."<<nr<<endl;
  theMessenger<<" -> Type "<<finder->DynamicType()->Name()<<endl;

  StartTrace (binder,finder,0,0);  // pb sout/S

  if (!aDestination.IsNull()) {
    theMessenger<<" ** Transient result, type "<<aDestination->DynamicType()->Name();
    const Handle(Interface_InterfaceModel) &model = Model();
    if (!model.IsNull())
    { theMessenger<<" In output Model, Entity "; model->Print(aDestination,theMessenger); }
    theMessenger<<endl;
  }

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
//function : ClearBinders
//purpose  : 
//=======================================================================

void Transfer_FinderProcess::ClearBinders ()
{
  //Due to big number of chains of binders it is necessary to 
  //collect head binders of each chain in the sequence
  TColStd_SequenceOfTransient aSeqBnd;
  TColStd_SequenceOfTransient aSeqShapes;
  Standard_Integer i;
  for (i = 1; i <= NbMapped(); i++) {
    Handle(Transfer_Binder) bnd = MapItem ( i );
    if(!bnd.IsNull())
      aSeqBnd.Append(bnd);
    const Handle(Standard_Transient) &ash = Mapped(i);
    aSeqShapes.Append(ash);
  }
  //removing finder process containing result of translation.
  Clear();
  //szv_x1:myWS->ClearData(1);
  //szv_x1:myWS->ClearData(5);
  
  //removing each chain of binders
  while(aSeqBnd.Length() >0) {
    Handle(Transfer_Binder) aBnd = Handle(Transfer_Binder)::DownCast(aSeqBnd.Value(1));
    Handle(Standard_Transient) ash = aSeqShapes.Value(1);
    aSeqBnd.Remove(1);
    aSeqShapes.Remove(1);
    ash.Nullify();
    while(!aBnd.IsNull()) {
      Handle(Transfer_Binder) aBndNext = aBnd->NextResult();
      aBnd.Nullify();
      aBnd = aBndNext;
    }
  }
}
