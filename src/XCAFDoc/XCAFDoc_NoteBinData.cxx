// Created on: 2017-02-13
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

#include <OSD_File.hxx>
#include <Standard_GUID.hxx>
#include <TCollection_HAsciiString.hxx>
#include <TDF_Label.hxx>
#include <XCAFDoc_NoteBinData.hxx>

IMPLEMENT_STANDARD_RTTIEXT(XCAFDoc_NoteBinData, XCAFDoc_Note)

const Standard_GUID& XCAFDoc_NoteBinData::GetID()
{
  static Standard_GUID s_ID("E9055501-F0FC-4864-BE4B-284FDA7DDEAC");
  return s_ID;
}

Standard_Boolean XCAFDoc_NoteBinData::IsMine(const TDF_Label& theLabel)
{
  Handle(XCAFDoc_NoteBinData) anAttr;
  return (!theLabel.IsNull() && theLabel.FindAttribute(XCAFDoc_NoteBinData::GetID(), anAttr));
}

Handle(XCAFDoc_NoteBinData) XCAFDoc_NoteBinData::Set(const TDF_Label&                           theLabel,
                                                     const Handle(TCollection_HExtendedString)& theUserName,
                                                     const Handle(TCollection_HExtendedString)& theTimeStamp,
                                                     OSD_File&                                  theFile,
                                                     const Handle(TCollection_HAsciiString)&    theMIMEtype)
{
  Handle(XCAFDoc_NoteBinData) aNoteBinData;
  if (!theLabel.IsNull() && !theLabel.FindAttribute(XCAFDoc_Note::GetID(), aNoteBinData))
  {
    aNoteBinData = new XCAFDoc_NoteBinData();
    aNoteBinData->XCAFDoc_Note::Set(theUserName, theTimeStamp);
    aNoteBinData->Set(theFile, theMIMEtype);
    theLabel.AddAttribute(aNoteBinData);
  }
  return aNoteBinData;
}

XCAFDoc_NoteBinData::XCAFDoc_NoteBinData()
{
}

void XCAFDoc_NoteBinData::Set(OSD_File&                               theFile,
                              const Handle(TCollection_HAsciiString)& theMIMEtype)
{
  if (!theFile.IsOpen() || !theFile.IsReadable())
    return;

  TCollection_AsciiString aStr;
  theFile.Read(aStr, theFile.Size());

  Backup();

  myData.reset(new TCollection_HAsciiString(aStr));
  myMIMEtype = theMIMEtype;
}

void XCAFDoc_NoteBinData::Set(const Handle(TCollection_HAsciiString)& theData,
                              const Handle(TCollection_HAsciiString)& theMIMEtype)
{
  Backup();

  myData = theData;
  myMIMEtype = theMIMEtype;
}

Handle(TCollection_HAsciiString) XCAFDoc_NoteBinData::Data() const
{
  return myData;
}

Handle(TCollection_HAsciiString) XCAFDoc_NoteBinData::MIMEtype() const
{
  return myMIMEtype;
}

const Standard_GUID& XCAFDoc_NoteBinData::ID() const
{
  return GetID();
}

Handle(TDF_Attribute) XCAFDoc_NoteBinData::NewEmpty() const
{
  return new XCAFDoc_Note();
}

void XCAFDoc_NoteBinData::Restore(const Handle(TDF_Attribute)& theAttr)
{
  XCAFDoc_Note::Restore(theAttr);
  myData = Handle(XCAFDoc_NoteBinData)::DownCast(theAttr)->myData;
  myMIMEtype = Handle(XCAFDoc_NoteBinData)::DownCast(theAttr)->myMIMEtype;
}

void XCAFDoc_NoteBinData::Paste(const Handle(TDF_Attribute)&       theAttrInto,
                                const Handle(TDF_RelocationTable)& theRT) const
{
  XCAFDoc_Note::Paste(theAttrInto, theRT);
  Handle(XCAFDoc_NoteBinData)::DownCast(theAttrInto)->Set(myData, myMIMEtype);
}

Standard_OStream& XCAFDoc_NoteBinData::Dump(Standard_OStream& theOS) const
{
  XCAFDoc_Note::Dump(theOS);
  theOS
    << "\n"
    << "MIME type : "
    << (myMIMEtype ? myMIMEtype->String() : "<none>")
    << "\n"
    << "<BEGIN>"
    << (myData ? myData->String() : "")
    << "<END>"
    ;
  return theOS;
}
