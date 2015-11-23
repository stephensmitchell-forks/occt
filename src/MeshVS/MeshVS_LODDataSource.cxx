// Created on: 2015-10-23
// Created by: Varvara POSKONINA
// Copyright (c) 2005-2014 OPEN CASCADE SAS
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

#include <MeshVS_LODDataSource.hxx>

#include <Graphic3d_ArrayOfPoints.hxx>
#include <Graphic3d_ArrayOfTriangles.hxx>
#include <Graphic3d_ArrayOfSegments.hxx>
#include <Graphic3d_LOD.hxx>
#include <MeshVS_Drawer.hxx>
#include <MeshVS_DrawerAttribute.hxx>
#include <MeshVS_Mesh.hxx>
#include <MeshVS_MapOfTwoNodes.hxx>
#include <MeshVS_MeshPrsBuilder.hxx>
#include <MeshVS_SymmetricPairHasher.hxx>
#include <MeshVS_Tool.hxx>
#include <Prs3d_Presentation.hxx>
#include <Prs3d_Root.hxx>
#include <TColStd_HPackedMapOfInteger.hxx>
#include <TColStd_MapIteratorOfPackedMapOfInteger.hxx>
#include <TColgp_SequenceOfXYZ.hxx>
#include <StlMesh_SequenceOfMeshTriangle.hxx>
#include <StlMesh_MeshTriangle.hxx>

//=======================================================================
// function : MeshVS_LODDataSource
// purpose  :
//=======================================================================
MeshVS_LODDataSource::MeshVS_LODDataSource (const Handle(StlMesh_Mesh)& theMesh)
  : myTriangles (1, theMesh->Triangles().Length()),
    myNodes     (1, theMesh->Vertices().Length())
{
  if (theMesh.IsNull())
    return;

  const TColgp_SequenceOfXYZ& aVerts = theMesh->Vertices();
  for (Standard_Integer aNodeIdx = 1; aNodeIdx <= aVerts.Length(); ++aNodeIdx)
  {
    myNodeIdxs.Add (aNodeIdx);
    myNodes.ChangeValue (aNodeIdx) = aVerts (aNodeIdx);
  }

  const StlMesh_SequenceOfMeshTriangle& aTriangles = theMesh->Triangles();
  for (Standard_Integer aTrgIdx = 1; aTrgIdx <= aTriangles.Length(); ++aTrgIdx)
  {
    myTriangleIdxs.Add (aTrgIdx);
    const Handle(StlMesh_MeshTriangle)& aTriangle = aTriangles.Value (aTrgIdx);
    Standard_Integer aNodeIdxs[3];
    Standard_Real aNormalX, aNormalY, aNormalZ;
    aTriangle->GetVertexAndOrientation (aNodeIdxs[0], aNodeIdxs[1], aNodeIdxs[2], aNormalX, aNormalY, aNormalZ);
    myTriangles.ChangeValue (aTrgIdx) = Poly_Triangle (aNodeIdxs[0], aNodeIdxs[1], aNodeIdxs[2]);
  }
}

//=======================================================================
// function : GetGeom
// purpose  : Returns full info about the entity according to the index and type of element given
//=======================================================================
Standard_Boolean MeshVS_LODDataSource::GetGeom (const Standard_Integer theId,
                                                const Standard_Boolean theIsElement,
                                                TColStd_Array1OfReal&  theCoords,
                                                Standard_Integer&      theNodesNb,
                                                MeshVS_EntityType&     theEntityType) const
{
  if (theId < 1)
    return Standard_False;

  if (theIsElement)
  {
    if (theId > myTriangleIdxs.Extent() || theCoords.Length() < 9)
      return Standard_False;

    Standard_Integer aNode1, aNode2, aNode3;
    myTriangles.Value (theId).Get (aNode1, aNode2, aNode3);
    gp_Pnt aPnts[3] = { myNodes.Value (aNode1),
                        myNodes.Value (aNode2),
                        myNodes.Value (aNode3) };
    for (Standard_Integer aPntIdx = 0, anIdx = 1; aPntIdx < 3; ++aPntIdx)
    {
      for (Standard_Integer aCoordIdx = 0; aCoordIdx < 3; aCoordIdx++)
      {
        theCoords (anIdx++) = aPnts[aPntIdx].XYZ().GetData()[aCoordIdx];
      }
    }

    theNodesNb = 3;
    theEntityType = MeshVS_ET_Face;

    return Standard_True;
  }
  else
  {
    if (theId > myNodeIdxs.Extent() || theCoords.Length() < 3)
      return Standard_False;

    const gp_Pnt& aNode = myNodes.Value (theId);
    theCoords (1) = aNode.X();
    theCoords (2) = aNode.Y();
    theCoords (3) = aNode.Z();

    theNodesNb = 1;
    theEntityType = MeshVS_ET_Node;

    return Standard_True;
  }
}

//=======================================================================
// function : GetGeomType
// purpose  : Returns type of geometry according to the index and type of element given
//=======================================================================
Standard_Boolean MeshVS_LODDataSource::GetGeomType (const Standard_Integer /*theId*/,
                                                    const Standard_Boolean theIsElement,
                                                    MeshVS_EntityType&     theEntityType) const
{
  theEntityType = theIsElement ? MeshVS_ET_Face : MeshVS_ET_Node;
  return Standard_True;
}

//=======================================================================
// function : GetAddr
// purpose  :
//=======================================================================
Standard_Address MeshVS_LODDataSource::GetAddr (const Standard_Integer /*theId*/,
                                                const Standard_Boolean /*theIsElement*/) const
{
  return NULL;
}

//=======================================================================
// function : GetNodesByElement
// purpose  : Returns nodes forming a triangle with id theId
//=======================================================================
Standard_Boolean MeshVS_LODDataSource::GetNodesByElement (const Standard_Integer   theId,
                                                          TColStd_Array1OfInteger& theNodeIds,
                                                          Standard_Integer&        theNodesNb) const
{
  if (theId < 1 || theId > myTriangleIdxs.Extent() || theNodeIds.Length() < 3)
    return Standard_False;

  const Standard_Integer aLowerIdx = theNodeIds.Lower();
  myTriangles.Value (theId).Get (theNodeIds.ChangeValue (aLowerIdx),
                                 theNodeIds.ChangeValue (aLowerIdx + 1),
                                 theNodeIds.ChangeValue (aLowerIdx + 2));
  theNodesNb = 3;
  return Standard_True;
}

//=======================================================================
// function : GetAllNodes
// purpose  : This method makes storing usless map excusable
//=======================================================================
const TColStd_PackedMapOfInteger& MeshVS_LODDataSource::GetAllNodes() const
{
  return myNodeIdxs;
}

//=======================================================================
// function : GetAllElements
// purpose  : This method makes storing usless map excusable
//=======================================================================
const TColStd_PackedMapOfInteger& MeshVS_LODDataSource::GetAllElements() const
{
  return myTriangleIdxs;
}

//=======================================================================
// function : ComputePrs
// purpose  :
//=======================================================================
void MeshVS_LODDataSource::ComputePrs (const Handle(AIS_InteractiveObject) theMesh)
{
  Handle(MeshVS_Mesh) aMesh = Handle(MeshVS_Mesh)::DownCast (theMesh);
  if (aMesh.IsNull())
    return;

  // Standard_Boolean hasNodes = !myNodeIdxs.IsEmpty();
  Standard_Boolean hasTrgs  = !myTriangleIdxs.IsEmpty();
  Handle(Prs3d_Presentation) aMainPrs = theMesh->Presentation();
  const Standard_Integer aDispMode = theMesh->DisplayMode();

  //if (hasNodes)
  //{
  //  Standard_Boolean hasSelectFlag = ((aDispMode & MeshVS_DMF_SelectionPrs) != 0);
  //  Standard_Boolean hasHilightFlag = ((aDispMode & MeshVS_DMF_HilightPrs) != 0);

  //  Standard_Real aCoordsBuf[3];
  //  TColStd_Array1OfReal aCoords (*aCoordsBuf, 1, 3);
  //  Standard_Integer aNodesNb;
  //  MeshVS_EntityType aType;

  //  TColStd_PackedMapOfInteger anIDs;
  //  anIDs.Assign (myNodeIdxs);
  //  if (!hasSelectFlag && !hasHilightFlag)
  //  {
  //    // subtract the hidden nodes and ids to exclude (to minimize allocated memory)
  //    Handle(TColStd_HPackedMapOfInteger) aHiddenNodes = aMesh->GetHiddenNodes();
  //    if (!aHiddenNodes.IsNull())
  //      anIDs.Subtract (aHiddenNodes->Map());
  //  }

  //  Standard_Integer aSize = anIDs.Extent();
  //  if (aSize > 0)
  //  {
  //    Handle(Graphic3d_ArrayOfPoints) aNodePoints = new Graphic3d_ArrayOfPoints (aSize);
  //    Standard_Integer aPntsNb = 0;
  //    for (TColStd_MapIteratorOfPackedMapOfInteger aNodeIdxsIter (myNodeIdxs); aNodeIdxsIter.More(); aNodeIdxsIter.Next())
  //    {
  //      const Standard_Integer aKey = aNodeIdxsIter.Key();
  //      if (GetGeom (aKey, Standard_False, aCoords, aNodesNb, aType))
  //      {
  //        aPntsNb++;
  //        aNodePoints->AddVertex (aCoords(1), aCoords(2), aCoords(3));
  //      }
  //    }

  //    if (aPntsNb > 0)
  //    {
  //      Handle(Graphic3d_LOD) aNewLod = Prs3d_Root::NewLOD (aMainPrs);
  //      Handle(Graphic3d_Group) aLODGroup = aNewLod->NewGroup();
  //      aLODGroup->AddPrimitiveArray (aNodePoints);
  //    }
  //    return;
  //  }
  //}
  if (hasTrgs)
  {
    Standard_Integer aMaxNodesNb;

    Handle(MeshVS_MeshPrsBuilder) aBuilder = Handle(MeshVS_MeshPrsBuilder)::DownCast (aMesh->GetBuilder (1));
    if (aBuilder.IsNull())
      return;
    Handle(MeshVS_Drawer) aDrawer = aBuilder->GetDrawer();
    if (aDrawer.IsNull() ||
        !aDrawer->GetInteger (MeshVS_DA_MaxFaceNodes, aMaxNodesNb) ||
        aMaxNodesNb <= 0)
      return;

    //----------- extract useful display mode flags ----------
    Standard_Integer aDispStatus = (aDispMode & aBuilder->GetFlags());
    if ((aDispStatus & MeshVS_DMF_DeformedMask) != 0)
    {
      aDispStatus /= MeshVS_DMF_DeformedPrsWireFrame;
      // This transformation turns deformed mesh flags to real display modes
    }
    aDispStatus &= MeshVS_DMF_OCCMask;
    //--------------------------------------------------------

    Standard_Real aShrinkCoef;
    aDrawer->GetDouble (MeshVS_DA_ShrinkCoeff, aShrinkCoef);

    const Standard_Boolean isWireframe = aDispMode == MeshVS_DMF_WireFrame;
    Standard_Boolean isShading = aDispMode == MeshVS_DMF_Shading;
    Standard_Boolean isShrink = aDispMode == MeshVS_DMF_Shrink;
    const Standard_Boolean hasHilightFlag = (aDispStatus & MeshVS_DMF_HilightPrs) != 0;
    const Standard_Boolean hasSelFlag =(aDispStatus & MeshVS_DMF_SelectionPrs) != 0;
    Standard_Boolean isMeshSmoothShading = Standard_False;
    Standard_Boolean isMeshReflect, isMeshAllowOverlap, isReflect;

    aDrawer->GetBoolean (MeshVS_DA_Reflection, isMeshReflect);
    aDrawer->GetBoolean (MeshVS_DA_IsAllowOverlapped, isMeshAllowOverlap);
    isReflect = isMeshReflect && !hasHilightFlag;
    aDrawer->GetBoolean (MeshVS_DA_SmoothShading, isMeshSmoothShading);

    // display mode for highlighted prs of groups
    isShrink = isShrink && !hasHilightFlag;
    isShading = isShading || hasHilightFlag;

        Graphic3d_MaterialAspect aMatAspect;
    aDrawer->GetMaterial (MeshVS_DA_FrontMaterial, aMatAspect);
    if (!isReflect)
    {
      aMatAspect.SetReflectionModeOff(Graphic3d_TOR_AMBIENT);
      aMatAspect.SetReflectionModeOff(Graphic3d_TOR_DIFFUSE);
      aMatAspect.SetReflectionModeOff(Graphic3d_TOR_SPECULAR);
      aMatAspect.SetReflectionModeOff(Graphic3d_TOR_EMISSION);
    }
    Handle(Graphic3d_AspectFillArea3d ) aFill = MeshVS_Tool::CreateAspectFillArea3d (aDrawer, aMatAspect);
    Handle(Graphic3d_AspectLine3d ) aBeam = MeshVS_Tool::CreateAspectLine3d (aDrawer);

    const Standard_Boolean isOverlapControl =
      !isMeshAllowOverlap && (isWireframe || isShading) && !hasSelFlag;

    // subtract the hidden elements and ids to exclude (to minimize allocated memory)
    TColStd_PackedMapOfInteger anIDs;
    anIDs.Assign (myTriangleIdxs);
    Handle(TColStd_HPackedMapOfInteger) aHiddenElems = aMesh->GetHiddenElems();
    if (!aHiddenElems.IsNull())
      anIDs.Subtract (aHiddenElems->Map());

    Handle(MeshVS_HArray1OfSequenceOfInteger) aTopo;

    Standard_Boolean toShowEdges = Standard_True;
    aDrawer->GetBoolean (MeshVS_DA_ShowEdges, toShowEdges);

    toShowEdges = isWireframe || toShowEdges;

    Standard_Integer* aNodesBuf = (Standard_Integer*)alloca (aMaxNodesNb * sizeof (Standard_Integer));
    Standard_Real*    aCoordsBuf = (Standard_Real*)alloca (3 * aMaxNodesNb * sizeof (Standard_Real));

    TColStd_Array1OfInteger aNodes (*aNodesBuf, 1, aMaxNodesNb);
    TColStd_Array1OfReal    aCoords (*aCoordsBuf, 1, 3 * aMaxNodesNb);

    Standard_Integer aNbFacePrimitives = 0;
    Standard_Integer aNbVolmPrimitives = 0;
    Standard_Integer aNbEdgePrimitives = 0;
    Standard_Integer aNbLinkPrimitives = 0;

    MeshVS_EntityType aType;

    TColStd_MapIteratorOfPackedMapOfInteger anIdxIter (anIDs);
    for (anIdxIter.Reset(); anIdxIter.More(); anIdxIter.Next())
    {
      Standard_Integer aNbNodes = 0;

      if (!GetGeom (anIdxIter.Key(), Standard_True, aCoords, aNbNodes, aType))
        continue;

      if (aType == MeshVS_ET_Volume)
      {
        if (Get3DGeom (anIdxIter.Key(), aNbNodes, aTopo))
        {
          for (Standard_Integer aFaceIdx = aTopo->Lower(); aFaceIdx <= aTopo->Upper(); ++aFaceIdx)
          {
            const TColStd_SequenceOfInteger& aFaceNodes = aTopo->Value(aFaceIdx);

            if (toShowEdges) // add edge segments
            {
              aNbEdgePrimitives += aFaceNodes.Length();
            }

            if (isShading || isShrink) // add volumetric cell triangles
            {
              if (!hasSelFlag)
                aNbVolmPrimitives += aFaceNodes.Length() - 2;
            }
          }
        }
      }
      else if (aType == MeshVS_ET_Link)
      {
        if (toShowEdges)
        {
          aNbLinkPrimitives += 1; // add link segment
        }
      }
      else if (aType == MeshVS_ET_Face)
      {
        if (toShowEdges)
        {
          aNbEdgePrimitives += aNbNodes; // add edge segments
        }

        if (!isOverlapControl || isShading)
        {
          if ((isShading || isShrink) && !hasSelFlag)
          {
            aNbFacePrimitives += aNbNodes - 2; // add face triangles
          }
        }
      }
    }

    // Here we do not use indices arrays because they are not effective for some mesh
    // drawing modes: shrinking mode (displaces the vertices inside the polygon), 3D
    // cell rendering (normal interpolation is not always applicable - flat shading),
    // elemental coloring (color interpolation is impossible)
    Handle(Graphic3d_ArrayOfTriangles) aVolmTriangles =
      new Graphic3d_ArrayOfTriangles (aNbVolmPrimitives * 3, 0, isReflect);
    Handle(Graphic3d_ArrayOfTriangles) aFaceTriangles =
      new Graphic3d_ArrayOfTriangles (aNbFacePrimitives * 3, 0, isReflect);

    Handle(Graphic3d_ArrayOfSegments) aLinkSegments;
    Handle(Graphic3d_ArrayOfSegments) aEdgeSegments;

    if (toShowEdges)
    {
      aLinkSegments = new Graphic3d_ArrayOfSegments (aNbLinkPrimitives * 2);
      aEdgeSegments = new Graphic3d_ArrayOfSegments (aNbEdgePrimitives * 2);
    }

    TColStd_PackedMapOfInteger aCustomElements;

    MeshVS_MapOfTwoNodes aLinkNodes;

    Quantity_Color       anOldEdgeColor;
    Quantity_Color       anEdgeColor;
    Quantity_Color       anIntColor;
    Aspect_InteriorStyle anIntType;
    Aspect_TypeOfLine    aLine;
    Standard_Real        aWidth;

    aFill->Values (anIntType, anIntColor, anEdgeColor, aLine, aWidth);

    // Forbid drawings of edges which overlap with some links
    if (toShowEdges && isOverlapControl)
    {
      for (anIdxIter.Reset(); anIdxIter.More(); anIdxIter.Next())
      {
        if (GetGeomType (anIdxIter.Key(), Standard_True, aType) && aType == MeshVS_ET_Link)
        {
          Standard_Integer aNbNodes;

          if (GetNodesByElement (anIdxIter.Key(), aNodes, aNbNodes) && aNbNodes == 2)
          {
            aLinkNodes.Add (MeshVS_TwoNodes (aNodes(1), aNodes(2)));
          }
        }
      }
    }

    NCollection_Map<MeshVS_NodePair, MeshVS_SymmetricPairHasher> aSegmentMap;

    for (anIdxIter.Reset(); anIdxIter.More(); anIdxIter.Next())
    {
      const Standard_Integer aKey = anIdxIter.Key();

      Standard_Integer NbNodes;
      if (!GetGeom (aKey, Standard_True, aCoords, NbNodes, aType))
        continue;

      if (!GetNodesByElement (aKey, aNodes, NbNodes))
        continue;

      switch (aType)
      {
      case MeshVS_ET_Volume:
      {
        if (Get3DGeom (aKey, NbNodes, aTopo))
        {
          // Add wire-frame presentation (draw edges for shading mode as well)
          if (toShowEdges)
          {
            aBuilder->AddVolumePrs (aTopo, aCoords, NbNodes,
                                    aEdgeSegments, isReflect,
                                    isShrink, hasSelFlag,
                                    aShrinkCoef);
          }

          // Add shading presentation
          if ((isShading || isShrink) && !hasSelFlag)
          {
            aBuilder->AddVolumePrs (aTopo, aCoords, NbNodes,
                                    aVolmTriangles, isReflect,
                                    isShrink, hasSelFlag,
                                    aShrinkCoef);
          }
        }
      }
      break;

      case MeshVS_ET_Link:
      {
        if (toShowEdges)
        {
          aBuilder->AddLinkPrs (aCoords, aLinkSegments, isShrink || hasSelFlag, aShrinkCoef);
        }
      }
      break;

      case MeshVS_ET_Face:
      {
        if (toShowEdges && isOverlapControl)
        {
          Standard_Integer Last = 0;

          MeshVS_TwoNodes aTwoNodes (aNodes(1));

          for (Standard_Integer i = 1; i <= NbNodes; ++i)
          {
            if (i > 1)
              aTwoNodes.First = aTwoNodes.Second;

            aTwoNodes.Second = (i < NbNodes) ? aNodes (i + 1) : aNodes (1);

            if (aLinkNodes.Contains (aTwoNodes))
            {
              for (Standard_Integer aNodeIdx = Last + 1; aNodeIdx < i; ++aNodeIdx)
              {
                const Standard_Integer aNextIdx = aNodeIdx + 1;

                aEdgeSegments->AddVertex (
                  aCoords (3 * aNodeIdx - 2), aCoords (3 * aNodeIdx - 1), aCoords (3 * aNodeIdx));
                aEdgeSegments->AddVertex(
                  aCoords (3 * aNextIdx - 2), aCoords (3 * aNextIdx - 1), aCoords (3 * aNextIdx));
              }

              Last = i;
            }
          }

          if (NbNodes - Last > 0)
          {
            for (Standard_Integer aNodeIdx = Last; aNodeIdx < NbNodes; ++aNodeIdx)
            {
              const Standard_Integer aNextIdx = (aNodeIdx + 1) % NbNodes;

              const MeshVS_NodePair aSegment (aNodes (aNodeIdx + 1), aNodes (aNextIdx + 1));

              if (!aSegmentMap.Contains (aSegment))
              {
                aEdgeSegments->AddVertex (aCoords (3 * aNodeIdx + 1),
                                          aCoords (3 * aNodeIdx + 2),
                                          aCoords (3 * aNodeIdx + 3));

                aEdgeSegments->AddVertex (aCoords (3 * aNextIdx + 1),
                                          aCoords (3 * aNextIdx + 2),
                                          aCoords (3 * aNextIdx + 3));

                aSegmentMap.Add (aSegment);
              }
            }
          }
        }

        if (!isOverlapControl || isShading)
        {
          if (!isOverlapControl && toShowEdges)
          {
            aBuilder->AddFaceWirePrs (aCoords, NbNodes, aEdgeSegments, isShrink || hasSelFlag, aShrinkCoef);
          }

          if ((isShading || isShrink) && !hasSelFlag)
          {
            aBuilder->AddFaceSolidPrs (this, aKey, aCoords, NbNodes, aMaxNodesNb, aFaceTriangles, isReflect,
              isShrink || hasSelFlag, aShrinkCoef, isMeshSmoothShading);
          }
        }
      }
      break;

      default:
        aCustomElements.Add (aKey);
      }
    }

    if (isShrink)
    {
      anOldEdgeColor = anEdgeColor;
      aFill->SetEdgeColor (Quantity_NOC_BLACK);
    }

    Standard_Boolean isSupressBackFaces = Standard_False;
    if (!aDrawer.IsNull())
    {
      aDrawer->GetBoolean (MeshVS_DA_SupressBackFaces, isSupressBackFaces);
    }
    drawArrays (aMainPrs, aFaceTriangles, aEdgeSegments, aLinkSegments, aVolmTriangles,
      !toShowEdges, hasSelFlag, isSupressBackFaces, aFill, aBeam);
  }
}

//================================================================
// Function : DrawArrays
// Purpose  :
//================================================================
void MeshVS_LODDataSource::drawArrays (const Handle(Prs3d_Presentation)& theBasePrs,
                                       const Handle(Graphic3d_ArrayOfPrimitives)& thePolygons,
                                       const Handle(Graphic3d_ArrayOfPrimitives)& theLines,
                                       const Handle(Graphic3d_ArrayOfPrimitives)& theLinkLines,
                                       const Handle(Graphic3d_ArrayOfPrimitives)& theVolumesInShad,
                                       const Standard_Boolean theIsPolygonsEdgesOff,
                                       const Standard_Boolean theIsSelected,
                                       const Standard_Boolean theIsSupressBackFaces,
                                       const Handle(Graphic3d_AspectFillArea3d)& theFillAsp,
                                       const Handle(Graphic3d_AspectLine3d)& theLineAsp) const
{
  if (theFillAsp.IsNull())
    return;

  Standard_Boolean isFacePolygons   = (!thePolygons.IsNull() && thePolygons->ItemNumber() > 0),
                   isVolumePolygons = (!theVolumesInShad.IsNull() && theVolumesInShad->ItemNumber() > 0),
                   isPolygons       = isFacePolygons || isVolumePolygons,
                   isPolylines      = (!theLines.IsNull() && theLines->ItemNumber() > 0),
                   isLinkPolylines  = (!theLinkLines.IsNull() && theLinkLines->ItemNumber() > 0);

  Aspect_InteriorStyle aStyle;
  Quantity_Color anIntColor, aBackColor, anEdgeColor;
  Aspect_TypeOfLine aType;
  Standard_Real aWidth;

  Handle(Graphic3d_LOD) aNewLod = theBasePrs->NewLOD();
  theFillAsp->Values (aStyle, anIntColor, aBackColor, anEdgeColor, aType, aWidth);

  if (isPolygons && theFillAsp->FrontMaterial().Transparency() < 0.01)
  {
    Handle (Graphic3d_Group) aGroup = aNewLod->NewGroup (theBasePrs);

    theFillAsp->SetEdgeOff();

    if (anIntColor != aBackColor)
      theFillAsp->SetDistinguishOn();
    else
      theFillAsp->SetDistinguishOff();

    aGroup->SetClosed (theIsSupressBackFaces);
    Handle(Graphic3d_AspectFillArea3d) aFillAsp = new Graphic3d_AspectFillArea3d (*(theFillAsp.operator->()));
    if (theIsSupressBackFaces)
    {
      aFillAsp->SuppressBackFace();
    }
    aGroup->SetPrimitivesAspect (aFillAsp);

    if (isFacePolygons)
    {
      aGroup->AddPrimitiveArray (thePolygons);
    }

    if (isVolumePolygons)
    {
      aGroup->AddPrimitiveArray (theVolumesInShad);
    }
  }

  if (isPolylines && !theIsPolygonsEdgesOff)
  {
    Handle (Graphic3d_Group) aLGroup = aNewLod->NewGroup (theBasePrs);

    theFillAsp->SetEdgeOff();
    if (theIsSelected)
      aLGroup->SetPrimitivesAspect (theLineAsp);
    else
    {
      aLGroup->SetPrimitivesAspect (theFillAsp);
      aLGroup->SetPrimitivesAspect (new Graphic3d_AspectLine3d (anEdgeColor, Aspect_TOL_SOLID, aWidth));
    }
    aLGroup->AddPrimitiveArray (theLines);
    theFillAsp->SetEdgeOn();
  }

  if (isLinkPolylines)
  {
    Handle (Graphic3d_Group) aBeamGroup = aNewLod->NewGroup (theBasePrs);

    theFillAsp->SetEdgeOff();
    if (!theIsSelected)
      aBeamGroup->SetPrimitivesAspect (theFillAsp);
    aBeamGroup->SetPrimitivesAspect (theLineAsp);
    aBeamGroup->AddPrimitiveArray (theLinkLines);
    theFillAsp->SetEdgeOn();
  }

  if (isPolygons && theFillAsp->FrontMaterial().Transparency() >= 0.01)
  {
    Handle (Graphic3d_Group) aGroup = aNewLod->NewGroup (theBasePrs);

    theFillAsp->SetEdgeOff();

    if (anIntColor != aBackColor)
      theFillAsp->SetDistinguishOn();
    else
      theFillAsp->SetDistinguishOff();

    aGroup->SetClosed (theIsSupressBackFaces);
    Handle(Graphic3d_AspectFillArea3d) aFillAsp = new Graphic3d_AspectFillArea3d (*(theFillAsp.operator->()));
    if (theIsSupressBackFaces)
    {
      aFillAsp->SuppressBackFace();
    }
    aGroup->SetPrimitivesAspect (aFillAsp);

    if (isFacePolygons)
    {
      aGroup->AddPrimitiveArray (thePolygons);
    }

    if (isVolumePolygons)
    {
      aGroup->AddPrimitiveArray (theVolumesInShad);
    }
  }
}
