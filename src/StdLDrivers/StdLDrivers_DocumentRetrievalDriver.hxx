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

#ifndef _StdLDrivers_DocumentRetrievalDriver_HeaderFile
#define _StdLDrivers_DocumentRetrievalDriver_HeaderFile

#include <PCDM_RetrievalDriver.hxx>
#include <Storage_Error.hxx>

class StdObjMgt_MapOfInstantiators;

//! retrieval driver of a Part document
class StdLDrivers_DocumentRetrievalDriver : public PCDM_RetrievalDriver
{
public:
  //! Create an empty TDocStd_Document.
  Standard_EXPORT virtual Handle(CDM_Document) CreateDocument() Standard_OVERRIDE;

  //! Retrieve the content of a file into a new document.
  Standard_EXPORT virtual void Read (const TCollection_ExtendedString& theFileName,
                                     const Handle(CDM_Document)&       theNewDocument,
                                     const Handle(CDM_Application)&    theApplication) Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT (StdLDrivers_DocumentRetrievalDriver, PCDM_RetrievalDriver)

private:
  //! Update the reader status and raise an exception appropriate for the given storage error.
  Standard_EXPORT Standard_Boolean RaiseOnStorageError (Storage_Error theError);

  //! Register types.
  Standard_EXPORT virtual void BindTypes (StdObjMgt_MapOfInstantiators& theMap);
};

#endif // _StdLDrivers_DocumentRetrievalDriver_HeaderFile
