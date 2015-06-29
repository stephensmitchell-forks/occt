// Created on: 2007-07-06
// Created by: Alexander GRIGORIEV
// Copyright (c) 2007-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and / or modify it
// under the terms of the GNU Lesser General Public version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#ifndef NIS_View_HeaderFile
#define NIS_View_HeaderFile

#include <Handle_OpenGl_Context.hxx>
#include <Handle_NIS_InteractiveObject.hxx>
#include <Bnd_B3f.hxx>
#include <NCollection_List.hxx>
#include <NCollection_Vector.hxx>
#include <NIS_Drawer.hxx>
#include <NIS_SelectFilter.hxx>
#include <NIS_ViewData.hxx>
#include <Standard_DefineHandle.hxx>
#include <TCollection_AsciiString.hxx>
#include <TColStd_PackedMapOfInteger.hxx>
#include <V3d_OrthographicView.hxx>
#include <gp_XY.hxx>

class NIS_InteractiveContext;
class Handle_NIS_View;
class gp_Ax1;

/**
 * Manager of a single window with OpenGL context, used by one or more
 * NIS_InteractiveContext instances.
 * This class inherits V3d_OrthograpicView therefore its instances can be used
 * to display any object that is normally handled by Graphic3d/V3d/AIS classes.
 * Also the standard view operations: Pan, Rotate, Zoom, mouse API, etc. are
 * supported due to this inheritance.<p>
 * The new services added by this class is the visualisation of
 * NIS_InteractiveObject (i.e., accommodation of NIS_InteractiveContext
 * instances). This visualisation is done by the method MyCallback that is
 * called just before redrawing the scene by Graphic3d/OpenGl drivers.<p>
 * The View instance can be understood as a collector of several interactive
 * contexts that exist independently (do not interfere) though they output
 * into the same OpenGL context. The contexts are:
 * <ul>
 *  <li>AIS_InteractiveContext associated with the view by means of V3d_Viewer
 *  instance (the traditional AIS architecture);</li>
 *  <li>1st associated NIS_InteractiveContext</li>
 *  <li>2nd associated NIS_InteractiveContext</li>
 *  <li>...</li>
 * </ul>
 * The list of associated NIS_InteractiveContext can be modified by methods
 * AddContext and RemoveContext.
 * @ingroup nis_library
 */

class NIS_View : public V3d_OrthographicView
{
 public:
  // ---------- PUBLIC METHODS ----------


  /**
   * Attach the view to the window.
   */
  Standard_EXPORT NIS_View (const Handle_V3d_Viewer&    theViewer,
                            const Handle_Aspect_Window& theWindow = NULL);

  /**
   * Attach the view to the window.
   */
  Standard_EXPORT void SetWindow(const Handle_Aspect_Window &theWindow);

  /**
   * Indicate whether to draw hilighted objects on top of all other ones
   */
  inline void SetHilightOnTop(const Standard_Boolean theTop = Standard_True)
  { myIsTopHilight = theTop; }
  
  /**
   * Indicate whether to draw dynamic hilighted objects on top of all other ones
   */
  inline void SetDynHilightOnTop(const Standard_Boolean theTop = Standard_True)
  { myIsTopDynHilight = theTop; }

  /**
   * Indicate whether to hilight selected object dynamically
   * By default dynamic hilight works on all objects independently on its
   * selected/non-selected state.
   * This behaviour differs from the behaviour of AIS interactive context,
   * that doesn't hilight dynamically (on mouse movements) selected objects. 
   * In the case both context are used in the same view the behaviour of both
   * context can be made consistent by setting this flag to False
   */
  inline void SetDynHilightSelected     (const Standard_Boolean
                                         theHilight = Standard_True)
  { myDoHilightSelected = theHilight; }

  /**
   * Gets indentifier whether a selected object should be dynamically highlighted
   */
  inline Standard_Boolean GetDynHilightSelected () const
  { return myDoHilightSelected; }

  /**
   * Zoom the view to fit to visible objects size and positions.
   * @param theCoef
   *   Relative margin in both X and Y dimensions. For example, value 1.0
   *   will fit to twice the actual size.
   * @return
   *   True if operation performed, False if failed (most likely because of
   *   very big actual scale)
   */
  Standard_EXPORT Standard_Boolean FitAll3d (const Quantity_Coefficient theCoef
                                             = 0.01);

  /**
   * Get the reference to GL context.
   */
  inline const Handle_OpenGl_Context& GetGlContext() const
  { return myGlCtx; }

  /**
   * Get the reference to GL Data.
   * @return
   *   Handle to ViewData for the unique value of theKey.
   *   If theKey is unknown, a ViewData handle is created and added to the Map
   */
  Standard_EXPORT Handle_NIS_ViewData& GetViewData (const char * theKey);

  /**
   * Expressly close internal data structures, particularly OpenGl buffer
   * objects, that may be hard to close in usual way, using destructor.
   */
  Standard_EXPORT virtual void  Remove ();

  /**
   * Query the size of 1 pixel in the middle, in X and Y directions.
   */
  inline const gp_XY&           GetPixelSize    () const
  { return myPixelSize; }

  /**
   * Query the view size. X and Y directions components are half dimensions
   * of the view box. To obtain the view box you can use:
   * @code
   *   aViewBox.SetCenter(0., 0., 0.);
   *   aViewBox.SetHSize(aV->GetViewSize().X(), aV->GetViewSize().Y(), 10000.);
   * @endcode
   * The last parameter (10000) is arbitrary.
   */
  inline const gp_XY&           GetViewSize     () const
  { return myViewSize; }

  /**
   * Get the viewport values as returned by GL_VIEWPORT OpenGl call.
   * Normally, Index 0 and 1 - upper left corner, index 2 and 3 - width and
   * height in pixels.
   */
  inline Standard_Integer       Viewport        (const Standard_Integer i) const
  { return myViewport[i&3]; }

  /**
   * Check the input X, Y coordinates if they are outside the viewport and
   * if they happen to be then correct to match.
   * @return
   *   True if correction has been made
   */
  Standard_EXPORT  Standard_Boolean
                                ClipToViewport  (Standard_Integer& theX,
                                                 Standard_Integer& theY) const;

  /**
   * Get view transformation. If you apply it to a real-space object then the
   * object is moved to the coordinate system of the view box (see GetViewSize)
   * that has its center in the middle of the view area and is oriented
   * according to the current view matrix.
   */
  inline const gp_Trsf&         Transformation  () const
  { return myTrsf; }

  /**
   * Gets bounding box covering objects displayed in viewer.
   */
  Standard_EXPORT Bnd_B3f GetBndBox() const;

  /**
   * Gets bounding box covering objects displayed in viewer.
   * If operation is fails when Xmax < Xmin abd Ymax < Ymin
   */
  Standard_EXPORT void GetBndBox(Standard_Integer& theXMin,
                                 Standard_Integer& theXMax, 
                                 Standard_Integer& theYMin,
                                 Standard_Integer& theYMax ) const;

//   /**
//    * Destructor.
//    */
//   Standard_EXPORT virtual ~NIS_View ();

  /**
   * Set or unset the dynamically hilighted object, depending on the
   * given mouse coordinates.
   * @return True if hilighting/unhilighting takes place.
   */
  Standard_EXPORT Standard_Boolean  DynamicHilight
                                        (const Standard_Integer theX,
                                         const Standard_Integer theY);

  /**
   * Unhilights the hilighted object if it coincides with the given
   * object instance.
   */
  Standard_EXPORT void  DynamicUnhilight(const Handle_NIS_InteractiveObject&);

  /**
   * Unhilights the currently hilighted object.
   */
  inline void  DynamicUnhilight()  { DynamicUnhilight(myDynHilighted); }

  /**
   * Set or unset the selected (hilighted) state of the object that is under
   * the coordinates theX, theY.
   * @param theX
   *   X coordinate of the view window
   * @param theY
   *   X coordinate of the view window
   * @param isModifierUsed
   *   The selection modifier state. If the effect of multi-Selection should be
   *   forced in the Normal selection mode (the Shift is pressed).
   * @param theRedraw
   *   True to redraw view automatically (default value).
   */
  Standard_EXPORT void  Select          (const Standard_Integer theX,
                                         const Standard_Integer theY,
                                         const Standard_Boolean isModifierUsed
                                                         = Standard_False,
                                         const Standard_Boolean theRedraw
                                                         = Standard_True);

  /**
   * Set or unset the selected (hilighted) state of the objects that are
   * intersected by 2D box in the view
   * @param theXmin
   *   left X coordinate of the view window
   * @param theYmin
   *   lower Y coordinate of the view window
   * @param theXmax
   *   right X coordinate of the view window
   * @param theYmax
   *   upper Y coordinate of the view window
   * @param isModifierUsed
   *   The selection modifier state. If the effect of multi-Selection should be
   *   forced in the Normal selection mode (the Shift is pressed).
   * @param isFullyIncluded
   *   True if only those objects are processed that are fully inside the
   *   selection rectangle. False if objects fully or partially included in
   *   the rectangle are processed. 
   * @param theRedraw
   *   True to redraw view automatically (default value).
   */
  Standard_EXPORT void  Select          (const Standard_Integer  theXmin,
                                         const Standard_Integer  theYmin,
                                         const Standard_Integer  theXmax,
                                         const Standard_Integer  theYmax,
                                         const Standard_Boolean  isModifierUsed
                                                         = Standard_False,
                                         const Standard_Boolean isFullyIncluded
                                                         = Standard_False,
                                         const Standard_Boolean theRedraw
                                                         = Standard_True);

  /**
   * Set or unset the selected (hilighted) state of the objects that are
   * intersected by 2D polygon in the view
   * @param thePolygon
   *   defines the vertices of a free-form closed polygon without
   *   self-intersections. The last point should not coincide with the first
   *   point of the list. Points are interpreted as X and Y integer coordinates
   *   of the view window. Any two neighbor points should not be confused.
   * @param isModifierUsed
   *   The selection modifier state. If the effect of multi-Selection should be
   *   forced in the Normal selection mode (the Shift is pressed).
   * @param isFullyIncluded
   *   True if only those objects are processed that are fully inside the
   *   selection rectangle. False if objects fully or partially included in
   *   the rectangle are processed. 
   * @param theRedraw
   *   True to redraw view automatically (default value).
   */
  Standard_EXPORT void  Select   (const NCollection_List<gp_XY> &thePolygon,
                                  const Standard_Boolean         isModifierUsed
                                                         = Standard_False,
                                  const Standard_Boolean         isFullyIncluded
                                                         = Standard_False,
                                  const Standard_Boolean theRedraw
                                                         = Standard_True);

  /**
   * Computation of 3D axis for objects selection
   * @param theX
   *   X coordinate of mouse pointer in the window.
   * @param theY
   *   Y coordinate of mouse pointer in the window.
   * @param theOver
   *   Overlap for the returned 3D axis
   * @return
   *   3D axis for objects selection
   */
  Standard_EXPORT gp_Ax1 PickAxis       (const Standard_Integer theX,
                                         const Standard_Integer theY,
                                         Standard_Real&         theOver);

  /**
   * Interactive selection by mouse click. Selection itself is performed in each
   * Interactive Context attached to this View, accordingto the local parameters
   * (mode, filter) of selection in each context. 
   * @param theX
   *   X coordinate of mouse pointer in the window.
   * @param theY
   *   Y coordinate of mouse pointer in the window.
   * @param theEvent
   *   Event that invokes this method
   * @return
   *   the selected object picked by the minimal intersection distance among
   *   all contexts attached to this View.
   */
  Standard_EXPORT Handle_NIS_InteractiveObject
                        Pick            (const Standard_Integer theX,
                                         const Standard_Integer theY,
                                         const NIS_SelectFilter::Event theEvent
                                         = NIS_SelectFilter::Indefinite);

  /**
   * Interactive selection by mouse click. Selection itself is performed in each
   * Interactive Context attached to this View, according to the local parameters
   * (mode, filter) of selection in each context. 
   * @param theAxis
   *   3D axis for objects selection
   * @param theOver
   *   Overlap for the selecting axis
   * @param isOnlySel
   *   If False, any displayed object can be picked, otherwise only selectable
   *   ones.
   * @param pDistance
   *   Optional parameter, if defined then the shortest distance for picked
   *   object along the axis to projection point will be stored in it.
   * @param theEvent
   *   Event that invokes this method
   * @return
   *   the selected object picked by the minimal intersection distance among
   *   all contexts attached to this View.
   */
  Standard_EXPORT Handle_NIS_InteractiveObject
                        Pick            (const gp_Ax1&          theAxis,
                                         const Standard_Real    theOver,
                                         const Standard_Boolean isOnlySel,
                                         Standard_Real*         pDistance = 0L,
                                         const NIS_SelectFilter::Event theEvent
                                         = NIS_SelectFilter::Indefinite);

  /**
   * Having pixel coordinates (X, Y starting from the top left corner) returns
   * 3D coordinates of the corresponding point on view plane.
   */
  Standard_EXPORT gp_XYZ UnConvert      (const Standard_Integer theX,
                                         const Standard_Integer theY) const;

  /**
   * Called outside the loop of refresing the view. Makes this OpenGl context
   * current for the actual thread.
   * @return False if failed.
   */
  Standard_EXPORT Standard_Boolean
                         MakeCurrent    () const;

  /**
   * Gets all objects detected by last call of Pick() method
   */
  inline NCollection_Vector<NIS_InteractiveObject *> GetDetected() const
  { return myDetected; }

  /**
   * Obtain the IDs of ex-lists.
   */
  inline TColStd_PackedMapOfInteger& GetExListId ()
  { return myExListId; }

  /**
   * Get all contexts of view
   */
  inline NCollection_List<NIS_InteractiveContext *> GetContexts () const
  { return myContexts; }

  inline const Handle_NIS_InteractiveObject& GetDynHilighted() const
  { return myDynHilighted; }

 protected:
  // ---------- PROTECTED METHODS ----------

  static int MyCallback (Aspect_Drawable                /* Window ID */,
                         void*                          /* user data */, 
                         Aspect_GraphicCallbackStruct*  /* call data */);

  Standard_EXPORT void redrawCallback();

  /**
   * Redraw all interactive contexts.
   */
  Standard_EXPORT void redraw (const Handle_NIS_View&     theView,
                               const NIS_Drawer::DrawType theType);

  /**
   * Register an interactive context for this view.
   */
  Standard_EXPORT void  AddContext      (NIS_InteractiveContext *);

  /**
   * Detach an interactive context from this view.
   */
  Standard_EXPORT void  RemoveContext   (NIS_InteractiveContext *);

 private:
  // ---------- PRIVATE FIELDS ----------
  struct NamedViewData {
    TCollection_AsciiString Name;
    Handle_NIS_ViewData     Data;
  };

  NCollection_List<NIS_InteractiveContext *>       myContexts;
  Handle_NIS_InteractiveObject                     myDynHilighted;
  Standard_Boolean                                 myIsTopHilight      : 1;
  Standard_Boolean                                 myIsTopDynHilight   : 1;
  Standard_Boolean                                 myDoHilightSelected : 1;
  Standard_Boolean                                 myIsDrawArea        : 1;
  NCollection_Vector<NIS_InteractiveObject *>      myDetected;
  TColStd_PackedMapOfInteger                       myExListId;
  Handle_OpenGl_Context                            myGlCtx;

  Standard_Integer                                 myViewport[4];
  gp_XY                                            myPixelSize;
  gp_XY                                            myViewSize;
  gp_XYZ                                           myProj;  // view Z direction
  gp_Trsf                                          myTrsf;
  NCollection_List<struct NamedViewData>           myViewData;

  void *                                           myHDC;
  void *                                           myHGLRC;

  friend class NIS_InteractiveContext;

 public:
// Declaration of CASCADE RTTI
DEFINE_STANDARD_RTTI (NIS_View)
};

// Definition of HANDLE object using Standard_DefineHandle.hxx
//DEFINE_STANDARD_HANDLE (NIS_View, V3d_OrthographicView)

#include <Handle_NIS_View.hxx>

#endif
