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

#ifndef _IGESDefs_UnitsData_HeaderFile
#define _IGESDefs_UnitsData_HeaderFile

#include <Interface_HArray1OfHAsciiString.hxx>
#include <IGESData_IGESEntity.hxx>
class TCollection_HAsciiString;
class TColStd_HArray1OfReal;


class IGESDefs_UnitsData;
DEFINE_STANDARD_HANDLE(IGESDefs_UnitsData, IGESData_IGESEntity)

//! defines IGES UnitsData Entity, Type <316> Form <0>
//! in package IGESDefs
//! This class stores data about a model's fundamental units.
class IGESDefs_UnitsData : public IGESData_IGESEntity
{
 public:

  Standard_EXPORT virtual Standard_Integer TypeNumber() const Standard_OVERRIDE { return 316; }

  Standard_EXPORT virtual Standard_Integer FormNumber() const Standard_OVERRIDE { return 0; }

  IGESDefs_UnitsData() {}
  
  //! This method is used to set the fields of the class
  //! UnitsData
  //! - unitTypes  : Types of the units being defined
  //! - unitValues : Unit Values of the units
  //! - unitScales : Multiplicative Scale Factors
  //! raises exception if lengths of unitTypes, unitValues and
  //! unitScale are not same
  //Standard_EXPORT void Init (const Handle(Interface_HArray1OfHAsciiString)& unitTypes, const Handle(Interface_HArray1OfHAsciiString)& unitValues, const Handle(TColStd_HArray1OfReal)& unitScales);
  
  //! returns the Number of units defined by this entity
  Standard_EXPORT Standard_Integer NbUnits() const;
  
  //! returns the Type of the UnitNum'th unit being defined
  //! raises exception if UnitNum <= 0 or UnitNum > NbUnits()
  Standard_EXPORT const Handle(TCollection_HAsciiString) & UnitType (const Standard_Integer UnitNum) const;
  
  //! returns the Units of the UnitNum'th unit being defined
  //! raises exception if UnitNum <= 0 or UnitNum > NbUnits()
  Standard_EXPORT const Handle(TCollection_HAsciiString) & UnitValue (const Standard_Integer UnitNum) const;
  
  //! returns the multiplicative scale factor to be apllied to the
  //! UnitNum'th unit being defined
  //! raises exception if UnitNum <= 0 or UnitNum > NbUnits()
  Standard_EXPORT Standard_Real ScaleFactor (const Standard_Integer UnitNum) const;

  Standard_EXPORT virtual void OwnRead (IGESFile_Reader &) Standard_OVERRIDE;
  
  Standard_EXPORT virtual void OwnWrite (IGESData_IGESWriter &) const Standard_OVERRIDE;

  Standard_EXPORT virtual IGESData_DirChecker DirChecker () const Standard_OVERRIDE;

  Standard_EXPORT virtual void OwnDump (const IGESData_IGESDumper &, const Handle(Message_Messenger) &, const Standard_Integer) const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(IGESDefs_UnitsData,IGESData_IGESEntity)

 private:

  Handle(Interface_HArray1OfHAsciiString) theUnitTypes;
  Handle(Interface_HArray1OfHAsciiString) theUnitValues;
  Handle(TColStd_HArray1OfReal) theUnitScales;
};

#endif // _IGESDefs_UnitsData_HeaderFile
