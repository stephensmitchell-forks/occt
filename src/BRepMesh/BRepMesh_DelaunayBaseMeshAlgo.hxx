// Created on: 2016-07-07
// Copyright (c) 2016 OPEN CASCADE SAS
// Created by: Oleg AGASHIN
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

#ifndef _BRepMesh_DelaunayBaseMeshAlgo_HeaderFile
#define _BRepMesh_DelaunayBaseMeshAlgo_HeaderFile

#include <BRepMesh_BaseMeshAlgo.hxx>
#include <NCollection_Shared.hxx>
#include <IMeshTools_Parameters.hxx>

class BRepMesh_DataStructureOfDelaun;
class BRepMesh_Delaun;

//! Class provides base fuctionality to build face triangulation using Dealunay approach.
//! Performs generation of mesh using raw data from model.
class BRepMesh_DelaunayBaseMeshAlgo : public BRepMesh_BaseMeshAlgo
{
public:

  //! Constructor.
  Standard_EXPORT BRepMesh_DelaunayBaseMeshAlgo();

  //! Destructor.
  Standard_EXPORT virtual ~BRepMesh_DelaunayBaseMeshAlgo();

  DEFINE_STANDARD_RTTI_INLINE(BRepMesh_DelaunayBaseMeshAlgo, BRepMesh_BaseMeshAlgo)

protected:

  //! Generates mesh for the contour stored in data structure.
  Standard_EXPORT virtual void generateMesh();

  //! Perfroms processing of generated mesh.
  //! By default does nothing.
  Standard_EXPORT virtual void postProcessMesh(BRepMesh_Delaun& /*theMesher*/)
  {
  }
};

#endif