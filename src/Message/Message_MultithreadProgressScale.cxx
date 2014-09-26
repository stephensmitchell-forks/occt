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

#include <Message_MultithreadProgressScale.hxx>
#include <Message_MultithreadProgressIndicator.hxx>

IMPLEMENT_STANDARD_HANDLE (Message_MultithreadProgressScale, Message_ProgressInfo)
IMPLEMENT_STANDARD_RTTIEXT(Message_MultithreadProgressScale, Message_ProgressInfo)

//=======================================================================
//function : Default constructor
//purpose  : 
//=======================================================================
Message_MultithreadProgressScale::Message_MultithreadProgressScale(
  const Handle(Message_MultithreadProgressScale)&     theParent,
  const Handle(Message_MultithreadProgressIndicator)& theProgress)
  : Message_ProgressInfo(),
    myParent  (theParent),
    myProgress(theProgress)
{
}

//=======================================================================
//function : Constructor
//purpose  : 
//=======================================================================
Message_MultithreadProgressScale::Message_MultithreadProgressScale(
  const Standard_CString                              theName,
  const Standard_Real                                 theMin,
  const Standard_Real                                 theMax,
  const Standard_Integer                              theSubScopesNb,
  const Handle(Message_MultithreadProgressScale)&     theParent,
  const Handle(Message_MultithreadProgressIndicator)& theProgress)
  : Message_ProgressInfo("", theMin, theMax, theSubScopesNb),
    myParent  (theParent),
    myProgress(theProgress)
{
  Message_ProgressInfo::SetName(theName);
}

//=======================================================================
//function : Constructor
//purpose  : 
//=======================================================================
Message_MultithreadProgressScale::Message_MultithreadProgressScale(
  const Handle(TCollection_HAsciiString)&             theName,
  const Standard_Real                                 theMin,
  const Standard_Real                                 theMax,
  const Standard_Integer                              theSubScopesNb,
  const Handle(Message_MultithreadProgressScale)&     theParent,
  const Handle(Message_MultithreadProgressIndicator)& theProgress)
  : Message_ProgressInfo("", theMin, theMax, theSubScopesNb),
    myParent  (theParent),
    myProgress(theProgress)
{
  SetName(theName);
}

//=======================================================================
//function : Destructor.
//purpose  : 
//=======================================================================
Message_MultithreadProgressScale::~Message_MultithreadProgressScale()
{
  Close();
}

//=======================================================================
//function : Close
//purpose  : 
//=======================================================================
void Message_MultithreadProgressScale::Close()
{
  Increment(GetMax() - GetPosition());

  myParent.Nullify();
  myProgress.Nullify();
}

//=======================================================================
//function : SetName
//purpose  : 
//=======================================================================
void Message_MultithreadProgressScale::SetName(
  const Handle(TCollection_HAsciiString)& theName)
{
  Message_ProgressInfo::SetName(theName);
  if (!myProgress.IsNull() && myProgress->GetName().IsNull())
    myProgress->SetName(theName);
}

//=======================================================================
//function : UserBreak
//purpose  : 
//=======================================================================
Standard_Boolean Message_MultithreadProgressScale::UserBreak()
{
  if (!myProgress.IsNull() && myProgress->UserBreak())
    return Standard_True;

  if (!myParent.IsNull() && myParent->UserBreak())
    return Standard_True;

  return Standard_False;
}

//=======================================================================
//function : Increment
//purpose  : 
//=======================================================================
void Message_MultithreadProgressScale::Increment(const Standard_Real theStep)
{
  Message_ProgressInfo::Increment(theStep);
  incrementParent(theStep, myParent);
  incrementParent(theStep, myProgress);
}

//=======================================================================
//function : incrementParent
//purpose  : 
//=======================================================================
void Message_MultithreadProgressScale::incrementParent(
  const Standard_Real                theStep,
  const Handle(Message_ProgressInfo) theParentNode)
{
  if (theParentNode.IsNull())
    return;

  const Standard_Real aSubScopeRange = theParentNode->GetStep();
  const Standard_Real aThisRange     = GetMax() - GetMin();

  const Standard_Real aScaledStep = aSubScopeRange * theStep / aThisRange;
  theParentNode->Increment(aScaledStep);
}
