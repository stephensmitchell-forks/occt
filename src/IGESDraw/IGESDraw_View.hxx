// Created on: 1993-01-11
// Created by: CKY / Contract Toubro-Larsen ( Niraj RANGWALA )
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

#ifndef _IGESDraw_View_HeaderFile
#define _IGESDraw_View_HeaderFile

#include <IGESData_ViewKindEntity.hxx>
class IGESGeom_Plane;
class IGESData_ViewKindEntity;
class IGESData_TransfEntity;
class gp_XYZ;


//! defines IGES View Entity, Type <410> Form <0>
//! in package IGESDraw
//!
//! Used to define a framework for specifying a viewing
//! orientation of an object in three dimensional model
//! space (X,Y,Z). The framework is also used to support
//! the projection of all or part of model space onto a
//! view volume.
class IGESDraw_View : public IGESData_ViewKindEntity
{
 public:

  Standard_EXPORT virtual Standard_Integer TypeNumber() const Standard_OVERRIDE { return 410; }

  Standard_EXPORT virtual Standard_Integer FormNumber() const Standard_OVERRIDE { return 0; }

  IGESDraw_View() {}
  
  //! Returns True (for a single view)
  Standard_EXPORT virtual Standard_Boolean IsSingle() const Standard_OVERRIDE;
  
  //! Returns 1 (single view)
  Standard_EXPORT virtual Standard_Integer NbViews() const Standard_OVERRIDE;
  
  //! For a single view, returns <me> whatever <num>
  Standard_EXPORT virtual Handle(IGESData_ViewKindEntity) ViewItem (const Standard_Integer num) const Standard_OVERRIDE;
  
  //! returns integer number identifying view orientation
  Standard_Integer ViewNumber() const { return theViewNumber; }

  //! returns the scale factor(Default = 1.0)
  Standard_Real ScaleFactor() const { return theScaleFactor; }

  //! returns False if left side of view volume is not present
  Standard_Boolean HasLeftPlane() const { return  (! theLeftPlane.IsNull()); }

  //! returns the left side of view volume, or null handle
  const Handle(IGESGeom_Plane) & LeftPlane() const { return theLeftPlane; }

  //! returns False if top of view volume is not present
  Standard_Boolean HasTopPlane() const { return  (! theTopPlane.IsNull()); }

  //! returns the top of view volume, or null handle
  const Handle(IGESGeom_Plane) & TopPlane() const { return theTopPlane; }

  //! returns False if right side of view volume is not present
  Standard_Boolean HasRightPlane() const { return  (! theRightPlane.IsNull()); }

  //! returns the right side of view volume, or null handle
  const Handle(IGESGeom_Plane) & RightPlane() const { return theRightPlane; }

  //! returns False if bottom of view volume is not present
  Standard_Boolean HasBottomPlane() const { return  (! theBottomPlane.IsNull()); }

  //! returns the bottom of view volume, or null handle
  const Handle(IGESGeom_Plane) & BottomPlane() const { return  theBottomPlane; }

  //! returns False if back of view volume is not present
  Standard_Boolean HasBackPlane() const { return  (! theBackPlane.IsNull()); }

  //! returns the back of view volume, or null handle
  const Handle(IGESGeom_Plane) & BackPlane() const { return  theBackPlane; }

  //! returns False if front of view volume is not present
  Standard_Boolean HasFrontPlane() const { return  (! theFrontPlane.IsNull()); }

  //! returns the front of view volume, or null handle
  const Handle(IGESGeom_Plane) & FrontPlane() const { return theFrontPlane; }

  //! returns the Transformation Matrix
  Handle(IGESData_TransfEntity) ViewMatrix() const { return (Transf()); }
  
  //! returns XYZ from the Model space to the View space by
  //! applying the View Matrix
  Standard_EXPORT gp_XYZ ModelToView (const gp_XYZ& coords) const;

  Standard_EXPORT virtual void OwnRead (IGESFile_Reader &) Standard_OVERRIDE;
  
  Standard_EXPORT virtual void OwnWrite (IGESData_IGESWriter &) const Standard_OVERRIDE;

  Standard_EXPORT virtual void OwnShared (Interface_EntityIterator &) const Standard_OVERRIDE;

  Standard_EXPORT virtual IGESData_DirChecker DirChecker () const Standard_OVERRIDE;

  Standard_EXPORT virtual void OwnDump (const IGESData_IGESDumper &, const Handle(Message_Messenger) &, const Standard_Integer) const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(IGESDraw_View,IGESData_ViewKindEntity)

 private:

  Standard_Integer theViewNumber;
  Standard_Real theScaleFactor;
  Interface_Pointer<IGESGeom_Plane> theLeftPlane;
  Interface_Pointer<IGESGeom_Plane> theTopPlane;
  Interface_Pointer<IGESGeom_Plane> theRightPlane;
  Interface_Pointer<IGESGeom_Plane> theBottomPlane;
  Interface_Pointer<IGESGeom_Plane> theBackPlane;
  Interface_Pointer<IGESGeom_Plane> theFrontPlane;
};

#endif // _IGESDraw_View_HeaderFile
