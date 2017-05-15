// File:	Unfolding_Surface.cdl
// Created:	Tue Jul 22 12:50:10 2008
// Author:	Sergey KHROMOV
//		<skv@dimox>
//-Copyright:	Open CASCADE 2008

#ifndef _Unfolding_Surface_HeaderFile
#define _Unfolding_Surface_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <gp_Pln.hxx>
#include <Standard_Real.hxx>
#include <Unfolding_HArray2OfPoint.hxx>
#include <Unfolding_ErrorStatus.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>
#include <TopTools_ListOfShape.hxx>
class Unfolding_FaceDataContainer;
class TopoDS_Face;
class gp_Pln;
#include <TopTools_DataMapOfShapeInteger.hxx>
class TopoDS_Wire;
class TopoDS_Edge;
class TopoDS_Vertex;
class gp_XY;

//! This class is used to perform unfolding of a face onto a plane.
//! To perform  this  operation it  is necessary  to initialize the
//! object by a  face to  be unfolded,  a plane and a tolerance for
//! operation. Then to call  the method Perform. The  result planar
//! face can be  obtained using the  method GetResult. Error status
//! can be obtained by the method ErrorStatus.
class Unfolding_Surface 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Empty constructor
  Standard_EXPORT Unfolding_Surface();
  
  //! Constructor. Initializes the object with the face, the plane and
  //! the tolerance for operation.
  Standard_EXPORT Unfolding_Surface(const TopoDS_Face& theFace, const gp_Pln& thePlane, const Standard_Real theContourTolerance, const Standard_Real theCurvatureTolerance = 0.001, const Standard_Real theDeflection = 0.001);
  
  //! Sets the face.
    void SetFace (const TopoDS_Face& theFace);
  
  //! Returns the face.
    const TopoDS_Face& GetFace() const;
  
  //! Sets the plane.
    void SetPlane (const gp_Pln& thePlane);
  
  //! Returns the plane.
    const gp_Pln& GetPlane() const;
  
  //! Sets the tolerance for the operation.
    void SetCurvatureTolerance (const Standard_Real theTolerance);
  
  //! Returns the tolerance for the operation.
    Standard_Real GetCurvatureTolerance() const;
  
  //! Sets the tolerance for the operation.
    void SetContourTolerance (const Standard_Real theTolerance);
  
  //! Returns the tolerance for the operation.
    Standard_Real GetContourTolerance() const;
  
  //! Sets the tolerance for the operation.
    void SetDeflection (const Standard_Real theDeflection);
  
  //! Returns the tolerance for the operation.
    Standard_Real GetDeflection() const;
  
  //! Performs computation of  the unfolded  surface. It returns
  //! Standard_True if the operation  succeeds otherwise returns
  //! Standard_False. It is  possible to get the error status of
  //! the  performed operation  using the  method ErrorStatus().
  //! theMapEdgeNbSamples  is  the  map  of  edges as  keys  and
  //! number of samples  for this  edge  as item. It is required
  //! for predefined sampling of  edges of a face. If an edge is
  //! absent in this map its sampling is automatically computed.
  //! This feature is used to get same sampling for shared edges
  //! on different faces.
  Standard_EXPORT Standard_Boolean Perform (const TopTools_DataMapOfShapeInteger& theMapEdgeNbSamples);
  
  //! Returns error status of the operation. The error status can have
  //! one of the following values:
  //! - Unfolding_Done: operation is succeeded;
  //! - Unfolding_NotDone: the method Perform() is not called yet;
  //! - Unfolding_Failure: the operation is failed;
  //! - Unfolding_InvalidSurface: the surface cannot be unfolded
  //! without distortion;
  //! - Unfolding_InvalidInput: invalid input for the operation.
    Unfolding_ErrorStatus ErrorStatus() const;
  
  //! Returns data container. That stores all results of the operation.
    Handle(Unfolding_FaceDataContainer) GetDataContainer() const;




protected:

  
  //! Resets data to the initial state.
    void Reset();




private:

  
  //! Initializes the grid on surface. Computes a rectangular grid in
  //! the parametric space of the face and computes the corresponding
  //! 3d points on the surface.
  Standard_EXPORT Standard_Boolean InitGrid();
  
  //! Computes and returns the numbers of sampling points
  //! for U and V directions.
  Standard_EXPORT void NbPoints (const Standard_Real theUMin, const Standard_Real theUMax, const Standard_Real theVMin, const Standard_Real theVMax, Standard_Integer& theNbPointsU, Standard_Integer& theNbPointsV) const;
  
  //! Performs unfolding of the grid of points onto the plane. Returns
  //! Standard_True in  case of success and  Standard_False otherwise.
  //! Initializes the error status of the operation.
  Standard_EXPORT Standard_Boolean Unfolding();
  
  //! Constructs and returns a planar face.
  Standard_EXPORT Standard_Boolean BuildPlanarFace (const TopTools_DataMapOfShapeInteger& theMapEdgeNbSamples);
  
  //! Constructs and returns planar unfolded wire from original one.
  Standard_EXPORT Standard_Boolean BuildPlanarWire (const TopoDS_Wire& theWire, const TopTools_DataMapOfShapeInteger& theMapEdgeNbSamples, TopoDS_Wire& thePlanarWire);
  
  //! Constructs and returns a set of planar unfolded edges
  //! from theEdge.
  Standard_EXPORT Standard_Boolean UnfoldEdge (const TopoDS_Edge& theEdge, const TopoDS_Vertex& theStartVtx, const TopoDS_Vertex& theEndVtx, const TopTools_DataMapOfShapeInteger& theMapEdgeNbSamples, gp_XY& theStartPnt, gp_XY& theEndPnt, TopTools_ListOfShape& thePlnEdges) const;
  
  //! Computes and returns a point on plane that corresponds
  //! to a point on a surface.
  Standard_EXPORT Standard_Boolean ComputePointOnPlane (const gp_XY& thePoint, const Standard_Real theEdgeTol, gp_XY& thePointOnPlane) const;


  Handle(Unfolding_FaceDataContainer) myDataContainer;
  gp_Pln myPlane;
  Standard_Real myTolContour;
  Standard_Real myTolCurvature;
  Standard_Real myDeflection;
  Handle(Unfolding_HArray2OfPoint) myGrid;
  Unfolding_ErrorStatus myErrorStatus;


};


#include <Unfolding_Surface.lxx>





#endif // _Unfolding_Surface_HeaderFile
