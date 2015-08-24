// Created on: 1997-07-08
// Created by: Sergey RUIN
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

#include <MDataXtd_PresentationStorageDriver.ixx>

#include <PDataXtd_Presentation.hxx>
#include <PDataXtd_Presentation_1.hxx>
#include <TDataXtd_Presentation.hxx>
#include <TCollection_ExtendedString.hxx>
#include <PCollection_HExtendedString.hxx>
#include <CDM_MessageDriver.hxx>

//=======================================================================
//function : MDataXtd_PresentationStorageDriver
//purpose  : 
//=======================================================================

MDataXtd_PresentationStorageDriver::MDataXtd_PresentationStorageDriver(const Handle(CDM_MessageDriver)& theMsgDriver)
: MDF_ASDriver(theMsgDriver)
{
}

Standard_Integer MDataXtd_PresentationStorageDriver::VersionNumber() const
{
  return 0;
}

Handle(Standard_Type) MDataXtd_PresentationStorageDriver::SourceType() const
{
  return STANDARD_TYPE(TDataXtd_Presentation);
}

Handle(PDF_Attribute) MDataXtd_PresentationStorageDriver::NewEmpty() const
{
  return new PDataXtd_Presentation_1;
}

void MDataXtd_PresentationStorageDriver::Paste(const Handle(TDF_Attribute)& Source,
                                               const Handle(PDF_Attribute)& Target,
                                               const Handle(MDF_SRelocationTable)& /*RelocTable*/) const
{
  Handle(TDataXtd_Presentation) S = Handle(TDataXtd_Presentation)::DownCast(Source);

  Handle(PDataXtd_Presentation_1) T = Handle(PDataXtd_Presentation_1)::DownCast(Target);

  T->SetDisplayed( S->IsDisplayed() );

  TCollection_ExtendedString  extstr("00000000-0000-0000-0000-000000000000");  //covert GUID into ExtendedString
  Standard_PExtCharacter pStr = const_cast<Standard_PExtCharacter>(extstr.ToExtString());
  S->GetDriverGUID().ToExtString(pStr);

  Handle(PCollection_HExtendedString) guid = new PCollection_HExtendedString ( extstr );
  T->SetDriverGUID ( guid );

  if( S->IsHasOwnColor() ) T->SetColor( S->Color() );
  else T->SetColor(-1);

  if( S->IsHasOwnMaterial() ) T->SetMaterial( S->Material() );
  else T->SetMaterial(-1);

  if( S->IsHasOwnTransparency() ) T->SetTransparency( S->Transparency() );
  else T->SetTransparency(-1.);

  if( S->IsHasOwnWidth() ) T->SetWidth( S->Width() );
  else T->SetWidth(-1.);

  T->SetMode(S->Mode());

#ifdef OCCT_DEBUG
  cout << "AISPresentationStorageDriver "  << "storaged DriverGUID ==> "   << guid->Convert() << endl;
#endif
}
