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

#ifndef _IGESAppli_PartNumber_HeaderFile
#define _IGESAppli_PartNumber_HeaderFile

#include <IGESData_IGESEntity.hxx>
class TCollection_HAsciiString;


class IGESAppli_PartNumber;
DEFINE_STANDARD_HANDLE(IGESAppli_PartNumber, IGESData_IGESEntity)

//! defines PartNumber, Type <406> Form <9>
//! in package IGESAppli
//! Attaches a set of text strings that define the common
//! part numbers to an entity being used to represent a
//! physical component
class IGESAppli_PartNumber : public IGESData_IGESEntity
{
 public:

  Standard_EXPORT virtual Standard_Integer TypeNumber() const Standard_OVERRIDE { return 406; }

  Standard_EXPORT virtual Standard_Integer FormNumber() const Standard_OVERRIDE { return 9; }

  IGESAppli_PartNumber() {}

  //! returns Generic part number or name
  const Handle(TCollection_HAsciiString) & GenericNumber() const { return myGenericNumber; }

  //! returns Military Standard (MIL-STD) part number
  const Handle(TCollection_HAsciiString) & MilitaryNumber() const { return myMilitaryNumber; }

  //! returns Vendor part number or name
  const Handle(TCollection_HAsciiString) & VendorNumber() const { return myVendorNumber; }

  //! returns Internal part number
  const Handle(TCollection_HAsciiString) & InternalNumber() const { return myInternalNumber; }

  Standard_EXPORT virtual void OwnRead (IGESFile_Reader &) Standard_OVERRIDE;
  
  Standard_EXPORT virtual void OwnWrite (IGESData_IGESWriter &) const Standard_OVERRIDE;

  Standard_EXPORT virtual IGESData_DirChecker DirChecker () const Standard_OVERRIDE;

  Standard_EXPORT virtual void OwnDump (const IGESData_IGESDumper &, const Handle(Message_Messenger) &, const Standard_Integer) const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(IGESAppli_PartNumber,IGESData_IGESEntity)

 private:

  Handle(TCollection_HAsciiString) myGenericNumber;
  Handle(TCollection_HAsciiString) myMilitaryNumber;
  Handle(TCollection_HAsciiString) myVendorNumber;
  Handle(TCollection_HAsciiString) myInternalNumber;
};

#endif // _IGESAppli_PartNumber_HeaderFile
