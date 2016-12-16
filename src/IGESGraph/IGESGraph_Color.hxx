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

#ifndef _IGESGraph_Color_HeaderFile
#define _IGESGraph_Color_HeaderFile

#include <IGESData_ColorEntity.hxx>
class TCollection_HAsciiString;


//! defines IGESColor, Type <314> Form <0>
//! in package IGESGraph
//!
//! The Color Definition Entity is used to communicate the
//! relationship of primary colors to the intensity level of
//! the respective graphics devices as a percent of full
//! intensity range.
class IGESGraph_Color : public IGESData_ColorEntity
{
 public:

  Standard_EXPORT virtual Standard_Integer TypeNumber() const Standard_OVERRIDE { return 314; }

  Standard_EXPORT virtual Standard_Integer FormNumber() const Standard_OVERRIDE { return 0; }

  //! Default constructor
  IGESGraph_Color()
  : myRed(0.),
    myGreen(0.),
    myBlue(0.)
  {}
  
  //! Constructor with parameters
  //! - red        : Red   color intensity (range 0.0 to 100.0)
  //! - green      : Green color intensity (range 0.0 to 100.0)
  //! - blue       : Blue  color intensity (range 0.0 to 100.0)
  //! - aColorName : Name of the color (optional)
  IGESGraph_Color(const Standard_Real R, const Standard_Real G, const Standard_Real B, const Handle(TCollection_HAsciiString) &theColorName)
  : myRed(R),
    myGreen(G),
    myBlue(B),
    myColorName(theColorName)
  {}
  
  void RGBIntensity (Standard_Real& Red, Standard_Real& Green, Standard_Real& Blue) const
  {
    Red   = myRed;
    Green = myGreen;
    Blue  = myBlue;
  }

  void CMYIntensity (Standard_Real& Cyan, Standard_Real& Magenta, Standard_Real& Yellow) const
  {
    Cyan    = 100.0 - myRed;
    Magenta = 100.0 - myGreen;
    Yellow  = 100.0 - myBlue;
  }
  
  Standard_EXPORT void HLSPercentage (Standard_Real& Hue, Standard_Real& Lightness, Standard_Real& Saturation) const;
  
  //! returns True if optional character string is assigned,
  //! False otherwise.
  Standard_Boolean HasColorName() const { return (!myColorName.IsNull()); }

  //! if HasColorName() is True  returns the Verbal description of
  //! the Color.
  const Handle(TCollection_HAsciiString) & ColorName() const { return myColorName; }

  Standard_EXPORT virtual void OwnRead (IGESFile_Reader &) Standard_OVERRIDE;
  
  Standard_EXPORT virtual void OwnWrite (IGESData_IGESWriter &) const Standard_OVERRIDE;

  Standard_EXPORT virtual IGESData_DirChecker DirChecker () const Standard_OVERRIDE;

  Standard_EXPORT virtual void OwnDump (const IGESData_IGESDumper &, const Handle(Message_Messenger) &, const Standard_Integer) const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(IGESGraph_Color,IGESData_ColorEntity)

 private:

  Standard_Real myRed;
  Standard_Real myGreen;
  Standard_Real myBlue;
  Handle(TCollection_HAsciiString) myColorName;
};

#endif // _IGESGraph_Color_HeaderFile
