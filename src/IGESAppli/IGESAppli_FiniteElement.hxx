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

#ifndef _IGESAppli_FiniteElement_HeaderFile
#define _IGESAppli_FiniteElement_HeaderFile

#include <IGESAppli_HArray1OfNode.hxx>
#include <IGESData_IGESEntity.hxx>
class TCollection_HAsciiString;
class IGESAppli_Node;


class IGESAppli_FiniteElement;
DEFINE_STANDARD_HANDLE(IGESAppli_FiniteElement, IGESData_IGESEntity)

//! defines FiniteElement, Type <136> Form <0>
//! in package IGESAppli
//! Used to define a finite element with the help of an
//! element topology.
class IGESAppli_FiniteElement : public IGESData_IGESEntity
{
 public:

  Standard_EXPORT virtual Standard_Integer TypeNumber() const Standard_OVERRIDE { return 136; }

  Standard_EXPORT virtual Standard_Integer FormNumber() const Standard_OVERRIDE { return 0; }

  IGESAppli_FiniteElement()
  : myTopology(0)
  {}
  
  //! returns Topology type
  Standard_Integer Topology() const { return myTopology; }

  //! returns the number of nodes defining the element
  Standard_EXPORT Standard_Integer NbNodes() const;
  
  //! returns Node defining element entity
  //! raises exception if Index <= 0 or Index > NbNodes()
  Standard_EXPORT const Handle(IGESAppli_Node) & Node (const Standard_Integer Index) const;
  
  //! returns Element Type Name
  const Handle(TCollection_HAsciiString) & Name() const { return myName; }

  Standard_EXPORT virtual void OwnRead (IGESFile_Reader &) Standard_OVERRIDE;
  
  Standard_EXPORT virtual void OwnWrite (IGESData_IGESWriter &) const Standard_OVERRIDE;

  Standard_EXPORT virtual void OwnShared(Interface_EntityIterator &theIter) const Standard_OVERRIDE;

  Standard_EXPORT virtual IGESData_DirChecker DirChecker () const Standard_OVERRIDE;
  
  Standard_EXPORT virtual void OwnDump (const IGESData_IGESDumper &, const Handle(Message_Messenger) &, const Standard_Integer) const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(IGESAppli_FiniteElement,IGESData_IGESEntity)

 private:

  Standard_Integer myTopology;
  Handle(IGESAppli_HArray1OfNode) myNodes;
  Handle(TCollection_HAsciiString) myName;
};

#endif // _IGESAppli_FiniteElement_HeaderFile
