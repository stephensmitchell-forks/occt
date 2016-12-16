// Created on: 1993-01-09
// Created by: CKY / Contract Toubro-Larsen ( TCD )
// Copyright (c) 1993-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#ifndef _IGESDraw_Drawing_HeaderFile
#define _IGESDraw_Drawing_HeaderFile

#include <IGESDraw_HArray1OfViewKindEntity.hxx>
#include <TColgp_HArray1OfXY.hxx>
#include <IGESData_HArray1OfIGESEntity.hxx>
#include <IGESData_IGESEntity.hxx>
class IGESData_ViewKindEntity;
class gp_Pnt2d;
class gp_XY;
class gp_XYZ;


//! defines IGESDrawing, Type <404> Form <0>
//! in package IGESDraw
//!
//! Specifies a drawing as a collection of annotation entities
//! defined in drawing space, and views which together
//! constitute a single representation of a part
class IGESDraw_Drawing : public IGESData_IGESEntity
{
 public:

  Standard_EXPORT virtual Standard_Integer TypeNumber() const Standard_OVERRIDE { return 404; }

  Standard_EXPORT virtual Standard_Integer FormNumber() const Standard_OVERRIDE { return 0; }

  IGESDraw_Drawing() {}

  //! returns the number of view pointers in <me>
  Standard_EXPORT Standard_Integer NbViews() const;
  
  //! returns the ViewKindEntity indicated by ViewIndex
  //! raises an exception if ViewIndex <= 0 or ViewIndex > NbViews().
  Standard_EXPORT const Handle(IGESData_ViewKindEntity) & ViewItem (const Standard_Integer ViewIndex) const;
  
  //! returns the Drawing space coordinates of the origin of the
  //! Transformed view indicated by TViewIndex
  //! raises an exception if TViewIndex <= 0 or TViewIndex > NbViews().
  Standard_EXPORT gp_Pnt2d ViewOrigin (const Standard_Integer TViewIndex) const;
  
  //! returns the number of Annotation entities in <me>
  Standard_EXPORT Standard_Integer NbAnnotations() const;
  
  //! returns the Annotation entity in this Drawing, indicated by the
  //! AnnotationIndex
  //! raises an exception if AnnotationIndex <= 0 or
  //! AnnotationIndex > NbAnnotations().
  Standard_EXPORT const Handle(IGESData_IGESEntity) & Annotation (const Standard_Integer AnnotationIndex) const;
  
  Standard_EXPORT gp_XY ViewToDrawing (const Standard_Integer NumView, const gp_XYZ& ViewCoords) const;
  
  //! Returns the Drawing Unit Value if it is specified (by a
  //! specific property entity)
  //! If not specified, returns False, and val as zero :
  //! unit to consider is then the model unit in GlobalSection
  Standard_EXPORT Standard_Boolean DrawingUnit (Standard_Real& value) const;
  
  //! Returns the Drawing Size if it is specified (by a
  //! specific property entity)
  //! If not specified, returns False, and X,Y as zero :
  //! unit to consider is then the model unit in GlobalSection
  Standard_EXPORT Standard_Boolean DrawingSize (Standard_Real& X, Standard_Real& Y) const;

  Standard_EXPORT virtual void OwnRead (IGESFile_Reader &) Standard_OVERRIDE;
  
  Standard_EXPORT virtual void OwnWrite (IGESData_IGESWriter &) const Standard_OVERRIDE;

  Standard_EXPORT virtual void OwnShared (Interface_EntityIterator &) const Standard_OVERRIDE;

  Standard_EXPORT virtual IGESData_DirChecker DirChecker () const Standard_OVERRIDE;

  Standard_EXPORT virtual void OwnCheck (const Interface_ShareTool &, Handle(Interface_Check) &) const Standard_OVERRIDE;

  Standard_EXPORT virtual void OwnDump (const IGESData_IGESDumper &, const Handle(Message_Messenger) &, const Standard_Integer) const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(IGESDraw_Drawing,IGESData_IGESEntity)

 private:

  Handle(IGESDraw_HArray1OfViewKindEntity) theViews;
  Handle(TColgp_HArray1OfXY) theViewOrigins;
  Handle(IGESData_HArray1OfIGESEntity) theAnnotations;
};

#endif // _IGESDraw_Drawing_HeaderFile
