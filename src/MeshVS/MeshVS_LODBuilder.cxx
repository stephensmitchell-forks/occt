// Created on: 2015-11-25
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

#include <Graphic3d_ArrayOfTriangles.hxx>
#include <Graphic3d_ArrayOfSegments.hxx>
#include <Graphic3d_LOD.hxx>
#include <MeshVS_Drawer.hxx>
#include <MeshVS_DrawerAttribute.hxx>
#include <MeshVS_LODBuilder.hxx>
#include <MeshVS_MapOfTwoNodes.hxx>
#include <MeshVS_MeshPrsBuilder.hxx>
#include <MeshVS_SymmetricPairHasher.hxx>
#include <MeshVS_Tool.hxx>
#include <TColStd_HPackedMapOfInteger.hxx>
#include <TColStd_MapIteratorOfPackedMapOfInteger.hxx>

IMPLEMENT_STANDARD_RTTIEXT (MeshVS_LODBuilder, MeshVS_PrsBuilder)

//=======================================================================
// function : Creation
// purpose  :
//=======================================================================
MeshVS_LODBuilder::MeshVS_LODBuilder (const Handle(MeshVS_Mesh)& theParentMesh,
                                      const MeshVS_DisplayModeFlags& theFlags,
                                      const Handle(MeshVS_LODDataSource)& theDataSource,
                                      const Standard_Integer theId,
                                      const MeshVS_BuilderPriority& thePriority)
  : MeshVS_PrsBuilder (theParentMesh, theFlags, theDataSource, theId, thePriority)
{}

//=======================================================================
// function : Build
// purpose  :
//=======================================================================
void MeshVS_LODBuilder::Build (const Handle(Prs3d_Presentation)& theBasePrs,
                               const TColStd_PackedMapOfInteger& theIDs,
                               TColStd_PackedMapOfInteger& /*theIDsToExclude*/,
                               const Standard_Boolean theIsElement,
                               const Standard_Integer theDisplayMode) const
{
  if (myParentMesh == NULL)
    return;

  if (theIsElement)
  {
    Standard_Integer aMaxNodesNb;

    Handle(MeshVS_MeshPrsBuilder) aBuilder = Handle(MeshVS_MeshPrsBuilder)::DownCast (myParentMesh->GetBuilder (1));
    if (aBuilder.IsNull())
      return;
    Handle(MeshVS_Drawer) aDrawer = aBuilder->GetDrawer();
    if (aDrawer.IsNull() ||
        !aDrawer->GetInteger (MeshVS_DA_MaxFaceNodes, aMaxNodesNb) ||
        aMaxNodesNb <= 0)
      return;

    //----------- extract useful display mode flags ----------
    Standard_Integer aDispStatus = (theDisplayMode & aBuilder->GetFlags());
    if ((aDispStatus & MeshVS_DMF_DeformedMask) != 0)
    {
      aDispStatus /= MeshVS_DMF_DeformedPrsWireFrame;
      // This transformation turns deformed mesh flags to real display modes
    }
    aDispStatus &= MeshVS_DMF_OCCMask;
    //--------------------------------------------------------

    Standard_Real aShrinkCoef;
    aDrawer->GetDouble (MeshVS_DA_ShrinkCoeff, aShrinkCoef);

    const Standard_Boolean isWireframe = theDisplayMode == MeshVS_DMF_WireFrame;
    Standard_Boolean isShading = theDisplayMode == MeshVS_DMF_Shading;
    Standard_Boolean isShrink = theDisplayMode == MeshVS_DMF_Shrink;
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
    anIDs.Assign (theIDs);
    Handle(TColStd_HPackedMapOfInteger) aHiddenElems = myParentMesh->GetHiddenElems();
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

      if (!DataSource()->GetGeom (anIdxIter.Key(), Standard_True, aCoords, aNbNodes, aType))
        continue;

      if (aType == MeshVS_ET_Volume)
      {
        if (DataSource()->Get3DGeom (anIdxIter.Key(), aNbNodes, aTopo))
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
        if (DataSource()->GetGeomType (anIdxIter.Key(), Standard_True, aType) && aType == MeshVS_ET_Link)
        {
          Standard_Integer aNbNodes;

          if (DataSource()->GetNodesByElement (anIdxIter.Key(), aNodes, aNbNodes) && aNbNodes == 2)
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
      if (!DataSource()->GetGeom (aKey, Standard_True, aCoords, NbNodes, aType))
        continue;

      if (!DataSource()->GetNodesByElement (aKey, aNodes, NbNodes))
        continue;

      switch (aType)
      {
      case MeshVS_ET_Volume:
      {
        if (DataSource()->Get3DGeom (aKey, NbNodes, aTopo))
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
            aBuilder->AddFaceSolidPrs (DataSource(), aKey, aCoords, NbNodes, aMaxNodesNb, aFaceTriangles, isReflect,
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
    drawArrays (theBasePrs, aFaceTriangles, aEdgeSegments, aLinkSegments, aVolmTriangles,
      !toShowEdges, hasSelFlag, isSupressBackFaces, aFill, aBeam);
  }
}

#include <Prs3d_Root.hxx>

//================================================================
// Function : drawArrays
// Purpose  :
//================================================================
void MeshVS_LODBuilder::drawArrays (const Handle(Prs3d_Presentation)& theBasePrs,
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
    //Prs3d_Root::NewGroup ( theBasePrs );
    //Handle (Graphic3d_Group) aGroup = Prs3d_Root::CurrentGroup ( theBasePrs );

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
    //Prs3d_Root::NewGroup ( theBasePrs );
    //Handle (Graphic3d_Group) aLGroup = Prs3d_Root::CurrentGroup ( theBasePrs );

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
    //Prs3d_Root::NewGroup ( theBasePrs );
    //Handle (Graphic3d_Group) aBeamGroup = Prs3d_Root::CurrentGroup ( theBasePrs );

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
    //Prs3d_Root::NewGroup ( theBasePrs );
    //Handle (Graphic3d_Group) aGroup = Prs3d_Root::CurrentGroup ( theBasePrs );

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
