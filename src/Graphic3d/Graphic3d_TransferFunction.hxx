// Created on: 2015-01-26
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

#ifndef _Graphic3d_TransferFunction_HeaderFile
#define _Graphic3d_TransferFunction_HeaderFile

#include <Image_PixMap.hxx>

//! Defines abstract transfer function that maps values
//! of scalar field to specific RGB color and opacity.
class Graphic3d_TransferFunction : public Standard_Transient
{
public:

  //! Default size of discretization table.
  static const Standard_Integer SIZE_OF_TABLE = 512;

public:

  //! Creates empty transfer function.
  Graphic3d_TransferFunction() : myState (1)
  {
    //
  }

  //! Releases resources of transfer function.
  virtual ~Graphic3d_TransferFunction()
  {
    //
  }

  //! Returns current state of transfer function.
  Standard_Size State() const
  {
    return myState;
  }

  //! Updates current state of transfer function.
  void Update()
  {
    ++myState;
  }

  //! Computes discretization table as 1D image.
  virtual Handle(Image_PixMap) ImageTable (Standard_Real&         theNormMinValue,
                                           Standard_Real&         theNormMaxValue,
                                           const Standard_Integer theNumberOfRows = SIZE_OF_TABLE) const = 0;

protected:

  //! Current state of transfer function.
  Standard_Size myState;

public:

  DEFINE_STANDARD_RTTI (Graphic3d_TransferFunction, Standard_Transient)

};

DEFINE_STANDARD_HANDLE (Graphic3d_TransferFunction, Standard_Transient)

#endif // _Graphic3d_TransferFunction_HeaderFile
