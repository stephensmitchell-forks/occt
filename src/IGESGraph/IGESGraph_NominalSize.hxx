// Created on: 1993-01-09
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

#ifndef _IGESGraph_NominalSize_HeaderFile
#define _IGESGraph_NominalSize_HeaderFile

#include <IGESData_IGESEntity.hxx>
class TCollection_HAsciiString;


//! defines IGESNominalSize, Type <406> Form <13>
//! in package IGESGraph
//!
//! Specifies a value, a name, and optionally a
//! reference to an engineering standard
class IGESGraph_NominalSize : public IGESData_IGESEntity
{
 public:

  Standard_EXPORT virtual Standard_Integer TypeNumber() const Standard_OVERRIDE { return 406; }

  Standard_EXPORT virtual Standard_Integer FormNumber() const Standard_OVERRIDE { return 13; }

  IGESGraph_NominalSize()
  : myNominalSizeValue(0.)
  {}

  //! returns the value of <me>
  Standard_Real NominalSizeValue() const { return myNominalSizeValue; }

  //! returns the name of <me>
  const Handle(TCollection_HAsciiString) & NominalSizeName() const { return myNominalSizeName; }

  //! returns True if an engineering Standard is defined for <me>
  //! else, returns False
  Standard_Boolean HasStandardName() const { return (! myStandardName.IsNull() ); }

  //! returns the name of the relevant engineering standard of <me>
  const Handle(TCollection_HAsciiString) & StandardName() const { return myStandardName; }
  
  Standard_EXPORT virtual void OwnRead (IGESFile_Reader &) Standard_OVERRIDE;
  
  Standard_EXPORT virtual void OwnWrite (IGESData_IGESWriter &) const Standard_OVERRIDE;

  Standard_EXPORT virtual IGESData_DirChecker DirChecker () const Standard_OVERRIDE;

  Standard_EXPORT virtual void OwnDump (const IGESData_IGESDumper &, const Handle(Message_Messenger) &, const Standard_Integer) const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(IGESGraph_NominalSize,IGESData_IGESEntity)

 private:

  Standard_Real myNominalSizeValue;
  Handle(TCollection_HAsciiString) myNominalSizeName;
  Handle(TCollection_HAsciiString) myStandardName;
};

#endif // _IGESGraph_NominalSize_HeaderFile
