// Created on: 1993-01-09
// Created by: CKY / Contract Toubro-Larsen (Anand NATRAJAN)
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

#ifndef _IGESDefs_GenericData_HeaderFile
#define _IGESDefs_GenericData_HeaderFile

#include <IGESData_IGESEntity.hxx>
class TCollection_HAsciiString;
class TColStd_HArray1OfInteger;
class TColStd_HArray1OfTransient;


//! defines IGES Generic Data, Type <406> Form <27>
//! in package IGESDefs
//! Used to communicate information defined by the system
//! operator while creating the model. The information is
//! system specific and does not map into one of the
//! predefined properties or associativities. Properties
//! and property values can be defined by multiple
//! instances of this property.
class IGESDefs_GenericData : public IGESData_IGESEntity
{
 public:

  Standard_EXPORT virtual Standard_Integer TypeNumber() const Standard_OVERRIDE { return 406; }

  Standard_EXPORT virtual Standard_Integer FormNumber() const Standard_OVERRIDE { return 27; }

  IGESDefs_GenericData()
  : myNbPropertyValues(0)
  {}

  //! This method is used to set the fields of the class
  //! GenericData
  //! - nbPropVal : Number of property values
  //! - aName     : Property Name
  //! - allTypes  : Property Types
  //! - allValues : Property Values
  Standard_EXPORT void Init (const Standard_Integer nbPropVal, const Handle(TCollection_HAsciiString)& aName, const Handle(TColStd_HArray1OfInteger)& allTypes, const Handle(TColStd_HArray1OfTransient)& allValues);
  
  //! returns the number of property values
  Standard_Integer NbPropertyValues() const { return myNbPropertyValues; }

  //! returns property name
  const Handle(TCollection_HAsciiString) & Name() const { return myName; }

  //! returns the number of TYPE/VALUE pairs
  Standard_EXPORT Standard_Integer NbTypeValuePairs() const;
  
  //! returns the Index'th property value data type
  //! raises exception if Index <= 0 or Index > NbTypeValuePairs()
  Standard_EXPORT Standard_Integer Type (const Standard_Integer Index) const;
  
  //! HArray1OfInteger (length 1), HArray1OfReal (length 1) for
  //! Integer, Real, Boolean (= Integer 0/1),
  //! HAsciiString for String (the value itself),
  //! IGESEntity for Entity (the value itself)
  Standard_EXPORT Handle(Standard_Transient) Value (const Standard_Integer Index) const;
  
  //! Returns Attribute Value <AttrNum, rank ValueNum> as an Integer
  //! Error if Index out of Range, or not an Integer
  Standard_EXPORT Standard_Integer ValueAsInteger (const Standard_Integer ValueNum) const;
  
  //! Returns Attribute Value <AttrNum, rank ValueNum> as a Real
  //! Error if Index out of Range, or not a Real
  Standard_EXPORT Standard_Real ValueAsReal (const Standard_Integer ValueNum) const;
  
  //! Returns Attribute Value <AttrNum, rank ValueNum> as an Integer
  Standard_EXPORT Handle(TCollection_HAsciiString) ValueAsString (const Standard_Integer ValueNum) const;
  
  //! Returns Attribute Value <AttrNum, rank ValueNum> as an Entity
  //! Error if Index out of Range, or not a Entity
  Standard_EXPORT Handle(IGESData_IGESEntity) ValueAsEntity (const Standard_Integer ValueNum) const;
  
  //! Returns Attribute Value <AttrNum, rank ValueNum> as a Boolean
  //! Error if Index out of Range, or not a Logical
  Standard_EXPORT Standard_Boolean ValueAsLogical (const Standard_Integer ValueNum) const;

  Standard_EXPORT virtual void OwnRead (IGESFile_Reader &) Standard_OVERRIDE;
  
  Standard_EXPORT virtual void OwnWrite (IGESData_IGESWriter &) const Standard_OVERRIDE;

  Standard_EXPORT virtual void OwnShared (Interface_EntityIterator& iter) const Standard_OVERRIDE;

  Standard_EXPORT virtual IGESData_DirChecker DirChecker () const Standard_OVERRIDE;

  Standard_EXPORT virtual void OwnCheck (const Interface_ShareTool &, Handle(Interface_Check) &) const Standard_OVERRIDE;

  Standard_EXPORT virtual void OwnDump (const IGESData_IGESDumper &, const Handle(Message_Messenger) &, const Standard_Integer) const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(IGESDefs_GenericData,IGESData_IGESEntity)

 private:

  Standard_Integer myNbPropertyValues;
  Handle(TCollection_HAsciiString) myName;
  Handle(TColStd_HArray1OfInteger) myTypes;
  Handle(TColStd_HArray1OfTransient) myValues;
};

#endif // _IGESDefs_GenericData_HeaderFile
