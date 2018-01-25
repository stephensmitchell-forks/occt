// Created on: 2017-06-26
// Created by: Andrey Betenev
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

#include <Message_Alert.hxx>
#include <Standard_Assert.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Message_Alert,Standard_Transient)

//=======================================================================
//function : GetMessageKey
//purpose  :
//=======================================================================

Standard_CString Message_Alert::GetMessageKey () const
{
  return myName.IsEmpty() ? DynamicType()->Name() : myName.ToCString();
}

//=======================================================================
//function : SupportsMerge
//purpose  : 
//=======================================================================

Standard_Boolean Message_Alert::SupportsMerge () const
{
  // by default, support merge
  return Standard_True;
}

//=======================================================================
//function : Merge
//purpose  : 
//=======================================================================

Standard_Boolean Message_Alert::Merge (const Handle(Message_Alert)& /*theTarget*/)
{
  // by default, merge trivially
  return Standard_True;
}

//=======================================================================
//function : GetAlerts
//purpose  :
//=======================================================================

Message_ListOfAlert& Message_Alert::GetAlerts (Message_Gravity theGravity)
{
  return myChildAlerts[theGravity];
}
