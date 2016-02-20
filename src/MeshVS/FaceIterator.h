// Copyright (c) 2015 OPEN CASCADE SAS
//
// This file is part of commercial software by OPEN CASCADE SAS.
//
// This software is furnished in accordance with the terms and conditions
// of the contract and with the inclusion of this copyright notice.
// This software or any other copy thereof may not be provided or otherwise
// be made available to any third party.
// No ownership title to the software is transferred hereby.
//
// OPEN CASCADE SAS makes no representation or warranties with respect to the
// performance of this software, and specifically disclaims any responsibility
// for any damages, special or consequential, connected with its use.

#ifndef FaceIterator_H
#define FaceIterator_H

#include <AIS_ListOfInteractive.hxx>
#include <gp_Trsf.hxx>
#include <TopoDS_Face.hxx>
#include <TopLoc_Location.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>

class CafShapePrs;
class Prs3d_Drawer;
class Poly_Triangulation;

//! Auxiliary class to iterate through triangulated faces in presentations list.
class FaceIterator
{

public:

  Handle(CafShapePrs)        ShapePrs;
  TopoDS_Face                Face;
  Handle(Prs3d_Drawer)       Drawer;
  Handle(Poly_Triangulation) Triangulation;
  gp_Trsf                    Trsf;

public:

  //! Main constructor.
  Standard_EXPORT FaceIterator (const AIS_ListOfInteractive& thePrsList);

  //! Return true if iterator points to the valid triangulation.
  bool More() const
  {
    return !Triangulation.IsNull();
  }

  //! Find next value.
  Standard_EXPORT void Next();

private:

  AIS_ListOfInteractive::Iterator                     myPrsIter;
  TopLoc_Location                                     myPrsLocation;
  TopTools_IndexedDataMapOfShapeListOfShape           myFacesMap;
  TopTools_IndexedDataMapOfShapeListOfShape::Iterator myFaceIter;
  TopLoc_Location                                     myFaceLocation;

};

#endif // FaceIterator_H
