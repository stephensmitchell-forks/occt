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

#ifndef _BRepMesh_ProgressRoot_HeaderFile
#define _BRepMesh_ProgressRoot_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineHandle.hxx>
#include <Standard_DefineException.hxx>
#include <Standard_ProgramError.hxx>
#include <Handle_Message_ProgressIndicator.hxx>
#include <Standard_Mutex.hxx>

class Message_ProgressIndicator;

//! Auxiliary class implementing simple protection for multi thread execution.
class BRepMesh_ProgressIndicator : public Standard_Transient
{
public:

  //! Constructor.
  //! @param theProgress progress indicator to be used in successor algorithms.
  Standard_EXPORT BRepMesh_ProgressIndicator(
    const Handle(Message_ProgressIndicator)& theProgress);

  //! Destructor.
  Standard_EXPORT virtual ~BRepMesh_ProgressIndicator();

  //! Sets custom progress indicator.
  //! @param theProgress progress indicator to be used in successor algorithms.
  Standard_EXPORT void SetProgressIndicator(
    const Handle(Message_ProgressIndicator)& theProgress);

  //! Gets current progress indicator.
  Standard_EXPORT const Handle(Message_ProgressIndicator)&
    GetProgressIndicator() const;

  //! Checks whether break of algorithm is requested by user.
  //! Throws BRepMesh_UserBreak exception.
  Standard_EXPORT void UserBreak() const;

  //! Sets parameters of current scale.
  //! @param theMin minimum value of scale.
  //! @param theMax maximum value of scale.
  //! @param theStep step of scale.
  Standard_EXPORT void SetScale(const Standard_Real theMin,
                                const Standard_Real theMax,
                                const Standard_Real theStep);

  //! Increments progress value by one step of scale.
  Standard_EXPORT void Increment() const;

  DEFINE_STANDARD_RTTI(BRepMesh_ProgressIndicator)

private:

  Handle(Message_ProgressIndicator) myProgress;
  mutable Standard_Mutex            myProgressMutex;
};

DEFINE_STANDARD_HANDLE(BRepMesh_ProgressIndicator, Standard_Transient)

DEFINE_STANDARD_HANDLE   (BRepMesh_UserBreak, Standard_ProgramError)
DEFINE_STANDARD_EXCEPTION(BRepMesh_UserBreak, Standard_ProgramError)


//! Base class for classes that are expected to 
//! use progress indication functionality.
//! Implements common methods to control progress 
//! indication and user break features.
class BRepMesh_ProgressRoot : public Standard_Transient
{
public:

  //! Constructor.
  Standard_EXPORT BRepMesh_ProgressRoot();

  //! Constructor.
  //! @param theProgress progress indicator to be used in successor algorithms.
  Standard_EXPORT BRepMesh_ProgressRoot(
    const Handle(Message_ProgressIndicator)& theProgress);

  //! Constructor.
  //! @param theProgress progress indicator to be used in successor algorithms.
  Standard_EXPORT BRepMesh_ProgressRoot(
    const Handle(BRepMesh_ProgressIndicator)& theProgress);

  //! Destructor.
  Standard_EXPORT virtual ~BRepMesh_ProgressRoot();

  //! Gets current progress indicator.
  Standard_EXPORT const Handle(BRepMesh_ProgressIndicator)& 
    ProgressIndicator() const;

  DEFINE_STANDARD_RTTI(BRepMesh_ProgressRoot)

private:

  Handle(BRepMesh_ProgressIndicator) myProgress;
};

DEFINE_STANDARD_HANDLE(BRepMesh_ProgressRoot, Standard_Transient)

#endif
