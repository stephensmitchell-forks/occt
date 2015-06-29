// File:      NIS_Text.hxx
// Created:   12.12.11 06:54
// Author:    Alexander GRIGORIEV
// Copyright: Open Cascade 2011


#ifndef NIS_Text_HeaderFile
#define NIS_Text_HeaderFile

#include <NIS_InteractiveObject.hxx>
#include <TCollection_AsciiString.hxx>
#include <gp_XYZ.hxx>

class NIS_TextDrawer;

/**
 * Bitmap Text presentation.
 * @ingroup nis_library
 */

class NIS_Text : public NIS_InteractiveObject
{
 public:
  enum FontFamily {
    FontFamily_DEFAULT = 0,
    FontFamily_Decorative,
    FontFamily_Modern,
    FontFamily_Roman,
    FontFamily_Script,
    FontFamily_Swiss,
    FontFamily_Times,
    FontFamily_Helvetica,
    FontFamily_Lucida,
    FontFamily_Courier,
    FontFamily_Symbol
  };

  enum FontWeight {
    FontWeight_DEFAULT = 0,
    FontWeight_Thin,
    FontWeight_Extralight,
    FontWeight_Ultralight,
    FontWeight_Light,
    FontWeight_Book,
    FontWeight_Normal,
    FontWeight_Regular,
    FontWeight_Medium,
    FontWeight_Semibold,
    FontWeight_Demibold,
    FontWeight_Bold,
    FontWeight_Extrabold,
    FontWeight_Ultrabold,
    FontWeight_Heavy,
    FontWeight_Black
  };

  enum FontCharset {
    FontCharset_DEFAULT = 0,
    FontCharset_ANSI,
    FontCharset_EastEurope,
    FontCharset_Greek,
    FontCharset_Russian,
    FontCharset_Chinese,
    FontCharset_Symbol,
    FontCharset_Turkish,
    FontCharset_Hebrew,
    FontCharset_Arabic
  };

 public:
  // ---------- PUBLIC METHODS ----------


  /**
   * Empty constructor.
   */
  Standard_EXPORT       NIS_Text        (const char * theText = 0L);

  /**
   * Initialization with a text string
   */
  Standard_EXPORT void  SetText         (const char * theText);

  /**
   * Initialization with the text position
   */
  Standard_EXPORT void  SetPosition     (const gp_XYZ& thePosition);

  /**
   * Define the font type (family).
   */
  Standard_EXPORT void  SetFontFamily   (const FontFamily theFamily);

  /**
   * Define the font weight.
   */
  Standard_EXPORT void  SetFontWeight   (const FontWeight theWeight);

  /**
   * Define the charset of the created font.
   */
  Standard_EXPORT void  SetFontCharset  (const FontCharset theCharset);

  /**
   * Define the character height in pixels.
   */
  Standard_EXPORT void  SetFontHeight   (const Standard_Integer theValue);

  /**
   * Define the angle of the text relative to OX.
   */
  Standard_EXPORT void  SetFontAngle    (const Standard_Real theValue);

  /**
   * Defines if italic font type to use.
   */
  Standard_EXPORT void  SetItalic
                        (const Standard_Boolean isItalic = Standard_True);

  /**
   * Defines if Underline font to use.
   */
  Standard_EXPORT void  SetUnderline
                        (const Standard_Boolean isUnderline = Standard_True);

  /**
   * Create a default drawer instance.
   */
  Standard_EXPORT virtual NIS_Drawer *
                        DefaultDrawer   (NIS_Drawer *) const;

  /**
   * Create a copy of theObject except its ID.
   * @param theAll
   *   Allocator where the Dest should store its private data.
   * @param theDest
   *   <tt>[in-out]</tt> The target object where the data are copied. If
   *   passed NULL then the target should be created.
   */
  Standard_EXPORT virtual void
                        Clone (const Handle_NCollection_BaseAllocator& theAll,
                               Handle_NIS_InteractiveObject& theDest) const;

  /**
   * Intersect method - empty implementation.
   */
  Standard_EXPORT virtual Standard_Real
                        Intersect       (const gp_Ax1&       theAxis,
                                         const Standard_Real theOver) const;


 protected:
  // ---------- PROTECTED METHODS ----------

  /**
   * Create a 3D bounding box of the object.
   */
  Standard_EXPORT virtual void
                        computeBox      ();

 private:
  // ---------- PRIVATE FIELDS ----------

  TCollection_AsciiString       myText;
  gp_XYZ                        myPosition;

 public:
// Declaration of CASCADE RTTI
DEFINE_STANDARD_RTTI (NIS_Text)
friend class NIS_TextDrawer;
};

// Definition of HANDLE object using Standard_DefineHandle.hxx
DEFINE_STANDARD_HANDLE (NIS_Text, NIS_InteractiveObject)


#endif
