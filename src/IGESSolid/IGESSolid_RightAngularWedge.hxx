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

#ifndef _IGESSolid_RightAngularWedge_HeaderFile
#define _IGESSolid_RightAngularWedge_HeaderFile

#include <gp_XYZ.hxx>
#include <IGESData_IGESEntity.hxx>
class gp_Pnt;
class gp_Dir;


class IGESSolid_RightAngularWedge;
DEFINE_STANDARD_HANDLE(IGESSolid_RightAngularWedge, IGESData_IGESEntity)

//! defines RightAngularWedge, Type <152> Form Number <0>
//! in package IGESSolid
//! A right angular wedge is a triangular/trapezoidal prism
class IGESSolid_RightAngularWedge : public IGESData_IGESEntity
{
 public:

  IGESSolid_RightAngularWedge() {}
  
  //! This method is used to set the fields of the class
  //! RightAngularWedge
  //! - aSize    : the lengths along the local axes
  //! - lowX     : the length at the smaller X-side
  //! - aCorner  : the corner point coordinates
  //! default (0,0,0)
  //! - anXAxis  : the unit vector defining local X-axis
  //! default (1,0,0)
  //! - anZAxis  : the unit vector defining local Z-axis
  //! default (0,0,1)
  Standard_EXPORT void Init (const gp_XYZ& aSize, const Standard_Real lowX, const gp_XYZ& aCorner, const gp_XYZ& anXAxis, const gp_XYZ& anZAxis);
  
  //! returns the size
  const gp_XYZ & Size() const { return theSize; }

  //! returns the length along the local X-axis
  Standard_Real XBigLength() const { return theSize.X(); }

  //! returns the smaller length along the local X-direction at Y=LY
  Standard_Real XSmallLength() const { return theXSmallLength; }

  //! returns the length along the local Y-axis
  Standard_Real YLength() const { return theSize.Y(); }

  //! returns the length along the local Z-axis
  Standard_Real ZLength() const { return theSize.Z(); }

  //! returns the corner point coordinates
  Standard_EXPORT gp_Pnt Corner() const;
  
  //! returns the corner point coordinates after applying
  //! TransformationMatrix
  Standard_EXPORT gp_Pnt TransformedCorner() const;
  
  //! returns the direction defining the local X-axis
  Standard_EXPORT gp_Dir XAxis() const;
  
  //! returns the direction defining the local X-axis
  //! after applying the TransformationMatrix
  Standard_EXPORT gp_Dir TransformedXAxis() const;
  
  //! returns the direction defining the local Y-axis
  //! it is got by taking the cross product of ZAxis and XAxis
  Standard_EXPORT gp_Dir YAxis() const;
  
  //! returns the direction defining the local Y-axis
  //! after applying the TransformationMatrix
  Standard_EXPORT gp_Dir TransformedYAxis() const;
  
  //! returns the direction defining the local Z-axis
  Standard_EXPORT gp_Dir ZAxis() const;
  
  //! returns the direction defining the local Z-axis
  //! after applying the TransformationMatrix
  Standard_EXPORT gp_Dir TransformedZAxis() const;

  Standard_EXPORT virtual void OwnCheck (const Interface_ShareTool &, const Handle(Interface_Check) &) const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(IGESSolid_RightAngularWedge,IGESData_IGESEntity)

 private:

  gp_XYZ theSize;
  Standard_Real theXSmallLength;
  gp_XYZ theCorner;
  gp_XYZ theXAxis;
  gp_XYZ theZAxis;
};

#endif // _IGESSolid_RightAngularWedge_HeaderFile
