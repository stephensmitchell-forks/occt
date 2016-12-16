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

#ifndef _IGESAppli_RegionRestriction_HeaderFile
#define _IGESAppli_RegionRestriction_HeaderFile

#include <IGESData_IGESEntity.hxx>

class IGESAppli_RegionRestriction;
DEFINE_STANDARD_HANDLE(IGESAppli_RegionRestriction, IGESData_IGESEntity)

//! defines RegionRestriction, Type <406> Form <2>
//! in package IGESAppli
//! Defines regions to set an application's restriction
//! over a region.
class IGESAppli_RegionRestriction : public IGESData_IGESEntity
{
 public:

  Standard_EXPORT virtual Standard_Integer TypeNumber() const Standard_OVERRIDE { return 406; }

  Standard_EXPORT virtual Standard_Integer FormNumber() const Standard_OVERRIDE { return 2; }

  IGESAppli_RegionRestriction()
  : myElectViasRestrict(0),
    myElectCompRestrict(0),
    myElectCktRestrict(0)
  {}

  //! returns the Electrical vias restriction
  //! is 0, 1 or 2
  Standard_Integer ElectricalViasRestriction() const { return myElectViasRestrict; }

  //! returns the Electrical components restriction
  //! is 0, 1 or 2
  Standard_Integer ElectricalComponentRestriction() const { return myElectCompRestrict; }

  //! returns the Electrical circuitry restriction
  //! is 0, 1 or 2
  Standard_Integer ElectricalCktRestriction() const { return myElectCktRestrict; }

  Standard_EXPORT virtual void OwnRead (IGESFile_Reader &) Standard_OVERRIDE;
  
  Standard_EXPORT virtual void OwnWrite (IGESData_IGESWriter &) const Standard_OVERRIDE;

  Standard_EXPORT virtual IGESData_DirChecker DirChecker () const Standard_OVERRIDE;

  Standard_EXPORT virtual void OwnCheck (const Interface_ShareTool &, const Handle(Interface_Check) &) const Standard_OVERRIDE;

  Standard_EXPORT virtual Standard_Boolean OwnCorrect () Standard_OVERRIDE;

  Standard_EXPORT virtual void OwnDump (const IGESData_IGESDumper &, const Handle(Message_Messenger) &, const Standard_Integer) const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(IGESAppli_RegionRestriction,IGESData_IGESEntity)

 private:

  Standard_Integer myElectViasRestrict;
  Standard_Integer myElectCompRestrict;
  Standard_Integer myElectCktRestrict;
};

#endif // _IGESAppli_RegionRestriction_HeaderFile
