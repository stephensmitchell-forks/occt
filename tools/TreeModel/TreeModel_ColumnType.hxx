// Created on: 2017-06-16
// Created by: Natalia ERMOLAEVA
// Copyright (c) 2017 OPEN CASCADE SAS
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

#ifndef TreeModel_ColumnType_H
#define TreeModel_ColumnType_H

#include <Standard_WarningsDisable.hxx>
#include <Qt>
#include <Standard_WarningsRestore.hxx>

//! Sets custom item role of Tree view wmodel
enum TreeModel_ColumnType
{
  TreeModel_ColumnType_Name = 0, //! name column
  TreeModel_ColumnType_Visibility //! visibility state column
};

#endif
