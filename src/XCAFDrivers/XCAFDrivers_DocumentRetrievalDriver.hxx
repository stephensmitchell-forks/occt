// Copyright (c) 2016 OPEN CASCADE SAS
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

#ifndef _XCAFDrivers_DocumentRetrievalDriver_HeaderFile
#define _XCAFDrivers_DocumentRetrievalDriver_HeaderFile

#include <StdDrivers_DocumentRetrievalDriver.hxx>

//! retrieval driver of a Part document
class XCAFDrivers_DocumentRetrievalDriver : public StdDrivers_DocumentRetrievalDriver
{
public:
  DEFINE_STANDARD_RTTIEXT (XCAFDrivers_DocumentRetrievalDriver, StdDrivers_DocumentRetrievalDriver)

protected:
  //! Register types.
  Standard_EXPORT virtual void bindTypes (StdObjMgt_MapOfInstantiators& theMap) Standard_OVERRIDE;
};

#endif // _XCAFDrivers_DocumentRetrievalDriver_HeaderFile
