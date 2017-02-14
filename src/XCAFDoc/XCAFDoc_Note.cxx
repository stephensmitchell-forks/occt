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
#include <XCAFDoc.hxx>
#include <XCAFDoc_GraphNode.hxx>
#include <XCAFDoc_NoteComment.hxx>
#include <XCAFDoc_NoteBinData.hxx>

IMPLEMENT_STANDARD_RTTIEXT(XCAFDoc_Note, TDF_Attribute)

Standard_Boolean XCAFDoc_Note::IsMine(const TDF_Label& theLabel)
{
  return !Get(theLabel).IsNull();
}

XCAFDoc_Note::XCAFDoc_Note()
{
}

Handle(XCAFDoc_Note) XCAFDoc_Note::Get(const TDF_Label& theLabel)
{
  Handle(XCAFDoc_Note) aNote;
  if (theLabel.FindAttribute(XCAFDoc_NoteComment::GetID(), aNote) ||
      theLabel.FindAttribute(XCAFDoc_NoteBinData::GetID(), aNote))
    return aNote;
  return aNote;
}

void XCAFDoc_Note::Set(const TCollection_ExtendedString& theUserName,
                       const TCollection_ExtendedString& theTimeStamp)
{
  Backup();

  myUserName = theUserName;
  myTimeStamp = theTimeStamp;
}

const TCollection_ExtendedString& XCAFDoc_Note::UserName() const
{
  return myUserName;
}

const TCollection_ExtendedString& XCAFDoc_Note::TimeStamp() const
{
  return myTimeStamp;
}

Standard_Boolean XCAFDoc_Note::IsAttached() const
{
  if (!IsMine(Label()))
    return Standard_False;

  Handle(XCAFDoc_GraphNode) aFather;
  return Label().FindAttribute(XCAFDoc::NoteRefGUID(), aFather) &&
         (aFather->NbChildren() > 0);
}

void XCAFDoc_Note::Attach(const TDF_LabelSequence& theLabels)
{
  if (!IsMine(Label()) || theLabels.Length() == 0)
    return;

  Handle(XCAFDoc_GraphNode) aFather = XCAFDoc_GraphNode::Set(Label(), XCAFDoc::NoteRefGUID());

  for (Standard_Integer i = theLabels.Lower(); i <= theLabels.Upper(); i++)
  {
    Handle(XCAFDoc_GraphNode) aChild = XCAFDoc_GraphNode::Set(theLabels.Value(i), XCAFDoc::NoteRefGUID());
    aChild->SetFather(aFather);
    aFather->SetChild(aChild);
  }
}

void XCAFDoc_Note::Detach(const TDF_LabelSequence& theLabels)
{
  if (!IsMine(Label()) || theLabels.Length() == 0)
    return;

  Handle(XCAFDoc_GraphNode) aFather;
  if (Label().FindAttribute(XCAFDoc::NoteRefGUID(), aFather))
  {
    for (Standard_Integer i = theLabels.Lower(); i <= theLabels.Upper(); i++)
    {
      Handle(XCAFDoc_GraphNode) aChild;
      if (theLabels.Value(i).FindAttribute(XCAFDoc::NoteRefGUID(), aChild))
      {
        Standard_Integer iFather = aChild->FatherIndex(aFather);
        if (iFather > 0)
          aChild->UnSetFather(iFather);
        if (aChild->NbFathers() == 0)
          theLabels.Value(i).ForgetAttribute(aChild);
      }
    }
    if (aFather->NbChildren() == 0)
      Label().ForgetAttribute(aFather);
  }
}

void XCAFDoc_Note::DetachAll()
{
  if (!IsMine(Label()))
    return;

  Handle(XCAFDoc_GraphNode) aFather;
  if (Label().FindAttribute(XCAFDoc::NoteRefGUID(), aFather))
  {
    Standard_Integer nbChildren = aFather->NbChildren();
    for (Standard_Integer iChild = 1; iChild <= nbChildren; ++iChild)
    {
      Handle(XCAFDoc_GraphNode) aChild = aFather->GetChild(iChild);
      aFather->UnSetChild(iChild);
      if (aChild->NbFathers() == 0)
        aChild->Label().ForgetAttribute(aChild);
    }
    if (aFather->NbChildren() == 0)
      Label().ForgetAttribute(aFather);
  }
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
    << (myUserName.IsEmpty() ? myUserName : "<anonymous>")
    << " on "
    << (myTimeStamp.IsEmpty() ? myTimeStamp : "<unknown>")
    ;
  return theOS;
}
