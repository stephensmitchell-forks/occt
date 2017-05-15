// File:	Unfolding_Shell.cxx
// Created:	Tue Sep  9 17:14:33 2008
// Author:	Mikhail KLOKOV
//		<mkk@kurox>


#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <Geom_Plane.hxx>
#include <gp_Pln.hxx>
#include <math_FunctionRoot.hxx>
#include <math_NewtonFunctionRoot.hxx>
#include <ShapeAnalysis_FreeBoundData.hxx>
#include <ShapeAnalysis_FreeBoundsProperties.hxx>
#include <TColgp_Array1OfXY.hxx>
#include <TColgp_HArray1OfXY.hxx>
#include <TColgp_SequenceOfXY.hxx>
#include <TColStd_ListOfInteger.hxx>
#include <TColStd_MapOfInteger.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>
#include <TopTools_DataMapIteratorOfDataMapOfShapeListOfShape.hxx>
#include <TopTools_DataMapOfShapeInteger.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_MapOfShape.hxx>
#include <Unfolding.hxx>
#include <Unfolding_FaceDataContainer.hxx>
#include <Unfolding_FunctionWithDerivative.hxx>
#include <Unfolding_Shell.hxx>
#include <Unfolding_Surface.hxx>

static Standard_Boolean CombineCurves(const TColgp_Array1OfXY& theMasterPolyLine,
				      const TColgp_Array1OfXY& theSlavePolyLine,
				      const Standard_Boolean   IsFixed,
				      const Standard_Real      theConfTolerance, 
				      const Standard_Real      theAreaTolerance, 
				      gp_Trsf2d&               theTransformation,
				      Standard_Real&           theDistortionArea);

// ==============================================================================
// function: GetCommonEdges
// purpose : This function returns the list of common edges of two faces.
// ==============================================================================
static void GetCommonEdges(const TopoDS_Face          &theFace1,
			   const TopoDS_Face          &theFace2,
			         TopTools_ListOfShape &theCommonEdges)
{
  TopTools_IndexedMapOfShape aMapEdges1;
  TopExp_Explorer            anExp2(theFace2, TopAbs_EDGE);

  TopExp::MapShapes(theFace1, TopAbs_EDGE, aMapEdges1);
  theCommonEdges.Clear();

  for (; anExp2.More(); anExp2.Next()) {
    const TopoDS_Shape &anEdge2 = anExp2.Current();

    if (aMapEdges1.Contains(anEdge2))
      theCommonEdges.Append(anEdge2);
  }
}

// ==============================================================================
// function: GetWires
// purpose : This function returns a list of wires edges composed from theEdges.
// ==============================================================================
static Standard_Boolean GetWires(const TopTools_ListOfShape &theEdges,
				       TopTools_ListOfShape &theWires)
{
  TopTools_ListIteratorOfListOfShape anEdgeIter(theEdges);
  TopTools_DataMapOfShapeListOfShape aMapVE;
  TopExp_Explorer                    anExp;
  TopTools_MapOfShape                aMapUsed;

  for (; anEdgeIter.More(); anEdgeIter.Next()) {
    const TopoDS_Shape &anEdge = anEdgeIter.Value();

    if (!aMapUsed.Add(anEdge) || BRep_Tool::Degenerated(TopoDS::Edge(anEdge)))
      continue;

    for (anExp.Init(anEdge, TopAbs_VERTEX); anExp.More(); anExp.Next()) {
      const TopoDS_Shape &aVertex = anExp.Current();

      if (!aMapVE.IsBound(aVertex)) {
	TopTools_ListOfShape aList;
	aMapVE.Bind(aVertex, aList);
      }

      TopTools_ListOfShape &aListAncEdges = aMapVE.ChangeFind(aVertex);

      aListAncEdges.Append(anEdge);
    }
  }

  // Construct wires as chains of edges.
  Standard_Boolean isToTreat = Standard_True;
  TopoDS_Vertex    aFirstVtx;
  TopoDS_Edge      aCurEdge;
  TopoDS_Vertex    aV[2];
  BRep_Builder     aBuilder;

  for(;;) {
    isToTreat = Standard_False;

    // Get the bounding vertex, i.e. the vertex that contains only one edge in
    // the list of ancestors.
    TopTools_DataMapIteratorOfDataMapOfShapeListOfShape aMapIter(aMapVE);

    for (; aMapIter.More(); aMapIter.Next()) {
      const TopTools_ListOfShape &anEdges = aMapIter.Value();

      if (anEdges.Extent() == 1) {
	isToTreat = Standard_True;
	break;
      }
    }

    if (!isToTreat) {
      // Get a closed loop.
      for (aMapIter.Initialize(aMapVE); aMapIter.More(); aMapIter.Next()) {
	const TopTools_ListOfShape &anEdges = aMapIter.Value();

	if (anEdges.Extent() == 1) {
	  isToTreat = Standard_True;
	  break;
	}
      }
    }

    // Stop treatment if there is no more edges.
    if (!isToTreat)
      break;

    // Treat the list of edges starting from the current iterator.
    TopoDS_Wire aWire;

    aBuilder.MakeWire(aWire);
    aFirstVtx = TopoDS::Vertex(aMapIter.Key());

    while (!aFirstVtx.IsNull()) {
      const TopTools_ListOfShape &anEdges = aMapVE.ChangeFind(aFirstVtx);

      aCurEdge  = TopoDS::Edge(anEdges.First());
      TopExp::Vertices(aCurEdge, aV[0], aV[1], Standard_True);

      if (aFirstVtx.IsSame(aV[1])) {
	aV[1] = aV[0];
	aV[0] = aFirstVtx;
	aCurEdge.Reverse();
      }

      aBuilder.Add(aWire, aCurEdge);

      TopTools_ListOfShape &anEdges1 = aMapVE.ChangeFind(aV[0]);
      TopTools_ListOfShape &anEdges2 = aMapVE.ChangeFind(aV[1]);

      // Remove the current edge from aMapVE.
      anEdges1.RemoveFirst();

      if (anEdges1.IsEmpty())
	aMapVE.UnBind(aV[0]);

      if (anEdges2.IsEmpty() || anEdges2.Extent() > 2)
	return Standard_False;

      if (aCurEdge.IsSame(anEdges2.First())) {
	// Remove first element.
	anEdges2.RemoveFirst();
      } else if (aCurEdge.IsSame(anEdges2.Last())) {
	// Remove last element.
	TopTools_ListIteratorOfListOfShape anEdgeIt(anEdges2);

	anEdgeIt.Next();
	anEdges2.Remove(anEdgeIt);
      } else {
	return Standard_False;
      }

      if (anEdges2.IsEmpty()) {
	aMapVE.UnBind(aV[1]);
	aFirstVtx.Nullify();
      } else {
	aFirstVtx = aV[1];
      }
    }

    theWires.Append(aWire);
  }

  return Standard_True;
}

// ==============================================================================
// function: Constructor
// ==============================================================================
Unfolding_Shell::Unfolding_Shell()
     : myTolContour(0.001),
       myTolCurvature(0.001),
       myDeflection(0.001),
       myDistortionArea(0.),
       myCurvature(0.)
{
  Reset();
}

// ==============================================================================
// function: Constructor
// ==============================================================================
Unfolding_Shell::Unfolding_Shell(const TopoDS_Shell& theShell,
				 const gp_Pln&       thePlane,
				 const Standard_Real theContourTolerance,
				 const Standard_Real theCurvatureTolerance,
				 const Standard_Real theDeflection)
     : myShell(theShell),
       myPlane(thePlane),
       myTolContour(theContourTolerance),
       myTolCurvature(theCurvatureTolerance),
       myDeflection(theDeflection),
       myDistortionArea(0.),
       myCurvature(0.)
{
  Reset();
}

// ==============================================================================
// function: Perform
// ==============================================================================
Standard_Boolean Unfolding_Shell::Perform() 
{
  // Check if the operation is already performed and there is nothing to do.
  if (myErrorStatus != Unfolding_NotDone)
    return (myErrorStatus == Unfolding_Done);
  
  // Check input data validity.
  if (myShell.IsNull() || 
      ( myTolContour <= RealEpsilon() ) ||
      ( myTolCurvature <= RealEpsilon() ) ||
      ( myDeflection <= RealEpsilon() ) ) {
    myErrorStatus = Unfolding_InvalidInput;
    return Standard_False;
  }

  // Compute number of samples for each edge of the shell.
  TopTools_DataMapOfShapeInteger            aMapEdgeNbSamples;
  TopTools_IndexedDataMapOfShapeListOfShape aMapEF;
  Standard_Integer                          i;
  Standard_Integer                          aNbEdges;
  

  TopExp::MapShapesAndAncestors(myShell, TopAbs_EDGE, TopAbs_FACE, aMapEF);
  aNbEdges = aMapEF.Extent();

  for (i = 1; i <= aNbEdges; i++) {
    TopoDS_Edge                 anEdge    = TopoDS::Edge(aMapEF.FindKey(i));
    const TopTools_ListOfShape &aFaces    = aMapEF.FindFromIndex(i);
    Standard_Integer            aNbPoints = Unfolding::NbSamples(anEdge, aFaces,
								 myTolContour);

    aMapEdgeNbSamples.Bind(anEdge, aNbPoints);
  }

  // Perform unfolding of each face.
  TopTools_ListOfShape aList2Sew;
  myDistortionArea = 0.;
  myCurvature = 0.;
  TopExp_Explorer anExp( myShell, TopAbs_FACE );
  Standard_Boolean isFirstReversed = Standard_False;

  for ( ; anExp.More(); anExp.Next() ) {
    TopoDS_Face aFaceOriginal = TopoDS::Face( anExp.Current() );
    TopoDS_Face aFace = aFaceOriginal;
    aFace.Orientation( TopAbs_FORWARD );

    Unfolding_Surface anUnfoldFace(aFace, myPlane, myTolContour, 
				   myTolCurvature, myDeflection);

    Standard_Boolean                    isDone    =
                                   anUnfoldFace.Perform(aMapEdgeNbSamples);
    Handle(Unfolding_FaceDataContainer) aFaceData = anUnfoldFace.GetDataContainer();

    // Keep the original orientation of unfolding data container.
    aFaceData->SetFace(aFaceOriginal);

    if ( myCurvature < aFaceData->GetMaxGaussCurvature() )
      myCurvature = aFaceData->GetMaxGaussCurvature();
    myDistortionArea += aFaceData->GetDistortionArea();

    if ( !isDone ) {
      myErrorStatus = anUnfoldFace.ErrorStatus();
      return Standard_False;
    }
    if ( myMapFaceData.IsEmpty() ) {
      isFirstReversed = ( aFaceOriginal.Orientation() == TopAbs_REVERSED );
    }

    TopoDS_Face aNewFace = aFaceData->GetUnfoldedFace();

    Standard_Boolean toReverse = ( aFaceOriginal.Orientation() == TopAbs_REVERSED );
    if ( isFirstReversed ) 
      toReverse = !toReverse;
    gp_Trsf aTrsfPln;

    if ( toReverse ) {
      // Take into account the faces of different orientation are unfolded
      // as mirror compared to the first unfolded face representation
      aTrsfPln.SetMirror( gp_Ax1( myPlane.Location(), myPlane.Position().XDirection() ) );
      aFaceData->ApplyTrsf(aTrsfPln);
    }

    myMapFaceData.Add(aFaceData);
  }
  //

  if ( !ComputeTransformed( aList2Sew ) ) {
    return Standard_False;
  }

  //
  BRep_Builder                       aBuilder;
  TopoDS_Compound                    aCompound;
  TopTools_ListIteratorOfListOfShape anIt( aList2Sew );

  aBuilder.MakeCompound(aCompound);

  for ( ; anIt.More(); anIt.Next() )
    aBuilder.Add(aCompound, anIt.Value());

  Unfolding_ErrorStatus aStatus;
  TopoDS_Shell          aShell = Unfolding::ToShell(aCompound,
						    myTolContour, aStatus);

  if (aShell.IsNull()) {
    switch (aStatus) {
    case Unfolding_ComplexShape:
      myErrorStatus = Unfolding_ComplexShape;
      break;
    case Unfolding_InvalidShape:
      myErrorStatus = Unfolding_InvalidShape;
      break;
    default:
      myErrorStatus = Unfolding_Failure;
      break;
    }
    return Standard_False;
  }

  ShapeAnalysis_FreeBoundsProperties aFreeBoundAlgo(aShell);

  if ( !aFreeBoundAlgo.Perform() ) {
    myErrorStatus = Unfolding_Failure;
    return Standard_False;
  }

  Handle(ShapeAnalysis_HSequenceOfFreeBounds) aSeqWire;
  aSeqWire = aFreeBoundAlgo.ClosedFreeBounds();

  BRepBuilderAPI_MakeFace aFaceMaker(myPlane);   

  if ( !aSeqWire.IsNull() ) {
    for ( Standard_Integer w = 1; w <= aSeqWire->Length(); w++ ) {
      aFaceMaker.Add( aSeqWire->Value(w)->FreeBound() );
    }
  }
  myResult = aFaceMaker.Face();

#ifdef DEB
  cout << "Checking the result..."   << endl;
#endif
  BRepCheck_Analyzer aChecker( myResult, Standard_True );
  if ( !aChecker.IsValid() ) {
    myErrorStatus = Unfolding_ComplexShape;
    return Standard_False;
  }
  return Standard_True;
}

// ==============================================================================
// function: ComputeTransformed
// ==============================================================================
Standard_Boolean Unfolding_Shell::ComputeTransformed(TopTools_ListOfShape& theResult) 
{
  theResult.Clear();

  // If the map of face data is empty, nothing to do.
  if (myMapFaceData.IsEmpty())
    return Standard_True;

  TopTools_MapOfShape                       aMapTreated;
  TColStd_MapOfInteger                      aMapTreatedIndices;
  TopTools_IndexedDataMapOfShapeListOfShape aMapEF;
  Standard_Integer                          i;
  Standard_Integer                          aNbFaces = myMapFaceData.Extent();
  TColStd_ListOfInteger                     aListToTreat;
  Handle(Unfolding_FaceDataContainer)       aDummyFaceData;

  aDummyFaceData = new Unfolding_FaceDataContainer;
  TopExp::MapShapesAndAncestors( myShell, TopAbs_EDGE, TopAbs_FACE, aMapEF );

  for (i = 1; i <= aNbFaces; i++) {
    // Skip index already treated.
    if (aMapTreatedIndices.Contains(i))
      continue;

    aListToTreat.Append(i);

    while (!aListToTreat.IsEmpty()) {
      Standard_Integer anIndex = aListToTreat.First();

      if (!aMapTreatedIndices.Add(anIndex))
	continue;

      aListToTreat.RemoveFirst();

      const Handle(Unfolding_FaceDataContainer) &aFaceData =
                                               myMapFaceData.FindKey(anIndex);
      const TopoDS_Face                         &aFace     =
	                                       aFaceData->GetFace();

      aMapTreated.Add(aFace);

      TopExp_Explorer anEdgeExp(aFace, TopAbs_EDGE);

      for ( ; anEdgeExp.More(); anEdgeExp.Next() ) {
	const TopoDS_Shape &anEdge = anEdgeExp.Current();

	if (!aMapTreated.Add(anEdge) || !aMapEF.Contains(anEdge)) {
	  // This edge is already treated or it is absent in aMapEF. Skip it.
	  continue;
	}

	// Treat faces share the edge anEdge with the face aFace.
	const TopTools_ListOfShape         &aFaces = aMapEF.FindFromKey(anEdge);
	TopTools_ListIteratorOfListOfShape  aFaceIter(aFaces);

	for (; aFaceIter.More(); aFaceIter.Next()) {
	  const TopoDS_Shape &aNeighborFace = aFaceIter.Value();

	  // Skip aFace
	  if (aFace.IsSame(aNeighborFace))
	    continue;

	  aDummyFaceData->SetFace(TopoDS::Face(aNeighborFace));

	  Standard_Boolean isFixed        = !aMapTreated.Add(aNeighborFace);
	  Standard_Integer aNeighborIndex = myMapFaceData.FindIndex(aDummyFaceData);

	  if (aNeighborIndex == 0) {
	    myErrorStatus = Unfolding_Failure;

	    return Standard_False; // This is an invalid case.
	  }

	  const Handle(Unfolding_FaceDataContainer) &aNeighborFaceData =
                                           myMapFaceData.FindKey(aNeighborIndex);
	  TopTools_ListOfShape aCommonEdges;

	  GetCommonEdges(aFace, aNeighborFaceData->GetFace(), aCommonEdges);

	  if (!MoveFace2ToFace1(aFaceData, aNeighborFaceData,
				aCommonEdges, isFixed))
	    return Standard_False;

	  // Add common edge to the map of treated shapes.
	  TopTools_ListIteratorOfListOfShape anEdgeIter(aCommonEdges);

	  for (; anEdgeIter.More(); anEdgeIter.Next())
	    aMapTreated.Add(anEdgeIter.Value());

	  if (!isFixed)
	    aListToTreat.Append(aNeighborIndex);
	}
      }

      theResult.Append(aFaceData->GetUnfoldedFace());
    }
  }

  return Standard_True;
}


// ==============================================================================
// function: MoveFace2ToFace1
// ==============================================================================
Standard_Boolean Unfolding_Shell::MoveFace2ToFace1
                     (const Handle(Unfolding_FaceDataContainer) &theFaceData1,
		      const Handle(Unfolding_FaceDataContainer) &theFaceData2,
		      const TopTools_ListOfShape                &theCommonEdges,
		      const Standard_Boolean                     theIsFixed) 
{
  TopTools_ListOfShape aWires;

  if (!GetWires(theCommonEdges, aWires)) {
    myErrorStatus = Unfolding_Failure;

    return Standard_False;
  }

  // Get a wire with the greatest distance between first and last vertices.
  TopoDS_Wire                        aRefWire;
  TopoDS_Wire                        aWire;
  TopTools_ListIteratorOfListOfShape anIter(aWires);
  Standard_Real                      aMaxDist = -1;
  Standard_Real                      aDist;
  TopoDS_Vertex                      aV[2];

  for (; anIter.More(); anIter.Next()) {
    aWire = TopoDS::Wire(anIter.Value());
    TopExp::Vertices(aWire, aV[0], aV[1]);

    gp_Pnt aP1 = BRep_Tool::Pnt(aV[0]);
    gp_Pnt aP2 = BRep_Tool::Pnt(aV[1]);

    aDist = aP1.Distance(aP2);

    if (aDist > aMaxDist) {
      aMaxDist = aDist;
      aRefWire = aWire;
    }
  }

  if (aRefWire.IsNull()) {
    myErrorStatus = Unfolding_Failure;

    return Standard_False;
  }

  // Compute points.
  Standard_Boolean isReversedOn1 = Standard_False;
  Standard_Boolean isReversedOn2 = Standard_False;
  TopExp_Explorer  anExp(aRefWire, TopAbs_EDGE);

  if (anExp.More()) {
    TopoDS_Edge aFirstEdge = TopoDS::Edge(anExp.Current());
    TopoDS_Face aFace1     = theFaceData1->GetFace();
    TopoDS_Face aFace2     = theFaceData2->GetFace();

    TopExp_Explorer aFaceExp(aFace1, TopAbs_EDGE);

    for (; aFaceExp.More(); aFaceExp.Next()) {
      const TopoDS_Shape &anEdgeOnF1 = aFaceExp.Current();

      if (aFirstEdge.IsSame(anEdgeOnF1)) {
	if (aFirstEdge.Orientation() != anEdgeOnF1.Orientation())
	  isReversedOn1 = Standard_True;

	break;
      }
    }

    // If aFirstEdge is not found on the face1. It is not possible by construction.
    // Return with error status.
    if (!aFaceExp.More()) {
      myErrorStatus = Unfolding_Failure;

      return Standard_False;
    }

    if (aFace1.Orientation() == aFace2.Orientation())
      isReversedOn2 = !isReversedOn1;
    else
      isReversedOn2 =  isReversedOn1;
  }

  // Collect points.
  TopoDS_Shape         aFace1   = theFaceData1->GetUnfoldedFace();
  TopoDS_Shape         aFace2   = theFaceData2->GetUnfoldedFace();
  TColgp_SequenceOfXY  aMasterPoints;
  TColgp_SequenceOfXY  aSlavePoints;
  gp_Pnt2d             aUV[2];
  Handle(Geom_Surface) aSurface = new Geom_Plane(myPlane);
  TopLoc_Location      aLoc;

  for (; anExp.More(); anExp.Next()) {
    TopoDS_Edge                 anEdge   = TopoDS::Edge(anExp.Current());
    const TopTools_ListOfShape &anUnfld1 = theFaceData1->GetEdgesForEdge(anEdge);
    const TopTools_ListOfShape &anUnfld2 = theFaceData2->GetEdgesForEdge(anEdge);

    // Fill aMasterPoints.
    for (anIter.Initialize(anUnfld1); anIter.More(); anIter.Next()) {
      TopoDS_Edge anUnfoldEdge = TopoDS::Edge(anIter.Value());

      if (isReversedOn1)
	BRep_Tool::UVPoints(anUnfoldEdge, aSurface, aLoc, aUV[1], aUV[0]);
      else
	BRep_Tool::UVPoints(anUnfoldEdge, aSurface, aLoc, aUV[0], aUV[1]);

      // Add the first point if the collection is empty.
      if (aMasterPoints.IsEmpty())
	aMasterPoints.Append(aUV[0].XY());

      // Add the last point.
      if (isReversedOn1)
	aMasterPoints.Prepend(aUV[1].XY());
      else
	aMasterPoints.Append(aUV[1].XY());
    }

    // Fill aSlavePoints.
    for (anIter.Initialize(anUnfld2); anIter.More(); anIter.Next()) {
      TopoDS_Edge anUnfoldEdge = TopoDS::Edge(anIter.Value());

      if (isReversedOn2)
	BRep_Tool::UVPoints(anUnfoldEdge, aSurface, aLoc, aUV[1], aUV[0]);
      else
	BRep_Tool::UVPoints(anUnfoldEdge, aSurface, aLoc, aUV[0], aUV[1]);

      // Add the first point if the collection is empty.
      if (aSlavePoints.IsEmpty())
	aSlavePoints.Append(aUV[0].XY());

      // Add the last point.
      if (isReversedOn2)
	aSlavePoints.Prepend(aUV[1].XY());
      else
	aSlavePoints.Append(aUV[1].XY());
    }
  }

  // Construct array of points.
  Handle(TColgp_HArray1OfXY) aMasterPolyLine;
  Handle(TColgp_HArray1OfXY) aSlavePolyLine;
  Standard_Integer           i = 0;

  aMasterPolyLine = new TColgp_HArray1OfXY( 1, aMasterPoints.Length() );
  aSlavePolyLine  = new TColgp_HArray1OfXY( 1, aSlavePoints.Length() );

  for ( i = aMasterPolyLine->Lower(); i <= aMasterPolyLine->Upper(); i++ )
    aMasterPolyLine->SetValue( i, aMasterPoints(i) );

  for ( i = aSlavePolyLine->Lower(); i <= aSlavePolyLine->Upper(); i++ )
    aSlavePolyLine->SetValue( i, aSlavePoints(i) );

  // Compute transformation
  gp_Trsf2d     aTransformation2d;
  Standard_Real aDistortionArea;

  if ( !CombineCurves(aMasterPolyLine->Array1(), aSlavePolyLine->Array1(),
		      theIsFixed, myTolContour, myTolContour*myTolContour,
		      aTransformation2d, aDistortionArea) ) {
    myDistortionArea += aDistortionArea;
    myErrorStatus     = Unfolding_InvalidSurface;

    return Standard_False;
  }

  myDistortionArea += aDistortionArea;

  // Trasform face
  if (!theIsFixed) {
    gp_Trsf aTrsf2dTo3d( aTransformation2d );
    gp_Trsf aTrsfPln1, aTrsfPln2;
    gp_Ax3  anAx3( gp_Pnt(0,0,0), gp_Dir(0,0,1), gp_Dir(1,0,0));

    aTrsfPln1.SetDisplacement( myPlane.Position(), anAx3 );
    aTrsfPln2.SetDisplacement( anAx3, myPlane.Position() );

    gp_Trsf aTransformation = aTrsf2dTo3d * aTrsfPln1;

    aTransformation = aTrsfPln2 * aTransformation;
    theFaceData2->ApplyTrsf(aTransformation);
  }

  return Standard_True;
}

//-----------------------------------------------------------------------
//function : CombineCurves
//purpose  : 
//-----------------------------------------------------------------------
static Standard_Boolean CombineCurves(const TColgp_Array1OfXY& theMasterPolyLine,
				      const TColgp_Array1OfXY& theSlavePolyLine,
				      const Standard_Boolean   IsFixed,
				      const Standard_Real      theConfTolerance, 
				      const Standard_Real      theAreaTolerance, 
				      gp_Trsf2d&               theTransformation,
				      Standard_Real&           theDistortionArea)
{
  theDistortionArea = 0.;

  if ( ( theMasterPolyLine.Length() < 2 ) || ( theSlavePolyLine.Length() < 2 ) )
    return Standard_False;

  gp_XY aPMasterFirst = theMasterPolyLine.Value(theMasterPolyLine.Lower());
  gp_XY aPMasterLast = theMasterPolyLine.Value(theMasterPolyLine.Upper());
  gp_XY aPSlaveFirst = theSlavePolyLine.Value(theSlavePolyLine.Lower());
  gp_XY aPSlaveLast = theSlavePolyLine.Value(theSlavePolyLine.Upper());

  if (aPMasterFirst.IsEqual(aPMasterLast, theConfTolerance) ||
      aPSlaveFirst.IsEqual(aPSlaveLast, theConfTolerance)) {
    // One of polylines is closed. Get the points previous to the last ones.
    aPMasterLast = theMasterPolyLine.Value(theMasterPolyLine.Upper() - 1);
    aPSlaveLast  = theSlavePolyLine.Value(theSlavePolyLine.Upper() - 1);
    
    if (aPMasterFirst.IsEqual(aPMasterLast, theConfTolerance) ||
	aPSlaveFirst.IsEqual(aPSlaveLast, theConfTolerance))
      return Standard_False;
  }

  Standard_Real aRotation = 0.;
  gp_XY aTranslation;
      
  gp_Vec2d aLineDir1(aPMasterLast - aPMasterFirst);
  gp_Vec2d aLineDir2(aPSlaveLast - aPSlaveFirst);

  aTranslation = aPMasterFirst - aPSlaveFirst;
  Standard_Real aLen1 = aLineDir1.Magnitude();
  Standard_Real aLen2 = aLineDir2.Magnitude();
  Standard_Real aShift = ( aLen2 - aLen1 ) * 0.5;
  if ( aLen1 > gp::Resolution() ) {
    aTranslation = aTranslation - aLineDir1.XY() * (aShift / aLen1);
    if ( aLen2 > gp::Resolution() )
      aRotation = aLineDir1.Angle( aLineDir2 );
  }

  // compute base transformation
  gp_Trsf2d aRotTrsf;
  aRotTrsf.SetRotation(aPSlaveFirst, -aRotation);

  gp_Trsf2d aTransTrsf;
  aTransTrsf.SetTranslation( gp_Vec2d(aTranslation) );

  theTransformation = aTransTrsf * aRotTrsf;
  //

  // Compute transformation of shift to minimize the area of 
  // poly-lines difference
  gp_Dir2d aDir(1,0);
  if ( aLen1 > gp::Resolution() )
    aDir = gp_Dir2d( gp_XY(-aLineDir1.Y(), aLineDir1.X() ) ) ;
  Unfolding_FunctionWithDerivative aFunc( theMasterPolyLine, theSlavePolyLine, aDir, theTransformation );

  if (IsFixed) {
    Standard_Real aVal;

    aFunc.Value(0, aVal);
    theDistortionArea = aFunc.Area();    
    
    return Standard_True;
  } else {
    Standard_Real aBoundary = aShift * 20.;
    const Standard_Real aTolX = 1.e-05;
    if ( fabs(aBoundary) < aTolX * 2. ) {
      Standard_Real aVal = 0.;
      aFunc.Value(0, aVal);
      theDistortionArea = aFunc.Area();    
      return Standard_True;
    }
    const Standard_Real aTolF = ( theAreaTolerance < 1.e-06 ) ? 1.e-06 : theAreaTolerance;
  
    math_NewtonFunctionRoot R( aFunc, 0., aTolX, aTolF, -aBoundary, aBoundary, 10 );

    if ( R.IsDone() ) {
      gp_Trsf2d aTransTrsfAdd;
      aTransTrsfAdd.SetTranslation( gp_Vec2d(aDir.XY() * R.Root() ) );
      theTransformation = aTransTrsfAdd * theTransformation;
      theDistortionArea = aFunc.Area();
    }
    else {
      math_FunctionRoot R2( aFunc, 0., aTolX, -aBoundary, aBoundary );
      if ( R2.IsDone() ) {
	gp_Trsf2d aTransTrsfAdd;
	aTransTrsfAdd.SetTranslation( gp_Vec2d(aDir.XY() * R2.Root() ) );
	theTransformation = aTransTrsfAdd * theTransformation;
	theDistortionArea = aFunc.Area();
      }
    }
  }

  return Standard_True;
}
