// Created on: 2000-05-24
// Created by: Edward AGAPOV
// Copyright (c) 2000-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and / or modify it
// under the terms of the GNU Lesser General Public version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#include <XCAFDrivers.hxx>
#include <XCAFDrivers_DocumentRetrievalDriver.hxx>

#include <StdDrivers.hxx>

#include <StdLPersistent.hxx>
#include <StdPersistent.hxx>
#include <ShapePersistent.hxx>
#include <XCAFPersistent.hxx>

#include <Standard_Failure.hxx>
#include <Standard_GUID.hxx>
#include <Plugin_Macro.hxx>

static Standard_GUID XSRetrievalDriver ("ed8793f9-3142-11d4-b9b5-0060b0ee281b");

//=======================================================================
//function : Factory
//purpose  : Depending from the ID, returns a list of storage
//           or retrieval attribute drivers. Used for plugin
//=======================================================================
Handle(Standard_Transient) XCAFDrivers::Factory (const Standard_GUID& aGUID)
{
  if (aGUID == XSRetrievalDriver) 
  {  
#ifdef OCCT_DEBUG
    cout << "XCAFDrivers : Retrieval Plugin" << endl;
#endif

    static Handle (XCAFDrivers_DocumentRetrievalDriver) model_rd = new XCAFDrivers_DocumentRetrievalDriver;
    return model_rd;
  }
  
  return StdDrivers::Factory (aGUID);
}

//=======================================================================
//function : BindTypes
//purpose  : Register types
//=======================================================================
void XCAFDrivers::BindTypes (StdObjMgt_MapOfInstantiators& theMap)
{
  StdLPersistent ::BindTypes (theMap);
  StdPersistent  ::BindTypes (theMap);
  ShapePersistent::BindTypes (theMap);
  XCAFPersistent ::BindTypes (theMap);
}


#ifdef _MSC_VER
#pragma warning(disable:4190) /* disable warning on C++ type returned by C function; should be OK for C++ usage */
#endif

// Declare entry point PLUGINFACTORY
PLUGIN(XCAFDrivers)
