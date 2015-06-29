// File:      NIS_PointDrawer.h
// Created:   11.05.12
// Author:    Natalia Kopnova
// Copyright: LPKF Laser and Electronics AG 2012

#ifndef NIS_PointDrawer_HeaderFile
#define NIS_PointDrawer_HeaderFile

#include <NIS_Drawer.hxx>

class NIS_Point;

/**
 * NIS drawer for AISObject_Point interactive object type.
 * @ingroup aisobject
 */

class NIS_PointDrawer : public NIS_Drawer
{
 public:
  // ---------- PUBLIC METHODS ----------

  /**
   * Constructor.
   */
  Standard_EXPORT NIS_PointDrawer (const Quantity_Color& theColor);

  /**
   * Destructor.
   */
  Standard_EXPORT virtual ~NIS_PointDrawer ();

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

  /**
   * Draw single point represented as texture at input position 
   */
  Standard_EXPORT virtual void DrawPoint (const Standard_ShortReal* thePosition);


 private:
  // ---------- PRIVATE FIELDS ----------

  Standard_ShortReal  myPointSize;
  Standard_Integer    myType;
  Standard_Integer    mySizeIndex;
  Standard_Boolean    myInvertColor;
  Standard_Boolean    myIsWhite;
  Standard_Boolean    myIsUpdateAngle;
  Standard_Real       myAngle;
  unsigned char*      myRotatedRaster;


 public:
// Declaration of CASCADE RTTI
DEFINE_STANDARD_RTTI (NIS_PointDrawer)

  friend class NIS_Point;
};

// Definition of HANDLE object using Standard_DefineHandle.hxx
DEFINE_STANDARD_HANDLE (NIS_PointDrawer, NIS_Drawer)


#endif
