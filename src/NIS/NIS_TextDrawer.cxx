// File:      NIS_TextDrawer.cxx
// Created:   12.12.11 07:37
// Author:    Alexander GRIGORIEV
// Copyright: Open Cascade 2011

#include <NIS_TextDrawer.hxx>
#ifdef WNT
#ifdef NOGDI
#undef NOGDI
#endif
#ifdef NOUSER
#undef NOUSER
#endif
#include <windows.h>
#endif
#include <GL/gl.h>

IMPLEMENT_STANDARD_HANDLE  (NIS_TextDrawer, NIS_Drawer)
IMPLEMENT_STANDARD_RTTIEXT (NIS_TextDrawer, NIS_Drawer)

//=======================================================================
//function : NIS_TextDrawer()
//purpose  : Constructor
//=======================================================================

NIS_TextDrawer::NIS_TextDrawer (  )
  : myFontFamily        (NIS_Text::FontFamily_DEFAULT),
    myFontWeight        (NIS_Text::FontWeight_Normal),
    myFontCharset       (NIS_Text::FontCharset_ANSI),
    myFontHeight        (18),
    myFontAngle         (0),
    myIsItalic          (Standard_False),
    myIsUnderline       (Standard_False)
{
  SetCompiled(Standard_False);
}

//=======================================================================
//function : createDefaultList
//purpose  : 
//=======================================================================

NIS_DrawList* NIS_TextDrawer::createDefaultList
                        (const Handle_NIS_View& theView) const
{
  return new DrawList(theView);
}

//=======================================================================
//function : SetListID
//purpose  : 
//=======================================================================

void NIS_TextDrawer::DrawList::SetListID (const NIS_Drawer::DrawType theType,
                                          const Standard_Size        nLists)
{
  myListID[theType] = (Standard_Integer)glGenLists(nLists);
}

//=======================================================================
//function : Assign
//purpose  : 
//=======================================================================

void NIS_TextDrawer::Assign (const Handle_NIS_Drawer& theOther)
{
  if (theOther.IsNull() == Standard_False) {
    NIS_Drawer::Assign(theOther);
    const Handle(NIS_TextDrawer)& anOther =
      static_cast <const Handle(NIS_TextDrawer)&> (theOther);
    myFontFamily  = anOther->myFontFamily;
    myFontWeight  = anOther->myFontWeight;
    myFontCharset = anOther->myFontCharset;
    myFontHeight  = anOther->myFontHeight;
    myFontAngle   = anOther->myFontAngle;
    myIsItalic    = anOther->myIsItalic;
    myIsUnderline = anOther->myIsUnderline;
  }
}

//=======================================================================
//function : IsEqual
//purpose  : 
//=======================================================================

Standard_Boolean NIS_TextDrawer::IsEqual
                                (const Handle_NIS_Drawer& theOther) const
{
  Standard_Boolean aResult (Standard_False);
  if (NIS_Drawer::IsEqual(theOther)) {
    const Handle(NIS_TextDrawer) anOther =
      Handle(NIS_TextDrawer)::DownCast(theOther);
    aResult = (myFontFamily == anOther->myFontFamily &&
               myFontWeight == anOther->myFontWeight &&
               myFontCharset == anOther->myFontCharset &&
               myFontHeight == anOther->myFontHeight &&
               myFontAngle == anOther->myFontAngle &&
               myIsItalic == anOther->myIsItalic &&
               myIsUnderline == anOther->myIsUnderline);
  }
  return aResult;
}

//=======================================================================
//function : SetFontUpdated
//purpose  : 
//=======================================================================

void NIS_TextDrawer::SetFontUpdated () const
{
  NCollection_List<NIS_DrawList*>::Iterator anIter(myLists);
  for (; anIter.More(); anIter.Next()) {
    DrawList* pList = static_cast<DrawList *>(anIter.Value());
    if (pList)
      pList->IsFontUpdated = Standard_True;
  }
}

//=======================================================================
//function : BeforeDraw
//purpose  : 
//=======================================================================

void NIS_TextDrawer::BeforeDraw(const DrawType      theType,
                                const NIS_DrawList& theDrawList)
{
  // indicate start of glyph display lists
  const DrawList& aList = static_cast<const DrawList&> (theDrawList);

  if (aList.IsFontUpdated || aList.GetListID(Draw_Normal) == 0)
    updateFontBitmaps(const_cast<DrawList&>(aList));
  glListBase (aList.GetListID(Draw_Normal));

  Quantity_Parameter aValue[4];
  const Quantity_TypeOfColor bidTC (Quantity_TOC_RGB);
  GetColor(theType).Values (aValue[0], aValue[1], aValue[2], bidTC);
  aValue[3] = 1. - myTransparency;
  if (theType == Draw_Transparent && myTransparency > 0.01) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  }
  glColor4d (aValue[0], aValue[1], aValue[2], aValue[3]);

  glDisable(GL_LIGHTING);  
}

//=======================================================================
//function : AfterDraw
//purpose  : 
//=======================================================================

void NIS_TextDrawer::AfterDraw(const DrawType         theType,
                               const NIS_DrawList&    /*theDrawList*/)
{
  if (theType == Draw_Transparent && myTransparency > 0.01)
    glDisable(GL_BLEND);
}

//=======================================================================
//function : Draw
//purpose  : Display the given interactive object in the given view.
//=======================================================================

void NIS_TextDrawer::Draw (const Handle_NIS_InteractiveObject& theObj,
                           const DrawType                      /*theType*/,
                           const NIS_DrawList&                 /*theDrawList*/)
{
  const NIS_Text * pObj = static_cast<const NIS_Text *>(theObj.operator->());

  glRasterPos3d(pObj->myPosition.X(),pObj->myPosition.Y(),pObj->myPosition.Z());
  // display a string:
  glCallLists (pObj->myText.Length(), GL_UNSIGNED_BYTE,
               pObj->myText.ToCString());
}

//=======================================================================
//function : updateFontBitmaps
//purpose  : 
//=======================================================================

void NIS_TextDrawer::updateFontBitmaps (NIS_TextDrawer::DrawList& theList)
{
  if (theList.GetListID(Draw_Normal) == 0) {
    theList.SetListID(Draw_Normal, 256);
  }

#ifdef WNT
  int aWeight, aFamily, aCharset;
  switch (myFontWeight) {
    default:
    case NIS_Text::FontWeight_DEFAULT:
      aWeight = FW_DONTCARE; break;
    case NIS_Text::FontWeight_Thin:
      aWeight = FW_THIN; break;      
    case NIS_Text::FontWeight_Extralight:
      aWeight = FW_EXTRALIGHT; break;      
    case NIS_Text::FontWeight_Ultralight:
      aWeight = FW_ULTRALIGHT; break;      
    case NIS_Text::FontWeight_Book:
    case NIS_Text::FontWeight_Light:
      aWeight = FW_LIGHT; break;      
    case NIS_Text::FontWeight_Normal:
      aWeight = FW_NORMAL; break;      
    case NIS_Text::FontWeight_Regular:
      aWeight = FW_REGULAR; break;      
    case NIS_Text::FontWeight_Medium:
      aWeight = FW_MEDIUM; break;      
    case NIS_Text::FontWeight_Semibold:
      aWeight = FW_SEMIBOLD; break;      
    case NIS_Text::FontWeight_Demibold:
      aWeight = FW_DEMIBOLD; break;      
    case NIS_Text::FontWeight_Bold:
      aWeight = FW_BOLD; break;      
    case NIS_Text::FontWeight_Extrabold:
      aWeight = FW_EXTRABOLD; break;      
    case NIS_Text::FontWeight_Ultrabold:
      aWeight = FW_ULTRABOLD; break;      
    case NIS_Text::FontWeight_Heavy:
      aWeight = FW_HEAVY; break;      
    case NIS_Text::FontWeight_Black:    
      aWeight = FW_BLACK; break;      
  }
  switch (myFontFamily) {
    case NIS_Text::FontFamily_Decorative:
      aFamily = FF_DECORATIVE; break;
    case NIS_Text::FontFamily_Courier:
    case NIS_Text::FontFamily_Modern:
      aFamily = FF_MODERN; break;
    case NIS_Text::FontFamily_Times:
    case NIS_Text::FontFamily_Roman:
      aFamily = FF_ROMAN; break;
    case NIS_Text::FontFamily_Lucida:
    case NIS_Text::FontFamily_Script:
      aFamily = FF_SCRIPT; break;
    case NIS_Text::FontFamily_Helvetica:
    case NIS_Text::FontFamily_Swiss:
      aFamily = FF_SWISS; break;
    case NIS_Text::FontFamily_Symbol:
    default:
    case NIS_Text::FontFamily_DEFAULT:
      aFamily = FF_DONTCARE; break;
  }
  switch (myFontCharset) {
    case NIS_Text::FontCharset_ANSI:
      aCharset = ANSI_CHARSET; break;
    case NIS_Text::FontCharset_EastEurope:
      aCharset = EASTEUROPE_CHARSET; break;
    case NIS_Text::FontCharset_Greek:
      aCharset = GREEK_CHARSET; break;
    case NIS_Text::FontCharset_Russian:
      aCharset = RUSSIAN_CHARSET; break;
    case NIS_Text::FontCharset_Chinese:
      aCharset = CHINESEBIG5_CHARSET; break;
    case NIS_Text::FontCharset_Symbol:
      aCharset = SYMBOL_CHARSET; break;
    case NIS_Text::FontCharset_Turkish:
      aCharset = TURKISH_CHARSET; break;
    case NIS_Text::FontCharset_Hebrew:
      aCharset = HEBREW_CHARSET; break;
    case NIS_Text::FontCharset_Arabic:
      aCharset = ARABIC_CHARSET; break;
    default:
    case NIS_Text::FontCharset_DEFAULT:
      aCharset = DEFAULT_CHARSET; break;
  }
  // create font
  HFONT hFont = ::CreateFont
    (myFontHeight,
     0,                         // character width
     myFontAngle,               // angle
     0,                         // orientation
     aWeight,
     (DWORD)myIsItalic,
     (DWORD)myIsUnderline,
     FALSE,                     // strikeout
     aCharset,
     OUT_TT_PRECIS,             // output precision (TrueType font)
     CLIP_DEFAULT_PRECIS,       // clipping precision (default)
     NONANTIALIASED_QUALITY,    // output quality
     aFamily,                   // pitch and family
     NULL);                     // face name

  HDC hdc = GetDC(NULL);    // screen device
  HGDIOBJ aPrev = ::SelectObject(hdc, hFont);
//  SelectObject (hdc, GetStockObject (SYSTEM_FONT));

// create the bitmap display lists
// we're making images of glyphs 0 thru 255
  wglUseFontBitmaps (hdc, 0, 255, theList.GetListID(Draw_Normal));

  aPrev = ::SelectObject(hdc, aPrev);
  ::DeleteObject(aPrev);
  ::ReleaseDC(NULL, hdc);
#endif

  theList.IsFontUpdated = Standard_False;
}
