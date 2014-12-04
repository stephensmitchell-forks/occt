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

#ifndef _Message_MultithreadProgressIndicator_HeaderFile
#define _Message_MultithreadProgressIndicator_HeaderFile

#include <Standard.hxx>
#include <Standard_Transient.hxx>
#include <Message_ProgressInfo.hxx>

DEFINE_STANDARD_HANDLE(Message_MultithreadProgressIndicator, Message_ProgressInfo)

//! Defines abstract interface from program to the user.
//! That includes progress indication and user break mechanisms to be used
//! in single-thread as well as in parallel code safely.
//! 
//! Progress indication mechanism is adapted for convenient 
//! usage in hierarchical processes that require indication of 
//! progress at several (sub)levels of the process.
//! For that purpose, it is possible to create restricted sub-scope of
//! indication by specifying part of a current scale that is to be
//! used by the subprocess. Each sub-scope can have its own progress 
//! indicator, so it is possible to create interfaces indicating progress
//! of whole process as well as progress of its part:
//! 
//! 
//! Main progress scale with sub-scales (Associated Main Progress Indicator)
//!                            |
//!        ------------------------------------------------------------
//!        |                   |                   |                  |
//!  Sub-scale 1         Sub-scale 2              ...            Sub-scale N
//!                (Associated progress indicator)
//!                            |
//!                    -----------------
//!                    |               |
//!               Sub-scale 1      Sub-scale 2
//! 
class Message_MultithreadProgressIndicator : public Message_ProgressInfo
{
public:

  //! Update presentation of the progress indicator <br>
  //! Called when progress position is changed <br>
  //! Flag force is intended for forcing update in case if it is <br>
  //! optimized; all internal calls from ProgressIndicator are <br>
  //! done with this flag equal to False <br>
  Standard_EXPORT virtual Standard_Boolean Show(const Standard_Boolean isForce = Standard_True) = 0;

  //! Increments progress position by the given step and calls Show method.
  //! Internally calls increment() method wrapped by mutex.
  //! @param theStep value the progress position should be incremented by,
  //!                should be consistent with range of current scale.
  Standard_EXPORT virtual void Increment(const Standard_Real theStep);

  //! Should return True if user has send a break signal. <br>
  //! Default implementation returns False. <br>
  Standard_EXPORT virtual Standard_Boolean UserBreak();


  DEFINE_STANDARD_RTTI(Message_MultithreadProgressIndicator)

protected:

  Standard_EXPORT Message_MultithreadProgressIndicator();

  Standard_EXPORT Message_MultithreadProgressIndicator(
    const Standard_CString theName,
    const Standard_Real    theMin,
    const Standard_Real    theMax,
    const Standard_Integer theSubScopesNb);

  Standard_EXPORT Message_MultithreadProgressIndicator(
    const Handle(TCollection_HAsciiString)& theName,
    const Standard_Real                     theMin,
    const Standard_Real                     theMax,
    const Standard_Integer                  theSubScopesNb);

  //! Performs updating of progress indicator data within thread safe scope.
  //! By default increments position by the given step.
  Standard_EXPORT virtual Standard_Boolean increment(const Standard_Real theStep);

private:
};

#endif