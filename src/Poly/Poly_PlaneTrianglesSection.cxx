// File:        Poly_PlaneTrianglesSection.cxx
// Created:     02.07.2010
// Author:      Sergey KUUL
// Copyright:   Open Cascade 2010

#include <Poly_PlaneTrianglesSection.hxx>
#include <Poly_Curve2dBuilder.hxx>
#include <Poly_Triangulation.hxx>
#include <TopLoc_Location.hxx>

//=======================================================================
//function : Constructor
//purpose  : 
//=======================================================================

Poly_PlaneTrianglesSection::Poly_PlaneTrianglesSection
                    (const gp_Ax3&              thePlane,
                     const Standard_Real        theTol,
                     const Standard_Real        theGTol,
                     const Handle_NCollection_BaseAllocator& theAlloc)
  : myLocation  (thePlane.Location(),
                 thePlane.Direction(), thePlane.XDirection()),
    myWidth (0.),
    myIsStripe (Standard_False),
    myBuilder   (* new Poly_Curve2dBuilder(theTol, theGTol, theAlloc))
{
  myBuilder.SetLoc(thePlane);
}

//=======================================================================
//function : Constructor
//purpose  : 
//=======================================================================

Poly_PlaneTrianglesSection::Poly_PlaneTrianglesSection
                    (const gp_Ax3&              thePlane,
                     const Standard_Real        theWidth,
                     const Standard_Real        theTol,
                     const Standard_Real        theGTol,
                     const Handle_NCollection_BaseAllocator& theAlloc)
  : myLocation  (thePlane.Location(),
                 thePlane.Direction(), thePlane.XDirection()),
    myWidth (theWidth),
    myIsStripe (Standard_True),
    myBuilder   (* new Poly_Curve2dBuilder(theTol, theGTol, theAlloc))
{
  myBuilder.SetLoc(thePlane);
}

//=======================================================================
//function : ~Poly_PlaneTrianglesSection
//purpose  : 
//=======================================================================

Poly_PlaneTrianglesSection::~Poly_PlaneTrianglesSection()
{
  delete &myBuilder;
}

//=======================================================================
//function : Perform
//purpose  : Build result.
//=======================================================================

Standard_Integer Poly_PlaneTrianglesSection::Perform()
{
  return myBuilder.Perform();
}

//=======================================================================
//function : SetTreatOpenSections
//purpose  : Defines whether the open sections should be treated
//=======================================================================
void Poly_PlaneTrianglesSection::SetTreatOpenSections
                        (Standard_Boolean theIsTreatOpenSections)
{
  myBuilder.SetTreatOpenCurves(theIsTreatOpenSections);
}

//=======================================================================
//function : SetShellMode
//purpose  : Defines whether it is necessary to take into account 
//           the links orientations while building chains
//=======================================================================
void Poly_PlaneTrianglesSection::SetShellMode
                        (Standard_Boolean theIsShellMode)
{
  myBuilder.SetShellMode(theIsShellMode);
}

//=======================================================================
//function : GetResult
//purpose  : Returns a result
//=======================================================================

Handle_Poly_PolyCurve2d Poly_PlaneTrianglesSection::GetResult() const
{
  return myBuilder.GetResult();
}

//=======================================================================
//function : AddTriangulation
//purpose  : Add a triangulation for intersection
//=======================================================================

void Poly_PlaneTrianglesSection::AddTriangulation
            (const Handle(Poly_Triangulation)& theTriangulation,
             const TopLoc_Location&            theLocation,
             const Standard_Boolean            theNeedToReverse)
{
  // Intersect triangles and put result segments in the curve builder
  if (myIsStripe == Standard_False) {
    addTriangulationForPlane(theTriangulation, theLocation, theNeedToReverse);
  } else {
    addTriangulationForStripe(theTriangulation, theLocation, theNeedToReverse);   
  }
}

//=======================================================================
//function : addTriangulationForPlane
//purpose  : Add a triangulation for intersection by plane
//=======================================================================
void Poly_PlaneTrianglesSection::addTriangulationForPlane
                                 (const Handle(Poly_Triangulation)& theTriangulation,
                                  const TopLoc_Location& theLocation,
                                  const Standard_Boolean theNeedToReverse)
{
  const Poly_Array1OfTriangle& aTrias = theTriangulation->Triangles();
  Standard_Integer aNbTriangles = aTrias.Length();
  const TColgp_Array1OfPnt& aNodes = theTriangulation->Nodes();
  Standard_Integer aNbNodes = aNodes.Length();

  if ( aNbNodes == 0 || aNbTriangles == 0 )
    return; // Null triangulation

  Poly_Segment2d aSegment;
  Standard_Integer i = aTrias.Lower();
  
  gp_Trsf aTrsf;
  aTrsf.SetTransformation(myLocation);
  aTrsf.Multiply(theLocation.Transformation());

  for (; i <= aTrias.Upper(); i++) {
    const Poly_Triangle& aTr = aTrias.Value(i);
    Standard_Integer N1,N2,N3;
    aTr.Get(N1,N2,N3);
    gp_XYZ aPoints[3] = {
      aNodes.Value(N1).XYZ(),
      aNodes.Value(N2).XYZ(),
      aNodes.Value(N3).XYZ(),
    };

    Standard_Integer j = 0;
    for (; j < 3; j++) {
      aTrsf.Transforms(aPoints[j]);
    }

    if ( theNeedToReverse ) {
      gp_XYZ aTmpPnt(aPoints[1]);
      aPoints[1] = aPoints[2];
      aPoints[2] = aTmpPnt;
    };
  
    IntTriPlaneCode anErr = IntersectTriPlane (aPoints, aSegment);
    if (anErr==ITP_Done) {
      myBuilder.AddSegment(aSegment);
    }    
  }
}

//=======================================================================
//function : addTriangulationForStripe
//purpose  : Add a triangulation for intersection by stripe
//=======================================================================
void Poly_PlaneTrianglesSection::addTriangulationForStripe
                                 (const Handle(Poly_Triangulation)& theTriangulation,
                                  const TopLoc_Location& theLocation,
                                  const Standard_Boolean theNeedToReverse)
{
  const Poly_Array1OfTriangle& aTrias = theTriangulation->Triangles();
  Standard_Integer aNbTriangles = aTrias.Length();
  const TColgp_Array1OfPnt& aNodes = theTriangulation->Nodes();
  Standard_Integer aNbNodes = aNodes.Length();

  if ( aNbNodes == 0 || aNbTriangles == 0 )
    return; // Null triangulation

  gp_Trsf aTrsf;
  aTrsf.SetTransformation(myLocation);
  aTrsf.Multiply(theLocation.Transformation());
  
  Poly_Segment2d aSegment;
  Standard_Integer i = aTrias.Lower();
  for (; i <= aTrias.Upper(); i++) {
    const Poly_Triangle& aTr = aTrias.Value(i);
    Standard_Integer N1,N2,N3;
    aTr.Get(N1,N2,N3);
    gp_XYZ aPoints[3] = {
      aNodes.Value(N1).XYZ(),
      aNodes.Value(N2).XYZ(),
      aNodes.Value(N3).XYZ(),
    };

    Standard_Integer j = 0;
    for (; j < 3; j++) {
      aTrsf.Transforms(aPoints[j]);
    }
    
    // Check if the triangle is not outside the stripe
    Standard_Real X1 = aPoints[0].X();
    Standard_Real X2 = aPoints[1].X();
    Standard_Real X3 = aPoints[2].X();

    if ( ( 0 < X1 || 0 < X2 || 0 < X3) && 
         ( X1 < myWidth || X2 < myWidth || X3 < myWidth ) ) 
    {
      if ( theNeedToReverse ) {
        gp_XYZ aTmpPnt(aPoints[1]);
        aPoints[1] = aPoints[2];
        aPoints[2] = aTmpPnt;
      }
    
      IntTriPlaneCode anErr = IntersectTriPlane (aPoints, aSegment);
      if (anErr==ITP_Done) {
        
        Standard_Real aX0 = aSegment.Point(0).X();
        Standard_Real aX1 = aSegment.Point(1).X();

        if ((aX0 < 0.0 && aX1 < 0.0) || (aX0 > myWidth && aX1 > myWidth)) {
          continue; // The segment is outside the stripe
        }

        gp_XY aStartSegBound(aX0, aSegment.Point(0).Y());
        gp_XY anEndSegBound(aX1, aSegment.Point(1).Y());
        if (aX0 < 0.0) {
          Standard_Real aParam = aX0 / (aX0 - aX1);
          aSegment.Interpolate(aParam, aStartSegBound);
        } else if (aX1 < 0.0) {
          Standard_Real aParam = aX0 / (aX0 - aX1);
          aSegment.Interpolate(aParam, anEndSegBound);
        } 
        if (aX0 > myWidth) {
          Standard_Real aParam = (myWidth - aX0) / (aX1 - aX0);
          aSegment.Interpolate(aParam, aStartSegBound);
        } else if (aX1 > myWidth) {
          Standard_Real aParam = (myWidth - aX0) / (aX1 - aX0);
          aSegment.Interpolate(aParam, anEndSegBound);
        }
        aSegment.SetPoint(0, aStartSegBound);
        aSegment.SetPoint(1, anEndSegBound);
        myBuilder.AddSegment(aSegment);   
      }
    }
  }
}

//=======================================================================
//function : IntersectTriPlane
//purpose  : 
//=======================================================================

//Standard_Boolean Poly_PlaneTrianglesSection::IntersectTriPlane
IntTriPlaneCode Poly_PlaneTrianglesSection::IntersectTriPlane
                   (const gp_XYZ theTriNodes[3],
                    Poly_Segment2d& theSegment)
{
  Standard_Real aConfTol = Precision::PConfusion();

  // Define states of triangle nodes with respect to the plane.
  Standard_Real aParam[3];
  Standard_Integer aTriStates[3];
  Standard_Integer aSum = 0;
  Standard_Integer j = 0;
  for (; j < 3; j++) {
    aParam[j] = theTriNodes[j].Z();
    if (aParam[j] < -aConfTol)
      aTriStates[j] = -1; // Negative side.
    else if (aParam[j] > aConfTol)
      aTriStates[j] = 1;  // Positive side.
    else
      aTriStates[j] = 0;  // Node is on the plane.
    aSum += aTriStates[j];
  }
  if (Abs(aSum) >= 2) {
    return ITP_NoSection; // The triangle doesn't intersect the plane.
  }

  // Compute boundary points of intersection segment.
  gp_XYZ  aNodes[3];
  Standard_Integer aCurIndex = 0;
  for (j = 0; j < 3 && aCurIndex < 3; j++) {
    if (aTriStates[j] == 0) {
      // This node is on the plane. Add it.
      aNodes[aCurIndex++] = theTriNodes[j];
    }
    else {
      // This node is not on the plane.
      // Try the edge from this node to the next one.
      Standard_Integer jp1 = (j == 2) ? 0 : j + 1;
      if (aTriStates[j]*aTriStates[jp1] < 0) {
        // There is an intersection, as one point is above
        // and the other one is below the plane.
        // Compute coordinates by linear interpolation.
        Standard_Real aCoeff = -aParam[j] / (aParam[jp1] - aParam[j]);
        aNodes[aCurIndex++].SetLinearForm (1-aCoeff, theTriNodes[j],
                                           aCoeff, theTriNodes[jp1]);
      }
    }
  }

  if (aCurIndex == 3)
    return ITP_TriInPlane; // Triangle lies in the plane
  if (aCurIndex != 2)
    return ITP_NoSection;

  // There is an intersection segment.
  // Compute the normal to the triangle
  gp_XYZ aV[2] = { theTriNodes[1] - theTriNodes[0],
                   theTriNodes[2] - theTriNodes[0] };
  gp_XYZ aNorm(aV[0].Crossed(aV[1]));

  Standard_Real aNormLen = aNorm.Modulus();
  if (aNormLen <= aConfTol)
    // This is a degenerated triangle
    return ITP_DegTri;
  aNorm.Divide(aNormLen);

  gp_XY  aNorm2d;
  gp_XY  aNodes2d[2];

  // Get projection of a normal on plane
  // and 2d points of the intersection segment.

  aNorm2d.SetCoord(aNorm.X(), aNorm.Y());
  for (j = 0; j < 2; j++) {
    aNodes2d[j].SetCoord(aNodes[j].X(), aNodes[j].Y());
  }

  // Construct correctly oriented 2d intersection segment.
  gp_XYZ        aSeg(aNodes[1] - aNodes[0]);
  gp_XY         aSeg2d(aNodes2d[1] - aNodes2d[0]);
  Standard_Real aCrossProd = aNorm2d.Crossed(aSeg2d);

  if (aCrossProd < 0.) {
    // Reverse the segment.
    gp_XY aTmpNode(aNodes2d[0]);
    aNodes2d[0] = aNodes2d[1];
    aNodes2d[1] = aTmpNode;
    aSeg.Reverse();
    aSeg2d.Reverse();
  }

  // Construct the segment
  theSegment.SetPoint(0, aNodes2d[0]);
  theSegment.SetPoint(1, aNodes2d[1]);
  theSegment.SetNormal (aNorm);

  return ITP_Done;
}

