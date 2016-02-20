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

#ifndef ObjDataWriter_H
#define ObjDataWriter_H

#include <MeshDataSource.h>

#include <AIS_ListOfInteractive.hxx>

//! Mesh writer to the OBJ format.
class ObjDataWriter : public Standard_Transient
{

public:

  //! Empty constructor.
  Standard_EXPORT ObjDataWriter();

  //! Write the mesh into specified file.
  Standard_EXPORT Standard_Boolean Write (const Handle(MeshDataSource)&            theDataSrc,
                                          const TCollection_AsciiString&           theFile,
                                          const Handle(Message_ProgressIndicator)& theProgress);

  //! Write the mesh presentation of the model into specified file.
  Standard_EXPORT Standard_Boolean Write (const AIS_ListOfInteractive&             thePrsList,
                                          const TCollection_AsciiString&           theFile,
                                          const Handle(Message_ProgressIndicator)& theProgress);

public:

  DEFINE_STANDARD_RTTIEXT(ObjDataWriter, Standard_Transient)

};

DEFINE_STANDARD_HANDLE(ObjDataWriter, Standard_Transient)

#endif // ObjDataWriter_H
