// Created on: 1999-07-08
// Created by: Sergey RUIN
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

#include <MDataXtd_PresentationRetrievalDriver.ixx>

#include <PDataXtd_Presentation.hxx>
#include <TDataXtd_Presentation.hxx>
#include <AIS_Drawer.hxx>
#include <TCollection_ExtendedString.hxx>
#include <PCollection_HExtendedString.hxx>
#include <CDM_MessageDriver.hxx>

//=======================================================================
//function : MDataXtd_PresentationRetrievalDriver
//purpose  : 
//=======================================================================

MDataXtd_PresentationRetrievalDriver::MDataXtd_PresentationRetrievalDriver
                                (const Handle(CDM_MessageDriver)& theMsgDriver)
: MDF_ARDriver(theMsgDriver)
{
}

Standard_Integer MDataXtd_PresentationRetrievalDriver::VersionNumber() const
{
  return 0;
}

Handle(Standard_Type) MDataXtd_PresentationRetrievalDriver::SourceType() const
{
  return STANDARD_TYPE(PDataXtd_Presentation);
}

Handle(TDF_Attribute) MDataXtd_PresentationRetrievalDriver::NewEmpty() const
{
  return new TDataXtd_Presentation;
}

void MDataXtd_PresentationRetrievalDriver::Paste(const Handle(PDF_Attribute)& Source,
                                                   const Handle(TDF_Attribute)& Target,
                                                   const Handle(MDF_RRelocationTable)& /*RelocTable*/) const
{
  Handle(PDataXtd_Presentation) S = Handle(PDataXtd_Presentation)::DownCast (Source);
  Handle(TDataXtd_Presentation) T = Handle(TDataXtd_Presentation)::DownCast (Target);

  T->SetDisplayed( S->IsDisplayed() );

  TCollection_ExtendedString extstr = (S->GetDriverGUID())->Convert();
  Standard_GUID guid( extstr.ToExtString() );
 
  T->SetDriverGUID( guid ); 

  if( S->Color() != -1 ) T->SetColor( S->Color() );
  else T->UnsetColor();

  if( S->Material() != -1 ) T->SetMaterial( S->Material() );
  else T->UnsetMaterial(); 

  if( S->Transparency() != -1. ) T->SetTransparency( S->Transparency() );
  else T->UnsetTransparency(); 

  if( S->Width() != -1. ) T->SetWidth( S->Width() );
  else T->UnsetWidth(); 

#ifdef DEB
  cout << "AISPresentationRetrievalDriver "  << "retrieved DriverGUID ==> ";
  guid.ShallowDump(cout);
  cout << endl;
#endif
}
