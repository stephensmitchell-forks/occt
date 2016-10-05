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
#include <Message_Messenger.hxx>
#include <TColStd_HSequenceOfTransient.hxx>
#include <Transfer_Binder.hxx>
#include <Transfer_TransientProcess.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Transfer_TransientProcess,Transfer_Process)

//=======================================================================
//function : SetGraph
//purpose  : 
//=======================================================================

void Transfer_TransientProcess::SetGraph(const Handle(Interface_HGraph)& HG)
{
  thegraph = HG;
  if (!thegraph.IsNull())
    SetModel(thegraph->Graph().Model());
  else
    themodel.Nullify();
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

void Transfer_TransientProcess::PrintTrace (const Handle(Standard_Transient)& start, const Handle(Message_Messenger)& S) const
{
  if (!start.IsNull()) {
    S << "Entity ";
    if (!themodel.IsNull())
      themodel->Print (start,S,1);
    else
      S<<" Type:"<< Interface_InterfaceModel::ClassName(start->DynamicType()->Name());
  }
}


//=======================================================================
//function : CheckNum
//purpose  : 
//=======================================================================

Standard_Integer Transfer_TransientProcess::CheckNum (const Handle(Standard_Transient)& start) const
{
  return (themodel.IsNull() ? 0 : themodel->Number(start));
}
