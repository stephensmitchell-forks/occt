// File:        Poly_Curve2dBuilder.hxx
// Created:     01.07.2010
// Author:      Mikhail SAZONOV
// Copyright:   Open Cascade 2010

#ifndef Poly_Curve2dBuilder_HeaderFile
#define Poly_Curve2dBuilder_HeaderFile

#include <Poly_PolyCurve2d.hxx>
#include <NCollection_IncAllocator.hxx>
#include <Poly_Segment2dSewer.hxx>
#include <Poly_MakeLoops.hxx>

class Poly_Segment2d;

/**
 * This class takes on input a set of 2D segments, connects them
 * and builds a set of closed curves
 */
class Poly_Curve2dBuilder
{
public:
  //! Constructor. All result curves will be placed in the given allocator,
  //! or in a new one if NULL is given.
  //! theTol is used to connect ends of segments.
  //! theGTol is used to sew remaining free nodes, it should be greater
  //! than theTol.
  //! If theGTol is not greater than theTol additional sewing is not performed.
  Standard_EXPORT Poly_Curve2dBuilder
                   (const Standard_Real theTol,
                    const Standard_Real theGTol,
                    const Handle_NCollection_BaseAllocator& theAlloc = NULL);

  //! Adds a next segment.
  //! Orientation of the corresponding segment in the result will be the same.
  Standard_EXPORT void AddSegment(const Poly_Segment2d& theSeg,
                                  const Standard_Boolean theAllowDelay
                                                        = Standard_True);

  //! Adds a next open curve.
  //! Orientation of the corresponding segments in the result will be the same.
  Standard_EXPORT void AddCurve(const Handle_Poly_PolyCurve2d& theCurve,
                                const Standard_Boolean theAllowDelay
                                                        = Standard_True);

  //! Defines whether it is necessary to treat open curves.
  Standard_EXPORT void SetTreatOpenCurves(Standard_Boolean theIsTreatOpenCurves);

  //! Defines whether it is necessary to take into account the links orientations while building chains
  Standard_EXPORT void SetShellMode(Standard_Boolean theIsShellMode);

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

  enum ErrorCode
  {
    CB_HangingSegments = 1,  // not all segments have been added to the result
    CB_Empty           = 2   // no one curve in the result
  };

  //! Does the work. Returns the collection of error codes in bits
  Standard_EXPORT Standard_Integer Perform();

  //! Returns the result curve. It is a composite curve containing children
  Standard_EXPORT const Handle_Poly_PolyCurve2d& GetResult() const;

protected:
  Standard_EXPORT void  sewFreeNodes        ();
  Standard_EXPORT void  removeInsideLinks   ();
  Standard_EXPORT Standard_Integer buildLoop( const Poly_MakeLoops::ListOfLink&,
                                              Handle_Poly_PolyCurve2d& );
  Standard_EXPORT void buildChains( const Poly_MakeLoops::ListOfLink&, 
                                    NCollection_List<Handle_Poly_PolyCurve2d>& );
  Standard_EXPORT Standard_Integer
                        countNotHangingLinks( const Poly_MakeLoops::ListOfLink& );

private:
  // FIELDS
  Standard_Real                        myTol;
  Standard_Real                        myGTol;
  Standard_Boolean                     myIsTreatOpenCurves;
  Standard_Boolean                     myIsShellMode;
  Handle_NCollection_BaseAllocator     myAlloc;
  Handle_NCollection_IncAllocator      myTempAlloc;
  Handle_Poly_PolyCurve2d              myResult;
  Poly_Segment2dSewer                  myHelper;
  Poly_MakeLoops2D                     myLoopMaker;
  NCollection_List<Poly_Segment2d>     myDelayedSegList;
  Poly_ListOfCurve2d                   myDelayedCrvList;
  gp_Ax3                               myLoc;
};

#endif
