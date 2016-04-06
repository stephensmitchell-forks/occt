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

#ifndef _MeshVS_LODBuilder_Header
#define _MeshVS_LODBuilder_Header

#include <MeshVS_Mesh.hxx>
#include <MeshVS_MeshPrsBuilder.hxx>
#include <MeshVS_LODDataSource.hxx>

//! The class is used to build graphic groups for one LOD of the presentation.
//! The only difference between usual MeshVS_MeshPrsBuilder is in approach to
//! filling the presentation: LOD groups are created instead of own groups of
//! the presentation.
class MeshVS_LODBuilder : public MeshVS_MeshPrsBuilder
{
public:
  
  //! Creates builder with certain display mode flags, data source, ID and priority
  Standard_EXPORT MeshVS_LODBuilder (const Handle(MeshVS_Mesh)& theParentMesh,
                                     const MeshVS_DisplayModeFlags& theFlags = MeshVS_DMF_OCCMask,
                                     const Handle(MeshVS_LODDataSource)& theDataSource = NULL,
                                     const Standard_Integer theId = -1,
                                     const MeshVS_BuilderPriority& thePriority = MeshVS_BP_Mesh);

  Standard_EXPORT virtual ~MeshVS_LODBuilder() {};

  DEFINE_STANDARD_RTTIEXT (MeshVS_LODBuilder, MeshVS_PrsBuilder)

protected:

  //! Creates LOD graphic groups, adds them to base presentation and fills according to the build
  //! parameters
  Standard_EXPORT virtual void DrawArrays (const Handle(Prs3d_Presentation)& theBasePrs,
                                           const Handle(Graphic3d_ArrayOfPrimitives)& thePolygons,
                                           const Handle(Graphic3d_ArrayOfPrimitives)& theLines,
                                           const Handle(Graphic3d_ArrayOfPrimitives)& theLinkLines,
                                           const Handle(Graphic3d_ArrayOfPrimitives)& theVolumesInShad,
                                           const Standard_Boolean theIsPolygonsEdgesOff,
                                           const Standard_Boolean theIsSelected,
                                           const Handle(Graphic3d_AspectFillArea3d)& theFillAsp,
                                           const Handle(Graphic3d_AspectLine3d)& theLineAsp) const Standard_OVERRIDE;
};

DEFINE_STANDARD_HANDLE (MeshVS_LODBuilder, MeshVS_MeshPrsBuilder)

#endif // _MeshVS_LODBuilder_Header
