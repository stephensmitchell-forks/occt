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

Handle(XCAFDoc_Note) XCAFDoc_Note::Set(const TDF_Label& theLabel)
{
  Handle(XCAFDoc_Note) aTool;
  if (!theLabel.FindAttribute(XCAFDoc_Note::GetID(), aTool))
  {
    aTool = new XCAFDoc_Note();
    theLabel.AddAttribute(aTool);
  }
  return aTool;
}

XCAFDoc_Note::XCAFDoc_Note()
{
}

const Standard_GUID& XCAFDoc_Note::ID() const
{
  return GetID();
}

Handle(TDF_Attribute) XCAFDoc_Note::NewEmpty() const
{
  return new XCAFDoc_Note();
}

void XCAFDoc_Note::Restore(const Handle(TDF_Attribute)& /*theAttr*/)
{
}

void XCAFDoc_Note::Paste(const Handle(TDF_Attribute)&       /*theAttrInto*/,
                         const Handle(TDF_RelocationTable)& /*theRT*/) const
{
}
