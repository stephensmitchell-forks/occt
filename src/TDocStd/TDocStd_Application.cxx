// Created on: 1999-06-30
// Created by: Denis PASCAL
// Copyright (c) 1999-1999 Matra Datavision
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

#include <TDocStd_Application.ixx>

#include <CDF_Session.hxx>
#include <CDF_DirectoryIterator.hxx>
#include <CDF_Directory.hxx>

#include <Standard_ErrorHandler.hxx>
#include <Standard_NotImplemented.hxx>
#include <Standard_DomainError.hxx>
#include <Storage_File.hxx>
#include <Storage_IStream.hxx>
#include <Storage_OStream.hxx>
#include <Plugin_Failure.hxx>
#include <TDF_Label.hxx>
#include <CDF_Store.hxx>
#include <TDocStd_PathParser.hxx>

#include <CDM_MessageDriver.hxx>

// TDocStd_Owner attribute have pointer of closed TDocStd_Document

#include <TDocStd_Owner.hxx>

//=======================================================================
//function : TDocStd_Application
//purpose  :
//=======================================================================

TDocStd_Application::TDocStd_Application()
     : myIsDriverLoaded (Standard_True)
{
  Handle(CDF_Session) S;
  if (!CDF_Session::Exists()) S = new CDF_Session();
  else S = CDF_Session::CurrentSession();
  S->SetCurrentApplication(this);
  try
  {
    OCC_CATCH_SIGNALS
    S->LoadDriver();
  }
  catch (Plugin_Failure)
  {
    myIsDriverLoaded = Standard_False;
  }
}


//=======================================================================
//function : IsDriverLoaded
//purpose  :
//=======================================================================
Standard_Boolean TDocStd_Application::IsDriverLoaded() const
{
  return myIsDriverLoaded;
}

//=======================================================================
//function : Resources
//purpose  :
//=======================================================================

Handle(Resource_Manager)  TDocStd_Application::Resources()  {
  if (myResources.IsNull()) {
    myResources = new Resource_Manager(ResourcesName());
  }
  return myResources;
}


//=======================================================================
//function : NbDocuments
//purpose  :
//=======================================================================

Standard_Integer TDocStd_Application::NbDocuments() const
{
  if (!CDF_Session::Exists())
    Standard_DomainError::Raise("TDocStd_Application::NbDocuments");
  Handle(CDF_Session) S = CDF_Session::CurrentSession();
  return S->Directory()->Length();
}

//=======================================================================
//function : GetDocument
//purpose  :
//=======================================================================

void TDocStd_Application::GetDocument(const Standard_Integer index,Handle(TDocStd_Document)& aDoc) const
{
  if (!CDF_Session::Exists())
    Standard_DomainError::Raise("TDocStd_Application::NbDocuments");
  Handle(CDF_Session) S = CDF_Session::CurrentSession();
  CDF_DirectoryIterator it (S->Directory());
  Standard_Integer current = 0;
  for (;it.MoreDocument();it.NextDocument()) {
    current++;
    if (index == current) {
      Handle(TDocStd_Document) D =
        Handle(TDocStd_Document)::DownCast(it.Document());
      aDoc = D;
      return;
    }
  }
}

//=======================================================================
//function : NewDocument
//purpose  :
//=======================================================================

void TDocStd_Application::NewDocument(const TCollection_ExtendedString& format,Handle(TDocStd_Document)& aDoc)
{
  Handle(TDocStd_Document) D = new TDocStd_Document(format);
  InitDocument (D);
  CDF_Application::Open(D); // add the document in the session
  aDoc = D;
}

//=======================================================================
//function : InitDocument
//purpose  : do nothing
//=======================================================================

void TDocStd_Application::InitDocument(const Handle(TDocStd_Document)& /*aDoc*/) const
{
}

//=======================================================================
//function : Close
//purpose  :
//=======================================================================

void TDocStd_Application::Close(const Handle(TDocStd_Document)& aDoc)
{
  if (aDoc.IsNull())
  {
    return;
  }

  Handle(TDocStd_Owner) Owner;
  if (aDoc->Main().Root().FindAttribute(TDocStd_Owner::GetID(),Owner)) {
    Handle(TDocStd_Document) emptyDoc;
    Owner->SetDocument(emptyDoc);
  }
  aDoc->BeforeClose();
  CDF_Application::Close(aDoc);
}

//=======================================================================
//function : IsInSession
//purpose  :
//=======================================================================

Standard_Integer TDocStd_Application::IsInSession (const TCollection_ExtendedString& path) const
{
    TCollection_ExtendedString unifiedPath(path);
    unifiedPath.ChangeAll('/', '|');
    unifiedPath.ChangeAll('\\', '|');

    Standard_Integer nbdoc = NbDocuments();
    Handle(TDocStd_Document) D;
    for (Standard_Integer i = 1; i <= nbdoc; i++) 
    {
        GetDocument(i,D);
        if (D->IsSaved()) 
        {
            TCollection_ExtendedString unifiedDocPath(D->GetPath());
            unifiedDocPath.ChangeAll('/', '|');
            unifiedDocPath.ChangeAll('\\', '|');

            if (unifiedPath == unifiedDocPath) 
                return i;
        }
    }
    return 0;
}

PCDM_ReaderStatus TDocStd_Application::Open(const TCollection_ExtendedString& thePath,
                                            Handle(TDocStd_Document)& theDoc)
{
  Handle(Storage_File) aDevice = new Storage_File(thePath);
  return Open (aDevice, theDoc);
}

//=======================================================================
//function : Open
//purpose  :
//=======================================================================

PCDM_ReaderStatus TDocStd_Application::Open(Handle(Storage_IODevice)& aDevice,
                                            Handle(TDocStd_Document)& aDoc)
{
  PCDM_ReaderStatus status = CanRetrieve(aDevice);
  if (status != PCDM_RS_OK)
    return status;
  try {
    OCC_CATCH_SIGNALS
    Handle(TDocStd_Document) D =
      Handle(TDocStd_Document)::DownCast(Retrieve(aDevice));
    CDF_Application::Open(D);
    aDoc = D;
  }
  catch (Standard_Failure) {
//    status = GetRetrieveStatus();
    Handle(Standard_Failure) F = Standard_Failure::Caught();
    if (!F.IsNull() && !MessageDriver().IsNull()) {
      TCollection_ExtendedString aString (F->GetMessageString());
      MessageDriver()->Write(aString.ToExtString());
    }
  }
  status = GetRetrieveStatus();
#ifdef OCCT_DEBUG
  cout<<"TDocStd_Application::Open(): The status = "<<status<<endl;
#endif
  return status;
}

//=======================================================================
//function : Open
//purpose  :
//=======================================================================

PCDM_ReaderStatus TDocStd_Application::Open( Standard_IStream& theIStream,
                                             Handle(TDocStd_Document)& aDoc )
{
  Handle(Storage_IStream) aDevice = new Storage_IStream(theIStream);
  return Open (aDevice, aDoc);
}

//=======================================================================
//function : SaveAs
//purpose  :
//=======================================================================

PCDM_StoreStatus TDocStd_Application::SaveAs(const Handle(TDocStd_Document)& theDoc,
                                             const TCollection_ExtendedString& thePath)
{
  Handle(Storage_File) aDevice = new Storage_File(thePath);
  return SaveAs (theDoc, aDevice);
}

//=======================================================================
//function : SaveAs
//purpose  :
//=======================================================================

PCDM_StoreStatus TDocStd_Application::SaveAs(const Handle(TDocStd_Document)& D,
                                             const Handle(Storage_IODevice)& aDevice)
{
  D->Open(this);
  CDF_Store storer (D);
  if (!storer.SetDevice(aDevice))
  {
    TCollection_ExtendedString aMsg ("TDocStd_Application::SaveAs() - device ");
    aMsg += " not valid";
    if(!MessageDriver().IsNull())
      MessageDriver()->Write(aMsg.ToExtString());
    return storer.StoreStatus(); //CDF_SS_Failure;
  }
  //  storer.SetName (file);
  try {
    OCC_CATCH_SIGNALS
    storer.Realize();
  }
  catch (Standard_Failure) {
    Handle(Standard_Failure) F = Standard_Failure::Caught();
    if (!F.IsNull() && !MessageDriver().IsNull()) {
      TCollection_ExtendedString aString (F->GetMessageString());
      MessageDriver()->Write(aString.ToExtString());
    }
  }
  if(storer.StoreStatus() == PCDM_SS_OK)
    D->SetSaved();
#ifdef OCCT_DEBUG
  cout<<"TDocStd_Application::SaveAs(): The status = "<<storer.StoreStatus()<<endl;
#endif
  return storer.StoreStatus();
}

//=======================================================================
//function : Save
//purpose  :
//=======================================================================


PCDM_StoreStatus TDocStd_Application::SaveAs(const Handle(TDocStd_Document)& theDoc,
                                             Standard_OStream& theOStream)
{
  Handle(Storage_OStream) aDevice = new Storage_OStream (theOStream);
  return SaveAs (theDoc, aDevice);
}

//=======================================================================
//function : Save
//purpose  :
//=======================================================================

PCDM_StoreStatus TDocStd_Application::Save (const Handle(TDocStd_Document)& D) {
  PCDM_StoreStatus status = PCDM_SS_OK;
  if (D->IsSaved()) {
    CDF_Store storer (D);
    try{
      OCC_CATCH_SIGNALS
      storer.Realize();
    }
    catch (Standard_Failure) {
      Handle(Standard_Failure) F = Standard_Failure::Caught();
      if (!F.IsNull() && !MessageDriver().IsNull()) {
        TCollection_ExtendedString aString (F->GetMessageString());
        MessageDriver()->Write(aString.ToExtString());
      }
    }
    if(storer.StoreStatus() == PCDM_SS_OK)
      D->SetSaved();
    status = storer.StoreStatus();
  } else {
    if(!MessageDriver().IsNull()) {
      TCollection_ExtendedString aMsg("Document has not been saved yet");
      MessageDriver()->Write(aMsg.ToExtString());
    }
    status = PCDM_SS_Failure;
  }
#ifdef OCCT_DEBUG
  cout<<"TDocStd_Application::Save(): The status = "<<status<<endl;
#endif
  return status;
}

//=======================================================================
//function : SetViewer
//purpose  :
//=======================================================================

// void TDocStd_Application::SetViewer(const Handle(TDocStd_Document)& D,
//                                  const Handle(V3d_Viewer)& viewer)
// {
//   TPrsStd_AISViewer::New (D->Main(),viewer);
//   InitViewer(D);
// }


//=======================================================================
//function : SetViewer
//purpose  :
//=======================================================================

// void TDocStd_Application::SetViewer(const Handle(TDocStd_Document)& D,
//                                  const Handle(AIS_InteractiveContext)& IC)
// {
//   TPrsStd_AISViewer::New (D->Main(),IC);
//   InitViewer(D);
// }


//=======================================================================
//function : SaveAs
//purpose  : 
//=======================================================================

PCDM_StoreStatus TDocStd_Application::SaveAs(const Handle(TDocStd_Document)& D,
                                             const Handle(Storage_IODevice)& aDev,
                                             TCollection_ExtendedString& theStatusMessage)
{ 
  PCDM_StoreStatus aStatus = PCDM_SS_Failure;
  /*
  TDocStd_PathParser tool (path);
  TCollection_ExtendedString directory = tool.Trek();   
  TCollection_ExtendedString file = tool.Name();   
  file+=".";   
  file+=tool.Extension();
  */
  D->Open(this);
  CDF_Store storer (D);  
  if (storer.SetDevice(aDev)) {
    //    storer.SetName (file);
    try {
      OCC_CATCH_SIGNALS
      storer.Realize();
    }
    catch (Standard_Failure) {
      Handle(Standard_Failure) F = Standard_Failure::Caught();
      if (!F.IsNull() && !MessageDriver().IsNull()) {
        TCollection_ExtendedString aString (F->GetMessageString());
        MessageDriver()->Write(aString.ToExtString());
      }
    }
    if(storer.StoreStatus() == PCDM_SS_OK)
      D->SetSaved();
    theStatusMessage = storer.AssociatedStatusText();
    aStatus = storer.StoreStatus();
  } else {
    theStatusMessage =
      TCollection_ExtendedString("TDocStd_Application::SaveAs: No valid device ");
    aStatus = PCDM_SS_Failure;
  }
  return aStatus;
}

//=======================================================================
//function : Save
//purpose  : 
//=======================================================================

PCDM_StoreStatus TDocStd_Application::Save (const Handle(TDocStd_Document)& D,
					   TCollection_ExtendedString& theStatusMessage)
{  
  PCDM_StoreStatus status = PCDM_SS_OK;
  if (D->IsSaved()) {  
    CDF_Store storer (D);  
    try {
      OCC_CATCH_SIGNALS
      storer.Realize(); 
    }
    catch (Standard_Failure) {
      Handle(Standard_Failure) F = Standard_Failure::Caught();
      if (!F.IsNull() && !MessageDriver().IsNull()) {
        TCollection_ExtendedString aString (F->GetMessageString());
        MessageDriver()->Write(aString.ToExtString());
      }
    }
    if(storer.StoreStatus() == PCDM_SS_OK)
      D->SetSaved();
    status = storer.StoreStatus();
    theStatusMessage = storer.AssociatedStatusText();
  } else {
    theStatusMessage = "TDocStd_Application::the document has not been saved yet";
    status = PCDM_SS_Failure;
  }
  return status;
}


//=======================================================================
//function : OnOpenTransaction
//purpose  : 
//=======================================================================

void TDocStd_Application::OnOpenTransaction (const Handle(TDocStd_Document)&)
{
  // nothing to do on this level
}

//=======================================================================
//function : OnAbortTransaction
//purpose  : 
//=======================================================================

void TDocStd_Application::OnAbortTransaction (const Handle(TDocStd_Document)&)
{
  // nothing to do on this level
}

//=======================================================================
//function : OnCommitTransaction
//purpose  : 
//=======================================================================

void TDocStd_Application::OnCommitTransaction (const Handle(TDocStd_Document)&)
{
  // nothing to do on this level
}
