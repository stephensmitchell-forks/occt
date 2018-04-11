
// Created on: 2018-04-11
// Created by: Sergey Nikonov
// Copyright (c) 2004-2018 OPEN CASCADE SAS
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


#ifndef _XCAFNoteObjects_NoteObject_HeaderFile
#define _XCAFNoteObjects_NoteObject_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <gp_Ax2.hxx>
#include <gp_Pnt.hxx>
#include <Standard_Transient.hxx>
#include <TopoDS_Shape.hxx>

class XCAFNoteObjects_NoteObject;
DEFINE_STANDARD_HANDLE(XCAFNoteObjects_NoteObject, Standard_Transient)
                            
//! object to store note auxiliary data
class XCAFNoteObjects_NoteObject : public Standard_Transient
{

public:
  
  //! Empty object
  Standard_EXPORT XCAFNoteObjects_NoteObject();
  
  Standard_EXPORT XCAFNoteObjects_NoteObject(const Handle(XCAFNoteObjects_NoteObject)& theObj);

  //! Returns True if plane is specified
  Standard_EXPORT Standard_Boolean HasPlane() const;

  //! Returns a right-handed coordinate system of the plane
  Standard_EXPORT const gp_Ax2& GetPlane() const;

  //! Sets a right-handed coordinate system of the plane
  Standard_EXPORT void SetPlane(const gp_Ax2& thePlane);

  //! Returns True if the attachment point on the annotated object is specified
  Standard_EXPORT Standard_Boolean HasPoint() const;

  //! Returns the attachment point on the annotated object
  Standard_EXPORT const gp_Pnt& GetPoint() const;

  //! Sets the anchor point on the annotated object
  Standard_EXPORT void SetPoint(const gp_Pnt thePnt);

  //! Returns True if the text position is specified
  Standard_EXPORT Standard_Boolean HasPointText() const;

  //! Returns the text position
  Standard_EXPORT const gp_Pnt& GetPointText() const;

  //! Sets the text position
  Standard_EXPORT void SetPointText(const gp_Pnt thePnt);

  //! Returns a tesselated annotation if specified
  Standard_EXPORT const TopoDS_Shape& GetPresentation() const;

  //! Sets a tesselated annotation
  Standard_EXPORT void SetPresentation(const TopoDS_Shape& thePresentation);

  //! Resets data to the state after calling the default constructor
  Standard_EXPORT void Reset();

  DEFINE_STANDARD_RTTIEXT(XCAFNoteObjects_NoteObject, Standard_Transient)

private: 

  gp_Ax2 myPlane;
  gp_Pnt myPnt, myPntTxt;
  Standard_Boolean myHasPlane;
  Standard_Boolean myHasPnt;
  Standard_Boolean myHasPntTxt;
  TopoDS_Shape myPresentation;

};

#endif // _XCAFNoteObjects_NoteObject_HeaderFile
