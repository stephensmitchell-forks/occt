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

#ifndef _IGESGraph_TextFontDef_HeaderFile
#define _IGESGraph_TextFontDef_HeaderFile

#include <IGESData_IGESEntity.hxx>
class TCollection_HAsciiString;
class TColStd_HArray1OfInteger;
class IGESBasic_HArray1OfHArray1OfInteger;


//! defines IGES Text Font Definition Entity, Type <310>
//! in package IGESGraph
//!
//! Used to define the appearance of characters in a text font.
//! It may be used to describe a complete font or a
//! modification to a subset of characters in another font.
class IGESGraph_TextFontDef : public IGESData_IGESEntity
{
 public:

  Standard_EXPORT virtual Standard_Integer TypeNumber() const Standard_OVERRIDE { return 310; }

  Standard_EXPORT virtual Standard_Integer FormNumber() const Standard_OVERRIDE { return 0; }

  IGESGraph_TextFontDef()
  : myFontCode(0),
    mySupersededFontCode(0),
    myScale(0)
  {}

  //! returns the font code.
  Standard_Integer FontCode() const { return myFontCode; }

  //! returns the font name.
  const Handle(TCollection_HAsciiString) & FontName() const { return myFontName; }

  //! True if this definition supersedes another
  //! TextFontDefinition Entity,
  //! False if it supersedes value.
  Standard_Boolean IsSupersededFontEntity() const { return (!mySupersededFontEntity.IsNull()); }

  //! returns the font number which this entity modifies.
  Standard_Integer SupersededFontCode() const { return mySupersededFontCode; }

  //! returns the font entity which this entity modifies.
  const Handle(IGESGraph_TextFontDef) & SupersededFontEntity() const { return mySupersededFontEntity; }

  //! returns the number of grid units which equal one text height unit.
  Standard_Integer Scale() const { return myScale; }

  //! returns the number of characters in this definition.
  Standard_EXPORT Standard_Integer NbCharacters() const;
  
  //! returns the ASCII code of Chnum'th character.
  //! Exception OutOfRange is raised if Chnum <= 0 or Chnum > NbCharacters
  Standard_EXPORT Standard_Integer ASCIICode (const Standard_Integer Chnum) const;
  
  //! returns grid location of origin of character next to Chnum'th char.
  //! Exception OutOfRange is raised if Chnum <= 0 or Chnum > NbCharacters
  Standard_EXPORT void NextCharOrigin (const Standard_Integer Chnum, Standard_Integer& NX, Standard_Integer& NY) const;
  
  //! returns number of pen motions for Chnum'th character.
  //! Exception OutOfRange is raised if Chnum <= 0 or Chnum > NbCharacters
  Standard_EXPORT Standard_Integer NbPenMotions (const Standard_Integer Chnum) const;
  
  //! returns pen status(True if 1, False if 0) of Motionnum'th motion
  //! of Chnum'th character.
  //! Exception raised if Chnum <= 0 or Chnum > NbCharacters or
  //! Motionnum <= 0 or Motionnum >  NbPenMotions
  Standard_EXPORT Standard_Boolean IsPenUp (const Standard_Integer Chnum, const Standard_Integer Motionnum) const;
  
  Standard_EXPORT void NextPenPosition (const Standard_Integer Chnum, const Standard_Integer Motionnum, Standard_Integer& IX, Standard_Integer& IY) const;

  Standard_EXPORT virtual void OwnRead (IGESFile_Reader &) Standard_OVERRIDE;
  
  Standard_EXPORT virtual void OwnWrite (IGESData_IGESWriter &) const Standard_OVERRIDE;

  Standard_EXPORT virtual void OwnShared (Interface_EntityIterator &) const Standard_OVERRIDE;

  Standard_EXPORT virtual IGESData_DirChecker DirChecker () const Standard_OVERRIDE;

  Standard_EXPORT virtual void OwnDump (const IGESData_IGESDumper &, const Handle(Message_Messenger) &, const Standard_Integer) const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(IGESGraph_TextFontDef,IGESData_IGESEntity)

 private:

  Standard_Integer myFontCode;
  Handle(TCollection_HAsciiString) myFontName;
  Standard_Integer mySupersededFontCode;
  Interface_Pointer<IGESGraph_TextFontDef> mySupersededFontEntity;
  Standard_Integer myScale;
  Handle(TColStd_HArray1OfInteger) myASCIICodes;
  Handle(TColStd_HArray1OfInteger) myNextCharOriginX;
  Handle(TColStd_HArray1OfInteger) myNextCharOriginY;
  Handle(TColStd_HArray1OfInteger) myNbPenMotions;
  Handle(IGESBasic_HArray1OfHArray1OfInteger) myPenMotions;
  Handle(IGESBasic_HArray1OfHArray1OfInteger) myPenMovesToX;
  Handle(IGESBasic_HArray1OfHArray1OfInteger) myPenMovesToY;
};

#endif // _IGESGraph_TextFontDef_HeaderFile
