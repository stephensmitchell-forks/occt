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

#ifndef _Message_MultithreadProgressScale_HeaderFile
#define _Message_MultithreadProgressScale_HeaderFile

#include <Standard.hxx>
#include <Standard_Macro.hxx>
#include <Message_ProgressInfo.hxx>
#include <Message_MultithreadProgressIndicator.hxx>

DEFINE_STANDARD_HANDLE(Message_MultithreadProgressScale, Message_ProgressInfo)

//! Represents sub-scale data model of hierarchical progress indicator.
//! Synchronizes position of parent sub-scale with own position.
//! Can be used in parallel application to indicate progress of distinct thread.
class Message_MultithreadProgressScale : public Message_ProgressInfo
{
public:
  static Standard_Real Zero()
  {
    return 1e-10;
  }

public:

  //! New sub-scale for the given scale.
  Standard_EXPORT Message_MultithreadProgressScale(
    const Handle(Message_MultithreadProgressScale)&     theParent   = NULL,
    const Handle(Message_MultithreadProgressIndicator)& theProgress = NULL);

  //! New sub-scale with the given parameters for the given scale.
  Standard_EXPORT Message_MultithreadProgressScale(
    const Standard_CString                              theName,
    const Standard_Real                                 theMin,
    const Standard_Real                                 theMax,
    const Standard_Integer                              theSubScopesNb,
    const Handle(Message_MultithreadProgressScale)&     theParent   = NULL,
    const Handle(Message_MultithreadProgressIndicator)& theProgress = NULL);

  //! New sub-scale with the given parameters for the given scale.
  Standard_EXPORT Message_MultithreadProgressScale(
    const Handle(TCollection_HAsciiString)&             theName,
    const Standard_Real                                 theMin,
    const Standard_Real                                 theMax,
    const Standard_Integer                              theSubScopesNb,
    const Handle(Message_MultithreadProgressScale)&     theParent   = NULL,
    const Handle(Message_MultithreadProgressIndicator)& theProgress = NULL);

  //! Destructor.
  Standard_EXPORT virtual ~Message_MultithreadProgressScale();

  //! Closes current scale.
  Standard_EXPORT void Close();

  //! Sets scale name.
  //! If scale has associated progress indicator and its 
  //! name is empty, sets progress name equal to scale name.
  Standard_EXPORT virtual void SetName(
    const Handle(TCollection_HAsciiString)& theName);

  //! Returns parent scale.
  inline const Handle(Message_MultithreadProgressScale)& GetParent() const
  {
    return myParent;
  }

  //! Returns progress indicator associated to current scale. 
  inline const Handle(Message_MultithreadProgressIndicator)& GetProgressIndicator() const
  {
    return myProgress;
  }

  //! Returns progress indicator associated to current scale for changing. 
  inline Handle(Message_MultithreadProgressIndicator)& ChangeProgressIndicator()
  {
    return myProgress;
  }

  //! Checks the nearest progress indicator in hierarchy for UserBreak.
  //! Returns FALSE in case if none of hierarchy nodes has associated one.
  Standard_EXPORT Standard_Boolean UserBreak();

  using Message_ProgressInfo::Increment;

  //! Increments progress position by the given step.
  //! Increments position of parent scale and associated progress indicator
  //! according to the given step scaled to parent range.
  //! @param theStep value the progress position should be incremented by,
  //!                should be consistent with range of current scale. 
  Standard_EXPORT virtual void Increment(const Standard_Real theStep);

  DEFINE_STANDARD_RTTI(Message_MultithreadProgressScale)

private:

  //! Increments position of the given parent node by 
  //! the value of the given step.
  //! Scales value of step the the range of parent node.
  void incrementParent(const Standard_Real                theStep,
                       const Handle(Message_ProgressInfo) theParentNode);

private:
  Handle(Message_MultithreadProgressScale)     myParent;   //!< Pointer to parent scale
  Handle(Message_MultithreadProgressIndicator) myProgress; //!< Progress indicator associated to current scale
};

#endif
