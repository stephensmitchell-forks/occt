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

IMPLEMENT_STANDARD_RTTIEXT (MeshVS_LODBuilder, MeshVS_MeshPrsBuilder)

//=======================================================================
// function : Creation
// purpose  :
//=======================================================================
MeshVS_LODBuilder::MeshVS_LODBuilder (const Handle(MeshVS_Mesh)& theParentMesh,
                                      const MeshVS_DisplayModeFlags& theFlags,
                                      const Handle(MeshVS_LODDataSource)& theDataSource,
                                      const Standard_Integer theId,
                                      const MeshVS_BuilderPriority& thePriority)
  : MeshVS_MeshPrsBuilder (theParentMesh, theFlags, theDataSource, theId, thePriority)
{}

//================================================================
// Function : DrawArrays
// Purpose  :
//================================================================
void MeshVS_LODBuilder::DrawArrays (const Handle(Prs3d_Presentation)& theBasePrs,
                                    const Handle(Graphic3d_ArrayOfPrimitives)& thePolygons,
                                    const Handle(Graphic3d_ArrayOfPrimitives)& theLines,
                                    const Handle(Graphic3d_ArrayOfPrimitives)& theLinkLines,
                                    const Handle(Graphic3d_ArrayOfPrimitives)& theVolumesInShad,
                                    const Standard_Boolean theIsPolygonsEdgesOff,
                                    const Standard_Boolean theIsSelected,
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

  Standard_Boolean isSupressBackFaces = Standard_False;
  Handle(MeshVS_Drawer) aDrawer = GetDrawer();
  if (!aDrawer.IsNull())
  {
    aDrawer->GetBoolean (MeshVS_DA_SupressBackFaces, isSupressBackFaces);
  }

  Aspect_InteriorStyle aStyle;
  Quantity_Color anIntColor, aBackColor, anEdgeColor;
  Aspect_TypeOfLine aType;
  Standard_Real aWidth;

  Handle(Graphic3d_LOD) aNewLOD = theBasePrs->NewLOD();
  theFillAsp->Values (aStyle, anIntColor, aBackColor, anEdgeColor, aType, aWidth);

  if (isPolygons && theFillAsp->FrontMaterial().Transparency() < 0.01)
  {
    Handle (Graphic3d_Group) aGroup = aNewLOD->NewGroup (theBasePrs);

    theFillAsp->SetEdgeOff();

    if (anIntColor != aBackColor)
      theFillAsp->SetDistinguishOn();
    else
      theFillAsp->SetDistinguishOff();

    aGroup->SetClosed (isSupressBackFaces);
    Handle(Graphic3d_AspectFillArea3d) aFillAsp = new Graphic3d_AspectFillArea3d (*(theFillAsp.operator->()));
    if (isSupressBackFaces)
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
    Handle (Graphic3d_Group) aLGroup = aNewLOD->NewGroup (theBasePrs);

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
    Handle (Graphic3d_Group) aBeamGroup = aNewLOD->NewGroup (theBasePrs);

    theFillAsp->SetEdgeOff();
    if (!theIsSelected)
      aBeamGroup->SetPrimitivesAspect (theFillAsp);
    aBeamGroup->SetPrimitivesAspect (theLineAsp);
    aBeamGroup->AddPrimitiveArray (theLinkLines);
    theFillAsp->SetEdgeOn();
  }

  if (isPolygons && theFillAsp->FrontMaterial().Transparency() >= 0.01)
  {
    Handle (Graphic3d_Group) aGroup = aNewLOD->NewGroup (theBasePrs);

    theFillAsp->SetEdgeOff();

    if (anIntColor != aBackColor)
      theFillAsp->SetDistinguishOn();
    else
      theFillAsp->SetDistinguishOff();

    aGroup->SetClosed (isSupressBackFaces);
    Handle(Graphic3d_AspectFillArea3d) aFillAsp = new Graphic3d_AspectFillArea3d (*(theFillAsp.operator->()));
    if (isSupressBackFaces)
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
