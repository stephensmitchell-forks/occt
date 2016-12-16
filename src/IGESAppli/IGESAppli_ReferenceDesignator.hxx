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

#ifndef _IGESAppli_ReferenceDesignator_HeaderFile
#define _IGESAppli_ReferenceDesignator_HeaderFile

#include <IGESData_IGESEntity.hxx>
class TCollection_HAsciiString;


class IGESAppli_ReferenceDesignator;
DEFINE_STANDARD_HANDLE(IGESAppli_ReferenceDesignator, IGESData_IGESEntity)

//! defines ReferenceDesignator, Type <406> Form <7>
//! in package IGESAppli
//! Used to attach a text string containing the value of
//! a component reference designator to an entity being
//! used to represent a component.
class IGESAppli_ReferenceDesignator : public IGESData_IGESEntity
{
 public:

  Standard_EXPORT virtual Standard_Integer TypeNumber() const Standard_OVERRIDE { return 406; }

  Standard_EXPORT virtual Standard_Integer FormNumber() const Standard_OVERRIDE { return 7; }

  IGESAppli_ReferenceDesignator() {}

  //! returns the Reference designator text
  const Handle(TCollection_HAsciiString) & RefDesignatorText() const { return myRefDesigText; }

  Standard_EXPORT virtual void OwnRead (IGESFile_Reader &) Standard_OVERRIDE;
  
  Standard_EXPORT virtual void OwnWrite (IGESData_IGESWriter &) const Standard_OVERRIDE;

  Standard_EXPORT virtual IGESData_DirChecker DirChecker () const Standard_OVERRIDE;

  Standard_EXPORT virtual void OwnCheck (const Interface_ShareTool &, const Handle(Interface_Check) &) const Standard_OVERRIDE;

  Standard_EXPORT virtual Standard_Boolean OwnCorrect () Standard_OVERRIDE;

  Standard_EXPORT virtual void OwnDump (const IGESData_IGESDumper &, const Handle(Message_Messenger) &, const Standard_Integer) const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(IGESAppli_ReferenceDesignator,IGESData_IGESEntity)

 private:

  Handle(TCollection_HAsciiString) myRefDesigText;
};

#endif // _IGESAppli_ReferenceDesignator_HeaderFile
