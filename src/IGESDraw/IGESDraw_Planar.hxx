// Created on: 1993-01-09
// Created by: CKY / Contract Toubro-Larsen ( TCD )
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

#ifndef _IGESDraw_Planar_HeaderFile
#define _IGESDraw_Planar_HeaderFile

#include <IGESData_HArray1OfIGESEntity.hxx>
#include <IGESData_IGESEntity.hxx>
class IGESGeom_TransformationMatrix;


//! defines IGESPlanar, Type <402> Form <16>
//! in package IGESDraw
//!
//! Indicates that a collection of entities is coplanar.The
//! entities may be geometric, annotative, and/or structural.
class IGESDraw_Planar : public IGESData_IGESEntity
{
 public:

  Standard_EXPORT virtual Standard_Integer TypeNumber() const Standard_OVERRIDE { return 402; }

  Standard_EXPORT virtual Standard_Integer FormNumber() const Standard_OVERRIDE { return 16; }

  IGESDraw_Planar() {}

  //! returns the number of Transformation matrices in <me>
  Standard_Integer NbMatrices() const { return theNbMatrices; }

  //! returns the number of Entities in the plane pointed to by this
  //! associativity
  Standard_EXPORT Standard_Integer NbEntities() const;
  
  //! returns True if TransformationMatrix is Identity Matrix,
  //! i.e:- No Matrix defined.
  Standard_Boolean IsIdentityMatrix() const { return ( theTransformationMatrix.IsNull() ); }

  //! returns the Transformation matrix moving data from the XY plane
  //! into space or zero
  const Handle(IGESGeom_TransformationMatrix) & TransformMatrix() const { return theTransformationMatrix; }

  //! returns the Entity on the specified plane, indicated by EntityIndex
  //! raises an exception if EntityIndex <= 0 or
  //! EntityIndex > NbEntities()
  Standard_EXPORT const Handle(IGESData_IGESEntity) & Entity (const Standard_Integer EntityIndex) const;

  Standard_EXPORT virtual void OwnRead (IGESFile_Reader &) Standard_OVERRIDE;
  
  Standard_EXPORT virtual void OwnWrite (IGESData_IGESWriter &) const Standard_OVERRIDE;

  Standard_EXPORT virtual void OwnShared (Interface_EntityIterator &) const Standard_OVERRIDE;

  Standard_EXPORT virtual IGESData_DirChecker DirChecker () const Standard_OVERRIDE;

  Standard_EXPORT virtual void OwnDump (const IGESData_IGESDumper &, const Handle(Message_Messenger) &, const Standard_Integer) const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(IGESDraw_Planar,IGESData_IGESEntity)

 private:

  Standard_Integer theNbMatrices;
  Interface_Pointer<IGESGeom_TransformationMatrix> theTransformationMatrix;
  Handle(IGESData_HArray1OfIGESEntity) theEntities;
};

#endif // _IGESDraw_Planar_HeaderFile
