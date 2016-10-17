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


#include <Interface_InterfaceModel.hxx>
#include <TCollection_AsciiString.hxx>
#include <Transfer_ActorOfTransientProcess.hxx>
#include <XSSelect_SelectForTransfer.hxx>
#include <Transfer_TransientProcess.hxx>

IMPLEMENT_STANDARD_RTTIEXT(XSSelect_SelectForTransfer,IFSelect_SelectExtract)

Standard_Boolean XSSelect_SelectForTransfer::Sort
  (const Standard_Integer /*rank*/, const Handle(Standard_Transient)& ent,
   const Handle(Interface_InterfaceModel)& /*model*/) const
{
  Handle(Transfer_ActorOfTransientProcess) act;
  if (!theTP.IsNull()) act = theTP->GetActor();
  if (!act.IsNull()) return act->Recognize(ent);
  return Standard_False;
}

 
TCollection_AsciiString XSSelect_SelectForTransfer::ExtractLabel () const
{
  return TCollection_AsciiString(theTP.IsNull()? "Recognized for Transfer" : "Recognized for Transfer (current actor)");
}
