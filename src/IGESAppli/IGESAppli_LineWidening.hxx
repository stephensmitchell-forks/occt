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

#ifndef _IGESAppli_LineWidening_HeaderFile
#define _IGESAppli_LineWidening_HeaderFile

#include <IGESData_IGESEntity.hxx>

class IGESAppli_LineWidening;
DEFINE_STANDARD_HANDLE(IGESAppli_LineWidening, IGESData_IGESEntity)

//! defines LineWidening, Type <406> Form <5>
//! in package IGESAppli
//! Defines the characteristics of entities when they are
//! used to define locations of items.
class IGESAppli_LineWidening : public IGESData_IGESEntity
{
 public:

  Standard_EXPORT virtual Standard_Integer TypeNumber() const Standard_OVERRIDE { return 406; }

  Standard_EXPORT virtual Standard_Integer FormNumber() const Standard_OVERRIDE { return 5; }

  IGESAppli_LineWidening()
  : myWidth(0.),
    myCorneringCode(0),
    myExtensionFlag(0),
    myJustificationFlag(0),
    myExtensionValue(0.)
  {}
  
  //! returns the width of metallization
  Standard_Real WidthOfMetalization() const { return myWidth; }

  //! returns the cornering code
  //! 0 = Rounded  /   1 = Squared
  Standard_Integer CorneringCode() const { return myCorneringCode; }

  //! returns the extension flag
  //! 0 = No extension
  //! 1 = One-half width extension
  //! 2 = Extension set by theExtnVal
  Standard_Integer ExtensionFlag() const { return myExtensionFlag; }

  //! returns the justification flag
  //! 0 = Centre justified
  //! 1 = Left justified
  //! 2 = Right justified
  Standard_Integer JustificationFlag() const { return myJustificationFlag; }

  //! returns the Extension Value
  //! Present only if theExtnFlag = 2
  Standard_Real ExtensionValue() const { return myExtensionValue; }

  Standard_EXPORT virtual void OwnRead (IGESFile_Reader &) Standard_OVERRIDE;
  
  Standard_EXPORT virtual void OwnWrite (IGESData_IGESWriter &) const Standard_OVERRIDE;

  Standard_EXPORT virtual IGESData_DirChecker DirChecker () const Standard_OVERRIDE;

  Standard_EXPORT virtual void OwnCheck (const Interface_ShareTool &, const Handle(Interface_Check) &) const Standard_OVERRIDE;

  Standard_EXPORT virtual Standard_Boolean OwnCorrect () Standard_OVERRIDE;

  Standard_EXPORT virtual void OwnDump (const IGESData_IGESDumper &, const Handle(Message_Messenger) &, const Standard_Integer) const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(IGESAppli_LineWidening,IGESData_IGESEntity)

 private:

  Standard_Real myWidth;
  Standard_Integer myCorneringCode;
  Standard_Integer myExtensionFlag;
  Standard_Integer myJustificationFlag;
  Standard_Real myExtensionValue;
};

#endif // _IGESAppli_LineWidening_HeaderFile
