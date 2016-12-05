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

#ifndef _IGESSolid_Sphere_HeaderFile
#define _IGESSolid_Sphere_HeaderFile

#include <gp_XYZ.hxx>
#include <IGESData_IGESEntity.hxx>
class gp_Pnt;


class IGESSolid_Sphere;
DEFINE_STANDARD_HANDLE(IGESSolid_Sphere, IGESData_IGESEntity)

//! defines Sphere, Type <158> Form Number <0>
//! in package IGESSolid
//! This defines a sphere with a center and radius
class IGESSolid_Sphere : public IGESData_IGESEntity
{
 public:

  IGESSolid_Sphere() {}
  
  //! This method is used to set the fields of the class Sphere
  //! - aRadius : the radius of the sphere
  //! - aCenter : the center point coordinates (default (0,0,0))
  Standard_EXPORT void Init (const Standard_Real aRadius, const gp_XYZ& aCenter);
  
  //! returns the radius of the sphere
  Standard_Real Radius() const { return theRadius; }

  //! returns the center of the sphere
  Standard_EXPORT gp_Pnt Center() const;
  
  //! returns the center of the sphere after applying
  //! TransformationMatrix
  Standard_EXPORT gp_Pnt TransformedCenter() const;

  Standard_EXPORT virtual void OwnCheck (const Interface_ShareTool &, const Handle(Interface_Check) &) const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(IGESSolid_Sphere,IGESData_IGESEntity)

 private:

  Standard_Real theRadius;
  gp_XYZ theCenter;
};

#endif // _IGESSolid_Sphere_HeaderFile
