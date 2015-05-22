// File:      NIS_LineDrawer.h
// Created:   11.05.12
// Author:    Natalia Kopnova
// Copyright: LPKF Laser and Electronics AG 2012

#ifndef NIS_LineDrawer_HeaderFile
#define NIS_LineDrawer_HeaderFile

#include <NIS_Drawer.hxx>

class NIS_Line;

/**
 * NIS drawer for NIS_Line interactive object type.
 * @ingroup aisobject
 */

class NIS_LineDrawer : public NIS_Drawer
{
 public:
  // ---------- PUBLIC METHODS ----------

  /**
   * Constructor.
   */
  Standard_EXPORT NIS_LineDrawer   (const Quantity_Color& theColor);

  /**
   * Copy the relevant information from another instance of Drawer.
   * raises exception if theOther has incompatible type (test IsKind).
   */
  Standard_EXPORT virtual void Assign    (const Handle_NIS_Drawer& theOther);

  /**
   * Called before execution of Draw(), once per group of interactive objects.
   */
  Standard_EXPORT virtual void BeforeDraw(const DrawType         theType,
                                          const NIS_DrawList&    theDrawList);

  /**
   * Called after execution of Draw(), once per group of interactive objects.
   */
  Standard_EXPORT virtual void AfterDraw(const DrawType         theType,
                                         const NIS_DrawList&    theDrawList);

  /**
   * Main function: display the given interactive object in the given view.
   */
  Standard_EXPORT virtual void Draw      (const Handle_NIS_InteractiveObject&,
                                          const DrawType         theType,
                                          const NIS_DrawList&    theDrawList);

  /**
   * Matching two instances, for Map interface.
   */
  Standard_EXPORT virtual Standard_Boolean
                               IsEqual   (const Handle_NIS_Drawer& theOth)const;


 protected:
  // ---------- PROTECTED METHODS ----------



 private:
  // ---------- PRIVATE FIELDS ----------

  Standard_ShortReal  myLineWidth;
  Standard_Integer    myType;
  Standard_Integer    myLineType;
  Standard_Boolean    myInvertColor;


 public:
// Declaration of CASCADE RTTI
DEFINE_STANDARD_RTTI (NIS_LineDrawer)

  friend class NIS_Line;
};

// Definition of HANDLE object using Standard_DefineHandle.hxx
DEFINE_STANDARD_HANDLE (NIS_LineDrawer, NIS_Drawer)


#endif
