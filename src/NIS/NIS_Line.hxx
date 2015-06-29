// File:      NIS_Line.h
// Created:   11.05.12
// Author:    Natalia Kopnova
// Copyright: LPKF Laser and Electronics AG 2012

#ifndef NIS_Line_HeaderFile
#define NIS_Line_HeaderFile

#include <NIS_InteractiveObject.hxx>
#include <NIS_LineDrawer.hxx>
#include <gp_Ax1.hxx>

class Handle_NIS_InteractiveContext;

//! Line style enumeration
//! @ingroup aisobject
enum NIS_LineStyle {
    NIS_LS_Solid = 0,
    NIS_LS_Dash,
    NIS_LS_Dot,
    NIS_LS_DotDash,
    NIS_LS_DoubleDotDash
};

//! Line type enumeration
//! @ingroup aisobject
enum NIS_LineType {
    NIS_LT_Line = 0,
    NIS_LT_Ray
};

/**
 * Interactive Object for 2D/3D lines
 * @ingroup aisobject
 */

class NIS_Line : public NIS_InteractiveObject
{
 public:
  // ---------- PUBLIC METHODS ----------

  /**
   * Constructor.
   */
  Standard_EXPORT NIS_Line (const gp_Ax1& theAxis);

  /**
   * Destructor.
   */
  Standard_EXPORT virtual ~NIS_Line ();

  /**
   * Re-initialize the internal structures, re-compute the presentation
   */
  Standard_EXPORT void      Init           (const gp_Ax1& theAxis);

  /**
   * Create a default drawer instance.
   */
  Standard_EXPORT virtual NIS_Drawer*
                            DefaultDrawer  (NIS_Drawer*) const;


  /**
   * Method required by NIS_InteractiveContext.
   */
  Standard_EXPORT virtual void
                            Clone    (const Handle_NCollection_BaseAllocator&,
                                      Handle_NIS_InteractiveObject&) const;

  /**
   * Set the line width in pixels.
   * @param theSize
   *   New line width to use for the presentation.
   */
  Standard_EXPORT void      SetLineWidth   (const Standard_Real theSize);

  /**
   * Set the line style.
   * @param theStyle
   *   Line style to be used for the presentation.
   */
  Standard_EXPORT void      SetLineStyle   (const NIS_LineStyle theStyle);

   /**
   * Set the line type.
   * @param theType
   *   Line type to be used for the presentation.
   */
  Standard_EXPORT void      SetLineType   (const NIS_LineType theType);

  /**
   * Set whether line color should be inverted depending on color of
   * underlying objects.
   * @param theInvert
   *   True to invert color.
   */
  Standard_EXPORT void      SetInvertColor (const Standard_Boolean theInvert);

  /**
   * Intersect the InteractiveObject geometry with an oriented box.
   * @param theBox
   *   3D box of selection
   * @param theTrf
   *   Position/Orientation of the box.
   * @param isFull
   *   True if full inclusion is required, False - if partial.
   * @return
   *   True if the InteractiveObject geometry intersects the box or is inside it
   */
  Standard_EXPORT virtual Standard_Boolean
                            Intersect      (const Bnd_B3f&         theBox,
                                            const gp_Trsf&         theTrf,
                                            const Standard_Boolean isFull)const;
  /**
   * Intersect the InteractiveObject geometry with a line/ray.
   * @param theAxis
   *   The line or ray in 3D space.
   * @param theOver
   *   Half-thickness of the selecting line.
   * @return
   *   If the return value is more than 0.1*RealLast() then no intersection is
   *   detected. Otherwise returns the coordinate of thePnt on the ray. May be
   *   negative.
   */
  Standard_EXPORT virtual Standard_Real
                            Intersect      (const gp_Ax1&         theAxis,
                                            const Standard_Real   theOver)const;

  /**
   * Intersect the InteractiveObject geometry with a selection polygon.
   * @param thePolygon
   *   the list of vertices of a free-form closed polygon without
   *   self-intersections. The last point should not coincide with the first
   *   point of the list. Any two neighbor points should not be confused.
   * @param theTrf
   *   Position/Orientation of the polygon.
   * @param isFull
   *   True if full inclusion is required, False - if partial.
   * @return
   *   True if the InteractiveObject geometry intersects the polygon or is
   *   inside it
   */
  Standard_EXPORT virtual Standard_Boolean Intersect
                    (const NCollection_List<gp_XY> &thePolygon,
                     const gp_Trsf                 &theTrf,
                     const Standard_Boolean         isFull) const;

  /**
   * Store an integer value as attribute of the Object.
   */
  inline void             SetAttributeId   (const Standard_Integer theId)
  {
    SetAttribute (reinterpret_cast <void*> (theId));
  }

  /**
   * Returns the attribute value previously stored using SetAttributeId.
   * Value 0 signifies that the AttributeId has not been set (as this is
   * the constructor default value).
   */
  inline Standard_Integer GetAttributeId   () const
  {
    return reinterpret_cast <Standard_Integer> (GetAttribute());
  } 

  /**
   * Query the point coordinates.
   */
  inline const Standard_ShortReal* GetPoint (const Standard_Integer ind) const
  {
    return (ind & 0x1) ? &myPoint1[0] : &myPoint2[0];
  }

 protected:

  /**
   * Create a 3D bounding box of the object.
   * Does nothing because line is considered as infinitive
   */
  Standard_EXPORT virtual void  computeBox      ()
  {}

 private:
  // ---------- PRIVATE METHODS ----------
  NIS_Line (const NIS_Line& theOther);
  NIS_Line& operator= (const NIS_Line& theOther);

  static Handle_NIS_LineDrawer defaultDrawer();

 private:
  // ---------- PRIVATE FIELDS ----------

  gp_Ax1                        myLine;

  /**
   * Array of coords
   */
  Standard_ShortReal            myPoint1[3];
  Standard_ShortReal            myPoint2[3];

 public:
// Declaration of CASCADE RTTI
DEFINE_STANDARD_RTTI (NIS_Line)

  friend class NIS_LineDrawer;
};

// Definition of HANDLE object using Standard_DefineHandle.hxx
DEFINE_STANDARD_HANDLE (NIS_Line, NIS_InteractiveObject)


#endif
