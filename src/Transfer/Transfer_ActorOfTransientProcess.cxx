// Copyright (c) 1994-1999 Matra Datavision
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

#include <Transfer_ActorOfTransientProcess.hxx>
#include <Transfer_Binder.hxx>
#include <Transfer_Finder.hxx>
#include <Standard_Transient.hxx>

 IMPLEMENT_STANDARD_HANDLE(Transfer_ActorOfTransientProcess, MMgt_TShared)
 IMPLEMENT_STANDARD_RTTIEXT(Transfer_ActorOfTransientProcess, MMgt_TShared)

Transfer_ActorOfTransientProcess::Transfer_ActorOfTransientProcess () { }

Handle(Transfer_Binder)  Transfer_ActorOfTransientProcess::Transfer
    (const Handle(Standard_Transient)& theEnt,
     const Handle(Transfer_TransientProcess)& theTP)
{
  Handle(Standard_Transient) res = TransferTransient (theEnt,theTP);
  if (res.IsNull()) return NullResult();
  return TransientResult (res);
}

Handle(Transfer_Binder)  Transfer_ActorOfTransientProcess::Transferring
  (const Handle(Standard_Transient)& theEnt,
   const Handle(Transfer_TransientProcess)& theTP)
{
  return Transfer (theEnt, Handle(Transfer_TransientProcess)::DownCast(theTP));
}

Handle(Standard_Transient)  Transfer_ActorOfTransientProcess::TransferTransient
  (const Handle(Standard_Transient)& /*theEnt*/,
   const Handle(Transfer_TransientProcess)& /*theTP*/)
{
  Handle(Standard_Transient) aNullRes;
  return aNullRes;
}
