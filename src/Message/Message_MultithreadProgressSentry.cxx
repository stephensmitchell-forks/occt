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

#include <Message_MultithreadProgressSentry.hxx>

IMPLEMENT_STANDARD_HANDLE (Message_MultithreadProgressSentry, Standard_Transient)
IMPLEMENT_STANDARD_RTTIEXT(Message_MultithreadProgressSentry, Standard_Transient)

//=======================================================================
//function : NewScope
//purpose  : 
//=======================================================================
Standard_Boolean Message_MultithreadProgressSentry::NewScope(
  const Handle(TCollection_HAsciiString)&             theName,
  const Standard_Real                                 theMin,
  const Standard_Real                                 theMax,
  const Standard_Integer                              theSubScopesNb,
  const Handle(Message_MultithreadProgressIndicator)& theProgress)
{
  if (!subScopeAllowed(theProgress))
  {
    myScale.Nullify();
    return Standard_False;
  }

  myScale = new Message_MultithreadProgressScale(theName,
    theMin, theMax, theSubScopesNb, NULL, theProgress);

  return Standard_True;
}

//=======================================================================
//function : NewScope
//purpose  : 
//=======================================================================
Standard_Boolean Message_MultithreadProgressSentry::NewScope(
  const Handle(TCollection_HAsciiString)&             theName,
  const Standard_Real                                 theMin,
  const Standard_Real                                 theMax,
  const Standard_Integer                              theSubScopesNb,
  const Message_MultithreadProgressSentry&            theParent,
  const Handle(Message_MultithreadProgressIndicator)& theProgress)
{
  if (!subScopeAllowed(theParent.GetScale()))
  {
    myScale.Nullify();
    return Standard_False;
  }

  myScale = new Message_MultithreadProgressScale(theName,
    theMin, theMax, theSubScopesNb, theParent.GetScale(), theProgress);

  return Standard_True;
}

//=======================================================================
//function : NextScope
//purpose  : 
//=======================================================================
Standard_Boolean Message_MultithreadProgressSentry::NextScope(
  const Handle(TCollection_HAsciiString)&             theName,
  const Standard_Real                                 theMin,
  const Standard_Real                                 theMax,
  const Standard_Integer                              theSubScopesNb,
  const Handle(Message_MultithreadProgressIndicator)& theProgress)
{
  Handle(Message_MultithreadProgressScale) aParentScale;
  if (!myScale.IsNull())
  {
    aParentScale = myScale->GetParent();
    myScale->Close();
    myScale.Nullify();
  }

  if (!subScopeAllowed(aParentScale))
    return Standard_False;

  myScale = new Message_MultithreadProgressScale(theName,
    theMin, theMax, theSubScopesNb, aParentScale, theProgress);

  return Standard_True;
}

//=======================================================================
//function : subScopeAllowed
//purpose  : 
//=======================================================================
Standard_Boolean Message_MultithreadProgressSentry::subScopeAllowed(
  const Handle(Message_ProgressInfo)& theParent) const
{
  if (theParent.IsNull())
    return Standard_False;

  Standard_Real aMax = theParent->GetMax();
  Standard_Real aPos = theParent->GetPosition();
  if (aMax - aPos < Message_MultithreadProgressScale::Zero())
  {
    // noting to manage.
    return Standard_False;
  }

  return Standard_True;
}
