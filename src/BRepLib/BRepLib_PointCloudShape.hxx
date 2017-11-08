// Copyright (c) 2004-2014 OPEN CASCADE SAS
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


#ifndef _BRepLib_PointCloudShape_HeaderFile
#define _BRepLib_PointCloudShape_HeaderFile

#include <TColgp_HSequenceOfXYZ.hxx>
#include <TopTools_DataMapOfShapeReal.hxx>
#include <NCollection_IndexedDataMap.hxx>
#include <NCollection_DataMap.hxx>
#include <TopoDS_Shape.hxx>
#include <Quantity_Color.hxx>
#include <TopTools_DataMapOfShapeInteger.hxx>
#include <Precision.hxx>


//! Intended to get points from shape with specified distance from shape along normal.
//! Con be used to simulation of points obtained in result of scan of shape
class BRepLib_PointCloudShape
{
public:

  //! Constructor initialized by shape
  Standard_EXPORT BRepLib_PointCloudShape(const TopoDS_Shape& theShape, Standard_Real theTol = Precision::Confusion()) : 
      myShape(theShape), myDist(0.0), myTol(theTol), myNbPoints(0) {}
 
  //! Sets value of the distance to define deflection of points from shape along normal to shape
  Standard_EXPORT void SetDistance( Standard_Real theDist)
  {
    myDist = theDist;
  }

  //! Returns size of the point cloud for density
  Standard_EXPORT Standard_Integer NbPointsDensity(Standard_Real theDensity = 0.);
 
  //! Returns size of the point cloud for using triangulation
  Standard_EXPORT Standard_Integer NbPointsTriangulation() const;
 
  //! Computes points with specified dencity for initial shape.
  //! If parameter Dencity is eqw\ual to 0 then dencity will be computed automatically
  //! by critera : 10 points per minimal face area 
  Standard_EXPORT Standard_Boolean GetPointsDensity( Standard_Real theDensity = 0.);
  

  //! Get points froim triangulation existing in the shape
  Standard_EXPORT Standard_Boolean GetPointsTriang();

  //! Adds colors to shape (compound containing faces, solid, shell , face) 
  Standard_EXPORT  void AddColor(const TopoDS_Shape& theShape, const Quantity_Color& theColor);



 protected:

   //! Compute area of the specified face
  Standard_Real faceArea(const TopoDS_Shape& theShape);

  //! Computes default density points per face
  Standard_Real computeDensity();

  //! Adds points to face in accordance with the specified density randomly in the specified range [0, Dist]
  Standard_Boolean addDensityPoints(const TopoDS_Shape& theFace );
  
  //! Adds points to face by nodes of the existing triangulation randomly in the specified range [0, Dist]
  Standard_Boolean addTriangPoints(const TopoDS_Shape& theFace);

  //! Method to add point , normal to surface in this point and face for which point computed
  virtual void addPoint( const gp_Pnt& thePoint,const gp_Vec& theNorm, const TopoDS_Shape& theFace) =0;

  //! Method to clear maps
   Standard_EXPORT virtual void clear();


private:

  TopoDS_Shape myShape;
  Standard_Real myDist;
  Standard_Real myTol;
  TopTools_DataMapOfShapeReal myFaceArea;
  TopTools_DataMapOfShapeInteger myFacePoints;
  Standard_Integer myNbPoints;

};

#endif