// File:        Poly_Segment2dSewer.hxx
// Created:     01.07.2010
// Author:      Mikhail SAZONOV
// Copyright:   Open Cascade 2010

#ifndef Poly_Segment2dSewer_HeaderFile
#define Poly_Segment2dSewer_HeaderFile

#include <Poly_PolyCurve2d.hxx>
#include <Poly_Segment2d.hxx>
#include <Poly_MakeLoops.hxx>
#include <NCollection_DataMap.hxx>
#include <NCollection_CellFilter.hxx>
// #include <NCollection_HeapAllocator.hxx>
#include <NCollection_Vector.hxx>
#include <gp_Dir2d.hxx>

class Handle_NCollection_IncAllocator;

/**
 * This class recognizes coinciding ends of segments or curves and
 * forms the topological map of nodes to links to be used
 * as helper when making loops.
 */

class Poly_Segment2dSewer : public Poly_MakeLoops2D::Helper
{
public:
  //! Constructor
  Standard_EXPORT Poly_Segment2dSewer
                   (const Standard_Real theTol,
                    const Handle_NCollection_IncAllocator& theAlloc);

  //! Adds one more segment link.
  //! Returns the link corresponding to this segment
  Standard_EXPORT Poly_MakeLoops::Link AddSegment
                   (const Poly_Segment2d& theSeg);

  //! Adds one more open curve link.
  //! Returns the link(s) corresponding to this curve
  Standard_EXPORT void AddCurve
                   (const Handle(Poly_PolyCurve2d)& theCurve,
                    Poly_MakeLoops::ListOfLink& theLinks,
                    const Standard_Boolean theAllowRecurse = Standard_True);

  //! Try to link free nodes with other free nodes or, as last resort,
  //! sew them with other non-free nodes, using theMaxDist as the max link distance.
  //! Outputs new created links
  Standard_EXPORT void LinkFreeNodes
                   (const Standard_Real    theTol,
                    Poly_MakeLoops2D&      theLoopMaker);

  //! Gets segment by link
  Standard_EXPORT Standard_Boolean GetLinkData
                   (const Poly_MakeLoops::Link& theLink,
                    Handle(Poly_PolyCurve2d)& theCurve,
                    Poly_Segment2d& theSeg) const;

  //! Get the number of nodes
  inline Standard_Integer GetNbNodes() const
  { return myNodes.Length(); }

  //! returns the tangent vector at the first node of a link
  Standard_EXPORT virtual Standard_Boolean GetFirstTangent
                   (const Poly_MakeLoops::Link& theLink,
                    gp_Dir2d& theDir) const;

  //! returns the tangent vector at the last node of a link
  Standard_EXPORT virtual Standard_Boolean GetLastTangent
                   (const Poly_MakeLoops::Link& theLink,
                    gp_Dir2d& theDir) const;

  //! returns the links adjacent to the given node
  Standard_EXPORT virtual const Poly_MakeLoops::ListOfLink&
                   GetAdjacentLinks (int theNode) const;

  //! draws link in debug mode (if DRAW_DEBUG is defined)
  Standard_EXPORT virtual void OnAddLink
                   (int theNum, const Poly_MakeLoops::Link& theLink) const;

  struct SewedNode
  {
    gp_XY Pnt;
    Poly_MakeLoops::ListOfLink Links;

    SewedNode()
      : Pnt   (0,0),
        Links (/*NCollection_HeapAllocator::GlobalHeapAllocator()*/)
    {}
  };

  struct LinkData
  {
    gp_Dir2d       FirstDir;
    gp_Dir2d       LastDir;
    Poly_Segment2d Segment;
    Handle(Poly_PolyCurve2d) Curve;

    LinkData() {}
    LinkData(const Poly_Segment2d& theSeg)
      : Segment (theSeg)
    { FirstDir = LastDir = theSeg.Dir(); }
    LinkData(const Handle(Poly_PolyCurve2d)& theCurve,
             const gp_XY& theNode1,
             const gp_XY& theNode2);
  };

  //! This inspector will find a node nearest to the given point
  //! not far than on the given tolerance
  class NodeInspector: public NCollection_CellFilter_InspectorXY
  {
  public:
    typedef Standard_Integer Target;  // node index as the target object

    NodeInspector (const NCollection_Vector<Poly_Segment2dSewer::SewedNode>& theNodes,
                   const Standard_Real theSqTol,
                   const gp_XY& thePnt)
      : myNodes (theNodes),
        myPnt (thePnt),
        myMinSqDist (theSqTol + 1e-10),
        myExcludeId (0),
        myResId (0)
    {}

    inline void SetExcludeId(const Standard_Integer theId)
    { myExcludeId = theId; }

    Standard_EXPORT NCollection_CellFilter_Action Inspect (const Target& theId);

    inline Standard_Integer GetResult () const
    { return myResId; }

  private:
    NodeInspector& operator = (const NodeInspector&);

    const NCollection_Vector<Poly_Segment2dSewer::SewedNode>& myNodes;
    gp_XY            myPnt;
    Standard_Real    myMinSqDist;
    Standard_Integer myExcludeId;
    Standard_Integer myResId;
  };

  //! This inspector creates links between the given node and other nodes 
  //! in the cell filter found on the distance not far then on the given tolerance
  class NodeInspectorAndLinker: public NCollection_CellFilter_InspectorXY
  {
  public:
    typedef Standard_Integer Target;  // node index as the target object

    Standard_EXPORT NodeInspectorAndLinker
      (NCollection_Vector<Poly_Segment2dSewer::SewedNode>& theNodes,
       NCollection_DataMap<Poly_MakeLoops::Link,
                           Poly_Segment2dSewer::LinkData>& theLinkDataMap,
       Poly_MakeLoops::ListOfLink& theOutLinks,
       const Standard_Real theSqTol);

    Standard_EXPORT void SetCurrentNode (const Standard_Integer theNodeId);

    Standard_EXPORT NCollection_CellFilter_Action Inspect (const Target& theId);

  private:
    NodeInspectorAndLinker& operator = (const NodeInspectorAndLinker&);

    NCollection_Vector<Poly_Segment2dSewer::SewedNode>& myNodes;
    NCollection_DataMap<Poly_MakeLoops::Link,LinkData>& myLinkDataMap;
    Poly_MakeLoops::ListOfLink&                         myOutLinks;
    Standard_Real                                       mySqTol;
    Standard_Real                                       mySqMinDist;
    Standard_Integer                                    myNodeId;
    Poly_Segment2dSewer::SewedNode*                     myNode;
    const Poly_Segment2dSewer::LinkData*                myLinkData;
    const gp_XY*                                        myLinkDir;
    Standard_Boolean                                    myIsOrigin;
    NCollection_Map<Poly_MakeLoops::Link>               myNewLinks;
  };

private:

  Standard_Boolean findOrCreateNodes
                   (const gp_XY& theP0, const gp_XY& theP1,
                    Standard_Integer& theId0, Standard_Integer& theId1);
  Standard_Integer findNode   (const gp_XY& thePnt,
                               gp_XY& thePMin, gp_XY& thePMax);
  Standard_Integer createNode (const gp_XY& thePnt,
                               const gp_XY& thePMin, const gp_XY& thePMax);

  Standard_Real                                             myTol;
  Standard_Real                                             mySqTol;
  NCollection_Vector<SewedNode>                             myNodes;
  NCollection_DataMap<Poly_MakeLoops::Link,LinkData>        myLinkDataMap;
  NCollection_CellFilter<NodeInspector>                     myCells;
};

#endif
