// Created on: 2017-02-10
// Created by: Sergey NIKONOV
// Copyright (c) 2000-2017 OPEN CASCADE SAS
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

#include <Standard_GUID.hxx>
#include <TDF_Label.hxx>
#include <XCAFDoc_Note.hxx>

IMPLEMENT_STANDARD_RTTIEXT(XCAFDoc_Note, TDF_Attribute)

const Standard_GUID& XCAFDoc_Note::GetID()
{
  static Standard_GUID s_ID("FDC0AF06-C4AC-468a-ACFB-4C9388C76D9E");
  return s_ID;
}

Standard_Boolean XCAFDoc_Note::IsMine(const TDF_Label& theLabel)
{
  Handle(XCAFDoc_Note) anAttr;
  return (!theLabel.IsNull() && theLabel.FindAttribute(XCAFDoc_Note::GetID(), anAttr));
}

Handle(XCAFDoc_Note) XCAFDoc_Note::Set(const TDF_Label&                  theLabel,
                                       const Handle(TCollection_HExtendedString)& theUserName,
                                       const Handle(TCollection_HExtendedString)& theTimeStamp)
{
  Handle(XCAFDoc_Note) aNote;
  if (!theLabel.IsNull() && !theLabel.FindAttribute(XCAFDoc_Note::GetID(), aNote))
  {
    aNote = new XCAFDoc_Note();
    aNote->Set(theUserName, theTimeStamp);
    theLabel.AddAttribute(aNote);
  }
  return aNote;
}

XCAFDoc_Note::XCAFDoc_Note()
{
}

void XCAFDoc_Note::Set(const Handle(TCollection_HExtendedString)& theUserName,
                       const Handle(TCollection_HExtendedString)& theTimeStamp)
{
  Backup();

  myUserName = theUserName;
  myTimeStamp = theTimeStamp;
}

Handle(TCollection_HExtendedString) XCAFDoc_Note::UserName() const
{
  return myUserName;
}

Handle(TCollection_HExtendedString) XCAFDoc_Note::TimeStamp() const
{
  return myTimeStamp;
}

const Standard_GUID& XCAFDoc_Note::ID() const
{
  return GetID();
}

Handle(TDF_Attribute) XCAFDoc_Note::NewEmpty() const
{
  return new XCAFDoc_Note();
}

void XCAFDoc_Note::Restore(const Handle(TDF_Attribute)& theAttr)
{
  myUserName = Handle(XCAFDoc_Note)::DownCast(theAttr)->myUserName;
  myTimeStamp = Handle(XCAFDoc_Note)::DownCast(theAttr)->myTimeStamp;
}

void XCAFDoc_Note::Paste(const Handle(TDF_Attribute)&       theAttrInto,
                         const Handle(TDF_RelocationTable)& /*theRT*/) const
{
  Handle(XCAFDoc_Note)::DownCast(theAttrInto)->Set(myUserName, myTimeStamp);
}

Standard_OStream& XCAFDoc_Note::Dump(Standard_OStream& theOS) const
{
  theOS 
    << "Note : " 
    << (myUserName ? myUserName->String() : "<anonymous>")
    << " on "
    << (myTimeStamp ? myTimeStamp->String() : "<unknown>")
    ;
  return theOS;
}
