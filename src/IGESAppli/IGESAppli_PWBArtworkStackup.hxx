// Created on: 1993-01-11
// Created by: CKY / Contract Toubro-Larsen ( Anand NATRAJAN )
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

#ifndef _IGESAppli_PWBArtworkStackup_HeaderFile
#define _IGESAppli_PWBArtworkStackup_HeaderFile

#include <TColStd_HArray1OfInteger.hxx>
#include <IGESData_IGESEntity.hxx>
class TCollection_HAsciiString;


class IGESAppli_PWBArtworkStackup;
DEFINE_STANDARD_HANDLE(IGESAppli_PWBArtworkStackup, IGESData_IGESEntity)

//! defines PWBArtworkStackup, Type <406> Form <25>
//! in package IGESAppli
//! Used to communicate which exchange file levels are to
//! be combined in order to create the artwork for a
//! printed wire board (PWB). This property should be
//! attached to the entity defining the printed wire
//! assembly (PWA) or if no such entity exists, then the
//! property should stand alone in the file.
class IGESAppli_PWBArtworkStackup : public IGESData_IGESEntity
{
 public:

  Standard_EXPORT virtual Standard_Integer TypeNumber() const Standard_OVERRIDE { return 406; }

  Standard_EXPORT virtual Standard_Integer FormNumber() const Standard_OVERRIDE { return 25; }

  IGESAppli_PWBArtworkStackup()
  : myNbPropertyValues(0)
  {}
  
  //! returns number of property values
  Standard_Integer NbPropertyValues() const { return myNbPropertyValues; }

  //! returns Artwork Stackup Identification
  const Handle(TCollection_HAsciiString) & Identification() const { return myArtworkStackupIdent; }

  //! returns total number of Level Numbers
  Standard_EXPORT Standard_Integer NbLevelNumbers() const;
  
  //! returns Level Number
  //! raises exception if Index <= 0 or Index > NbLevelNumbers
  Standard_EXPORT Standard_Integer LevelNumber (const Standard_Integer Index) const;

  Standard_EXPORT virtual void OwnRead (IGESFile_Reader &) Standard_OVERRIDE;
  
  Standard_EXPORT virtual void OwnWrite (IGESData_IGESWriter &) const Standard_OVERRIDE;

  Standard_EXPORT virtual IGESData_DirChecker DirChecker () const Standard_OVERRIDE;

  Standard_EXPORT virtual void OwnDump (const IGESData_IGESDumper &, const Handle(Message_Messenger) &, const Standard_Integer) const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(IGESAppli_PWBArtworkStackup,IGESData_IGESEntity)

 private:

  Standard_Integer myNbPropertyValues;
  Handle(TCollection_HAsciiString) myArtworkStackupIdent;
  Handle(TColStd_HArray1OfInteger) myLevelNumbers;
};

#endif // _IGESAppli_PWBArtworkStackup_HeaderFile
