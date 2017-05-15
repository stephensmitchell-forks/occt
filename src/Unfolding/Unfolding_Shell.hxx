// File:	Unfolding_Shell.cdl
// Created:	Tue Sep  9 16:56:09 2008
// Author:	Mikhail KLOKOV
//		<mkk@kurox>
//-Copyright:	Open CASCADE 2008

#ifndef _Unfolding_Shell_HeaderFile
#define _Unfolding_Shell_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TopoDS_Shell.hxx>
#include <gp_Pln.hxx>
#include <Standard_Real.hxx>
#include <Unfolding_IndexedMapOfFaceDataContainer.hxx>
#include <TopoDS_Face.hxx>
#include <Unfolding_ErrorStatus.hxx>
#include <Standard_Boolean.hxx>
class TopoDS_Shell;
class gp_Pln;
class TopoDS_Face;
#include <TopTools_ListOfShape.hxx>
class Unfolding_FaceDataContainer;


//! This class is used to perform unfolding of a shell onto a plane.
//! To perform  this  operation it  is necessary  to initialize the
//! object by a  shell to  be unfolded,  a plane and a tolerance for
//! operation. Then to call  the method Perform. The  result planar
//! shell can be  obtained using the  method GetResult. Error status
//! can be obtained by the method ErrorStatus.
class Unfolding_Shell 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Empty constructor
  Standard_EXPORT Unfolding_Shell();
  
  //! Constructor. Initializes the object with the shell, the plane and
  //! the tolerances for operation.
  Standard_EXPORT Unfolding_Shell(const TopoDS_Shell& theShell, const gp_Pln& thePlane, const Standard_Real theContourTolerance, const Standard_Real theCurvatureTolerance = 0.001, const Standard_Real theDeflection = 0.001);
  
  //! Sets the face.
    void SetShell (const TopoDS_Shell& theShell);
  
  //! Returns the shell.
    const TopoDS_Shell& GetShell() const;
  
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
  
  //! Performs computation of  the unfolded surface. It returns
  //! Standard_True if the operation succeeds otherwise returns
  //! Standard_False. It is possible to get the error status of
  //! the performed operation using the method ErrorStatus().
  Standard_EXPORT Standard_Boolean Perform();
  
  //! Returns error status of the operation. The error status can have
  //! one of the following values:
  //! - Unfolding_Done: operation is succeeded;
  //! - Unfolding_NotDone: the method Perform() is not called yet;
  //! - Unfolding_Failure: the operation is failed;
  //! - Unfolding_InvalidSurface: the surface cannot be unfolded
  //! without distortion;
  //! - Unfolding_InvalidInput: invalid input for the operation;
  //! - Unfolding_InvalidShape: can be returned by
  //! Unfolding::ToShape method;
  //! - Unfolding_ComplexShape: can be returned by
  //! Unfolding::ToShape method;
    Unfolding_ErrorStatus ErrorStatus() const;
  
  //! Returns the result of the operation. If the operation is failed,
  //! it returns a null shape.
    const TopoDS_Face& GetResult() const;
  
  //! Returns the area cumulated during primitive patches mergin.
  //! It shows computed distortion.
    Standard_Real GetAreaError() const;
  
  //! Returns the gauss curvature computed in the mesh points.
    Standard_Real GetMaxGaussCurvature() const;




protected:

  
  //! Resets data to the initial state.
    void Reset();




private:

  
  Standard_EXPORT Standard_Boolean ComputeTransformed (TopTools_ListOfShape& theResult);
  
  //! Perform transformation of unfolded face2 to glue with unfolded
  //! face1. If theIsFixed is Standard_True, the face 2 is not
  //! transformed only estimations of distortions are performed.
  Standard_EXPORT Standard_Boolean MoveFace2ToFace1 (const Handle(Unfolding_FaceDataContainer)& theFaceData1, const Handle(Unfolding_FaceDataContainer)& theFaceData2, const TopTools_ListOfShape& theCommonEdges, const Standard_Boolean theIsFixed);


  TopoDS_Shell myShell;
  gp_Pln myPlane;
  Standard_Real myTolContour;
  Standard_Real myTolCurvature;
  Standard_Real myDeflection;
  Unfolding_IndexedMapOfFaceDataContainer myMapFaceData;
  TopoDS_Face myResult;
  Unfolding_ErrorStatus myErrorStatus;
  Standard_Real myDistortionArea;
  Standard_Real myCurvature;


};


#include <Unfolding_Shell.lxx>





#endif // _Unfolding_Shell_HeaderFile
