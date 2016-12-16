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

#ifndef _IGESAppli_LevelToPWBLayerMap_HeaderFile
#define _IGESAppli_LevelToPWBLayerMap_HeaderFile

#include <TColStd_HArray1OfInteger.hxx>
#include <Interface_HArray1OfHAsciiString.hxx>
#include <IGESData_IGESEntity.hxx>
class TCollection_HAsciiString;


class IGESAppli_LevelToPWBLayerMap;
DEFINE_STANDARD_HANDLE(IGESAppli_LevelToPWBLayerMap, IGESData_IGESEntity)

//! defines LevelToPWBLayerMap, Type <406> Form <24>
//! in package IGESAppli
//! Used to correlate an exchange file level number with
//! its corresponding native level identifier, physical PWB
//! layer number and predefined functional level
//! identification
class IGESAppli_LevelToPWBLayerMap : public IGESData_IGESEntity
{
 public:

  Standard_EXPORT virtual Standard_Integer TypeNumber() const Standard_OVERRIDE { return 406; }

  Standard_EXPORT virtual Standard_Integer FormNumber() const Standard_OVERRIDE { return 24; }

  IGESAppli_LevelToPWBLayerMap() : myNbPropertyValues(0) {}

  //! returns number of level to layer definitions
  Standard_EXPORT Standard_Integer NbLevelToLayerDefs() const;
  
  //! returns Exchange File Level Number
  //! raises exception if Index <= 0 or Index > NbLevelToLayerDefs
  Standard_EXPORT Standard_Integer ExchangeFileLevelNumber (const Standard_Integer Index) const;
  
  //! returns Native Level Identification
  //! raises exception if Index <= 0 or Index > NbLevelToLayerDefs
  Standard_EXPORT const Handle(TCollection_HAsciiString) & NativeLevel (const Standard_Integer Index) const;
  
  //! returns Physical Layer Number
  //! raises exception if Index <= 0 or Index > NbLevelToLayerDefs
  Standard_EXPORT Standard_Integer PhysicalLayerNumber (const Standard_Integer Index) const;
  
  Standard_EXPORT const Handle(TCollection_HAsciiString) & ExchangeFileLevelIdent (const Standard_Integer Index) const;

  Standard_EXPORT virtual void OwnRead (IGESFile_Reader &) Standard_OVERRIDE;
  
  Standard_EXPORT virtual void OwnWrite (IGESData_IGESWriter &) const Standard_OVERRIDE;

  Standard_EXPORT virtual IGESData_DirChecker DirChecker () const Standard_OVERRIDE;

  Standard_EXPORT virtual void OwnDump (const IGESData_IGESDumper &, const Handle(Message_Messenger) &, const Standard_Integer) const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(IGESAppli_LevelToPWBLayerMap,IGESData_IGESEntity)

 private:

  Standard_Integer myNbPropertyValues;
  Handle(TColStd_HArray1OfInteger) myExchangeFileLevelNumber;
  Handle(Interface_HArray1OfHAsciiString) myNativeLevel;
  Handle(TColStd_HArray1OfInteger) myPhysicalLayerNumber;
  Handle(Interface_HArray1OfHAsciiString) myExchangeFileLevelIdent;
};

#endif // _IGESAppli_LevelToPWBLayerMap_HeaderFile
