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

#include <Message_MultithreadProgressIndicator.hxx>

#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_HANDLE (Message_MultithreadProgressIndicator, Message_ProgressInfo)
IMPLEMENT_STANDARD_RTTIEXT(Message_MultithreadProgressIndicator, Message_ProgressInfo)

//=======================================================================
//function : Default constructor
//purpose  : 
//=======================================================================
Message_MultithreadProgressIndicator::Message_MultithreadProgressIndicator()
  : Message_ProgressInfo()
{
}

//=======================================================================
//function : Constructor
//purpose  : 
//=======================================================================
Message_MultithreadProgressIndicator::Message_MultithreadProgressIndicator(
  const Standard_CString theName,
  const Standard_Real    theMin,
  const Standard_Real    theMax,
  const Standard_Integer theSubScopesNb)
  : Message_ProgressInfo(theName, theMin, theMax, theSubScopesNb)
{
}

//=======================================================================
//function : Constructor
//purpose  : 
//=======================================================================
Message_MultithreadProgressIndicator::Message_MultithreadProgressIndicator(
  const Handle(TCollection_HAsciiString)& theName,
  const Standard_Real                     theMin,
  const Standard_Real                     theMax,
  const Standard_Integer                  theSubScopesNb)
  : Message_ProgressInfo(theName, theMin, theMax, theSubScopesNb)
{
}

//=======================================================================
//function : UserBreak
//purpose  : 
//=======================================================================
Standard_Boolean Message_MultithreadProgressIndicator::UserBreak()
{
  return Standard_False;
}

//=======================================================================
//function : Increment
//purpose  : 
//=======================================================================
void Message_MultithreadProgressIndicator::Increment(
  const Standard_Real theStep)
{
  Message_ProgressInfo::Increment(theStep);
  Show(Standard_False);
}

//=======================================================================
//function : increment
//purpose  : 
//=======================================================================
Standard_Boolean Message_MultithreadProgressIndicator::increment(
  const Standard_Real theStep)
{
  if (!Message_ProgressInfo::increment(theStep))
    return Standard_False;

  Show(Standard_False);
  return Standard_True;
}
