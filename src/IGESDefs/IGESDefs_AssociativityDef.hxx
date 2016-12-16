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

#ifndef _IGESDefs_AssociativityDef_HeaderFile
#define _IGESDefs_AssociativityDef_HeaderFile

#include <TColStd_HArray1OfInteger.hxx>
#include <IGESData_IGESEntity.hxx>

//! defines IGES Associativity Definition Entity, Type <302>
//! Form <5001 - 9999> in package IGESDefs.
//! This class permits the preprocessor to define an
//! associativity schema. i.e., by using it preprocessor
//! defines the type of relationship.
class IGESDefs_AssociativityDef : public IGESData_IGESEntity
{
 public:

  Standard_EXPORT virtual Standard_Integer TypeNumber() const Standard_OVERRIDE { return 302; }

  Standard_EXPORT virtual Standard_Integer FormNumber() const Standard_OVERRIDE { return myForm; }

  IGESDefs_AssociativityDef(const Standard_Integer theForm)
  : myForm(theForm)
  {}
  
  //! returns the Number of class definitions
  Standard_EXPORT Standard_Integer NbClassDefs() const;
  
  //! returns 1 if the myBackPointerReqs(ClassNum) = 1
  //! returns 0 if the myBackPointerReqs(ClassNum) = 2
  //! raises exception if ClassNum <= 0 or ClassNum > NbClassDefs()
  Standard_EXPORT Standard_Boolean IsBackPointerReq (const Standard_Integer ClassNum) const;
  
  //! returns 1 or 2
  //! raises exception if ClassNum <= 0 or ClassNum > NbClassDefs()
  Standard_EXPORT Standard_Integer BackPointerReq (const Standard_Integer ClassNum) const;
  
  //! returns 1 if myClassOrders(ClassNum) = 1 (ordered class)
  //! returns 0 if myClassOrders(ClassNum) = 2 (unordered class)
  //! raises exception if ClassNum <= 0 or ClassNum > NbClassDefs()
  Standard_EXPORT Standard_Boolean IsOrdered (const Standard_Integer ClassNum) const;
  
  //! returns 1 or 2
  //! raises exception if ClassNum <= 0 or ClassNum > NbClassDefs()
  Standard_EXPORT Standard_Integer ClassOrder (const Standard_Integer ClassNum) const;
  
  //! returns no. of items per class entry
  //! raises exception if ClassNum <= 0 or ClassNum > NbClassDefs()
  Standard_EXPORT Standard_Integer NbItemsPerClass (const Standard_Integer ClassNum) const;
  
  //! returns ItemNum'th Item of ClassNum'th Class
  //! raises exception if
  //! ClassNum <= 0 or ClassNum > NbClassDefs()
  //! ItemNum <= 0 or ItemNum > NbItemsPerClass(ClassNum)
  Standard_EXPORT Standard_Integer Item (const Standard_Integer ClassNum, const Standard_Integer ItemNum) const;

  Standard_EXPORT virtual void OwnRead (IGESFile_Reader &) Standard_OVERRIDE;
  
  Standard_EXPORT virtual void OwnWrite (IGESData_IGESWriter &) const Standard_OVERRIDE;
  
  Standard_EXPORT virtual IGESData_DirChecker DirChecker () const Standard_OVERRIDE;

  Standard_EXPORT virtual void OwnDump (const IGESData_IGESDumper &, const Handle(Message_Messenger) &, const Standard_Integer) const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(IGESDefs_AssociativityDef,IGESData_IGESEntity)

 private:

  Standard_Integer myForm;
  Handle(TColStd_HArray1OfInteger) myBackPointerReqs;
  Handle(TColStd_HArray1OfInteger) myClassOrders;
  Handle(TColStd_HArray1OfInteger) myNbItemsPerClass;
  Handle(IGESBasic_HArray1OfHArray1OfInteger) myItems;
};

#endif // _IGESDefs_AssociativityDef_HeaderFile
