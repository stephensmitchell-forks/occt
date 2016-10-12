// Created on: 2016-04-07
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

#ifndef _IMeshData_Types_HeaderFile
#define _IMeshData_Types_HeaderFile

#include <NCollection_DataMap.hxx>
#include <NCollection_Vector.hxx>
#include <NCollection_Sequence.hxx>
#include <NCollection_List.hxx>
#include <NCollection_Shared.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <TopoDS_Shape.hxx>
#include <NCollection_DefineAlloc.hxx>
#include <NCollection_StdAllocator.hxx>
#include <IMeshData_ParametersListArrayAdaptor.hxx>
#include <TColStd_PackedMapOfInteger.hxx>
#include <TColStd_MapIteratorOfPackedMapOfInteger.hxx>
#include <Precision.hxx>
#include <NCollection_EBTree.hxx>
#include <Bnd_Box2d.hxx>
#include <NCollection_CellFilter.hxx>
#include <NCollection_IndexedDataMap.hxx>
#include <NCollection_UBTreeFiller.hxx>
#include <NCollection_IndexedMap.hxx>
#include <BRepMesh_OrientedEdge.hxx>
#include <BRepMesh_Vertex.hxx>
#include <Bnd_B2d.hxx>

#include <memory>
#include <queue>
#include <list>

class IMeshData_Shape;
class IMeshData_Face;
class IMeshData_Wire;
class IMeshData_Edge;
class IMeshData_Curve;
class IMeshData_PCurve;
class IMeshData_Model;
class BRepMesh_VertexInspector;
class BRepMesh_Triangle;
class BRepMesh_Edge;
class BRepMesh_PairOfIndex;
class BRepMesh_Circle;
class BRepMesh_CircleInspector;

#define DEFINE_INC_ALLOC                      \
  DEFINE_NCOLLECTION_ALLOC                    \
  void operator delete (void* /*theAddress*/) \
  {                                           \
    /*it's inc allocator, nothing to do*/     \
  }

namespace IMeshData
{
  //! Default size for memory block allocated by IncAllocator. 
  /**
  * The idea here is that blocks of the given size are returned to the system
  * rather than retained in the malloc heap, at least on WIN32 and WIN64 platforms.
  */
#ifdef _WIN64
  const size_t MEMORY_BLOCK_SIZE_HUGE = 1024 * 1024;
#else
  const size_t MEMORY_BLOCK_SIZE_HUGE = 512 * 1024;
#endif

  typedef std::weak_ptr<IMeshData_Shape>  IShapePtr;
  typedef std::weak_ptr<IMeshData_Edge>   IEdgePtr;
  typedef std::weak_ptr<IMeshData_Wire>   IWirePtr;
  typedef std::weak_ptr<IMeshData_Face>   IFacePtr;
  typedef std::weak_ptr<IMeshData_Curve>  ICurvePtr;
  typedef std::weak_ptr<IMeshData_PCurve> IPCurvePtr;
  typedef std::weak_ptr<IMeshData_Model>  IModelPtr;

  typedef std::shared_ptr<IMeshData_Shape>    IShapeHandle;
  typedef std::shared_ptr<IMeshData_Edge>     IEdgeHandle;
  typedef std::shared_ptr<IMeshData_Wire>     IWireHandle;
  typedef std::shared_ptr<IMeshData_Face>     IFaceHandle;
  typedef std::shared_ptr<IMeshData_Curve>    ICurveHandle;
  typedef std::shared_ptr<IMeshData_PCurve>   IPCurveHandle;
  typedef std::shared_ptr<IMeshData_Model>    IModelHandle;

  typedef IMeshData_ParametersListArrayAdaptor<ICurveHandle> ICurveArrayAdaptor;
  typedef std::shared_ptr<ICurveArrayAdaptor>                ICurveArrayAdaptorHandle;

  typedef NCollection_Shared<NCollection_EBTree<Standard_Integer, Bnd_Box2d> > BndBox2dTree;
  typedef NCollection_UBTreeFiller<Standard_Integer, Bnd_Box2d>                BndBox2dTreeFiller;

  // Vectors
  typedef NCollection_Vector<IFaceHandle>        VectorOfIFaceHandles;
  typedef NCollection_Vector<IWireHandle>        VectorOfIWireHandles;
  typedef NCollection_Vector<IEdgeHandle>        VectorOfIEdgeHandles;
  typedef NCollection_Vector<IPCurveHandle>      VectorOfIPCurveHandles;
  typedef NCollection_Vector<IEdgePtr>           VectorOfIEdgePtrs;
  typedef NCollection_Vector<IShapeHandle>       VectorOfIShapesHandles;
  typedef NCollection_Vector<Standard_Boolean>   VectorOfBoolean;
  typedef NCollection_Vector<Standard_Integer>   VectorOfInteger;
  typedef NCollection_Vector<TopAbs_Orientation> VectorOfOrientation;
  typedef NCollection_Vector<Handle(BndBox2dTree)>  VectorOfHBndBox2dTree;
  typedef NCollection_Vector<BRepMesh_Triangle>     VectorOfElements;
  typedef NCollection_Vector<Standard_Integer>      VectorOfInteger;
  typedef NCollection_Vector<BRepMesh_Circle>       VectorOfCircle;

  typedef NCollection_Array1<BRepMesh_Vertex>                       Array1OfVertexOfDelaun;
  typedef NCollection_Shared<NCollection_Vector<BRepMesh_Vertex> >  VectorOfVertex;

  // Sequences
  typedef NCollection_Shared<NCollection_Sequence<Bnd_B2d> >          SequenceOfBndB2d;
  typedef NCollection_Shared<NCollection_Sequence<Standard_Integer> > SequenceOfInteger;
  typedef NCollection_Shared<NCollection_Sequence<Standard_Real> >    SequenceOfReal;

  namespace Model
  {
    typedef std::deque<gp_Pnt, NCollection_StdAllocator<gp_Pnt> >                     SequenceOfPnt;
    typedef std::deque<gp_Pnt2d, NCollection_StdAllocator<gp_Pnt2d> >                 SequenceOfPnt2d;
    typedef std::deque<Standard_Real, NCollection_StdAllocator<Standard_Real> >       SequenceOfReal;
    typedef std::deque<Standard_Integer, NCollection_StdAllocator<Standard_Integer> > SequenceOfInteger;
  }

  // Lists
  typedef NCollection_Shared<NCollection_List<Standard_Integer> > ListOfInteger;
  typedef NCollection_Shared<NCollection_List<gp_Pnt2d> >         ListOfPnt2d;
  typedef NCollection_List<IPCurveHandle>                         ListOfIPCurves;

  typedef NCollection_Shared<TColStd_PackedMapOfInteger>          MapOfInteger;
  typedef TColStd_MapIteratorOfPackedMapOfInteger                 IteratorOfMapOfInteger;

  typedef NCollection_CellFilter<BRepMesh_CircleInspector>   CircleCellFilter;
  typedef NCollection_CellFilter<BRepMesh_VertexInspector>   VertexCellFilter;

  // Data Maps
  template<typename Type>
  struct WeakEqual
  {
    static Standard_Boolean IsEqual(const std::weak_ptr<Type>& theFirst,
                                    const std::weak_ptr<Type>& theSecond)
    {
      return (theFirst.lock().get() == theSecond.lock().get());
    }

    static Standard_Integer HashCode(const std::weak_ptr<Type>& thePtr, Standard_Integer theUpper)
    {
      return ::HashCode(thePtr.lock().get(), theUpper);
    }
  };

  typedef NCollection_Shared<NCollection_DataMap<TopoDS_Shape, Standard_Integer, TopTools_ShapeMapHasher> >     DMapOfShapeInteger;
  typedef NCollection_Shared<NCollection_DataMap<IFacePtr, ListOfInteger, WeakEqual<IMeshData_Face> > >         DMapOfIFacePtrsListOfInteger;
  typedef NCollection_Shared<NCollection_Map<IEdgePtr, WeakEqual<IMeshData_Edge> > >                            MapOfIEdgePtr;
  typedef NCollection_Shared<NCollection_Map<IFacePtr, WeakEqual<IMeshData_Face> > >                            MapOfIFacePtr;
  typedef NCollection_Shared<NCollection_Map<BRepMesh_OrientedEdge> >                                           MapOfOrientedEdges;
  typedef NCollection_Shared<NCollection_Map<Standard_Real> >                                                   MapOfReal;
  typedef NCollection_Shared<NCollection_IndexedDataMap<IFacePtr, ListOfIPCurves, WeakEqual<IMeshData_Face> > > IDMapOfIFacePtrsListOfIPCurves;
  typedef NCollection_Shared<NCollection_DataMap<IFacePtr, Handle(MapOfIEdgePtr), WeakEqual<IMeshData_Face> > > DMapOfIFacePtrsMapOfIEdgePtrs;
  typedef NCollection_IndexedDataMap<BRepMesh_Edge, BRepMesh_PairOfIndex>                                       IDMapOfLink;
  typedef NCollection_DataMap<Standard_Integer, ListOfInteger>                                                  DMapOfIntegerListOfInteger;
  typedef NCollection_DataMap<Standard_Integer, Standard_Integer>                                               MapOfIntegerInteger;
  typedef NCollection_IndexedMap<Standard_Real>                                                                 IMapOfReal;

  typedef NCollection_Array1<Standard_Integer>                                                                  Array1OfInteger;

  namespace Sweep
  {
    const Standard_Real CoincidencePrecision = 100 * Precision::Confusion();
    typedef NCollection_Shared<NCollection_DataMap<Standard_Integer, VectorOfInteger> >           DMapOfIntegerVectorOfInteger;
    typedef std::list<Standard_Integer>                                                           ListOfNodes;
    typedef ListOfNodes::iterator                                                                 NodeIterator;
    typedef NCollection_Shared<std::list<NodeIterator> >                                          ListOfNodesIterator;
    typedef NCollection_EBTree<NodeIterator, Bnd_Box2d>                                           BndBox2dNodeIteratorTree;

    struct NodeIteratorHasher
    {
      static Standard_Boolean IsEqual(const NodeIterator& theFirst,
                                      const NodeIterator& theSecond)
      {
        return theFirst == theSecond;
      }

      static Standard_Integer HashCode(const NodeIterator& theIterator,
                                       Standard_Integer    theUpper)
      {
        return ::HashCode(theIterator.operator->(), theUpper);
      }
    };
  }
}

#endif