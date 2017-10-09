// Created on: 2016-07-04
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

#include <BRepMesh_ModelPreProcessor.hxx>
#include <BRepMesh_ShapeTool.hxx>
#include <BRep_Tool.hxx>
#include <IMeshData_Model.hxx>
#include <IMeshData_Edge.hxx>
#include <IMeshData_Wire.hxx>
#include <IMeshData_PCurve.hxx>
#include <OSD_Parallel.hxx>

namespace
{
  //! Checks consistency of triangulation stored in topological face.
  class TriangulationConsistency
  {
  public:
    //! Constructor
    TriangulationConsistency(const Handle(IMeshData_Model)& theModel)
      : myModel (theModel)
    {
    }

    //! Main functor.
    void operator()(const Standard_Integer theFaceIndex) const
    {
      const IMeshData::IFaceHandle& aDFace = myModel->GetFace(theFaceIndex);
      if (aDFace->IsSet(IMeshData_Outdated))
      {
        return;
      }

      TopLoc_Location aLoc;
      const Handle(Poly_Triangulation)& aTriangulation =
        BRep_Tool::Triangulation(aDFace->GetFace(), aLoc);

      if (!aTriangulation.IsNull())
      {
        Standard_Boolean isTriangulationConsistent = Standard_False;
        if (aTriangulation->Deflection() < 1.1 * aDFace->GetDeflection())
        {
          // #25080: check that indices of links forming triangles are in range.
          const Standard_Integer aNodesNb = aTriangulation->NbNodes();
          const Poly_Array1OfTriangle& aTriangles = aTriangulation->Triangles();

          Standard_Integer i = aTriangles.Lower();
          for (; i <= aTriangles.Upper() && isTriangulationConsistent; ++i)
          {
            const Poly_Triangle& aTriangle = aTriangles(i);

            Standard_Integer aNode[3];
            aTriangle.Get(aNode[0], aNode[1], aNode[2]);
            for (Standard_Integer j = 0; j < 3 && isTriangulationConsistent; ++j)
            {
              isTriangulationConsistent = (aNode[j] >= 1 && aNode[j] <= aNodesNb);
            }
          }
        }

        if (isTriangulationConsistent)
        {
          aDFace->SetStatus(IMeshData_Reused);
          aDFace->SetDeflection(aTriangulation->Deflection());
        }
        else
        {
          aDFace->SetStatus(IMeshData_Outdated);
        }
      }
    }

  private:

    Handle(IMeshData_Model) myModel;
  };
}

//=======================================================================
// Function: Constructor
// Purpose : 
//=======================================================================
BRepMesh_ModelPreProcessor::BRepMesh_ModelPreProcessor()
{
}

//=======================================================================
// Function: Destructor
// Purpose : 
//=======================================================================
BRepMesh_ModelPreProcessor::~BRepMesh_ModelPreProcessor()
{
}

//=======================================================================
// Function: Perform
// Purpose : 
//=======================================================================
Standard_Boolean BRepMesh_ModelPreProcessor::Perform(
  const Handle(IMeshData_Model)& theModel,
  const IMeshTools_Parameters&   theParameters)
{
  if (theModel.IsNull())
  {
    return Standard_False;
  }

  OSD_Parallel::For(0, theModel->FacesNb(), TriangulationConsistency(theModel), !theParameters.InParallel);

  // Clean edges and faces from outdated polygons.
  Handle(NCollection_IncAllocator) aTmpAlloc(new NCollection_IncAllocator(IMeshData::MEMORY_BLOCK_SIZE_HUGE));
  NCollection_Map<IMeshData_Face*> aUsedFaces(1, aTmpAlloc);
  for (Standard_Integer aEdgeIt = 0; aEdgeIt < theModel->EdgesNb(); ++aEdgeIt)
  {
    const IMeshData::IEdgeHandle& aDEdge = theModel->GetEdge(aEdgeIt);
    if (aDEdge->IsFree())
    {
      if (aDEdge->IsSet(IMeshData_Outdated))
      {
        TopLoc_Location aLoc;
        BRep_Tool::Polygon3D(aDEdge->GetEdge(), aLoc);
        BRepMesh_ShapeTool::NullifyEdge(aDEdge->GetEdge(), aLoc);
      }

      continue;
    }
    
    for (Standard_Integer aPCurveIt = 0; aPCurveIt < aDEdge->PCurvesNb(); ++aPCurveIt)
    {
      // Find adjacent outdated face.
      const IMeshData::IFaceHandle& aDFace = aDEdge->GetPCurve(aPCurveIt)->GetFace().lock();
      if (!aUsedFaces.Contains(aDFace.get()))
      {
        aUsedFaces.Add(aDFace.get());
        if (aDFace->IsSet(IMeshData_Outdated))
        {
          TopLoc_Location aLoc;
          const Handle(Poly_Triangulation)& aTriangulation =
            BRep_Tool::Triangulation(aDFace->GetFace(), aLoc);

          // Clean all edges of oudated face.
          for (Standard_Integer aWireIt = 0; aWireIt < aDFace->WiresNb(); ++aWireIt)
          {
            const IMeshData::IWireHandle& aDWire = aDFace->GetWire(aWireIt);
            for (Standard_Integer aWireEdgeIt = 0; aWireEdgeIt < aDWire->EdgesNb(); ++aWireEdgeIt)
            {
              const IMeshData::IEdgeHandle& aTmpDEdge = aDWire->GetEdge(aWireEdgeIt).lock();
              BRepMesh_ShapeTool::NullifyEdge(aTmpDEdge->GetEdge(), aTriangulation, aLoc);
            }
          }

          BRepMesh_ShapeTool::NullifyFace(aDFace->GetFace());
        }
      }
    }
  }

  return Standard_True;
}

