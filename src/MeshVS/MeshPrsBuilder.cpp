// Copyright (c) 2015 OPEN CASCADE SAS
//
// This file is part of commercial software by OPEN CASCADE SAS.
//
// This software is furnished in accordance with the terms and conditions
// of the contract and with the inclusion of this copyright notice.
// This software or any other copy thereof may not be provided or otherwise
// be made available to any third party.
// No ownership title to the software is transferred hereby.
//
// OPEN CASCADE SAS makes no representation or warranties with respect to the
// performance of this software, and specifically disclaims any responsibility
// for any damages, special or consequential, connected with its use.

#ifdef __APPLE__
  #import <TargetConditionals.h>
#endif

#include "MeshPrsBuilder.h"

#include <Aspect_TypeOfLine.hxx>
#include <MeshVS_Buffer.hxx>
#include <MeshVS_DataSource.hxx>
#include <MeshVS_Drawer.hxx>
#include <MeshVS_DrawerAttribute.hxx>
#include <MeshVS_EntityType.hxx>
#include <MeshVS_Mesh.hxx>
#include <MeshVS_Tool.hxx>
#include <Message.hxx>
#include <Message_Messenger.hxx>
#include <Graphic3d_ArrayOfTriangles.hxx>
#include <Graphic3d_ArrayOfSegments.hxx>
#include <Graphic3d_AspectFillArea3d.hxx>
#include <Graphic3d_AspectLine3d.hxx>
#include <Graphic3d_Texture2Dmanual.hxx>
#include <Image_PixMap.hxx>
#include <Prs3d_Drawer.hxx>
#include <Prs3d_ShadingAspect.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <TColStd_HPackedMapOfInteger.hxx>
#include <TColStd_MapIteratorOfPackedMapOfInteger.hxx>

static const int THE_NB_COLORS = 128;

//! Auxiliary wrapper
class MeshTexture : public Graphic3d_Texture2Dmanual
{
public:

  //! Main constructor.
  MeshTexture (const TCollection_AsciiString& theFileName)
  : Graphic3d_Texture2Dmanual (theFileName), myPathOrig (theFileName) {}

  //! Image reader.
  virtual Handle(Image_PixMap) GetImage() const Standard_OVERRIDE
  {
  #if (defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE) || defined(__ANDROID__)
    // mobile version is build without FreeImage to reduce binaries size - use Qt instead
    Handle(PixMapQt) anImage = new PixMapQt();
    if(!anImage->Load (myPathOrig))
    {
      Message::DefaultMessenger()->Send (TCollection_AsciiString ("Can not read image ") + myPathOrig + "!", Message_Fail);
      return Handle(Image_PixMap)();
    }
    return anImage;
  #else
    return Graphic3d_Texture2Dmanual::GetImage();
  #endif
  }

private:

  TCollection_AsciiString myPathOrig;

public:

  DEFINE_STANDARD_RTTIEXT(MeshTexture, Graphic3d_Texture2Dmanual)

};

DEFINE_STANDARD_HANDLE(MeshTexture, Graphic3d_Texture2Dmanual)

IMPLEMENT_STANDARD_RTTIEXT(MeshPrsTexture, Graphic3d_Texture2D)
IMPLEMENT_STANDARD_RTTIEXT(MeshTexture, Graphic3d_Texture2Dmanual)
IMPLEMENT_STANDARD_RTTIEXT(MeshPrsBuilder, MeshVS_PrsBuilder)

//================================================================
// Function : MeshPrsBuilder
// Purpose  :
//================================================================
MeshPrsBuilder::MeshPrsBuilder (const Handle(MeshVS_Mesh)&        theMesh,
                                const MeshVS_DisplayModeFlags&    theFlags)
: MeshVS_PrsBuilder (theMesh, theFlags, Handle(MeshVS_DataSource)(), -1, MeshVS_BP_NodalColor),
  myIsElemental (Standard_False),
  myIsGrayscale (Standard_False),
  myRangeFrom   (0.0),
  myRangeTo     (1.0),
  myToMapTexture(Standard_False),
  myToFlipV     (Standard_False)
{
#if (defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE) || defined(__ANDROID__)
  myToFlipV = Standard_True;
#else
  myToFlipV = Standard_False;
#endif

  SetExcluding (Standard_True);
}

//================================================================
// Function : buildTextured
// Purpose  :
//================================================================
void MeshPrsBuilder::buildTextured (const Handle(Prs3d_Presentation)& thePrs,
                                    const TColStd_PackedMapOfInteger& theIDs,
                                    TColStd_PackedMapOfInteger&       theIDsToExclude) const

{
  myPrsGroup.Nullify();

  Handle(MeshDataSource) aSource = Handle(MeshDataSource)::DownCast (GetDataSource());
  Handle(MeshVS_Drawer)  aDrawer = GetDrawer();
  if (aSource.IsNull()
   || aDrawer.IsNull())
  {
    return;
  }

  // subtract the hidden elements and ids to exclude (to minimize allocated memory)
  TColStd_PackedMapOfInteger anIDs;
  anIDs.Assign (theIDs);
  Handle(TColStd_HPackedMapOfInteger) aHiddenElems = myParentMesh->GetHiddenElems();
  if (!aHiddenElems.IsNull())
  {
    anIDs.Subtract (aHiddenElems->Map());
  }
  anIDs.Subtract (theIDsToExclude);

  Standard_Boolean hasReflection   = Standard_True;
  Standard_Boolean isSmoothShading = Standard_False;
  aDrawer->GetBoolean (MeshVS_DA_Reflection,    hasReflection);
  aDrawer->GetBoolean (MeshVS_DA_SmoothShading, isSmoothShading);

  MeshGroup aFullGroup;
  aFullGroup.NodeLower = aSource->Nodes().Lower();
  aFullGroup.NodeUpper = aSource->Nodes().Upper();
  aFullGroup.ElemLower = aSource->Elements().Lower();
  aFullGroup.ElemUpper = aSource->Elements().Upper();
  for (Standard_Integer aGroupIter = 0; aGroupIter == 0 || aGroupIter <= aSource->Groups().Upper(); ++aGroupIter)
  {
    const MeshGroup& aGroup = !aSource->Groups().IsEmpty() ? aSource->Groups().Value (aGroupIter) : aFullGroup;
    const Standard_Integer aNbNodes = aGroup.NodeUpper - aGroup.NodeLower + 1;
    if (aGroup.NodeLower < 0
     || aGroup.NodeUpper < 0
     || aGroup.ElemLower < 0
     || aGroup.ElemUpper < 0
     || aNbNodes < 1)
    {
      continue;
    }

    Handle(MeshTexture) aTexture;
    if (!aGroup.Material.Texture.IsEmpty()
     && myToMapTexture
     && aSource->HasUV())
    {
      aTexture = new MeshTexture (aGroup.Material.Texture);
    }

    Standard_Integer aNbTris = 0;
    for (Standard_Integer anElemIter = aGroup.ElemLower; anElemIter <= aGroup.ElemUpper; ++anElemIter)
    {
      if (!anIDs.Contains (anElemIter)) { continue; }
      const Graphic3d_Vec4i& anElem = aSource->Elements().Value (anElemIter);
      ++aNbTris;
      if (anElem[3] != -1)
      {
        ++aNbTris;
      }
    }
    if (aNbTris < 1)
    {
      continue;
    }

    Handle(Graphic3d_ArrayOfTriangles) aTriangles = new Graphic3d_ArrayOfTriangles (aNbNodes, aNbTris * 3,
                                                                                    aSource->HasNormals() || isSmoothShading, Standard_False, !aTexture.IsNull());
    // fill nodes
    if (!aTexture.IsNull())
    {
      if (aSource->HasNormals())
      {
        for (Standard_Integer aNodeIter = aGroup.NodeLower; aNodeIter <= aGroup.NodeUpper; ++aNodeIter)
        {
          const Graphic3d_Vec3& aNode = aSource->Nodes()  .Value (aNodeIter);
          const Graphic3d_Vec3& aNorm = aSource->Normals().Value (aNodeIter);
          const Graphic3d_Vec2& anUV  = aSource->NodesUV().Value (aNodeIter);
          const float aV = myToFlipV ? (1.0f - anUV.y()) : anUV.y();
          aTriangles->AddVertex (aNode.x(), aNode.y(), aNode.z(),
                                 aNorm.x(), aNorm.y(), aNorm.z(),
                                 anUV.x(), aV);
        }
      }
      else
      {
        for (Standard_Integer aNodeIter = aGroup.NodeLower; aNodeIter <= aGroup.NodeUpper; ++aNodeIter)
        {
          const Graphic3d_Vec3& aNode = aSource->Nodes()  .Value (aNodeIter);
          const Graphic3d_Vec2& anUV  = aSource->NodesUV().Value (aNodeIter);
          const float aV = myToFlipV ? (1.0f - anUV.y()) : anUV.y();
          aTriangles->AddVertex (aNode.x(), aNode.y(), aNode.z(),
                                 anUV.x(), aV);
        }
      }
    }
    else
    {
      if (aSource->HasNormals())
      {
        for (Standard_Integer aNodeIter = aGroup.NodeLower; aNodeIter <= aGroup.NodeUpper; ++aNodeIter)
        {
          const Graphic3d_Vec3& aNode = aSource->Nodes()  .Value (aNodeIter);
          const Graphic3d_Vec3& aNorm = aSource->Normals().Value (aNodeIter);
          aTriangles->AddVertex (aNode.x(), aNode.y(), aNode.z(),
                                 aNorm.x(), aNorm.y(), aNorm.z());
        }
      }
      else
      {
        for (Standard_Integer aNodeIter = aGroup.NodeLower; aNodeIter <= aGroup.NodeUpper; ++aNodeIter)
        {
          const Graphic3d_Vec3& aNode = aSource->Nodes().Value (aNodeIter);
          aTriangles->AddVertex (aNode.x(), aNode.y(), aNode.z());
        }
      }
    }

    // fill indices
    for (Standard_Integer anElemIter = aGroup.ElemLower; anElemIter <= aGroup.ElemUpper; ++anElemIter)
    {
      if (!anIDs.Contains (anElemIter)) { continue; }
      const Graphic3d_Vec4i& anElem = aSource->Elements().Value (anElemIter);
      aTriangles->AddEdge (anElem[0] + 1 - aGroup.NodeLower);
      aTriangles->AddEdge (anElem[1] + 1 - aGroup.NodeLower);
      aTriangles->AddEdge (anElem[2] + 1 - aGroup.NodeLower);
      if (anElem[3] != -1)
      {
        aTriangles->AddEdge (anElem[0] + 1 - aGroup.NodeLower);
        aTriangles->AddEdge (anElem[2] + 1 - aGroup.NodeLower);
        aTriangles->AddEdge (anElem[3] + 1 - aGroup.NodeLower);
      }
    }

    // reconstruct normals
    if (!aSource->HasNormals() && isSmoothShading)
    {
      // compute normal at each node, taking into account size of each element (e.g. without pre-normalization)
      for (Standard_Integer anElemIter = aGroup.ElemLower; anElemIter <= aGroup.ElemUpper; ++anElemIter)
      {
        if (!anIDs.Contains (anElemIter)) { continue; }
        const Graphic3d_Vec4i& anElem = aSource->Elements().Value (anElemIter);

        const Graphic3d_Vec3& aNode0 = aSource->Nodes().Value (anElem[0]);
        const Graphic3d_Vec3& aNode1 = aSource->Nodes().Value (anElem[1]);
        const Graphic3d_Vec3& aNode2 = aSource->Nodes().Value (anElem[2]);

        const Graphic3d_Vec3  aVec01 = aNode1 - aNode0;
        const Graphic3d_Vec3  aVec02 = aNode2 - aNode0;
        const Graphic3d_Vec3  aCross = Graphic3d_Vec3::Cross (aVec01, aVec02);
        const Graphic3d_Vec3d aTriNorm (aCross.x(), aCross.y(), aCross.z());
        const Standard_Integer aNbElemNodes = anElem[3] != -1 ? 4 : 3;
        Graphic3d_Vec3d aNorm;
        for (Standard_Integer aNodeIter = 0; aNodeIter < aNbElemNodes; ++aNodeIter)
        {
          // Graphic3d_ArrayOfPrimitives zeroes values in costructor
          const Standard_Integer aNodeIndex = anElem[aNodeIter] + 1 - aGroup.NodeLower;
          aTriangles->VertexNormal (aNodeIndex, aNorm.x(), aNorm.y(), aNorm.z());
          aNorm += aTriNorm;
          aTriangles->SetVertexNormal (aNodeIndex, aNorm.x(), aNorm.y(), aNorm.z());
        }
      }

      // normalize
      for (Standard_Integer anElemIter = aGroup.ElemLower; anElemIter <= aGroup.ElemUpper; ++anElemIter)
      {
        if (!anIDs.Contains (anElemIter)) { continue; }
        const Graphic3d_Vec4i& anElem = aSource->Elements().Value (anElemIter);
        const Standard_Integer aNbElemNodes = anElem[3] != -1 ? 4 : 3;
        Graphic3d_Vec3d aNorm;
        for (Standard_Integer aNodeIter = 0; aNodeIter < aNbElemNodes; ++aNodeIter)
        {
          const Standard_Integer aNodeIndex = anElem[aNodeIter] + 1 - aGroup.NodeLower;
          aTriangles->VertexNormal (aNodeIndex, aNorm.x(), aNorm.y(), aNorm.z());
          aNorm.Normalize();
          aTriangles->SetVertexNormal (aNodeIndex, aNorm.x(), aNorm.y(), aNorm.z());
        }
      }
    }

    Graphic3d_MaterialAspect aMat[2];
    aDrawer->GetMaterial(MeshVS_DA_FrontMaterial, aMat[0]);
    aDrawer->GetMaterial(MeshVS_DA_BackMaterial,  aMat[1]);
    if (aMat[0].Name()                == Graphic3d_NOM_DEFAULT
     && aGroup.Material.Aspect.Name() != Graphic3d_NOM_DEFAULT)
    {
      aMat[0] = aGroup.Material.Aspect;
      aMat[1] = aGroup.Material.Aspect;
    }
    if (!hasReflection)
    {
      aMat[0].SetReflectionModeOff (Graphic3d_TOR_AMBIENT);
      aMat[0].SetReflectionModeOff (Graphic3d_TOR_DIFFUSE);
      aMat[0].SetReflectionModeOff (Graphic3d_TOR_SPECULAR);
      aMat[0].SetReflectionModeOff (Graphic3d_TOR_EMISSION);
    }

    Handle(Graphic3d_AspectFillArea3d) anAreaAsp = MeshVS_Tool::CreateAspectFillArea3d (aDrawer, aMat[0]);
    if (!aTexture.IsNull())
    {
      anAreaAsp->SetTextureMap (aTexture);
      anAreaAsp->SetTextureMapOn();
    }

    Handle(Graphic3d_Group) aPrsGroup = thePrs->NewGroup();
    aPrsGroup->SetGroupPrimitivesAspect (anAreaAsp);
    aPrsGroup->AddPrimitiveArray (aTriangles);
  }
}

//================================================================
// Function : Build
// Purpose  :
//================================================================
void MeshPrsBuilder::Build (const Handle(Prs3d_Presentation)& thePrs,
                            const TColStd_PackedMapOfInteger& theIDs,
                            TColStd_PackedMapOfInteger&       theIDsToExclude,
                            const Standard_Boolean            theIsElement,
                            const Standard_Integer            theDisplayMode) const
{
  myPrsGroup.Nullify();
  if (myProperty.IsNull())
  {
    buildTextured (thePrs, theIDs, theIDsToExclude);
    return;
  }

  Handle(MeshVS_DataSource) aSource = GetDataSource();
  Handle(MeshVS_Drawer)     aDrawer = GetDrawer();
  if (aSource.IsNull()
   || aDrawer.IsNull()
   || myProperty.IsNull())
  {
    return;
  }

  Standard_Integer aMaxFaceNodes = 0;
  if (!aDrawer->GetInteger (MeshVS_DA_MaxFaceNodes, aMaxFaceNodes)
    || aMaxFaceNodes <= 0 )
  {
    return;
  }

  MeshVS_Buffer aCoordsBuf (3 * aMaxFaceNodes * sizeof(Standard_Real));
  TColStd_Array1OfReal aCoords (aCoordsBuf, 1, 3 * aMaxFaceNodes);
  if ((theDisplayMode & GetFlags()) == 0
   || !theIsElement)
  {
    return;
  }

  const Standard_Real aLowerValue  = propertyLowerValue();
  const Standard_Real anUpperValue = propertyUpperValue();
  const Standard_Real aRange       = anUpperValue - aLowerValue;

  // subtract the hidden elements and ids to exclude (to minimize allocated memory)
  TColStd_PackedMapOfInteger anIDs;
  anIDs.Assign (theIDs);
  Handle(TColStd_HPackedMapOfInteger) aHiddenElems = myParentMesh->GetHiddenElems();
  if (!aHiddenElems.IsNull())
  {
    anIDs.Subtract (aHiddenElems->Map());
  }
  anIDs.Subtract (theIDsToExclude);

  Standard_Boolean isReflect = Standard_False, isMeshSmoothShading = Standard_False;
  aDrawer->GetBoolean (MeshVS_DA_ColorReflection, isReflect);
  aDrawer->GetBoolean (MeshVS_DA_SmoothShading,   isMeshSmoothShading);

  // Following parameter are used for texture presentation only
  int nbColors        = THE_NB_COLORS;///myTextureColorMap.Length(); // Number of colors from color map
  int nbTextureColors = nbColors;     // Number of colors in texture (it will be pow of 2)

  Graphic3d_MaterialAspect aMaterials[2] = 
  {
    Graphic3d_MaterialAspect (Graphic3d_NOM_PLASTIC),
    Graphic3d_MaterialAspect (Graphic3d_NOM_PLASTIC)
  };
  for (Standard_Integer aMatIter = 0; aMatIter < 2; ++aMatIter)
  {
    Graphic3d_MaterialAspect& aMat = aMaterials[aMatIter];
    if (!isReflect)
    {
      aMat.SetReflectionModeOff (Graphic3d_TOR_SPECULAR);
      aMat.SetReflectionModeOff (Graphic3d_TOR_AMBIENT);
      aMat.SetReflectionModeOff (Graphic3d_TOR_DIFFUSE);
      aMat.SetReflectionModeOff (Graphic3d_TOR_EMISSION);
    }
    else
    {
      aMat.SetAmbient (0.5);
      aMat.SetDiffuse (0.5);
      aMat.SetSpecular(0.0);
      aMat.SetEmissive(0.0);
    }
  }

  Standard_Integer aNbFacePrimitives = 0;
  Standard_Integer aNbEdgePrimitives = 0;
  for (TColStd_MapIteratorOfPackedMapOfInteger it (anIDs); it.More(); it.Next())
  {
    MeshVS_EntityType aType    = MeshVS_ET_NONE;
    Standard_Integer  aNbNodes = 0;
    if (!aSource->GetGeom (it.Key(), Standard_True, aCoords, aNbNodes, aType)
      || aType != MeshVS_ET_Face)
    {
      continue;
    }

    aNbEdgePrimitives += aNbNodes;     // add edge segments
    aNbFacePrimitives += aNbNodes - 2; // add face triangles
  }

  Standard_Boolean toShowEdges = Standard_True;
  aDrawer->GetBoolean (MeshVS_DA_ShowEdges, toShowEdges);

  // Here we do not use indices arrays because they are not effective for some mesh
  // drawing modes: shrinking mode (displaces the vertices inside the polygon), 3D
  // cell rendering (normal interpolation is not always applicable - flat shading),
  // elemental coloring (color interpolation is impossible)

  Handle(Graphic3d_ArrayOfTriangles) aFaceTriangles = new Graphic3d_ArrayOfTriangles (aNbFacePrimitives * 3, 0,
                                                                                      Standard_True, Standard_False, Standard_True);
  Handle(Graphic3d_ArrayOfSegments) anEdgeSegments;
  if (toShowEdges)
  {
    anEdgeSegments = new Graphic3d_ArrayOfSegments (aNbEdgePrimitives * 2);
  }

  gp_Pnt P, Start;
  Standard_Real aMin = gp::Resolution() * gp::Resolution();
  gp_Dir aDefNorm = gp::DZ();

  TColStd_Array1OfInteger aNodes (1, 10);
  for (TColStd_MapIteratorOfPackedMapOfInteger it (anIDs); it.More(); it.Next())
  {
    Standard_Integer  aKey     = it.Key();
    Standard_Integer  aNbNodes = 0;
    MeshVS_EntityType aType    = MeshVS_ET_NONE;
    if (!aSource->GetGeom (aKey, Standard_True, aCoords, aNbNodes, aType)
     ||  aType != MeshVS_ET_Face
     ||  aNbNodes > 10
     || !aSource->GetNodesByElement (aKey, aNodes, aNbNodes))
    {
      continue;
    }

    // Preparing normal(s) to show reflections if requested
    Handle(TColStd_HArray1OfReal) aNormals;
    Standard_Boolean hasNormals = isReflect
                               && aSource->GetNormalsByElement (aKey, isMeshSmoothShading, aMaxFaceNodes, aNormals);

    for (Standard_Integer aNodeIdx = 0; aNodeIdx < aNbNodes - 2; ++aNodeIdx)
    {
      for (Standard_Integer aSubIdx = 0; aSubIdx < 3; ++aSubIdx)
      {
        gp_XYZ aPnt (aCoords (3 * (aSubIdx == 0 ? 0 : (aNodeIdx + aSubIdx)) + 1),
                     aCoords (3 * (aSubIdx == 0 ? 0 : (aNodeIdx + aSubIdx)) + 2),
                     aCoords (3 * (aSubIdx == 0 ? 0 : (aNodeIdx + aSubIdx)) + 3));

        gp_Vec aNorm = aDefNorm;
        if (hasNormals)
        {
          gp_Vec aTestNorm (aNormals->Value (3 * (aSubIdx == 0 ? 0 : (aNodeIdx + aSubIdx)) + 1),
                            aNormals->Value (3 * (aSubIdx == 0 ? 0 : (aNodeIdx + aSubIdx)) + 2),
                            aNormals->Value (3 * (aSubIdx == 0 ? 0 : (aNodeIdx + aSubIdx)) + 3));

          if (aTestNorm.SquareMagnitude() > aMin)
          {
            aNorm = gp_Dir (aTestNorm);
          }
        }

        // retrieve nodal value
        Standard_Integer aNodeIndex = aKey;
        if (!myIsElemental)
        {
          aNodeIndex = aNodes (aSubIdx == 0 ? 1 : (aNodeIdx + aSubIdx + 1));
        }

        Standard_Real aTexCoordU = 0.25;
        Standard_Real aTexCoordV = 0.25;
        const Standard_Real aValue = myProperty->Value (aNodeIndex);
        if (aRange > gp::Resolution()
        && !IsNaN (aValue))
        {
          const Standard_Real aNormVal = (aValue - aLowerValue) / aRange;
          aTexCoordU = (aNormVal * (nbColors - 1.0) + 0.5) / nbTextureColors;
          aTexCoordV = 0.75;
        }

        // Transform texture coordinate in accordance with number of colors specified
        // by upper level and real size of OpenGL texture. The OpenGL texture has border
        // colors interpolated with the colors from the color map, thats why we need to
        // shrink texture coordinates around the middle point to exclude areas where the
        // map colors are interpolated with the borders color
        aFaceTriangles->AddVertex (aPnt, aNorm, gp_Pnt2d (aTexCoordU, aTexCoordV));
      }
    }

    if (!anEdgeSegments.IsNull())
    {
      for (Standard_Integer aNodeIdx = 0; aNodeIdx < aNbNodes; ++aNodeIdx) // border segmentation
      {
        const Standard_Integer aNextIdx = (aNodeIdx + 1) % aNbNodes;

        anEdgeSegments->AddVertex (aCoords (3 * aNodeIdx + 1),
                                   aCoords (3 * aNodeIdx + 2),
                                   aCoords (3 * aNodeIdx + 3));

        anEdgeSegments->AddVertex (aCoords (3 * aNextIdx + 1),
                                   aCoords (3 * aNextIdx + 2),
                                   aCoords (3 * aNextIdx + 3));
      }
    }

    // if IsExcludingOn then presentation must not be built by other builders
    if (IsExcludingOn())
    {
      theIDsToExclude.Add (aKey);
    }
  }

  Aspect_TypeOfLine anEdgeType = Aspect_TOL_SOLID;
  Standard_Integer  anEdgeInt;
  Standard_Real     anEdgeWidth;
  Quantity_Color    anInteriorColor, anEdgeColor, aLineColor;

  aDrawer->GetColor  (MeshVS_DA_InteriorColor, anInteriorColor);
  aDrawer->GetColor  (MeshVS_DA_EdgeColor,     anEdgeColor);
  aDrawer->GetColor  (MeshVS_DA_BeamColor,     aLineColor);
  aDrawer->GetDouble (MeshVS_DA_EdgeWidth,     anEdgeWidth);

  if (aDrawer->GetInteger (MeshVS_DA_EdgeType, anEdgeInt))
  {
    anEdgeType = (Aspect_TypeOfLine )anEdgeInt;
  }

  Handle(Prs3d_Drawer) aPrs3dDrawer = myParentMesh->Attributes();
  if (aPrs3dDrawer.IsNull())
  {
    return;
  }

  aPrs3dDrawer->SetShadingAspect (new Prs3d_ShadingAspect());

  myFillAsp  = new Graphic3d_AspectFillArea3d();
  *myFillAsp = *aPrs3dDrawer->ShadingAspect()->Aspect();

  myFillAsp->SetFrontMaterial(aMaterials[0]);
  myFillAsp->SetBackMaterial (aMaterials[1]);

  Handle(Graphic3d_Texture2D) aTexture = createTexture (myIsGrayscale, myRangeFrom, myRangeTo, myProperty->IsInversed());
  if (aTexture.IsNull())
  {
    return;
  }

  myFillAsp->SetTextureMapOn();
  myFillAsp->SetTextureMap (aTexture);
  myFillAsp->SetInteriorColor (Quantity_NOC_WHITE);

  myFillAsp->SetDistinguishOff();
  myFillAsp->SetEdgeOff();

  Handle(Graphic3d_Group) aGroup1 = thePrs->NewGroup();
  aGroup1->SetGroupPrimitivesAspect (myFillAsp);
  aGroup1->AddPrimitiveArray (aFaceTriangles);

  myPrsGroup = aGroup1;

  if (toShowEdges)
  {
    Handle(Graphic3d_Group) aGroup2 = thePrs->NewGroup();

    Handle(Graphic3d_AspectLine3d) anLAsp = new Graphic3d_AspectLine3d (anEdgeColor, anEdgeType, anEdgeWidth);
    aGroup2->SetGroupPrimitivesAspect (anLAsp);
    aGroup2->AddPrimitiveArray (anEdgeSegments);
  }
}

//================================================================
// Function : SetProperty
// Purpose  :
//================================================================
void MeshPrsBuilder::SetProperty (const Handle(MeshScalarProperty)& theProperty,
                                  const Standard_Boolean            theIsElemental)
{
  myProperty    = theProperty;
  myIsElemental = theIsElemental;
  if (!myProperty.IsNull())
  {
    myRangeFrom = myProperty->ColorscaleRangeFrom();
    myRangeTo   = myProperty->ColorscaleRangeTo();
  }
}

//================================================================
// Function : SetRange
// Purpose  :
//================================================================
void MeshPrsBuilder::SetRange (const Standard_Real theFrom,
                               const Standard_Real theTo,
                               const bool          theIsInversed)
{
  Standard_Real aPrec = 1.0 / Standard_Real(THE_NB_COLORS * 10);
  if (Abs (myRangeFrom - theFrom) < aPrec
   && Abs (myRangeTo   - theTo)   < aPrec
   && (myProperty.IsNull() || myProperty->IsInversed() == theIsInversed))
  {
    return;
  }

  myRangeFrom = theFrom;
  myRangeTo   = theTo;
  if (!myProperty.IsNull())
  {
    myProperty->SetInversed (theIsInversed);
    myProperty->SetColorscaleRange (theFrom, theTo);
  }
  if (myPrsGroup.IsNull())
  {
    return;
  }

  Handle(Graphic3d_Texture2D) aTexture = createTexture (myIsGrayscale, myRangeFrom, myRangeTo, theIsInversed);
  if (aTexture.IsNull())
  {
    return;
  }

  myFillAsp->SetTextureMap (aTexture);
  myPrsGroup->SetGroupPrimitivesAspect (myFillAsp);
}

//================================================================
// Function : createTexture
// Purpose  :
//================================================================
Handle(Graphic3d_Texture2D) MeshPrsBuilder::createTexture (const Standard_Boolean theIsGrayscale,
                                                           const Standard_Real    theFrom,
                                                           const Standard_Real    theTo,
                                                           const bool             theIsInversed)
{
  const Standard_Integer aNbColors = THE_NB_COLORS;

  const Graphic3d_Vec3d anOut = Graphic3d_Vec3d (0.5, 0.5, 0.5);
  Graphic3d_Vec3d aScale00 = theIsGrayscale ? Graphic3d_Vec3d (0.0, 0.0, 0.0) : Graphic3d_Vec3d (0.0, 0.0, 1.0);
  Graphic3d_Vec3d aScale05 = theIsGrayscale ? Graphic3d_Vec3d (0.5, 0.5, 0.5) : Graphic3d_Vec3d (0.0, 1.0, 0.0);
  Graphic3d_Vec3d aScale10 = theIsGrayscale ? Graphic3d_Vec3d (1.0, 1.0, 1.0) : Graphic3d_Vec3d (1.0, 0.0, 0.0);
  if (theIsInversed)
  {
    std::swap (aScale00, aScale10);
  }

  const Standard_Real aRange    = Max (theTo - theFrom, 0.01);
  const Standard_Real aRangeInv = 1.0 / aRange;
  const Standard_Real aMiddle   = theFrom + 0.5 * aRange;
  const Standard_Real aFrom     = theFrom;

  // create and fill image with colors
  Handle(Image_PixMap) anImage = new Image_PixMap();
  if (!anImage->InitTrash (Image_PixMap::ImgRGBA, aNbColors, 2))
  {
    return NULL;
  }

  Image_ColorRGBA anOutColor;
  anOutColor.r() = Standard_Byte(255.0 * anOut.r());
  anOutColor.g() = Standard_Byte(255.0 * anOut.g());
  anOutColor.b() = Standard_Byte(255.0 * anOut.b());
  anOutColor.a() = 0xFF;

  anImage->SetTopDown (false);
  for (Standard_Size aColIter = 0; aColIter < Standard_Size(aNbColors); ++aColIter)
  {
    const Standard_Real aValue = Standard_Real(aColIter) / Standard_Real(aNbColors - 1);
    Graphic3d_Vec3d aColorVal;
    if (aValue < theFrom
     || aValue > theTo)
    {
      aColorVal = anOut;
    }
    else if (aValue < aMiddle)
    {
      const Standard_Real aHalfVal = (aValue - aFrom) * 2.0 * aRangeInv;
      aColorVal = aScale00 * (1.0 - aHalfVal) + aScale05 * aHalfVal;
    }
    else
    {
      const Standard_Real aHalfVal = (aValue - aFrom) * 2.0 * aRangeInv - 1.0;
      aColorVal = aScale05 * (1.0 - aHalfVal) + aScale10 * aHalfVal;
    }

    Image_ColorRGBA& aColor0 = anImage->ChangeValue<Image_ColorRGBA> (0, aColIter);
    Image_ColorRGBA& aColor1 = anImage->ChangeValue<Image_ColorRGBA> (1, aColIter);
    aColor0.r() = Standard_Byte(255.0 * aColorVal.r());
    aColor0.g() = Standard_Byte(255.0 * aColorVal.g());
    aColor0.b() = Standard_Byte(255.0 * aColorVal.b());
    aColor0.a() = 0xFF;
    aColor1 = anOutColor;
  }

  // create texture
  return new MeshPrsTexture (anImage);
}
