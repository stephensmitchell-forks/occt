// Created on: 2014-12-25
// Created by: Ilya Sevrikov
// Copyright (c) 2013-2014 OPEN CASCADE SAS
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

#ifndef _Graphic3d_TransferFunctionNode_HeaderFile
#define _Graphic3d_TransferFunctionNode_HeaderFile

#include <Graphic3d_Vec.hxx>

//! Describes control point (knot) of the transfer function.
template<class PointType, class ValueType>
struct Graphic3d_TransferFunctionNode
{
  //! Position of control point.
  PointType Point;

  //! Property at the control point.
  ValueType Value;

  //! Creates unitialized control point.
  Graphic3d_TransferFunctionNode()
  {
    //
  }

  //! Creates new control point.
  Graphic3d_TransferFunctionNode (const PointType thePoint,
                                  const ValueType theValue)
  : Point (thePoint),
    Value (theValue)
  {
    //
  }
};

#endif // _Graphic3d_TransferFunctionNode_HeaderFile
