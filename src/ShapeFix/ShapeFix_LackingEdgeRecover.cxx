// File:      ShapeFix_LackingEdgeRecover.cxx
// Created:   10.10.2013
// Author:    Briginas Ivan

#include <ShapeFix_LackingEdgeRecover.hxx>

#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>

#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>

#include <Geom_Curve.hxx>
#include <Geom_Surface.hxx>

#include <BRepLib.hxx>

#include <Geom2d_BSplineCurve.hxx>

#include <gp_Pnt2d.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec2d.hxx>

#include <TColgp_Array1OfPnt2d.hxx>
#include <TColgp_SequenceOfPnt.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_Array1OfInteger.hxx>

#include <NCollection_List.hxx>
#include <NCollection_Map.hxx>
#include <NCollection_DataMap.hxx>

#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Vertex.hxx>

#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>

#include <TopTools_IndexedMapOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>

#include <ShapeBuild_Edge.hxx>

#include <ShapeExtend.hxx>
#include <ShapeExtend_WireData.hxx>
#include <ShapeBuild_ReShape.hxx>

#include <ShapeFix_Edge.hxx>

#include <Standard_Assert.hxx>

IMPLEMENT_STANDARD_HANDLE (ShapeFix_LackingEdgeRecover, ShapeFix_Root)
IMPLEMENT_STANDARD_RTTIEXT(ShapeFix_LackingEdgeRecover, ShapeFix_Root)

inline Standard_Boolean IsEqual(const TopoDS_Edge& theEdge1,
                                const TopoDS_Edge& theEdge2)
{ 
  return theEdge1.IsSame(theEdge2); 
}

inline Standard_Boolean IsEqual(const TopoDS_Face& theFace1,
                                const TopoDS_Face& theFace2)
{ 
  return theFace1.IsEqual(theFace2); 
}

ShapeFix_LackingEdgeRecover::ShapeFix_LackingEdgeRecover()
{
  myStatus = ShapeExtend::EncodeStatus ( ShapeExtend_OK );
  SetPrecision(Precision::Confusion());
}

ShapeFix_LackingEdgeRecover::ShapeFix_LackingEdgeRecover(const TopoDS_Shape& theShape)
{
  this->Init(theShape);
}

TopoDS_Shape ShapeFix_LackingEdgeRecover::Shape() const
{
  return myRecoveredShape;
}

void ShapeFix_LackingEdgeRecover::Init(const TopoDS_Shape& theShape)
{
  myShape = theShape;
  if ( Context().IsNull() ) 
    SetContext ( new ShapeBuild_ReShape );

  myRecoveredShape = theShape;
}

void ShapeFix_LackingEdgeRecover::Perform()
{
  myRecoveredShape = Context()->Apply(myShape);
  
  TopTools_IndexedDataMapOfShapeListOfShape aVertexEdgesMap, aVertexWiresMap,
                                            anEdgeFacesMap,  aWireFacesMap;

  TopExp::MapShapesAndAncestors(myRecoveredShape, TopAbs_VERTEX, TopAbs_EDGE, aVertexEdgesMap);
  TopExp::MapShapesAndAncestors(myRecoveredShape, TopAbs_VERTEX, TopAbs_WIRE, aVertexWiresMap);
  TopExp::MapShapesAndAncestors(myRecoveredShape, TopAbs_EDGE,   TopAbs_FACE, anEdgeFacesMap);
  TopExp::MapShapesAndAncestors(myRecoveredShape, TopAbs_WIRE,   TopAbs_FACE, aWireFacesMap);

  NCollection_List< std::pair<TopoDS_Vertex, TopoDS_Vertex> > aVerticesForLackingEdgesBuilding;
  typedef NCollection_DataMap<TopoDS_Face, Standard_Integer> FaceDataMap;

  // search vertices with tolerance greater than specified threshold (theMaxTol)
  TopTools_IndexedMapOfShape aShapeVertexMap;
  TopExp::MapShapes(myRecoveredShape, TopAbs_VERTEX, aShapeVertexMap);

  Standard_Integer aVertIter = 1; // start from 1
  for (; aVertIter <= aShapeVertexMap.Extent(); ++aVertIter)
  {
    TopoDS_Vertex aCurVertex = TopoDS::Vertex (aShapeVertexMap.FindKey(aVertIter));
    Standard_Real aCurTol    = BRep_Tool::Tolerance (aCurVertex);

    // skip vertices with tolerance lesser than theMaxTol
    if (aCurTol < this->MaxTolerance())
      continue;

    const TopTools_ListOfShape& aSharingWiresOfCurVertex = aVertexWiresMap.FindFromKey (aCurVertex);

    // process each edge
    Standard_Boolean anIsVertexReplaced = Standard_False;
    gp_Pnt aFirstNewPnt, aSecondNewPnt; // these points will replace vertex with big tolerance

    TopTools_ListIteratorOfListOfShape anWireIter(aSharingWiresOfCurVertex);
    for (;anWireIter.More() && !anIsVertexReplaced; anWireIter.Next())
    {
      const TopoDS_Wire& aCurWire = TopoDS::Wire(anWireIter.Value());

      const TopTools_ListOfShape& aSharingFacesOfCurWire =
        aWireFacesMap.FindFromKey (anWireIter.Value());

      // (IMPORTANT) process just first found face
      TopTools_ListIteratorOfListOfShape aSharingFacesOfCurWireIter(aSharingFacesOfCurWire);
      for (; aSharingFacesOfCurWireIter.More() && !anIsVertexReplaced; aSharingFacesOfCurWireIter.Next())
      {
        // replace the vertex if sharing wire is strip
        anIsVertexReplaced = IsWireStrip(aCurWire, TopoDS::Face(aSharingFacesOfCurWireIter.Value()),
                                         aCurVertex, aFirstNewPnt, aSecondNewPnt);
      }
    }

    //int todo_streach_degenerated_edge;

    if (!anIsVertexReplaced)
      continue;


    const TopTools_ListOfShape& aSharingEdgesOfCurVertex =
      aVertexEdgesMap.FindFromKey (aCurVertex);

    FaceDataMap aSharingFacesMap;
    Standard_Boolean anIsCurVertexSkipped = Standard_False;

    // split sharing edges into two groups
    TColgp_SequenceOfPnt aFirstPntList, aSecondPntList;
    NCollection_Map<TopoDS_Edge> anEdgeMap;
    TopTools_ListIteratorOfListOfShape aSharingEdgesOfCurVertexIter(aSharingEdgesOfCurVertex);
    for (; aSharingEdgesOfCurVertexIter.More(); aSharingEdgesOfCurVertexIter.Next())
    {
      const TopoDS_Edge& aCurIntEdge = TopoDS::Edge(aSharingEdgesOfCurVertexIter.Value());

      if (!anEdgeMap.Contains(aCurIntEdge))
        anEdgeMap.Add(aCurIntEdge);
      else
        continue;

      if (!anEdgeFacesMap.Contains(aCurIntEdge))
        continue;
      const TopTools_ListOfShape& aSharingFacesOfCurIntEdge = anEdgeFacesMap.FindFromKey (aCurIntEdge);

      if (aSharingFacesOfCurIntEdge.IsEmpty())
        continue;

      // collect all sharing faces
      TopTools_ListIteratorOfListOfShape aSharingFacesIter(aSharingFacesOfCurIntEdge);
      for(; aSharingFacesIter.More(); aSharingFacesIter.Next())
      {
        const TopoDS_Face aCurShFace = TopoDS::Face(aSharingFacesIter.Value());

        if (!aSharingFacesMap.IsBound(aCurShFace))
          aSharingFacesMap.Bind(aCurShFace, 1);
        else
          aSharingFacesMap(aCurShFace)++;
      }

      // skip the case when a vertex is being shared by more 
      // than 2 edges that belong to one face
      Standard_Integer aMaxNbEdgesOnOneFace = 2;
      for (FaceDataMap::Iterator anIt(aSharingFacesMap);
           anIt.More() && !anIsCurVertexSkipped; anIt.Next())
      {
        if (anIt.Value() > aMaxNbEdgesOnOneFace)
          anIsCurVertexSkipped = Standard_True;
      }

      // break processing of edges that share the cur vertex
      if (anIsCurVertexSkipped)
        break;

      // take first sharing face
      const TopoDS_Face& aCurIntFace = TopoDS::Face(aSharingFacesOfCurIntEdge.First());

      TopLoc_Location aLocation;
      Handle(Geom_Surface) aCurIntSurf = BRep_Tool::Surface (aCurIntFace, aLocation);

      Standard_Real aPFirstParamOfCurIntEdge, aPLastParamOfCurIntEdge;
      Handle(Geom2d_Curve) aPCurveOfCurIntEdge = 
        BRep_Tool::CurveOnSurface (aCurIntEdge, aCurIntFace, 
        aPFirstParamOfCurIntEdge, aPLastParamOfCurIntEdge);

      // sort couple of vertices of the current internal edge
      TopoDS_Iterator anVerticesIter(aCurIntEdge, TopAbs_VERTEX);
      for (; anVerticesIter.More(); anVerticesIter.Next())
      {
        const TopoDS_Vertex& aCurInternalVertex = TopoDS::Vertex(anVerticesIter.Value());

        if(!aCurInternalVertex.IsSame(aCurVertex))
          continue;

        gp_Pnt2d anAddingPnt2D = ConvertTo2D(aCurIntEdge, aCurInternalVertex, aPCurveOfCurIntEdge,
          aPFirstParamOfCurIntEdge, aPLastParamOfCurIntEdge);

        gp_Pnt anAddingPnt;
        aCurIntSurf->D0(anAddingPnt2D.X(), anAddingPnt2D.Y(), anAddingPnt);

        Standard_Real aDist2ToFirstNewPnt  = aFirstNewPnt.SquareDistance(anAddingPnt);
        Standard_Real aDist2ToSecondNewPnt = aSecondNewPnt.SquareDistance(anAddingPnt);

        if (aDist2ToFirstNewPnt < aDist2ToSecondNewPnt)
          aFirstPntList.Append(anAddingPnt);
        else
          aSecondPntList.Append(anAddingPnt);
      }
    }

    if (anIsCurVertexSkipped)
      continue;

    if (aFirstPntList.IsEmpty() || aSecondPntList.IsEmpty())
      continue;

    // === process FIRST group
    // mean point of first group
    gp_XYZ aTotalXYZ;
    for (Standard_Integer aCurIndex = 1; aCurIndex <= aFirstPntList.Length(); aCurIndex++)
      aTotalXYZ += aFirstPntList.Value(aCurIndex).XYZ();

    gp_Pnt aMeanFirstNewPnt(aTotalXYZ/aFirstPntList.Length());
    // === END process FIRST group

    // === process SECOND group
    // mean point of second group
    aTotalXYZ = gp_XYZ();
    for (Standard_Integer aCurIndex = 1; aCurIndex <= aSecondPntList.Length(); aCurIndex++)
      aTotalXYZ += aSecondPntList.Value(aCurIndex).XYZ();

    gp_Pnt aMeanSecondNewPnt(aTotalXYZ/aSecondPntList.Length());
    // === END process SECOND group

    // create new vertices
    BRep_Builder aBuilder;
    TopoDS_Vertex aFirstNewVertex, aSecondNewVertex;
    // tolerance of vertices will be corrected during future edge replacement
    aBuilder.MakeVertex (aFirstNewVertex, aMeanFirstNewPnt, Precision::Confusion());
    aBuilder.MakeVertex (aSecondNewVertex, aMeanSecondNewPnt, Precision::Confusion());

    // fill in for building lacking edges
    aVerticesForLackingEdgesBuilding.Append(std::make_pair(aFirstNewVertex, aSecondNewVertex));

    // tool for fixing of vertex tolerance
    Handle(ShapeFix_Edge) anEdgeFixer = new ShapeFix_Edge;

    // update the shape to replace vertex with big tolerance by two new vertices 
    aSharingEdgesOfCurVertexIter.Initialize(aSharingEdgesOfCurVertex);
    for (; aSharingEdgesOfCurVertexIter.More(); aSharingEdgesOfCurVertexIter.Next())
    {
      const TopoDS_Edge& aCurIntUnUpdatedEdge = TopoDS::Edge(aSharingEdgesOfCurVertexIter.Value());

      // current updated edge
      TopoDS_Edge aCurIntEdge  = TopoDS::Edge(Context()->Apply(aCurIntUnUpdatedEdge));

      // the copy of the current internal updated edge
      TopoDS_Edge aModifedEdge = TopoDS::Edge(Context()->Apply(aCurIntUnUpdatedEdge));

      // take first sharing face
      const TopTools_ListOfShape& aSharingFacesOfCurIntEdge = anEdgeFacesMap.FindFromKey (aCurIntUnUpdatedEdge);
      const TopoDS_Face& aCurIntFace = TopoDS::Face(aSharingFacesOfCurIntEdge.First());

      TopoDS_Iterator aVertIterOfCurIntEdge(aCurIntEdge);
      for (; aVertIterOfCurIntEdge.More(); aVertIterOfCurIntEdge.Next())
      {
        const TopoDS_Vertex& aCurVertexOfCurIntEdge = TopoDS::Vertex(aVertIterOfCurIntEdge.Value());

        if (!aCurVertexOfCurIntEdge.IsSame(aCurVertex))
          continue;

        // replace the current vertex of the current internal edge by couple of new vertices
        aModifedEdge = ReplaceVertex(aCurVertexOfCurIntEdge, aModifedEdge, 
          aCurIntFace, aFirstPntList,
          aFirstNewVertex, aSecondNewVertex);

        // increase tolerance of vertices of the modified 
        // edge if the ones don't cover the tolerance 
        // of the modified edge
        anEdgeFixer->FixVertexTolerance(aModifedEdge);
      }

      // update the context
      Context()->Replace(aCurIntEdge, aModifedEdge);
    }
  }

  // apply the changes containing all replacements with vertices
  myRecoveredShape = Context()->Apply(myRecoveredShape);

  // add lacking edges into the modified shape (it doesn't contain vertices with big tolerance already)
  TopTools_IndexedDataMapOfShapeListOfShape anUpdatedVertexWiresMap, anUpdatedWireFacesMap;
  TopExp::MapShapesAndAncestors(myRecoveredShape, TopAbs_VERTEX, TopAbs_WIRE, anUpdatedVertexWiresMap);
  TopExp::MapShapesAndAncestors(myRecoveredShape, TopAbs_WIRE,   TopAbs_FACE, anUpdatedWireFacesMap);

  NCollection_List< std::pair<TopoDS_Vertex, TopoDS_Vertex> >::Iterator
    aVerticesForLackingEdgesIter(aVerticesForLackingEdgesBuilding);

  // run through all pairs of new vertices to add lacking edge between them
  for (; aVerticesForLackingEdgesIter.More(); aVerticesForLackingEdgesIter.Next())
  {
    const TopoDS_Vertex& aFirstNewVertex = aVerticesForLackingEdgesIter.Value().first;
    const TopoDS_Vertex& aSecondNewVertex = aVerticesForLackingEdgesIter.Value().second;

    // wires share the first new vertex
    if (!anUpdatedVertexWiresMap.Contains(aFirstNewVertex))
      continue;
    const TopTools_ListOfShape& aSharingWiresOfFirstNewVertex =
      anUpdatedVertexWiresMap.FindFromKey (aFirstNewVertex);

    // wires share the second new vertex
    if (!anUpdatedVertexWiresMap.Contains(aSecondNewVertex))
      continue;
    const TopTools_ListOfShape& aSharingWiresOfSecondNewVertex =
      anUpdatedVertexWiresMap.FindFromKey (aSecondNewVertex);

    // seek the intersection of two lists of sharing wires
    TopTools_ListIteratorOfListOfShape anIterOfFirstWireList(aSharingWiresOfFirstNewVertex);

    TopTools_ListOfShape anIntersectOfTwoLists;
    for (; anIterOfFirstWireList.More(); anIterOfFirstWireList.Next())
    {
      TopTools_ListIteratorOfListOfShape anIterOfSecondWireList(aSharingWiresOfSecondNewVertex);
      for (; anIterOfSecondWireList.More(); anIterOfSecondWireList.Next())
      {
        if (anIterOfFirstWireList.Value().IsSame(anIterOfSecondWireList.Value()))
          anIntersectOfTwoLists.Append(anIterOfFirstWireList.Value());
      }
    }

    // tool for fixing of vertex tolerance to cover edge tolerance that contains this vertex
    Handle(ShapeFix_Edge) anEdgeFixer = new ShapeFix_Edge;
    TopoDS_Edge aLackingEdge;
    BRep_Builder aBuilder;

    // add lacking edge to each intersected wire by replacing one their edge
    TopTools_ListIteratorOfListOfShape anIterOfIntersetionList(anIntersectOfTwoLists);
    for (; anIterOfIntersetionList.More(); anIterOfIntersetionList.Next())
    {
      const TopoDS_Wire& aCurWire = TopoDS::Wire(anIterOfIntersetionList.Value());

      // faces of doesn't updated wire
      const TopTools_ListOfShape& aSharingFacesOfCurWire =
        anUpdatedWireFacesMap.FindFromKey (aCurWire);

      if (aSharingFacesOfCurWire.Extent() == 0)
        continue;

      // _first_ found face containing the current wire
      const TopoDS_Face& aCurFace = TopoDS::Face(aSharingFacesOfCurWire.First());

      TopLoc_Location aLocation;
      Handle(Geom_Surface) aCurSurf = BRep_Tool::Surface (aCurFace, aLocation);

      TopoDS_Wire anUpdatedCurWire = TopoDS::Wire(Context()->Apply(aCurWire));

      // seek 2d points (from doesn't updated wire) for 2d pcurve building
      gp_Pnt2d aFirst2D, aSecond2D;
      ConvertLackingVerticesTo2D(aCurWire, aCurFace, aFirstNewVertex, aSecondNewVertex, aFirst2D, aSecond2D);

      // create 2d pcurve for lacking edge
      const Handle(Geom2d_BSplineCurve)& aBSplineLackingCurve2D = 
                                           MakeBSplineLackingCurve2D(aFirst2D, aSecond2D);

      if (aLackingEdge.IsNull())
        aLackingEdge = BRepBuilderAPI_MakeEdge(aBSplineLackingCurve2D, aCurSurf, aFirstNewVertex, aSecondNewVertex);

      // add pcurve to the lacking edge
      aBuilder.UpdateEdge(aLackingEdge, aBSplineLackingCurve2D, aCurSurf, aLocation, 
                          BRep_Tool::Tolerance(aLackingEdge));

      // set orientation of the lacking edge
      TopAbs_Orientation anOrientOfLackingEdge = TopAbs_FORWARD;
      for (TopoDS_Iterator anEdgeIter(aCurWire); anEdgeIter.More(); anEdgeIter.Next())
      {
        const TopoDS_Edge& aCurIntEdge = TopoDS::Edge(anEdgeIter.Value());

        if (aFirstNewVertex.IsSame(TopExp::FirstVertex(aCurIntEdge)))
        {
          if (aCurIntEdge.Orientation() == TopAbs_FORWARD)
            anOrientOfLackingEdge = TopAbs_REVERSED;
          else
            anOrientOfLackingEdge = TopAbs_FORWARD;

          break;
        }
        else if (aFirstNewVertex.IsSame(TopExp::LastVertex(aCurIntEdge)))
        {
          if (aCurIntEdge.Orientation() == TopAbs_FORWARD)
            anOrientOfLackingEdge = TopAbs_FORWARD;
          else
            anOrientOfLackingEdge = TopAbs_REVERSED;

          break;
        }
      }

      aLackingEdge = TopoDS::Edge(aLackingEdge.Oriented(anOrientOfLackingEdge));

      // add lacking edge to current wire
      anUpdatedCurWire.Free(Standard_True);
      aBuilder.Add(anUpdatedCurWire, aLackingEdge);

      Context()->Replace(aCurWire, anUpdatedCurWire);
    }

    BRepLib::BuildCurve3d(aLackingEdge);
    BRepLib::SameParameter(aLackingEdge, 1.e-5, Standard_True);
  }

  myRecoveredShape = Context()->Apply(myRecoveredShape);
  
  myStatus = ShapeExtend::EncodeStatus ( ShapeExtend_OK );
}

//=======================================================================
//function : ReplaceVertex
//purpose  : replace the input vertex by couple of new ones
//=======================================================================
TopoDS_Edge ShapeFix_LackingEdgeRecover::ReplaceVertex(const TopoDS_Vertex& theVert /*theBeingReplacedVertex*/,
                                                       const TopoDS_Edge&   theEdge /*theBeingReplacedEdge  */,
                                                       const TopoDS_Face&   theFace,
                                                       const TColgp_SequenceOfPnt&
                                                                            theListOfFirstPnt,
                                                       const TopoDS_Vertex& theFirstVertex,
                                                       const TopoDS_Vertex& theSecondVertex) const
{
  Standard_Real aPFirstParamOfEdge, aPLastParamOfEdge;
  Handle(Geom2d_Curve) aPCurveOfCurIntEdge = 
    BRep_Tool::CurveOnSurface (theEdge, theFace, aPFirstParamOfEdge, aPLastParamOfEdge);

  TopAbs_Orientation anOrientOfVert = theVert.Orientation();

  // seek the end of pcurve that shares the input vertex
  gp_Pnt2d aBeingSoughtPnt2D = ConvertTo2D(theEdge, theVert, aPCurveOfCurIntEdge, 
                                           aPFirstParamOfEdge, aPLastParamOfEdge);

  TopLoc_Location aLocation;
  Handle(Geom_Surface) aSurf = BRep_Tool::Surface (theFace, aLocation);

  // convert the found end of pcurve to point in 3D space
  gp_Pnt aBeingSoughtPnt;
  aSurf->D0(aBeingSoughtPnt2D.X(), aBeingSoughtPnt2D.Y(), aBeingSoughtPnt);

  // seek what list contains found 3D point
  Standard_Boolean anIsBeingSoughtPntInFirstList = Standard_False;
  Standard_Integer aCurIndex = 1;
  for (; aCurIndex <= theListOfFirstPnt.Length() && !anIsBeingSoughtPntInFirstList; aCurIndex++)
    if (aBeingSoughtPnt.Distance(theListOfFirstPnt.Value(aCurIndex)) <= Precision::Confusion())
      anIsBeingSoughtPntInFirstList = Standard_True;

  // choose what vertex in the edge will be replaced taking into account found 
  // list containing this found 3d point
  TopoDS_Vertex aBeingSoughtVertex;
  if (anIsBeingSoughtPntInFirstList)
    aBeingSoughtVertex = TopoDS::Vertex(theFirstVertex.Oriented(anOrientOfVert));
  else
    aBeingSoughtVertex = TopoDS::Vertex(theSecondVertex.Oriented(anOrientOfVert));

  TopoDS_Edge aModifiedEdge;
  ShapeBuild_Edge anEdgeBuilder;

  // replace the vertex in the edge by chosen vertex
  if (anOrientOfVert == TopAbs_FORWARD)
    aModifiedEdge = anEdgeBuilder.CopyReplaceVertices(theEdge, aBeingSoughtVertex, TopoDS_Vertex());
  else
    aModifiedEdge = anEdgeBuilder.CopyReplaceVertices(theEdge, TopoDS_Vertex(), aBeingSoughtVertex);

  return aModifiedEdge;
}

//=======================================================================
//function : IsWireStrip
//purpose  : find out wire is strip or not
//=======================================================================
Standard_Boolean ShapeFix_LackingEdgeRecover::IsWireStrip(const TopoDS_Wire& theWire,
                                                          const TopoDS_Face& theFace,
                                                          const TopoDS_Vertex& theVertex,
                                                          gp_Pnt& theFirstPnt, gp_Pnt& theSecondPnt) const
{
  // if the wide of the wire projection in parametric space of the face
  // on vector (which is formed by the first and second input points; theirs analogues in
  // parametric space of the face) will be less than this parameter - aStripRation 
  // then wire is detected as a strip
  Standard_Real aStripRatio = 2.0; 
  
  // number of equidistant points on each edge for wire projection
  // in parametric space on the vector that was described previously
  Standard_Integer aPntNbPerEdgeForProj = 10; 

  // the analogues of the first and second points in parametric space of the face
  gp_Pnt2d aPFirst2D, aPSecond2D;

  // the wire is explored by BRepTools_WireExplorer and it is 
  // guaranteed that edges will be SEQUENCIALLY connected
  Standard_Boolean isNOTExpByBRepTools_WireExplorer = Standard_False;
  Handle(ShapeExtend_WireData) anEdgeExp = 
    new ShapeExtend_WireData(theWire, isNOTExpByBRepTools_WireExplorer);

  Standard_Integer aStartIndex = 1; // start index of ShapeExtend_WireData
  Standard_Integer anEndIndex  = anEdgeExp->NbEdges();

  if (anEndIndex == 0) // the WIRE IS EMPTY
    return Standard_False;

  // run through all edges and seek couple of edges which share the vertex
  Standard_Integer anEdgeIndex, anEdgeIndex_Prev, anEdgeIndex_Next;
  for (anEdgeIndex = aStartIndex; anEdgeIndex <= anEndIndex; anEdgeIndex++)
  {
    // index of previous edge
    if (anEdgeIndex == aStartIndex) anEdgeIndex_Prev = anEndIndex;
    else anEdgeIndex_Prev = anEdgeIndex-1;

    // index of next element
    if (anEdgeIndex == anEndIndex) anEdgeIndex_Next = aStartIndex;
    else anEdgeIndex_Next = anEdgeIndex+1;

    const TopoDS_Edge& aCurEdge = anEdgeExp->Edge(anEdgeIndex);

    TopoDS_Vertex aCurFirstVertex = TopExp::FirstVertex(aCurEdge);
    TopoDS_Vertex aCurLastVertex  = TopExp::LastVertex(aCurEdge);

    Standard_Boolean anIsFirstBelong = theVertex.IsSame(aCurFirstVertex);
    Standard_Boolean anIsLastBelong  = theVertex.IsSame(aCurLastVertex);

    if (!anIsFirstBelong && !anIsLastBelong)
      continue;

    //int todo_investigate_degenerated_edge;

    Standard_Real aPFirstParamOfCurEdge, aPLastParamOfCurEdge;
    Handle(Geom2d_Curve) aPCurveOfCurEdge = 
      BRep_Tool::CurveOnSurface (aCurEdge, theFace, aPFirstParamOfCurEdge, aPLastParamOfCurEdge);

    if (anIsFirstBelong && anIsLastBelong) // both ends of current edge belong to the vertex
    {
      aPCurveOfCurEdge->D0(aPFirstParamOfCurEdge, aPFirst2D);
      aPCurveOfCurEdge->D0(aPLastParamOfCurEdge, aPSecond2D);
    }
    else
    {
      if (anIsFirstBelong) // first end of current edge
        aPCurveOfCurEdge->D0(aPFirstParamOfCurEdge, aPFirst2D);
      else if (anIsLastBelong) // last end of current edge
        aPCurveOfCurEdge->D0(aPLastParamOfCurEdge, aPFirst2D);

      // is second edge nether next edge in the sequence or previous?
      TopAbs_Orientation aCurEdgeOrient = aCurEdge.Orientation();
      Standard_Boolean anIsSecondANextEdge = 
        (aCurEdgeOrient == TopAbs_FORWARD  && anIsLastBelong) ||
        (aCurEdgeOrient == TopAbs_REVERSED && anIsFirstBelong);

      Standard_Integer anIndexOfEdgeWithSecondPnt = 
        (anIsSecondANextEdge ? anEdgeIndex_Next : anEdgeIndex_Prev);

      //
      const TopoDS_Edge& anEdgeWithSecondPnt = anEdgeExp->Edge(anIndexOfEdgeWithSecondPnt);
      TopAbs_Orientation anOrientOfEdgeWithSecondPnt = anEdgeWithSecondPnt.Orientation();

      Standard_Boolean isFirstVertexOfEdgeWithSecondPnt = Standard_True;
      if (anIsSecondANextEdge) // next edge in the edge sequence
      {
        if (anOrientOfEdgeWithSecondPnt == TopAbs_FORWARD)
          isFirstVertexOfEdgeWithSecondPnt = Standard_True;
        else
          isFirstVertexOfEdgeWithSecondPnt = Standard_False;
      }
      else // previous edge in sequence in the edge sequence
      {
        if (anOrientOfEdgeWithSecondPnt == TopAbs_FORWARD)
          isFirstVertexOfEdgeWithSecondPnt = Standard_False;
        else
          isFirstVertexOfEdgeWithSecondPnt = Standard_True;
      }

      Standard_Real aFirstParamOfEdgeWithSecondPnt, aLastParamOfEdgeWithSecondPnt;
      Handle(Geom2d_Curve) aPCurveOfEdgeWithSecondPnt =
        BRep_Tool::CurveOnSurface (anEdgeWithSecondPnt, theFace,
        aFirstParamOfEdgeWithSecondPnt,
        aLastParamOfEdgeWithSecondPnt);

      if (isFirstVertexOfEdgeWithSecondPnt)
        aPCurveOfEdgeWithSecondPnt->D0(aFirstParamOfEdgeWithSecondPnt, aPSecond2D);
      else
        aPCurveOfEdgeWithSecondPnt->D0(aLastParamOfEdgeWithSecondPnt, aPSecond2D);
    }

    if (aPFirst2D.Distance(aPSecond2D) <= Precision::Confusion())
      continue;

    // project points of edges on aPVec2D
    Standard_Real aProjLength = ProjectTo2DVector(anEdgeExp, theFace, aPntNbPerEdgeForProj, aPFirst2D, aPSecond2D);
    
    gp_Vec2d aPVec2D(aPFirst2D, aPSecond2D);
    if (aProjLength <= aStripRatio*aPVec2D.Magnitude())
    {
      TopLoc_Location aLocation;
      Handle(Geom_Surface) aSurf = BRep_Tool::Surface (theFace, aLocation);

      aSurf->D0(aPFirst2D.X() , aPFirst2D.Y(),  theFirstPnt);
      aSurf->D0(aPSecond2D.X(), aPSecond2D.Y(), theSecondPnt);

      return Standard_True;
    }
  }

  return Standard_False;
}

//=======================================================================
//function : ConvertLackingVerticesTo2D
//purpose  : 
//=======================================================================
void ShapeFix_LackingEdgeRecover::ConvertLackingVerticesTo2D(const TopoDS_Wire&   theWire, 
                                                             const TopoDS_Face&   theFace, 
                                                             const TopoDS_Vertex& theFirstVertex,
                                                             const TopoDS_Vertex& theSecondVertex, 
                                                             gp_Pnt2d&      theFirstPnt2D, 
                                                             gp_Pnt2d&      theSecondPnt2D) const
{
  Standard_Boolean aHasFirstBeenFound  = Standard_False;
  Standard_Boolean aHasSecondBeenFound = Standard_False;
  TopoDS_Iterator anEdgeIter(theWire);
  for (; anEdgeIter.More(); anEdgeIter.Next())
  {
    const TopoDS_Edge& aCurEdge = TopoDS::Edge(anEdgeIter.Value());

    Standard_Real aPFirstParamOfCurEdge, aPLastParamOfCurEdge;
    Handle(Geom2d_Curve) aCurCurve2D = 
      BRep_Tool::CurveOnSurface(aCurEdge, theFace, aPFirstParamOfCurEdge, aPLastParamOfCurEdge);

    TopoDS_Iterator aVertexIter(aCurEdge);
    for (; aVertexIter.More(); aVertexIter.Next())
    {
      const TopoDS_Vertex& aCurVertex = TopoDS::Vertex(aVertexIter.Value());
      if (!aHasFirstBeenFound)
      {
        if (theFirstVertex.IsSame(aCurVertex))
        {
          aHasFirstBeenFound = Standard_True;
          theFirstPnt2D = ConvertTo2D(aCurEdge, aCurVertex, aCurCurve2D, 
                                      aPFirstParamOfCurEdge, aPLastParamOfCurEdge);
        }
      }

      if (!aHasSecondBeenFound)
      {
        if (theSecondVertex.IsSame(aCurVertex))
        {
          aHasSecondBeenFound = Standard_True;
          theSecondPnt2D = ConvertTo2D(aCurEdge, aCurVertex, aCurCurve2D, 
                                       aPFirstParamOfCurEdge, aPLastParamOfCurEdge);
        }
      }
    }
    
    if (aHasFirstBeenFound && aHasSecondBeenFound)
      break;
  }
}

//=======================================================================
//function : MakeBSplineLackingCurve2D
//purpose  :
//=======================================================================
Handle(Geom2d_BSplineCurve) ShapeFix_LackingEdgeRecover::MakeBSplineLackingCurve2D(
                                                                  const gp_Pnt2d& theFirstPnt2D,
                                                                  const gp_Pnt2d& theSecondPnt2D) const
{
  TColgp_Array1OfPnt2d aPolesOfBSplineCurve (1, 2);
  aPolesOfBSplineCurve(1).SetCoord(theFirstPnt2D.X(), theFirstPnt2D.Y());
  aPolesOfBSplineCurve(2).SetCoord(theSecondPnt2D.X(), theSecondPnt2D.Y());

  TColStd_Array1OfReal aKnotesOfBSplineCurve(1, 2);
  aKnotesOfBSplineCurve(1) = 0.0;
  aKnotesOfBSplineCurve(2) = 1.0;

  TColStd_Array1OfInteger aMultiplicities (1, 2);
  aMultiplicities(1) = 2;
  aMultiplicities(2) = 2;

  return new Geom2d_BSplineCurve(aPolesOfBSplineCurve, aKnotesOfBSplineCurve, aMultiplicities, 1, 0);
}

//=======================================================================
//function : ConvertTo2D
//purpose  :
//=======================================================================
gp_Pnt2d ShapeFix_LackingEdgeRecover::ConvertTo2D(const TopoDS_Edge& theEdge,
                                                  const TopoDS_Vertex& theVertex,
                                                  const Handle(Geom2d_Curve)& theCurve2D,
                                                  const Standard_Real thePFirst,
                                                  const Standard_Real thePLast) const
{
  gp_Pnt2d aPnt2d;
  if (theEdge.Orientation() == TopAbs_FORWARD)
  {
    if (theVertex.Orientation() == TopAbs_FORWARD)
      theCurve2D->D0(thePFirst, aPnt2d);
    else
      theCurve2D->D0(thePLast, aPnt2d);
  }
  else
  {
    if (theVertex.Orientation() == TopAbs_FORWARD)
      theCurve2D->D0(thePLast, aPnt2d);
    else
      theCurve2D->D0(thePFirst, aPnt2d);
  }

  return aPnt2d;
}

//=======================================================================
//function : ProjectTo2DVector
//purpose  :
//=======================================================================
Standard_Real ShapeFix_LackingEdgeRecover::ProjectTo2DVector(const Handle(ShapeExtend_WireData)& theWireData,
                                                             const TopoDS_Face& theFace,
                                                             const Standard_Real thePntNbPerEdgeForProj,
                                                             const gp_Pnt2d& thePFirst2D,
                                                             const gp_Pnt2d& thePSecond2D) const
{
  gp_Vec2d aPVec2D(thePFirst2D, thePSecond2D);

  Standard_Integer aStartIndex = 1; // start index of ShapeExtend_WireData
  Standard_Integer anEndIndex  = theWireData->NbEdges();

  // project points of edges on aPVec2D
  Standard_Real aMaxWireValue = -Precision::Infinite();
  Standard_Real aMinWireValue =  Precision::Infinite();

  Standard_Integer aCurEdgeIndex = aStartIndex;
  for (; aCurEdgeIndex <= anEndIndex; aCurEdgeIndex++)
  {
    Standard_Real aPFirstParamOfIntCurEdge, aPLastParamOfIntCurEdge;
    Handle(Geom2d_Curve) aPCurveOfIntCurEdge = 
      BRep_Tool::CurveOnSurface (theWireData->Edge(aCurEdgeIndex), theFace,
      aPFirstParamOfIntCurEdge, aPLastParamOfIntCurEdge);

    // split the curve of the current edge on aPntNbPerEdgeForProj parts
    Standard_Integer aCurStep = 0;
    Standard_Real  aPCurParam = aPFirstParamOfIntCurEdge;
    Standard_Real   aStepSize = (aPLastParamOfIntCurEdge - aPFirstParamOfIntCurEdge)/thePntNbPerEdgeForProj;
    while (aPCurParam <= aPLastParamOfIntCurEdge)
    {
      aPCurParam = aPFirstParamOfIntCurEdge + aCurStep*aStepSize;

      gp_Pnt2d aPCur2D;
      aPCurveOfIntCurEdge->D0(aPCurParam, aPCur2D);

      // project current parametric point on the vector (by scalar product)
      Standard_Real aStepFromPFirst2D = aPVec2D*gp_Vec2d(thePFirst2D, aPCur2D) / aPVec2D.Magnitude();

      aMaxWireValue = Max(aMaxWireValue, aStepFromPFirst2D);
      aMinWireValue = Min(aMinWireValue, aStepFromPFirst2D);

      aCurStep++;
    }
  }

  return Abs(aMaxWireValue - aMinWireValue);
}