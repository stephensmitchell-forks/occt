// File:      NIS_Point.h
// Created:   11.05.12
// Author:    Natalia Kopnova
// Copyright: LPKF Laser and Electronics AG 2012

#ifndef NIS_Point_HeaderFile
#define NIS_Point_HeaderFile

#include <NIS_InteractiveObject.hxx>

//! Marker type enumeration.
//! @ingroup aisobject
enum NIS_MarkerType {
    NIS_MT_POINT = 0,
    NIS_MT_PLUS,
    NIS_MT_STAR,
    NIS_MT_CROSS,
    NIS_MT_CIRCLE,
    NIS_MT_FILLED_CIRCLE,
    NIS_MT_ARROW_RIGHT,
    NIS_MT_ARROW_UP,
    NIS_MT_ARROW_LEFT,
    NIS_MT_ARROW_DOWN,
    NIS_MT_ARROW_UP_RIGHT,
    NIS_MT_ARROW_UP_LEFT,
    NIS_MT_ARROW_DOWN_LEFT,
    NIS_MT_ARROW_DOWN_RIGHT,
    NIS_MT_SQUARE,
    NIS_MT_TRIANGLE,
    NIS_MT_CIRCLE_BOLD,
    NIS_MT_CROSS_BOLD,
    NIS_MT_ROMB,
    NIS_MT_CROSS_CIRCLE,
    NIS_MT_LINE
};

/**
 * Interactive Object for 2D/3D points
 * @ingroup aisobject
 */

class NIS_Point : public NIS_InteractiveObject
{
public:
 // ---------- PUBLIC METHODS ----------

  /**
   * Constructor for 2D point.
   */
  Standard_EXPORT NIS_Point (const gp_XY&        thePoint,
                                   const Standard_Real theZ);

  /**
   * Constructor for 3D point.
   */
  Standard_EXPORT NIS_Point (const gp_XYZ& thePoint);

  /**
   * Destructor.
   */
  Standard_EXPORT virtual ~NIS_Point ();

  /**
   * Re-initialize the internal structures, re-compute the presentation
   * If only Z is changed, then recompute is much faster.
   */
  Standard_EXPORT void      Init(const gp_XY&        thePoint,
                                 const Standard_Real theZ);

  /**
   * Re-initialize the internal structures, re-compute the presentation
   */
  Standard_EXPORT void      Init(const gp_XYZ& thePoint);

  /**
   * Create a default drawer instance.
   */
  Standard_EXPORT virtual NIS_Drawer*
                            DefaultDrawer  (NIS_Drawer*) const;


  /**
   * Method required by NIS_InteractiveContext.
   */
  Standard_EXPORT virtual void Clone (const Handle_NCollection_BaseAllocator&,
                                      Handle_NIS_InteractiveObject&) const;

  /**
   * Return coordinates of the Point 
   */
  inline gp_Pnt GetLocation() const
  {
    return gp_Pnt(myCoords[0], myCoords[1], myCoords[2]);
  }

  /**
   * Set the size of point presentations in pixels.
   * @param theSize
   *   New point size to use for the presentation.
   */
  Standard_EXPORT void      SetPointSize   (const Standard_Real    theSize);

  /**
   * Set the type of point presentations.
   * @param theType
   *   Type of marker to use for the presentation.
   */
  Standard_EXPORT void      SetMarkerType  (const NIS_MarkerType theType);

  /**
   * Set whether the point color should be inverted depending on
   * the color of underlying objects.
   * @param theInvert
   *   True to invert color.
   */
  Standard_EXPORT void      SetInvertColor (const Standard_Boolean theInvert);
  
  /**
   * Set the angle of the bitmap rotation.
   * New bitmap will be generated if theAngle is not equal to zero.
   * @param theAngle
   *   The angle of the bitmap rotation.
   */
  Standard_EXPORT void      SetAngle       (const Standard_Real theAngle);

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
   * Query if the Point is a 3D object.
   */
  inline Standard_Boolean Is3D             () const
  {
    return myIs3D;
  }


 protected:

  //! Empty constructor for method Clone()
  inline NIS_Point()
  {}

  /**
   * Create a 3D bounding box of the object.
   */
  Standard_EXPORT virtual void  computeBox      ();

 private:
  // ---------- PRIVATE METHODS ----------
  NIS_Point (const NIS_Point& theOther);
  NIS_Point& operator= (const NIS_Point& theOther);

 protected:
  // ---------- PRIVATE FIELDS ----------

  Standard_Boolean              myIs3D;
  Standard_Real                 myZ;

  /**
   * Array of coords
   */
  Standard_ShortReal            myCoords[3];


public:
// Declaration of CASCADE RTTI
DEFINE_STANDARD_RTTI (NIS_Point)

  friend class NIS_PointDrawer;
};

// Definition of HANDLE object using Standard_DefineHandle.hxx
DEFINE_STANDARD_HANDLE (NIS_Point, NIS_InteractiveObject)

#endif
