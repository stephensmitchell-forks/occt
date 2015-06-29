// File:      NIS_Text.cxx
// Created:   12.12.11 07:16
// Author:    Alexander GRIGORIEV
// Copyright: Open Cascade 2011

#include <NIS_Text.hxx>
#include <NIS_TextDrawer.hxx>

IMPLEMENT_STANDARD_HANDLE  (NIS_Text, NIS_InteractiveObject)
IMPLEMENT_STANDARD_RTTIEXT (NIS_Text, NIS_InteractiveObject)

//=======================================================================
//function : NIS_Text()
//purpose  : Constructor
//=======================================================================

NIS_Text::NIS_Text (const char * theText)
  : myText         (theText? theText : ""),
    myPosition     (gp_XYZ(0., 0., 0.))
{
}

//=======================================================================
//function : SetText
//purpose  : Initialization with a text string
//=======================================================================

void NIS_Text::SetText (const char * theText)
{
  myText = theText;
  if (GetDrawer().IsNull() == Standard_False)
    setDrawerUpdate();
}

//=======================================================================
//function : SetPosition
//purpose  : Initialization with the text position
//=======================================================================

void NIS_Text::SetPosition (const gp_XYZ& thePosition)
{
  myPosition = thePosition;
  if (GetDrawer().IsNull() == Standard_False)
    setDrawerUpdate();  
}

//=======================================================================
//function : SetFontFamily
//purpose  : Define the font type (family).
//=======================================================================

void NIS_Text::SetFontFamily (const FontFamily theFamily)
{
  const Handle(NIS_TextDrawer) aDrawer =
    static_cast<NIS_TextDrawer *>(DefaultDrawer(0L));
  aDrawer->Assign (GetDrawer());
  aDrawer->myFontFamily = theFamily;
  const NIS_TextDrawer * pDrawer =
    static_cast<const NIS_TextDrawer *>(SetDrawer(aDrawer).operator->());
  pDrawer->SetFontUpdated();
}

//=======================================================================
//function : SetFontWeight
//purpose  : Define the font weight.
//=======================================================================

void NIS_Text::SetFontWeight (const FontWeight theWeight)
{
  const Handle(NIS_TextDrawer) aDrawer =
    static_cast<NIS_TextDrawer *>(DefaultDrawer(0L));
  aDrawer->Assign (GetDrawer());
  aDrawer->myFontWeight = theWeight;
  const NIS_TextDrawer * pDrawer =
    static_cast<const NIS_TextDrawer *>(SetDrawer(aDrawer).operator->());
  pDrawer->SetFontUpdated();
}

//=======================================================================
//function : SetFontCharset
//purpose  : Define the charset of the created font.
//=======================================================================

void NIS_Text::SetFontCharset (const FontCharset theCharset)
{
  const Handle(NIS_TextDrawer) aDrawer =
    static_cast<NIS_TextDrawer *>(DefaultDrawer(0L));
  aDrawer->Assign (GetDrawer());
  aDrawer->myFontCharset = theCharset;
  const NIS_TextDrawer * pDrawer =
    static_cast<const NIS_TextDrawer *>(SetDrawer(aDrawer).operator->());
  pDrawer->SetFontUpdated();
}

//=======================================================================
//function : SetFontHeight
//purpose  : Define the character height in pixels.
//=======================================================================

void NIS_Text::SetFontHeight (const Standard_Integer theValue)
{
  const Handle(NIS_TextDrawer) aDrawer =
    static_cast<NIS_TextDrawer *>(DefaultDrawer(0L));
  aDrawer->Assign (GetDrawer());
  aDrawer->myFontHeight = theValue;
  const NIS_TextDrawer * pDrawer =
    static_cast<const NIS_TextDrawer *>(SetDrawer(aDrawer).operator->());
  pDrawer->SetFontUpdated();
}

//=======================================================================
//function : SetFontAngle
//purpose  : Define the angle of the text relative to OX.
//=======================================================================

void NIS_Text::SetFontAngle (const Standard_Real theValue)
{
  const Handle(NIS_TextDrawer) aDrawer =
    static_cast<NIS_TextDrawer *>(DefaultDrawer(0L));
  aDrawer->Assign (GetDrawer());
  aDrawer->myFontAngle =
    static_cast<Standard_Integer>(0.1 + (theValue * 1800) / M_PI);
  const NIS_TextDrawer * pDrawer =
    static_cast<const NIS_TextDrawer *>(SetDrawer(aDrawer).operator->());
  pDrawer->SetFontUpdated();
}

//=======================================================================
//function : SetItalic
//purpose  : Defines if italic font type to use.
//=======================================================================

void NIS_Text::SetItalic (const Standard_Boolean isItalic)
{
  const Handle(NIS_TextDrawer) aDrawer =
    static_cast<NIS_TextDrawer *>(DefaultDrawer(0L));
  aDrawer->Assign (GetDrawer());
  aDrawer->myIsItalic = isItalic;
  const NIS_TextDrawer * pDrawer =
    static_cast<const NIS_TextDrawer *>(SetDrawer(aDrawer).operator->());
  pDrawer->SetFontUpdated();
}

//=======================================================================
//function : SetUnderline
//purpose  : Defines if Underline font to use.
//=======================================================================

void NIS_Text::SetUnderline (const Standard_Boolean isUnderline)
{
  const Handle(NIS_TextDrawer) aDrawer =
    static_cast<NIS_TextDrawer *>(DefaultDrawer(0L));
  aDrawer->Assign (GetDrawer());
  aDrawer->myIsUnderline = isUnderline;
  const NIS_TextDrawer * pDrawer =
    static_cast<const NIS_TextDrawer *>(SetDrawer(aDrawer).operator->());
  pDrawer->SetFontUpdated();
}

//=======================================================================
//function : DefaultDrawer
//purpose  : Create a default drawer instance.
//=======================================================================

NIS_Drawer * NIS_Text::DefaultDrawer (NIS_Drawer * theDrawer) const
{
  if (!theDrawer)
    theDrawer = new NIS_TextDrawer;
  return theDrawer;
}

//=======================================================================
//function : Clone
//purpose  : 
//=======================================================================

void NIS_Text::Clone (const Handle_NCollection_BaseAllocator& theAllocator,
                      Handle_NIS_InteractiveObject&           theDest) const
{
  Handle(NIS_Text) aNewObj;
  if (theDest.IsNull()) {
    aNewObj = new NIS_Text();
    theDest = aNewObj;
  } else {
    aNewObj = reinterpret_cast<NIS_Text*> (theDest.operator->());
  }
  NIS_InteractiveObject::Clone(theAllocator, theDest);  
  aNewObj->myText = myText;
  aNewObj->myPosition = myPosition;
}

//=======================================================================
//function : Intersect
//purpose  : 
//=======================================================================

Standard_Real NIS_Text::Intersect (const gp_Ax1&       /*theAxis*/,
                                   const Standard_Real /*theOver*/) const
{
  return RealLast();
}

//=======================================================================
//function : computeBox
//purpose  : empty method
//=======================================================================

void NIS_Text::computeBox ()
{
}
