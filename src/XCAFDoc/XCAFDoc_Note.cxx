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

#include <gp_Pln.hxx>
#include <Standard_GUID.hxx>
#include <TDataStd_RealArray.hxx>
#include <TDataXtd_Geometry.hxx>
#include <TDataXtd_Plane.hxx>
#include <TDF_AttributeIterator.hxx>
#include <TDF_ChildIterator.hxx>
#include <TDF_Label.hxx>
#include <TNaming_Builder.hxx>
#include <TNaming_NamedShape.hxx>
#include <TNaming_Tool.hxx>
#include <XCAFDoc.hxx>
#include <XCAFDoc_GraphNode.hxx>
#include <XCAFDoc_Note.hxx>

IMPLEMENT_STANDARD_RTTIEXT(XCAFDoc_Note, TDF_Attribute)

enum ChildLab
{
  ChildLab_PntText = 1,
  ChildLab_Plane,
  ChildLab_Pnt,
  ChildLab_Presentation
};

Standard_Boolean 
XCAFDoc_Note::IsMine(const TDF_Label& theLabel)
{
  return !Get(theLabel).IsNull();
}

XCAFDoc_Note::XCAFDoc_Note()
{
}

Handle(XCAFDoc_Note) 
XCAFDoc_Note::Get(const TDF_Label& theLabel)
{
  Handle(XCAFDoc_Note) aNote;
  for (TDF_AttributeIterator anIt(theLabel); anIt.More(); anIt.Next())
  {
    aNote = Handle(XCAFDoc_Note)::DownCast(anIt.Value());
    if (!aNote.IsNull())
      break;
  }
  return aNote;
}

void 
XCAFDoc_Note::Set(const TCollection_ExtendedString& theUserName,
                  const TCollection_ExtendedString& theTimeStamp)
{
  Backup();

  myUserName = theUserName;
  myTimeStamp = theTimeStamp;
}

const TCollection_ExtendedString& 
XCAFDoc_Note::UserName() const
{
  return myUserName;
}

const TCollection_ExtendedString& 
XCAFDoc_Note::TimeStamp() const
{
  return myTimeStamp;
}

Standard_Boolean 
XCAFDoc_Note::IsOrphan() const
{
  Handle(XCAFDoc_GraphNode) aFather;
  return !Label().FindAttribute(XCAFDoc::NoteRefGUID(), aFather) ||
         (aFather->NbChildren() == 0);
}

Handle(XCAFNoteObjects_NoteObject) XCAFDoc_Note::GetObject() const
{
  Handle(XCAFNoteObjects_NoteObject) anObj = new XCAFNoteObjects_NoteObject();

  Handle(TDataStd_RealArray) aPnt;
  if (Label().FindChild(ChildLab_Pnt).FindAttribute(TDataStd_RealArray::GetID(), aPnt) && aPnt->Length() == 3)
  {
    gp_Pnt aP(aPnt->Value(aPnt->Lower()), aPnt->Value(aPnt->Lower() + 1), aPnt->Value(aPnt->Lower() + 2));
    anObj->SetPoint(aP);
  }

  Handle(TDataXtd_Plane) aPln;
  if (Label().FindChild(ChildLab_Plane).FindAttribute(TDataXtd_Plane::GetID(), aPln))
  {
    gp_Pln aP;
    if (TDataXtd_Geometry::Plane(aPln->Label(), aP))
    {
      anObj->SetPlane(aP.Position().Ax2());
    }
  }

  Handle(TDataStd_RealArray) aPntText;
  if (Label().FindChild(ChildLab_PntText).FindAttribute(TDataStd_RealArray::GetID(), aPntText) && aPntText->Length() == 3)
  {
    gp_Pnt aP(aPntText->Value(aPntText->Lower()), aPntText->Value(aPntText->Lower() + 1), aPntText->Value(aPntText->Lower() + 2));
    anObj->SetPointText(aP);
  }

  Handle(TNaming_NamedShape) aNS;
  TDF_Label aLPres = Label().FindChild(ChildLab_Presentation);
  if (aLPres.FindAttribute(TNaming_NamedShape::GetID(), aNS))
  {
    TopoDS_Shape aPresentation = TNaming_Tool::GetShape(aNS);
    if (!aPresentation.IsNull())
    {
      anObj->SetPresentation(aPresentation);
    }
  }

  return anObj;
}

void XCAFDoc_Note::SetObject(const Handle(XCAFNoteObjects_NoteObject)& theObject)
{
  Backup();

  TDF_ChildIterator anIter(Label());
  for (; anIter.More(); anIter.Next())
  {
    anIter.Value().ForgetAllAttributes();
  }

  if (theObject->HasPoint())
  {
    gp_Pnt aPnt1 = theObject->GetPoint();

    Handle(TColStd_HArray1OfReal) aPntArr = new TColStd_HArray1OfReal(1, 3);
    for (Standard_Integer i = 1; i <= 3; i++)
      aPntArr->SetValue(i, aPnt1.Coord(i));
    Handle(TDataStd_RealArray) aPnt = TDataStd_RealArray::Set(Label().FindChild(ChildLab_Pnt), 1, 3);
    if (!aPnt.IsNull())
      aPnt->ChangeArray(aPntArr);
  }

  if (theObject->HasPlane())
  {
    gp_Ax2 anAx = theObject->GetPlane();

    gp_Pln aP(anAx);
    TDataXtd_Plane::Set(Label().FindChild(ChildLab_Plane), aP);
  }

  if (theObject->HasPointText())
  {
    gp_Pnt aPntText = theObject->GetPointText();

    Handle(TColStd_HArray1OfReal) aLocArr = new TColStd_HArray1OfReal(1, 3);
    for (Standard_Integer i = 1; i <= 3; i++)
      aLocArr->SetValue(i, aPntText.Coord(i));
    Handle(TDataStd_RealArray) aLoc = TDataStd_RealArray::Set(Label().FindChild(ChildLab_PntText), 1, 3);
    if (!aLoc.IsNull())
      aLoc->ChangeArray(aLocArr);
  }

  TopoDS_Shape aPresentation = theObject->GetPresentation();
  if (!aPresentation.IsNull())
  {
    TDF_Label aLPres = Label().FindChild(ChildLab_Presentation);
    TNaming_Builder tnBuild(aLPres);
    tnBuild.Generated(aPresentation);
  }
}

void 
XCAFDoc_Note::Restore(const Handle(TDF_Attribute)& theAttr)
{
  myUserName = Handle(XCAFDoc_Note)::DownCast(theAttr)->myUserName;
  myTimeStamp = Handle(XCAFDoc_Note)::DownCast(theAttr)->myTimeStamp;
}

void 
XCAFDoc_Note::Paste(const Handle(TDF_Attribute)&       theAttrInto,
                    const Handle(TDF_RelocationTable)& /*theRT*/) const
{
  Handle(XCAFDoc_Note)::DownCast(theAttrInto)->Set(myUserName, myTimeStamp);
}

Standard_OStream& 
XCAFDoc_Note::Dump(Standard_OStream& theOS) const
{
  TDF_Attribute::Dump(theOS);
  theOS 
    << "Note : " 
    << (myUserName.IsEmpty() ? myUserName : "<anonymous>")
    << " on "
    << (myTimeStamp.IsEmpty() ? myTimeStamp : "<unknown>")
    ;
  return theOS;
}
