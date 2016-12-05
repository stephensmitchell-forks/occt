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

#ifndef _IGESSolid_PlaneSurface_HeaderFile
#define _IGESSolid_PlaneSurface_HeaderFile

#include <IGESData_IGESEntity.hxx>
class IGESGeom_Point;
class IGESGeom_Direction;


class IGESSolid_PlaneSurface;
DEFINE_STANDARD_HANDLE(IGESSolid_PlaneSurface, IGESData_IGESEntity)

//! defines PlaneSurface, Type <190> Form Number <0,1>
//! in package IGESSolid
//! A plane surface entity is defined by a point on the
//! surface and a normal to it.
class IGESSolid_PlaneSurface : public IGESData_IGESEntity
{
 public:

  IGESSolid_PlaneSurface() {}
  
  //! This method is used to set the fields of the class
  //! PlaneSurface
  //! - aLocation : the point on the surface
  //! - aNormal   : the surface normal direction
  //! - refdir    : the reference direction (default NULL) for
  //! unparameterised curves
  Standard_EXPORT void Init (const Handle(IGESGeom_Point)& aLocation, const Handle(IGESGeom_Direction)& aNormal, const Handle(IGESGeom_Direction)& refdir);
  
  //! returns the point on the surface
  const Handle(IGESGeom_Point) & LocationPoint() const { return theLocationPoint; }

  //! returns the normal to the surface
  const Handle(IGESGeom_Direction) & Normal() const { return theNormal; }

  //! returns the reference direction (for parameterised curve)
  //! returns NULL for unparameterised curve
  const Handle(IGESGeom_Direction) & ReferenceDir() const { return theRefDir; }

  //! returns True if parameterised, else False
  Standard_Boolean IsParametrised() const { return (!theRefDir.IsNull()); }

  Standard_EXPORT virtual void OwnShared(Interface_EntityIterator &theIter) const Standard_OVERRIDE;

  Standard_EXPORT virtual void OwnCheck (const Interface_ShareTool &, const Handle(Interface_Check) &) const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(IGESSolid_PlaneSurface,IGESData_IGESEntity)

 private:

  Handle(IGESGeom_Point) theLocationPoint;
  Handle(IGESGeom_Direction) theNormal;
  Handle(IGESGeom_Direction) theRefDir;
};

#endif // _IGESSolid_PlaneSurface_HeaderFile
