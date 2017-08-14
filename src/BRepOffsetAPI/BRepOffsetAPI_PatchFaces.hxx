// Created on: 2012-08-06
// Created by: jgv@ROLEX
// Copyright (c) 2012-2014 OPEN CASCADE SAS
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

#ifndef _BRepOffsetAPI_PatchFaces_HeaderFile
#define _BRepOffsetAPI_PatchFaces_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TopoDS_Shape.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Vertex.hxx>
#include <Standard_Boolean.hxx>
#include <TopTools_IndexedDataMapOfShapeShape.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <TopTools_DataMapOfShapeShape.hxx>
#include <TopTools_DataMapOfOrientedShapeShape.hxx>
#include <TopTools_MapOfShape.hxx>
#include <BRepBuilderAPI_MakeShape.hxx>
class TopoDS_Shape;


//! Describes functions to replace some faces in a shape
//! by patches
class BRepOffsetAPI_PatchFaces  : public BRepBuilderAPI_MakeShape
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! General constructor.
  Standard_EXPORT BRepOffsetAPI_PatchFaces(const TopoDS_Shape& aShape);
  
  //! Adds the patch face for the face in the shape.
  Standard_EXPORT void AddPatchFace (const TopoDS_Face& theFace, const TopoDS_Face& thePatchFace);
  
  Standard_EXPORT virtual void Build() Standard_OVERRIDE;




protected:





private:


  Standard_EXPORT void UpdateEdgesAndVertex(const TopoDS_Edge& thePrevEdge,
                                            TopoDS_Edge&       thePrevNewEdge,
                                            const TopoDS_Edge& theCurEdge,
                                            TopoDS_Edge&       theCurNewEdge,
                                            TopoDS_Vertex&     theCurVertex);
  
  Standard_EXPORT void PutVertexToEdge(const TopoDS_Vertex& theVertex,
                                       const TopoDS_Vertex& theProVertex,
                                       TopoDS_Edge& theEdge,
                                       const TopoDS_Edge& theProEdge,
                                       const Standard_Real theParamOnEdge);
  

  TopoDS_Shape myInitialShape;
  
  TopTools_IndexedDataMapOfShapeShape myFacePatchFace;
  TopTools_IndexedDataMapOfShapeShape myFaceNewFace;
  TopTools_DataMapOfShapeShape myNewFaceBoundedFace;
  TopTools_DataMapOfShapeShape myEdgeNewEdge;
  TopTools_DataMapOfOrientedShapeShape myOrientedEdgeNewEdge;
  TopTools_DataMapOfShapeShape myVertexNewVertex;
  TopTools_MapOfShape myTangentEdges;

  TopTools_IndexedDataMapOfShapeListOfShape myEFmap;

};







#endif // _BRepOffsetAPI_PatchFaces_HeaderFile
