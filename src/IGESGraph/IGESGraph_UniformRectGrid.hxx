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

#ifndef _IGESGraph_UniformRectGrid_HeaderFile
#define _IGESGraph_UniformRectGrid_HeaderFile

#include <gp_XY.hxx>
#include <IGESData_IGESEntity.hxx>
class gp_Pnt2d;
class gp_Vec2d;


//! defines IGESUniformRectGrid, Type <406> Form <22>
//! in package IGESGraph
//!
//! Stores sufficient information for the creation of
//! a uniform rectangular grid within a drawing
class IGESGraph_UniformRectGrid : public IGESData_IGESEntity
{
 public:

  Standard_EXPORT virtual Standard_Integer TypeNumber() const Standard_OVERRIDE { return 406; }

  Standard_EXPORT virtual Standard_Integer FormNumber() const Standard_OVERRIDE { return 22; }

  IGESGraph_UniformRectGrid()
  : isItFinite(0),
    isItLine(0),
    isItWeighted(0),
    theNbPointsX(0),
    theNbPointsY(0)
  {}

  //! returns False if <me> is an infinite grid,
  //! True  if <me> is a finite grid.
  Standard_Boolean IsFinite() const { return (isItFinite == 1); }

  //! returns False if <me> is a Point grid,
  //! True  if <me> is a Line grid.
  Standard_Boolean IsLine() const { return (isItLine == 1); }

  //! returns False if <me> is a Weighted grid,
  //! True  if <me> is not a Weighted grid.
  Standard_Boolean IsWeighted() const { return (isItWeighted == 0); }

  //! returns coordinates of lower left corner,
  //! if <me> is a finite grid,
  //! coordinates of an arbitrary point,
  //! if <me> is an infinite grid.
  Standard_EXPORT gp_Pnt2d GridPoint() const;
  
  //! returns the grid-spacing in drawing coordinates.
  Standard_EXPORT gp_Vec2d GridSpacing() const;
  
  //! returns the no. of points/lines in X direction
  //! (only applicable if IsFinite() = 1, i.e: a finite grid).
  Standard_Integer NbPointsX() const { return theNbPointsX; }

  //! returns the no. of points/lines in Y direction
  //! (only applicable if IsFinite() = 1, i.e: a finite grid).
  Standard_Integer NbPointsY() const { return theNbPointsY; }

  Standard_EXPORT virtual void OwnRead (IGESFile_Reader &) Standard_OVERRIDE;
  
  Standard_EXPORT virtual void OwnWrite (IGESData_IGESWriter &) const Standard_OVERRIDE;

  Standard_EXPORT virtual IGESData_DirChecker DirChecker () const Standard_OVERRIDE;

  Standard_EXPORT virtual void OwnDump (const IGESData_IGESDumper &, const Handle(Message_Messenger) &, const Standard_Integer) const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(IGESGraph_UniformRectGrid,IGESData_IGESEntity)

 private:

  Standard_Integer isItFinite;
  Standard_Integer isItLine;
  Standard_Integer isItWeighted;
  gp_XY theGridPoint;
  gp_XY theGridSpacing;
  Standard_Integer theNbPointsX;
  Standard_Integer theNbPointsY;
};

#endif // _IGESGraph_UniformRectGrid_HeaderFile
