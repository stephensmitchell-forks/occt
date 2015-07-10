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

#ifndef _Graphic3d_TransferFunctionFilter_HeaderFile
#define _Graphic3d_TransferFunctionFilter_HeaderFile

#include <Graphic3d_TransferFunctionNode.hxx>

//! Transfer function filter used for generating discretization table.
template<typename PointType, typename ValueType>
class Graphic3d_TransferFunctionFilter
{
public:

  //! Type of control point.
  typedef Graphic3d_TransferFunctionNode<PointType, ValueType> Node;

public:

  //! Initializes internal structures.
  virtual void Init (const Node*            theNodes,
                     const Standard_Integer theCount)
  {
    myNodes = theNodes;
    myCount = theCount;
  }

  //! Returns interpolated value at the given point.
  virtual ValueType Value (const PointType thePoint) const = 0;

protected:

  //! Array of control points.
  const Node* myNodes;

  //! Total number of control points.
  Standard_Integer myCount;
};

#endif // _Graphic3d_TransferFunctionFilter_HeaderFile
