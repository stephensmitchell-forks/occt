// Copyright (c) 1999-2013 OPEN CASCADE SAS
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
