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
#include <TDF_ChildIterator.hxx>
#include <TDF_LabelSequence.hxx>
#include <XCAFDoc.hxx>
#include <XCAFDoc_GraphNode.hxx>
#include <XCAFDoc_NotesTool.hxx>
#include <XCAFDoc_Note.hxx>

IMPLEMENT_STANDARD_RTTIEXT(XCAFDoc_NotesTool, TDF_Attribute)

const Standard_GUID& XCAFDoc_NotesTool::GetID()
{
  static Standard_GUID s_ID("8F8174B1-6125-47a0-B357-61BD2D89380C");
  return s_ID;
}

Handle(XCAFDoc_NotesTool) XCAFDoc_NotesTool::Set(const TDF_Label& theLabel)
{
  Handle(XCAFDoc_NotesTool) aTool;
  if (!theLabel.FindAttribute(XCAFDoc_NotesTool::GetID(), aTool)) 
  {
    aTool = new XCAFDoc_NotesTool();
    theLabel.AddAttribute(aTool);
  }
  return aTool;
}

XCAFDoc_NotesTool::XCAFDoc_NotesTool()
{
}

void XCAFDoc_NotesTool::GetNotes(TDF_LabelSequence& theNoteLabels) const
{
  theNoteLabels.Clear();
  for (TDF_ChildIterator anIter(Label()); anIter.More(); anIter.Next()) 
  {
    const TDF_Label aLabel = anIter.Value();
    if (XCAFDoc_Note::IsMine(aLabel))
    {
      theNoteLabels.Append(aLabel);
    }
  }
}

Handle(XCAFDoc_Note) XCAFDoc_NotesTool::AddNote(const Handle(TCollection_HExtendedString)& theUserName,
                                                const Handle(TCollection_HExtendedString)& theTimeStamp)
{
  TDF_Label aNoteLabel;
  TDF_TagSource aTag;
  aNoteLabel = aTag.NewChild(Label());
  return XCAFDoc_Note::Set(aNoteLabel, theUserName, theTimeStamp);
}

const Standard_GUID& XCAFDoc_NotesTool::ID() const
{
  return GetID();
}

Handle(TDF_Attribute) XCAFDoc_NotesTool::NewEmpty() const
{
  return new XCAFDoc_NotesTool();
}

void XCAFDoc_NotesTool::Restore(const Handle(TDF_Attribute)& /*theAttr*/)
{
}

void XCAFDoc_NotesTool::Paste(const Handle(TDF_Attribute)&       /*theAttrInto*/,
                              const Handle(TDF_RelocationTable)& /*theRT*/) const
{
}
