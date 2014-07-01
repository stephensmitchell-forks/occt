// Created on: 1993-05-12
// Created by: Didier PIFFAULT
// Copyright (c) 1993-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#include <BRepMesh_Delaun.hxx>

#include <gp.hxx>
#include <gp_XY.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec2d.hxx>

#include <Precision.hxx>
#include <Bnd_Box2d.hxx>

#include <TColStd_MapOfInteger.hxx>
#include <TColStd_MapIteratorOfMapOfInteger.hxx>
#include <TColStd_Array1OfBoolean.hxx>
#include <TColStd_ListIteratorOfListOfInteger.hxx>
#include <TColStd_ListOfInteger.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TColStd_SequenceOfInteger.hxx>

#include <BRepMesh_MapOfIntegerInteger.hxx>
#include <BRepMesh_HeapSortIndexedVertexOfDelaun.hxx>
#include <BRepMesh_ComparatorOfIndexedVertexOfDelaun.hxx>
#include <BRepMesh_HeapSortIndexedVertexOfDelaun.hxx>
#include <BRepMesh_SelectorOfDataStructureOfDelaun.hxx>
#include <BRepMesh_HeapSortVertexOfDelaun.hxx>
#include <BRepMesh_ComparatorOfVertexOfDelaun.hxx>
#include <BRepMesh_Array1OfVertexOfDelaun.hxx>

#include <BRepMesh_Edge.hxx>
#include <BRepMesh_Vertex.hxx>
#include <BRepMesh_Triangle.hxx>

typedef TColStd_ListIteratorOfListOfInteger  IteratorOnListOfInteger;
typedef TColStd_ListOfInteger                ListOfInteger;

const Standard_Real Precision    = Precision::PConfusion();
const Standard_Real Precision2   = Precision * Precision;

const gp_XY SortingDirection(M_SQRT1_2, M_SQRT1_2);

//=======================================================================
//function : fillBndBox
//purpose  : Add boundig box for edge defined by start & end point to
//           the given sequence of bounding boxes for triangulation edges
//=======================================================================
static void fillBndBox( NCollection_Sequence <Bnd_Box2d>& theVB,
                        const BRepMesh_Vertex& theV1,
                        const BRepMesh_Vertex& theV2,
                        const Standard_Integer thePosition = 0)
{
  Bnd_Box2d aBox;      
  aBox.Add( gp_Pnt2d( theV1.Coord() ) );
  aBox.Add( gp_Pnt2d( theV2.Coord() ) );
  if ( thePosition < 1 || thePosition > theVB.Size() )
    theVB.Append( aBox );
  else
    theVB.SetValue( thePosition, aBox );
}

//=======================================================================
//function : BRepMesh_Delaun
//purpose  : Creates the triangulation with an empty Mesh data structure
//=======================================================================
BRepMesh_Delaun::BRepMesh_Delaun( BRepMesh_Array1OfVertexOfDelaun& theVertices,
                                  const Standard_Boolean isPositive,
                                  const Handle_Message_ProgressIndicator & theProgress)
: myIsPositiveOrientation( isPositive ),
  myCircles( theVertices.Length(), new NCollection_IncAllocator() ),
  myProgress(theProgress)
{
  if ( theVertices.Length() > 2 )
  {
    myMeshData = new BRepMesh_DataStructureOfDelaun( new NCollection_IncAllocator(),
                                                     theVertices.Length() );
    Init( theVertices );
  }
}

//=======================================================================
//function : BRepMesh_Delaun
//purpose  : Creates the triangulation with and existent Mesh data structure
//=======================================================================
BRepMesh_Delaun::BRepMesh_Delaun ( const Handle( BRepMesh_DataStructureOfDelaun )& theOldMesh, 
                                   BRepMesh_Array1OfVertexOfDelaun& theVertices,
                                   const Standard_Boolean isPositive,
                                   const Handle_Message_ProgressIndicator & theProgress )
 : myIsPositiveOrientation( isPositive ),
   myCircles( theVertices.Length(), theOldMesh->Allocator() ),
   myProgress(theProgress)
{
  myMeshData = theOldMesh;
  if ( theVertices.Length() > 2 )
    Init( theVertices );
}

//=======================================================================
//function : BRepMesh_Delaun
//purpose  : Creates the triangulation with and existent Mesh data structure
//=======================================================================
BRepMesh_Delaun::BRepMesh_Delaun( const Handle( BRepMesh_DataStructureOfDelaun )& theOldMesh, 
                                  TColStd_Array1OfInteger& theVertexIndices,
                                  const Standard_Boolean isPositive,
                                  const Handle_Message_ProgressIndicator & theProgress )
 : myIsPositiveOrientation( isPositive ),
   myCircles( theVertexIndices.Length(), theOldMesh->Allocator() ),
   myProgress(theProgress)
{
  myMeshData = theOldMesh;
  if ( theVertexIndices.Length() > 2 )
  {
    Bnd_Box2d aBox;
    Standard_Integer anIndex = theVertexIndices.Lower();
    Standard_Integer anUpper = theVertexIndices.Upper();
    for ( ; anIndex <= anUpper; ++anIndex )
      aBox.Add( gp_Pnt2d( GetVertex( theVertexIndices( anIndex) ).Coord() ) );

    Perform( aBox, theVertexIndices );
  }
}

//=======================================================================
//function : Init
//purpose  : Initializes the triangulation with an Array of Vertex
//=======================================================================
void BRepMesh_Delaun::Init( BRepMesh_Array1OfVertexOfDelaun& theVertices )
{
  Bnd_Box2d aBox;
  Standard_Integer aLowerIdx  = theVertices.Lower();
  Standard_Integer anUpperIdx = theVertices.Upper();
  TColStd_Array1OfInteger aVertexIndices( aLowerIdx, anUpperIdx );
  
  Standard_Integer anIndex = aLowerIdx;
  for ( ; anIndex <= anUpperIdx; ++anIndex )
  {
    aBox.Add( gp_Pnt2d( theVertices( anIndex ).Coord() ) );
    aVertexIndices( anIndex ) = myMeshData->AddNode( theVertices( anIndex ) );
  }

  Perform( aBox, aVertexIndices );
}

//=======================================================================
//function : Perform
//purpose  : Create super mesh and run triangulation procedure
//=======================================================================
void BRepMesh_Delaun::Perform( Bnd_Box2d&               theBndBox,
                               TColStd_Array1OfInteger& theVertexIndices )
{
  theBndBox.Enlarge( Precision::PConfusion() );
  SuperMesh( theBndBox );

  BRepMesh_HeapSortIndexedVertexOfDelaun::Sort( theVertexIndices, 
      BRepMesh_ComparatorOfIndexedVertexOfDelaun( SortingDirection,
        Precision::PConfusion(), myMeshData ) );

  Compute( theVertexIndices );
}

//=======================================================================
//function : SuperMesh
//purpose  : Build the super mesh
//=======================================================================
void BRepMesh_Delaun::SuperMesh( const Bnd_Box2d& theBox )
{
  Standard_Real aMinX, aMinY, aMaxX, aMaxY;
  theBox.Get( aMinX, aMinY, aMaxX, aMaxY );
  Standard_Real aDeltaX = aMaxX - aMinX;
  Standard_Real aDeltaY = aMaxY - aMinY;

  Standard_Real aDeltaMin = Min( aDeltaX, aDeltaY );
  Standard_Real aDeltaMax = Max( aDeltaX, aDeltaY );
  Standard_Real aDelta    = aDeltaX + aDeltaY;
  
  myCircles.SetMinMaxSize( gp_XY( aMinX, aMinY ), gp_XY( aMaxX, aMaxY ) );

  Standard_Integer aScaler = 2;
  if ( myMeshData->NbNodes() > 100 )
    aScaler = 5;
  else if( myMeshData->NbNodes() > 1000 )
    aScaler = 7;

  myCircles.SetCellSize( aDeltaX / aScaler,
                         aDeltaY / aScaler );

  mySupVert[0] = myMeshData->AddNode(
    BRepMesh_Vertex( ( aMinX + aMaxX ) / 2, aMaxY + aDeltaMax, BRepMesh_Free ) );
    
  mySupVert[1] = myMeshData->AddNode(
    BRepMesh_Vertex( aMinX - aDelta, aMinY - aDeltaMin, BRepMesh_Free ) );
    
  mySupVert[2] = myMeshData->AddNode(
    BRepMesh_Vertex( aMaxX + aDelta, aMinY - aDeltaMin, BRepMesh_Free ) );

  if ( !myIsPositiveOrientation )
  {
    Standard_Integer aTmp;
    aTmp         = mySupVert[1];
    mySupVert[1] = mySupVert[2];
    mySupVert[2] = aTmp;
  }

  Standard_Integer anEdgeId[3];
  
  for (Standard_Integer aNodeId = 0; aNodeId < 3; ++aNodeId)
  {
    Standard_Integer aFirstNode = aNodeId;
    Standard_Integer aLastNode  = (aNodeId + 1) % 3;
    anEdgeId[aNodeId] = myMeshData->AddLink( BRepMesh_Edge( 
      mySupVert[aFirstNode], mySupVert[aLastNode], BRepMesh_Free ) );
  }
  
  mySupTrian = BRepMesh_Triangle( 
    Abs( anEdgeId[0] ),  Abs( anEdgeId[1] ),  Abs( anEdgeId[2] ), 
    ( anEdgeId[0] > 0 ), ( anEdgeId[1] > 0 ), ( anEdgeId[2] > 0),
    BRepMesh_Free);
}

//=======================================================================
//function : DeleteTriangle
//purpose  : Deletes the triangle with the given index and adds the free
//           edges into the map.
//           When an edge is suppressed more than one time it is destroyed.
//=======================================================================
void  BRepMesh_Delaun::DeleteTriangle( const Standard_Integer theIndex, 
                                       BRepMesh_MapOfIntegerInteger& theLoopEdges )
{
  myCircles.Delete( theIndex );

  Standard_Integer e[3];
  Standard_Boolean o[3];
  GetTriangle( theIndex ).Edges( e[0], e[1], e[2],
                                 o[0], o[1], o[2] );
  
  myMeshData->RemoveElement( theIndex );

  for ( Standard_Integer i = 0; i < 3; ++i )
  {
    if ( !theLoopEdges.Bind( e[i], o[i] ) ) 
    {
      theLoopEdges.UnBind( e[i] );
      myMeshData->RemoveLink( e[i] );
    }
  }
}

//=======================================================================
//function : Compute
//purpose  : Computes the triangulation and add the vertices edges and 
//           triangles to the Mesh data structure
//=======================================================================
void BRepMesh_Delaun::Compute( TColStd_Array1OfInteger& theVertexIndexes )
{
  // Insertion of edges of super triangles in the list of free edges: 
  BRepMesh_MapOfIntegerInteger aLoopEdges( 10, myMeshData->Allocator() );
  Standard_Integer e[3];
  Standard_Boolean o[3];
  mySupTrian.Edges( e[0], e[1], e[2],
                    o[0], o[1], o[2] );
                    
  aLoopEdges.Bind( e[0], Standard_True );
  aLoopEdges.Bind( e[1], Standard_True );
  aLoopEdges.Bind( e[2], Standard_True );

  if ( theVertexIndexes.Length() > 0 )
  {
    // Creation of 3 trianglers with the first node and the edges of the super triangle:
    Standard_Integer anVertexIdx = theVertexIndexes.Lower();
    CreateTriangles( theVertexIndexes( anVertexIdx ), aLoopEdges );

    // Add other nodes to the mesh
    CreateTrianglesOnNewVertices( theVertexIndexes, Standard_True );
  }

  // Destruction of triangles containing a top of the super triangle
  BRepMesh_SelectorOfDataStructureOfDelaun aSelector( myMeshData );
  for (Standard_Integer aSupVertId = 0; aSupVertId < 3; ++aSupVertId)
    aSelector.NeighboursOfNode( mySupVert[aSupVertId] );
  
  aLoopEdges.Clear();
  BRepMesh_MapOfInteger::Iterator aFreeTriangles( aSelector.Elements() );
  for ( ; aFreeTriangles.More(); aFreeTriangles.Next() )
    DeleteTriangle( aFreeTriangles.Key(), aLoopEdges );

  // All edges that remain free are removed from aLoopEdges;
  // only the boundary edges of the triangulation remain there
  BRepMesh_MapOfIntegerInteger::Iterator aFreeEdges( aLoopEdges );
  for ( ; aFreeEdges.More(); aFreeEdges.Next() )
  {
    if ( myMeshData->ElemConnectedTo( aFreeEdges.Key() ).IsEmpty() )
      myMeshData->RemoveLink( aFreeEdges.Key() );
  }

  // The tops of the super triangle are destroyed
  for (Standard_Integer aSupVertId = 0; aSupVertId < 3; ++aSupVertId)
    myMeshData->RemoveNode( mySupVert[aSupVertId] );
}

//=======================================================================
//function : CreateTriangles
//purpose  : Creates the triangles beetween the node and the polyline.
//=======================================================================
void BRepMesh_Delaun::CreateTriangles ( const Standard_Integer theVertexIndex,  
                                        BRepMesh_MapOfIntegerInteger& thePoly )
{
  if (!myProgress.IsNull() && myProgress->UserBreak())
    return;

  ListOfInteger aLoopEdges, anExternalEdges;
  const gp_XY& aVertexCoord = myMeshData->GetNode( theVertexIndex ).Coord();
  
  BRepMesh_MapOfIntegerInteger::Iterator anEdges( thePoly );
  for ( ; anEdges.More(); anEdges.Next() )
  {
    Standard_Integer     anEdgeId = anEdges.Key();
    const BRepMesh_Edge& anEdge   = GetEdge( anEdgeId );

    Standard_Boolean isPositive = (Standard_Boolean)thePoly( anEdgeId );

    Standard_Integer aNodes[3];
    if ( isPositive )
    {
      aNodes[0] = anEdge.FirstNode();
      aNodes[2] = anEdge.LastNode();
    }
    else
    {
      aNodes[0] = anEdge.LastNode();
      aNodes[2] = anEdge.FirstNode();
    }
    aNodes[1] = theVertexIndex;

    const BRepMesh_Vertex& aFirstVertex = GetVertex( aNodes[0] );
    const BRepMesh_Vertex& aLastVertex  = GetVertex( aNodes[2] );

    gp_XY anEdgeDir( aLastVertex.Coord() - aFirstVertex.Coord() );
    Standard_Real anEdgeLen = anEdgeDir.Modulus();
    if ( anEdgeLen < Precision )
    {
      continue;
    }

    anEdgeDir.SetCoord( anEdgeDir.X() / anEdgeLen,
                        anEdgeDir.Y() / anEdgeLen );

    gp_XY aFirstLinkDir( aFirstVertex.Coord() - aVertexCoord );
    gp_XY aLastLinkDir ( aVertexCoord         - aLastVertex.Coord() );
                      
    Standard_Real aDist12 = aFirstLinkDir ^ anEdgeDir;
    Standard_Real aDist23 = anEdgeDir     ^ aLastLinkDir;
    if ( Abs( aDist12 ) < Precision || 
         Abs( aDist23 ) < Precision )
    {
      continue;
    }

    Standard_Boolean isSensOK;
    if ( myIsPositiveOrientation )
      isSensOK = ( aDist12 > 0. && aDist23 > 0.);
    else
      isSensOK = ( aDist12 < 0. && aDist23 < 0.);

    BRepMesh_Edge aFirstLink( aNodes[1], aNodes[0], BRepMesh_Free );
    BRepMesh_Edge aLastLink ( aNodes[2], aNodes[1], BRepMesh_Free );

    if ( isSensOK )
    {
      Standard_Integer anEdgesInfo[3] = {
          myMeshData->AddLink( aFirstLink ),
          isPositive ? anEdgeId : -anEdgeId,
          myMeshData->AddLink( aLastLink ) };

      Standard_Integer anEdges[3];
      Standard_Boolean anEdgesOri[3];
      for ( Standard_Integer aTriLinkIt = 0; aTriLinkIt < 3; ++aTriLinkIt )
      {
        const Standard_Integer& anEdgeInfo = anEdgesInfo[aTriLinkIt];
        anEdges[aTriLinkIt]    = Abs( anEdgeInfo );
        anEdgesOri[aTriLinkIt] = anEdgeInfo > 0;
      }

      AddTriangle( anEdges, anEdgesOri, aNodes );
    }
    else
    {
      aLoopEdges.Append( (isPositive)? anEdgeId : -1 * anEdgeId );
        
      if ( aFirstLinkDir.SquareModulus() > aLastLinkDir.SquareModulus() )
      {
        Standard_Integer aNewEdge = myMeshData->AddLink( aFirstLink );
        anExternalEdges.Append( Abs( aNewEdge ) );
      }
      else
      {
        Standard_Integer aNewEdge = myMeshData->AddLink( aLastLink );
        anExternalEdges.Append( Abs( aNewEdge ) );
      }
    }
  }
  
  thePoly.Clear();
  while ( !anExternalEdges.IsEmpty() )
  {
    const BRepMesh_PairOfIndex& aPair = 
      myMeshData->ElemConnectedTo( Abs( anExternalEdges.First() ) );
    
    if ( !aPair.IsEmpty() )
      DeleteTriangle( aPair.FirstIndex(), thePoly );
      
    anExternalEdges.RemoveFirst();
  }

  for ( anEdges.Initialize( thePoly ); anEdges.More(); anEdges.Next() )
  {
    if ( myMeshData->ElemConnectedTo( anEdges.Key() ).IsEmpty() )
      myMeshData->RemoveLink( anEdges.Key() );
  }

  while ( !aLoopEdges.IsEmpty() )
  {
    const BRepMesh_Edge& anEdge = GetEdge( Abs( aLoopEdges.First() ) );
    if ( anEdge.Movability() != BRepMesh_Deleted )
    {
      Standard_Integer anEdgeIdx = aLoopEdges.First();
      MeshLeftPolygonOf( Abs( anEdgeIdx ), ( anEdgeIdx > 0 ) );
    }
      
    aLoopEdges.RemoveFirst();
  }
}

//=======================================================================
//function : CreateTrianglesOnNewVertices
//purpose  : Creation of triangles from the new nodes
//=======================================================================
void BRepMesh_Delaun::CreateTrianglesOnNewVertices( TColStd_Array1OfInteger& theVertexIndexes, const Standard_Boolean adjustFrontier )
{
  BRepMesh_MapOfIntegerInteger aLoopEdges( 10, myMeshData->Allocator() );

  // Insertion of nodes :
  Standard_Boolean isModify = Standard_True;
  
  Standard_Integer anIndex = theVertexIndexes.Lower();
  Standard_Integer anUpper = theVertexIndexes.Upper();
  for( ; anIndex <= anUpper; ++anIndex ) 
  {
    aLoopEdges.Clear();

    if (!myProgress.IsNull() && myProgress->UserBreak())
      break;
    
    Standard_Integer aVertexIdx = theVertexIndexes( anIndex );    
    const BRepMesh_Vertex& aVertex = GetVertex( aVertexIdx );

    // Iterator in the list of indexes of circles containing the node
    BRepMesh_ListOfInteger& aCirclesList = myCircles.Select( aVertex.Coord() );
    
    Standard_Integer onEgdeId = 0, aTriangleId = 0;
    BRepMesh_ListOfInteger::Iterator aCircleIt( aCirclesList );
    for ( ; aCircleIt.More(); aCircleIt.Next() )
    {
      // To add a node in the mesh it is necessary to check conditions: 
      // - the node should be within the boundaries of the mesh and so in an existing triangle
      // - all adjacent triangles should belong to a component connected with this triangle
      if ( Contains( aCircleIt.Value(), aVertex, onEgdeId ) )
      {
        if ( onEgdeId == 0 )
        {
          aTriangleId = aCircleIt.Value();
          aCirclesList.Remove( aCircleIt );
          break;
        }
        else if ( GetEdge( onEgdeId ).Movability() == BRepMesh_Free )
        {
          aTriangleId = aCircleIt.Value();
          aCirclesList.Remove( aCircleIt );
          break;
        }
      }
    }

    if ( aTriangleId > 0 )
    {
      DeleteTriangle( aTriangleId, aLoopEdges );

      isModify = Standard_True;    
      while ( isModify && !aCirclesList.IsEmpty() )
      {
        isModify = Standard_False;
        BRepMesh_ListOfInteger::Iterator aCircleIt1( aCirclesList );
        for ( ; aCircleIt1.More(); aCircleIt1.Next() )
        {
          Standard_Integer e[3];
          Standard_Boolean o[3];
          GetTriangle( aCircleIt1.Value() ).Edges( e[0], e[1], e[2],
                                                   o[0], o[1], o[2] );
                                                   
          if ( aLoopEdges.IsBound( e[0] ) || 
               aLoopEdges.IsBound( e[1] ) || 
               aLoopEdges.IsBound( e[2] ) )
          {
            isModify = Standard_True;
            DeleteTriangle( aCircleIt1.Value(), aLoopEdges );
            aCirclesList.Remove( aCircleIt1 );
            break;
          }
        }
      }

      // Creation of triangles with the current node and free edges
      // and removal of these edges from the list of free edges
      CreateTriangles( aVertexIdx, aLoopEdges );
    }
  }

  {
    // Check that internal edges are not crossed by triangles
    Handle(BRepMesh_MapOfInteger) anInternalEdges = InternalEdges();

    // Destruction of triancles intersecting internal edges 
    // and their replacement by makeshift triangles
    BRepMesh_MapOfInteger::Iterator anInternalEdgesIt( *anInternalEdges );
    for ( ; anInternalEdgesIt.More(); anInternalEdgesIt.Next() )
    {
      Standard_Integer aNbC;
      aNbC = myMeshData->ElemConnectedTo( anInternalEdgesIt.Key() ).Extent();
      if ( aNbC == 0 )
      {
        MeshLeftPolygonOf( anInternalEdgesIt.Key(), Standard_True  ); 
        MeshLeftPolygonOf( anInternalEdgesIt.Key(), Standard_False ); 
      }
    }
  }

  if (adjustFrontier)
  {
    // Adjustment of meshes to boundary edges
    FrontierAdjust();
  }
}

//=======================================================================
//function : CleanupMesh
//purpose  : Cleanup mesh from the free triangles
//=======================================================================
void BRepMesh_Delaun::CleanupMesh()
{
  BRepMesh_MapOfIntegerInteger aLoopEdges( 10, myMeshData->Allocator() );
  ListOfInteger aTrianglesList;

  for(;;)
  {
    aTrianglesList.Clear();
    aLoopEdges.Clear();

    if (!myProgress.IsNull() && myProgress->UserBreak())
      break;

    Handle(BRepMesh_MapOfInteger) aFreeEdges = FreeEdges();
    BRepMesh_MapOfInteger::Iterator aFreeEdgesIt( *aFreeEdges );
    for ( ; aFreeEdgesIt.More(); aFreeEdgesIt.Next() )
    {
      const BRepMesh_Edge& anEdge = GetEdge( aFreeEdgesIt.Key() );
      if ( anEdge.Movability() != BRepMesh_Frontier )
      {
        Standard_Integer aFrontierNb = 0;
        if ( myMeshData->ElemConnectedTo( aFreeEdgesIt.Key() ).IsEmpty() ) 
          aLoopEdges.Bind( aFreeEdgesIt.Key(), Standard_True );
        else
        {
          BRepMesh_ListOfInteger::Iterator aConnectedLinksIt( 
            myMeshData->LinkNeighboursOf( anEdge.FirstNode() ) );

          for ( ; aConnectedLinksIt.More(); aConnectedLinksIt.Next() )
          {
            if ( GetEdge( aConnectedLinksIt.Value() ).Movability() == BRepMesh_Frontier )
            {
              aFrontierNb++;
              if ( aFrontierNb > 1 )
                break;
            }
          }

          if ( aFrontierNb == 2 )
          {
            const BRepMesh_PairOfIndex& aPair = myMeshData->ElemConnectedTo( aFreeEdgesIt.Key() );
            for ( Standard_Integer j = 1, jn = aPair.Extent(); j <= jn; ++j )
            {
              const Standard_Integer anElemId = aPair.Index(j);
              if ( anElemId < 0 )
                continue;

              Standard_Integer e[3];
              Standard_Boolean o[3];
              GetTriangle( anElemId ).Edges( e[0], e[1], e[2],
                                             o[0], o[1], o[2] );

              Standard_Boolean isTriangleToDelete = Standard_True;
              for ( Standard_Integer k = 0; k < 3; ++k )
              {
                if ( GetEdge( e[k] ).Movability() != BRepMesh_Free )
                {
                  isTriangleToDelete = Standard_False;
                  break;
                }
              }

              if ( isTriangleToDelete )
                aTrianglesList.Append( anElemId );
            }
          }
        }
      }
    }

    // Destruction of triangles :
    Standard_Integer aDeletedTrianglesNb = 0;
    for ( ; !aTrianglesList.IsEmpty(); aTrianglesList.RemoveFirst() )
    {
      DeleteTriangle( aTrianglesList.First(), aLoopEdges );
      aDeletedTrianglesNb++;
    }

    // Destruction of remaining hanging edges
    BRepMesh_MapOfIntegerInteger::Iterator aLoopEdgesIt( aLoopEdges );
    for ( ; aLoopEdgesIt.More(); aLoopEdgesIt.Next() )
    {
      if ( myMeshData->ElemConnectedTo( aLoopEdgesIt.Key() ).IsEmpty() )
        myMeshData->RemoveLink( aLoopEdgesIt.Key() );
    }

    if ( aDeletedTrianglesNb == 0 )
      break;
  }
}

//=======================================================================
//function : FrontierAdjust
//purpose  : Adjust the mesh on the frontier
//=======================================================================
void BRepMesh_Delaun::FrontierAdjust()
{
  if (!myProgress.IsNull() && myProgress->UserBreak())
    return;

  BRepMesh_MapOfIntegerInteger aLoopEdges( 10, myMeshData->Allocator() );
  TColStd_MapOfInteger aTrianglesList;
  Standard_Boolean isModified = Standard_True;

  NCollection_DataMap<Standard_Integer, Bnd_Box2d> aFrontBoxes;
  Standard_Integer aPass = 1;

  TColStd_SequenceOfInteger aProcessSequence;

  for ( ; isModified; ++aPass )
  {
    isModified = Standard_False;

    if (!myProgress.IsNull() && myProgress->UserBreak())
      break;

    // Find all external triangles (bounded to frontier with negative orientation)
    Handle(BRepMesh_MapOfInteger) aFrontier = Frontier();
    BRepMesh_MapOfInteger::Iterator aFrontierIt( *aFrontier );
    for ( ; aFrontierIt.More(); aFrontierIt.Next() )
    {
      const BRepMesh_PairOfIndex& aPair = myMeshData->ElemConnectedTo( aFrontierIt.Key() );
      for( Standard_Integer j = 1, jn = aPair.Extent(); j <= jn; ++j )
      {
        const Standard_Integer aPriorElemId = aPair.Index(j);
        if( aPriorElemId < 0 )
          continue;
        
        Standard_Integer e[3];
        Standard_Boolean o[3];
        GetTriangle( aPriorElemId ).Edges( e[0], e[1], e[2],
                                           o[0], o[1], o[2] );

        for ( Standard_Integer n = 0; n < 3; ++n )
        {
          if ( aFrontierIt.Key() == e[n] && !o[n] )
          {
            if ( !aTrianglesList.Contains(aPriorElemId) )
              aTrianglesList.Add( aPriorElemId );
            break;
          }
        }
      }
    }

    // destruction  of external triangles on boundary edges
    TColStd_MapIteratorOfMapOfInteger anTriangleIt( aTrianglesList );
    for ( ; anTriangleIt.More(); anTriangleIt.Next() )
      DeleteTriangle( anTriangleIt.Key(), aLoopEdges );
    aTrianglesList.Clear();

    { // destrucrion of remaining hanging edges :
      BRepMesh_MapOfIntegerInteger::Iterator aLoopEdgesIt( aLoopEdges );
      for ( ; aLoopEdgesIt.More(); aLoopEdgesIt.Next() )
      {
        if (myMeshData->ElemConnectedTo( aLoopEdgesIt.Key() ).IsEmpty() )
          myMeshData->RemoveLink( aLoopEdgesIt.Key() );
      }
      aLoopEdges.Clear();
    }

    if (!myProgress.IsNull() && myProgress->UserBreak())
      break;

    CleanupMesh();

    // Find the triangles crossing the frontier edges
    BRepMesh_MapOfInteger::Iterator aTriangleIt( myMeshData->ElemOfDomain() );
    for ( ; aTriangleIt.More(); aTriangleIt.Next() )
    {
      if (!myProgress.IsNull() && myProgress->UserBreak())
        break;

      Standard_Integer tid = aTriangleIt.Key();

      Standard_Integer e[3];
      Standard_Boolean o[3];
      
      GetTriangle(tid).Edges(e[0], e[1], e[2], o[0], o[1], o[2]);
      const BRepMesh_Edge& anTEdge0 = GetEdge( e[0] );
      const BRepMesh_Edge& anTEdge1 = GetEdge( e[1] );
      const BRepMesh_Edge& anTEdge2 = GetEdge( e[2] );

      Bnd_Box2d aTriangleBox;
      Standard_Integer v[3];

      v[0] = anTEdge0.FirstNode();
      v[1] = anTEdge0.LastNode();
      if (v[0] == anTEdge1.FirstNode() || v[1] == anTEdge1.FirstNode())
        v[2] = anTEdge1.LastNode();
      else
        v[2] = anTEdge1.FirstNode();

      BRepMesh_Vertex bV[3];

      for (Standard_Integer j = 0; j < 3; ++j)
      {
        bV[j] = GetVertex(v[j]);
        aTriangleBox.Add( (gp_Pnt2d)bV[j].Coord () );
      }

      aFrontierIt.Reset();
      for ( ; !isModified && aFrontierIt.More(); aFrontierIt.Next() )
      {
        if (!myProgress.IsNull() && myProgress->UserBreak())
          break;

        if (aFrontierIt.Key() == e[0] || aFrontierIt.Key() == e[1] || aFrontierIt.Key() == e[2])
          continue;

        if (aFrontBoxes.IsBound(aFrontierIt.Key()) && aTriangleBox.IsOut(aFrontBoxes.Find(aFrontierIt.Key())))
          continue;

        const BRepMesh_Edge& anEdge = GetEdge( aFrontierIt.Key() );
        const BRepMesh_Vertex& aV1  = GetVertex( anEdge.FirstNode() );
        const BRepMesh_Vertex& aV2  = GetVertex( anEdge.LastNode()  );

        if (!aFrontBoxes.IsBound(aFrontierIt.Key()))
        {
          Bnd_Box2d anEdgeBox;
          anEdgeBox.Add((gp_Pnt2d)aV1.Coord());
          anEdgeBox.Add((gp_Pnt2d)aV2.Coord());
          aFrontBoxes.Bind(aFrontierIt.Key(), anEdgeBox);
          if (aTriangleBox.IsOut(anEdgeBox))
            continue;
        }

        Standard_Integer aOnEdge = 0;

        if ( ( Contains(tid, aV1, aOnEdge) && !( (aV1.IsEqual(bV[0]) || aV1.IsEqual(bV[1]) || aV1.IsEqual(bV[2])) ) ) ||
             ( Contains(tid, aV2, aOnEdge) && !( (aV2.IsEqual(bV[0]) || aV2.IsEqual(bV[1]) || aV2.IsEqual(bV[2])) ) ) ||
             ( IntSegSeg(anEdge, anTEdge0) || IntSegSeg(anEdge, anTEdge1) || IntSegSeg(anEdge, anTEdge2) )
           )
        {
          if ( !myMeshData->ElemConnectedTo( aFrontierIt.Key() ).IsEmpty() )
          {
            DeleteTriangle( myMeshData->ElemConnectedTo( aFrontierIt.Key() ).Index( 1 ), aLoopEdges );
            if ( !myMeshData->ElemConnectedTo( aFrontierIt.Key() ).IsEmpty() )
              DeleteTriangle( myMeshData->ElemConnectedTo( aFrontierIt.Key() ).Index( 1 ), aLoopEdges );
          }
          DeleteTriangle( tid, aLoopEdges );

          aProcessSequence.Append(aFrontierIt.Key());
          break;
        }
      }

    }

    { // destrucrion of remaining hanging edges :
      BRepMesh_MapOfIntegerInteger::Iterator aLoopEdgesIt( aLoopEdges );
      for ( ; aLoopEdgesIt.More(); aLoopEdgesIt.Next() )
      {
        if (myMeshData->ElemConnectedTo( aLoopEdgesIt.Key() ).IsEmpty() )
          myMeshData->RemoveLink( aLoopEdgesIt.Key() );
      }
      aLoopEdges.Clear();
    }

    if (!myProgress.IsNull() && myProgress->UserBreak())
      break;

    aFrontierIt.Reset ();
    for ( ; aFrontierIt.More(); aFrontierIt.Next() )
    {
      const BRepMesh_Edge& anEdge = GetEdge( aFrontierIt.Key() );

      if ( myMeshData->ElemConnectedTo( aFrontierIt.Key() ).IsEmpty() && (anEdge.FirstNode() != anEdge.LastNode())) // we fix mesh only for non-zero edges
      {
        MeshLeftPolygonOf( aFrontierIt.Key(), Standard_True ); 
        isModified = Standard_True;
      }
    }

    if (aPass > 2 && isModified && !aProcessSequence.IsEmpty())
    {
      TColStd_SequenceOfInteger loop;
      Standard_Integer last = aProcessSequence.Last();
      loop.Append(last);
      Standard_Integer i = aProcessSequence.Length() - 1;
      for ( ; i > 0; --i )
      {
        Standard_Integer edge = aProcessSequence(i);
        if (edge == last)
        {
          Standard_Integer j = 2;
          Standard_Integer k = i - 1;
          for ( ; j <= loop.Length() && k > 0; ++j, --k )
          {
            if (loop(j) != aProcessSequence(k))
              break;
          }

          if (j > loop.Length()) // we found a loop in our processing sequence, this means, that the face isn't consistent, for example it has self-intersecting wires
          {
            return;
          }
        }
        loop.Append(edge);
      }
    }

    if ( aPass > 3 ) {
      isModified = Standard_False;
    }
  }

  CleanupMesh();
}

//=======================================================================
//function : CleanupPolygon
//purpose  : Remove internal triangles from the given polygon
//=======================================================================
void BRepMesh_Delaun::CleanupPolygon( const TColStd_SequenceOfInteger& thePolygon, const TColStd_MapOfInteger & theUsedEdges )
{
  if (!myProgress.IsNull() && myProgress->UserBreak())
    return;

  Standard_Integer aPolyLen = thePolygon.Length();

  if (aPolyLen < 3) // nothing to cleanup in empty polygons
    return;

  TColStd_SequenceOfInteger aTrianglesQueue;
  TColStd_MapOfInteger aDeletedTriangles;
  BRepMesh_MapOfIntegerInteger theLoopEdges( 10, myMeshData->Allocator() );
  //TColStd_MapOfInteger theInfectedEdges;

  Standard_Integer aPivotNode;
  Standard_Integer e[3];
  Standard_Boolean o[3];
  Standard_Real aRefTotalAngle = 2 * M_PI;

  for ( Standard_Integer i = 1; i <= aPolyLen; ++i )
  {
    if (!myProgress.IsNull() && myProgress->UserBreak())
      break;

    Standard_Integer aCurrPolyEdgeId = thePolygon(i);
    Standard_Integer aNextPolyEdgeId = thePolygon( i % aPolyLen + 1 );
    Standard_Boolean anIsForward = aCurrPolyEdgeId > 0;

    {
      const BRepMesh_PairOfIndex& aPair = myMeshData->ElemConnectedTo( Abs( aCurrPolyEdgeId ) );
      for ( Standard_Integer j = 1, jn = aPair.Extent(); j <= jn; ++j )
      {
        Standard_Integer anElemId = aPair.Index(j);
        if( anElemId < 0 )
          continue;

        GetTriangle( anElemId ).Edges( e[0], e[1], e[2],
          o[0], o[1], o[2] );

        for ( Standard_Integer anIndex = 0; anIndex < 3; ++anIndex )
        {
          if ( e[anIndex] == Abs ( aCurrPolyEdgeId ) && o[anIndex] == anIsForward )
          {
            // triangle detected
            aTrianglesQueue.Append( anElemId );
            break;
          }
        }

        if ( !aTrianglesQueue.IsEmpty() )
          break;
      }
    }

    const BRepMesh_Edge & aCurrEdge = GetEdge( Abs(aCurrPolyEdgeId) );
    const BRepMesh_Edge & aNextEdge = GetEdge( Abs(aNextPolyEdgeId) );

    gp_XY aCurrVEdge;
    gp_XY aNextVEdge;

    {
      if (aCurrPolyEdgeId > 0)
      {
        aCurrVEdge = GetVertex( aCurrEdge.LastNode() ).Coord();
        aCurrVEdge.Subtract( GetVertex( aCurrEdge.FirstNode() ).Coord() );
        aPivotNode = aCurrEdge.LastNode();
      }
      else
      {
        aCurrVEdge = GetVertex( aCurrEdge.FirstNode() ).Coord();
        aCurrVEdge.Subtract( GetVertex( aCurrEdge.LastNode() ).Coord() );
        aPivotNode = aCurrEdge.FirstNode();
      }
    }

    {
      if (aNextPolyEdgeId > 0)
      {
        aNextVEdge = GetVertex( aNextEdge.LastNode() ).Coord();
        aNextVEdge.Subtract( GetVertex( aNextEdge.FirstNode() ).Coord() );
      }
      else
      {
        aNextVEdge = GetVertex( aNextEdge.FirstNode() ).Coord();
        aNextVEdge.Subtract( GetVertex( aNextEdge.LastNode() ).Coord() );
      }
    }

    Standard_Real aRefAngle = gp_Vec2d( aCurrVEdge ).Angle( gp_Vec2d( aNextVEdge ) );

    BRepMesh_ListOfInteger::Iterator aLinkIt;
    aLinkIt.Init( myMeshData->LinkNeighboursOf( aPivotNode ) );
    for ( ; aLinkIt.More(); aLinkIt.Next() )
    {
      Standard_Integer aNextLink = aLinkIt.Value();
      Standard_Integer aNextLinkId = Abs( aNextLink );

      if ( aNextLinkId == Abs(aCurrPolyEdgeId) || aNextLinkId == Abs(aNextPolyEdgeId) )
        continue;

      const BRepMesh_Edge& aNextEdge = GetEdge( aNextLinkId );

      // if link isn't in mesh, we skip it
      if ( aNextEdge.Movability() == BRepMesh_Free && myMeshData->ElemConnectedTo( aLinkIt.Value() ).IsEmpty() )
        continue;

      Standard_Integer anOtherNode = aNextEdge.FirstNode();
      if ( anOtherNode == aPivotNode )
        anOtherNode = aNextEdge.LastNode();
      if ( anOtherNode == aPivotNode )
        continue;

      gp_XY aTmpVEdge = GetVertex( anOtherNode ).Coord();
      aTmpVEdge.Subtract( GetVertex( aPivotNode ).Coord() );

      Standard_Real anAngle = gp_Vec2d( aCurrVEdge ).Angle( gp_Vec2d( aTmpVEdge ) );

      if  ( ( myIsPositiveOrientation && anAngle > aRefAngle ) ||
        ( !myIsPositiveOrientation && anAngle < aRefAngle ) )
      {
        const BRepMesh_PairOfIndex& aPair = myMeshData->ElemConnectedTo( aNextLinkId );
        for ( Standard_Integer j = 1, jn = aPair.Extent(); j <= jn; ++j )
        {
          Standard_Integer anElemId = aPair.Index(j);
          if( anElemId < 0 )
            continue;

          aTrianglesQueue.Append( anElemId );
        }
        //theInfectedEdges.Add( aNextLinkId );
      }
    }

    while ( !aTrianglesQueue.IsEmpty() )
    {
      Standard_Integer aTriangle = aTrianglesQueue.First();
      if ( aDeletedTriangles.Contains(aTriangle) )
      {
        aTrianglesQueue.Remove( 1 );
        continue;
      }

      GetTriangle( aTriangle ).Edges( e[0], e[1], e[2],
                                      o[0], o[1], o[2] );

      for ( Standard_Integer anIndex = 0; anIndex < 3; ++anIndex )
      {
        if ( !theUsedEdges.Contains(e[anIndex]) && GetEdge(e[anIndex]).Movability() != BRepMesh_Frontier)
        {
          Standard_Integer aTmpEdgeId = e[anIndex];

          Standard_Boolean doesIntersects = Standard_False;
          Standard_Real aFirstTotalAngle = .0;
          Standard_Real aLastTotalAngle = .0;

          {
            const BRepMesh_Edge& anInfectedEdge = GetEdge( aTmpEdgeId );
            gp_XY aFirstPointXY = GetVertex( anInfectedEdge.FirstNode() ).Coord();
            gp_XY aLastPointXY = GetVertex( anInfectedEdge.LastNode() ).Coord();

            for ( Standard_Integer i = 1; i <= aPolyLen; ++i )
            {
              Standard_Integer aPolyEdgeId = thePolygon(i);
              const BRepMesh_Edge& aPolyEdge = GetEdge( Abs( aPolyEdgeId ) );

              if (IntSegSeg(anInfectedEdge, aPolyEdge, Standard_False))
              {
                doesIntersects = Standard_True;
                break;
              }

              Standard_Integer aStartPoint, anEndPoint;
              if ( aPolyEdgeId >= 0 )
              {
                aStartPoint = aPolyEdge.FirstNode();
                anEndPoint  = aPolyEdge.LastNode();
              }
              else
              {
                aStartPoint = aPolyEdge.LastNode();
                anEndPoint  = aPolyEdge.FirstNode();
              }

              {
                gp_XY aStartV = GetVertex( aStartPoint ).Coord() - aFirstPointXY;
                gp_XY anEndV  = GetVertex( anEndPoint ).Coord()  - aFirstPointXY;

                if (aStartV.Modulus() >= gp::Resolution() && anEndV.Modulus() >= gp::Resolution())
                {
                  Standard_Real anAngle = gp_Vec2d( anEndV ).Angle( gp_Vec2d( aStartV ) );
                  aFirstTotalAngle += anAngle;
                }
              }

              {
                gp_XY aStartV = GetVertex( aStartPoint ).Coord() - aLastPointXY;
                gp_XY anEndV  = GetVertex( anEndPoint ).Coord()  - aLastPointXY;

                if (aStartV.Modulus() >= gp::Resolution() && anEndV.Modulus() >= gp::Resolution())
                {
                  Standard_Real anAngle = gp_Vec2d( anEndV ).Angle( gp_Vec2d( aStartV ) );
                  aLastTotalAngle += anAngle;
                }
              }
            }
          }

          // true in case of outer edge
          if (!doesIntersects && ( Abs( aRefTotalAngle - aFirstTotalAngle ) > Precision::Angular() || Abs( aRefTotalAngle - aLastTotalAngle ) > Precision::Angular() ) )
            continue;

          const BRepMesh_PairOfIndex& aPair = myMeshData->ElemConnectedTo( aTmpEdgeId );
          for ( Standard_Integer j = 1, jn = aPair.Extent(); j <= jn; ++j)
          {
            Standard_Integer anElemId = aPair.Index(j);
            if( anElemId < 0 || anElemId == aTriangle)
              continue;

            if ( !doesIntersects )
            {
              aTrianglesQueue.Append( anElemId );
            }
            else
            {
              DeleteTriangle(anElemId, theLoopEdges);
              aDeletedTriangles.Add( anElemId );
              --j;
              jn = aPair.Extent();
            }
          }

        }
      }

      DeleteTriangle(aTriangle, theLoopEdges);
      aDeletedTriangles.Add( aTriangle );
      aTrianglesQueue.Remove( 1 );
    }
  }

  BRepMesh_MapOfIntegerInteger::Iterator aLoopEdgesIt( theLoopEdges );
  for ( ; aLoopEdgesIt.More(); aLoopEdgesIt.Next() )
  {
    if ( myMeshData->ElemConnectedTo( aLoopEdgesIt.Key() ).IsEmpty() )
      myMeshData->RemoveLink( aLoopEdgesIt.Key() );
  }
}

//=======================================================================
//function : MeshLeftPolygonOf
//purpose  : Collect the polygon at the left of the given edge (material side)
//=======================================================================
void BRepMesh_Delaun::MeshLeftPolygonOf
(
  const Standard_Integer theEdgeIndex,
  const Standard_Boolean isForward
)
{
  if (!myProgress.IsNull() && myProgress->UserBreak())
    return;

  const BRepMesh_Edge& anEdge = GetEdge( theEdgeIndex );
  NCollection_Map<Standard_Integer> aDealLinks;
  TColStd_SequenceOfInteger aPolygon;
  TColStd_MapOfInteger anUsedEdges;

  BRepMesh_MapOfIntegerInteger aLoopEdges( 10, myMeshData->Allocator() );
  TColStd_MapOfInteger anInfectedEdges;

  // Find the aPolygonon
  anUsedEdges.Add( theEdgeIndex );
  Standard_Integer aFirstNode, aStartNode, aPivotNode, aLastNode;

  if ( isForward )
  {
    aPolygon.Append( theEdgeIndex );
    aStartNode = anEdge.FirstNode();
    aPivotNode = anEdge.LastNode();
  }
  else
  {
    aPolygon.Append( -theEdgeIndex );
    aStartNode = anEdge.LastNode();
    aPivotNode = anEdge.FirstNode();
  }
  aFirstNode = aStartNode;
  aLastNode = aPivotNode;

  const BRepMesh_Vertex& aStartVertex = GetVertex( aFirstNode );
  const BRepMesh_Vertex& anEndVertex  = GetVertex( aPivotNode );

  Standard_Integer aRefOtherNode = 0, anOtherNode = 0;
  // Check the presence of the previous edge <theEdgeIndex> :
  BRepMesh_ListOfInteger::Iterator aLinkIt( myMeshData->LinkNeighboursOf( aStartNode ) );
  for ( ; aLinkIt.More(); aLinkIt.Next() )
  {
    if ( aLinkIt.Value() != theEdgeIndex )
    {
      const BRepMesh_Edge& aNextEdge = GetEdge( aLinkIt.Value() );
      anOtherNode = aNextEdge.LastNode();
      if ( anOtherNode == aStartNode )
        anOtherNode = aNextEdge.FirstNode();
      break;
    }
  }
  if ( anOtherNode == 0 )
    return;

  gp_XY aVEdge( anEndVertex.Coord() );
  aVEdge.Subtract( aStartVertex.Coord() );
  gp_XY aPrevVEdge( aVEdge );
  gp_XY aRefCurrVEdge, aCurrVEdge;

  Standard_Integer aCurrEdgeId = theEdgeIndex;
  Standard_Integer aNextEdgeId;

  // Find the nearest to <theEdgeIndex> closed aPolygonon :
  Standard_Boolean isNotInters;
  Standard_Real anAngle, aRefAngle;

  NCollection_Sequence <Bnd_Box2d> aBoxes;
  fillBndBox( aBoxes, aStartVertex, anEndVertex );

  while ( aPivotNode != aFirstNode )
  {
    aNextEdgeId = 0;
    if ( myIsPositiveOrientation )
      aRefAngle = RealFirst();
    else
      aRefAngle = RealLast();

    const BRepMesh_Vertex& aPivotVertex = GetVertex( aPivotNode );

    // Find the next edge with the greatest angle with <theEdgeIndex>
    // and non intersecting <theEdgeIndex> :

    aLinkIt.Init( myMeshData->LinkNeighboursOf( aPivotNode ) );
    for ( ; aLinkIt.More(); aLinkIt.Next() )
    {
      Standard_Integer aNextLink = aLinkIt.Value();
      Standard_Integer aNextLinkId = Abs( aNextLink );
      if ( aDealLinks.Contains( aNextLinkId ) )
        continue;

      if ( aNextLinkId == aCurrEdgeId )
        continue;

      isNotInters = Standard_True;
      const BRepMesh_Edge& aNextEdge = GetEdge( aNextLinkId );

      // if link isn't in mesh, we skip it
      if ( aNextEdge.Movability() == BRepMesh_Free && myMeshData->ElemConnectedTo( aLinkIt.Value() ).IsEmpty() )
        continue;

      anOtherNode = aNextEdge.FirstNode();
      if ( anOtherNode == aPivotNode )
        anOtherNode = aNextEdge.LastNode();

      aCurrVEdge = GetVertex( anOtherNode ).Coord();
      aCurrVEdge.Subtract( aPivotVertex.Coord() );

      // skip small edges
      if ( aCurrVEdge.Modulus() < gp::Resolution() ||
           aPrevVEdge.Modulus() < gp::Resolution() )
        continue;

      anAngle = gp_Vec2d( aPrevVEdge ).Angle( gp_Vec2d( aCurrVEdge ) );

      if ( ( myIsPositiveOrientation && anAngle > aRefAngle ) ||
          ( !myIsPositiveOrientation && anAngle < aRefAngle ) )
      {
        Bnd_Box2d aBox;
        aBox.Add( gp_Pnt2d( GetVertex( aNextEdge.FirstNode() ).Coord() ) );
        aBox.Add( gp_Pnt2d( GetVertex( aNextEdge.LastNode()  ).Coord() ) );

        // checking if the last segment of polygon is reasonable
        // we check all edges except for the last, because it can't intersect with the last
        for ( Standard_Integer k = 1, aLen = aPolygon.Length(); k <= aLen; ++k )
        {
          if ( !aBox.IsOut( aBoxes.Value( k ) ) )
          {
            // intersection is possible... we should check the ends as well in order to avoid cycles
            // but we should keep in mind the end edges, which close the polygon
            const BRepMesh_Edge& aCompare = GetEdge( Abs( aPolygon( k ) ) );
            if ( ( IntSegSeg( aNextEdge, aCompare ) || ( anOtherNode == aCompare.FirstNode() ) || ( anOtherNode == aCompare.LastNode() ) )
              && (k != 1 || anOtherNode != aFirstNode )
              && (k != aLen || anOtherNode != aStartNode )
              )
            {
              // intersection found, we can't take this edge
              isNotInters = Standard_False;
              break;
            }
          }
        }

        if( isNotInters )
        {              
          aRefAngle = anAngle;
          aRefCurrVEdge = aCurrVEdge;

          if ( aNextEdge.FirstNode() == aPivotNode )
            aNextEdgeId =  aLinkIt.Value();
          else
            aNextEdgeId = -aLinkIt.Value();

          aRefOtherNode = anOtherNode;
        }
      }
    }

    // have we found some relevant edge?
    if ( aNextEdgeId != 0 )
    {
      if ( Abs( aNextEdgeId ) != theEdgeIndex && Abs( aNextEdgeId ) != aCurrEdgeId )
      {
        if ( !anUsedEdges.Add( Abs( aNextEdgeId ) ) )
        {
          //if this edge has already been added to the aPolygon, 
          //there is a risk of looping (attention to open contours)
          //remove last edge and continue
          aDealLinks.Add( Abs( aNextEdgeId ) );

          //roll back
          continue;
        }

        aCurrEdgeId = Abs( aNextEdgeId );
        aPolygon.Append( aNextEdgeId );

        const BRepMesh_Edge& aCurrEdge = GetEdge( aCurrEdgeId );
        Standard_Integer aVert1 = aCurrEdge.FirstNode();
        Standard_Integer aVert2 = aCurrEdge.LastNode();
        fillBndBox( aBoxes, GetVertex( aVert1 ), GetVertex( aVert2 ), aPolygon.Length() );
      }
    }
    else
    {
      //hanging end
      if ( aPolygon.Length() == 1 )
        return;

      Standard_Integer aDeadEdgeId = Abs( aPolygon.Last() );
      aDealLinks.Add( aDeadEdgeId );

      anUsedEdges.Remove( aDeadEdgeId );
      aPolygon.Remove( aPolygon.Length() );

      // return to previous point
      Standard_Integer aLastValidEdge = aPolygon.Last();
      const BRepMesh_Edge& aLastEdge = GetEdge( Abs( aLastValidEdge ) );

      if( aLastValidEdge > 0 )
      {
        aStartNode = aLastEdge.FirstNode();
        aPivotNode = aLastEdge.LastNode();
      }
      else
      {
        aStartNode = aLastEdge.LastNode();
        aPivotNode = aLastEdge.FirstNode();
      }

      const BRepMesh_Vertex& dV = GetVertex( aStartNode );
      const BRepMesh_Vertex& fV = GetVertex( aPivotNode );
      aPrevVEdge = fV.Coord() - dV.Coord();
      continue;
    }

    aStartNode = aPivotNode;
    aPivotNode = aRefOtherNode;
    aPrevVEdge = aRefCurrVEdge;
  }

  CleanupPolygon( aPolygon, anUsedEdges );
  MeshPolygon( aPolygon, aBoxes );
}

//=======================================================================
//function : MeshPolygon
//purpose  : Triangulatiion of a closed aPolygon described by the list of indexes of
//           its edges in the structure. (negative index == reversed)
//=======================================================================
void BRepMesh_Delaun::MeshPolygon
(
  TColStd_SequenceOfInteger&        thePoly,
  NCollection_Sequence<Bnd_Box2d>&  thePolyBoxes
)
{
  if ( !myProgress.IsNull() && myProgress->UserBreak() )
    return;

  // If the poly contains 3 and less edges
  if ( MeshElementaryPolygon(thePoly) )
  {
    return;
  }

  Standard_Integer aPivotNode, aVertex3 = 0;
  Standard_Integer aFirstNode, aLastNode;
  Standard_Integer aTriId;
  Standard_Integer aPolyIdx, aUsedIdx = 0;
  Standard_Integer aPolyLen = thePoly.Length();

  const BRepMesh_Edge& anEdge = GetEdge( Abs( thePoly(1) ) );
  Standard_Real aMinDist = RealLast();
  Standard_Real aBestAngle = RealLast();

  if ( thePoly(1) > 0 )
  {
    aFirstNode = anEdge.FirstNode();
    aLastNode  = anEdge.LastNode();
  }
  else
  {
    aFirstNode = anEdge.LastNode();
    aLastNode  = anEdge.FirstNode();
  }

  gp_XY aVEdge( GetVertex( aLastNode  ).Coord() -
                GetVertex( aFirstNode ).Coord() );

  Standard_Real aVEdgeLen = aVEdge.Modulus();
  if ( aVEdgeLen > 0.)
  {
    aVEdge.SetCoord( aVEdge.X() / aVEdgeLen,
                     aVEdge.Y() / aVEdgeLen );

    for ( aPolyIdx = 3; aPolyIdx <= aPolyLen; ++aPolyIdx )
    {
      const BRepMesh_Edge& aNextEdge = GetEdge( Abs( thePoly( aPolyIdx ) ) );
      if ( thePoly( aPolyIdx ) > 0)
        aPivotNode = aNextEdge.FirstNode();
      else
        aPivotNode = aNextEdge.LastNode();

      gp_XY aVEdgePivot( GetVertex( aPivotNode ).Coord() -
                          GetVertex( aLastNode  ).Coord() );

      Standard_Real aDist = aVEdge ^ aVEdgePivot;
      Standard_Real anAngle = Abs(aVEdge * aVEdgePivot);
      if ( Abs( aDist ) > Precision::PConfusion() )
      {
        if ( ( aDist > 0. &&  myIsPositiveOrientation ) || 
              ( aDist < 0. && !myIsPositiveOrientation ) )
        { 
          if ( Abs( aDist ) < aMinDist || ( ( Abs( aDist ) - aMinDist ) < Precision::PConfusion() && anAngle < aBestAngle ) )
          {
            Bnd_Box2d aBoxFirst, aBoxLast;
            aBoxFirst.Add( gp_Pnt2d( GetVertex( aPivotNode ).Coord() ) );
            aBoxFirst.Add( gp_Pnt2d( GetVertex( aLastNode  ).Coord() ) );

            aBoxLast.Add( gp_Pnt2d( GetVertex( aPivotNode ).Coord() ) );
            aBoxLast.Add( gp_Pnt2d( GetVertex( aFirstNode ).Coord() ) );

            BRepMesh_Edge aCheckE1( aLastNode,  aPivotNode, BRepMesh_Free );
            BRepMesh_Edge aCheckE2( aFirstNode, aPivotNode, BRepMesh_Free );

            Standard_Boolean isIntersect = Standard_False;
            for ( Standard_Integer aPolyIdx1 = 2; aPolyIdx1 <= aPolyLen; ++aPolyIdx1 )
            {
              if( aPolyIdx1 == aPolyIdx )
                continue;

              const BRepMesh_Edge& aNextEdge1 = GetEdge( Abs( thePoly( aPolyIdx1 ) ) );
              if ( !aBoxFirst.IsOut( thePolyBoxes.Value( aPolyIdx1 ) ) )
              {                    
                // intersection is possible...                  
                if( IntSegSeg( aCheckE1, aNextEdge1 ) )
                {
                  isIntersect = Standard_True;
                  break;
                }
              }

              if ( !aBoxLast.IsOut( thePolyBoxes.Value( aPolyIdx1 ) ) &&
                    !aCheckE2.IsEqual( aNextEdge1 ) )
              {
                // intersection is possible...                  
                if( IntSegSeg( aCheckE2, aNextEdge1 ) )
                {
                  isIntersect = Standard_True;
                  break;
                }
              }
            }

            if( !isIntersect )
            {
              aMinDist = aDist;
              aBestAngle = anAngle;
              aVertex3 = aPivotNode;
              aUsedIdx = aPolyIdx;
            }
          }
        }
      }
    }
  }

  Standard_Integer aNewEdge2, aNewEdge3;
  if ( aMinDist < RealLast() )
  {
    aNewEdge2 = myMeshData->AddLink( BRepMesh_Edge( aLastNode, aVertex3,   BRepMesh_Free ) );
    aNewEdge3 = myMeshData->AddLink( BRepMesh_Edge( aVertex3,  aFirstNode, BRepMesh_Free ) );
    aTriId    = myMeshData->AddElement( BRepMesh_Triangle( 
      Abs( thePoly(1) ), Abs( aNewEdge2 ), Abs( aNewEdge3 ), 
      thePoly(1) > 0,    aNewEdge2 > 0,    aNewEdge3 > 0,
      BRepMesh_Free ) );

    Standard_Boolean isAdded = myCircles.Add( GetVertex( aFirstNode ).Coord(), 
      GetVertex( aLastNode ).Coord(), GetVertex( aVertex3 ).Coord(), aTriId );

    if ( !isAdded )
      myMeshData->RemoveElement( aTriId );

    if ( aUsedIdx < aPolyLen )
    {
      TColStd_SequenceOfInteger aSuitePoly;
      NCollection_Sequence<Bnd_Box2d> aSuitePolyBoxes;
      thePoly.Split( aUsedIdx, aSuitePoly );
      thePolyBoxes.Split( aUsedIdx, aSuitePolyBoxes );

      aSuitePoly.Prepend( -aNewEdge3 );
      aSuitePolyBoxes.Prepend( Bnd_Box2d() );
      fillBndBox(aSuitePolyBoxes, GetVertex( aVertex3 ), GetVertex( aFirstNode ), 1);

      MeshPolygon( aSuitePoly, aSuitePolyBoxes );
    }
    else
    {
      thePoly.Remove( aPolyLen );
      thePolyBoxes.Remove( aPolyLen );
    }

    if ( aUsedIdx > 3 )
    {
      thePoly.SetValue( 1, -aNewEdge2 );
      fillBndBox( thePolyBoxes, GetVertex( aLastNode ), GetVertex( aVertex3 ), 1);

      MeshPolygon( thePoly, thePolyBoxes );
    }
  }
}

//=======================================================================
//function : AddTriangle
//purpose  : Add a triangle based on the given oriented edges into mesh
//=======================================================================
inline void BRepMesh_Delaun::AddTriangle( const Standard_Integer (&theEdgesId)[3],
                                          const Standard_Boolean (&theEdgesOri)[3],
                                          const Standard_Integer (&theNodesId)[3] )
{
  Standard_Integer aNewTriangleId = 
    myMeshData->AddElement( BRepMesh_Triangle( 
      theEdgesId[0],  theEdgesId[1],  theEdgesId[2], 
      theEdgesOri[0], theEdgesOri[1], theEdgesOri[2],
      BRepMesh_Free ) );

  Standard_Boolean isAdded = myCircles.Add( 
    GetVertex( theNodesId[0] ).Coord(), 
    GetVertex( theNodesId[1] ).Coord(),
    GetVertex( theNodesId[2] ).Coord(),
    aNewTriangleId );
    
  if ( !isAdded )
    myMeshData->RemoveElement( aNewTriangleId );
}

//=======================================================================
//function : MeshElementaryPolygon
//purpose  : Triangulation of closed polygon containing only three edges.
//=======================================================================
inline Standard_Boolean BRepMesh_Delaun::MeshElementaryPolygon( 
  const TColStd_SequenceOfInteger& thePolygon )
{
  Standard_Integer aPolyLen = thePolygon.Length();
  if ( aPolyLen < 3 )
    return Standard_True;
  else if ( aPolyLen > 3 )
    return Standard_False;

  // Just create a triangle
  Standard_Integer anEdges[3];
  Standard_Boolean anEdgesOri[3];

  for ( Standard_Integer anEdgeIt = 0; anEdgeIt < 3; ++anEdgeIt )
  {
    Standard_Integer anEdgeInfo = thePolygon( anEdgeIt + 1 );
    anEdges[anEdgeIt]           = Abs( anEdgeInfo );
    anEdgesOri[anEdgeIt]        = ( anEdgeInfo > 0 );
  }
    
  const BRepMesh_Edge& anEdge1 = GetEdge( anEdges[0] );
  const BRepMesh_Edge& anEdge2 = GetEdge( anEdges[1] );
  
  Standard_Integer aNodes[3] = { anEdge1.FirstNode(),
                                 anEdge1.LastNode(),
                                 anEdge2.FirstNode() };
  if ( aNodes[2] == aNodes[0] || 
       aNodes[2] == aNodes[1] )
  {
    aNodes[2] = anEdge2.LastNode();
  }

  AddTriangle( anEdges, anEdgesOri, aNodes );
  return Standard_True;
}

//=======================================================================
//function : RemoveVertex
//purpose  : Removes a vertex from the triangulation
//=======================================================================
void  BRepMesh_Delaun::RemoveVertex( const BRepMesh_Vertex& theVertex )
{
  BRepMesh_SelectorOfDataStructureOfDelaun aSelector( myMeshData );
  aSelector.NeighboursOf( theVertex );

  BRepMesh_MapOfIntegerInteger aLoopEdges( 10, myMeshData->Allocator() );

  // Loop on triangles to be destroyed :
  BRepMesh_MapOfInteger::Iterator aTriangleIt( aSelector.Elements() );
  for ( ; aTriangleIt.More(); aTriangleIt.Next() )
    DeleteTriangle( aTriangleIt.Key(), aLoopEdges );

  NCollection_Sequence<Bnd_Box2d> aBoxes;
  TColStd_SequenceOfInteger     aPolygon;
  Standard_Integer aLoopEdgesCount = aLoopEdges.Extent();
  BRepMesh_MapOfIntegerInteger::Iterator aLoopEdgesIt( aLoopEdges );

  if ( aLoopEdgesIt.More() )
  {
    const BRepMesh_Edge& anEdge = GetEdge( aLoopEdgesIt.Key() );
    Standard_Integer aFirstNode = anEdge.FirstNode();
    Standard_Integer aLastNode;
    Standard_Integer aPivotNode = anEdge.LastNode();
    Standard_Integer anEdgeId   = aLoopEdgesIt.Key();
    
    Standard_Boolean isPositive = (Standard_Boolean)aLoopEdges( anEdgeId );
    if ( !isPositive )
    {
      Standard_Integer aTmp;
      aTmp       = aFirstNode;
      aFirstNode = aPivotNode;
      aPivotNode = aTmp;
      
      aPolygon.Append( -anEdgeId );
    }
    else
      aPolygon.Append( anEdgeId );

    fillBndBox( aBoxes, GetVertex( aFirstNode ), GetVertex( aPivotNode ) );

    aLoopEdges.UnBind( anEdgeId );
    
    aLastNode = aFirstNode;
    while ( aPivotNode != aLastNode )
    {
      BRepMesh_ListOfInteger::Iterator aLinkIt( myMeshData->LinkNeighboursOf( aPivotNode ) );
      for ( ; aLinkIt.More(); aLinkIt.Next() )
      {
        if ( aLinkIt.Value() != anEdgeId &&
             aLoopEdges.IsBound( aLinkIt.Value() ) )
        {
          Standard_Integer aCurrentNode;
          anEdgeId = aLinkIt.Value();
          const BRepMesh_Edge& anEdge1 = GetEdge( anEdgeId );
          
          aCurrentNode = anEdge1.LastNode();
          if ( aCurrentNode != aPivotNode )
          {
            aCurrentNode = anEdge1.FirstNode();
            aPolygon.Append( -anEdgeId );
          }
          else
            aPolygon.Append( anEdgeId );

          fillBndBox( aBoxes, GetVertex( aCurrentNode ), GetVertex( aPivotNode ) );
            
          aPivotNode = aCurrentNode;
          aLoopEdges.UnBind( anEdgeId );
          break;
        }
      }
      
      if ( aLoopEdgesCount <= 0 )
        break;
      --aLoopEdgesCount;
    }
    
    MeshPolygon( aPolygon, aBoxes );
  }
}


//=======================================================================
//function : AddVertices
//purpose  : Adds some vertices in the triangulation.
//=======================================================================
void  BRepMesh_Delaun::AddVertices( BRepMesh_Array1OfVertexOfDelaun& theVertices )
{
  BRepMesh_HeapSortVertexOfDelaun::Sort( theVertices, 
    BRepMesh_ComparatorOfVertexOfDelaun( SortingDirection ) );

  Standard_Integer aLower  = theVertices.Lower();
  Standard_Integer anUpper = theVertices.Upper();
    
  TColStd_Array1OfInteger aVertexIndices( aLower, anUpper );
  for ( Standard_Integer i = aLower; i <= anUpper; ++i )     
    aVertexIndices(i) = myMeshData->AddNode( theVertices(i) );

  CreateTrianglesOnNewVertices( aVertexIndices );
}

//=======================================================================
//function : UseEdge
//purpose  : Modify mesh to use the edge. Return True if done
//=======================================================================
Standard_Boolean BRepMesh_Delaun::UseEdge( const Standard_Integer /*theIndex*/ )
{
  /*
  const BRepMesh_PairOfIndex& aPair = myMeshData->ElemConnectedTo( theIndex );
  if ( aPair.Extent() == 0 )
  {
    const BRepMesh_Edge& anEdge = GetEdge( theIndex );
    
    Standard_Integer aStartNode, aPivotNode, anOtherNode;
    aStartNode = anEdge.FirstNode();
    aPivotNode = anEdge.LastNode();
    
    const BRepMesh_ListOfInteger& aStartNodeNeighbors = myMeshData->LinkNeighboursOf( aStartNode );
    const BRepMesh_ListOfInteger& aPivotNodeNeighbors = myMeshData->LinkNeighboursOf( aPivotNode );
    
    if ( aStartNodeNeighbors.Extent() > 0 &&
         aPivotNodeNeighbors.Extent() > 0 )
    {
      const BRepMesh_Vertex& aStartVertex = GetVertex( aStartNode );
      const BRepMesh_Vertex& aPivotVertex = GetVertex( aPivotNode );

      gp_XY aVEdge   ( aPivotVertex.Coord() );
      aVEdge.Subtract( aStartVertex.Coord() );

      Standard_Real    anAngle    = 0.;
      Standard_Real    anAngleMin = RealLast();
      Standard_Real    anAngleMax = RealFirst();
      Standard_Integer aLeftEdge  = 0, aRightEdge = 0;

      BRepMesh_ListOfInteger::Iterator aNeighborIt( aPivotNodeNeighbors );
      for ( ; aNeighborIt.More(); aNeighborIt.Next() )
      {
        Standard_Integer anEdgeId = aNeighborIt.Value();
        if ( anEdgeId != theIndex )
        {
          const BRepMesh_Edge& aNextEdge = GetEdge( anEdgeId );

          Standard_Boolean isInMesh = Standard_True;
          if ( aNextEdge.Movability() == BRepMesh_Free )
          {
            if ( myMeshData->ElemConnectedTo( anEdgeId ).IsEmpty() ) 
              isInMesh = Standard_False;
          }

          if ( isInMesh )
          {
            anOtherNode = aNextEdge.FirstNode();
            if ( anOtherNode == aPivotNode )
              anOtherNode = aNextEdge.LastNode();

            gp_XY aVEdgeCur = GetVertex( anOtherNode ).Coord();
            aVEdgeCur.Subtract( aPivotVertex.Coord() );

            anAngle = gp_Vec2d( aVEdge ).Angle( gp_Vec2d( aVEdgeCur ) );
          }
          
          if ( anAngle > anAngleMax )
          {
            anAngleMax = anAngle;
            aLeftEdge  = anEdgeId;
          }
          if ( anAngle < anAngleMin )
          {
            anAngleMin = anAngle;
            aRightEdge = anEdgeId;
          }
        }
      }
      
      if ( aLeftEdge > 0 )
      {
        if (aLeftEdge==aRightEdge)
        {
        }
        else
        {
        }
      }
    }
  }
  */
  return Standard_False;
}

//=======================================================================
//function : GetEdgesByType
//purpose  : Gives the list of edges with type defined by input parameter
//=======================================================================
Handle(BRepMesh_MapOfInteger) BRepMesh_Delaun::GetEdgesByType( const BRepMesh_DegreeOfFreedom theEdgeType ) const
{
  Handle(BRepMesh_MapOfInteger) aResult = new BRepMesh_MapOfInteger;
  BRepMesh_MapOfInteger::Iterator anEdgeIt( myMeshData->LinkOfDomain() );

  for ( ; anEdgeIt.More(); anEdgeIt.Next() )
  {
    Standard_Integer anEdge = anEdgeIt.Key();
    Standard_Boolean isToAdd = (theEdgeType == BRepMesh_Free) ?
      (myMeshData->ElemConnectedTo( anEdge ).Extent() <= 1) :
      (GetEdge( anEdge ).Movability() == theEdgeType);

    if (isToAdd)
      aResult->Add( anEdge );
  }
  
  return aResult;
}

//=======================================================================
//function : calculateDist
//purpose  : Calculates distances between the given point and edges of
//           triangle
//=======================================================================
static Standard_Real calculateDist( const gp_XY theVEdges[3],
                                    const gp_XY thePoints[3],
                                    const Standard_Integer theEdgesId[3],
                                    const BRepMesh_Vertex& theVertex,
                                    Standard_Real theDistance[3],
                                    Standard_Real theSqModulus[3],
                                    Standard_Integer& theEdgeOn )
{
  Standard_Real aMinDist = -1;
  if ( !theVEdges || !thePoints || !theEdgesId || 
       !theDistance || !theSqModulus )
    return aMinDist;
    
  for( Standard_Integer i = 0; i < 3; ++i )
  {
    theSqModulus[i] = theVEdges[i].SquareModulus();
    if ( theSqModulus[i] <= Precision2 )
      return -1;
      
    theDistance[i] = theVEdges[i] ^ ( theVertex.Coord() - thePoints[i] );
    
    Standard_Real aDist = theDistance[i] * theDistance[i];
    aDist /= theSqModulus[i];
    
    if ( aMinDist < 0 || aDist < aMinDist )
    {
      theEdgeOn = theEdgesId[i];
      aMinDist  = aDist;
    }
  }
  
  return aMinDist;
}

//=======================================================================
//function : Contains
//purpose  : Test if triangle of index <TrianIndex> contains geometricaly
//           <theVertex>. If <theEdgeOn> is != 0 then theVertex is on Edge 
//           of  index <theEdgeOn>
//=======================================================================
Standard_Boolean BRepMesh_Delaun::Contains( const Standard_Integer theTriangleId,
                                            const BRepMesh_Vertex& theVertex,
                                            Standard_Integer&      theEdgeOn ) const
{
  theEdgeOn = 0;
  
  Standard_Integer e[3];
  Standard_Boolean o[3];
  Standard_Integer p[3];

  GetTriangle( theTriangleId ).Edges( e[0], e[1], e[2],
                                      o[0], o[1], o[2] );
                                      
  const BRepMesh_Edge* anEdges[3] = { &GetEdge( e[0] ),
                                      &GetEdge( e[1] ),
                                      &GetEdge( e[2] ) };
  if ( o[0] )
  {
    p[0] = anEdges[0]->FirstNode();
    p[1] = anEdges[0]->LastNode();
  }
  else
  {
    p[1] = anEdges[0]->FirstNode();
    p[0] = anEdges[0]->LastNode();
  }
  
  if ( o[2] )
    p[2] = anEdges[2]->FirstNode();
  else
    p[2] = anEdges[2]->LastNode();

  gp_XY aPoints[3];
  aPoints[0] = GetVertex( p[0] ).Coord();
  aPoints[1] = GetVertex( p[1] ).Coord();
  aPoints[2] = GetVertex( p[2] ).Coord();
  
  gp_XY aVEdges[3];
  aVEdges[0] = aPoints[1]; 
  aVEdges[0].Subtract( aPoints[0] );
  
  aVEdges[1] = aPoints[2]; 
  aVEdges[1].Subtract( aPoints[1] );
  
  aVEdges[2] = aPoints[0];
  aVEdges[2].Subtract( aPoints[2] );

  Standard_Real aDistance[3];
  Standard_Real aSqModulus[3];

  Standard_Real aMinDist;  
  aMinDist = calculateDist( aVEdges, aPoints, e, theVertex, aDistance, aSqModulus, theEdgeOn );
  if ( aMinDist < 0 )
    return Standard_False;
      
  if ( aMinDist > Precision2 )
  {
    Standard_Integer anEdgeId = theEdgeOn;
    theEdgeOn = 0;
    
    if ( anEdgeId != 0 ) 
    {
      Standard_Integer i = 0;
      for ( ; i < 3; ++i )
      {
        if( e[i] == anEdgeId )
          break;
      }
      
      if( anEdges[i]->Movability() != BRepMesh_Free )
        if ( aDistance[i] < ( aSqModulus[i] / 5. ) )
          theEdgeOn = e[i];
    }
  }

  return ( aDistance[0] + aDistance[1] + aDistance[2] != 0. &&
            ( ( aDistance[0] >= 0. && aDistance[1] >= 0. && aDistance[2] >= 0. ) ||
              ( aDistance[0] <= 0. && aDistance[1] <= 0. && aDistance[2] <= 0. ) ) );
}


//=============================================================================
//function : classifyPoint
//purpose  : Classifies the point in case of coincidence of two vectors.
//           Returns zero value if point is out of segment and non zero 
//           value if point is between the first and the second point of segment.
//           thePoint1       - the start point of a segment (base point)
//           thePoint2       - the end point of a segment
//           thePointToCheck - the point to classify
//=============================================================================
static Standard_Integer classifyPoint( const gp_XY& thePoint1,
                                       const gp_XY& thePoint2,
                                       const gp_XY& thePointToCheck )
{
  gp_XY aP1 = thePoint2       - thePoint1;
  gp_XY aP2 = thePointToCheck - thePoint1;
  
  Standard_Real aDist = Abs( aP1 ^ aP2 );
  if ( aDist >= Precision )
  {
    aDist = ( aDist * aDist ) / aP1.SquareModulus();
    if ( aDist >= Precision2 )
      return 0; //out
  }
    
  gp_XY aMult = aP1.Multiplied( aP2 );
  if ( aMult.X() < 0.0 || aMult.Y() < 0.0 )
    return 0; //out
    
  if ( aP1.SquareModulus() < aP2.SquareModulus() )
    return 0; //out
    
  if ( thePointToCheck.IsEqual( thePoint1, Precision ) || 
       thePointToCheck.IsEqual( thePoint2, Precision ) )
    return -1; //coinsides with an end point
    
  return 1;
}

//=============================================================================
// Function: IntSegSeg
//purpose  : Checks intersection between the two segments.
//=============================================================================
Standard_Boolean BRepMesh_Delaun::IntSegSeg( const BRepMesh_Edge& theEdg1,
                                             const BRepMesh_Edge& theEdg2,
                                             const Standard_Boolean includeEnds) const
{
  gp_XY p1, p2, p3, p4;
  p1 = GetVertex( theEdg1.FirstNode() ).Coord();
  p2 = GetVertex( theEdg1.LastNode()  ).Coord();
  p3 = GetVertex( theEdg2.FirstNode() ).Coord();
  p4 = GetVertex( theEdg2.LastNode()  ).Coord();
  
  Standard_Integer aPoint1 = classifyPoint( p1, p2, p3 );
  Standard_Integer aPoint2 = classifyPoint( p1, p2, p4 );
  Standard_Integer aPoint3 = classifyPoint( p3, p4, p1 );
  Standard_Integer aPoint4 = classifyPoint( p3, p4, p2 );
  
  if ( aPoint1 > 0 || aPoint2 > 0 ||
       aPoint3 > 0 || aPoint4 > 0 )
    return Standard_True;

  if (includeEnds &&
      (aPoint1 == 0 || aPoint2 == 0 ||
       aPoint3 == 0 || aPoint4 == 0)
     )
    return Standard_True;

  gp_XY aPl1 = p2 - p1;
  gp_XY aPl2 = p4 - p3;
  gp_XY aPl3 = p1 - p3;
    
  Standard_Real aCrossD1D2 = aPl1 ^ aPl2;
  Standard_Real aCrossD1D3 = aPl1 ^ aPl3;
  if ( Abs( aCrossD1D2 ) < Precision::PConfusion() )
  {
    if( Abs( aCrossD1D3 ) < Precision::PConfusion() )
    {
      Standard_Integer aPosHash = aPoint1 + aPoint2;
      if ( ( !aPosHash && aPoint3 ) ) //|| aPosHash < -1 )
        return Standard_True;
        
      return Standard_False;
    }
    else
      //parallel case
      return Standard_False;
  }

  Standard_Real aPar = aCrossD1D3 / aCrossD1D2;
  // inrersects out of first segment range
  if( aPar < Precision::Angular() || aPar > 1 - Precision::Angular() )
    return Standard_False;
 
  Standard_Real aCrossD2D3 = aPl2 ^ aPl3;
  aPar = aCrossD2D3 / aCrossD1D2;
  // inrersects out of second segment range
  if( aPar < Precision::Angular() || aPar > 1 - Precision::Angular() )
    return Standard_False;
 
  return Standard_True;
}
