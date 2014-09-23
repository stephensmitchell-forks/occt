// File:      Poly_Outline.hxx
// Created:   25.07.10 10:12
// Author:    Alexander GRIGORIEV
// Copyright: Open Cascade 2010


#ifndef Poly_Outline_HeaderFile
#define Poly_Outline_HeaderFile

#include <Bnd_B3d.hxx>
#include <Handle_Poly_PolyCurve2d.hxx>
#include <Handle_Poly_Triangulation.hxx>
#include <Poly_IBuffer.hxx>
#include <TopLoc_Location.hxx>
#include <gp_Ax3.hxx>

struct Poly_CntExtractor;

/**
 * Create Outline contours of a triangulation from the given view direction.
 */

class Poly_Outline 
{
 public:
  // ---------- PUBLIC METHODS ----------
  struct LocatedTriangulation {
    Handle_Poly_Triangulation   Tri;
    TopLoc_Location             Loc;
  };

  /**
   * Empty constructor.
   */
  Standard_EXPORT Poly_Outline (const Standard_Real                     theTol,
                                const Handle_NCollection_BaseAllocator& theAlloc
                                                        = NULL);

  /**
   * Destructor.
   */
  Standard_EXPORT virtual ~Poly_Outline ();

  /**
   * Add a triangulation to the working set. Should be called at least once.
   */
  Standard_EXPORT void
        AddTriangulation        (const Handle_Poly_Triangulation& theTri,
                                 const TopLoc_Location&           theLoc);

  /**
   * Run the algorithm.
   */
  Standard_EXPORT Standard_Boolean
        Perform                 (const gp_Ax3& thePln);

  /**
   * Get the result.
   */
  Standard_EXPORT const NCollection_List<Handle_Poly_PolyCurve2d>&
        Result                  ();

 protected:
  // ---------- PROTECTED METHODS ----------

  /**
   * Convert from the absolute to local Plane coordinate system.
   */
  inline static gp_XY PointTo2d (const gp_XYZ& thePnt, const gp_Ax3& thePln)
  {
    const gp_XYZ aPntLoc(thePnt - thePln.Location().XYZ());
    return gp_XY(aPntLoc * thePln.XDirection().XYZ(),
                 aPntLoc * thePln.YDirection().XYZ());
  }


 private:
  Poly_Outline (const Poly_Outline& theOther);
  Poly_Outline& operator= (const Poly_Outline& theOther);

  void fillIntervals (const gp_Ax3& thePln);

  void goNext (struct Poly_CntExtractor& theExtr);

 private:
  // ---------- PRIVATE FIELDS ----------
  Handle_NCollection_BaseAllocator          myAlloc;
  Standard_Real                             myTol;
  NCollection_List<LocatedTriangulation>    myListTriangulation;
  Poly_IBuffer *                            myBuffers;
  Standard_Real                             myY0;
  Standard_Real                             myYStep;
  Standard_Integer                          myNYSteps;
  Standard_Size                             myNBuffers;
  Bnd_B3d                                   myBox;
  NCollection_List<Handle_Poly_PolyCurve2d> myOutline;
};


#endif
