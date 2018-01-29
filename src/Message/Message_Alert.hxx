// Created on: 2017-06-26
// Created by: Andrey Betenev
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

#ifndef _Message_Alert_HeaderFile
#define _Message_Alert_HeaderFile

#include <Message_Gravity.hxx>
#include <Message_ListOfAlert.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>

class Message_Alert;
class Message_Report;

DEFINE_STANDARD_HANDLE(Message_Alert, Standard_Transient)

//! Base class of the hierarchy of classes describing various situations
//! occurring during execution of some algorithm or procedure.
//!
//! Alert should provide unique text identifier that can be used to distinguish 
//! particular type of alerts, e.g. to get text message string describing it. 
//! See method GetMessageKey(); by default, dynamic type name is used.
//!
//! Alert can contain some data. To avoid duplication of data, new alert
//! can be merged with another one of the same type. Method SupportsMerge() 
//! should return true if merge is supported; method Merge() should do the
//! merge if possible and return true in that case and false otherwise.
//! 
class Message_Alert : public Standard_Transient
{
public:
  //! Empty constructor
  Standard_EXPORT Message_Alert (const TCollection_AsciiString& theName = TCollection_AsciiString())
    : myElapsedTime (-1), myName (theName) {}

  //! Return a C string to be used as a key for generating text user 
  //! messages describing this alert.
  //! The messages are generated with help of Message_Msg class, in
  //! Message_Report::Dump().
  //! Base implementation returns dynamic type name of the instance.
  virtual Standard_EXPORT Standard_CString GetMessageKey () const;

  //! Returns custom name of alert if it is set
  //! @return alert name
  const TCollection_AsciiString& GetName() const { return myName; }

  //! Sets the custom name of alert
  //! @param theName a name for the alert
  void SetName (const TCollection_AsciiString& theName) { myName = theName; }

  //! Return true if this type of alert can be merged with other
  //! of the same type to avoid duplication.
  //! Basis implementation returns true.
  virtual Standard_EXPORT Standard_Boolean SupportsMerge () const;
  
  //! If possible, merge data contained in this alert to theTarget.
  //! @return True if merged.
  //! Base implementation always returns true.
  virtual Standard_EXPORT Standard_Boolean Merge (const Handle(Message_Alert)& theTarget);
  
  //! Returns list of collected alerts with specified gravity
  Standard_EXPORT Message_ListOfAlert& GetAlerts (Message_Gravity theGravity);

  //! Returns the alert elapsed time
  //! @return double value
  Standard_EXPORT Standard_Real ElapsedTime() const { return myElapsedTime; }

  //! Sets elapsed time of alert
  //! \param theElapsedTime time of the alert
  Standard_EXPORT void SetElapsedTime(const Standard_Real theElapsedTime)
  { myElapsedTime = theElapsedTime; }


  // OCCT RTTI
  DEFINE_STANDARD_RTTIEXT(Message_Alert,Standard_Transient)

protected:
  // store messages in a lists sorted by gravity;
  // here we rely on knowledge that Message_Fail is the last element of the enum
  Message_ListOfAlert myChildAlerts[Message_Fail + 1];

  Standard_Real myElapsedTime;
  TCollection_AsciiString myName;
};

//! Macro allowing to define simple alert with name argument in single line of code
#define DEFINE_SIMPLE_ALERT(Alert) \
  class Alert : public Message_Alert \
  { \
  public:\
    Alert (const TCollection_AsciiString& theName = TCollection_AsciiString()) : Message_Alert(theName) {} \
    DEFINE_STANDARD_RTTI_INLINE(Alert, Message_Alert) \
  };

#endif // _Message_Alert_HeaderFile
