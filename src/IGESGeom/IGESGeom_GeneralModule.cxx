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


#include <IGESData_DirChecker.hxx>
#include <IGESData_IGESEntity.hxx>
#include <IGESGeom_Boundary.hxx>
#include <IGESGeom_BoundedSurface.hxx>
#include <IGESGeom_BSplineCurve.hxx>
#include <IGESGeom_BSplineSurface.hxx>
#include <IGESGeom_CircularArc.hxx>
#include <IGESGeom_CompositeCurve.hxx>
#include <IGESGeom_ConicArc.hxx>
#include <IGESGeom_CopiousData.hxx>
#include <IGESGeom_CurveOnSurface.hxx>
#include <IGESGeom_Direction.hxx>
#include <IGESGeom_Flash.hxx>
#include <IGESGeom_GeneralModule.hxx>
#include <IGESGeom_Line.hxx>
#include <IGESGeom_OffsetCurve.hxx>
#include <IGESGeom_OffsetSurface.hxx>
#include <IGESGeom_Plane.hxx>
#include <IGESGeom_Point.hxx>
#include <IGESGeom_RuledSurface.hxx>
#include <IGESGeom_SplineCurve.hxx>
#include <IGESGeom_SplineSurface.hxx>
#include <IGESGeom_SurfaceOfRevolution.hxx>
#include <IGESGeom_TabulatedCylinder.hxx>
#include <IGESGeom_ToolBoundary.hxx>
#include <IGESGeom_ToolBoundedSurface.hxx>
#include <IGESGeom_ToolBSplineCurve.hxx>
#include <IGESGeom_ToolBSplineSurface.hxx>
#include <IGESGeom_ToolCircularArc.hxx>
#include <IGESGeom_ToolCompositeCurve.hxx>
#include <IGESGeom_ToolConicArc.hxx>
#include <IGESGeom_ToolCopiousData.hxx>
#include <IGESGeom_ToolCurveOnSurface.hxx>
#include <IGESGeom_ToolDirection.hxx>
#include <IGESGeom_ToolFlash.hxx>
#include <IGESGeom_ToolLine.hxx>
#include <IGESGeom_ToolOffsetCurve.hxx>
#include <IGESGeom_ToolOffsetSurface.hxx>
#include <IGESGeom_ToolPlane.hxx>
#include <IGESGeom_ToolPoint.hxx>
#include <IGESGeom_ToolRuledSurface.hxx>
#include <IGESGeom_ToolSplineCurve.hxx>
#include <IGESGeom_ToolSplineSurface.hxx>
#include <IGESGeom_ToolSurfaceOfRevolution.hxx>
#include <IGESGeom_ToolTabulatedCylinder.hxx>
#include <IGESGeom_ToolTransformationMatrix.hxx>
#include <IGESGeom_ToolTrimmedSurface.hxx>
#include <IGESGeom_TransformationMatrix.hxx>
#include <IGESGeom_TrimmedSurface.hxx>
#include <Interface_Category.hxx>
#include <Interface_Check.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_Macros.hxx>
#include <Interface_ShareTool.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESGeom_GeneralModule,IGESData_GeneralModule)


    IGESData_DirChecker  IGESGeom_GeneralModule::DirChecker
  (const Standard_Integer CN, const Handle(IGESData_IGESEntity)& ent) const 
{
  switch (CN) {
    case  1 : {
      DeclareAndCast(IGESGeom_BSplineCurve,anent,ent);
      if (anent.IsNull()) break;
      IGESGeom_ToolBSplineCurve tool;
      return tool.DirChecker(anent);
    }
    case  2 : {
      DeclareAndCast(IGESGeom_BSplineSurface,anent,ent);
      if (anent.IsNull()) break;
      IGESGeom_ToolBSplineSurface tool;
      return tool.DirChecker(anent);
    }
    case  3 : {
      DeclareAndCast(IGESGeom_Boundary,anent,ent);
      if (anent.IsNull()) break;
      IGESGeom_ToolBoundary tool;
      return tool.DirChecker(anent);
    }
    case  4 : {
      DeclareAndCast(IGESGeom_BoundedSurface,anent,ent);
      if (anent.IsNull()) break;
      IGESGeom_ToolBoundedSurface tool;
      return tool.DirChecker(anent);
    }
    case  5 : {
      DeclareAndCast(IGESGeom_CircularArc,anent,ent);
      if (anent.IsNull()) break;
      IGESGeom_ToolCircularArc tool;
      return tool.DirChecker(anent);
    }
    case  6 : {
      DeclareAndCast(IGESGeom_CompositeCurve,anent,ent);
      if (anent.IsNull()) break;
      IGESGeom_ToolCompositeCurve tool;
      return tool.DirChecker(anent);
    }
    case  7 : {
      DeclareAndCast(IGESGeom_ConicArc,anent,ent);
      if (anent.IsNull()) break;
      IGESGeom_ToolConicArc tool;
      return tool.DirChecker(anent);
    }
    case  8 : {
      DeclareAndCast(IGESGeom_CopiousData,anent,ent);
      if (anent.IsNull()) break;
      IGESGeom_ToolCopiousData tool;
      return tool.DirChecker(anent);
    }
    case  9 : {
      DeclareAndCast(IGESGeom_CurveOnSurface,anent,ent);
      if (anent.IsNull()) break;
      IGESGeom_ToolCurveOnSurface tool;
      return tool.DirChecker(anent);
    }
    case 10 : {
      DeclareAndCast(IGESGeom_Direction,anent,ent);
      if (anent.IsNull()) break;
      IGESGeom_ToolDirection tool;
      return tool.DirChecker(anent);
    }
    case 11 : {
      DeclareAndCast(IGESGeom_Flash,anent,ent);
      if (anent.IsNull()) break;
      IGESGeom_ToolFlash tool;
      return tool.DirChecker(anent);
    }
    case 12 : {
      DeclareAndCast(IGESGeom_Line,anent,ent);
      if (anent.IsNull()) break;
      IGESGeom_ToolLine tool;
      return tool.DirChecker(anent);
    }
    case 13 : {
      DeclareAndCast(IGESGeom_OffsetCurve,anent,ent);
      if (anent.IsNull()) break;
      IGESGeom_ToolOffsetCurve tool;
      return tool.DirChecker(anent);
    }
    case 14 : {
      DeclareAndCast(IGESGeom_OffsetSurface,anent,ent);
      if (anent.IsNull()) break;
      IGESGeom_ToolOffsetSurface tool;
      return tool.DirChecker(anent);
    }
    case 15 : {
      DeclareAndCast(IGESGeom_Plane,anent,ent);
      if (anent.IsNull()) break;
      IGESGeom_ToolPlane tool;
      return tool.DirChecker(anent);
    }
    case 16 : {
      DeclareAndCast(IGESGeom_Point,anent,ent);
      if (anent.IsNull()) break;
      IGESGeom_ToolPoint tool;
      return tool.DirChecker(anent);
    }
    case 17 : {
      DeclareAndCast(IGESGeom_RuledSurface,anent,ent);
      if (anent.IsNull()) break;
      IGESGeom_ToolRuledSurface tool;
      return tool.DirChecker(anent);
    }
    case 18 : {
      DeclareAndCast(IGESGeom_SplineCurve,anent,ent);
      if (anent.IsNull()) break;
      IGESGeom_ToolSplineCurve tool;
      return tool.DirChecker(anent);
    }
    case 19 : {
      DeclareAndCast(IGESGeom_SplineSurface,anent,ent);
      if (anent.IsNull()) break;
      IGESGeom_ToolSplineSurface tool;
      return tool.DirChecker(anent);
    }
    case 20 : {
      DeclareAndCast(IGESGeom_SurfaceOfRevolution,anent,ent);
      if (anent.IsNull()) break;
      IGESGeom_ToolSurfaceOfRevolution tool;
      return tool.DirChecker(anent);
    }
    case 21 : {
      DeclareAndCast(IGESGeom_TabulatedCylinder,anent,ent);
      if (anent.IsNull()) break;
      IGESGeom_ToolTabulatedCylinder tool;
      return tool.DirChecker(anent);
    }
    case 22 : {
      DeclareAndCast(IGESGeom_TransformationMatrix,anent,ent);
      if (anent.IsNull()) break;
      IGESGeom_ToolTransformationMatrix tool;
      return tool.DirChecker(anent);
    }
    case 23 : {
      DeclareAndCast(IGESGeom_TrimmedSurface,anent,ent);
      if (anent.IsNull()) break;
      IGESGeom_ToolTrimmedSurface tool;
      return tool.DirChecker(anent);
    }
    default : break;
  }
  return IGESData_DirChecker();    // by default, no specific criterium
}


    Standard_Integer  IGESGeom_GeneralModule::CategoryNumber
  (const Standard_Integer CN, const Handle(Standard_Transient)& ent,
   const Interface_ShareTool& ) const
{
  if (CN == 11) return Interface_Category::Number("Drawing");
  if (CN == 15) {
    DeclareAndCast(IGESGeom_Plane,anent,ent);
    if (anent->HasSymbolAttach()) return Interface_Category::Number("Drawing");
  }
  if (CN == 16) {
    DeclareAndCast(IGESGeom_Point,anent,ent);
    if (anent->HasDisplaySymbol()) return Interface_Category::Number("Drawing");
  }
  if (CN == 22) return Interface_Category::Number("Auxiliary");
  return Interface_Category::Number("Shape");
}
