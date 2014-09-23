// File:        Poly_Seg2dIterator.hxx
// Created:     01.07.2010
// Author:      Sergey KUUL
// Copyright:   Open Cascade 2010

#ifndef Poly_Seg2dIterator_HeaderFile
#define Poly_Seg2dIterator_HeaderFile

#include <Poly_Segment2d.hxx>

/**
 * Iterator that is used to explore a list of 2d elements.
 * The exploration begins from the element that is used to initialize
 * the iterator. It stops when a NULL value is found or when the initial
 * element is retrieved again. Therefore this Iterator is suitable for
 * both linear and round lists of elements.
 */

class Poly_Seg2dIterator 
{
 public:
  /**
   * Enumerated sense of iteration.
   */
  enum Sense {
    Forward = 0,
    Reverse = 1,
    Unknown = 2
  };

 public:
  // ---------- PUBLIC METHODS ----------

  /**
   * Empty constructor.
   */
  inline Poly_Seg2dIterator ()
    : myFirst (0L), myCurrent (0L), mySense (Unknown) {}

  /**
   * Constructor.
   */
  inline Poly_Seg2dIterator(const Poly_Segment2d& theSegment,
                            const Sense theSense = Forward)
    : myFirst   (&theSegment),
      myCurrent (&theSegment),
      mySense   (theSense)
  {}

  /**
   * Restart the iteration from the initial position.
   */
  inline void Reset ()
  {
    myCurrent = myFirst;
  }

  /**
   * Restart the iteration using the given element as the first one.
   */
  inline void Init(const Poly_Segment2d& theSegment,
                   const Sense           theSense)
  {
    myFirst   = &theSegment;
    myCurrent = &theSegment;
    mySense   = theSense;
  }

  /**
   * Restart the iteration using the given element as the first one.
   */
  inline void Init(const Poly_Segment2d& theSegment)
  {
    myFirst   = &theSegment;
    myCurrent = &theSegment;
    if (mySense == Unknown) {
      mySense = Forward;
    }
  }

  /**
   * Query if a valid Poly_Segment is available on the current iteration.
   */
  inline Standard_Boolean More() const
  { return myCurrent != 0L; }

  /**
   * Increment the iterator (move to the next iterated object).
   */
  inline void  Next()
  {
    if (myCurrent) {
      if (mySense == Forward) {
        myCurrent = myCurrent->myNext;
      }
      else {
        myCurrent = myCurrent->myPrevious;
      }
      if (myCurrent == myFirst)
        myCurrent = 0L;
    }
  }

  /**
   * Query the current iterated object.
   */
  inline const Poly_Segment2d& Value () const
  {
    return * myCurrent;
  }

  /**
   * Query the current iterated object.
   */
  inline Poly_Segment2d& ChangeValue () const
  {
    return const_cast <Poly_Segment2d&> (* myCurrent);
  }

  /**
   * Query the sense of iterations (forward/reverse).
   */
  inline Sense GetSense() const
  { return mySense; }

  /**
   * Comparison operator.
   */
  inline Standard_Boolean operator== (const Poly_Seg2dIterator& theOther) const
  {
    return (myCurrent == theOther.myCurrent && mySense == theOther.mySense);
  }

  /**
   * Comparison operator.
   */
  inline Standard_Boolean operator!= (const Poly_Seg2dIterator& theOther) const
  {
    return (myCurrent != theOther.myCurrent || mySense != theOther.mySense);
  }

  /**
   * Returns the sense that ius opposite to the given one.
   */
  inline static Sense Opposite (const Sense theSense)
  {
    if (theSense == Forward)
      return Reverse;
    else if (theSense == Reverse)
      return Forward;
    return Unknown;
  }

 private:
  // ---------- PRIVATE FIELDS ----------
  const Poly_Segment2d* myFirst;
  const Poly_Segment2d* myCurrent;
  Sense                 mySense;
};

#endif
