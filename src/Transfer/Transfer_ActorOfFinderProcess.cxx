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

#include <Transfer_ActorOfFinderProcess.hxx>
#include <Transfer_Binder.hxx>
#include <Transfer_Finder.hxx>
#include <Transfer_TransientMapper.hxx>

IMPLEMENT_STANDARD_HANDLE(Transfer_ActorOfFinderProcess, MMgt_TShared)
IMPLEMENT_STANDARD_RTTIEXT(Transfer_ActorOfFinderProcess, MMgt_TShared)
 
Transfer_ActorOfFinderProcess::Transfer_ActorOfFinderProcess ()    {  myModeTrans = 0;  }

Standard_Integer& Transfer_ActorOfFinderProcess::ModeTrans ()
{  return myModeTrans;  }

Handle(Transfer_Binder)  Transfer_ActorOfFinderProcess::Transfer
  (const Handle(Transfer_Finder)& theFinder,
   const Handle(Transfer_FinderProcess)& theFP)
{
  Handle(Transfer_TransientMapper) tm = Handle(Transfer_TransientMapper)::DownCast (theFinder);
  if (tm.IsNull()) return NullResult();
  Handle(Standard_Transient) res = TransferTransient (tm->Value(),theFP);
  if (res.IsNull()) return NullResult();
  return TransientResult (res);
}

Handle(Transfer_Binder)  Transfer_ActorOfFinderProcess::Transferring
  (const Handle(Transfer_Finder)& theFinder,
   const Handle(Transfer_FinderProcess)& theTP)
{
  return Transfer(theFinder,Handle(Transfer_FinderProcess)::DownCast(theTP));
}

Handle(Standard_Transient)  Transfer_ActorOfFinderProcess::TransferTransient
  (const Handle(Standard_Transient)& /*theEnt*/,
   const Handle(Transfer_FinderProcess)& )
{
  Handle(Standard_Transient) aNullRes;
  return aNullRes;
}
