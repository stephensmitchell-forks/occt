// Copyright (c) 1999-2018 OPEN CASCADE SAS
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

#include <XCAFNoteObjects_NoteObject.hxx>

IMPLEMENT_STANDARD_RTTIEXT(XCAFNoteObjects_NoteObject, Standard_Transient)

//=======================================================================
//function : XCAFDimTolObjects_DimensionObject
//purpose  : 
//=======================================================================

XCAFNoteObjects_NoteObject::XCAFNoteObjects_NoteObject()
{
  myHasPlane = Standard_False;
  myHasPnt = Standard_False;
  myHasPntTxt = Standard_False;
}

//=======================================================================
//function : 
//purpose  : 
//=======================================================================

XCAFNoteObjects_NoteObject::XCAFNoteObjects_NoteObject(const Handle(XCAFNoteObjects_NoteObject)& theObj)
{
  myHasPnt = theObj->myHasPnt;
  myPnt = theObj->myPnt;
  myHasPntTxt = theObj->myHasPntTxt;
  myPntTxt = theObj->myPntTxt;
  myHasPlane = theObj->myHasPlane;
  myPlane = theObj->myPlane;
  myPresentation = theObj->myPresentation;
}

Standard_Boolean XCAFNoteObjects_NoteObject::HasPlane() const
{
  return myHasPlane;
}

const gp_Ax2& XCAFNoteObjects_NoteObject::GetPlane() const
{
  return myPlane;
}

void XCAFNoteObjects_NoteObject::SetPlane(const gp_Ax2& thePlane)
{
  myPlane = thePlane;
  myHasPlane = Standard_True;
}

Standard_Boolean XCAFNoteObjects_NoteObject::HasPoint() const
{
  return myHasPnt;
}

const gp_Pnt& XCAFNoteObjects_NoteObject::GetPoint() const
{
  return myPnt;
}

void XCAFNoteObjects_NoteObject::SetPoint(const gp_Pnt thePnt)
{
  myPnt = thePnt;
  myHasPnt = Standard_True;
}

Standard_Boolean XCAFNoteObjects_NoteObject::HasPointText() const
{
  return myHasPntTxt;
}

const gp_Pnt& XCAFNoteObjects_NoteObject::GetPointText() const
{
  return myPntTxt;
}

void XCAFNoteObjects_NoteObject::SetPointText(const gp_Pnt thePnt)
{
  myPntTxt = thePnt;
  myHasPntTxt = Standard_True;
}

const TopoDS_Shape& XCAFNoteObjects_NoteObject::GetPresentation() const
{
  return myPresentation;
}

void XCAFNoteObjects_NoteObject::SetPresentation(const TopoDS_Shape& thePresentation)
{
  myPresentation = thePresentation;
}

void XCAFNoteObjects_NoteObject::Reset()
{
  myHasPlane = Standard_False;
  myHasPnt = Standard_False;
  myHasPntTxt = Standard_False;
  myPresentation.Nullify();
}
