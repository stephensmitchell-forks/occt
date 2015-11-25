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
#include <MeshVS_PrsBuilder.hxx>
#include <MeshVS_LODDataSource.hxx>

class MeshVS_LODBuilder : public MeshVS_PrsBuilder
{
public:
  
  //! Creates builder with certain display mode flags, data source, ID and priority
  Standard_EXPORT MeshVS_LODBuilder (const Handle(MeshVS_Mesh)& theParentMesh,
                                     const MeshVS_DisplayModeFlags& theFlags = MeshVS_DMF_OCCMask,
                                     const Handle(MeshVS_LODDataSource)& theDataSource = NULL,
                                     const Standard_Integer theId = -1,
                                     const MeshVS_BuilderPriority& thePriority = MeshVS_BP_Mesh);

  Standard_EXPORT virtual ~MeshVS_LODBuilder() {};

  //! Builds presentation of certain type of data.
  //! Prs is presentation object which this method constructs.
  //! IDs is set of numeric identificators forming object appearance.
  //! IDsToExclude is set of IDs to exclude from processing. If some entity
  //! has been excluded, it is not processed by other builders.
  //! IsElement indicates, IDs is identificators of nodes or elements.
  //! DisplayMode is numeric constant describing display mode (see MeshVS_DisplayModeFlags.hxx)
  Standard_EXPORT virtual void Build (const Handle(Prs3d_Presentation)& theBasePrs,
                                      const TColStd_PackedMapOfInteger& theIDs,
                                      TColStd_PackedMapOfInteger& theIDsToExclude,
                                      const Standard_Boolean theIsElement,
                                      const Standard_Integer theDisplayMode) const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTI (MeshVS_LODBuilder, MeshVS_PrsBuilder)

protected:
  void drawArrays (const Handle(Prs3d_Presentation)& theBasePrs,
                   const Handle(Graphic3d_ArrayOfPrimitives)& thePolygons,
                   const Handle(Graphic3d_ArrayOfPrimitives)& theLines,
                   const Handle(Graphic3d_ArrayOfPrimitives)& theLinkLines,
                   const Handle(Graphic3d_ArrayOfPrimitives)& theVolumesInShad,
                   const Standard_Boolean theIsPolygonsEdgesOff,
                   const Standard_Boolean theIsSelected,
                   const Standard_Boolean theIsSupressBackFaces,
                   const Handle(Graphic3d_AspectFillArea3d)& theFillAsp,
                   const Handle(Graphic3d_AspectLine3d)& theLineAsp) const;
};

DEFINE_STANDARD_HANDLE (MeshVS_LODBuilder, MeshVS_PrsBuilder)

#endif // _MeshVS_LODBuilder_Header
