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

#ifndef _BRepMesh_ProgressRoot_HeaderFile
#define _BRepMesh_ProgressRoot_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineHandle.hxx>
#include <Standard_DefineException.hxx>
#include <Standard_ProgramError.hxx>
#include <Message_MultithreadProgressIndicator.hxx>
#include <Message_MultithreadProgressSentry.hxx>

DEFINE_STANDARD_HANDLE   (BRepMesh_UserBreakException, Standard_ProgramError)
DEFINE_STANDARD_EXCEPTION(BRepMesh_UserBreakException, Standard_ProgramError)

//! Base class for classes that are expected to use 
//! progress indication functionality in BRepMesh package.
//! It always has an instance of ProgressSentry to be used to
//! create sub-scales.
//! Implements user break
class BRepMesh_ProgressRoot : public Standard_Transient
{
public:

  //! Constructor.
  //! @param theProgressRootSentry root progress sentry. If Null, new sentry will be created.
  //! @param theProgressIndicator progress indicator to be used in successor algorithms.
  Standard_EXPORT BRepMesh_ProgressRoot(
    const Handle(Message_MultithreadProgressSentry)&    theProgressRootSentry = NULL,
    const Handle(Message_MultithreadProgressIndicator)& theProgressIndicator  = NULL)
  {
    ProgressInit(theProgressRootSentry, theProgressIndicator);
  }

  //! Destructor.
  Standard_EXPORT virtual ~BRepMesh_ProgressRoot();
  
  //! Checks whether break of algorithm is requested by user.
  //! Throws BRepMesh_UserBreak exception.
  Standard_EXPORT virtual void UserBreak() const;
  
  //! Initializes tool.
  //! @param theProgressRootSentry root progress sentry.
  //! If Null, new sentry will be created, otherwise the 
  //! given one will be used.
  //! @param theProgressIndicator progress indicator to be 
  //! used in successor algorithms. Replaces progress indicator
  //! of the given sentry if it is not Null.
  Standard_EXPORT virtual void ProgressInit(
    const Handle(Message_MultithreadProgressSentry)&    theProgressRootSentry = NULL,
    const Handle(Message_MultithreadProgressIndicator)& theProgressIndicator  = NULL);

  //! Gets current progress indicator.
  Standard_EXPORT Handle(Message_MultithreadProgressIndicator) 
    ProgressIndicator() const
  {
    return myRootSentry->GetProgressIndicator();
  }
    
  //! Gets current progress indicator.
  Standard_EXPORT const Handle(Message_MultithreadProgressSentry)& 
    ProgressRootSentry() const
  {
    return myRootSentry;
  }

  DEFINE_STANDARD_RTTI(BRepMesh_ProgressRoot)

private:

  Handle(Message_MultithreadProgressSentry) myRootSentry;
};

DEFINE_STANDARD_HANDLE(BRepMesh_ProgressRoot, Standard_Transient)

#endif
