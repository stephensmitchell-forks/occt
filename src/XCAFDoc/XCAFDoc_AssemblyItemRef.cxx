// Created on: 2017-02-16
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
#include <TDF_RelocationTable.hxx>
#include <XCAFDoc_AssemblyItemRef.hxx>

IMPLEMENT_STANDARD_RTTIEXT(XCAFDoc_AssemblyItemRef, TDF_Attribute)

const Standard_GUID& 
XCAFDoc_AssemblyItemRef::GetID()
{
  static Standard_GUID s_ID("3F2E4CD6-169B-4747-A321-5670E4291F5D");
  return s_ID;
}

Handle(XCAFDoc_AssemblyItemRef) 
XCAFDoc_AssemblyItemRef::Get(const TDF_Label& theLabel)
{
  Handle(XCAFDoc_AssemblyItemRef) aThis;
  theLabel.FindAttribute(XCAFDoc_AssemblyItemRef::GetID(), aThis);
  return aThis;
}

Handle(XCAFDoc_AssemblyItemRef) 
XCAFDoc_AssemblyItemRef::Set(const TDF_Label&              theLabel,
                             const XCAFDoc_AssemblyItemId& theItemId)
{
  Handle(XCAFDoc_AssemblyItemRef) aThis;
  if (!theLabel.IsNull() && !theLabel.FindAttribute(XCAFDoc_AssemblyItemRef::GetID(), aThis))
  {
    aThis = new XCAFDoc_AssemblyItemRef();
    aThis->Set(theItemId);
    theLabel.AddAttribute(aThis);
  }
  return aThis;
}

void 
XCAFDoc_AssemblyItemRef::Set(const TColStd_ListOfAsciiString& thePath)
{ 
  myItemId.Init(thePath);
}

void 
XCAFDoc_AssemblyItemRef::Set(const TCollection_AsciiString& theString)
{
  myItemId.Init(theString);
}

XCAFDoc_AssemblyItemRef::XCAFDoc_AssemblyItemRef()
{

}

Standard_Boolean 
XCAFDoc_AssemblyItemRef::IsOrphan() const
{
  // TODO...
  return Standard_False;
}

const XCAFDoc_AssemblyItemId& 
XCAFDoc_AssemblyItemRef::Get() const
{
  return myItemId;
}

void 
XCAFDoc_AssemblyItemRef::Set(const XCAFDoc_AssemblyItemId& theItemId)
{
  Backup();
  myItemId = theItemId;
}

const Standard_GUID& 
XCAFDoc_AssemblyItemRef::ID() const
{
  return GetID();
}

Handle(TDF_Attribute) 
XCAFDoc_AssemblyItemRef::NewEmpty() const
{
  return new XCAFDoc_AssemblyItemRef();
}

void 
XCAFDoc_AssemblyItemRef::Restore(const Handle(TDF_Attribute)& theAttrFrom)
{
  Handle(XCAFDoc_AssemblyItemRef) aThis = Handle(XCAFDoc_AssemblyItemRef)::DownCast(theAttrFrom);
  if (!aThis.IsNull())
    myItemId = aThis->myItemId;
}

void 
XCAFDoc_AssemblyItemRef::Paste(const Handle(TDF_Attribute)&       theAttrInto,
                               const Handle(TDF_RelocationTable)& /*theRT*/) const
{
  Handle(XCAFDoc_AssemblyItemRef) aThis = Handle(XCAFDoc_AssemblyItemRef)::DownCast(theAttrInto);
  if (!aThis.IsNull())
    aThis->myItemId = myItemId;
}

Standard_OStream& 
XCAFDoc_AssemblyItemRef::Dump(Standard_OStream& theOS) const
{
  theOS << myItemId.ToString();
  return theOS;
}
