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

#include <BRepLib_PointCloudShape.hxx>
#include <Precision.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS_Shape.hxx>
#include <GProp_GProps.hxx>
#include <BRepGProp.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS.hxx>
#include <BRepTools.hxx>
#include <BRepTopAdaptor_FClass2d.hxx>
#include <BRep_Tool.hxx>
#include <Geom_Surface.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>


//=======================================================================
//function :NbPointsDensity
//purpose  : 
//=======================================================================

Standard_Integer BRepLib_PointCloudShape::NbPointsDensity(Standard_Real theDensity)
{
  clear();
  Standard_Real aDensity = ( theDensity < Precision::Confusion() ? computeDensity() : theDensity);
  if( aDensity < Precision::Confusion())
    return Standard_False;
  // at first step find the face with smallest area
  TopExp_Explorer aExpF(myShape, TopAbs_FACE);
  Standard_Integer aNbPoints =0;

  for (  ; aExpF.More(); aExpF.Next())
  {   
    Standard_Real anArea = myFaceArea.IsBound(aExpF.Current()) ? myFaceArea.Find(aExpF.Current()) : faceArea(aExpF.Current());
    Standard_Integer aNbPnts = Max ((Standard_Integer )std::ceil (anArea / theDensity), 1);
    myFacePoints.Bind(aExpF.Current(),aNbPnts);
    aNbPoints+= aNbPnts;
  }
  return aNbPoints;
}

//=======================================================================
//function :GetPoints
//purpose  : 
//=======================================================================

Standard_Boolean BRepLib_PointCloudShape::GetPointsDensity( Standard_Real theDensity)
{
  if(!myFacePoints.Extent())
  {
    if(!NbPointsDensity(theDensity))
      return Standard_False;
  }
    
  TopExp_Explorer aExpF(myShape, TopAbs_FACE);
  Standard_Integer nbAdded =0 ;
  for (  ; aExpF.More(); aExpF.Next())
  {
    if(addDensityPoints(aExpF.Current() ))
      nbAdded++;
  }
  return (nbAdded > 0);
}

//=======================================================================
//function :GetPointsTriang
//purpose  : 
//=======================================================================
Standard_Boolean BRepLib_PointCloudShape::GetPointsTriang()
{
  clear();
  
  Standard_Integer nbAdded =0 ;
  TopExp_Explorer aExpF(myShape, TopAbs_FACE);
  for (  ; aExpF.More(); aExpF.Next())
  {
    if(addTriangPoints(aExpF.Current()))
      nbAdded++;
  }
  return (nbAdded >0);
}


//=======================================================================
//function :faceArea
//purpose  : 
//=======================================================================

Standard_Real BRepLib_PointCloudShape::faceArea(const TopoDS_Shape& theShape)
{
  GProp_GProps aFaceProps;
  BRepGProp::SurfaceProperties (theShape, aFaceProps);
  Standard_Real anArea = aFaceProps.Mass();
  myFaceArea.Bind(theShape, anArea);
  return anArea;
}

//=======================================================================
//function :computeDensity
//purpose  : 
//=======================================================================

Standard_Real BRepLib_PointCloudShape::computeDensity()
{
  // at first step find the face with smallest area
  TopExp_Explorer aExpF(myShape, TopAbs_FACE);
  Standard_Real anAreaMin = Precision::Infinite();
  for (  ; aExpF.More(); aExpF.Next())
  {
    Standard_Real anArea = faceArea(aExpF.Current());
    if(anArea < myTol * myTol)
      continue;

    if (anArea < anAreaMin )
      anAreaMin = anArea;

  }
  return anAreaMin * 0.1;

}
//=======================================================================
//function : computeNbPointsTriang
//purpose  : 
//=======================================================================

Standard_Integer BRepLib_PointCloudShape::NbPointsTriangulation() const
{
  // at first step find the face with smallest area
  TopExp_Explorer aExpF(myShape, TopAbs_FACE);
  Standard_Integer aNbPoints =0;
  for (  ; aExpF.More(); aExpF.Next())
  {
    TopLoc_Location aLoc;
    Handle(Poly_Triangulation) aTriangulation = BRep_Tool::Triangulation(TopoDS::Face(aExpF.Current()), aLoc);
    if(aTriangulation.IsNull())
      continue;
    aNbPoints += aTriangulation->NbNodes();
  }
  return aNbPoints;
}
//=======================================================================
//function :addDensityPoints
//purpose  : 
//=======================================================================

Standard_Boolean BRepLib_PointCloudShape::addDensityPoints(const TopoDS_Shape& theFace )
{
  //addition of the points with specified density on the face by random way
  Standard_Integer aNbPnts = (myFacePoints.IsBound(theFace) ? myFacePoints.Find(theFace) : 0);
  if( !aNbPnts )
    return Standard_False;
  TopoDS_Face aFace = TopoDS::Face(theFace);
  Standard_Real anUMin, anUMax, anVMin, anVMax;
  BRepTools::UVBounds (aFace, anUMin, anUMax, anVMin, anVMax);
  BRepTopAdaptor_FClass2d aClassifier  (aFace, Precision::Confusion());
  Handle(Geom_Surface) aSurf = BRep_Tool::Surface(aFace);
  Standard_Integer nbCurPnts = 1;
  for ( ; nbCurPnts <= aNbPnts; )
  {
    const Standard_Real aU = anUMin + Standard_Real(::rand()) / Standard_Real(RAND_MAX) * (anUMax - anUMin);
    const Standard_Real aV = anVMin + Standard_Real(::rand()) / Standard_Real(RAND_MAX) * (anVMax - anVMin);
    const TopAbs_State  aState = aClassifier.Perform (gp_Pnt2d (aU, aV));
    if (aState == TopAbs_OUT )
      continue;
    
    nbCurPnts++;

   gp_Pnt aP1;
   gp_Vec dU, dV;
   aSurf->D1( aU, aV, aP1, dU, dV);
   gp_Vec aNorm = dU ^ dV;
   if(aFace.Orientation() == TopAbs_REVERSED)
     aNorm.Reverse();
   Standard_Real aMod =aNorm.Magnitude();
   if( aMod > gp::Resolution())
     aNorm /= aMod;
   if( myDist > Precision::Confusion())
   {
     Standard_Real aDist = fabs(Standard_Real(::rand())) / Standard_Real(RAND_MAX) * myDist;
     gp_XYZ aDeflPoint =  aP1.XYZ() + aNorm.XYZ() * aDist;
     aP1.SetXYZ(aDeflPoint);
   }
   addPoint( aP1, aNorm, aFace);
  }
  return Standard_True;   
}

//=======================================================================
//function :addTriangPoints
//purpose  : 
//=======================================================================
Standard_Boolean  BRepLib_PointCloudShape::addTriangPoints(const TopoDS_Shape& theFace)
{
  TopLoc_Location aLoc;
  TopoDS_Face aFace =TopoDS::Face(theFace);
  Handle(Poly_Triangulation) aTriangulation = BRep_Tool::Triangulation(aFace, aLoc);
  if(aTriangulation.IsNull())
    return Standard_False;
  
  TopLoc_Location aLoc1;
  Handle(Geom_Surface) aSurf = BRep_Tool::Surface(aFace, aLoc1);
  const TColgp_Array1OfPnt& aNodes = aTriangulation->Nodes();
  gp_Trsf aTrsf(aLoc);
  Standard_Integer i =1;
 
  for( ; i <= aNodes.Length() ; i++)
  {
    gp_Pnt aP1 = aNodes(i);
    if( !aLoc.IsIdentity())
      aP1.Transform(aTrsf);
    gp_Vec aNorm(0.,0.,0.);
    if(aTriangulation->HasNormals())
      aNorm = aTriangulation->Normal(i);
    else if(aTriangulation->HasUVNodes())
    {
      //compute normal by UV node
      const gp_Pnt2d& aP2d = aTriangulation->UVNode(i);
      gp_Pnt aP1;
      gp_Vec dU, dV;
      aSurf->D1( aP2d.X(),aP2d.Y(), aP1, dU, dV);
      aNorm = (dU ^ dV);
    }
    if(aNorm.SquareMagnitude() > gp::Resolution())
    { 
      aNorm.Normalize();
      if( !aLoc.IsIdentity())
        aNorm.Transform(aTrsf);
            
      if(aFace.Orientation() == TopAbs_REVERSED)
        aNorm.Reverse();

      if( myDist > Precision::Confusion())
      {
        Standard_Real aDist = fabs(Standard_Real(::rand())) / Standard_Real(RAND_MAX) * myDist;
        gp_XYZ aDeflPoint =  aP1.XYZ() + aNorm.XYZ() * aDist;
        aP1.SetXYZ(aDeflPoint);
      }
    }
    addPoint( aP1, aNorm, aFace);
  }
  return Standard_True;
}


////=======================================================================
////function :clear
////purpose  : 
////=======================================================================
void BRepLib_PointCloudShape::clear()
{
  myFaceArea.Clear();
  myFacePoints.Clear();
}