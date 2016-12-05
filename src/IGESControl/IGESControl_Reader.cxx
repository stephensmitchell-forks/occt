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

//gka 06.01.99 S3767
//abv 10.04.99 S4136: eliminate using BRepAPI::Precision()

#include <IGESControl_Controller.hxx>
#include <IGESControl_Reader.hxx>
#include <IGESData_IGESEntity.hxx>
#include <IGESData_IGESModel.hxx>
#include <Interface_InterfaceModel.hxx>
#include <Interface_Macros.hxx>
#include <Interface_ShareFlags.hxx>
#include <Interface_Static.hxx>
#include <Message_Messenger.hxx>
#include <TopoDS_Shape.hxx>
#include <Transfer_Binder.hxx>
#include <Transfer_TransientProcess.hxx>
#include <TransferBRep.hxx>
#include <XSControl_WorkSession.hxx>

//=======================================================================
//function : IGESControl_Reader
//purpose  : 
//=======================================================================

IGESControl_Reader::IGESControl_Reader ()
: myReadOnlyVisible(Standard_False)
{
  IGESControl_Controller::Init();
  SetWS(new XSControl_WorkSession);
  SetNorm("IGES");
}


//=======================================================================
//function : IGESControl_Reader
//purpose  : 
//=======================================================================

IGESControl_Reader::IGESControl_Reader (const Handle(XSControl_WorkSession)& WS, const Standard_Boolean scratch)
: myReadOnlyVisible(Standard_False)
{
  IGESControl_Controller::Init();
  SetWS(WS,scratch);
  SetNorm("IGES");
}


//=======================================================================
//function : IGESModel
//purpose  : 
//=======================================================================

Handle(IGESData_IGESModel) IGESControl_Reader::IGESModel () const
{
  return Handle(IGESData_IGESModel)::DownCast(Model());
}



//=======================================================================
//function : NbRootsForTransfer
//purpose  : 
//=======================================================================

Standard_Integer IGESControl_Reader::NbRootsForTransfer()
{
  if (therootsta) return theroots.Length();
  therootsta = Standard_True;
  
  Handle(IGESData_IGESModel) model = IGESModel(); 
  if (model.IsNull()) return 0;
  
  const Handle(XSControl_WorkSession) &session = WS();
  const Handle(XSControl_Controller) &controller = session->NormAdaptor();
  const Handle(Interface_Protocol) &protocol = controller->Protocol();
  Handle(Transfer_ActorOfTransientProcess) actor = controller->ActorRead(model);
  
  Interface_ShareFlags SH (model,protocol);
   
  // sln 11.06.2002 OCC448
  Interface_Static::SetIVal("read.iges.onlyvisible",myReadOnlyVisible);
  
  Standard_Integer nb = model->NbEntities();
  for (Standard_Integer i = 1; i <= nb; i ++) {
    Handle(IGESData_IGESEntity) ent = model->Entity(i);
    if ( SH.IsShared(ent) || ! actor->Recognize (ent) ) continue;
    // on ajoute un traitement pour ne prendre que les entites visibles
    if ( ! myReadOnlyVisible || ent->BlankStatus() == 0 ) {
      theroots.Append(ent);
    }
  }
  
  return theroots.Length();
}
