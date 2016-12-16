// Created on: 1993-01-11
// Created by: CKY / Contract Toubro-Larsen ( Arun MENON )
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

#ifndef _IGESAppli_PinNumber_HeaderFile
#define _IGESAppli_PinNumber_HeaderFile

#include <IGESData_IGESEntity.hxx>
class TCollection_HAsciiString;


class IGESAppli_PinNumber;
DEFINE_STANDARD_HANDLE(IGESAppli_PinNumber, IGESData_IGESEntity)

//! defines PinNumber, Type <406> Form <8>
//! in package IGESAppli
//! Used to attach a text string representing a component
//! pin number to an entity being used to represent an
//! electrical component's pin
class IGESAppli_PinNumber : public IGESData_IGESEntity
{
 public:

  Standard_EXPORT virtual Standard_Integer TypeNumber() const Standard_OVERRIDE { return 406; }

  Standard_EXPORT virtual Standard_Integer FormNumber() const Standard_OVERRIDE { return 8; }

  IGESAppli_PinNumber() {}
  
  //! returns the pin number value
  const Handle(TCollection_HAsciiString) & PinNumberVal() const { return myPinNumber; }

  Standard_EXPORT virtual void OwnRead (IGESFile_Reader &) Standard_OVERRIDE;
  
  Standard_EXPORT virtual void OwnWrite (IGESData_IGESWriter &) const Standard_OVERRIDE;

  Standard_EXPORT virtual IGESData_DirChecker DirChecker () const Standard_OVERRIDE;

  Standard_EXPORT virtual void OwnCheck (const Interface_ShareTool &, const Handle(Interface_Check) &) const Standard_OVERRIDE;

  Standard_EXPORT Standard_Boolean OwnCorrect () Standard_OVERRIDE;

  Standard_EXPORT virtual void OwnDump (const IGESData_IGESDumper &, const Handle(Message_Messenger) &, const Standard_Integer) const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(IGESAppli_PinNumber,IGESData_IGESEntity)

 private:

  Handle(TCollection_HAsciiString) myPinNumber;
};

#endif // _IGESAppli_PinNumber_HeaderFile
