// Copyright (c) 2015 OPEN CASCADE SAS
//
// This file is part of commercial software by OPEN CASCADE SAS.
//
// This software is furnished in accordance with the terms and conditions
// of the contract and with the inclusion of this copyright notice.
// This software or any other copy thereof may not be provided or otherwise
// be made available to any third party.
// No ownership title to the software is transferred hereby.
//
// OPEN CASCADE SAS makes no representation or warranties with respect to the
// performance of this software, and specifically disclaims any responsibility
// for any damages, special or consequential, connected with its use.

#ifndef CafShapePrs_Header
#define CafShapePrs_Header

#include <XCAFPrs_AISObject.hxx>
#include <XCAFPrs_Style.hxx>

typedef NCollection_DataMap<TopoDS_Shape, Handle(AIS_ColoredDrawer), TopTools_ShapeMapHasher> CafDataMapOfShapeColor;

//! Interactive object for shape in DECAF document.
class CafShapePrs : public XCAFPrs_AISObject
{

public:

  //! Default constructor.
  Standard_EXPORT CafShapePrs (const TDF_Label&                theLabel,
                               const XCAFPrs_Style&            theStyle,
                               const Graphic3d_MaterialAspect& theMaterial);

  //! Search custom aspect for specified shape.
  Standard_Boolean FindCustomAspects (const TopoDS_Shape&        theShape,
                                      Handle(AIS_ColoredDrawer)& theAspects) const
  {
    return myShapeColors.Find (theShape, theAspects);
  }

  //! Access the styles map.
  const CafDataMapOfShapeColor& ShapeColors() const { return myShapeColors; }

  //! Override default style.
  virtual void DefaultStyle (XCAFPrs_Style& theStyle) const Standard_OVERRIDE
  {
    theStyle = myDefStyle;
  }

protected:

  XCAFPrs_Style myDefStyle; //!< default style

public: //! @name Declaration of CASCADE RTTI

  DEFINE_STANDARD_RTTIEXT(CafShapePrs, XCAFPrs_AISObject)

};

DEFINE_STANDARD_HANDLE (CafShapePrs, XCAFPrs_AISObject)

#endif
