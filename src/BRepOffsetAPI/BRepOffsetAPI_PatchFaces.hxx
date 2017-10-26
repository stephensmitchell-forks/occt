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
#include <TopTools_DataMapOfShapeListOfShape.hxx>
#include <TopTools_DataMapOfShapeShape.hxx>
#include <TopTools_DataMapOfOrientedShapeShape.hxx>
#include <TopTools_MapOfShape.hxx>
#include <TColStd_SequenceOfReal.hxx>
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

  //! Empty constructor.
  Standard_EXPORT BRepOffsetAPI_PatchFaces();

  //! Sets the model
  void SetShape(const TopoDS_Shape& theShape)
  {
    myInitialShape = theShape;
  }

  //! Clears the contents from the previous runs
  Standard_EXPORT void Clear();

  //! Adds the patch face for the face in the shape.
  Standard_EXPORT void AddPatchFace (const TopoDS_Face& theFace, const TopoDS_Face& thePatchFace);
  
  Standard_EXPORT virtual void Build() Standard_OVERRIDE;

  //! Returns the patched face, updated to fit the model, for the given face <theFace>.
  //! If there is no patched faces for the given face, the returned face will be null.
  Standard_EXPORT TopoDS_Face GetPatchFace(const TopoDS_Face& theFace) const;

  //! Returns the list of the faces, updated to fit the model, adjacent to the given replaced face.
  //! If the given face has not been replaced, the list will be empty.
  Standard_EXPORT void GetAdjacentFaces(const TopoDS_Face& theFace, TopTools_ListOfShape& theNeighbors) const;


protected:





private:


  Standard_EXPORT void UpdateEdgesAndVertex(const TopoDS_Edge& thePrevEdge,
                                            TopoDS_Edge&       thePrevNewEdge,
                                            const TopoDS_Edge& theCurEdge,
                                            TopoDS_Edge&       theCurNewEdge,
                                            const TopoDS_Vertex& theCurVertex,
                                            const TopoDS_Face& theFace,
                                            const TopoDS_Face& theNewFace,
                                            const TopoDS_Face& theBoundedNewFace);
  
  Standard_EXPORT void PutVertexToEdge(const TopoDS_Vertex&     theVertex,
                                       const TopAbs_Orientation theProVertexOrientation,
                                       const TopoDS_Edge&       theEdge,
                                       const TopoDS_Edge&       theProEdge,
                                       const TopoDS_Face&       theFace,
                                       const Standard_Real      theParamOnEdge);
  
  Standard_EXPORT void ProjectVertexOnNewEdge_2d(const TopoDS_Vertex& theVertex,
                                                 const TopoDS_Edge&   theEdge,
                                                 const TopoDS_Face&   theFace,
                                                 const TopoDS_Edge&   theNewEdge,
                                                 const TopoDS_Face&   theNewFace,
                                                 Standard_Real&       theParamOnConst,
                                                 Standard_Real&       theParam,
                                                 gp_Pnt&              thePntOnConst,
                                                 gp_Pnt&              thePnt,
                                                 gp_Pnt&              thePntSol,
                                                 gp_Pnt2d&            thePnt2d,
                                                 Standard_Real&       theTolReached);

  Standard_EXPORT Standard_Boolean IsMoreThan3Edges(const TopoDS_Vertex& theVertex);

  Standard_EXPORT Standard_Boolean AreSmoothlyConnected(const TopoDS_Edge&   theEdge1,
                                                        const TopoDS_Edge&   theEdge2,
                                                        const TopoDS_Vertex& theVertex,
                                                        const TopoDS_Face&   theFace,
                                                        TopoDS_Edge&         theThirdEdge);

  Standard_EXPORT void UpdateVertexTol(const TopoDS_Vertex&     theVertex,
                                       const TopoDS_Edge&       theEdge1,
                                       const TopoDS_Edge&       theProEdge1,
                                       const TopAbs_Orientation theOrOfVertexForEdge1,
                                       const TopoDS_Edge&       theEdge2,
                                       const TopoDS_Edge&       theProEdge2,
                                       const TopoDS_Face&       theFace,
                                       TColStd_SequenceOfReal&  theIntParamsOnFirst,
                                       TColStd_SequenceOfReal&  theIntParamsOnSecond);
  
  Standard_EXPORT void UpdateVertexTolOnAllFaces(const TopoDS_Vertex& theVertex,
                                                 const TopoDS_Vertex& theProVertex);
  
  TopoDS_Shape myInitialShape;
  
  TopTools_IndexedDataMapOfShapeShape myFacePatchFace;
  TopTools_IndexedDataMapOfShapeShape myFaceNewFace;
  TopTools_DataMapOfShapeShape myNewFaceBoundedFace;
  TopTools_DataMapOfShapeShape myEdgeNewEdge;
  TopTools_DataMapOfShapeShape myVertexNewVertex;
  TopTools_MapOfShape myTangentEdges;
  TopTools_MapOfShape mySmoothEdges;

  TopTools_IndexedDataMapOfShapeListOfShape myEFmap;
  TopTools_IndexedDataMapOfShapeListOfShape myVEmap;
  TopTools_DataMapOfShapeListOfShape        myVFmap;

};







#endif // _BRepOffsetAPI_PatchFaces_HeaderFile
