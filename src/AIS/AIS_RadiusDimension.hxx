// Copyright (c) 1995-1999 Matra Datavision
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

#ifndef _AIS_RadiusDimension_HeaderFile
#define _AIS_RadiusDimension_HeaderFile

#include <AIS.hxx>
#include <AIS_Dimension.hxx>
#include <gp_Pnt.hxx>
#include <gp_Circ.hxx>
#include <Standard.hxx>
#include <Standard_Macro.hxx>

class AIS_RadiusDimension;
DEFINE_STANDARD_HANDLE (AIS_RadiusDimension,AIS_Dimension)

//! Radius dimension. Can be constructued:
//! - On generic circle.
//! - On generic circle with user-defined anchor point on that circle.
//! - On generic shape containing geometry that can be measured
//!   by diameter dimension: circle wire, arc, circular face, etc.
//! The anchor point is the location of left attachement point of
//! dimension on the circle. It can be user-specified, or computed as
//! middle point on the arc. The radius dimension always lies in the
//! plane of the measured circle. The dimension is considered as
//! invalid if the user-specified anchor point is not lying on the circle,
//! if the radius of the circle is less than Precision::Confusion().
//! In case if the dimension is built on the arbitrary shape,
//! it can be considered as invalid if the shape does not contain
//! circle geometry.
//! Use IsValid() method to check that created dimension is valid.
class AIS_RadiusDimension : public AIS_Dimension
{
protected:
  
  //! Setting of default construction parameters
  void init();

public:

  //! Create radius dimension for the circle geometry.
  //! @param theCircle [in] the circle to measure.
  //! @param theParameter [in] the value of parameter for parametric representation of the input circle
  //! that defines the point where the dimension is to be attached.
  Standard_EXPORT AIS_RadiusDimension (const gp_Circ& theCircle, const Standard_Real theParameter = 0);

  //! Create radius dimension for the circle geometry and define its
  //! orientation by location of the first point on that circle.
  //! @param theCircle [in] the circle to measure.
  //! @param theAnchorPoint [in] the point to define the position
  //!        of the dimension attachment on the circle.
  Standard_EXPORT AIS_RadiusDimension (const gp_Circ& theCircle,
                                       const gp_Pnt& theAnchorPoint);

  //! Create radius dimension for the arbitrary shape (if possible).
  //! @param theShape [in] the shape to measure.
  Standard_EXPORT AIS_RadiusDimension (const TopoDS_Shape& theShape);

public:

  //! @return measured geometry circle.
  Standard_EXPORT const gp_Circ& Circle() const;

  //! @return anchor point on circle for radius dimension.
  Standard_EXPORT const gp_Pnt& AnchorPoint() const;

  //! @return the measured shape.
  Standard_EXPORT const TopoDS_Shape& Shape() const;

public:

  //! Measure radius of the circle.
  //! The dimension will become invalid if the radius of the circle
  //! is less than Precision::Confusion().
  //! @param theCircle [in] the circle to measure.
  //! @param theParameter [in] the value of parameter for parametric representation of the input circle
  //! that defines the point where the dimension is to be attached.
  Standard_EXPORT void SetMeasuredGeometry (const gp_Circ& theCircle, const Standard_Real theParameter = 0);

  //! Measure radius of the circle and orient the dimension so
  //! the dimension lines attaches to anchor point on the circle.
  //! The dimension will become invalid if the radius of the circle
  //! is less than Precision::Confusion().
  //! @param theCircle [in] the circle to measure.
  //! @param theAnchorPoint [in] the point to attach the dimension lines.
  Standard_EXPORT void SetMeasuredGeometry (const gp_Circ& theCircle,
                                            const gp_Pnt& theAnchorPoint);

  //! Measure radius on the passed shape, if applicable.
  //! The dimension will become invalid if the passed shape is not
  //! measurable or if measured diameter value is less than Precision::Confusion().
  //! @param theShape [in] the shape to measure.
  Standard_EXPORT void SetMeasuredGeometry (const TopoDS_Shape& theShape);

  //! @return the display units string.
  Standard_EXPORT virtual const TCollection_AsciiString& GetDisplayUnits() const;
  
  //! @return the model units string.
  Standard_EXPORT virtual const TCollection_AsciiString& GetModelUnits() const;

  Standard_EXPORT virtual void SetDisplayUnits (const TCollection_AsciiString& theUnits);

  Standard_EXPORT virtual void SetModelUnits (const TCollection_AsciiString& theUnits);

  Standard_EXPORT virtual void SetTextPosition (const gp_Pnt& theTextPos);

  Standard_EXPORT virtual const gp_Pnt GetTextPosition() const;

public:

  DEFINE_STANDARD_RTTI (AIS_RadiusDimension, AIS_Dimension)

protected:

  Standard_EXPORT virtual void ComputePlane() Standard_OVERRIDE;

  //! Checks if anchor point and the center of the circle are on the plane.
  Standard_EXPORT virtual Standard_Boolean CheckPlane (const gp_Pln& thePlane) const Standard_OVERRIDE;

  Standard_EXPORT virtual Standard_Real ComputeValue() const Standard_OVERRIDE;

  Standard_EXPORT virtual void Compute (const Handle(PrsMgr_PresentationManager3d)& thePresentationManager,
                                        const Handle(Prs3d_Presentation)& thePresentation,
                                        const Standard_Integer theMode = 0) Standard_OVERRIDE;

protected:

  Standard_EXPORT Standard_Boolean IsValidCircle (const gp_Circ& theCircle) const;

  Standard_EXPORT Standard_Boolean IsValidAnchor (const gp_Circ& theCircle,
                                                  const gp_Pnt& thePnt) const;

private:

  gp_Circ      myCircle;
  gp_Pnt       myAnchorPoint;
  TopoDS_Shape myShape;
};

#endif // _AIS_RadiusDimension_HeaderFile
