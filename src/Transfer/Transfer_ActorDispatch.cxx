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

#include <Standard_Type.hxx>
#include <Interface_InterfaceError.hxx>
#include <Interface_InterfaceModel.hxx>
#include <Interface_Protocol.hxx>
#include <Transfer_ActorOfTransientProcess.hxx>
#include <Transfer_TransferDispatch.hxx>
#include <Transfer_Binder.hxx>
#include <Standard_Transient.hxx>
#include <Transfer_TransientProcess.hxx>
#include <Transfer_ActorDispatch.hxx>
#include <Transfer_TransientProcess.hxx>

IMPLEMENT_STANDARD_TYPE(Transfer_ActorDispatch)
IMPLEMENT_STANDARD_SUPERTYPE_ARRAY()
  STANDARD_TYPE(Transfer_ActorOfTransientProcess),
  STANDARD_TYPE(MMgt_TShared),
  STANDARD_TYPE(Standard_Transient),

IMPLEMENT_STANDARD_SUPERTYPE_ARRAY_END()
IMPLEMENT_STANDARD_TYPE_END(Transfer_ActorDispatch)

IMPLEMENT_DOWNCAST(Transfer_ActorDispatch,Standard_Transient)
IMPLEMENT_STANDARD_RTTI(Transfer_ActorDispatch)


Transfer_ActorDispatch::Transfer_ActorDispatch
  (const Handle(Interface_InterfaceModel)& amodel,
   const Interface_GeneralLib& lib)
    :  thetool (amodel,lib)
{
  SetLast(Standard_True);  // actor par defaut
  thetool.TransientProcess()->SetActor(this);
}

    Transfer_ActorDispatch::Transfer_ActorDispatch
  (const Handle(Interface_InterfaceModel)& amodel,
   const Handle(Interface_Protocol)& protocol)
    :  thetool (amodel,protocol)
{
  SetLast(Standard_True);  // actor par defaut
  thetool.TransientProcess()->SetActor(this);
}

    Transfer_ActorDispatch::Transfer_ActorDispatch
  (const Handle(Interface_InterfaceModel)& amodel)
    :  thetool (amodel)
{
  SetLast(Standard_True);  // actor par defaut
  thetool.TransientProcess()->SetActor(this);
}


    void  Transfer_ActorDispatch::AddActor
  (const Handle(Transfer_ActorOfTransientProcess)& actor)
      {  thetool.TransientProcess()->SetActor(actor);  }

    Transfer_TransferDispatch&  Transfer_ActorDispatch::TransferDispatch ()
      {  return thetool;  }


    Handle(Transfer_Binder)  Transfer_ActorDispatch::Transfer
  (const Handle(Standard_Transient)& start,
   const Handle(Transfer_TransientProcess)& /*TP*/)
{
  thetool.TransferEntity(start);
  return thetool.TransientProcess()->Find(start);
}
