// File:      NIS_TextDrawer.hxx
// Created:   12.12.11 07:31
// Author:    Alexander GRIGORIEV
// Copyright: Open Cascade 2011


#ifndef NIS_TextDrawer_HeaderFile
#define NIS_TextDrawer_HeaderFile

#include <NIS_Drawer.hxx>
#include <NIS_Text.hxx>

/**
 * Drawer for bitmap text object
 * @ingroup nis_library
 */

class NIS_TextDrawer : public NIS_Drawer
{
 public:
  // ---------- PUBLIC METHODS ----------
  class DrawList : public NIS_DrawList
  {
  protected:
    inline DrawList(const Handle_NIS_View& theView)
      : NIS_DrawList (theView), IsFontUpdated(Standard_True) {}
    //! Find an empty range of list IDs and allocate it.
    Standard_EXPORT void SetListID (const NIS_Drawer::DrawType theType,
                                    const Standard_Size        nLists);
    Standard_Boolean IsFontUpdated;
    friend class NIS_TextDrawer;
  };

  /**
   * Empty constructor.
   */
  Standard_EXPORT NIS_TextDrawer ();

  /**
   * Special implementation - create DrawList for this object.
   */
  Standard_EXPORT virtual NIS_DrawList*
                createDefaultList (const Handle_NIS_View& theView) const;

  /**
   * Copy the relevant information from another instance of Drawer.
   * raises exception if theOther has incompatible type (test IsKind).
   */
  Standard_EXPORT virtual void Assign   (const Handle_NIS_Drawer& theOther);

  /**
   * Matching two instances, for Map interface.
   */
  Standard_EXPORT virtual Standard_Boolean
                               IsEqual  (const Handle_NIS_Drawer& theOth)const;

  /**
   * Called before execution of Draw(), once per group of interactive objects.
   */
  Standard_EXPORT virtual void BeforeDraw(const DrawType         theType,
                                          const NIS_DrawList&    theDrawList);

  /**
   * Main function: display the given interactive object in the given view.
   */
  Standard_EXPORT virtual void Draw     (const Handle_NIS_InteractiveObject&,
                                         const DrawType         theType,
                                         const NIS_DrawList&    theDrawList);

  /**
   * Called after execution of Draw(), once per group of interactive objects.
   */
  Standard_EXPORT virtual void AfterDraw(const DrawType         theType,
                                         const NIS_DrawList&    theDrawList);

 protected:
  // ---------- PROTECTED METHODS ----------

  Standard_EXPORT void SetFontUpdated   () const;

 private:
  void                 updateFontBitmaps(DrawList& theList);

 private:
  // ---------- PRIVATE FIELDS ----------
  NIS_Text::FontFamily          myFontFamily;
  NIS_Text::FontWeight          myFontWeight;
  NIS_Text::FontCharset         myFontCharset;
  Standard_Integer              myFontHeight;
  Standard_Integer              myFontAngle;    //!< in tenths of degree
  Standard_Boolean              myIsItalic;
  Standard_Boolean              myIsUnderline;

 public:
// Declaration of CASCADE RTTI
DEFINE_STANDARD_RTTI (NIS_TextDrawer)
friend class NIS_Text;
};

// Definition of HANDLE object using Standard_DefineHandle.hxx
DEFINE_STANDARD_HANDLE (NIS_TextDrawer, NIS_Drawer)


#endif
