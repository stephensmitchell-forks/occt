// Created on: 2015-04-20
// Created by: Alexander Zaikin
// Copyright (c) 1998-1999 Matra Datavision
// Copyright (c) 1999-2015 OPEN CASCADE SAS
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

#include <TDataXtd_Presentation.hxx>

#include <TDF_DefaultDeltaOnRemoval.hxx>
#include <TDF_DefaultDeltaOnModification.hxx>
#include <TDF_DeltaOnAddition.hxx>
#include <TDF_Tool.hxx>
#include <TCollection_ExtendedString.hxx>

//=======================================================================
//function : TDataXtd_Presentation
//purpose  : Default constructor.
//=======================================================================
TDataXtd_Presentation::TDataXtd_Presentation()
: myDriverGUID           ("00000000-0000-0000-0000-000000000000"),
  myColor                (516), // Quantity_NOC_WHITE
  myMaterial             (0),   // Graphic3d_NOM_BRASS
  myMode                 (0),
  mySelectionMode        (0),
  myTransparency         (0.0),
  myWidth                (0.0),
  myIsDisplayed          (Standard_False),
  myIsHasOwnColor        (Standard_False),
  myIsHasOwnMaterial     (Standard_False),
  myIsHasOwnTransparency (Standard_False),
  myIsHasOwnWidth        (Standard_False),
  myIsHasOwnMode         (Standard_False),
  myIsHasOwnSelectionMode(Standard_False)
{}


//=======================================================================
//function : Set
//purpose  : 
//=======================================================================
Handle(TDataXtd_Presentation) TDataXtd_Presentation::Set(const TDF_Label&     theLabel,
                                                         const Standard_GUID& theDriverId)
{
  Handle(TDataXtd_Presentation) aPresentation;

  if ( !theLabel.FindAttribute(TDataXtd_Presentation::GetID(), aPresentation) )
  {
    aPresentation = new TDataXtd_Presentation();
    theLabel.AddAttribute(aPresentation);
  }

  aPresentation->SetDriverGUID(theDriverId);
  return aPresentation;
}


//=======================================================================
//function : Set
//purpose  : 
//=======================================================================
Handle(TDataXtd_Presentation) TDataXtd_Presentation::Set(const Handle(TDF_Attribute)& theMaster)
{
  Handle(TDataXtd_Presentation) aPresentation;

  const TDF_Label aLabel = theMaster->Label();
  if (!aLabel.FindAttribute(TDataXtd_Presentation::GetID(), aPresentation))
  {
    aPresentation = new TDataXtd_Presentation();
    aLabel.AddAttribute(aPresentation);
  }

  aPresentation->SetDriverGUID( theMaster->ID() );
  return aPresentation;
}


//=======================================================================
//function : Unset
//purpose  : 
//=======================================================================
void TDataXtd_Presentation::Unset(const TDF_Label& theLabel)
{
  Handle(TDataXtd_Presentation) aPresentation;
  if (theLabel.FindAttribute(TDataXtd_Presentation::GetID(), aPresentation))
    theLabel.ForgetAttribute(aPresentation);
}


//=======================================================================
//function : GetID
//purpose  : 
//=======================================================================
const Standard_GUID& TDataXtd_Presentation::GetID()
{
  static Standard_GUID TDataXtd_PresentationID("04fb4d00-5690-11d1-8940-080009dc3333");
  return TDataXtd_PresentationID;
}


//=======================================================================
//function : ID
//purpose  : 
//=======================================================================
const Standard_GUID& TDataXtd_Presentation::ID() const
{
  return GetID();
}


//=======================================================================
//function :GetDriverGUID
//purpose  : 
//=======================================================================
Standard_GUID TDataXtd_Presentation::GetDriverGUID() const
{
  return myDriverGUID;
}


//=======================================================================
//function :SetDriverGUID
//purpose  : 
//=======================================================================
void TDataXtd_Presentation::SetDriverGUID(const Standard_GUID& theGUID)
{
  if ( myDriverGUID != theGUID )
  {
    Backup();
    myDriverGUID = theGUID;
  }
}


//=======================================================================
//function : IsDisplayed
//purpose  : 
//=======================================================================
Standard_Boolean TDataXtd_Presentation::IsDisplayed() const
{
  return myIsDisplayed;
}


//=======================================================================
//function : IsHasOwnMaterial
//purpose  : 
//=======================================================================
Standard_Boolean TDataXtd_Presentation::IsHasOwnMaterial() const
{
  return myIsHasOwnMaterial;
}


//=======================================================================
//function : IsHasOwnTransparency
//purpose  : 
//=======================================================================
Standard_Boolean TDataXtd_Presentation::IsHasOwnTransparency() const
{
  return myIsHasOwnTransparency;
}


//=======================================================================
//function : IsHasOwnColor
//purpose  : 
//=======================================================================
Standard_Boolean TDataXtd_Presentation::IsHasOwnColor() const
{
  return myIsHasOwnColor;
}


//=======================================================================
//function : IsHasOwnWidth
//purpose  : 
//=======================================================================
Standard_Boolean TDataXtd_Presentation::IsHasOwnWidth() const
{
  return myIsHasOwnWidth;
}


//=======================================================================
//function : IsHasOwnMode
//purpose  : 
//=======================================================================
Standard_Boolean TDataXtd_Presentation::IsHasOwnMode() const
{
  return myIsHasOwnMode;
}


//=======================================================================
//function : IsHasOwnSelectionMode
//purpose  : 
//=======================================================================
Standard_Boolean TDataXtd_Presentation::IsHasOwnSelectionMode() const
{
  return myIsHasOwnSelectionMode;
}


//=======================================================================
//function : SetDisplayed
//purpose  : 
//=======================================================================
void TDataXtd_Presentation::SetDisplayed(const Standard_Boolean theIsDisplayed)
{
  Backup();
  myIsDisplayed = theIsDisplayed;
}


//=======================================================================
//function : SetMaterial
//purpose  : 
//=======================================================================
void TDataXtd_Presentation::SetMaterial(const Standard_Integer theName)
{
  Backup();
  myMaterial         = theName;
  myIsHasOwnMaterial = Standard_True;
}


//=======================================================================
//function : SetTransparency
//purpose  : 
//=======================================================================
void TDataXtd_Presentation::SetTransparency(const Standard_Real theValue)
{
  Backup();
  myTransparency = theValue;
  myIsHasOwnTransparency = Standard_True;
}


//=======================================================================
//function : SetColor
//purpose  : 
//=======================================================================
void TDataXtd_Presentation::SetColor(const Standard_Integer theColor)
{
  Backup();
  myColor         = theColor;
  myIsHasOwnColor = Standard_True;
}


//=======================================================================
//function : SetWidth
//purpose  : 
//=======================================================================
void TDataXtd_Presentation::SetWidth(const Standard_Real theWidth)
{
  Backup();
  myWidth         = theWidth;
  myIsHasOwnWidth = Standard_True;
}


//=======================================================================
//function : SetMode
//purpose  : 
//=======================================================================
void TDataXtd_Presentation::SetMode(const Standard_Integer theMode)
{
  Backup();
  myMode         = theMode;
  myIsHasOwnMode = Standard_True;
}


//=======================================================================
//function : SetSelectionMode
//purpose  : 
//=======================================================================
void TDataXtd_Presentation::SetSelectionMode(const Standard_Integer theSelectionMode)
{
  Backup();
  myIsHasOwnSelectionMode = Standard_True;
  mySelectionMode         = theSelectionMode;
}


//=======================================================================
//function : Material
//purpose  : 
//=======================================================================
Standard_Integer TDataXtd_Presentation::Material() const
{
  return myMaterial;
}


//=======================================================================
//function : Transparency
//purpose  : 
//=======================================================================
Standard_Real TDataXtd_Presentation::Transparency() const
{
  return myTransparency;
}


//=======================================================================
//function : Color
//purpose  : 
//=======================================================================
Standard_Integer TDataXtd_Presentation::Color() const
{
  return myColor;
}


//=======================================================================
//function : Width
//purpose  : 
//=======================================================================
Standard_Real TDataXtd_Presentation::Width() const
{
   return myWidth;
}


//=======================================================================
//function : Mode
//purpose  : 
//=======================================================================
Standard_Integer TDataXtd_Presentation::Mode() const
{
  return myMode;
}


//=======================================================================
//function : SelectionMode
//purpose  : 
//=======================================================================
Standard_Integer TDataXtd_Presentation::SelectionMode() const
{
  return mySelectionMode;
}


//=======================================================================
//function : UnsetMaterial
//purpose  : 
//=======================================================================
void TDataXtd_Presentation::UnsetMaterial()
{
  Backup();
  myIsHasOwnMaterial = Standard_False;
}


//=======================================================================
//function : UnsetTransparency
//purpose  : 
//=======================================================================
void TDataXtd_Presentation::UnsetTransparency()
{
  myIsHasOwnTransparency = Standard_False;
}


//=======================================================================
//function : UnsetColor
//purpose  : 
//=======================================================================
void TDataXtd_Presentation::UnsetColor()
{
  Backup();
  myIsHasOwnColor = Standard_False;
}


//=======================================================================
//function : UnsetWidth
//purpose  : 
//=======================================================================
void TDataXtd_Presentation::UnsetWidth()
{
  Backup();
  myIsHasOwnWidth = Standard_False;
}


//=======================================================================
//function : UnsetMode
//purpose  : 
//=======================================================================
void TDataXtd_Presentation::UnsetMode()
{
  Backup();
  myIsHasOwnMode = Standard_False;
}


//=======================================================================
//function : UnsetSelectionMode
//purpose  : 
//=======================================================================
void TDataXtd_Presentation::UnsetSelectionMode()
{
  Backup();
  myIsHasOwnSelectionMode = Standard_False;
}


//=======================================================================
//function : BackupCopy
//purpose  : 
//=======================================================================
Handle(TDF_Attribute) TDataXtd_Presentation::BackupCopy() const
{
  Handle(TDataXtd_Presentation) aCopy = new TDataXtd_Presentation;

  aCopy->myIsDisplayed   = myIsDisplayed;
  aCopy->myDriverGUID    = myDriverGUID;
  aCopy->mySelectionMode = mySelectionMode;
  aCopy->myTransparency  = myTransparency;
  aCopy->myColor         = myColor;
  aCopy->myMode          = myMode;
  aCopy->myWidth         = myWidth;
  aCopy->myMaterial      = myMaterial;

  aCopy->myIsHasOwnColor         = myIsHasOwnColor;
  aCopy->myIsHasOwnMaterial      = myIsHasOwnMaterial;
  aCopy->myIsHasOwnWidth         = myIsHasOwnWidth;
  aCopy->myIsHasOwnMode          = myIsHasOwnMode;
  aCopy->myIsHasOwnTransparency  = myIsHasOwnTransparency;
  aCopy->myIsHasOwnSelectionMode = myIsHasOwnSelectionMode;

  return aCopy;
}


//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================
Handle(TDF_Attribute) TDataXtd_Presentation::NewEmpty() const
{
  return new TDataXtd_Presentation();
}


//=======================================================================
//function : Restore
//purpose  : 
//=======================================================================
void TDataXtd_Presentation::Restore(const Handle(TDF_Attribute)& theAttribute)
{
  Handle(TDataXtd_Presentation) aPresentation =
    Handle(TDataXtd_Presentation)::DownCast(theAttribute);

  myIsHasOwnMaterial = aPresentation->IsHasOwnMaterial();
  myMaterial = aPresentation->Material();

  myIsHasOwnColor = aPresentation->IsHasOwnColor();
  myColor = aPresentation->Color();

  myIsHasOwnWidth = aPresentation->IsHasOwnWidth();
  myWidth = aPresentation->Width();

  myIsHasOwnMode = aPresentation->IsHasOwnMode();
  myMode = aPresentation->Mode();

  myIsHasOwnSelectionMode = aPresentation->IsHasOwnSelectionMode();
  mySelectionMode = aPresentation->SelectionMode();

  myIsHasOwnTransparency = aPresentation->IsHasOwnTransparency();
  myTransparency = aPresentation->Transparency();

  myIsDisplayed = aPresentation->IsDisplayed();
  myDriverGUID  = aPresentation->GetDriverGUID();
}


//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================
void TDataXtd_Presentation::Paste(const Handle(TDF_Attribute)& theInto,
                                  const Handle(TDF_RelocationTable)&) const
{
  Handle(TDataXtd_Presentation) anInto =
    Handle(TDataXtd_Presentation)::DownCast(theInto);

  anInto->Backup();

  if (myIsHasOwnMaterial)
  {
    anInto->myMaterial = myMaterial;
    anInto->myIsHasOwnMaterial = Standard_True;
  }
  else
  {
    anInto->myIsHasOwnMaterial = Standard_False;
  }

  if (myIsHasOwnColor)
  {
    anInto->myColor = myColor;
    anInto->myIsHasOwnColor = Standard_True;
  }
  else
  {
    anInto->myIsHasOwnColor = Standard_False;
  }

  if(myIsHasOwnWidth)
  {
    anInto->myWidth = myWidth;
    anInto->myIsHasOwnWidth = Standard_True;
  }
  else
  {
    anInto->myIsHasOwnWidth = Standard_False;
  }

  if (myIsHasOwnMode)
  {
    anInto->myMode = myMode;
    anInto->myIsHasOwnMode = Standard_True;
  }
  else
  {
    anInto->myIsHasOwnMode = Standard_False;
  }

  if (myIsHasOwnSelectionMode)
  {
    anInto->mySelectionMode = mySelectionMode;
    anInto->myIsHasOwnSelectionMode = Standard_True;
  }
  else
  {
    anInto->myIsHasOwnSelectionMode = Standard_False;
  }

  if (myIsHasOwnTransparency)
  {
    anInto->myTransparency = myTransparency;
    anInto->myIsHasOwnTransparency = Standard_True;
  }
  else
  {
    anInto->myIsHasOwnTransparency = Standard_False;
  }

  anInto->myIsDisplayed = myIsDisplayed;
  anInto->myDriverGUID  = myDriverGUID;
}
