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

#ifndef _BRepMeshTools_DelaunayDeflectionControlMeshAlgo_HeaderFile
#define _BRepMeshTools_DelaunayDeflectionControlMeshAlgo_HeaderFile

#include <BRepMesh_DelaunayNodeInsertionMeshAlgo.hxx>
#include <BRepMesh_GeomTool.hxx>

//! Extends node insertion Delaunay meshing algo in order to control 
//! deflection of generated trianges. Splits triangles failing the check.
template<class RangeSplitter>
class BRepMesh_DelaunayDeflectionControlMeshAlgo : public BRepMesh_DelaunayNodeInsertionMeshAlgo<RangeSplitter>
{
private:
  // Typedef for OCCT RTTI
  typedef BRepMesh_DelaunayNodeInsertionMeshAlgo<RangeSplitter> DelaunayInsertionBaseClass;

public:

  //! Constructor.
  Standard_EXPORT BRepMesh_DelaunayDeflectionControlMeshAlgo()
    : myMaxSqDeflection(-1.),
      myIsAllDegenerated(Standard_False)
  {
  }

  //! Destructor.
  Standard_EXPORT virtual ~BRepMesh_DelaunayDeflectionControlMeshAlgo()
  {
  }

  DEFINE_STANDARD_RTTI_INLINE(BRepMesh_DelaunayDeflectionControlMeshAlgo, DelaunayInsertionBaseClass)

protected:

  //! Perfroms processing of generated mesh. Generates surface nodes and inserts them into structure.
  Standard_EXPORT virtual void postProcessMesh(BRepMesh_Delaun& theMesher) Standard_OVERRIDE
  {
    // Insert surface nodes.
    DelaunayInsertionBaseClass::postProcessMesh(theMesher);

    if (getParameters().ControlSurfaceDeflection &&
        getStructure()->ElementsOfDomain().Extent() > 0)
    {
      optimizeMesh(theMesher);
    }
  }

  //! Checks deviation of a mesh from geometrical surface.
  //! Inerts additional nodes in case of huge deviation.
  Standard_EXPORT virtual void optimizeMesh(BRepMesh_Delaun& theMesher)
  {
    Handle(NCollection_IncAllocator) aTmpAlloc =
      new NCollection_IncAllocator(IMeshData::MEMORY_BLOCK_SIZE_HUGE);

    myCouplesMap   = new IMeshData::MapOfOrientedEdges(3 * getStructure()->ElementsOfDomain().Extent(), aTmpAlloc);
    myControlNodes = new IMeshData::ListOfPnt2d(aTmpAlloc);
    myCircles      = &theMesher.Circles();

    const Standard_Integer aIterationsNb = 11;
    Standard_Boolean isInserted = Standard_True;
    for (Standard_Integer aPass = 1; aPass <= aIterationsNb && isInserted && !myIsAllDegenerated; ++aPass)
    {
      // Reset stop condition
      myMaxSqDeflection = -1.;
      myIsAllDegenerated = Standard_True;
      myControlNodes->Clear();

      if (getStructure()->ElementsOfDomain().Extent() < 1)
      {
        break;
      }

      // Iterate on current triangles
      IMeshData::IteratorOfMapOfInteger aTriangleIt(getStructure()->ElementsOfDomain());
      for (; aTriangleIt.More(); aTriangleIt.Next())
      {
        const BRepMesh_Triangle& aTriangle = getStructure()->GetElement(aTriangleIt.Key());
        splitTriangleGeometry(aTriangle);
      }

      isInserted = insertNodes(myControlNodes, theMesher);
    }

    myCouplesMap.Nullify();
    myControlNodes.Nullify();

    if (!(myMaxSqDeflection < 0.))
    {
      getDFace()->SetDeflection(Sqrt(myMaxSqDeflection));
    }
  }

private:
  //! Contains geometrical data related to node of triangle.
  struct TriangleNodeInfo
  {
    gp_XY            Point2d;
    gp_XYZ           Point;
    Standard_Boolean isFrontierLink;
  };

  //! Functor computing deflection of a point from surface.
  class NormalDeviation
  {
  public:
    NormalDeviation(
      const gp_Pnt& theRefPnt,
      const gp_Vec& theNormal)
      : myRefPnt(theRefPnt),
        myNormal(theNormal)
    {
    }

    Standard_Real SquareDeviation(const gp_Pnt& thePoint) const
    {
      const Standard_Real aDeflection = Abs(myNormal.Dot(gp_Vec(myRefPnt, thePoint)));
      return aDeflection * aDeflection;
    }

  private:

    NormalDeviation (const NormalDeviation& theOther);

    void operator= (const NormalDeviation& theOther);

  private:

    const gp_Pnt& myRefPnt;
    const gp_Vec& myNormal;
  };

  //! Functor computing deflection of a point on triangle link from surface.
  class LineDeviation
  {
  public:

    LineDeviation(
      const gp_Pnt& thePnt1,
      const gp_Pnt& thePnt2)
      : myPnt1(thePnt1),
        myPnt2(thePnt2)
    {
    }

    Standard_Real SquareDeviation(const gp_Pnt& thePoint) const
    {
      return BRepMesh_GeomTool::SquareDeflectionOfSegment(myPnt1, myPnt2, thePoint);
    }

  private:

    LineDeviation (const LineDeviation& theOther);

    void operator= (const LineDeviation& theOther);

  private:
    const gp_Pnt& myPnt1;
    const gp_Pnt& myPnt2;
  };

  //! Returns nodes info of the given triangle.
  inline void getTriangleInfo(
    const BRepMesh_Triangle& theTriangle,
    const Standard_Integer (&theNodesIndices)[3],
    TriangleNodeInfo       (&theInfo)[3]) const
  {
    const Standard_Integer(&e)[3] = theTriangle.myEdges;
    for (Standard_Integer i = 0; i < 3; ++i)
    {
      const BRepMesh_Vertex& aVertex = getStructure()->GetNode(theNodesIndices[i]);
      theInfo[i].Point2d        = getRangeSplitter().Scale(aVertex.Coord(), Standard_False).XY();
      theInfo[i].Point          = getNodesMap()->Value(aVertex.Location3d()).XYZ();
      theInfo[i].isFrontierLink = (getStructure()->GetLink(e[i]).Movability() == BRepMesh_Frontier);
    }
  }

  // Check geometry of the given triangle. If triangle does not suit specified deflection, inserts new point.
  void splitTriangleGeometry(const BRepMesh_Triangle& theTriangle)
  {
    if (theTriangle.Movability() != BRepMesh_Deleted)
    {
      Standard_Integer aNodexIndices[3];
      getStructure()->ElementNodes(theTriangle, aNodexIndices);

      TriangleNodeInfo aNodesInfo[3];
      getTriangleInfo(theTriangle, aNodexIndices, aNodesInfo);

      gp_Vec aNormal;
      gp_Vec aLinkVec[3];
      if (computeTriangleGeometry(aNodesInfo, aLinkVec, aNormal))
      {
        myIsAllDegenerated = Standard_False;

        const gp_XY aCenter2d = (aNodesInfo[0].Point2d +
                                 aNodesInfo[1].Point2d +
                                 aNodesInfo[2].Point2d) / 3.;

        usePoint(aCenter2d, NormalDeviation(aNodesInfo[0].Point, aNormal));
        splitLinks(aNodesInfo, aNodexIndices);
      }
    }
  }

  //! Updates array of links vectors.
  //! @return False on degenerative triangle.
  inline Standard_Boolean computeTriangleGeometry(
    const TriangleNodeInfo(&theNodesInfo)[3],
    gp_Vec                (&theLinks)[3],
    gp_Vec                 &theNormal)
  {
    if (checkTriangleForDegenerativityAndGetLinks(theNodesInfo, theLinks))
    {
      if (checkTriangleArea2d(theNodesInfo))
      {
        if (computeNormal(theLinks[0], theLinks[1], theNormal))
        {
          return Standard_True;
        }
      }
    }

    return Standard_False;
  }

  //! Updates array of links vectors.
  //! @return False on degenerative triangle.
  inline Standard_Boolean checkTriangleForDegenerativityAndGetLinks(
    const TriangleNodeInfo (&theNodesInfo)[3],
    gp_Vec                 (&theLinks)[3])
  {
    const Standard_Real MinimalSqLength3d = 1.e-12;
    for (Standard_Integer i = 0; i < 3; ++i)
    {
      theLinks[i] = theNodesInfo[(i + 1) % 3].Point - theNodesInfo[i].Point;
      if (theLinks[i].SquareMagnitude() < MinimalSqLength3d)
      {
        return Standard_False;
      }
    }

    return Standard_True;
  }

  //! Checks area of triangle in parametric space for degenerativity.
  //! @return False on degenerative triangle.
  inline Standard_Boolean checkTriangleArea2d(
    const TriangleNodeInfo (&theNodesInfo)[3])
  {
    const gp_Vec2d aLink2d1(theNodesInfo[0].Point2d, theNodesInfo[1].Point2d);
    const gp_Vec2d aLink2d2(theNodesInfo[1].Point2d, theNodesInfo[2].Point2d);

    const Standard_Real MinimalArea2d = 1.e-9;
    return (Abs(aLink2d1 ^ aLink2d2) > MinimalArea2d);
  }

  //! Computes normal using two link vectors.
  //! @return True on success, False in case of normal of null magnitude.
  inline Standard_Boolean computeNormal(const gp_Vec& theLink1,
                                        const gp_Vec& theLink2,
                                        gp_Vec&       theNormal)
  {
    const gp_Vec aNormal(theLink1 ^ theLink2);
    if (aNormal.SquareMagnitude() > gp::Resolution())
    {
      theNormal = aNormal.Normalized();
      return Standard_True;
    }

    return Standard_False;
  }

  //! Computes deflection of midpoints of triangles links.
  //! @return True if point fits specified deflection.
  inline void splitLinks(
    const TriangleNodeInfo (&theNodesInfo)[3],
    const Standard_Integer (&theNodesIndices)[3])
  {
    // Check deflection at triangle links
    for (Standard_Integer i = 0; i < 3; ++i)
    {
      if (theNodesInfo[i].isFrontierLink)
      {
        continue;
      }

      const Standard_Integer j = (i + 1) % 3;
      // Check if this link was already processed
      Standard_Integer aFirstVertex, aLastVertex;
      if (theNodesIndices[i] < theNodesIndices[j])
      {
        aFirstVertex = theNodesIndices[i];
        aLastVertex  = theNodesIndices[j];
      }
      else
      {
        aFirstVertex = theNodesIndices[j];
        aLastVertex  = theNodesIndices[i];
      }

      if (myCouplesMap->Add(BRepMesh_OrientedEdge(aFirstVertex, aLastVertex)))
      {
        const gp_XY aMidPnt2d = (theNodesInfo[i].Point2d +
                                 theNodesInfo[j].Point2d) / 2.;

        usePoint(aMidPnt2d, LineDeviation(theNodesInfo[i].Point, theNodesInfo[j].Point));
      }
    }
  }

  //! Computes deflection of the given point and caches it for
  //! insertion in case if it overflows deflection.
  //! @return True if point has been cached for insertion.
  template<class DeflectionFunctor>
  inline void usePoint(
    const gp_XY&             thePnt2d,
    const DeflectionFunctor& theDeflectionFunctor)
  {
    gp_Pnt aPnt;
    getDFace()->GetSurface()->D0(thePnt2d.X(), thePnt2d.Y(), aPnt);
    if (!checkDeflectionOfPointAndUpdateCache(thePnt2d, aPnt, theDeflectionFunctor.SquareDeviation(aPnt)))
    {
      myControlNodes->Append(thePnt2d);
    }
  }

  //! Checks the given point for specified deflection.
  //! Updates value of total mesh defleciton.
  Standard_Boolean checkDeflectionOfPointAndUpdateCache(
    const gp_XY&        thePnt2d,
    const gp_Pnt&       thePnt3d,
    const Standard_Real theSqDeflection)
  {
    if (theSqDeflection > myMaxSqDeflection)
    {
      myMaxSqDeflection = theSqDeflection;
    }

    const Standard_Real aSqDeflection = getDFace()->GetDeflection() * getDFace()->GetDeflection();
    if (theSqDeflection < aSqDeflection)
    {
      return Standard_True;
    }

    if (getParameters().MinSize > Precision::Confusion())
    {
      return rejectByMinSize(thePnt2d, thePnt3d);
    }

    return Standard_False;
  }

  //! Checks the given node for 
  Standard_Boolean rejectByMinSize(
    const gp_XY&  thePnt2d,
    const gp_Pnt& thePnt3d)
  {
    const Standard_Real aSqMinSize = getParameters().MinSize * getParameters().MinSize;

    Handle(NCollection_IncAllocator) aTmpAlloc =
      new NCollection_IncAllocator(IMeshData::MEMORY_BLOCK_SIZE_HUGE);

    IMeshData::MapOfInteger aUsedNodes;
    IMeshData::ListOfInteger& aCirclesList =
      const_cast<BRepMesh_CircleTool&>(*myCircles).Select(
        getRangeSplitter().Scale(thePnt2d, Standard_True).XY());

    IMeshData::ListOfInteger::Iterator aCircleIt(aCirclesList);
    for (; aCircleIt.More(); aCircleIt.Next())
    {
      const BRepMesh_Triangle& aTriangle = getStructure()->GetElement(aCircleIt.Value());

      Standard_Integer aNodes[3];
      getStructure()->ElementNodes(aTriangle, aNodes);

      for (Standard_Integer i = 0; i < 3; ++i)
      {
        if (!aUsedNodes.Contains(aNodes[i]))
        {
          aUsedNodes.Add(aNodes[i]);
          const BRepMesh_Vertex& aVertex = getStructure()->GetNode(aNodes[i]);
          const gp_Pnt& aPoint = getNodesMap()->Value(aVertex.Location3d());

          if (thePnt3d.SquareDistance(aPoint) < aSqMinSize)
          {
            return Standard_True;
          }
        }
      }
    }

    return Standard_False;
  }

private:
  Standard_Real                         myMaxSqDeflection;
  Standard_Boolean                      myIsAllDegenerated;
  Handle(IMeshData::MapOfOrientedEdges) myCouplesMap;
  Handle(IMeshData::ListOfPnt2d)        myControlNodes;
  const BRepMesh_CircleTool*            myCircles;
};

#endif
