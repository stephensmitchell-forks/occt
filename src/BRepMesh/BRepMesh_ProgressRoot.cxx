// Created on: 2014-12-04
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

IMPLEMENT_STANDARD_HANDLE (BRepMesh_ProgressRoot, Standard_Transient)
IMPLEMENT_STANDARD_RTTIEXT(BRepMesh_ProgressRoot, Standard_Transient)

IMPLEMENT_STANDARD_HANDLE   (BRepMesh_UserBreakException, Standard_ProgramError)
IMPLEMENT_STANDARD_RTTIEXT  (BRepMesh_UserBreakException, Standard_ProgramError)
IMPLEMENT_STANDARD_EXCEPTION(BRepMesh_UserBreakException)

//=======================================================================
//function : ProgressInit
//purpose  : 
//=======================================================================
void BRepMesh_ProgressRoot::ProgressInit(
  const Handle(Message_MultithreadProgressSentry)&    theProgressRootSentry,
  const Handle(Message_MultithreadProgressIndicator)& theProgressIndicator)
{
  const Standard_CString aName = "Root sentry";
  const Standard_Real aMin = 0., aMax = 100.;
  const Standard_Integer aSubNb = 1;
  if (theProgressRootSentry.IsNull())
  {
    myRootSentry = new Message_MultithreadProgressSentry(
      aName, aMin, aMax, aSubNb, theProgressIndicator);
  }
  else
  {
    myRootSentry = theProgressRootSentry;
    if (!theProgressIndicator.IsNull())
    {
      const Handle(Message_MultithreadProgressScale)& aScale = 
        myRootSentry->GetScale();

      if (aScale.IsNull())
        return;

      aScale->ChangeProgressIndicator() = theProgressIndicator;
    }
  }
}

//=======================================================================
//function : Destructor
//purpose  : 
//=======================================================================
BRepMesh_ProgressRoot::~BRepMesh_ProgressRoot()
{
}

//=======================================================================
//function : UserBreak
//purpose  : 
//=======================================================================
void BRepMesh_ProgressRoot::UserBreak() const
{
  if (myRootSentry->UserBreak())
    BRepMesh_UserBreakException::Raise("Operation is cancelled by user");
}
