// Created by: Ekaterina SMIRNOVA
// Copyright (c) 2008-2014 OPEN CASCADE SAS
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

#include <BRepMesh_FaceAttribute.hxx>
#include <TopoDS_Vertex.hxx>
#include <BRepAdaptor_HSurface.hxx>
#include <BRepMesh_ShapeTool.hxx>
#include <BRepMesh_Classifier.hxx>
#include <BRepTools.hxx>

IMPLEMENT_STANDARD_HANDLE (BRepMesh_FaceAttribute, Standard_Transient)
IMPLEMENT_STANDARD_RTTIEXT(BRepMesh_FaceAttribute, Standard_Transient)

//=======================================================================
//function : Constructor
//purpose  : 
//=======================================================================
BRepMesh_FaceAttribute::BRepMesh_FaceAttribute(
  const TopoDS_Face&                theFace,
  BRepMeshCol::DMapOfVertexInteger& theBoundaryVertices,
  BRepMeshCol::DMapOfIntegerPnt&    theBoundaryPoints)
  : myDefFace         (0.),
    myUMin            (0.),
    myUMax            (0.),
    myVMin            (0.),
    myVMax            (0.),
    myDeltaX          (1.),
    myDeltaY          (1.),
    myStatus          (BRepMesh_NoError),
    myBoundaryVertices(theBoundaryVertices),
    myBoundaryPoints  (theBoundaryPoints),
    myAllocator       (new NCollection_IncAllocator(64000))
{
  myVertexEdgeMap = new BRepMeshCol::IMapOfInteger;
  myInternalEdges = new BRepMeshCol::DMapOfShapePairOfPolygon;
  mySurfacePoints = new BRepMeshCol::DMapOfIntegerPnt;
  myClassifier    = new BRepMesh_Classifier;

  myFace = theFace;
  BRepTools::Update(myFace);
  myFace.Orientation(TopAbs_FORWARD);

  BRepAdaptor_Surface aSurfAdaptor(myFace, Standard_False);
  mySurface = new BRepAdaptor_HSurface(aSurfAdaptor);

  ResetStructure();
}

//=======================================================================
//function : Destructor
//purpose  : 
//=======================================================================
BRepMesh_FaceAttribute::~BRepMesh_FaceAttribute()
{
  clearLocal();

  mySurfaceVertices.Clear();
  mySurfacePoints->Clear();

  myClassifier.Nullify();
  myAllocator.Nullify();
}

//=======================================================================
//function : clearLocal
//purpose  : 
//=======================================================================
void BRepMesh_FaceAttribute::clearLocal()
{
  myStructure.Nullify();

  myLocation2D.Clear();
  myVertexEdgeMap->Clear();
  myInternalEdges->Clear();
}

//=======================================================================
//function : ResetStructure
//purpose  : 
//=======================================================================
Handle(BRepMesh_DataStructureOfDelaun)& BRepMesh_FaceAttribute::ResetStructure()
{
  clearLocal();
  myStructure = new BRepMesh_DataStructureOfDelaun(myAllocator);

  BRepTools::UVBounds(myFace, myUMin, myUMax, myVMin, myVMax);
  Standard_Real aTolU = ToleranceU();
  Standard_Real aTolV = ToleranceV();

  myStructure->Data().SetCellSize(14.0 * aTolU, 14.0 * aTolV);
  myStructure->Data().SetTolerance(aTolU, aTolV);
  return myStructure;
}

//=======================================================================
//function : computeParametricTolerance
//purpose  : 
//=======================================================================
Standard_Real BRepMesh_FaceAttribute::computeParametricTolerance(
  const Standard_Real theFirstParam,
  const Standard_Real theLastParam) const
{
  const Standard_Real aDeflectionUV = 1.e-06;
  return Max(aDeflectionUV, (theLastParam - theFirstParam) * aDeflectionUV);
}

//=======================================================================
//function : getVertexIndex
//purpose  : 
//=======================================================================
Standard_Boolean BRepMesh_FaceAttribute::getVertexIndex(
  const TopoDS_Vertex& theVertex,
  Standard_Integer&    theVertexIndex) const
{
  if (myBoundaryVertices.IsBound(theVertex))
    theVertexIndex = myBoundaryVertices.Find(theVertex);
  else if (mySurfaceVertices.IsBound(theVertex))
    theVertexIndex = mySurfaceVertices.Find(theVertex);
  else
    return Standard_False;

  return Standard_True;
}

//=======================================================================
//function : AddNode
//purpose  : 
//=======================================================================
void BRepMesh_FaceAttribute::AddNode(
  const Standard_Integer         theIndex,
  const gp_XY&                   theUV,
  const BRepMesh_DegreeOfFreedom theMovability,
  Standard_Integer&              theNodeIndex,
  Standard_Integer&              theNodeOnEdgeIndex)
{
  BRepMesh_Vertex aNode(theUV, theIndex, theMovability);
  theNodeIndex = myStructure->AddNode(aNode);
  theNodeOnEdgeIndex = myVertexEdgeMap->FindIndex(theNodeIndex);
  if (theNodeOnEdgeIndex == 0)
    theNodeOnEdgeIndex = myVertexEdgeMap->Add(theNodeIndex);
}

//=======================================================================
//function : Scale
//purpose  : 
//=======================================================================
gp_XY BRepMesh_FaceAttribute::Scale(const gp_XY&           thePoint2d,
                                    const Standard_Boolean isToFaceBasis)
{
  return isToFaceBasis ?
    gp_XY((thePoint2d.X() - myUMin) / myDeltaX, (thePoint2d.Y() - myVMin) / myDeltaY) :
    gp_XY(thePoint2d.X() * myDeltaX + myUMin, thePoint2d.Y() * myDeltaY + myVMin);
}
