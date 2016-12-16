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

#ifndef _IGESGraph_LineFontDefPattern_HeaderFile
#define _IGESGraph_LineFontDefPattern_HeaderFile

#include <IGESData_LineFontEntity.hxx>
class TCollection_HAsciiString;
class TColStd_HArray1OfReal;


//! defines IGESLineFontDefPattern, Type <304> Form <2>
//! in package IGESGraph
//!
//! Line Font may be defined by repetition of a basic pattern
//! of visible-blank(or, on-off) segments superimposed on
//! a line or a curve. The line or curve is then displayed
//! according to the basic pattern.
class IGESGraph_LineFontDefPattern : public IGESData_LineFontEntity
{
 public:

  Standard_EXPORT virtual Standard_Integer TypeNumber() const Standard_OVERRIDE { return 304; }

  Standard_EXPORT virtual Standard_Integer FormNumber() const Standard_OVERRIDE { return 2; }

  IGESGraph_LineFontDefPattern() {}
  
  //! returns the number of segments in the visible-blank pattern
  Standard_EXPORT Standard_Integer NbSegments() const;
  
  //! returns the Length of Index'th segment of the basic pattern
  //! raises exception if Index <= 0 or Index > NbSegments
  Standard_EXPORT Standard_Real Length (const Standard_Integer Index) const;
  
  //! returns the string indicating which segments of the basic
  //! pattern are visible and which are blanked.
  //! e.g:
  //! theNbSegments = 5 and if Bit Pattern = 10110, which means that
  //! segments 2, 3 and 5 are visible, whereas segments 1 and 4 are
  //! blank. The method returns "2H16" as the HAsciiString.
  //! Note: The bits are right justified. (16h = 10110)
  const Handle(TCollection_HAsciiString) & DisplayPattern() const { return myDisplayPattern; }

  //! The Display Pattern is decrypted to
  //! return True if the Index'th basic pattern is Visible,
  //! False otherwise.
  //! If Index > NbSegments or Index <= 0 then return value is
  //! False.
  Standard_EXPORT Standard_Boolean IsVisible (const Standard_Integer Index) const;

  Standard_EXPORT virtual void OwnRead (IGESFile_Reader &) Standard_OVERRIDE;
  
  Standard_EXPORT virtual void OwnWrite (IGESData_IGESWriter &) const Standard_OVERRIDE;

  Standard_EXPORT virtual IGESData_DirChecker DirChecker () const Standard_OVERRIDE;

  Standard_EXPORT virtual void OwnCheck (const Interface_ShareTool &, Handle(Interface_Check) &) const Standard_OVERRIDE;

  Standard_EXPORT virtual void OwnDump (const IGESData_IGESDumper &, const Handle(Message_Messenger) &, const Standard_Integer) const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(IGESGraph_LineFontDefPattern,IGESData_LineFontEntity)

 private:

  Handle(TColStd_HArray1OfReal) mySegmentLengths;
  Handle(TCollection_HAsciiString) myDisplayPattern;
};

#endif // _IGESGraph_LineFontDefPattern_HeaderFile
