// File:      NIS_PolygonSelector.hxx
// Created:   26.10.11 21:17
// Author:    Alexander GRIGORIEV
// Copyright: Open Cascade 2011


#ifndef NIS_PolygonSelector_HeaderFile
#define NIS_PolygonSelector_HeaderFile

#include <NIS_InteractiveObject.hxx>
#include <NCollection_List.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_Handle.hxx>

class NIS_PolygonSelectorDrawer;

typedef NCollection_Array1<Standard_ShortReal> ShortRealArray;
typedef NCollection_Handle<ShortRealArray> Handle(ShortRealArray);

/**
 * Data structure and presentation of a polygon or rectangle shown above all
 * other objects in 3D viewer. Can be used for interactive seclection.
 * @ingroup nis_library
 */

class NIS_PolygonSelector : public NIS_InteractiveObject
{
 public:
  // ---------- PUBLIC METHODS ----------

  /**
   * Constructor.
   * @param isRectangle
   *   If True then a rectangle is drawn by 2 points, if False - a polygon on
   *   a set of points.
   * @param isLoop
   *   This parameter makes sence only for polygon mode.
   *   If 'false' the line between the first and the last points is nopt displayed.
   *   otherwise these points are connected.
   * @param theView
   *   If defined then the polygon/rectangle will be shown only in tthis view.
   *   If undefined then all views will contain the presentation.
   */
  inline NIS_PolygonSelector (const Standard_Boolean isRectangle,
                              const Standard_Boolean isLoop = 1,
                              const Handle_NIS_View& theView = 0L)
  : myView        (theView),
    myIsRectangle (isRectangle),
    myIsLoop      (isLoop) {}

  /**
   * Query the number of polygon vertices.
   */ 
  inline Standard_Integer NPoints       () const
  { return myIsRectangle ? 4 : myPoints.Extent(); }

  /**
   * Query the drawing mode.
   */
  inline Standard_Boolean IsRectangle   () const
  { return myIsRectangle; }

  Standard_EXPORT ~NIS_PolygonSelector ();

  Standard_EXPORT virtual NIS_Drawer *
                          DefaultDrawer (NIS_Drawer * theDrv) const;

  /**
   * Create a copy of theObject except its ID.
   */
  Standard_EXPORT virtual void
                          Clone (const Handle_NCollection_BaseAllocator& theAll,
                                 Handle_NIS_InteractiveObject& theDest) const;

  Standard_EXPORT virtual Standard_Real
                          Intersect     (const gp_Ax1&       theAxis,
                                         const Standard_Real theOver)const;
  Standard_EXPORT virtual void
                          computeBox    ();

  /**
   * Add a point to polygon.
   */
  Standard_EXPORT void    AddPoint      (const Standard_Integer theX,
                                         const Standard_Integer theY);

  /**
   * Change the location of the last added point.
   */
  Standard_EXPORT void    MovePoint     (const Standard_Integer theX,
                                         const Standard_Integer theY);
  /**
   * Remove the last added point.
   */
  Standard_EXPORT void    RemovePoint   ();

  /**
   * Perform the selection in the current view (myView). If myView.IsNull() then
   * the selection is performed in the first view attached to the Interactive
   * context. The selection is only performed for NIS_InteractiveObjects; for
   * AIS_InteractiveObject type use additional selection call.
   * @param isModifierUsed
   *    The modifier buttons state (e.g., when Shift is pressed).
   * @param isFulInc
   *    Tells if only those objects are selected that are fully inside the
   *    selection area.
   * @param isRedraw
   *   True to redraw view automatically (default value).
   */
  Standard_EXPORT void    Perform       (const Standard_Boolean isModifierUsed,
                                         const Standard_Boolean isFulInc,
                                         const Standard_Boolean isRedraw = Standard_True) const;

  Standard_EXPORT NCollection_List<gp_XY> GetPolygon    () const;

 private:

     const Handle_NIS_View&     getView         () const;

     gp_XYZ                     convertPoint    (const Standard_Integer theX,
                                                 const Standard_Integer theY);
     void                       updateVertexes  ();

  // ---------- PRIVATE FIELDS ----------

  Handle_NIS_View              myView;
  NCollection_List<gp_XYZ>     myPoints;
  Handle(ShortRealArray)       myVertexes;
  Standard_Boolean             myIsRectangle;
  Standard_Boolean             myIsLoop;

 public:
// Declaration of CASCADE RTTI
DEFINE_STANDARD_RTTI (NIS_PolygonSelector)
  friend class NIS_PolygonSelectorDrawer;
};

// Definition of HANDLE object using Standard_DefineHandle.hxx
DEFINE_STANDARD_HANDLE (NIS_PolygonSelector, NIS_InteractiveObject)

#endif
