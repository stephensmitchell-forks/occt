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

#ifndef _IGESAppli_LevelFunction_HeaderFile
#define _IGESAppli_LevelFunction_HeaderFile

#include <IGESData_IGESEntity.hxx>
class TCollection_HAsciiString;


class IGESAppli_LevelFunction;
DEFINE_STANDARD_HANDLE(IGESAppli_LevelFunction, IGESData_IGESEntity)

//! defines LevelFunction, Type <406> Form <3>
//! in package IGESAppli
//! Used to transfer the meaning or intended use of a level
//! in the sending system
class IGESAppli_LevelFunction : public IGESData_IGESEntity
{
 public:

  Standard_EXPORT virtual Standard_Integer TypeNumber() const Standard_OVERRIDE { return 406; }

  Standard_EXPORT virtual Standard_Integer FormNumber() const Standard_OVERRIDE { return 3; }

  IGESAppli_LevelFunction()
  : myFuncDescripCode(0)
  {}

  //! returns the function description code . Default = 0
  Standard_Integer FuncDescriptionCode() const { return myFuncDescripCode; }

  //! returns the function description
  //! Default = null string
  const Handle(TCollection_HAsciiString) & FuncDescription() const { return myFuncDescrip; }

  Standard_EXPORT virtual void OwnRead (IGESFile_Reader &) Standard_OVERRIDE;

  Standard_EXPORT virtual void OwnWrite (IGESData_IGESWriter &) const Standard_OVERRIDE;

  Standard_EXPORT virtual IGESData_DirChecker DirChecker () const Standard_OVERRIDE;

  Standard_EXPORT virtual void OwnDump (const IGESData_IGESDumper &, const Handle(Message_Messenger) &, const Standard_Integer) const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(IGESAppli_LevelFunction,IGESData_IGESEntity)

 private:

  Standard_Integer myFuncDescripCode;
  Handle(TCollection_HAsciiString) myFuncDescrip;
};

#endif // _IGESAppli_LevelFunction_HeaderFile
