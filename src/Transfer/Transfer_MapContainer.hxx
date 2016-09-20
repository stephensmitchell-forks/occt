// Created on: 2001-09-10
// Created by: Sergey KUUL
// Copyright (c) 2001-2014 OPEN CASCADE SAS
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

#ifndef _Transfer_MapContainer_HeaderFile
#define _Transfer_MapContainer_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <MMgt_TShared.hxx>
#include <NCollection_DataMap.hxx>

typedef NCollection_DataMap<Handle(Standard_Transient),Handle(Standard_Transient)> Transfer_DataMapOfTransient;

class Transfer_MapContainer : public MMgt_TShared
{
 public:

  Transfer_MapContainer() {}
  
  Standard_Boolean Find(const Handle(Standard_Transient) &theKey, Handle(Standard_Transient) &theValue)
  { return myMapObj.Find(theKey,theValue); }

  Standard_Boolean Bind(const Handle(Standard_Transient) &theKey, const Handle(Standard_Transient) &theValue)
  { return myMapObj.Bind(theKey,theValue); }

  DEFINE_STANDARD_RTTIEXT(Transfer_MapContainer,MMgt_TShared)

 private:

  Transfer_DataMapOfTransient myMapObj;
};

#endif // _Transfer_MapContainer_HeaderFile
