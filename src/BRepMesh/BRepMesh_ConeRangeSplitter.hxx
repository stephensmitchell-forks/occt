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

#ifndef _BRepMesh_ConeRangeSplitter_HeaderFile
#define _BRepMesh_ConeRangeSplitter_HeaderFile

#include <BRepMesh_DefaultRangeSplitter.hxx>
#include <GCPnts_TangentialDeflection.hxx>
#include <IMeshTools_Parameters.hxx>

//! Auxiliary class extending default range splitter in
//! order to generate internal nodes for conical surface.
class BRepMesh_ConeRangeSplitter : public BRepMesh_DefaultRangeSplitter
{
public:

  //! Constructor.
  Standard_EXPORT BRepMesh_ConeRangeSplitter()
  {
  }

  //! Destructor.
  Standard_EXPORT virtual ~BRepMesh_ConeRangeSplitter()
  {
  }

  //! Returns list of nodes generated using surface data and specified parameters.
  Standard_EXPORT virtual Handle(IMeshData::ListOfPnt2d) GenerateSurfaceNodes(
    const IMeshTools_Parameters& theParameters) const Standard_OVERRIDE
  {
    const std::pair<Standard_Real, Standard_Real>& aRangeU = GetRangeU();
    const std::pair<Standard_Real, Standard_Real>& aRangeV = GetRangeV();

    gp_Cone aCone = GetDFace()->GetSurface()->Cone();
    Standard_Real aRefR = aCone.RefRadius();
    Standard_Real aSAng = aCone.SemiAngle();
    Standard_Real aRadius = Max(Abs(aRefR + aRangeV.first  * Sin(aSAng)),
                                Abs(aRefR + aRangeV.second * Sin(aSAng)));

    Standard_Real Dv, Du = GCPnts_TangentialDeflection::ArcAngularStep(
      aRadius, GetDFace()->GetDeflection(), theParameters.Angle, theParameters.MinSize);

    const Standard_Real aDiffU = aRangeU.second - aRangeU.first;
    const Standard_Real aDiffV = aRangeV.second - aRangeV.first;
    Standard_Integer nbU = (Standard_Integer) (aDiffU / Du);
    Standard_Integer nbV = (Standard_Integer) (nbU * (aDiffV) / (aDiffU * aRadius));
    Du = aDiffU / (nbU + 1);
    Dv = aDiffV / (nbV + 1);

    const Handle(NCollection_IncAllocator) aTmpAlloc =
      new NCollection_IncAllocator(IMeshData::MEMORY_BLOCK_SIZE_HUGE);
    Handle(IMeshData::ListOfPnt2d) aNodes = new IMeshData::ListOfPnt2d(aTmpAlloc);

    const Standard_Real aPasMaxV = aRangeV.second - Dv*0.5;
    const Standard_Real aPasMaxU = aRangeU.second - Du*0.5;
    for (Standard_Real aPasV = aRangeV.first + Dv; aPasV < aPasMaxV; aPasV += Dv)
    {
      for (Standard_Real aPasU = aRangeV.first + Du; aPasU < aPasMaxU; aPasU += Du)
      {
        aNodes->Append(gp_Pnt2d(aPasU, aPasV));
      }
    }

    return aNodes;
  }
};

#endif
