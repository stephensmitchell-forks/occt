// Created on: 2014-07-31
// Created by: Oleg AGASHIN
// Copyright (c) 1993-1999 Matra Datavision
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

#include <BRepMesh_ProgressRoot.hxx>
#include <Message_ProgressIndicator.hxx>

IMPLEMENT_STANDARD_HANDLE (BRepMesh_ProgressIndicator, Standard_Transient)
IMPLEMENT_STANDARD_RTTIEXT(BRepMesh_ProgressIndicator, Standard_Transient)

IMPLEMENT_STANDARD_HANDLE (BRepMesh_ProgressRoot, Standard_Transient)
IMPLEMENT_STANDARD_RTTIEXT(BRepMesh_ProgressRoot, Standard_Transient)

IMPLEMENT_STANDARD_HANDLE   (BRepMesh_UserBreak, Standard_ProgramError)
IMPLEMENT_STANDARD_RTTIEXT  (BRepMesh_UserBreak, Standard_ProgramError)
IMPLEMENT_STANDARD_EXCEPTION(BRepMesh_UserBreak)

//***********************************************************************
//
// class BRepMesh_ProgressIndicator
//
//***********************************************************************
//=======================================================================
//function : Constructor
//purpose  : 
//=======================================================================
BRepMesh_ProgressIndicator::BRepMesh_ProgressIndicator(
  const Handle(Message_ProgressIndicator)& theProgress)
  : myProgress(theProgress)
{
}

//=======================================================================
//function : Destructor
//purpose  : 
//=======================================================================
BRepMesh_ProgressIndicator::~BRepMesh_ProgressIndicator()
{
  Standard_Mutex::Sentry aSentry(myProgressMutex);
  myProgress.Nullify();
}

//=======================================================================
//function : SetProgressIndicator
//purpose  : 
//=======================================================================
void BRepMesh_ProgressIndicator::SetProgressIndicator(
  const Handle(Message_ProgressIndicator)& theProgress)
{
  myProgress = theProgress;
}

//=======================================================================
//function : UserBreak
//purpose  : 
//=======================================================================
const Handle(Message_ProgressIndicator)&
BRepMesh_ProgressIndicator::GetProgressIndicator() const
{
  return myProgress;
}

//=======================================================================
//function : UserBreak
//purpose  : 
//=======================================================================
void BRepMesh_ProgressIndicator::UserBreak() const
{
  if (myProgress.IsNull())
    return;

  if (myProgress->UserBreak())
    Raise_BRepMesh_UserBreak();
}

//=======================================================================
//function : SetScale
//purpose  : 
//=======================================================================
void BRepMesh_ProgressIndicator::SetScale(const Standard_Real theMin,
                                          const Standard_Real theMax,
                                          const Standard_Real theStep)
{
  if (myProgress.IsNull())
    return;

  myProgress->SetScale(theMin, theMax, theStep);
}

//=======================================================================
//function : Increment
//purpose  : 
//=======================================================================
void BRepMesh_ProgressIndicator::Increment() const
{
  if (myProgress.IsNull())
    return;

  Standard_Mutex::Sentry aSentry(myProgressMutex);
  myProgress->Increment();
}



//***********************************************************************
//
// class BRepMesh_ProgressRoot
//
//***********************************************************************
//=======================================================================
//function : Ñonstructor
//purpose  : 
//=======================================================================
BRepMesh_ProgressRoot::BRepMesh_ProgressRoot()
{
  myProgress = new BRepMesh_ProgressIndicator(NULL);
}

//=======================================================================
//function : Ñonstructor
//purpose  : 
//=======================================================================
BRepMesh_ProgressRoot::BRepMesh_ProgressRoot(
  const Handle(Message_ProgressIndicator)& theProgress)
{
  myProgress = new BRepMesh_ProgressIndicator(theProgress);
}

//=======================================================================
//function : Ñonstructor
//purpose  : 
//=======================================================================
BRepMesh_ProgressRoot::BRepMesh_ProgressRoot(
  const Handle(BRepMesh_ProgressIndicator)& theProgress)
{
  if (theProgress.IsNull())
    myProgress = new BRepMesh_ProgressIndicator(NULL);
  else
    myProgress = theProgress;
}

//=======================================================================
//function : Destructor
//purpose  : 
//=======================================================================
BRepMesh_ProgressRoot::~BRepMesh_ProgressRoot()
{
}

//=======================================================================
//function : GetProgressIndicator
//purpose  : 
//=======================================================================
const Handle(BRepMesh_ProgressIndicator)& 
  BRepMesh_ProgressRoot::ProgressIndicator() const
{
  return myProgress;
}
