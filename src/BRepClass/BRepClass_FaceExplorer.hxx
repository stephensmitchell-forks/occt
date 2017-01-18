// Created on: 1992-11-19
// Created by: Remi LEQUETTE
// Copyright (c) 1992-1999 Matra Datavision
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

#ifndef _BRepClass_FaceExplorer_HeaderFile
#define _BRepClass_FaceExplorer_HeaderFile

#include <NCollection_Vector.hxx>
#include <TopClass_GeomEdge.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Wire.hxx>
#include <NCollection_List.hxx>
#include <NCollection_IndexedDataMap.hxx>

class TopClass_RayInfo;

//! Provide an   exploration of a  BRep Face   for the
//! classification. Return UV edges.
class BRepClass_FaceExplorer 
{
public:

  DEFINE_STANDARD_ALLOC

  Standard_EXPORT BRepClass_FaceExplorer(const TopoDS_Face& F);

  //! Checks the point and change its coordinates if it is located too far
  //! from the bounding box of the face. New Coordinates of the point 
  //! will be on the line between the point and the center of the 
  //! bounding box. Returns True if point was not changed.
  Standard_EXPORT Standard_Boolean CheckPoint(gp_Pnt2d& thePoint);

  //! Puts ray(s) (see TopClass_RayInfo for detail information) to theList.
  Standard_EXPORT void ListOfRays(const gp_Pnt2d& P, NCollection_Vector<TopClass_RayInfo>& theList);

  //! Starts an exploration of the wires.
  Standard_EXPORT void InitWires();
  
  //! Returns True if there is  a current wire.
    Standard_Boolean MoreWires() const;
  
  //! Sets the explorer  to the  next  wire.
  void NextWire();

  //! Starts an exploration of the  edges of the current
  //! wire.
  Standard_EXPORT void InitEdges();
  
  //! Returns True if there is a current edge.
  Standard_Boolean MoreEdges() const;

  //! Sets the explorer  to the  next  edge.
  void NextEdge();

  //! Returns analyzed edge.
  Standard_EXPORT void CurrentEdge(TopClass_GeomEdge& E) const;

  //! Returns TRUE if thePnt is covered by the tolerance of at least one
  //! vertex of the analyzed edge.
  //! If it is true then stores the parameter of the vertex on the curve.
  //! Otherwise, returns *theParameter = 0.0.
  //! This parameter is used in TopOpeBRep class
  //! and must be deleted after this class will be eliminated.
  Standard_EXPORT Standard_Boolean IsPointInEdgeVertex(const gp_Pnt2d &thePnt,
                                                       Standard_Real* const theParameter = 0) const;

  //! This method is used only for compatibility with TopOpeBRep
  //! packages. It should be deleted after this package will be eliminated.
  const TopoDS_Edge& GetTopoEdge() const
  {
    return myEExplorer.Value().GetTopoEdge();
  }

protected:

  //! Excludes all necessary information from theEdge
  //! in order to create pseudo-edges in the future.
  //! Returns FALSE if pseudo-edge creation is not necessary.
  //! Returns 2D-point on the myFace (thePnt) respected to
  //! theCurrentVertex.
  //! Output parameter theOri indicates if theCurrentVertex
  //! is first (FORWARD) or last (reversed) vertex of the theEdge.
  //! ATTENTION!!! theCurrentVertex must be in theEdge.
  Standard_EXPORT Standard_Boolean
                    PrepareEdge(const TopoDS_Edge& theEdge,
                                const TopoDS_Vertex& theCurrentVertex,
                                gp_Pnt2d& thePnt,
                                TopAbs_Orientation& theOri);

  //! Inserts topological edge theEdge to myEList without
  //! any additional checks.
  Standard_EXPORT void SimpleAdd(const TopoDS_Edge &theEdge);

  //! Creates pseudo-edge between and inserts it to myEList.
  //! Pseudo-edge is 2D-line, which joins ends of theE1 and theE2
  //! real edges. This edge does not have any 3D- and topological-
  //! representations (pseudo-edge is not synonym of degenerated edge).
  //! I.e. it is simple gap in the myFace. This gap must be covered
  //! by theVertex (otherwise, myFace is not valid for using in OCCT-algorithms).
  //! theUPeriod and theVPeriod are periods of myFace (are equal to 0.0 if
  //! the surface is not periodic)
  Standard_EXPORT void AddPseudo(const TopoDS_Edge& theE1,
                                 const TopoDS_Edge& theE2,
                                 const TopoDS_Vertex& theVertex,
                                 const Standard_Real theUPeriod,
                                 const Standard_Real theVPeriod);

private:

  //! Classified face
  TopoDS_Face myFace;

  //! Explores myFace to wires
  NCollection_IndexedDataMap <TopoDS_Wire,
                NCollection_List<TopClass_GeomEdge>> myMapWE;

  //! Iterator of myMapWE
  NCollection_IndexedDataMap <TopoDS_Wire,
                      NCollection_List<TopClass_GeomEdge>>::Iterator myWExplorer;

  //! Iterator of myEList
  NCollection_List<TopClass_GeomEdge>::Iterator myEExplorer;

};

#include <BRepClass_FaceExplorer.lxx>

#endif // _BRepClass_FaceExplorer_HeaderFile
