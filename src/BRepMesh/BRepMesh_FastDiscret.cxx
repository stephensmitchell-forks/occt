// Created on: 1996-02-27
// Created by: Ekaterina SMIRNOVA
// Copyright (c) 1996-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#include <BRepMesh_FastDiscret.hxx>

#include <BRepMesh_WireChecker.hxx>
#include <BRepMesh_FastDiscretFace.hxx>
#include <BRepMesh_FaceAttribute.hxx>
#include <BRepMesh_DataStructureOfDelaun.hxx>
#include <BRepMesh_GeomTool.hxx>
#include <BRepMesh_PairOfPolygon.hxx>
#include <BRepMesh_Classifier.hxx>
#include <BRepMesh_EdgeParameterProvider.hxx>
#include <BRepMesh_IEdgeTool.hxx>
#include <BRepMesh_EdgeTessellator.hxx>
#include <BRepMesh_EdgeTessellationExtractor.hxx>

#include <BRepAdaptor_Curve.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepAdaptor_HSurface.hxx>

#include <Bnd_Box.hxx>
#include <BRepTools.hxx>
#include <BRepBndLib.hxx>
#include <BndLib_Add3dCurve.hxx>
#include <Poly_Triangulation.hxx>
#include <Poly_PolygonOnTriangulation.hxx>

#include <Precision.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <Geom_Surface.hxx>
#include <Geom_Plane.hxx>
#include <GeomAbs_SurfaceType.hxx>
#include <Extrema_LocateExtPC.hxx>

#include <TColStd_Array1OfInteger.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <TColGeom2d_SequenceOfCurve.hxx>

#include <TopTools_SequenceOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>

#include <TopAbs.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>

#include <OSD_Parallel.hxx>

#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <NCollection_IncAllocator.hxx>

#include <BRep_ListIteratorOfListOfPointRepresentation.hxx>
#include <BRep_PointRepresentation.hxx>
#include <ShapeAnalysis_Wire.hxx>
#include <ShapeAnalysis_WireOrder.hxx>

#include <vector>

IMPLEMENT_STANDARD_RTTIEXT(BRepMesh_FastDiscret,Standard_Transient)

#define UVDEFLECTION 1.e-05

//=======================================================================
//function : BRepMesh_FastDiscret
//purpose  : 
//=======================================================================
BRepMesh_FastDiscret::BRepMesh_FastDiscret( const Bnd_Box&         theBox,
                                            const BRepMesh_FastDiscret::Parameters& theParams)
   :
  myBoundaryVertices(new BRepMesh::DMapOfVertexInteger),
  myBoundaryPoints(new BRepMesh::DMapOfIntegerPnt),
  myParameters(theParams),
  myDtotale(0.)
{ 
  if ( myParameters.Relative )
    BRepMesh_ShapeTool::BoxMaxDimension(theBox, myDtotale);
}

//=======================================================================
//function : InitSharedFaces
//purpose  : 
//=======================================================================
void BRepMesh_FastDiscret::InitSharedFaces(const TopoDS_Shape& theShape)
{
  TopExp::MapShapesAndAncestors(theShape, TopAbs_EDGE, TopAbs_FACE, mySharedFaces);
}

//=======================================================================
//function : Perform(shape)
//purpose  : 
//=======================================================================
void BRepMesh_FastDiscret::Perform(const TopoDS_Shape& theShape)
{
  InitSharedFaces(theShape);

  std::vector<TopoDS_Face> aFaces;
  TopExp_Explorer anExplorer(theShape, TopAbs_FACE);
  for (; anExplorer.More(); anExplorer.Next())
  {
    const TopoDS_Face& aFace = TopoDS::Face(anExplorer.Current());
    Add(aFace);
    aFaces.push_back(aFace);
  }

  OSD_Parallel::ForEach(aFaces.begin(), aFaces.end(), *this, !myParameters.InParallel);
}


//=======================================================================
//function : Process
//purpose  : 
//=======================================================================
void BRepMesh_FastDiscret::Process(const TopoDS_Face& theFace) const
{
  Handle(BRepMesh_FaceAttribute) anAttribute;
  if (GetFaceAttribute(theFace, anAttribute))
  {
    try
    {
      OCC_CATCH_SIGNALS

      BRepMesh_FastDiscretFace aTool(myParameters.Angle, myParameters.MinSize, 
        myParameters.InternalVerticesMode, myParameters.ControlSurfaceDeflection);
      aTool.Perform(anAttribute);
    }
    catch (Standard_Failure)
    {
      anAttribute->SetStatus(BRepMesh_Failure);
    }
  }
}

//=======================================================================
//function : resetDataStructure
//purpose  : 
//=======================================================================
void BRepMesh_FastDiscret::resetDataStructure()
{
  Handle(NCollection_IncAllocator) aAllocator;
  if (myAttribute->ChangeStructure().IsNull())
    aAllocator = new NCollection_IncAllocator(BRepMesh::MEMORY_BLOCK_SIZE_HUGE);
  else
    aAllocator = myAttribute->ChangeStructure()->Allocator();

  myAttribute->Clear();
  aAllocator->Reset(Standard_False);
  Handle(BRepMesh_DataStructureOfDelaun) aStructure = 
    new BRepMesh_DataStructureOfDelaun(aAllocator);

  const Standard_Real aTolU = myAttribute->ToleranceU();
  const Standard_Real aTolV = myAttribute->ToleranceV();
  const Standard_Real uCellSize = 14.0 * aTolU;
  const Standard_Real vCellSize = 14.0 * aTolV;

  aStructure->Data()->SetCellSize ( uCellSize, vCellSize);
  aStructure->Data()->SetTolerance( aTolU    , aTolV    );

  myAttribute->ChangeStructure() = aStructure;
}

//=======================================================================
//function : discretizeFaceBoundary
//purpose  : 
//=======================================================================
Standard_Boolean BRepMesh_FastDiscret::discretizeFaceBoundary ()
{
  BRepMesh::HIMapOfInteger&            aVertexEdgeMap = myAttribute->ChangeVertexEdgeMap();
  BRepMesh::HDMapOfShapePairOfPolygon& aInternalEdges = myAttribute->ChangeInternalEdges();

  resetDataStructure();

  Standard_Real defedge;
  Standard_Integer nbEdge = 0;
  Standard_Real savangle = myParameters.Angle;
  Standard_Real cdef;
  Standard_Real maxdef = 2.* BRepMesh_ShapeTool::MaxFaceTolerance(myAttribute->Face ());

  Standard_Real defface = 0.;
  if (!myParameters.Relative)
    defface = Max(myParameters.Deflection, maxdef);

  NCollection_Sequence<TopoDS_Edge>   aFaceEdges;
  const TopoDS_Face&                  aFace = myAttribute->Face();
  const Handle(BRepAdaptor_HSurface)& gFace = myAttribute->Surface();
  TopExp_Explorer aWireIt(aFace, TopAbs_WIRE);
  for (; aWireIt.More(); aWireIt.Next())
  {
    if (!computeUVToleranceForVerticesOfEdgesOf(TopoDS::Wire(aWireIt.Current())))
    {
      myAttribute->SetStatus(BRepMesh_Failure);
      return Standard_False;
    }

    TopExp_Explorer aEdgeIt(aWireIt.Current(), TopAbs_EDGE);
    for (; aEdgeIt.More(); aEdgeIt.Next(), ++nbEdge)
    {
      const TopoDS_Edge& aEdge = TopoDS::Edge(aEdgeIt.Current());
      if (!myMapdefle.IsBound(aEdge))
      {
        if (myParameters.Relative)
        {
          if (myEdges.IsBound(aEdge))
          {
            const BRepMesh_PairOfPolygon& aPair = myEdges.Find(aEdge);
            const Handle(Poly_PolygonOnTriangulation)& aPolygon = aPair.First();
            defedge = aPolygon->Deflection();
          }
          else
          {
            defedge = BRepMesh_ShapeTool::RelativeEdgeDeflection(
              aEdge, myParameters.Deflection, myDtotale, cdef);

            myParameters.Angle = savangle * cdef;
          }

          defface += defedge;
          defface = Max(maxdef, defface);
        }
        else
        {
          defedge = myParameters.Deflection;
        }

        defedge = Max(maxdef, defedge);
        defedge = Max(UVDEFLECTION, defedge);
        myMapdefle.Bind(aEdge, defedge);
      }
      else
      {
        defedge = myMapdefle(aEdge);
        if ( myParameters.Relative )
        {
          defface += defedge;
          defface = Max(maxdef, defface);
        }
      }

      if (add(aEdge, defedge))
      {
        aFaceEdges.Append(aEdge);
      }

      myParameters.Angle = savangle;
    }
  }

  if ( nbEdge == 0 || aVertexEdgeMap->Extent() < 3 )
  {
    myAttribute->SetStatus(BRepMesh_Failure);
    return Standard_False;
  }

  if ( myParameters.Relative )
  {
    defface = defface / nbEdge;
  }
  else
  {
    defface = myParameters.Deflection;
  }

  defface = Max(maxdef, defface);

  TopLoc_Location aLoc;
  Handle(Poly_Triangulation) aTriangulation = BRep_Tool::Triangulation(aFace, aLoc);

  if ( aTriangulation.IsNull() )
  {
    Standard_Real xCur, yCur;
    Standard_Real maxX, minX, maxY, minY;

    minX = minY = 1.e100;
    maxX = maxY =-1.e100;

    Standard_Integer ipn = 0;
    Standard_Integer i1 = 1;
    for ( i1 = 1; i1 <= aVertexEdgeMap->Extent(); ++i1 )
    {
      const BRepMesh_Vertex& aVertex = 
        myAttribute->ChangeStructure()->GetNode(aVertexEdgeMap->FindKey(i1));

      ++ipn;

      xCur = aVertex.Coord().X();
      yCur = aVertex.Coord().Y();

      minX = Min(xCur, minX);
      maxX = Max(xCur, maxX);
      minY = Min(yCur, minY);
      maxY = Max(yCur, maxY);
    }

    Standard_Real myumin = minX;
    Standard_Real myumax = maxX;
    Standard_Real myvmin = minY;
    Standard_Real myvmax = maxY;

    const Standard_Real umin = gFace->FirstUParameter();
    const Standard_Real umax = gFace->LastUParameter();
    const Standard_Real vmin = gFace->FirstVParameter();
    const Standard_Real vmax = gFace->LastVParameter();

    if (myumin < umin || myumax > umax)
    {
      if (gFace->IsUPeriodic())
      {
        if ((myumax - myumin) > (umax - umin))
          myumax = myumin + (umax - umin);
      }
      else
      {
        if (umin > myumin)
          myumin = umin;

        if (umax < myumax)
          myumax = umax;
      }
    }

    if (myvmin < vmin || myvmax > vmax)
    {
      if (gFace->IsVPeriodic())
      {
        if ((myvmax - myvmin) > (vmax - vmin))
          myvmax = myvmin + (vmax - vmin);
      }
      else
      {
        if ( vmin > myvmin )
          myvmin = vmin;

        if (vmax < myvmax)
          myvmax = vmax;
      }
    }

    GeomAbs_SurfaceType aSurfType = gFace->GetType();
    // Fast verification of the validity of calculated limits.
    // If wrong, sure a problem of pcurve.
    if (aSurfType == GeomAbs_BezierSurface &&
        (myumin < -0.5 || myumax > 1.5 || myvmin < -0.5 || myvmax > 1.5) )
    {
      myAttribute->SetStatus(BRepMesh_Failure);
      return Standard_False;
    }

    //define parameters for correct parametrics
    Standard_Real deltaX = 1.0;
    Standard_Real deltaY = 1.0;

    {
      Standard_Real aTolU, aTolV;
      myAttribute->ChangeStructure()->Data()->GetTolerance(aTolU, aTolV);
      const Standard_Real aTol = Sqrt(aTolU * aTolU + aTolV * aTolV);

      BRepMesh::HClassifier& aClassifier = myAttribute->ChangeClassifier();
      BRepMesh_WireChecker aDFaceChecker(aFace, aTol, aInternalEdges, 
        aVertexEdgeMap, myAttribute->ChangeStructure(),
        myumin, myumax, myvmin, myvmax, myParameters.InParallel );

      aDFaceChecker.ReCompute(aClassifier);
      BRepMesh_Status aCheckStatus = aDFaceChecker.Status();

      if (aCheckStatus == BRepMesh_SelfIntersectingWire)
      {
        Standard_Integer nbmaill = 0;
        Standard_Real eps = Precision::Confusion();
        while (nbmaill < 5 && aCheckStatus != BRepMesh_ReMesh)
        {
          ++nbmaill;

          resetDataStructure();
          for (Standard_Integer j = 1; j <= aFaceEdges.Length(); ++j)
          {
            const TopoDS_Edge& anEdge = aFaceEdges(j);
            if (myEdges.IsBound(anEdge))
              myEdges.UnBind(anEdge);

            defedge = Max(myMapdefle(anEdge) / 3.0, eps);
            myMapdefle.Bind(anEdge, defedge);

            add(anEdge, defedge);
          }

          aDFaceChecker.ReCompute(aClassifier);
          if (aDFaceChecker.Status() == BRepMesh_NoError)
            aCheckStatus = BRepMesh_ReMesh;
        }
      }

      myAttribute->SetStatus(aCheckStatus);
      if (!myAttribute->IsValid())
      {
        return Standard_False;
      }
    }

    // try to find the real length:
    // akm (bug OCC16) : We must calculate these measures in non-singular
    //     parts of face. Let's try to compute average value of three
    //     (umin, (umin+umax)/2, umax), and respectively for v.
    //                 vvvvv
    Standard_Real longu = 0.0, longv = 0.0; //, last , first;
    gp_Pnt P11, P12, P21, P22, P31, P32;

    Standard_Real du = 0.05 * ( myumax - myumin );
    Standard_Real dv = 0.05 * ( myvmax - myvmin );
    Standard_Real dfuave = 0.5 * ( myumin + myumax );
    Standard_Real dfvave = 0.5 * ( myvmin + myvmax );
    Standard_Real dfucur, dfvcur;

    // U loop
    gFace->D0(myumin, myvmin, P11);
    gFace->D0(myumin, dfvave, P21);
    gFace->D0(myumin, myvmax, P31);
    for (i1=1, dfucur=myumin+du; i1 <= 20; i1++, dfucur+=du)
    {
      gFace->D0(dfucur, myvmin, P12);
      gFace->D0(dfucur, dfvave, P22);
      gFace->D0(dfucur, myvmax, P32);
      longu += ( P11.Distance(P12) + P21.Distance(P22) + P31.Distance(P32) );
      P11 = P12;
      P21 = P22;
      P31 = P32;
    }

    // V loop
    gFace->D0(myumin, myvmin, P11);
    gFace->D0(dfuave, myvmin, P21);
    gFace->D0(myumax, myvmin, P31);
    for (i1=1, dfvcur=myvmin+dv; i1 <= 20; i1++, dfvcur+=dv)
    {
      gFace->D0(myumin, dfvcur, P12);
      gFace->D0(dfuave, dfvcur, P22);
      gFace->D0(myumax, dfvcur, P32);
      longv += ( P11.Distance(P12) + P21.Distance(P22) + P31.Distance(P32) );
      P11 = P12;
      P21 = P22;
      P31 = P32;
    }

    longu /= 3.;
    longv /= 3.;
    // akm (bug OCC16) ^^^^^

    if (longu <= 1.e-16 || longv <= 1.e-16)
    {
      //yes, it is seen!!
      myAttribute->SetStatus(BRepMesh_Failure);
      return Standard_False;
    }


    if (aSurfType == GeomAbs_Torus)
    {
      gp_Torus Tor = gFace->Torus();
      Standard_Real r = Tor.MinorRadius(), R = Tor.MajorRadius();
      Standard_Real Du, Dv;//, pasu, pasv;

      Dv = Max(1.0e0 - (defface/r),0.0e0) ;
      Standard_Real oldDv = 2.0 * ACos (Dv);
      oldDv = Min(oldDv, myParameters.Angle);
      Dv  =  0.9*oldDv; //TWOTHIRD * oldDv;
      Dv = oldDv;

      Standard_Integer nbV = Max((Standard_Integer)((myvmax-myvmin)/Dv), 2);
      Dv = (myvmax-myvmin)/(nbV+1);

      Standard_Real ru = R + r;
      if ( ru > 1.e-16 )
      {
        Du = Max(1.0e0 - (defface/ru),0.0e0);
        Du  = (2.0 * ACos (Du));
        Du = Min(Du, myParameters.Angle);
        Standard_Real aa = sqrt(Du*Du + oldDv*oldDv);

        if (aa < gp::Resolution())
        {
          return Standard_False;
        }

        Du = Du * Min(oldDv, Du) / aa;
      }
      else
      {
        Du = Dv;
      }

      Standard_Integer nbU = Max((Standard_Integer)((myumax-myumin)/Du), 2);
      nbU = Max(nbU, (Standard_Integer)(nbV*(myumax-myumin)*R/((myvmax-myvmin)*r)/5.));
      
      Du = (myumax-myumin)/(nbU+1);
      //-- DeltaX and DeltaY are chosen so that to avoid "jumping" 
      //-- of points on the grid
      deltaX = Du;
      deltaY = Dv;
    }
    else if (aSurfType == GeomAbs_Cylinder)
    {
      gp_Cylinder Cyl = gFace->Cylinder();
      Standard_Real R = Cyl.Radius();

      // Calculate parameters for iteration in U direction
      Standard_Real Du = 1.0 - (defface/R);
      if (Du < 0.0)
        Du = 0.0;

      Du = 2.0 * ACos (Du);
      if (Du > myParameters.Angle)
        Du = myParameters.Angle;

      deltaX = Du / longv;
      deltaY = 1.;
    }
    else
    {
      deltaX = (myumax-myumin)/longu;
      deltaY = (myvmax-myvmin)/longv;
    }

    // Restore face attribute
    myAttribute->SetDefFace(defface);
    myAttribute->SetUMax(myumax);
    myAttribute->SetVMax(myvmax);
    myAttribute->SetUMin(myumin);
    myAttribute->SetVMin(myvmin);
    myAttribute->SetDeltaX(deltaX);
    myAttribute->SetDeltaY(deltaY);
  }

  return Standard_True;
}

//=======================================================================
//function : Add(face)
//purpose  : 
//=======================================================================
Standard_Integer BRepMesh_FastDiscret::Add(const TopoDS_Face& theFace)
{
  myAttribute.Nullify();
  GetFaceAttribute(theFace, myAttribute, Standard_True);

  try
  {
    OCC_CATCH_SIGNALS

    // Initialize face attributes
    if (!myAttribute->IsInitialized ())
      myAttribute->SetFace (theFace, myParameters.AdaptiveMin);

    if (discretizeFaceBoundary ())
    {
      myAttribute->ChangeMeshNodes() = 
        myAttribute->ChangeStructure()->Data()->Vertices();
    }
  }
  catch(Standard_Failure)
  {
    myAttribute->SetStatus(BRepMesh_Failure);
  }

  myDegenerativeEdgesCache.Clear ();
  myEdgeVerticesUVTolCache.Clear ();

  myAttribute->ChangeStructure().Nullify();
  return myAttribute->GetStatus();
}

//=======================================================================
//function : getEdgeAttributes
//purpose  :
//=======================================================================
Standard_Boolean BRepMesh_FastDiscret::getEdgeAttributes(
  const TopoDS_Edge&                      theEdge,
  const Standard_Real                     theDefEdge,
  BRepMesh_FastDiscret::EdgeAttributes&   theAttributes,
  Handle(Geom2d_Curve)&                   thePCurve) const
{
  EdgeAttributes& aEAttr = theAttributes;

  // Get vertices
  TopExp::Vertices(theEdge, aEAttr.FirstVertex, aEAttr.LastVertex);
  if (aEAttr.FirstVertex.IsNull() || aEAttr.LastVertex.IsNull())
    return Standard_False;

  // Get range on 2d curve
  const TopoDS_Face& aFace = myAttribute->Face();
  BRepMesh_ShapeTool::Range (theEdge, aFace, thePCurve, aEAttr.FirstParam, aEAttr.LastParam);
  if (thePCurve.IsNull ())
  {
    return Standard_False;
  }

  // Get end points on 2d curve
  BRepMesh_ShapeTool::UVPoints (theEdge, aFace, aEAttr.FirstUV, aEAttr.LastUV);

  aEAttr.IsSameUV = aEAttr.FirstUV.IsEqual(aEAttr.LastUV, Precision::PConfusion());

  //Control tolerance of vertices
  const Handle(BRepAdaptor_HSurface)& gFace = myAttribute->Surface();
  gp_Pnt pFirst = gFace->Value(aEAttr.FirstUV.X(), aEAttr.FirstUV.Y());
  gp_Pnt pLast  = gFace->Value(aEAttr.LastUV.X(),  aEAttr.LastUV.Y());

  aEAttr.MinDist = 10. * Max(pFirst.Distance(BRep_Tool::Pnt(aEAttr.FirstVertex)),
                             pLast .Distance(BRep_Tool::Pnt(aEAttr.LastVertex)));

  if (aEAttr.MinDist < BRep_Tool::Tolerance(aEAttr.FirstVertex) ||
      aEAttr.MinDist < BRep_Tool::Tolerance(aEAttr.LastVertex))
  {
    aEAttr.MinDist = theDefEdge;
  }

  return Standard_True;
}

//=======================================================================
//function : computeUVToleranceForVerticesOf
//purpose  :
//=======================================================================
BRepMesh::Segment& BRepMesh_FastDiscret::computeUVToleranceForVerticesOf(
  const TopoDS_Edge& theEdge,
  const TopoDS_Edge& thePrevEdge,
  const TopoDS_Edge& theNextEdge,
  gp_XY&             theDiff)
{

  const TopoDS_Face& aFace = myAttribute->Face ();

  // Take UV points of current edge.
  // Passing orientation flag to UVPoints we suppose to obtain oriented chain of edges.
  // By this we always compute tolerance for vertices of forward edge.
  gp_Pnt2d aCurrFirstPoint, aCurrLastPoint;
  BRepMesh_ShapeTool::UVPoints (theEdge, aFace, aCurrFirstPoint, aCurrLastPoint, Standard_True);

  // Compute UV tolerance using last vertex of previous edge.
  gp_Pnt2d aTmpPoint;
  gp_Pnt2d aPrevPoint;
  BRepMesh_ShapeTool::UVPoints (thePrevEdge, aFace, aTmpPoint, aPrevPoint, Standard_True);

  BRepMesh::Segment aEdgeVerticesUVTol;
  gp_XY& aFirstVertexUVTol = aEdgeVerticesUVTol.StartPnt;
  aFirstVertexUVTol = (aCurrFirstPoint.XY () - aPrevPoint.XY ());
  aFirstVertexUVTol.SetX (Abs (aFirstVertexUVTol.X ()));
  aFirstVertexUVTol.SetY (Abs (aFirstVertexUVTol.Y ()));

  // Compute UV tolerance using first vertex of next edge.
  gp_Pnt2d aNextPoint;
  BRepMesh_ShapeTool::UVPoints (theNextEdge, aFace, aNextPoint, aTmpPoint, Standard_True);

  gp_XY& aLastVertexUVTol = aEdgeVerticesUVTol.EndPnt;
  aLastVertexUVTol = (aCurrLastPoint.XY () - aNextPoint.XY ());
  aLastVertexUVTol.SetX (Abs (aLastVertexUVTol.X ()));
  aLastVertexUVTol.SetY (Abs (aLastVertexUVTol.Y ()));

  const gp_XY aTol (Precision::PConfusion (), Precision::PConfusion ());
  aFirstVertexUVTol.Add (aTol);
  aLastVertexUVTol .Add (aTol);

  theDiff = (aCurrFirstPoint.XY () - aCurrLastPoint.XY ());

  myEdgeVerticesUVTolCache.Bind (theEdge, aEdgeVerticesUVTol);
  return myEdgeVerticesUVTolCache.ChangeFind (theEdge);
}

//=======================================================================
//function : initUVToleranceForVerticesOfEdgesOf
//purpose  :
//=======================================================================
void BRepMesh_FastDiscret::initUVToleranceForVerticesOfEdgesOf(
  const Handle(ShapeExtend_WireData)& theWireData,
  const ShapeAnalysis_WireOrder&      theOrderTool)
{

  // Compute initial values of vertex tolerances.
  const Standard_Integer aEdgesNb = theOrderTool.NbEdges ();
  Standard_Integer aPrevIdx = theOrderTool.Ordered (aEdgesNb);
  for (Standard_Integer i = 1; i <= aEdgesNb; ++i)
  {
    const Standard_Integer aCurrIdx = theOrderTool.Ordered (i);
    const Standard_Integer aNextIdx = theOrderTool.Ordered ((i % aEdgesNb) + 1);

    const TopoDS_Edge& aPrevEdge = theWireData->Edge (aPrevIdx);
    const TopoDS_Edge& aCurrEdge = theWireData->Edge (aCurrIdx);
    const TopoDS_Edge& aNextEdge = theWireData->Edge (aNextIdx);

    gp_XY aDiff;
    BRepMesh::Segment& aEdgeVerticesUVTol = computeUVToleranceForVerticesOf (
      aCurrEdge, aPrevEdge, aNextEdge, aDiff);

    gp_XY& aFirstVertexUVTol = aEdgeVerticesUVTol.StartPnt;
    gp_XY&  aLastVertexUVTol = aEdgeVerticesUVTol.EndPnt;

    // In case of degenerative edge if distance beween its end-points is not null
    // and it is fully covered by tolerance of its vertex, it is necessary to
    // enchance computed tolerance by the edge length in order to prevent occasion
    // of a gap when one end-point is replaced by another one due to tolerance
    // coincidence whereas there is a point on another edge lying in tolerance
    // with replaced one only.
    const Standard_Real aFirstSqTol = aFirstVertexUVTol.SquareModulus ();
    const Standard_Real  aLastSqTol =  aLastVertexUVTol.SquareModulus ();

    gp_XY& aMaxTol = (aFirstSqTol > aLastSqTol) ? aFirstVertexUVTol : aLastVertexUVTol;
    if (aDiff.SquareModulus () < aMaxTol.SquareModulus ())
    {

      aMaxTol.SetX (aMaxTol.X () + Abs (aDiff.X ()));
      aMaxTol.SetY (aMaxTol.Y () + Abs (aDiff.Y ()));

      aFirstVertexUVTol = aMaxTol;
      aLastVertexUVTol  = aMaxTol;
    }

    aPrevIdx = aCurrIdx;
  }
}

//=======================================================================
//function : updateUVToleranceForVerticesOfEdgesOf
//purpose  :
//=======================================================================
void BRepMesh_FastDiscret::updateUVToleranceForVerticesOfEdgesOf(
  const Handle(ShapeExtend_WireData)& theWireData,
  const ShapeAnalysis_WireOrder&      theOrderTool)
{

  // Update adjusted values due to enchanced degenerative cases.
  const Standard_Integer aEdgesNb = theOrderTool.NbEdges ();
  Standard_Integer aPrevIdx = theOrderTool.Ordered (aEdgesNb);
  for (Standard_Integer i = 1; i <= aEdgesNb; ++i)
  {
    const Standard_Integer aCurrIdx = theOrderTool.Ordered (i);
    const TopoDS_Edge& aCurrEdge = theWireData->Edge (aCurrIdx);

    // Similar tolerance on edges - it could be a degenerative edge fully covered by tolerance
    // or just edge with similar tolerance at its end, so lets check and update its neighbours.
    BRepMesh::Segment& aCurrTolUV = myEdgeVerticesUVTolCache.ChangeFind (aCurrEdge);
    const gp_XY aTolDiffUV = (aCurrTolUV.StartPnt - aCurrTolUV.EndPnt);
    if (aTolDiffUV.SquareModulus () < Precision::SquareConfusion ())
    {
      const TopoDS_Edge& aPrevEdge = theWireData->Edge (aPrevIdx);
      gp_XY& aPrevLastTolUV = myEdgeVerticesUVTolCache.ChangeFind (aPrevEdge).EndPnt;

      const Standard_Integer aNextIdx = theOrderTool.Ordered ((i % aEdgesNb) + 1);
      const TopoDS_Edge& aNextEdge = theWireData->Edge (aNextIdx);
      gp_XY& aNextFirstTolUV = myEdgeVerticesUVTolCache.ChangeFind (aNextEdge).StartPnt;

      BRepMesh::Segment& aCurrTolUV = myEdgeVerticesUVTolCache.ChangeFind (aCurrEdge);
      if (aCurrTolUV.StartPnt.X () > aPrevLastTolUV.X () ||
          aCurrTolUV.StartPnt.Y () > aPrevLastTolUV.Y ())
      {
        aPrevLastTolUV = aCurrTolUV.StartPnt;
      }

      if (aCurrTolUV.EndPnt.X () > aNextFirstTolUV.X () ||
          aCurrTolUV.EndPnt.Y () > aNextFirstTolUV.Y ())
      {
        aNextFirstTolUV = aCurrTolUV.EndPnt;
      }
    }

    aPrevIdx = aCurrIdx;
  }
}

//=======================================================================
//function : computeUVToleranceForVerticesOfEdgesOf
//purpose  : 
//=======================================================================
Standard_Boolean BRepMesh_FastDiscret::computeUVToleranceForVerticesOfEdgesOf(
  const TopoDS_Wire& theWire)
{

  ShapeAnalysis_Wire aWireTool (theWire, myAttribute->Face(), Precision::Confusion());

  ShapeAnalysis_WireOrder aOrderTool;
  aWireTool.CheckOrder (aOrderTool, Standard_True, Standard_False);
  if (aWireTool.LastCheckStatus (ShapeExtend_FAIL))
  {
    return Standard_False;
  }

  if (aWireTool.LastCheckStatus (ShapeExtend_DONE3))
  {
    myAttribute->SetStatus (BRepMesh_UnorientedWire);
  }

  const Handle(ShapeExtend_WireData)& aWireData = aWireTool.WireData ();
  if (aOrderTool.NbEdges () != aWireData->NbEdges ())
  {
    return Standard_False;
  }

  initUVToleranceForVerticesOfEdgesOf   (aWireData, aOrderTool);
  updateUVToleranceForVerticesOfEdgesOf (aWireData, aOrderTool);

  return Standard_True;
}

//=======================================================================
//function : toleranceUV
//purpose  : 
//=======================================================================
gp_XY& BRepMesh_FastDiscret::toleranceUV (
  const TopoDS_Edge&     theEdge,
  const Standard_Boolean isFirstVertex) const
{

  if (!myEdgeVerticesUVTolCache.IsBound (theEdge))
  {
    // Internal edges, etc.
    BRepMesh::Segment aVerticesUVTol;
    gp_XY& aFirstVertexTolUV = aVerticesUVTol.StartPnt;
    gp_XY&  aLastVertexTolUV = aVerticesUVTol.EndPnt;

    const Standard_Real aTolU = 0.5 * (myAttribute->GetUMax () - myAttribute->GetUMin ());
    const Standard_Real aTolV = 0.5 * (myAttribute->GetVMax () - myAttribute->GetVMin ());
    aFirstVertexTolUV.SetX (aTolU);
    aFirstVertexTolUV.SetY (aTolV);

    aLastVertexTolUV.SetX (aTolU);
    aLastVertexTolUV.SetY (aTolV);

    myEdgeVerticesUVTolCache.Bind (theEdge, aVerticesUVTol);
  }

  BRepMesh::Segment& aVerticesUVTol = myEdgeVerticesUVTolCache.ChangeFind (theEdge);

  const Standard_Boolean isForward = (theEdge.Orientation () != TopAbs_REVERSED);
  gp_XY& aFirstTolUV = isForward ? aVerticesUVTol.StartPnt : aVerticesUVTol.EndPnt;
  gp_XY& aLastTolUV  = isForward ? aVerticesUVTol.EndPnt   : aVerticesUVTol.StartPnt;

  return isFirstVertex ?
    aFirstTolUV :
    aLastTolUV;
}

//=======================================================================
//function : registerEdgeVertices
//purpose  : 
//=======================================================================
void BRepMesh_FastDiscret::registerEdgeVertices(
  const TopoDS_Edge&                    theEdge,
  BRepMesh_FastDiscret::EdgeAttributes& theAttributes,
  Standard_Integer&                     ipf,
  Standard_Integer&                     ivf,
  Standard_Integer&                     isvf,
  Standard_Integer&                     ipl,
  Standard_Integer&                     ivl,
  Standard_Integer&                     isvl)
{
  EdgeAttributes& aEAttr = theAttributes;
  if (aEAttr.FirstVExtractor.IsNull())
  {
    // Use edge geometry to produce tesselation.
    aEAttr.FirstVExtractor = 
      new TopoDSVExplorer(aEAttr.FirstVertex, aEAttr.IsSameUV, aEAttr.LastVertex);
  }

  if (aEAttr.LastVExtractor.IsNull())
  {
    // Use edge geometry to produce tesselation.
    aEAttr.LastVExtractor = 
      new TopoDSVExplorer(aEAttr.LastVertex, aEAttr.IsSameUV, aEAttr.FirstVertex);
  }

  gp_XY aTmpUV;
  // Process first vertex
  ipf = myAttribute->GetVertexIndex(aEAttr.FirstVExtractor, Standard_True);

  Standard_Real aMinDist = Max (2. * BRep_Tool::Tolerance (aEAttr.FirstVertex), aEAttr.MinDist);
  aTmpUV = BRepMesh_ShapeTool::FindUV (ipf, aEAttr.FirstUV, aMinDist,
    toleranceUV (theEdge, Standard_True), myAttribute);

  myAttribute->AddNode(ipf, aTmpUV, BRepMesh_Frontier, ivf, isvf);

  // Process last vertex
  ipl = aEAttr.LastVertex.IsSame(aEAttr.FirstVertex) ? ipf :
    myAttribute->GetVertexIndex(aEAttr.LastVExtractor, Standard_True);

  aMinDist = Max (2. * BRep_Tool::Tolerance (aEAttr.LastVertex), aEAttr.MinDist);
  aTmpUV = BRepMesh_ShapeTool::FindUV (ipl, aEAttr.LastUV, aMinDist,
    toleranceUV (theEdge, Standard_False), myAttribute);

  myAttribute->AddNode(ipl, aTmpUV, BRepMesh_Frontier, ivl, isvl);
}

//=======================================================================
//function : add
//purpose  : 
//=======================================================================
Standard_Boolean BRepMesh_FastDiscret::add(
  const TopoDS_Edge&  theEdge,
  const Standard_Real theDefEdge)
{
  const TopAbs_Orientation orEdge = theEdge.Orientation();
  if (orEdge == TopAbs_EXTERNAL)
    return Standard_False;

  EdgeAttributes aEAttr;
  Handle(Geom2d_Curve) aCurve2d;
  if (!getEdgeAttributes(theEdge, theDefEdge, aEAttr, aCurve2d))
    return Standard_False;

  if (!myEdges.IsBound(theEdge))
  {
    update(theEdge, aCurve2d, theDefEdge, aEAttr);
    return Standard_True;
  }

  Standard_Integer ipf, ivf, isvf, ipl, ivl, isvl;
  registerEdgeVertices(theEdge, aEAttr, ipf, ivf, isvf, ipl, ivl, isvl);

  // If this Edge has been already checked and it is not degenerated, 
  // the points of the polygon calculated at the first check are retrieved :

  // retrieve the polygone:
  const BRepMesh_PairOfPolygon&              aPair    = myEdges.Find(theEdge);
  const Handle(Poly_PolygonOnTriangulation)& aPolygon = aPair.First();
  const TColStd_Array1OfInteger&             aNodes   = aPolygon->Nodes();
  Handle(TColStd_HArray1OfReal)              aParams  = aPolygon->Parameters();

  // creation anew:
  const Standard_Integer  aNodesNb = aNodes.Length();
  TColStd_Array1OfInteger aNewNodes (1, aNodesNb);
  TColStd_Array1OfReal    aNewParams(1, aNodesNb);

  aNewNodes (1) = isvf;
  aNewParams(1) = aEAttr.FirstParam;

  aNewNodes (aNodesNb) = isvl;
  aNewParams(aNodesNb) = aEAttr.LastParam;

  const TopoDS_Face& aFace = myAttribute->Face();
  if (!isDegenerated (theEdge))
  {
    BRepMesh_EdgeParameterProvider aProvider(theEdge, aFace, aParams);
    for (Standard_Integer i = 2; i < aNodesNb; ++i)
    {
      const Standard_Integer aPointId = aNodes(i);
      const gp_Pnt& aPnt = myBoundaryPoints->Find(aPointId);

      const Standard_Real aParam = aProvider.Parameter(i, aPnt);
      gp_Pnt2d aUV = aCurve2d->Value(aParam);

      Standard_Integer iv2, isv;
      myAttribute->AddNode(aPointId, aUV.Coord(), BRepMesh_OnCurve, iv2, isv);

      aNewNodes (i) = isv;
      aNewParams(i) = aParam;
    }
  }

  Handle(Poly_PolygonOnTriangulation) P1 = 
    new Poly_PolygonOnTriangulation(aNewNodes, aNewParams);

  storePolygon(theEdge, P1, theDefEdge);
  return Standard_True;
}

//=======================================================================
//function : update(edge)
//purpose  :
//=======================================================================
void BRepMesh_FastDiscret::update(
  const TopoDS_Edge&                                theEdge,
  const Handle(Geom2d_Curve)&                       theC2d,
  const Standard_Real                               theDefEdge,
  BRepMesh_FastDiscret::EdgeAttributes&             theAttributes)
{
  EdgeAttributes& aEAttr = theAttributes;

  const TopoDS_Face& aFace = myAttribute->Face();
  Handle(BRepMesh_IEdgeTool) aEdgeTool;
  // Try to find existing tessellation.
  for (Standard_Integer i = 1; aEdgeTool.IsNull(); ++i)
  {
    TopLoc_Location aLoc;
    Handle(Poly_Triangulation) aTriangulation;
    Handle(Poly_PolygonOnTriangulation) aPolygon;
    BRep_Tool::PolygonOnTriangulation(theEdge, aPolygon, aTriangulation, aLoc, i);

    if (aPolygon.IsNull())
      break;

    if (aTriangulation.IsNull() || !aPolygon->HasParameters())
      continue;

    if (aPolygon->Deflection() > 1.1 * theDefEdge)
      continue;

    const TColgp_Array1OfPnt&      aNodes   = aTriangulation->Nodes();
    const TColStd_Array1OfInteger& aIndices = aPolygon->Nodes();
    Handle(TColStd_HArray1OfReal)  aParams  = aPolygon->Parameters();

    aEAttr.FirstVExtractor = new PolyVExplorer(aEAttr.FirstVertex, 
      aEAttr.IsSameUV, aEAttr.LastVertex, aIndices(1), aNodes, aLoc);

    aEAttr.LastVExtractor = new PolyVExplorer(aEAttr.LastVertex, 
      aEAttr.IsSameUV, aEAttr.FirstVertex, aIndices(aIndices.Length()), aNodes, aLoc);

    aEdgeTool = new BRepMesh_EdgeTessellationExtractor(theEdge, theC2d, 
      aFace, aTriangulation, aPolygon, aLoc);
  }

  if (aEdgeTool.IsNull())
  {
    aEdgeTool = new BRepMesh_EdgeTessellator(theEdge, myAttribute, 
      mySharedFaces, theDefEdge, myParameters.Angle, myParameters.MinSize);
  }

  Standard_Integer ipf, ivf, isvf, ipl, ivl, isvl;
  registerEdgeVertices(theEdge, aEAttr, ipf, ivf, isvf, ipl, ivl, isvl);

  Handle(Poly_PolygonOnTriangulation) P1, P2;
  if (isDegenerated (theEdge))
  {
    const Standard_Integer  aNodesNb = 2;
    TColStd_Array1OfInteger aNewNodes      (1, aNodesNb);
    TColStd_Array1OfInteger aNewNodInStruct(1, aNodesNb);
    TColStd_Array1OfReal    aNewParams     (1, aNodesNb);

    aNewNodInStruct(1) = ipf;
    aNewNodes      (1) = isvf;
    aNewParams     (1) = aEAttr.FirstParam;

    aNewNodInStruct(aNodesNb) = ipl;
    aNewNodes      (aNodesNb) = isvl;
    aNewParams     (aNodesNb) = aEAttr.LastParam;

    P1 = new Poly_PolygonOnTriangulation(aNewNodes,       aNewParams);
    P2 = new Poly_PolygonOnTriangulation(aNewNodInStruct, aNewParams);
  }
  else
  {
    const Standard_Integer  aNodesNb = aEdgeTool->NbPoints();
    TColStd_Array1OfInteger aNewNodesVec        (1, aNodesNb);
    TColStd_Array1OfInteger aNewNodesInStructVec(1, aNodesNb);
    TColStd_Array1OfReal    aNewParamsVec       (1, aNodesNb);

    Standard_Integer aNodesCount = 1;
    aNewNodesInStructVec(aNodesCount) = ipf;
    aNewNodesVec        (aNodesCount) = isvf;
    aNewParamsVec       (aNodesCount) = aEAttr.FirstParam;

    ++aNodesCount;
    Standard_Integer aLastPointId = myAttribute->LastPointId();
    for (Standard_Integer i = 2; i < aNodesNb; ++i)
    {
      gp_Pnt        aPnt;
      gp_Pnt2d      aUV;
      Standard_Real aParam;
      if (!aEdgeTool->Value(i, aParam, aPnt, aUV))
        continue;

      myBoundaryPoints->Bind(++aLastPointId, aPnt);

      Standard_Integer iv2, isv;
      myAttribute->AddNode(aLastPointId, aUV.Coord(), BRepMesh_Frontier, iv2, isv);

      aNewNodesInStructVec(aNodesCount) = aLastPointId;
      aNewNodesVec        (aNodesCount) = isv;
      aNewParamsVec       (aNodesCount) = aParam;
      ++aNodesCount;
    }

    aNewNodesInStructVec(aNodesCount) = ipl;
    aNewNodesVec        (aNodesCount) = isvl;
    aNewParamsVec       (aNodesCount) = aEAttr.LastParam;

    TColStd_Array1OfInteger aNewNodes      (aNewNodesVec.First (),        1, aNodesCount);
    TColStd_Array1OfInteger aNewNodInStruct(aNewNodesInStructVec.First(), 1, aNodesCount);
    TColStd_Array1OfReal    aNewParams     (aNewParamsVec.First(),        1, aNodesCount);

    P1 = new Poly_PolygonOnTriangulation(aNewNodes,       aNewParams);
    P2 = new Poly_PolygonOnTriangulation(aNewNodInStruct, aNewParams);
  }

  storePolygon(theEdge, P1, theDefEdge);
  storePolygonSharedData(theEdge, P2, theDefEdge);
}

//=======================================================================
//function : storeInternalPolygon
//purpose  : 
//=======================================================================
void BRepMesh_FastDiscret::storePolygon(
  const TopoDS_Edge&                         theEdge,
  Handle(Poly_PolygonOnTriangulation)&       thePolygon,
  const Standard_Real                        theDeflection)
{
  thePolygon->Deflection(theDeflection);
  BRepMesh::HDMapOfShapePairOfPolygon& aInternalEdges = myAttribute->ChangeInternalEdges();
  if (aInternalEdges->IsBound(theEdge))
  {
    BRepMesh_PairOfPolygon& aPair = aInternalEdges->ChangeFind(theEdge);
    if (theEdge.Orientation() == TopAbs_REVERSED)
      aPair.Append(thePolygon);
    else
      aPair.Prepend(thePolygon);

    return;
  }

  BRepMesh_PairOfPolygon aPair;
  aPair.Append(thePolygon);
  aInternalEdges->Bind(theEdge, aPair);
}

//=======================================================================
//function : storePolygonSharedData
//purpose  : 
//=======================================================================
void BRepMesh_FastDiscret::storePolygonSharedData(
  const TopoDS_Edge&                         theEdge,
  Handle(Poly_PolygonOnTriangulation)&       thePolygon,
  const Standard_Real                        theDeflection)
{
  thePolygon->Deflection(theDeflection);
  BRepMesh_PairOfPolygon aPair;
  aPair.Append(thePolygon);
  myEdges.Bind(theEdge, aPair);
}

//=======================================================================
//function : GetFaceAttribute
//purpose  : 
//=======================================================================
Standard_Boolean BRepMesh_FastDiscret::GetFaceAttribute(
  const TopoDS_Face&              theFace,
  Handle(BRepMesh_FaceAttribute)& theAttribute,
  const Standard_Boolean          isForceCreate) const
{
  if (myAttributes.IsBound(theFace))
  {
    theAttribute = myAttributes(theFace);
    return Standard_True;
  }
  else if (isForceCreate)
  {
    theAttribute = new BRepMesh_FaceAttribute(myBoundaryVertices, myBoundaryPoints);
    myAttributes.Bind(theFace, theAttribute);
  }

  return Standard_False;
}

//=======================================================================
//function : RemoveFaceAttribute
//purpose  : 
//=======================================================================
void BRepMesh_FastDiscret::RemoveFaceAttribute(const TopoDS_Face& theFace)
{
  if (myAttributes.IsBound(theFace))
    myAttributes.UnBind(theFace);
}
