// Copyright (c) 2015 OPEN CASCADE SAS
//
// This file is part of commercial software by OPEN CASCADE SAS.
//
// This software is furnished in accordance with the terms and conditions
// of the contract and with the inclusion of this copyright notice.
// This software or any other copy thereof may not be provided or otherwise
// be made available to any third party.
// No ownership title to the software is transferred hereby.
//
// OPEN CASCADE SAS makes no representation or warranties with respect to the
// performance of this software, and specifically disclaims any responsibility
// for any damages, special or consequential, connected with its use.

#ifndef ObjDataSource_H
#define ObjDataSource_H

#include <MeshDataSource.h>

//! The DataSource for working with OBJ mesh reader.
class ObjDataSource : public MeshDataSource
{

public:

  //! Empty constructor.
  Standard_EXPORT ObjDataSource();

  //! Read the mesh from specified file.
  Standard_EXPORT virtual Standard_Boolean Read (const TCollection_AsciiString&           theFile,
                                                 const Handle(Message_ProgressIndicator)& theProgress,
                                                 const Standard_Integer                   theIndexLimit,
                                                 const Standard_Integer                   theMemoryLimitMiB) Standard_OVERRIDE;

public:

  DEFINE_STANDARD_RTTIEXT(ObjDataSource, MeshDataSource)

};

DEFINE_STANDARD_HANDLE(ObjDataSource, MeshDataSource)

#endif // ObjDataSource_H
