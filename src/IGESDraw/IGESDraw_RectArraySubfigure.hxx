// Created on: 1993-01-11
// Created by: CKY / Contract Toubro-Larsen ( Niraj RANGWALA )
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

#ifndef _IGESDraw_RectArraySubfigure_HeaderFile
#define _IGESDraw_RectArraySubfigure_HeaderFile

#include <gp_XYZ.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <IGESData_IGESEntity.hxx>
class gp_Pnt;


//! Defines IGES Rectangular Array Subfigure Instance Entity,
//! Type <412> Form Number <0> in package IGESDraw
//! Used to produce copies of object called the base entity,
//! arranging them in equally spaced rows and columns
class IGESDraw_RectArraySubfigure : public IGESData_IGESEntity
{
 public:

  Standard_EXPORT virtual Standard_Integer TypeNumber() const Standard_OVERRIDE { return 412; }

  Standard_EXPORT virtual Standard_Integer FormNumber() const Standard_OVERRIDE { return 0; }

  IGESDraw_RectArraySubfigure() {}
  
  //! returns the base entity, copies of which are produced
  const Handle(IGESData_IGESEntity) & BaseEntity() const { return theBaseEntity; }

  //! returns the scale factor
  Standard_Real ScaleFactor() const { return theScaleFactor; }

  //! returns coordinates of lower left hand corner for the entire array
  Standard_EXPORT gp_Pnt LowerLeftCorner() const;
  
  //! returns Transformed coordinates of lower left corner for the array
  Standard_EXPORT gp_Pnt TransformedLowerLeftCorner() const;
  
  //! returns number of columns in the array
  Standard_Integer NbColumns() const { return theNbColumns; }

  //! returns number of rows in the array
  Standard_Integer NbRows() const { return theNbRows; }

  //! returns horizontal distance between columns
  Standard_Real ColumnSeparation() const { return theColumnSeparation; }

  //! returns vertical distance between rows
  Standard_Real RowSeparation() const { return theRowSeparation; }

  //! returns rotation angle in radians
  Standard_Real RotationAngle() const { return theRotationAngle; }

  //! returns True if (ListCount = 0) i.e., all elements to be displayed
  Standard_Boolean DisplayFlag() const { return (thePositions.IsNull()); }

  //! returns 0 if all replicated entities to be displayed
  Standard_EXPORT Standard_Integer ListCount() const;
  
  //! returns 0 if half or fewer of the elements of  the array are defined
  //! 1 if half or more of the elements are defined
  Standard_Boolean DoDontFlag() const { return theDoDontFlag; }

  //! returns whether Index is to be processed (DO)
  //! or not to be processed(DON'T)
  //! if (ListCount = 0) return theDoDontFlag
  Standard_EXPORT Standard_Boolean PositionNum (const Standard_Integer Index) const;
  
  //! returns the Index'th value position
  //! raises exception if Index <= 0 or Index > ListCount()
  Standard_EXPORT Standard_Integer ListPosition (const Standard_Integer Index) const;

  Standard_EXPORT virtual void OwnRead (IGESFile_Reader &) Standard_OVERRIDE;
  
  Standard_EXPORT virtual void OwnWrite (IGESData_IGESWriter &) const Standard_OVERRIDE;

  Standard_EXPORT virtual void OwnShared (Interface_EntityIterator &) const Standard_OVERRIDE;

  Standard_EXPORT virtual IGESData_DirChecker DirChecker () const Standard_OVERRIDE;

  Standard_EXPORT virtual void OwnDump (const IGESData_IGESDumper &, const Handle(Message_Messenger) &, const Standard_Integer) const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(IGESDraw_RectArraySubfigure,IGESData_IGESEntity)

 private:

  Interface_Pointer<IGESData_IGESEntity> theBaseEntity;
  Standard_Real theScaleFactor;
  gp_XYZ theLowerLeftCorner;
  Standard_Integer theNbColumns;
  Standard_Integer theNbRows;
  Standard_Real theColumnSeparation;
  Standard_Real theRowSeparation;
  Standard_Real theRotationAngle;
  Standard_Boolean theDoDontFlag;
  Handle(TColStd_HArray1OfInteger) thePositions;
};

#endif // _IGESDraw_RectArraySubfigure_HeaderFile
