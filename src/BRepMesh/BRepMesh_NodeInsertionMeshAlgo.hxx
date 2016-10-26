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

#ifndef _BRepMesh_NodeInsertionMeshAlgo_HeaderFile
#define _BRepMesh_NodeInsertionMeshAlgo_HeaderFile

#include <BRepMesh_Classifier.hxx>
#include <IMeshData_Wire.hxx>
#include <IMeshData_Edge.hxx>
#include <IMeshData_PCurve.hxx>
#include <BRepMesh_Vertex.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS_Vertex.hxx>
#include <BRep_Tool.hxx>
#include <Standard_ErrorHandler.hxx>
#include <BRepMesh_Delaun.hxx>

//! Extends base meshing algo in order to enable possibility 
//! of addition of free vertices into the mesh.
template<class RangeSplitter, class BaseAlgo>
class BRepMesh_NodeInsertionMeshAlgo : public BaseAlgo
{
public:

  //! Constructor.
  Standard_EXPORT BRepMesh_NodeInsertionMeshAlgo()
  {
  }

  //! Destructor.
  Standard_EXPORT virtual ~BRepMesh_NodeInsertionMeshAlgo()
  {
  }

  //! Performs processing of the given face.
  Standard_EXPORT virtual void Perform(
    const IMeshData::IFaceHandle& theDFace,
    const IMeshTools_Parameters&  theParameters) Standard_OVERRIDE
  {
    myRangeSplitter.Reset(theDFace, theParameters);
    myClassifier = new BRepMesh_Classifier;
    BaseAlgo::Perform(theDFace, theParameters);
    myClassifier.Nullify();
  }

  DEFINE_STANDARD_RTTI_INLINE(BRepMesh_NodeInsertionMeshAlgo, BaseAlgo)

protected:

  typedef NCollection_Shared<NCollection_Sequence<const gp_Pnt2d*> > SequenceOfPnt2d;

  //! Performs initialization of data structure using existing model data.
  Standard_EXPORT virtual Standard_Boolean initDataStructure() Standard_OVERRIDE
  {
    Handle(NCollection_IncAllocator) aTmpAlloc =
      new NCollection_IncAllocator(IMeshData::MEMORY_BLOCK_SIZE_HUGE);

    const IMeshData::IFaceHandle& aDFace = getDFace();
    NCollection_Array1<Handle(SequenceOfPnt2d)> aWires(0, aDFace->WiresNb());
    for (Standard_Integer aWireIt = 0; aWireIt < aDFace->WiresNb(); ++aWireIt)
    {
      const IMeshData::IWireHandle& aDWire = aDFace->GetWire(aWireIt);
      if (aDWire->IsSet(IMeshData_SelfIntersectingWire) ||
         (aDWire->IsSet(IMeshData_OpenWire) && aWireIt != 0))
      {
        continue;
      }

      aWires(aWireIt) = collectWirePoints(aDWire, aTmpAlloc);
    }

    myRangeSplitter.AdjustRange();
    if (!myRangeSplitter.IsValid())
    {
      aDFace->SetStatus(IMeshData_Failure);
      return Standard_False;
    }

    const std::pair<Standard_Real, Standard_Real>& aDelta = myRangeSplitter.GetDelta();
    const std::pair<Standard_Real, Standard_Real>& aTolUV = myRangeSplitter.GetToleranceUV();
    const Standard_Real uCellSize = 14.0 * aTolUV.first;
    const Standard_Real vCellSize = 14.0 * aTolUV.second;

    getStructure()->Data()->SetCellSize (uCellSize    / aDelta.first, vCellSize     / aDelta.second);
    getStructure()->Data()->SetTolerance(aTolUV.first / aDelta.first, aTolUV.second / aDelta.second);

    for (Standard_Integer aWireIt = 0; aWireIt < aDFace->WiresNb(); ++aWireIt)
    {
      const Handle(SequenceOfPnt2d)& aWire = aWires(aWireIt);
      if (!aWire.IsNull() && !aWire->IsEmpty())
      {
        myClassifier->RegisterWire(*aWire, aTolUV,
                                   myRangeSplitter.GetRangeU(),
                                   myRangeSplitter.GetRangeV());
      }
    }

    if (getParameters().InternalVerticesMode)
    {
      insertInternalVertices();
    }

    return BaseAlgo::initDataStructure();
  }

  //! Adds the given 2d point to mesh data structure.
  //! Returns index of node in the structure.
  Standard_EXPORT virtual Standard_Integer addNodeToStructure(
    const gp_Pnt2d&                thePoint,
    const Standard_Integer         theLocation3d,
    const BRepMesh_DegreeOfFreedom theMovability,
    const Standard_Boolean         isForceAdd) Standard_OVERRIDE
  {
    return BaseAlgo::addNodeToStructure(
      myRangeSplitter.Scale(thePoint, Standard_True),
      theLocation3d, theMovability, isForceAdd);
  }

  //! Returns 2d point associated to the given vertex.
  Standard_EXPORT virtual gp_Pnt2d getNodePoint2d(
    const BRepMesh_Vertex& theVertex) const Standard_OVERRIDE
  {
    return myRangeSplitter.Scale(theVertex.Coord(), Standard_False);
  }

  //! Returns range splitter.
  const RangeSplitter& getRangeSplitter() const
  {
    return myRangeSplitter;
  }

  //! Returns classifier.
  const Handle(BRepMesh_Classifier)& getClassifier() const
  {
    return myClassifier;
  }

private:

  //! Creates collection of points representing discrete wire.
  Handle(SequenceOfPnt2d) collectWirePoints(
    const IMeshData::IWireHandle&           theDWire,
    const Handle(NCollection_IncAllocator)& theAllocator)
  {
    Handle(SequenceOfPnt2d) aWirePoints = new SequenceOfPnt2d(theAllocator);
    for (Standard_Integer aEdgeIt = 0; aEdgeIt < theDWire->EdgesNb(); ++aEdgeIt)
    {
      const IMeshData::IEdgeHandle&   aDEdge = theDWire->GetEdge(aEdgeIt).lock();
      const IMeshData::IPCurveHandle& aPCurve = aDEdge->GetPCurve(
        getDFace(), theDWire->GetEdgeOrientation(aEdgeIt));

      Standard_Integer aPointIt, aEndIndex, aInc;
      if (aPCurve->IsForward())
      {
        aPointIt = 0;
        aEndIndex = aPCurve->ParametersNb() - 1;
        aInc = 1;
      }
      else
      {
        aPointIt = aPCurve->ParametersNb() - 1;
        aEndIndex = 0;
        aInc = -1;
      }

      for (; aPointIt != aEndIndex; aPointIt += aInc)
      {
        const gp_Pnt2d& aPnt2d = aPCurve->GetPoint(aPointIt);
        aWirePoints->Append(&aPnt2d);
        myRangeSplitter.AddPoint(aPnt2d);
      }
    }

    return aWirePoints;
  }

  //! Iterates over internal vertices of a face and 
  //! creates corresponding nodes in data structure.
  void insertInternalVertices()
  {
    TopExp_Explorer aExplorer(getDFace()->GetFace(), TopAbs_VERTEX, TopAbs_EDGE);
    for (; aExplorer.More(); aExplorer.Next())
    {
      const TopoDS_Vertex& aVertex = TopoDS::Vertex(aExplorer.Current());
      if (aVertex.Orientation() != TopAbs_INTERNAL)
      {
        continue;
      }

      insertInternalVertex(aVertex);
    }
  }

  //! Inserts the given vertex into mesh.
  void insertInternalVertex(const TopoDS_Vertex& theVertex)
  {
    try
    {
      OCC_CATCH_SIGNALS

        gp_Pnt2d aPnt2d = BRep_Tool::Parameters(theVertex, getDFace()->GetFace());
      // check UV values for internal vertices
      if (myClassifier->Perform(aPnt2d) != TopAbs_IN)
        return;

      registerNode(BRep_Tool::Pnt(theVertex), aPnt2d,
                   BRepMesh_Fixed, Standard_False);
    }
    catch (Standard_Failure)
    {
    }
  }

private:

  RangeSplitter               myRangeSplitter;
  Handle(BRepMesh_Classifier) myClassifier;
};

#endif