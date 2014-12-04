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

#include <Message_ProgressInfo.hxx>

IMPLEMENT_STANDARD_HANDLE (Message_ProgressInfo, Standard_Transient)
IMPLEMENT_STANDARD_RTTIEXT(Message_ProgressInfo, Standard_Transient)

//=======================================================================
//function : Default constructor
//purpose  : 
//=======================================================================
Message_ProgressInfo::Message_ProgressInfo()
  : myMin     (0.),
    myMax     (1.),
    myScopesNb(1),
    myPosition(myMin)
{
  updateStep();
}

//=======================================================================
//function : Constructor
//purpose  : 
//=======================================================================
Message_ProgressInfo::Message_ProgressInfo(
  const Standard_CString theName,
  const Standard_Real    theMin,
  const Standard_Real    theMax,
  const Standard_Integer theSubScopesNb)
  : myMin     (theMin),
    myMax     (theMax),
    myScopesNb(theSubScopesNb),
    myPosition(myMin)
{
  SetName(theName);
  updateStep();
}

//=======================================================================
//function : Constructor
//purpose  : 
//=======================================================================
Message_ProgressInfo::Message_ProgressInfo(
  const Handle(TCollection_HAsciiString)& theName,
  const Standard_Real                     theMin,
  const Standard_Real                     theMax,
  const Standard_Integer                  theSubScopesNb)
  : myMin     (theMin),
    myMax     (theMax),
    myScopesNb(theSubScopesNb),
    myPosition(myMin)
{
  SetName(theName);
  updateStep();
}

//=======================================================================
//function : Increment
//purpose  : 
//=======================================================================
void Message_ProgressInfo::Increment(const Standard_Real theStep)
{
  Standard_Mutex::Sentry aSentry(myPositionMutex);
  increment(theStep);
}

//=======================================================================
//function : Increment
//purpose  : 
//=======================================================================
void Message_ProgressInfo::Reset()
{
  Standard_Mutex::Sentry aSentry(myPositionMutex);
  myPosition = myMin;
}

//=======================================================================
//function : increment
//purpose  : 
//=======================================================================
Standard_Boolean Message_ProgressInfo::increment(const Standard_Real theStep)
{
  if (myPosition < myMax)
  {
    myPosition = Min(myPosition + theStep, myMax);
    return Standard_True;
  }

  return Standard_False;
}

//=======================================================================
//function : SetName
//purpose  : 
//=======================================================================
void Message_ProgressInfo::SetName(const Standard_CString theName)
{
  SetName(new TCollection_HAsciiString(theName));
}

//=======================================================================
//function : SetName
//purpose  : 
//=======================================================================
void Message_ProgressInfo::SetName(
  const Handle(TCollection_HAsciiString)& theName)
{
  myName = theName;
}
