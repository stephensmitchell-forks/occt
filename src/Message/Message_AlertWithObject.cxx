// Created on:
// Created by:
// Copyright (c) 2017 OPEN CASCADE SAS
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

#include <Message_AlertWithObject.hxx>

#include <Message_Msg.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Message_AlertWithObject,Message_Alert)

//=======================================================================
//function : Message_AlertWithObject
//purpose  : 
//=======================================================================

Message_AlertWithObject::Message_AlertWithObject (const Handle(Standard_Transient)& theObject,
                                                  const TCollection_AsciiString& theName)
: Message_Alert (theName)
{
  myObject = theObject;
}
