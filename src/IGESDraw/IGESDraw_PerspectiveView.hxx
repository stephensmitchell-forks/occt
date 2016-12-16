// Created on: 1993-02-03
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

#ifndef _IGESDraw_PerspectiveView_HeaderFile
#define _IGESDraw_PerspectiveView_HeaderFile

#include <gp_XYZ.hxx>
#include <gp_XY.hxx>
#include <IGESData_ViewKindEntity.hxx>
class gp_Vec;
class gp_Pnt;
class gp_Pnt2d;
class IGESData_TransfEntity;


//! defines IGESPerspectiveView, Type <410> Form <1>
//! in package IGESDraw
//!
//! Supports a perspective view.
//! Any geometric projection is defined by a view plane
//! and the projectors that pass through the view plane.
//! Projectors can be visualized as rays of light that
//! form an image by passing through the viewed object
//! and striking the view plane.
//! The projectors are defined via a point called the
//! Centre-of-Projection or the eye-point.
//! A perspective view is formed by all projectors that
//! emanate from the Centre-of-Projection and pass
//! through the view plane.
class IGESDraw_PerspectiveView : public IGESData_ViewKindEntity
{
 public:

  Standard_EXPORT virtual Standard_Integer TypeNumber() const Standard_OVERRIDE { return 410; }

  Standard_EXPORT virtual Standard_Integer FormNumber() const Standard_OVERRIDE { return 1; }

  IGESDraw_PerspectiveView() {}

  //! Returns True (for a single view)
  Standard_EXPORT virtual Standard_Boolean IsSingle() const Standard_OVERRIDE;
  
  //! Returns 1 (single view)
  Standard_EXPORT virtual Standard_Integer NbViews() const Standard_OVERRIDE;
  
  //! For a single view, returns <me> whatever <num>
  Standard_EXPORT virtual Handle(IGESData_ViewKindEntity) ViewItem (const Standard_Integer num) const Standard_OVERRIDE;
  
  //! returns the view number associated with <me>
  Standard_Integer ViewNumber() const { return theViewNumber; }

  //! returns the scale factor associated with <me>
  Standard_Real ScaleFactor() const { return theScaleFactor; }

  //! returns the View plane normal vector (model space)
  Standard_EXPORT gp_Vec ViewNormalVector() const;
  
  //! returns the View reference point (model space)
  Standard_EXPORT gp_Pnt ViewReferencePoint() const;
  
  //! returns the Center Of Projection (model space)
  Standard_EXPORT gp_Pnt CenterOfProjection() const;
  
  //! returns the View up vector (model space)
  Standard_EXPORT gp_Vec ViewUpVector() const;
  
  //! returns the View plane distance (model space)
  Standard_Real ViewPlaneDistance() const { return theViewPlaneDistance; }

  //! returns the top left point of the clipping window
  Standard_EXPORT gp_Pnt2d TopLeft() const;
  
  //! returns the bottom right point of the clipping window
  Standard_EXPORT gp_Pnt2d BottomRight() const;
  
  //! returns the Depth clipping indicator
  //! 0 = No depth clipping
  //! 1 = Back clipping plane ON
  //! 2 = Front clipping plane ON
  //! 3 = Back and front clipping planes ON
  Standard_Integer DepthClip() const { return theDepthClip; }

  //! returns the View coordinate denoting the location of
  //! the back clipping plane
  Standard_Real BackPlaneDistance() const { return theBackPlaneDistance; }

  //! returns the View coordinate denoting the location of
  //! the front clipping plane
  Standard_Real FrontPlaneDistance() const { return theFrontPlaneDistance; }

  //! returns the Transformation Matrix
  Handle(IGESData_TransfEntity) ViewMatrix() const { return Transf(); }

  //! returns XYX from the Model space to the View space by
  //! applying the View Matrix
  Standard_EXPORT gp_XYZ ModelToView (const gp_XYZ& coords) const;

  Standard_EXPORT virtual void OwnRead (IGESFile_Reader &) Standard_OVERRIDE;
  
  Standard_EXPORT virtual void OwnWrite (IGESData_IGESWriter &) const Standard_OVERRIDE;

  Standard_EXPORT virtual IGESData_DirChecker DirChecker () const Standard_OVERRIDE;

  Standard_EXPORT virtual void OwnCheck (const Interface_ShareTool &, Handle(Interface_Check) &) const Standard_OVERRIDE;

  Standard_EXPORT virtual void OwnDump (const IGESData_IGESDumper &, const Handle(Message_Messenger) &, const Standard_Integer) const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(IGESDraw_PerspectiveView,IGESData_ViewKindEntity)

 private:

  Standard_Integer theViewNumber;
  Standard_Real theScaleFactor;
  gp_XYZ theViewNormalVector;
  gp_XYZ theViewReferencePoint;
  gp_XYZ theCenterOfProjection;
  gp_XYZ theViewUpVector;
  Standard_Real theViewPlaneDistance;
  gp_XY theTopLeft;
  gp_XY theBottomRight;
  Standard_Integer theDepthClip;
  Standard_Real theBackPlaneDistance;
  Standard_Real theFrontPlaneDistance;
};

#endif // _IGESDraw_PerspectiveView_HeaderFile
