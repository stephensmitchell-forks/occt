// Created on: 2015-01-28
// Created by: Denis BOGOLEPOV
// Copyright (c) 2015 OPEN CASCADE SAS
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

#ifndef _Graphic3d_TransferFunctionNearestFilter_HeaderFile
#define _Graphic3d_TransferFunctionNearestFilter_HeaderFile

#include <Graphic3d_TransferFunctionFilter.hxx>

//! Nearest filter used for generating discretization table.
template<typename PointType, typename ValueType>
class Graphic3d_TransferFunctionNearestFilter : public Graphic3d_TransferFunctionFilter<PointType, ValueType>
{
public:

  //! Returns nearest value at the given point.
  virtual ValueType Value (const PointType thePoint) const Standard_OVERRIDE;
};

#include <Graphic3d_TransferFunctionNearestFilter.lxx>

#endif // _Graphic3d_TransferFunctionNearestFilter_HeaderFile