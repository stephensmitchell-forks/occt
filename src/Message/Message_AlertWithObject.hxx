// Created on:
// Created by:
// Copyright (c) 2017 OPEN CASCADE SAS
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

#ifndef _Message_AlertWithObject_HeaderFile
#define _Message_AlertWithObject_HeaderFile

#include <Message_Alert.hxx>
#include <Standard_Transient.hxx>
#include <TCollection_AsciiString.hxx>

//! Alert object storing Transient object in its field
class Message_AlertWithObject : public Message_Alert
{
public:
  //! Constructor with string argument
  Standard_EXPORT Message_AlertWithObject (const Handle(Standard_Transient)& theObject,
    const TCollection_AsciiString& theName = TCollection_AsciiString());

  //! Sets the object
  //! @param theObject an instance
  void SetObject (Handle(Standard_Transient) theObject) { myObject = theObject; }

  //! Returns object
  Handle(Standard_Transient) GetObject() const { return myObject; }

  //! Returns false.
  virtual Standard_Boolean SupportsMerge () const Standard_OVERRIDE { return Standard_False; }

  //! Returns false.
  virtual Standard_Boolean Merge (const Handle(Message_Alert)& theTarget) Standard_OVERRIDE
  { (void)theTarget; return Standard_False; }

  // OCCT RTTI
  DEFINE_STANDARD_RTTIEXT(Message_AlertWithObject, Message_Alert)

private:
  Handle(Standard_Transient) myObject;
};

//! Helper macro allowing to define alert with shape argument in one line of code
#define DEFINE_ALERT_WITH_OBJECT(Alert) \
  class Alert : public Message_AlertWithObject \
  { \
  public:\
    Alert (const Handle(Standard_Transient)& theObject, const TCollection_AsciiString& theName = TCollection_AsciiString()) \
    : Message_AlertWithObject(theObject, theName) {} \
    DEFINE_STANDARD_RTTI_INLINE(Alert, Message_AlertWithObject) \
  };

#endif // _Message_AlertWithObject_HeaderFile
