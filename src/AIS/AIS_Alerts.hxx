// Created on:
// Created by:
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

#ifndef _AIS_Alerts_HeaderFile
#define _AIS_Alerts_HeaderFile

#include <TopoDS_AlertWithShape.hxx>
#include <Message_AlertWithObject.hxx>

//! Some text information
DEFINE_SIMPLE_ALERT (AIS_AlertInformation)

////! Some shapes information
DEFINE_ALERT_WITH_SHAPE (AIS_AlertShapeInformation)

////! Some transient object information
DEFINE_ALERT_WITH_OBJECT (AIS_AlertObjectInformation)

#endif // _AIS_Alerts_HeaderFile
