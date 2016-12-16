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

#ifndef _IGESAppli_Node_HeaderFile
#define _IGESAppli_Node_HeaderFile

#include <gp_XYZ.hxx>
#include <IGESData_IGESEntity.hxx>
class IGESGeom_TransformationMatrix;
class gp_Pnt;
class IGESData_TransfEntity;

//! defines Node, Type <134> Form <0>
//! in package IGESAppli
//! Geometric point used in the definition of a finite element.
class IGESAppli_Node : public IGESData_IGESEntity
{
 public:

  Standard_EXPORT virtual Standard_Integer TypeNumber() const Standard_OVERRIDE { return 134; }

  Standard_EXPORT virtual Standard_Integer FormNumber() const Standard_OVERRIDE { return 0; }

  IGESAppli_Node() {}
  
  //! returns the nodal coordinates
  Standard_EXPORT const gp_XYZ & Coord() const { return myCoord; }
  
  //! returns TransfEntity if a Nodal Displacement Coordinate
  //! System Entity is defined
  //! else (for Global Cartesien) returns Null Handle
  const Handle(IGESGeom_TransformationMatrix) & System() const { return mySystem; }
  
  //! Computes & returns the Type of Coordinate System :
  //! 0 GlobalCartesian, 1 Cartesian, 2 Cylindrical, 3 Spherical
  Standard_EXPORT Standard_Integer SystemType() const;
  
  //! returns the Nodal coordinates after transformation
  Standard_EXPORT gp_Pnt TransformedNodalCoord() const;

  Standard_EXPORT virtual void OwnRead (IGESFile_Reader &) Standard_OVERRIDE;
  
  Standard_EXPORT virtual void OwnWrite (IGESData_IGESWriter &) const Standard_OVERRIDE;

  Standard_EXPORT virtual void OwnShared(Interface_EntityIterator &theIter) const Standard_OVERRIDE;

  Standard_EXPORT virtual IGESData_DirChecker DirChecker () const Standard_OVERRIDE;

  Standard_EXPORT virtual void OwnCheck (const Interface_ShareTool &, const Handle(Interface_Check) &) const Standard_OVERRIDE;

  Standard_EXPORT virtual void OwnDump (const IGESData_IGESDumper &, const Handle(Message_Messenger) &, const Standard_Integer) const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(IGESAppli_Node,IGESData_IGESEntity)

 private:

  gp_XYZ myCoord;
  Interface_Pointer<IGESGeom_TransformationMatrix> mySystem;
};

#endif // _IGESAppli_Node_HeaderFile
