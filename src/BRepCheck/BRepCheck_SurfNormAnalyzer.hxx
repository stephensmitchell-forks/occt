
#ifndef _BRepCheck_SurfNormAnalyzer_HeaderFile
#define _BRepCheck_SurfNormAnalyzer_HeaderFile

#ifndef _Standard_HeaderFile
#include <Standard.hxx>
#endif
#ifndef _Standard_DefineAlloc_HeaderFile
#include <Standard_DefineAlloc.hxx>
#endif
#ifndef _Standard_Macro_HeaderFile
#include <Standard_Macro.hxx>
#endif

#ifndef _TopoDS_Shape_HeaderFile
#include <TopoDS_Shape.hxx>
#endif

#ifndef _Standard_Boolean_HeaderFile
#include <Standard_Boolean.hxx>
#endif

#ifndef _TopAbs_ShapeEnum_HeaderFile
#include <TopAbs_ShapeEnum.hxx>
#endif

#include <TopTools_ListOfShape.hxx>

class Handle_Geom_Surface;


//! A class to check the problems with calculations<br>
//! of normals of face surfaces when dS/du and dS/dv are almost parallel: 
//! normal to surface is calculated as N = dS/du^dS/dv and when<br>
//! dS/du || dS/dv N is poor defined. It can cause problems in intersection<br>
//! and other algoritms.<br>
//! This class diagnoses whether the area on the surface where angle between dS/du<br>
//! and dS/dv less then given angular tolerance. <br>
class BRepCheck_SurfNormAnalyzer  {
public:

  DEFINE_STANDARD_ALLOC
    //
   Standard_EXPORT BRepCheck_SurfNormAnalyzer();
   Standard_EXPORT BRepCheck_SurfNormAnalyzer(const TopoDS_Shape& theS, const Standard_Real theAngTol);
   //
   Standard_EXPORT void SetShape(const TopoDS_Shape& theS);
   Standard_EXPORT void SetTolerance(const Standard_Real theAngTol);
   //
   Standard_EXPORT Standard_Boolean IsValid();
   //
   Standard_EXPORT const TopoDS_Shape& GetShape() const;
   Standard_EXPORT Standard_Real GetTolerance() const;
   //
   Standard_EXPORT const TopTools_ListOfShape& BadFaces() const;
   //
   Standard_EXPORT static Standard_Boolean 
     IsProblemSurf(const Handle_Geom_Surface& theSurf, 
                   const Standard_Real theUMin,
                   const Standard_Real theUMax,
                   const Standard_Real theVMin,
                   const Standard_Real theVMax,                                                       
                   const Standard_Real theAngTol);


protected:





private:
 

TopoDS_Shape myShape;
TopTools_ListOfShape myBadFaces;
Standard_Real myAngTol;


};



#endif
