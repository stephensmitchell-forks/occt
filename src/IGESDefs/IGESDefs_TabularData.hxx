// Created on: 1993-01-09
// Created by: CKY / Contract Toubro-Larsen ( SIVA )
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

#ifndef _IGESDefs_TabularData_HeaderFile
#define _IGESDefs_TabularData_HeaderFile

#include <IGESData_IGESEntity.hxx>
class TColStd_HArray1OfInteger;
class TColStd_HArray1OfReal;
class IGESBasic_HArray1OfHArray1OfReal;


//! Defines IGES Tabular Data, Type <406> Form <11>,
//! in package IGESDefs
//! This Class is used to provide a Structure to accomodate
//! point form data.
class IGESDefs_TabularData : public IGESData_IGESEntity
{
 public:

  Standard_EXPORT virtual Standard_Integer TypeNumber() const Standard_OVERRIDE { return 406; }

  Standard_EXPORT virtual Standard_Integer FormNumber() const Standard_OVERRIDE { return 11; }

  IGESDefs_TabularData()
  : myNbPropertyValues(0),
    myPropertyType(0)
  {}
  
  //! This method is used to set the fields of the class
  //! TabularData
  //! - nbProps     : Number of property values
  //! - propType    : Property Type
  //! - typesInd    : Type of independent variables
  //! - nbValuesInd : Number of values of independent variables
  //! - valuesInd   : Values of independent variables
  //! - valuesDep   : Values of dependent variables
  //! raises exception if lengths of typeInd and nbValuesInd are not same
  //Standard_EXPORT void Init (const Standard_Integer nbProps, const Standard_Integer propType, const Handle(TColStd_HArray1OfInteger)& typesInd, const Handle(TColStd_HArray1OfInteger)& nbValuesInd, const Handle(IGESBasic_HArray1OfHArray1OfReal)& valuesInd, const Handle(IGESBasic_HArray1OfHArray1OfReal)& valuesDep);
  
  //! returns the number of property values (recorded)
  Standard_Integer NbPropertyValues() const { return myNbPropertyValues; }

  //! determines the number of property values required
  Standard_Integer ComputedNbPropertyValues() const { return myNbPropertyValues; }

  //! returns the property type
  Standard_Integer PropertyType() const { return myPropertyType; }

  //! returns the number of dependent variables
  Standard_EXPORT Standard_Integer NbDependents() const;
  
  //! returns the number of independent variables
  Standard_EXPORT Standard_Integer NbIndependents() const;
  
  //! returns the type of the num'th independent variable
  //! raises exception if num <= 0 or num > NbIndependents()
  Standard_EXPORT Standard_Integer TypeOfIndependents (const Standard_Integer num) const;
  
  //! returns the number of different values of the num'th indep. variable
  //! raises exception if num <= 0 or num > NbIndependents()
  Standard_EXPORT Standard_Integer NbValues (const Standard_Integer num) const;
  
  Standard_EXPORT Standard_Real IndependentValue (const Standard_Integer variablenum, const Standard_Integer valuenum) const;
  
  Standard_EXPORT Handle(TColStd_HArray1OfReal) DependentValues (const Standard_Integer num) const;
  
  Standard_EXPORT Standard_Real DependentValue (const Standard_Integer variablenum, const Standard_Integer valuenum) const;

  Standard_EXPORT virtual void OwnRead (IGESFile_Reader &) Standard_OVERRIDE;
  
  Standard_EXPORT virtual void OwnWrite (IGESData_IGESWriter &) const Standard_OVERRIDE;

  Standard_EXPORT virtual IGESData_DirChecker DirChecker () const Standard_OVERRIDE;

  Standard_EXPORT virtual void OwnDump (const IGESData_IGESDumper &, const Handle(Message_Messenger) &, const Standard_Integer) const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(IGESDefs_TabularData,IGESData_IGESEntity)

 private:

  Standard_Integer myNbPropertyValues;
  Standard_Integer myPropertyType;
  Handle(TColStd_HArray1OfInteger) myTypeOfIndependentVariables;
  Handle(TColStd_HArray1OfInteger) myNbValues;
  Handle(IGESBasic_HArray1OfHArray1OfReal) myIndependentValues;
  Handle(IGESBasic_HArray1OfHArray1OfReal) myDependentValues;
};

#endif // _IGESDefs_TabularData_HeaderFile
