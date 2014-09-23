// File:        Poly_PolyCurve2d.hxx
// Created:     01.07.2010
// Author:      Sergey KUUL
// Copyright:   Open Cascade 2010

#ifndef Poly_PolyCurve2d_HeaderFile
#define Poly_PolyCurve2d_HeaderFile

#include <Poly_Seg2dIterator.hxx>
#include <NCollection_List.hxx>
#include <gp_Ax3.hxx>

#include <Handle_Poly_PolyCurve2d.hxx>

typedef NCollection_List<Handle(Poly_PolyCurve2d)> Poly_ListOfCurve2d;
template <class A> class NCollection_Vector;

/**
 * Curve as a double-linked list of 2D segments.
 * Call the static method Create instead of constructor.
 */

class Poly_PolyCurve2d : public Standard_Transient
{
 public:

  enum CurveType
  {
    CurveType_Indefinite = 0,
    CurveType_ExternalBoundary,
    CurveType_InternalBoundary,
    CurveType_SetOfBoundaries
  };

  typedef enum {
    Class2d_Indefinite         = 0,
    Class2d_ON                 = 1,
    Class2d_IN                 = 2,
    Class2d_OUT                = 3
  } Class2d;

  /**
   * Struct describing location of a point on curve
   */
  struct Location
  {
    const Poly_Segment2d * pSeg;
    Standard_Real          Param;
    Standard_Integer       SegNum;
    inline Location () : pSeg (NULL), Param (0.), SegNum (0) {}
    inline Standard_Boolean operator < (const Location& theOther) const
    { return SegNum < theOther.SegNum ||
        SegNum == theOther.SegNum && Param < theOther.Param; }
    Standard_Boolean IsEqual (const Location&     theOther,
                              const Standard_Real theConf) const;
  };

  // ---------- PUBLIC METHODS ----------

  /**
   * Method replacing both "operator new" and constructor.
   * Use it instead of calling the constructor.
   */ 
  static Standard_EXPORT Poly_PolyCurve2d*
    Create (const Handle_NCollection_BaseAllocator& theAlloc = 0L,
            const CurveType theType = CurveType_Indefinite);

  /**
   * Create a copy of this Curve, using the given allocator.
   * @param theAlloc
   *   Allocator for the new copy of the curve
   * @param isReverse
   *   True means that the curve and all its children should be reversed in
   *   the created copy.
   */
  Standard_EXPORT Poly_PolyCurve2d * Clone
    (const Handle_NCollection_BaseAllocator& theAlloc,
     const Standard_Boolean  isReverse = Standard_False) const;

  /**
   * Destructor.
   */
  Standard_EXPORT virtual ~Poly_PolyCurve2d();

  /**
   * Query the area of the contour.
   * @return
   *   Negative if the contour is not closed (i.e., its area is nonsense)
   * If called after modification, this method recomputes the contour anew.
   */ 
  Standard_EXPORT Standard_Real Area();

  /**
   * Query the perimeter/length of the contour.
   * If called after modification, this method recomputes the contour anew.
   */ 
  Standard_EXPORT Standard_Real Perimeter();

  /**
   * Calculate the point on the goven distance from the beginning of the Curve.
   * @param theAbsc
   *   Abascisse (distance from the beginning of the curve) of the requested
   *   point.
   */ 
  Standard_EXPORT gp_XY Point(const Standard_Real anAbsc) const;

  /**
   * Query the Closedness of the contour.
   */ 
  Standard_EXPORT Standard_Boolean IsClosed(const Standard_Real theTol=0.) const;

  /**
   * Query the CCW attribute.
   */
  Standard_EXPORT Standard_Boolean IsCounterClockwise ();

  /**
   * Check if the curve is closed.
   * If it is not closed but within the given tolerance, try to correct it.
   * @param theTolMove
   *   Distance on which it is allowed to move an end of a segment in order
   *   to close the Curve.
   * @param theTolClo
   *   Maximal distance (generally greater than theTolMove) that can be
   *   closed by a new segment. 
   * @return
   *   True if the curve after this call becomes closed (including the case
   *   when it was originally closed).
   */
  Standard_EXPORT Standard_Boolean CloseCurve(const Standard_Real theTolMove,
                                              const Standard_Real theTolClo);

  /**
   * Query the children of this contour (non-const interface).
   */ 
  Poly_ListOfCurve2d& ChangeChildren()
  { return myChildren; }

  /**
   * Query the children of this contour (const interface).
   */ 
  const Poly_ListOfCurve2d& Children() const
    { return myChildren; }

  /**
   * Create iterator object that provides access to the list of Segments.
   */
  Poly_Seg2dIterator SegmentIterator
  (const Poly_Seg2dIterator::Sense theSense = Poly_Seg2dIterator::Forward) const
  {
    return Poly_Seg2dIterator
      (*(theSense != Poly_Seg2dIterator::Reverse || IsClosed()
         ? mySegments
         : mySegments->myPrevious), theSense);
  }

  /**
   * Query the number of curve segments in this curve.
   */
  Standard_Integer NSegments () const
  { return myNSegments; }

  /**
   * Query the first element.
   */
  inline const Poly_Segment2d&     FirstElement  () const
  {
    return *mySegments;
  }

  /**
   * Query the last element.
   */
  inline const Poly_Segment2d&     LastElement  () const
  {
    return mySegments->Previous();
  }

  /**
   * Append a contour to the children of this one.
   */
  const Handle_Poly_PolyCurve2d& AddChildCurve(const Handle_Poly_PolyCurve2d& crv)
  { return myChildren.Append(crv); }

  /**
   * Find the given curve in the list of children and remove it.
   * @return
   *  True if the child was found and removed, False otherwise.
   */
  Standard_EXPORT Standard_Boolean RemoveChildCurve (const Handle_Poly_PolyCurve2d&);

  /**
   * Remove from the curve all children that have less than theNbSeg segments
   * @return
   *  True if the curve became empty, False otherwise.
   */
  Standard_EXPORT Standard_Boolean
    RemoveChildren (const Standard_Integer theNbSeg);

  /**
   * Remove from the curve all children that have clock-wise orientation
   * @return
   *  True if the curve became empty, False otherwise.
   */
  Standard_EXPORT Standard_Boolean RemoveHoles ();

  /**
   * Create and append (or prepend) a 2d segment.
   */
  Standard_EXPORT Poly_Segment2d& CreateSegment
    (const gp_XY& theOrigin,
     const gp_XY& theExtremity,
     const Standard_Boolean isAppend = Standard_True);

  /**
   * Copy the given Segment to the segments of the current curve.
   * @param theElem
   *   Segment to copy.
   * @param isReverse
   *   True if the copied segment should be inverted. 
   * @param isAppend
   *   True if the new segment is appended, False - it is prepended to the curve
   * @return
   *   Reference to the created segment.
   */
  Standard_EXPORT Poly_Segment2d& CreateSegment
    (const Poly_Segment2d&  theElem,
     const Standard_Boolean isReverse = Standard_False,
     const Standard_Boolean isAppend  = Standard_True);

  /**
   * Query the curve type.
   */
  inline CurveType GetType() const
  { return myType; }

  /**
   * Set the type of the curve.
   */
  inline void SetType(const CurveType theType)
  { myType = theType; }

  /**
   * Query the curve location.
   */
  inline const gp_Ax3& GetLoc() const
  { return myLoc; }


  /**
   * Set the curve location.
   */
  inline void SetLoc(const gp_Ax3& theLoc)
  { myLoc = theLoc; }

  /**
   * Query the allocator of segments, this allocator can be used for other
   * objects (e.g., by the children of this contour.
   */
  inline const Handle_NCollection_BaseAllocator& Allocator() const
  { return myAllocator; }

  /**
   * Create an array of 2d points representing the curve.
   * @param Vec
   *   array of points, for a closed contour the last point of the array is
   *   a copy of the first one.
   */
  Standard_EXPORT void GetContour2d (NCollection_Vector<gp_XY>& theVec) const;

  /**
   * Transform the Curve.
   */
  Standard_EXPORT void Transform (const gp_Trsf2d& theTrsf);

  /**
   * Classifies a 2D point in the Curve.
   * @param thePnt
   *   The point to be evaluated.
   * @param theTol
   *   Tolerance for ON classification.
   * @param theCnt
   *   <tt>[out]</tt>Optional: if defined, stores the counter of ray
   *   intersections with the curve. It should be preset to 0, this method
   *   adds the intersections to the previous value of theCnt.
   *   This counter can be used if there are multiple curves for this point
   *   classification. 
   * @return one of 3 values: ON, IN, OUT
   */
  Standard_EXPORT Class2d ClassifyPoint(const gp_XY&        thePnt,
                                        const Standard_Real theTol,
                                        Standard_Integer*   theCnt=0L) const;

  /**
   * Remove the given segment. This method does not check that the given
   * segment belongs to this Curve (unless theElem is the head of the list
   * denoted as "mySegments").
   * This method does nothing if mySegments is void (the curve is empty).
   */
  Standard_EXPORT void RemoveSegment(Poly_Segment2d* theElem);

  /**
   * Filter the curve segments through a tube with a radius equal to theTol.
   * Segments are merged in more longer segments, thus smoothing the curve, but
   * leaving sharp turns.
   * theRemoveDegen, if true, forces removal of child curves having less
   * then 3 segments.
   */
  Standard_EXPORT Standard_Boolean Filter
    (const Standard_Real theTol,
     const Standard_Boolean theRemoveDegen = Standard_False);

  /**
   * Remove thin cusps in the curve.
   * theRemoveDegen, if true, forces removal of child curves having less
   * then 3 segments.
   */
  Standard_EXPORT void FilterCusps
    (const Standard_Real theTol,
     const Standard_Boolean theRemoveDegen = Standard_False);

  /**
   * Method scans the curve and finds self-intersection on it. This is
   * not only the case of exact intersection but also the case when the minimal
   * distance between two segments is less than tolerance.
   * @param outPnt
   * <tt>[out]</tt> The first intersection point detected
   * @param tol
   *   Tolerance for the proximity detection.
   * @return
   *   True if a self-intersection was found, False - if it was not.
   */
  Standard_EXPORT Standard_Boolean CheckSelfIntersection
    (gp_XY& outPnt, const Standard_Real tol) const;

  /**
   * Test if two curves have at least one common point.
   * @param theCurve0
   *   The first curve
   * @param tehCurve1
   *   The second curve
   * @param theTol
   *   Tolerance for the proximity detection
   * @return
   *   True if there is detected a mutual intersection.
   */
  Standard_EXPORT static Standard_Boolean CheckMutualIntersection
    (const Handle_Poly_PolyCurve2d& theCurve0,
     const Handle_Poly_PolyCurve2d& theCurve1,
     const Standard_Real         theTol);

  /**
   * Projection of a point on the Curve.
   * Output is a location of projected point on curve.
   * @return
   *   Square distance from point to curve
   */
  Standard_EXPORT Standard_Real Project (const gp_XY& thePnt,
                                         Location&    theLoc) const;

  /**
   * Diagnostic dump of the Curve.
   */
  Standard_EXPORT void Dump (Standard_OStream& theStream,
                             Standard_Integer  theCnt = 0) const;

 protected:
  // ---------- PROTECTED METHODS ----------

  /**
   * Allocator-based operator new for dynamic allocations
   */
  DEFINE_NCOLLECTION_ALLOC

  /**
   * Constructor (protected)
   */
  Poly_PolyCurve2d(const Handle_NCollection_BaseAllocator& theAlloc,
                   const CurveType theType)
  : myAllocator (theAlloc),
    mySegments  (0L),
    myNSegments (0),
    myChildren  (theAlloc),
    myArea      (-1.),
    myPerimeter (0.),
    myIsCCW     (Standard_True),
    myIsModified(Standard_False),
    myType      (theType)
  {}

  /**
   * Called from the destructor. Implements deallocation of the object instance
   */
  Standard_EXPORT virtual void Delete () const; 

  void ComputeProperties ();

  /**
   * Add the given segment to the end.
   */
  void appendSegment(Poly_Segment2d * theElem);

  void mergeSegments(Poly_Segment2d* thepFirst,
                     Poly_Segment2d* thepLast);

 private:
  // ---------- PRIVATE FIELDS ----------

  /**
   * Allocator for Segments
   */
  Handle_NCollection_BaseAllocator myAllocator;

  /**
   * List of elementary segments forming a 2D contour. This container does not
   * own the segments, they are stored by myAllocator object and this list of
   * segments stores only pointers. 
   */
  Poly_Segment2d * mySegments;

  /**
   * Number of segments in the Curve.
   */
  Standard_Integer myNSegments;

  /**
   * List of child contours, may be empty. If this contour is closed, it is
   * supposed that all child contours are contained inside this contour.
   */
  Poly_ListOfCurve2d myChildren;

  /**
   * Non-negative value if the contour is closed, otherwise should be negative.
   */
  Standard_ShortReal myArea;

  /**
   * Length of the contour, both for open and closed ones.
   */
  Standard_ShortReal myPerimeter;

  /**
   * True means that this contour is counter-clockwise (trigo orientation).
   */
  Standard_Boolean  myIsCCW : 1;

  /**
   * Internal flag that is set to True when the contour is modified. This
   * leads to recalculation of Area and perimeter at the first request.
   */
  Standard_Boolean myIsModified : 1;

  /**
   * Identifier of the curve type, to select the right curve processing
   * for child curves.
   */
  CurveType myType : 4;

  /**
   * Identifier of the base plane for this curve and its children.
   */
  gp_Ax3 myLoc;

 public:

  // Declaration of CASCADE RTTI
  DEFINE_STANDARD_RTTI (Poly_PolyCurve2d)

  friend class Poly_Segment2d;
};

#endif
