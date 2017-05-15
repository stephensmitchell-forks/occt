// File:	Unfolding_FaceDataContainer.cdl
// Created:	Fri Sep 19 16:52:05 2008
// Author:	Sergey KHROMOV
//		<skv@kurox>
//-Copyright:	 Matra Datavision 2008

#ifndef _Unfolding_FaceDataContainer_HeaderFile
#define _Unfolding_FaceDataContainer_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TopoDS_Face.hxx>
#include <TopTools_DataMapOfShapeListOfShape.hxx>
#include <Standard_Real.hxx>
#include <MMgt_TShared.hxx>

class TopoDS_Face;
class gp_Trsf;
class TopoDS_Edge;
class Unfolding_FaceDataContainer;

DEFINE_STANDARD_HANDLE(Unfolding_FaceDataContainer, MMgt_TShared)

//! This class represents a data container for data constructed
//! during unfolding operation.
class Unfolding_FaceDataContainer : public MMgt_TShared
{

public:

  
  //! Empty constructor
  Standard_EXPORT Unfolding_FaceDataContainer();
  
  //! Sets the original face.
  Standard_EXPORT void SetFace (const TopoDS_Face& theFace);
  
  //! Returns the original face.
    const TopoDS_Face& GetFace() const;
  
  //! Sets the unfolded face for the original one.
    void SetUnfoldedFace (const TopoDS_Face& theUnfoldedFace);
  
  //! Returns the unfolded face for the original one.
    const TopoDS_Face& GetUnfoldedFace() const;
  
  //! Sets the distortion area.
    void SetDistortionArea (const Standard_Real theDistortionArea);
  
  //! Returns the distortion area.
    Standard_Real GetDistortionArea() const;
  
  //! Sets the maximal Gauss curvature.
    void SetMaxGaussCurvature (const Standard_Real theCurvature);
  
  //! Returns the maximal Gauss curvature.
    Standard_Real GetMaxGaussCurvature() const;
  
  //! Resets the data container.
    void Reset();
  
  //! Applies the transformation to all unfolded shapes.
  Standard_EXPORT void ApplyTrsf (const gp_Trsf& theTrsf);
  
  //! Associates unfolded edges with the source edge.
  Standard_EXPORT void SetEdgesForEdge (const TopoDS_Edge& theEdge, const TopTools_ListOfShape& theUnfoldedEdges);
  
  //! Returns unfolded edges associated to the source edge.
  Standard_EXPORT const TopTools_ListOfShape& GetEdgesForEdge (const TopoDS_Edge& theEdge) const;




  DEFINE_STANDARD_RTTIEXT(Unfolding_FaceDataContainer,MMgt_TShared)

protected:




private:


  TopoDS_Face myFace;
  TopoDS_Face myUnfoldedFace;
  TopTools_DataMapOfShapeListOfShape myEdgeMap;
  Standard_Real myDistortionArea;
  Standard_Real myCurvature;


};


#include <Unfolding_FaceDataContainer.lxx>





#endif // _Unfolding_FaceDataContainer_HeaderFile
