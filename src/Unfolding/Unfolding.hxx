// File:	Unfolding.cdl
// Created:	Tue Jul 22 12:48:05 2008
// Author:	Sergey KHROMOV
//		<skv@dimox>
//-Copyright:	Open CASCADE 2008

#ifndef _Unfolding_HeaderFile
#define _Unfolding_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Real.hxx>
#include <Unfolding_ErrorStatus.hxx>
#include <Standard_Integer.hxx>
#include <TopTools_ListOfShape.hxx>

class TopoDS_Shell;
class TopoDS_Shape;
class TopoDS_Edge;
class Unfolding_Surface;
class Unfolding_Point;
class Unfolding_FunctionWithDerivative;
class Unfolding_Shell;
class Unfolding_FaceDataContainer;
class Unfolding_FaceDataMapHasher;


//! This package contains a tool for unfolding a surface on a plane.
class Unfolding 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! This method converts theShape to a shell. It sewes faces of the
  //! shell if it is necessary and possible with the given tolerance.
  //! If it is not possible to construct a single shell from theShape,
  //! this method returns null shell and the corresponding error
  //! status. The status can have the following values:
  //! -  Unfolding_Done: the operation succeeded
  //! -  Unfolding_InvalidInput: input shape type is less then
  //! TopAbs_SHELL.
  //! -  Unfolding_Failure: sewing failure.
  //! -  Unfolding_InvalidShape: the shape after sewing does not
  //! contain shells.
  //! -  Unfolding_ComplexShape: the shape after sewing contains
  //! either more then one shell or one shell and other not
  //! connected shapes.
  Standard_EXPORT static TopoDS_Shell ToShell (const TopoDS_Shape& theShape, const Standard_Real theTolerance, Unfolding_ErrorStatus& theStatus);
  
  //! This method returns the number of sample points for theEdge.
  //! theFaces is a list of faces that contain theEdge.
  //! This method returns the maximal number of points for sampling of
  //! edges and/or faces.
    static Standard_Integer NbSamples (const TopoDS_Edge& theEdge, const TopTools_ListOfShape& theFaces, const Standard_Real theTolerance);
  
  Standard_EXPORT static Standard_Integer GetMaxNbSamples();




protected:





private:




friend class Unfolding_Surface;
friend class Unfolding_Point;
friend class Unfolding_FunctionWithDerivative;
friend class Unfolding_Shell;
friend class Unfolding_FaceDataContainer;
friend class Unfolding_FaceDataMapHasher;

};


#include <Unfolding.lxx>





#endif // _Unfolding_HeaderFile
