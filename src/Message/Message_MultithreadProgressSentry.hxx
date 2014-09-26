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

#ifndef _Message_MultithreadProgressSentry_HeaderFile
#define _Message_MultithreadProgressSentry_HeaderFile

#include <Standard.hxx>
#include <Standard_Macro.hxx>
#include <Standard_Transient.hxx>
#include <Message_MultithreadProgressIndicator.hxx>
#include <Message_MultithreadProgressScale.hxx>

DEFINE_STANDARD_HANDLE(Message_MultithreadProgressSentry, Standard_Transient)

//! This tool class allows to manage opening/closing scopes 
//! in the ProgressIndicator in convenient and safe way.
class Message_MultithreadProgressSentry : public Standard_Transient
{
public: //! @name constructors

  //! Default constructor.
  Standard_EXPORT Message_MultithreadProgressSentry()
  {
  }

  //! Creates global sub-scale for the given progress indicator.
  //! If the given progress indicator is null handle does nothing.
  Standard_EXPORT Message_MultithreadProgressSentry(
    const Standard_CString                              theName,
    const Standard_Real                                 theMin,
    const Standard_Real                                 theMax,
    const Standard_Integer                              theSubScopesNb,
    const Handle(Message_MultithreadProgressIndicator)& theProgress)
  {
    NewScope(theName, theMin, theMax, theSubScopesNb, theProgress);
  }

  //! Creates global sub-scale for the given progress indicator.
  //! If the given progress indicator is null handle does nothing.
  Standard_EXPORT Message_MultithreadProgressSentry(
    const Handle(TCollection_HAsciiString)&             theName,
    const Standard_Real                                 theMin,
    const Standard_Real                                 theMax,
    const Standard_Integer                              theSubScopesNb,
    const Handle(Message_MultithreadProgressIndicator)& theProgress)
  {
    NewScope(theName, theMin, theMax, theSubScopesNb, theProgress);
  }

  //! Creates new sub-scale for the given one.
  //! If the given sentry is not initialized, 
  //! i.e. does not contain a scale, does nothing.
  Standard_EXPORT Message_MultithreadProgressSentry(
    const Standard_CString                              theName,
    const Standard_Real                                 theMin,
    const Standard_Real                                 theMax,
    const Standard_Integer                              theSubScopesNb,
    const Message_MultithreadProgressSentry&            theParent,
    const Handle(Message_MultithreadProgressIndicator)& theProgress = NULL)
  {
    NewScope(theName, theMin, theMax, theSubScopesNb, theParent, theProgress);
  }

  //! Creates new sub-scale for the given one.
  //! If the given sentry is not initialized, 
  //! i.e. does not contain a scale, does nothing.
  Standard_EXPORT Message_MultithreadProgressSentry(
    const Handle(TCollection_HAsciiString)&             theName,
    const Standard_Real                                 theMin,
    const Standard_Real                                 theMax,
    const Standard_Integer                              theSubScopesNb,
    const Message_MultithreadProgressSentry&            theParent,
    const Handle(Message_MultithreadProgressIndicator)& theProgress = NULL)
  {
    NewScope(theName, theMin, theMax, theSubScopesNb, theParent, theProgress);
  }

  //! Creates new sub-scale for the given one.
  //! If the given sentry is not initialized, 
  //! i.e. does not contain a scale, does nothing.
  Standard_EXPORT Message_MultithreadProgressSentry(
    const Standard_CString                              theName,
    const Standard_Real                                 theMin,
    const Standard_Real                                 theMax,
    const Standard_Integer                              theSubScopesNb,
    const Handle(Message_MultithreadProgressSentry)&    theParent,
    const Handle(Message_MultithreadProgressIndicator)& theProgress = NULL)
  {
    NewScope(theName, theMin, theMax, theSubScopesNb, theParent, theProgress);
  }

  //! Creates new sub-scale for the given one.
  //! If the given sentry is not initialized, 
  //! i.e. does not contain a scale, does nothing.
  Standard_EXPORT Message_MultithreadProgressSentry(
    const Handle(TCollection_HAsciiString)&             theName,
    const Standard_Real                                 theMin,
    const Standard_Real                                 theMax,
    const Standard_Integer                              theSubScopesNb,
    const Handle(Message_MultithreadProgressSentry)&    theParent,
    const Handle(Message_MultithreadProgressIndicator)& theProgress = NULL)
  {
    NewScope(theName, theMin, theMax, theSubScopesNb, theParent, theProgress);
  }


public: //! @name mangement
  
  //! Creates global sub-scale for the given progress indicator.
  //! If the given progress indicator is null handle does nothing.
  inline Standard_Boolean NewScope(
    const Standard_CString                              theName,
    const Standard_Real                                 theMin,
    const Standard_Real                                 theMax,
    const Standard_Integer                              theSubScopesNb,
    const Handle(Message_MultithreadProgressIndicator)& theProgress)
  {
    return NewScope(new TCollection_HAsciiString(theName), 
      theMin, theMax, theSubScopesNb, theProgress);
  }

  //! Creates global sub-scale for the given progress indicator.
  //! If the given progress indicator is null handle does nothing.
  Standard_EXPORT Standard_Boolean NewScope(
    const Handle(TCollection_HAsciiString)&             theName,
    const Standard_Real                                 theMin,
    const Standard_Real                                 theMax,
    const Standard_Integer                              theSubScopesNb,
    const Handle(Message_MultithreadProgressIndicator)& theProgress);

  //! Creates new sub-scale for the given one.
  //! If the given sentry is not initialized, 
  //! i.e. does not contain a scale, does nothing.
  inline Standard_Boolean NewScope(
    const Standard_CString                              theName,
    const Standard_Real                                 theMin,
    const Standard_Real                                 theMax,
    const Standard_Integer                              theSubScopesNb,
    const Message_MultithreadProgressSentry&            theParent,
    const Handle(Message_MultithreadProgressIndicator)& theProgress = NULL)
  {
    return NewScope(new TCollection_HAsciiString(theName), 
      theMin, theMax, theSubScopesNb, theParent, theProgress);
  }

  //! Creates new sub-scale for the given one.
  //! If the given sentry is not initialized, 
  //! i.e. does not contain a scale, does nothing.
  Standard_EXPORT Standard_Boolean NewScope(
    const Handle(TCollection_HAsciiString)&             theName,
    const Standard_Real                                 theMin,
    const Standard_Real                                 theMax,
    const Standard_Integer                              theSubScopesNb,
    const Message_MultithreadProgressSentry&            theParent,
    const Handle(Message_MultithreadProgressIndicator)& theProgress = NULL);

  //! Creates new sub-scale for the given one.
  //! If the given sentry is not initialized, 
  //! i.e. does not contain a scale, does nothing.
  inline Standard_Boolean NewScope(
    const Standard_CString                              theName,
    const Standard_Real                                 theMin,
    const Standard_Real                                 theMax,
    const Standard_Integer                              theSubScopesNb,
    const Handle(Message_MultithreadProgressSentry)&    theParent,
    const Handle(Message_MultithreadProgressIndicator)& theProgress = NULL)
  {
    return NewScope(new TCollection_HAsciiString(theName), 
      theMin, theMax, theSubScopesNb, theParent, theProgress);
  }

  //! Creates new sub-scale for the given one.
  //! If the given sentry is not initialized, 
  //! i.e. does not contain a scale, does nothing.
  inline Standard_Boolean NewScope(
    const Handle(TCollection_HAsciiString)&             theName,
    const Standard_Real                                 theMin,
    const Standard_Real                                 theMax,
    const Standard_Integer                              theSubScopesNb,
    const Handle(Message_MultithreadProgressSentry)&    theParent,
    const Handle(Message_MultithreadProgressIndicator)& theProgress = NULL)
  {
    return theParent.IsNull() ? 
      NewScope(theName, theMin, theMax, theSubScopesNb, theProgress) :
      NewScope(theName, theMin, theMax, theSubScopesNb, *theParent, theProgress);
  }


  //! Closes current scope and opens next one with specified parameters.
  //! Warning: do not use this method in case if current sentry is shared
  //! between several threads due to the occasion of closing of scope related
  //! to different thread. It is necessary to create main sub-scope in distinct 
  //! thread to prevent such situation.
  inline Standard_Boolean NextScope(
    const Standard_CString                              theName,
    const Standard_Real                                 theMin,
    const Standard_Real                                 theMax,
    const Standard_Integer                              theSubScopesNb,
    const Handle(Message_MultithreadProgressIndicator)& theProgress = NULL)
  {
    return NextScope(new TCollection_HAsciiString(theName), 
      theMin, theMax, theSubScopesNb, theProgress);
  }

  //! Closes current scope and opens next one with specified parameters.
  //! Warning: do not use this method in case if current sentry is shared
  //! between several threads due to the occasion of closing of scope related
  //! to different thread. It is necessary to create new scope in distinct 
  //! thread to prevent such situation.
  Standard_EXPORT Standard_Boolean NextScope(
    const Handle(TCollection_HAsciiString)&             theName,
    const Standard_Real                                 theMin,
    const Standard_Real                                 theMax,
    const Standard_Integer                              theSubScopesNb,
    const Handle(Message_MultithreadProgressIndicator)& theProgress = NULL);

  //! Returns current sub-scale.
  inline const Handle(Message_MultithreadProgressScale)& GetScale() const
  {
    return myScale;
  }

  //! Returns progress indicator associated to current scale. 
  inline Handle(Message_MultithreadProgressIndicator) GetProgressIndicator() const
  {
    return myScale.IsNull() ? NULL : myScale->GetProgressIndicator();
  }

  //! Returns user break flag of this scale or FALSE in case if scale is null.
  inline Standard_Boolean UserBreak()
  {
    return myScale.IsNull() ? Standard_False : myScale->UserBreak();
  }

  //! Increments progress position by default step.
  //! Does nothing in case if scale is null handle.
  inline void Increment()
  {
    if (!myScale.IsNull())
      myScale->Increment();
  }

  //! Increments progress position by the given step.
  //! Does nothing in case if scale is null handle.
  //! @param theStep value the progress position should be incremented by,
  //!                should be consistent with range of current scale. 
  inline void Increment(const Standard_Real theStep)
  {
    if (!myScale.IsNull())
      myScale->Increment(theStep);
  }


  DEFINE_STANDARD_RTTI(Message_MultithreadProgressSentry)

private:

  //! Returns TRUE in case if creation of sub-scope for the given parent is possible.
  Standard_Boolean subScopeAllowed(const Handle(Message_ProgressInfo)& theParent) const;

private:
  Handle(Message_MultithreadProgressScale) myScale;
};

#endif
