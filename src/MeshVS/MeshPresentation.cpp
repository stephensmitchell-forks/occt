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

#include "MeshPresentation.h"

#include <MeshVS_Drawer.hxx>
#include <MeshVS_DrawerAttribute.hxx>
#include <MeshVS_Tool.hxx>

IMPLEMENT_STANDARD_RTTIEXT(MeshPresentation, MeshVS_Mesh)

// =======================================================================
// function : MeshPresentation
// purpose  :
// =======================================================================
MeshPresentation::MeshPresentation()
{}

// =======================================================================
// function : SetMaterial
// purpose  :
// =======================================================================
void MeshPresentation::SetMaterial (const Graphic3d_MaterialAspect& theMat)
{
  Handle(MeshVS_Drawer) aDrawer = GetDrawer();
  if (aDrawer.IsNull())
  {
    return;
  }

  aDrawer->SetMaterial (MeshVS_DA_FrontMaterial, theMat);
  aDrawer->SetMaterial (MeshVS_DA_BackMaterial,  theMat);
  Standard_Boolean hasReflection = Standard_True;
  aDrawer->GetBoolean (MeshVS_DA_Reflection, hasReflection);
  Graphic3d_MaterialAspect aMat = theMat;
  if (!hasReflection)
  {
    aMat.SetReflectionModeOff (Graphic3d_TOR_AMBIENT);
    aMat.SetReflectionModeOff (Graphic3d_TOR_DIFFUSE);
    aMat.SetReflectionModeOff (Graphic3d_TOR_SPECULAR);
    aMat.SetReflectionModeOff (Graphic3d_TOR_EMISSION);
  }
  Handle(Graphic3d_AspectFillArea3d) anAreaAsp    = MeshVS_Tool::CreateAspectFillArea3d (aDrawer, aMat);
  Handle(Graphic3d_AspectLine3d)     anOldLineAsp = new Graphic3d_AspectLine3d();
  Handle(Graphic3d_AspectText3d)     anOldTextAsp = new Graphic3d_AspectText3d();
  Handle(Graphic3d_AspectMarker3d)   anOldPntAsp  = new Graphic3d_AspectMarker3d();
  Handle(Graphic3d_AspectFillArea3d) anOldAreaAps = new Graphic3d_AspectFillArea3d();

  const PrsMgr_Presentations& aPrsList = Presentations();
  for (Standard_Integer aPrsIt = 1; aPrsIt <= aPrsList.Length(); ++aPrsIt)
  {
    const PrsMgr_ModedPresentation& aPrsModed = aPrsList.Value (aPrsIt);
    if (aPrsModed.Mode() == MeshVS_DMF_WireFrame
     || aPrsModed.Mode() == MeshVS_DMF_NodalColorDataPrs
     || aPrsModed.Mode() == MeshVS_DMF_ElementalColorDataPrs)
    {
      continue;
    }

    const Handle(Prs3d_Presentation)& aPrs = aPrsModed.Presentation()->Presentation();
    for (Graphic3d_SequenceOfGroup::Iterator aGroupIt (aPrs->Groups()); aGroupIt.More(); aGroupIt.Next())
    {
      const Handle(Graphic3d_Group)& aGroup = aGroupIt.Value();
      if (!aGroup->IsGroupPrimitivesAspectSet (Graphic3d_ASPECT_FILL_AREA))
      {
        continue;
      }

      aGroup->GroupPrimitivesAspect (anOldLineAsp, anOldTextAsp, anOldPntAsp, anOldAreaAps);
      anAreaAsp->SetTextureMap (anOldAreaAps->TextureMap());
      if (anOldAreaAps->TextureMapState())
      {
        anAreaAsp->SetTextureMapOn();
      }
      else
      {
        anAreaAsp->SetTextureMapOff();
      }

      aGroup->SetGroupPrimitivesAspect (anAreaAsp);
    }
  }
}

// =======================================================================
// function : SetClosed
// purpose  :
// =======================================================================
void MeshPresentation::SetClosed (const bool theIsClosed)
{
  const PrsMgr_Presentations& aPrsList = Presentations();
  for (Standard_Integer aPrsIt = 1; aPrsIt <= aPrsList.Length(); ++aPrsIt)
  {
    const PrsMgr_ModedPresentation& aPrsModed = aPrsList.Value (aPrsIt);
    if (aPrsModed.Mode() == MeshVS_DMF_WireFrame
     || aPrsModed.Mode() == MeshVS_DMF_Shrink)
    {
      continue;
    }

    const Handle(Prs3d_Presentation)& aPrs = aPrsModed.Presentation()->Presentation();
    for (Graphic3d_SequenceOfGroup::Iterator aGroupIt (aPrs->Groups()); aGroupIt.More(); aGroupIt.Next())
    {
      const Handle(Graphic3d_Group)& aGroup = aGroupIt.Value();
      if (aGroup->IsGroupPrimitivesAspectSet (Graphic3d_ASPECT_FILL_AREA))
      {
        aGroup->SetClosed (theIsClosed);
      }
    }
  }
}

// =======================================================================
// function : SetClosed
// purpose  :
// =======================================================================
void MeshPresentation::Compute (const Handle(PrsMgr_PresentationManager3d)& thePrsMgr,
                                const Handle(Prs3d_Presentation)&           thePrs,
                                const Standard_Integer                      theMode)
{
  MeshVS_Mesh::Compute (thePrsMgr, thePrs, theMode);

  // mark mesh as closed without checks for Capping algorithm but without suppressing back faces,
  // thus artifacts on shells will appear only with capping option turned on and not in normal mode
  SetClosed (true);
}
