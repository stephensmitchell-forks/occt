// Created on: 1997-08-07
// Created by: Jean-Louis Frenkel
// Copyright (c) 1997-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#include <PCDM_RetrievalDriver.ixx>
#include <PCDM_BaseDriverPointer.hxx>
#include <PCDM.hxx>
#include <PCDM_TypeOfFileDriver.hxx>
#include <Storage_Data.hxx>
#include <Storage_HeaderData.hxx>
#include <Storage_Schema.hxx>
#include <Storage_Root.hxx>
#include <Storage_IODevice.hxx>
#include <TCollection_AsciiString.hxx>

#include <TColStd_SequenceOfAsciiString.hxx>
#include <Standard_TypeMismatch.hxx>

#include <PCDM_ReadWriter.hxx>
#include <Resource_Manager.hxx>
#include <Standard_ErrorHandler.hxx>
#include <UTL.hxx>
#include <PCDM.hxx>
#include <Storage_HSeqOfRoot.hxx>
#include <locale.h>

void PCDM_RetrievalDriver::RaiseIfUnknownTypes(const Handle(Storage_Schema)& aSchema, const Handle(Storage_IODevice)& aDevice) {

  PCDM_BaseDriverPointer theFileDriver;
  //  TCollection_AsciiString aFileNameU(aFileName);
  if(PCDM::FileDriverType(aDevice, theFileDriver) == PCDM_TOFD_Unknown)
    return;
  
  PCDM_ReadWriter::Open(*theFileDriver, aDevice, Storage_VSRead);
  
  TColStd_SequenceOfAsciiString theUnknownTypes;
  Standard_Boolean unknowns = aSchema->HasUnknownType(*theFileDriver,theUnknownTypes);

  theFileDriver->Close();
  delete theFileDriver;

  if(unknowns) {
    Standard_SStream aMsg; aMsg << "cannot read: `" << aDevice << "', because the following types: ";
    for (Standard_Integer i=1; i <= theUnknownTypes.Length(); i++) {
      aMsg << theUnknownTypes(i);
      if(i< theUnknownTypes.Length()) aMsg <<",";
    }
    aMsg << " have been found in it but not in the available Schema: " << aSchema->Name() << (char)0;
    Standard_TypeMismatch::Raise(aMsg);
  }
}

//=======================================================================
//function : Read
//purpose  : 
//=======================================================================

void PCDM_RetrievalDriver::Read(const Handle(Storage_IODevice)& aDevice,
				const Handle(CDM_Document)& aNewDocument,
				const Handle(CDM_Application)& theApplication)
{
  Standard_SStream aMsg;
  Standard_Boolean Failure=Standard_False;
  Handle(Storage_Schema) aSchema;
  TColStd_SequenceOfExtendedString theExtensions;
  myReaderStatus = PCDM_RS_OK;
  {
    try {
      OCC_CATCH_SIGNALS
      aSchema=PCDM::Schema(SchemaName(),theApplication);
      Extensions(aDevice, theExtensions, theApplication->MessageDriver());
      LoadExtensions(aSchema,theExtensions, theApplication->MessageDriver());
    } 
    catch (Standard_NoSuchObject) {
      aMsg << Standard_NoSuchObject::Caught() << endl;
      myReaderStatus = PCDM_RS_NoSchema;
      Failure=Standard_True;
    }
    catch (Standard_Failure) {
      aMsg << Standard_Failure::Caught() << endl;
      myReaderStatus = PCDM_RS_ExtensionFailure;
      Failure=Standard_True;
    }
    if(Failure) Standard_Failure::Raise(aMsg);
  }

  PCDM_BaseDriverPointer theFileDriver;
  //  TCollection_AsciiString aFileNameU(theFileName);
  if(PCDM::FileDriverType(aDevice, theFileDriver) == PCDM_TOFD_Unknown) {
    myReaderStatus = PCDM_RS_UnknownFileDriver;
    return;
  }
  {
    try {
      OCC_CATCH_SIGNALS
      RaiseIfUnknownTypes(aSchema, aDevice);
      PCDM_ReadWriter::Open(*theFileDriver, aDevice, Storage_VSRead);
    } 
    catch (Standard_TypeMismatch) {
      aMsg << Standard_TypeMismatch::Caught() << endl;
      myReaderStatus = PCDM_RS_TypeNotFoundInSchema;
      Failure=Standard_True;
    }
    catch (Standard_Failure) {
      aMsg << Standard_Failure::Caught() << endl;
      myReaderStatus = PCDM_RS_OpenError;
      Failure=Standard_True;
    }
    if(Failure) Standard_Failure::Raise(aMsg);
  } 
  
  Handle(Storage_Data) theData = aSchema->Read(*theFileDriver);
  Storage_Error aStatus = theData->ErrorStatus();
  if(aStatus != Storage_VSOk) {
    switch (aStatus) {
    case Storage_VSOpenError:
    case Storage_VSNotOpen:
    case Storage_VSAlreadyOpen:
      myReaderStatus = PCDM_RS_OpenError;
      aMsg << "Stream Open Error" << endl;
      break;
    case Storage_VSModeError:
      myReaderStatus = PCDM_RS_WrongStreamMode;
      aMsg << "Stream is opened with a wrong mode for operation" << endl;
      break;
    case Storage_VSSectionNotFound:
      aMsg << "Section is not found" << endl;
      myReaderStatus = PCDM_RS_FormatFailure;
      break;
    case Storage_VSFormatError:
      myReaderStatus = PCDM_RS_FormatFailure;
      aMsg << "Wrong format error" << endl;
      break;
    case Storage_VSUnknownType:
      myReaderStatus = PCDM_RS_TypeFailure;
      aMsg << "Try to read an unknown type" << endl;
      break;
    case Storage_VSTypeMismatch:
      myReaderStatus = PCDM_RS_TypeFailure;
      aMsg << "Try to read a wrong primitive type" << endl;
      break;
    default:
      myReaderStatus = PCDM_RS_DriverFailure;
      aMsg << "Retrieval Driver Failure" << endl;
    }
    Standard_Failure::Raise(aMsg);    
  }

  theFileDriver->Close();
  delete theFileDriver;
  
  Handle(Storage_HSeqOfRoot) theDocuments=theData->Roots();
  
  Handle(Storage_Root) theDocument;
  for(Standard_Integer i=1; i<=theDocuments->Length() && theDocument.IsNull(); i++) {
    theDocument=theDocuments->Value(i);
  }
  
  if(theDocument.IsNull()) {
    Standard_SStream aMsg;
    aMsg << "could not find any document in this file" << (char) 0;
    myReaderStatus = PCDM_RS_NoDocument;
    Standard_Failure::Raise(aMsg);
  }
  Handle(Standard_Persistent) theObject=theDocument->Object();
  Handle(PCDM_Document) thePDocument = *((Handle(PCDM_Document)*)&theObject);


  {
    try {
      OCC_CATCH_SIGNALS
      Make(thePDocument,aNewDocument);
    }
    catch (Standard_Failure) {
      aMsg << "Error during Make: ";
      aMsg << Standard_Failure::Caught() << (char)0;
      myReaderStatus = PCDM_RS_MakeFailure;
      Failure=Standard_True;
    }
  }

// read comments
  aNewDocument->SetComments(theData->Comments());

  PCDM_DriverError_Raise_if(Failure,aMsg);
}

void PCDM_RetrievalDriver::References(const Handle(Storage_IODevice)& aDevice, PCDM_SequenceOfReference& theReferences, const Handle(CDM_MessageDriver)& theMsgDriver) {

  PCDM_ReadWriter::Reader(aDevice)->ReadReferences(aDevice, theReferences, theMsgDriver);
}

void PCDM_RetrievalDriver::Extensions(const Handle(Storage_IODevice)& aDevice, TColStd_SequenceOfExtendedString& theExtensions,  const Handle(CDM_MessageDriver)& theMsgDriver) {
  
  PCDM_ReadWriter::Reader(aDevice)->ReadExtensions(aDevice,theExtensions, theMsgDriver);
}

Standard_Integer PCDM_RetrievalDriver::DocumentVersion(const Handle(Storage_IODevice)& aDevice, const Handle(CDM_MessageDriver)& theMsgDriver) {

  return PCDM_ReadWriter::Reader(aDevice)->ReadDocumentVersion(aDevice, theMsgDriver);
}

Standard_Integer PCDM_RetrievalDriver::ReferenceCounter(const Handle(Storage_IODevice)& aDevice, const Handle(CDM_MessageDriver)& theMsgDriver) {

  return PCDM_ReadWriter::Reader(aDevice)->ReadReferenceCounter(aDevice, theMsgDriver);
}

void PCDM_RetrievalDriver::LoadExtensions(const Handle(Storage_Schema)& , const TColStd_SequenceOfExtendedString& , const Handle(CDM_MessageDriver)&)
{
}

//=======================================================================
//function : Format
//purpose  : 
//=======================================================================

void PCDM_RetrievalDriver::SetFormat (const TCollection_ExtendedString& aformat)
{
  myFormat = aformat;
}


//=======================================================================
//function : Format
//purpose  : 
//=======================================================================

TCollection_ExtendedString PCDM_RetrievalDriver::GetFormat () const
{
  return myFormat;
}
