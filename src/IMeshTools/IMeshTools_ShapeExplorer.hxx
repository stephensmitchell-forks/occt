// Created on: 2016-04-07
// Copyright (c) 2016 OPEN CASCADE SAS
// Created by: Oleg AGASHIN
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

#ifndef _IMeshTools_ShapeExplorer_HeaderFile
#define _IMeshTools_ShapeExplorer_HeaderFile

#include <IMeshData_Shape.hxx>
#include <Standard_Type.hxx>
#include <IMeshTools_ShapeVisitor.hxx>
#include <TopoDS_Shape.hxx>

//! Interface class for explores of TopoDS_Shape.
class IMeshTools_ShapeExplorer : public IMeshData_Shape
{
public:

  //! Destructor.
  Standard_EXPORT virtual ~IMeshTools_ShapeExplorer()
  {
  }

  //! Starts exploring of a shape.
  Standard_EXPORT virtual void Accept (const Handle (IMeshTools_ShapeVisitor)& theVisitor) = 0;

  DEFINE_STANDARD_RTTI_INLINE(IMeshTools_ShapeExplorer, IMeshData_Shape)

protected:

  //! Constructor.
  Standard_EXPORT IMeshTools_ShapeExplorer (const TopoDS_Shape& theShape)
    : IMeshData_Shape(theShape)
  {
  }
};

#endif