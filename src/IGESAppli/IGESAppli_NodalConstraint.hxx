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

#ifndef _IGESAppli_NodalConstraint_HeaderFile
#define _IGESAppli_NodalConstraint_HeaderFile

#include <IGESDefs_HArray1OfTabularData.hxx>
#include <IGESData_IGESEntity.hxx>
class IGESAppli_Node;
class IGESDefs_TabularData;


class IGESAppli_NodalConstraint;
DEFINE_STANDARD_HANDLE(IGESAppli_NodalConstraint, IGESData_IGESEntity)

//! defines NodalConstraint, Type <418> Form <0>
//! in package IGESAppli
//! Relates loads and/or constraints to specific nodes in
//! the Finite Element Model by creating a relation between
//! Node entities and Tabular Data Property that contains
//! the load or constraint data
class IGESAppli_NodalConstraint : public IGESData_IGESEntity
{
 public:

  Standard_EXPORT virtual Standard_Integer TypeNumber() const Standard_OVERRIDE { return 418; }

  Standard_EXPORT virtual Standard_Integer FormNumber() const Standard_OVERRIDE { return 0; }

  IGESAppli_NodalConstraint()
  : myType(0)
  {}
  
  //! returns total number of cases
  Standard_EXPORT Standard_Integer NbCases() const;
  
  //! returns whether Loads (1) or Constraints (2)
  Standard_Integer Type() const { return myType; }

  //! returns the Node
  const Handle(IGESAppli_Node) & NodeEntity() const { return myNode; }

  //! returns Tabular Data Property carrying load or constraint vector
  //! raises exception if Index <= 0 or Index > NbCases
  Standard_EXPORT const Handle(IGESDefs_TabularData) & TabularData (const Standard_Integer Index) const;

  Standard_EXPORT virtual void OwnRead (IGESFile_Reader &) Standard_OVERRIDE;
  
  Standard_EXPORT virtual void OwnWrite (IGESData_IGESWriter &) const Standard_OVERRIDE;

  Standard_EXPORT virtual void OwnShared(Interface_EntityIterator &theIter) const Standard_OVERRIDE;

  Standard_EXPORT virtual IGESData_DirChecker DirChecker () const Standard_OVERRIDE;

  Standard_EXPORT virtual void OwnCheck (const Interface_ShareTool &, const Handle(Interface_Check) &) const Standard_OVERRIDE;

  Standard_EXPORT virtual void OwnDump (const IGESData_IGESDumper &, const Handle(Message_Messenger) &, const Standard_Integer) const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(IGESAppli_NodalConstraint,IGESData_IGESEntity)

 private:

  Standard_Integer myType;
  Interface_Pointer<IGESAppli_Node> myNode;
  Handle(IGESDefs_HArray1OfTabularData) myTabularDataProps;
};

#endif // _IGESAppli_NodalConstraint_HeaderFile
