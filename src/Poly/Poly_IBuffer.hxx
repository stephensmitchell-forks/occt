// File:      Poly_IBuffer.hxx
// Created:   15.06.07 08:08
// Author:    Alexander GRIGORIEV
// Copyright: Open CASCADE 2007


#ifndef Poly_IBuffer_HeaderFile
#define Poly_IBuffer_HeaderFile

#include <NCollection_BaseCollection.hxx>
#include <NCollection_List.hxx>
#include <gp_XY.hxx>
#include <Standard_DefineHandle.hxx>

DEFINE_STANDARD_HANDLE (Poly_IBuffer, Standard_Transient)

typedef NCollection_List<Handle_Poly_IBuffer> Poly_ListOfIBuffer;

//! Interval buffer to build outline curve on a set of polygons.
class Poly_IBuffer: public Standard_Transient
{
 public:
  class Interval
  {
  public:
    // ---------- PUBLIC METHODS ----------
    Interval (const gp_XY&              theXZfirst,
              const gp_XY&              theXZlast,
              const unsigned long       theModID = 0UL);

    /**
     * Allocator-based operator new for dynamic allocations
     */
    DEFINE_NCOLLECTION_ALLOC

    /**
     * Create a copy of the given Interval in the given allocator.
     * If theDest==NULL then a new instance is created.
     */
    Standard_EXPORT virtual Interval&
                         Clone        (const Handle_NCollection_BaseAllocator&,
                                       Interval * theDest = NULL) const;

    /**
     * Query the abscisse of the interval.
     */
    const gp_XY&         XZ           (const Standard_Integer ind) const;

    /**
     * Query the interval for modification.
     */ 
    gp_XY&               XZ           (const Standard_Integer ind);

    /**
     * Shorten the interval to the new value of Xmax.
     */
    Standard_EXPORT void Trim         (const Standard_Real theX,
                                       const Standard_Integer   theMode);
    /**
     * Query the previous interval in the buffer.
     */
    Interval *           Previous     () const;

    /**
     * Query the next interval in the buffer.
     */
    Interval *           Next         () const;

    /**
     * Query the RejectID indicating the modification status of the interval
     */
    unsigned long        GetModificationID   () const;

    /**
     * Query the abscisse value required for modification.
     */
    Standard_Real        GetModificationAbscisse() const;

    /**
     * Set the modification ID.
     */
    void                 SetModificationID   (const unsigned long theValue);

    /**
     * Check for intersection, returns the intersection point abscissa.
     * @return
     *   True if a modification or removal is required. The decision is based
     *   on 2 lower bits of myRejectID value:
     *    - 0 : remove this interval;
     *    - 1 : keep the lower abscisse part, remove the upper part;
     *    - 2 : keep the upper abscisse part, remove the lower part;
     *    - 3 : remove the middle.
     * @param theInt
     *   The other interval that may intersect this one.
     * @param theTol
     *   Tolerance of intersection: smaller intersections are neglected.
     * @param theRejectID
     *   Base ID that is stored in the interval with the corresponding
     *   modification status.
     */
    Standard_EXPORT Standard_Boolean
                         IsIntersect  (const Interval&     theInt,
                                       const Standard_Real theTol,
                                       const unsigned long theRejectID);
    /**
     * Generator of unique Reject ID values.
     */
    static Standard_EXPORT unsigned long
                         NewRejectID  ();

  private:
    // ---------- PRIVATE FIELDS ----------
    Interval            * myNext;
    gp_XY               myXZ[2];
    union {
      Standard_Real     myModX;
      Interval          * myPrev;
    }                   U;
    unsigned long       myModID;
    friend class Poly_IBuffer;
  };
  
 public:
  // ---------- PUBLIC METHODS ----------

  /**
   * Default constructor.
   */
  Poly_IBuffer ();

  /**
   * Constructor.
   */
  Standard_EXPORT Poly_IBuffer
                        (const Interval&                         theInterval,
                         const Standard_Real                     theTol,
                         const Handle_NCollection_BaseAllocator& theAlloc = 0L);

  /**
   * Get the next IBuffer in the list.
   */
  //inline const Poly_IBuffer& YNext() const
  //{ return * myNext; }
  inline const Poly_IBuffer *& YNext() 
  { return  myNext; }

  /**
   * Get the previous IBuffer in the list.
   */
  //inline const Poly_IBuffer& YPrevious() const
  //{ return * myPrevious; }
  inline const Poly_IBuffer *& YPrevious() 
  { return  myPrevious; }

  inline const Standard_Real MyY() const
  { return myY; }

  /**
   * Set myY value.
   */
  inline void SetY (const Standard_Real theY)
  { myY = theY; }

#ifdef _DEBUG
  inline int MyCount()
  { return myCount; }
#endif

  /**
   * Initialization, to be used after the default constructor.
   */
  Standard_EXPORT void Init
                        (const Interval&                         theInterval,
                         const Standard_Real                     theTol,
                         const Handle_NCollection_BaseAllocator& theAlloc = 0L);

  Standard_EXPORT ~Poly_IBuffer ();

  Standard_EXPORT Interval *
                       AddInterval        (const Interval&      theInterval,
                                           Interval *           theStart = 0L);

  /**
   * Called when all intervals are added, just before extraction of data.
   * This method sets the values 'myPrev' in all Intervals. Can be omittred
   * if the method Previous() is not to be called afterwards.
   * @param isFiltering
   *   If True then the method would merge the intervals that form a straight
   *   line. It can lead to loss of data attached to intervals, use this only
   *   when you are only interested in geometry of intervals.
   */
  Standard_EXPORT void CloseBuffer        (const Standard_Boolean isFiltering
                                           = Standard_False);

  const Interval *     First              () const;

  void                 GetOrdinateExtents (Standard_Real& theMin,
                                           Standard_Real& theMax) const;
  /**
   * Debug facility.
   */
  Standard_EXPORT void Dump          (Standard_OStream& theStream);

  /**
   * Debug facility.
   */
  Standard_EXPORT void DebugPolyline (Standard_OStream&   theStream,
                                      const Standard_Real theY = 1.);

 protected:
  // ---------- PROTECTED METHODS ----------

  typedef struct {
    Standard_Real  X;
    const Interval * Int;
  } PInterval;

  Interval *      addInt        (const Interval&          theInterval,
                                 const Standard_Real      theX[2],
                                 const Interval         * thePrevious);
  void            clearIntervals();

 private:
  Poly_IBuffer                (const Poly_IBuffer& theOther);
  Poly_IBuffer& operator =    (const Poly_IBuffer& theOther);

 private:
  // ---------- PRIVATE FIELDS ----------

  Handle_NCollection_BaseAllocator  myAllocator;
  Standard_Real                     myTol;
  Standard_Real                     myMinOrd;
  Standard_Real                     myMaxOrd;
  Standard_Size                     myNIntervals;
  Standard_Size                     myNHoles;
  PInterval                         * myIndex;
  //Next IBuffer in the list
  const Poly_IBuffer*               myNext;
  //Previous IBuffer in the list
  const Poly_IBuffer*               myPrevious;
  Standard_Real                     myY;
#ifdef _DEBUG
  int                               myCount;
  static int                        GlobalCount;
#endif

public:
  // Declaration of CASCADE RTTI
  DEFINE_STANDARD_RTTI (Poly_IBuffer)
};

#include <Poly_IBuffer.lxx>

#endif
