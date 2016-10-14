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

#ifndef _BRepMesh_NURBSRangeSplitter_HeaderFile
#define _BRepMesh_NURBSRangeSplitter_HeaderFile

#include <BRepMesh_UVParamRangeSplitter.hxx>
#include <IMeshData_Types.hxx>
#include <IMeshTools_Parameters.hxx>

//! Intended to generate internal mesh nodes using UV parameters of bounday discrete points.
class BRepMesh_NURBSRangeSplitter : public BRepMesh_UVParamRangeSplitter
{
public:

  //! Constructor.
  Standard_EXPORT BRepMesh_NURBSRangeSplitter()
  {
  }

  //! Destructor.
  Standard_EXPORT virtual ~BRepMesh_NURBSRangeSplitter()
  {
  }

  //! Updates discrete range of surface according to its geometric range.
  Standard_EXPORT virtual void AdjustRange() Standard_OVERRIDE;

  //! Returns list of nodes generated using surface data and specified parameters.
  Standard_EXPORT virtual Handle(IMeshData::ListOfPnt2d) GenerateSurfaceNodes(
    const IMeshTools_Parameters& theParameters) const Standard_OVERRIDE;

private:

  //! Amplifies parameters using CN continuity intervals.
  void amplifyParameters();

  //! Computes parameters of filter and applies it to the source parameters.
  Handle(IMeshData::SequenceOfReal) computeGrainAndFilterParameters(
    const IMeshData::IMapOfReal&            theSourceParams,
    const Standard_Real                     theTol2d,
    const Standard_Real                     theRangeDiff,
    const Standard_Real                     theDelta,
    const IMeshTools_Parameters&            theParameters,
    const Handle(NCollection_IncAllocator)& theAllocator) const;

  //! Filters parameters in order to avoid too dence distribution.
  Handle(IMeshData::SequenceOfReal) filterParameters(
    const IMeshData::IMapOfReal&            theParams,
    const Standard_Real                     theMinDist,
    const Standard_Real                     theFilterDist,
    const Handle(NCollection_IncAllocator)& theAllocator) const;

private:

  GeomAbs_SurfaceType mySurfaceType;
};

#endif
