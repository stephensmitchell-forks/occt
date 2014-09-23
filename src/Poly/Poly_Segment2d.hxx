// File:        Poly_Segment2d.hxx
// Created:     01.07.2010
// Author:      Sergey KUUL
// Copyright:   Open Cascade 2010

#ifndef Poly_Segment2d_HeaderFile
#define Poly_Segment2d_HeaderFile

#include <gp_XY.hxx>
#include <gp_XYZ.hxx>
#include <gp_Trsf2d.hxx>
#include <NCollection_BaseAllocator.hxx>

class Poly_PolyCurve2d;

/**
 * This class describes a 2D segment between two points
 * to be stored in Poly_PolyCurve2d
 */

class Poly_Segment2d
{
 public:
  // ---------- PUBLIC METHODS ----------

  /**
   * Empty constructor.
   */
  Poly_Segment2d() : myNext(0L), myPrevious(0L),
                     myCurve(0L), myInternal (Standard_False)
  {
    myPnt[0].SetCoord(0.,0.);
    myPnt[1].SetCoord(0.,0.);
  }

  /**
   * Constructor
   */
  Poly_Segment2d(const gp_XY& theStart, const gp_XY& theEnd)
    : myNext(0L), myPrevious(0L), myCurve(0L), myInternal (Standard_False)
  {
    myPnt[0] = theStart;
    myPnt[1] = theEnd;
  }

  /**
   * Copy constructor
   */
  Standard_EXPORT Poly_Segment2d(const Poly_Segment2d& theOther)
    : myNext(theOther.myNext), myPrevious(theOther.myPrevious),
    myCurve(theOther.myCurve), myInternal(theOther.myInternal)
  {
    myPnt[0] = theOther.myPnt[0];
    myPnt[1] = theOther.myPnt[1];
    myNormal = theOther.myNormal;
  }

  /**
   * Assign operator
   */
  Standard_EXPORT Poly_Segment2d& operator = (const Poly_Segment2d& theOther)
  {
    myNext      = theOther.myNext;
    myPrevious  = theOther.myPrevious;
    myCurve     = theOther.myCurve;
    myPnt[0]    = theOther.myPnt[0];
    myPnt[1]    = theOther.myPnt[1];
    myNormal    = theOther.myNormal;
    myInternal  = theOther.myInternal;
    return *this;
  }

  /**
   * Destructor, it removes this segment from lists of vertices if any
   */
  Standard_EXPORT ~Poly_Segment2d ();

  /**
   * Allocator-based operator new for dynamic allocations
   */
  DEFINE_NCOLLECTION_ALLOC

  /**
   * Get the next segment in the list.
   */
  inline const Poly_Segment2d& Next() const
  { return * myNext; }

  /**
   * Get the previous segment in the list.
   */
  inline const Poly_Segment2d& Previous() const
  { return * myPrevious; }

  /**
   * Query the ancestor curve
   */
  inline Poly_PolyCurve2d* PolyCurve2d() const
  { return myCurve; }

  /**
   * Set the ancestor curve
   */
  inline void SetCurve(Poly_PolyCurve2d* theCurve)
  { myCurve = theCurve; }

  /**
   * Query the extremity of this Segment.
   * @param i
   *   0 defines the beginning of the Segment, 1 -- its end.
   */
  const gp_XY& Point(const int i) const
  { return myPnt[i&0x1]; }

  /**
   * Change coordinates of the extremity of this Segment.
   * @param i
   *   0 defines the beginning of the Segment, 1 -- its end.
   */
  void SetPoint (const int i, const gp_XY& thePnt)
  { myPnt[i&0x1] = thePnt; }

  /**
   * Query the vertex at extremity of this Segment.
   * @param i
   *   0 defines the beginning of the Segment, 1 -- its end.
   */
  //Poly_Vertex* Vertex (const int i) const;

  /**
   * Set the vertex at extremity of this Segment.
   * @param i
   *   0 defines the beginning of the Segment, 1 -- its end.
   */
  //Standard_EXPORT void SetVertex (const int i, Poly_Vertex*);

  /**
   * Returns the tangent direction of the Segment.
   */
  Standard_EXPORT gp_XY Dir() const;

  /**
   * Set the normal direction to the surface
   */
  Standard_EXPORT void SetNormal(const gp_XYZ& theDir)
  { myNormal = theDir; }

  /**
   * Copy the normal direction from the other segment.
   */
  inline void CopyNormal (const Poly_Segment2d& theOther)
  { myNormal = theOther.myNormal; }

  /**
   * Get the normal direction to the surface
   */
  Standard_EXPORT Standard_Boolean GetNormal (gp_XYZ& theDir) const;

  /**
   * Calculate the 3d coordinates of end points, having the type of section
   */
  //Standard_EXPORT void Restore3d (const Poly_SectionType theType,
  //                                const Standard_Real theAbsc,
  //                                gp_XYZ& theP0,
  //                                gp_XYZ& theP1) const;

  /**
   * Compute the distance between the Segment extremities.
   * @return
   *   the length of the curve segment.
   */
  inline Standard_Real Length() const
  { return (myPnt[0] - myPnt[1]).Modulus(); }

  /**
   * Compute the signed area limited by the Segment and two segments:
   * P0 to Start and P0 to End.
   */
  inline Standard_Real Area(const gp_XY& P0) const
  { return 0.5 * ((myPnt[0] - P0) ^ (myPnt[1] - P0)); }

  /**
   * Transform the segment.
   */
  inline void Transform (const gp_Trsf2d& theTrsf)
  {
    theTrsf.Transforms (myPnt[0]);
    theTrsf.Transforms (myPnt[1]);
  }

  /**
   * Compute interpolated point on the segment.
   * @param theParam
   *   If outside the interval [0,1], the value 0 or 1 is used.
   * @param thePoint
   *   <tt>[out]</tt> Result of interpolation.
   */
  Standard_EXPORT void Interpolate(const Standard_Real theParam,
                                   gp_XY& thePoint) const;

  /**
   * Split the segment creating a smaller one.
   * @param theRes
   *  <tt>[out]</tt> The result of operation, new segment created.
   * @param theAbsc
   *  Abscisse of the splitting point
   * @param isFirst
   *  Defines the part of the split segment that is returned as theRes: True
   *  means the 1st part, False - the 2nd part from the beginning.
   * @param theTimer
   *  the identificator of the moment of modification, passed to the result.
   * @return
   *  True if operation successful, False otherwise (e.g. if theAbsc irrelevant)
   */
  //Standard_EXPORT Standard_Boolean Split(Poly_Segment2d&        theRes,
  //                                       const Standard_Real    theAbsc,
  //                                       const Standard_Boolean isFirst,
  //                                       const Standard_Integer theTimer) const;

  /**
   * Similar to Split() but the result consists in modification of the current
   * Segment instance, rather than creation of a new instance.
   */
  //Standard_EXPORT Standard_Boolean Delimit(const Standard_Real    theAbsc,
  //                                         const Standard_Boolean isFirst);

  /**
   * Divides this segment onto two parts using the given parameter.
   * This accepts the left part, while a new one is created for the second part.
   * The new segment is inserted into the sequence in the proper place.
   * theTimer is the identificator of the moment of modification.
   * Returns false and does nothing if the parameter is not in interval ]0,1[
   */
  Standard_EXPORT Standard_Boolean Divide(const Standard_Real theParam);

  /**
   * Projection of a point on the Segment.
   * @return
   *   Abscise of the projection in the range [0 .. 1].
   */
  Standard_EXPORT Standard_Real Project
    (const gp_XY& thePnt,
     const Standard_Boolean theRestrict = Standard_True) const;

  /**
   * Sets "internal" flag of the segment
   */
  void SetInternal(const Standard_Boolean theFlag)
  { myInternal = theFlag; }

  /**
   * Returns true if the segment has "internal" flag
   */
  Standard_Boolean IsInternal() const;

  /**
   * Diagnostic dump of the segment
   */
  Standard_EXPORT void Dump (Standard_OStream& theStream) const;

 private:
  // ---------- PRIVATE FIELDS ----------

  //! Extremities of the 2D segment.
  gp_XY                 myPnt[2];

  //! Normal direction to the surface of rest material
  gp_XYZ                myNormal;

  //! Next segment in the list
  const Poly_Segment2d* myNext;

  //! Previous segment in the list
  const Poly_Segment2d* myPrevious;

  //! Pointer to the curve containing this segment
  Poly_PolyCurve2d*     myCurve;

  //! Flag IsInternal
  Standard_Boolean      myInternal : 1;

  friend class Poly_PolyCurve2d;    // management of Next, Previous in lists 
  friend class Poly_Seg2dIterator;
};

#endif
