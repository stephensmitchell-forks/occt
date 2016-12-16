// Created on: 1993-01-13
// Created by: CKY / Contract Toubro-Larsen ( Deepak PRABHU )
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

#ifndef _IGESDefs_MacroDef_HeaderFile
#define _IGESDefs_MacroDef_HeaderFile

#include <IGESData_IGESEntity.hxx>
class TCollection_HAsciiString;
class Interface_HArray1OfHAsciiString;


//! defines IGES Macro Definition Entity, Type <306> Form <0>
//! in package IGESDefs
//! This Class specifies the action of a specific MACRO.
//! After specification MACRO can be used as necessary
//! by means of MACRO class instance entity.
class IGESDefs_MacroDef : public IGESData_IGESEntity
{
 public:

  Standard_EXPORT virtual Standard_Integer TypeNumber() const Standard_OVERRIDE { return 306; }

  Standard_EXPORT virtual Standard_Integer FormNumber() const Standard_OVERRIDE { return 0; }

  IGESDefs_MacroDef()
  : myEntityTypeID(0)
  {}
  
  //! This method is used to set the fields of the class
  //! MacroDef
  //! - macro          : MACRO
  //! - entityTypeID   : Entity Type ID
  //! - langStatements : Language Statements
  //! - endMacro       : END MACRO
  Standard_EXPORT void Init (const Handle(TCollection_HAsciiString)& macro, const Standard_Integer entityTypeID, const Handle(Interface_HArray1OfHAsciiString)& langStatements, const Handle(TCollection_HAsciiString)& endMacro);
  
  //! returns the number of language statements
  Standard_EXPORT Standard_Integer NbStatements() const;
  
  //! returns the MACRO(Literal)
  const Handle(TCollection_HAsciiString) & MACRO() const { return myMACRO; }

  //! returns the Entity Type ID
  Standard_Integer EntityTypeID() const { return myEntityTypeID; }

  Standard_EXPORT const Handle(TCollection_HAsciiString) & LanguageStatement (const Standard_Integer StatNum) const;
  
  //! returns the ENDM(Literal)
  const Handle(TCollection_HAsciiString) & ENDMACRO() const { return myENDMACRO; }

  Standard_EXPORT virtual void OwnRead (IGESFile_Reader &) Standard_OVERRIDE;
  
  Standard_EXPORT virtual void OwnWrite (IGESData_IGESWriter &) const Standard_OVERRIDE;

  Standard_EXPORT virtual IGESData_DirChecker DirChecker () const Standard_OVERRIDE;

  Standard_EXPORT virtual void OwnDump (const IGESData_IGESDumper &, const Handle(Message_Messenger) &, const Standard_Integer) const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(IGESDefs_MacroDef,IGESData_IGESEntity)

 private:

  Handle(TCollection_HAsciiString) myMACRO;
  Standard_Integer myEntityTypeID;
  Handle(Interface_HArray1OfHAsciiString) myLangStatements;
  Handle(TCollection_HAsciiString) myENDMACRO;
};

#endif // _IGESDefs_MacroDef_HeaderFile
