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

#ifndef _IGESAppli_FlowLineSpec_HeaderFile
#define _IGESAppli_FlowLineSpec_HeaderFile

#include <Interface_HArray1OfHAsciiString.hxx>
#include <IGESData_IGESEntity.hxx>
class TCollection_HAsciiString;


class IGESAppli_FlowLineSpec;
DEFINE_STANDARD_HANDLE(IGESAppli_FlowLineSpec, IGESData_IGESEntity)

//! defines FlowLineSpec, Type <406> Form <14>
//! in package IGESAppli
//! Attaches one or more text strings to entities being
//! used to represent a flow line
class IGESAppli_FlowLineSpec : public IGESData_IGESEntity
{
 public:

  Standard_EXPORT virtual Standard_Integer TypeNumber() const Standard_OVERRIDE { return 406; }

  Standard_EXPORT virtual Standard_Integer FormNumber() const Standard_OVERRIDE { return 14; }

  IGESAppli_FlowLineSpec() {}
  
  //! returns the number of property values
  Standard_EXPORT Standard_Integer NbPropertyValues() const;
  
  //! returns primary flow line specification name
  Standard_EXPORT const Handle(TCollection_HAsciiString) & FlowLineName() const;
  
  //! returns specified modifier element
  //! raises exception if Index <= 1 or Index > NbPropertyValues
  Standard_EXPORT const Handle(TCollection_HAsciiString) & Modifier (const Standard_Integer Index) const;

  Standard_EXPORT virtual void OwnRead (IGESFile_Reader &) Standard_OVERRIDE;
  
  Standard_EXPORT virtual void OwnWrite (IGESData_IGESWriter &) const Standard_OVERRIDE;

  Standard_EXPORT virtual IGESData_DirChecker DirChecker () const Standard_OVERRIDE;

  Standard_EXPORT virtual void OwnDump (const IGESData_IGESDumper &, const Handle(Message_Messenger) &, const Standard_Integer) const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(IGESAppli_FlowLineSpec,IGESData_IGESEntity)

 private:

  Handle(Interface_HArray1OfHAsciiString) myNameAndModifiers;
};

#endif // _IGESAppli_FlowLineSpec_HeaderFile
