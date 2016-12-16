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

#ifndef _IGESDraw_CircArraySubfigure_HeaderFile
#define _IGESDraw_CircArraySubfigure_HeaderFile

#include <IGESData_IGESEntity.hxx>
#include <gp_XYZ.hxx>
class TColStd_HArray1OfInteger;


//! Defines IGES Circular Array Subfigure Instance Entity,
//! Type <414> Form Number <0> in package IGESDraw
//!
//! Used to produce copies of object called the base entity,
//! arranging them around the edge of an imaginary circle
//! whose center and radius are specified
class IGESDraw_CircArraySubfigure : public IGESData_IGESEntity
{
 public:

  Standard_EXPORT virtual Standard_Integer TypeNumber() const Standard_OVERRIDE { return 414; }

  Standard_EXPORT virtual Standard_Integer FormNumber() const Standard_OVERRIDE { return 0; }

  IGESDraw_CircArraySubfigure() {}

  //! returns the base entity, copies of which are produced
  const Handle(IGESData_IGESEntity) & BaseEntity() const { return myBaseEntity; }

  //! returns total number of possible instance locations
  Standard_Integer NbLocations() const { return myNbLocations; }

  //! returns the center of the imaginary circle
  Standard_EXPORT gp_Pnt CenterPoint() const;
  
  //! returns the Transformed center of the imaginary circle
  Standard_EXPORT gp_Pnt TransformedCenterPoint() const;
  
  //! returns the radius of the imaginary circle
  Standard_Real CircleRadius() const { return myRadius; }

  //! returns the start angle in radians
  Standard_Real StartAngle() const { return myStartAngle; }

  //! returns the delta angle in radians
  Standard_Real DeltaAngle() const { return myDeltaAngle; }

  //! returns 0 if all elements to be displayed
  Standard_EXPORT Standard_Integer ListCount() const;
  
  //! returns True if (ListCount = 0) all elements are to be displayed
  Standard_Boolean DisplayFlag() const
  {
    return (myPositions.IsNull());
    // if LC == 0 then there is no allocation made for thePositions array
    // i.e., thePositions == Null Handle
  }

  //! returns 0 if half or fewer of the elements of  the array are defined
  //! 1 if half or more of the elements are defined
  Standard_Boolean DoDontFlag() const { return (myDoDontFlag != 0); }

  //! returns whether Index is to be processed (DO)
  //! or not to be processed(DON'T)
  //! if (ListCount = 0) return theDoDontFlag
  //! raises exception if Index <= 0 or Index > ListCount().
  Standard_EXPORT Standard_Boolean PositionNum (const Standard_Integer Index) const;
  
  //! returns the Index'th value position
  //! raises exception if Index <= 0 or Index > ListCount().
  Standard_EXPORT Standard_Integer ListPosition (const Standard_Integer Index) const;

  Standard_EXPORT virtual void OwnRead (IGESFile_Reader &) Standard_OVERRIDE;
  
  Standard_EXPORT virtual void OwnWrite (IGESData_IGESWriter &) const Standard_OVERRIDE;

  Standard_EXPORT virtual IGESData_DirChecker DirChecker () const Standard_OVERRIDE;
  
  Standard_EXPORT virtual void OwnDump (const IGESData_IGESDumper &, const Handle(Message_Messenger) &, const Standard_Integer) const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(IGESDraw_CircArraySubfigure,IGESData_IGESEntity)

 private:

  Interface_Pointer<IGESData_IGESEntity> myBaseEntity;
  Standard_Integer myNbLocations;
  gp_XYZ myCenter;
  Standard_Real myRadius;
  Standard_Real myStartAngle;
  Standard_Real myDeltaAngle;
  Standard_Integer myDoDontFlag;
  Handle(TColStd_HArray1OfInteger) myPositions;
};

#endif // _IGESDraw_CircArraySubfigure_HeaderFile
