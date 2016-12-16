// Created on: 1993-01-11
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

#ifndef _IGESGraph_TextDisplayTemplate_HeaderFile
#define _IGESGraph_TextDisplayTemplate_HeaderFile

#include <IGESData_IGESEntity.hxx>
#include <gp_XYZ.hxx>
class IGESGraph_TextFontDef;
class gp_Pnt;


//! defines IGES TextDisplayTemplate Entity,
//! Type <312>, form <0, 1> in package IGESGraph
//!
//! Used to set parameters for display of information
//! which has been logically included in another entity
//! as a parameter value
class IGESGraph_TextDisplayTemplate : public IGESData_IGESEntity
{
 public:

  Standard_EXPORT virtual Standard_Integer TypeNumber() const Standard_OVERRIDE { return 312; }

  Standard_EXPORT virtual Standard_Integer FormNumber() const Standard_OVERRIDE { return myForm; }

  IGESGraph_TextDisplayTemplate(const Standard_Integer theForm)
  : myForm(theForm),
    myBoxWidth(0.),
    myBoxHeight(0.),
    myFontCode(0),
    mySlantAngle(0.),
    myRotationAngle(0.),
    myMirrorFlag(0),
    myRotateFlag(0)
  {}
  
  //! Sets <me> to be Incremental (Form 1) if <mode> is True,
  //! or Basolute (Form 0) else
  void SetIncremental (const Standard_Boolean mode) { myForm = (mode? 1 : 0); }

  //! returns True  if entity is Incremental (Form 1).
  //! False if entity is Absolute    (Form 0).
  Standard_Boolean IsIncremental() const { return ( myForm == 1 ); }

  //! returns Character Box Width.
  Standard_Real BoxWidth() const { return myBoxWidth; }

  //! returns Character Box Height.
  Standard_Real BoxHeight() const { return myBoxHeight; }

  //! returns False if myFontEntity is Null, True otherwise.
  Standard_Boolean IsFontEntity() const { return (! myFontEntity.IsNull()); }

  //! returns the font code.
  Standard_Integer FontCode() const { return myFontCode; }

  //! returns Text Font Definition Entity used to define the font.
  const Handle(IGESGraph_TextFontDef) & FontEntity() const { return myFontEntity; }

  //! returns slant angle of character in radians.
  Standard_Real SlantAngle() const { return mySlantAngle; }

  //! returns Rotation angle of text block in radians.
  Standard_Real RotationAngle() const { return myRotationAngle; }

  //! returns Mirror flag
  //! Mirror flag : 0 = no mirroring.
  //! 1 = mirror axis perpendicular to text base line.
  //! 2 = mirror axis is text base line.
  Standard_Integer MirrorFlag() const { return myMirrorFlag; }

  //! returns Rotate internal text flag.
  //! Rotate internal text flag : 0 = text horizontal.
  //! 1 = text vertical.
  Standard_Integer RotateFlag() const { return myRotateFlag; }

  //! If IsIncremental() returns False,
  //! gets coordinates of lower left corner
  //! of first character box.
  //! If IsIncremental() returns True,
  //! gets increments from X, Y, Z coordinates
  //! found in parent entity.
  Standard_EXPORT gp_Pnt StartingCorner() const;
  
  //! If IsIncremental() returns False,
  //! gets coordinates of lower left corner
  //! of first character box.
  //! If IsIncremental() returns True,
  //! gets increments from X, Y, Z coordinates
  //! found in parent entity.
  Standard_EXPORT gp_Pnt TransformedStartingCorner() const;

  Standard_EXPORT virtual void OwnRead (IGESFile_Reader &) Standard_OVERRIDE;
  
  Standard_EXPORT virtual void OwnWrite (IGESData_IGESWriter &) const Standard_OVERRIDE;

  Standard_EXPORT virtual void OwnShared (Interface_EntityIterator &) const Standard_OVERRIDE;

  Standard_EXPORT virtual IGESData_DirChecker DirChecker () const Standard_OVERRIDE;

  Standard_EXPORT virtual void OwnDump (const IGESData_IGESDumper &, const Handle(Message_Messenger) &, const Standard_Integer) const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(IGESGraph_TextDisplayTemplate,IGESData_IGESEntity)

 private:

  Standard_Integer myForm;
  Standard_Real myBoxWidth;
  Standard_Real myBoxHeight;
  Standard_Integer myFontCode;
  Interface_Pointer<IGESGraph_TextFontDef> myFontEntity;
  Standard_Real mySlantAngle;
  Standard_Real myRotationAngle;
  Standard_Integer myMirrorFlag;
  Standard_Integer myRotateFlag;
  gp_XYZ myCorner;
};

#endif // _IGESGraph_TextDisplayTemplate_HeaderFile
