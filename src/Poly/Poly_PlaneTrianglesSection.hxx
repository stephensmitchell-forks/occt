// File:        Poly_PlaneTrianglesSection.hxx
// Created:     02.07.2010
// Author:      Sergey KUUL
// Copyright:   Open Cascade 2010

#ifndef Poly_PlaneTrianglesSection_HeaderFile
#define Poly_PlaneTrianglesSection_HeaderFile

#include <gp_Ax3.hxx>
#include <Handle_Poly_PolyCurve2d.hxx>
#include <NCollection_BaseAllocator.hxx>

class Handle_Poly_Triangulation;
class TopLoc_Location;
class Poly_Curve2dBuilder;
class Poly_Segment2d;

enum IntTriPlaneCode
  {
    ITP_NoSection,
    ITP_Done,      // there is intersection segment
    ITP_DegTri,    // triangle is degenerated
    ITP_TriInPlane // triangle lies in the plane
  };

  
/**
 * This class represents a tool for intersection plane with
 * Poly_Triangulation`s.
 * Result is represented as PolyCurve2d
 */

class Poly_PlaneTrianglesSection
{
public:
  
  /**
   * Constructor
   * \param thePlane - section plane.
   * \param theTol   - is used to connect ends of segments.
   * \param theGTol  - is used to sew remaining free nodes,
   *                   it should be greater than theTol.
   * \param theAlloc - Memory allocator for the algorithm
   */
  Standard_EXPORT Poly_PlaneTrianglesSection
                    (const gp_Ax3&              thePlane,
                     const Standard_Real        theTol,
                     const Standard_Real        theGTol,
                     const Handle_NCollection_BaseAllocator& theAlloc = NULL);

  /**
   * Constructor
   * \param thePlane - position of the section stripe.
   * \param theWidth - width of the section stripe from the 
   * \                 stripe location point along X direction.
   * \param theTol   - is used to connect ends of segments.
   * \param theGTol  - is used to sew remaining free nodes,
   *                   it should be greater than theTol.
   * \param theAlloc - Memory allocator for the algorithm
   */
  Standard_EXPORT Poly_PlaneTrianglesSection
                    (const gp_Ax3&              thePlane,
                     const Standard_Real        theWidth,
                     const Standard_Real        theTol,
                     const Standard_Real        theGTol,
                     const Handle_NCollection_BaseAllocator& theAlloc = NULL);

  /**
   * Destructor
   */
  Standard_EXPORT ~Poly_PlaneTrianglesSection();


  /**
   * Add a triangulation for intersection
   * \param theTriangulation - the triangulation.
   * \param theLocation - Location of the triangulation
   * \param theNeedToReverse - True if the triangulation is from reversed face
   */
  Standard_EXPORT void AddTriangulation
            (const Handle_Poly_Triangulation& theTriangulation,
             const TopLoc_Location&           theLocation,
             const Standard_Boolean           theNeedToReverse);
  
  /**
   * Defines whether it is necessary to treat open sections
   * \param theIsTreatOpenSections - True if it is necessary to treat open sections.
   */ 
 Standard_EXPORT void SetTreatOpenSections
                            (Standard_Boolean theIsTreatOpenSections);
  
  /**
   * Defines whether it is necessary to take into account 
   * the links orientations while building chains
   * \param theIsShellMode - True if it is necessary to treat 
   *                         the links orientations while building chains.
   */
  Standard_EXPORT void SetShellMode (Standard_Boolean theIsShellMode);

  /**
   * Build result
   */
  Standard_EXPORT Standard_Integer Perform();

  /**
   * Add others corves for creation common solution
   */
  //Standard_EXPORT void AddOtherCurves(const Poly_ListOfCurve2d& theListCurve);

  /**
   * Returns a result
   */
  Standard_EXPORT Handle_Poly_PolyCurve2d GetResult() const;

  /**
   * Intersect triangle with section plane.
   * Output is theSegment if the return value is 1.
   * Return error code.
   */
  Standard_EXPORT static IntTriPlaneCode IntersectTriPlane
                                        (const gp_XYZ    theTriNodes[3],
                                         Poly_Segment2d& theSegment);


protected:
  /**
   * Add a triangulation for intersection by plane
   * \param theTriangulation - the triangulation.
   * \param theLocation - Location of the triangulation
   * \param theNeedToReverse - True if the triangulation is from reversed face
  */
  void addTriangulationForPlane(const Handle(Poly_Triangulation)& theTriangulation,
                                const TopLoc_Location& theLocation,
                                const Standard_Boolean theNeedToReverse);

  /**
   * Add a triangulation for intersection by stripe
   * \param theTriangulation - the triangulation.
   * \param theLocation - Location of the triangulation
   * \param theNeedToReverse - True if the triangulation is from reversed face
  */
  void addTriangulationForStripe(const Handle(Poly_Triangulation)& theTriangulation,
                                 const TopLoc_Location& theLocation,
                                 const Standard_Boolean theNeedToReverse);

private:

  Poly_PlaneTrianglesSection& operator=(const Poly_PlaneTrianglesSection& theOther);

private:
  // === PRIVATE FIELDS ===
  gp_Ax3                myLocation;
  Standard_Real         myWidth;
  Standard_Boolean      myIsStripe;
  Poly_Curve2dBuilder&  myBuilder;  // reference to break header dependency
};

#endif
