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

/*
#include <IGESData_DirChecker.hxx>
#include <IGESData_IGESEntity.hxx>
#include <IGESSolid_Block.hxx>
#include <IGESSolid_BooleanTree.hxx>
#include <IGESSolid_ConeFrustum.hxx>
#include <IGESSolid_ConicalSurface.hxx>
#include <IGESSolid_Cylinder.hxx>
#include <IGESSolid_CylindricalSurface.hxx>
#include <IGESSolid_EdgeList.hxx>
#include <IGESSolid_Ellipsoid.hxx>
#include <IGESSolid_Face.hxx>
#include <IGESSolid_GeneralModule.hxx>
#include <IGESSolid_Loop.hxx>
#include <IGESSolid_ManifoldSolid.hxx>
#include <IGESSolid_PlaneSurface.hxx>
#include <IGESSolid_RightAngularWedge.hxx>
#include <IGESSolid_SelectedComponent.hxx>
#include <IGESSolid_Shell.hxx>
#include <IGESSolid_SolidAssembly.hxx>
#include <IGESSolid_SolidInstance.hxx>
#include <IGESSolid_SolidOfLinearExtrusion.hxx>
#include <IGESSolid_SolidOfRevolution.hxx>
#include <IGESSolid_Sphere.hxx>
#include <IGESSolid_SphericalSurface.hxx>
#include <IGESSolid_ToolBlock.hxx>
#include <IGESSolid_ToolBooleanTree.hxx>
#include <IGESSolid_ToolConeFrustum.hxx>
#include <IGESSolid_ToolConicalSurface.hxx>
#include <IGESSolid_ToolCylinder.hxx>
#include <IGESSolid_ToolCylindricalSurface.hxx>
#include <IGESSolid_ToolEdgeList.hxx>
#include <IGESSolid_ToolEllipsoid.hxx>
#include <IGESSolid_ToolFace.hxx>
#include <IGESSolid_ToolLoop.hxx>
#include <IGESSolid_ToolManifoldSolid.hxx>
#include <IGESSolid_ToolPlaneSurface.hxx>
#include <IGESSolid_ToolRightAngularWedge.hxx>
#include <IGESSolid_ToolSelectedComponent.hxx>
#include <IGESSolid_ToolShell.hxx>
#include <IGESSolid_ToolSolidAssembly.hxx>
#include <IGESSolid_ToolSolidInstance.hxx>
#include <IGESSolid_ToolSolidOfLinearExtrusion.hxx>
#include <IGESSolid_ToolSolidOfRevolution.hxx>
#include <IGESSolid_ToolSphere.hxx>
#include <IGESSolid_ToolSphericalSurface.hxx>
#include <IGESSolid_ToolToroidalSurface.hxx>
#include <IGESSolid_ToolTorus.hxx>
#include <IGESSolid_ToolVertexList.hxx>
#include <IGESSolid_ToroidalSurface.hxx>
#include <IGESSolid_Torus.hxx>
#include <IGESSolid_VertexList.hxx>
#include <Interface_Category.hxx>
#include <Interface_Check.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_Macros.hxx>
#include <Interface_ShareTool.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESSolid_GeneralModule,IGESData_GeneralModule)


    IGESData_DirChecker  IGESSolid_GeneralModule::DirChecker
  (const Standard_Integer CN, const Handle(IGESData_IGESEntity)& ent) const 
{
  switch (CN) {
    case  1 : {
      DeclareAndCast(IGESSolid_Block,anent,ent);
      if (anent.IsNull()) break;
      IGESSolid_ToolBlock tool;
      return tool.DirChecker(anent);
    }
    case  2 : {
      DeclareAndCast(IGESSolid_BooleanTree,anent,ent);
      if (anent.IsNull()) break;
      IGESSolid_ToolBooleanTree tool;
      return tool.DirChecker(anent);
    }
    case  3 : {
      DeclareAndCast(IGESSolid_ConeFrustum,anent,ent);
      if (anent.IsNull()) break;
      IGESSolid_ToolConeFrustum tool;
      return tool.DirChecker(anent);
    }
    case  4 : {
      DeclareAndCast(IGESSolid_ConicalSurface,anent,ent);
      if (anent.IsNull()) break;
      IGESSolid_ToolConicalSurface tool;
      return tool.DirChecker(anent);
    }
    case  5 : {
      DeclareAndCast(IGESSolid_Cylinder,anent,ent);
      if (anent.IsNull()) break;
      IGESSolid_ToolCylinder tool;
      return tool.DirChecker(anent);
    }
    case  6 : {
      DeclareAndCast(IGESSolid_CylindricalSurface,anent,ent);
      if (anent.IsNull()) break;
      IGESSolid_ToolCylindricalSurface tool;
      return tool.DirChecker(anent);
    }
    case  7 : {
      DeclareAndCast(IGESSolid_EdgeList,anent,ent);
      if (anent.IsNull()) break;
      IGESSolid_ToolEdgeList tool;
      return tool.DirChecker(anent);
    }
    case  8 : {
      DeclareAndCast(IGESSolid_Ellipsoid,anent,ent);
      if (anent.IsNull()) break;
      IGESSolid_ToolEllipsoid tool;
      return tool.DirChecker(anent);
    }
    case  9 : {
      DeclareAndCast(IGESSolid_Face,anent,ent);
      if (anent.IsNull()) break;
      IGESSolid_ToolFace tool;
      return tool.DirChecker(anent);
    }
    case 10 : {
      DeclareAndCast(IGESSolid_Loop,anent,ent);
      if (anent.IsNull()) break;
      IGESSolid_ToolLoop tool;
      return tool.DirChecker(anent);
    }
    case 11 : {
      DeclareAndCast(IGESSolid_ManifoldSolid,anent,ent);
      if (anent.IsNull()) break;
      IGESSolid_ToolManifoldSolid tool;
      return tool.DirChecker(anent);
    }
    case 12 : {
      DeclareAndCast(IGESSolid_PlaneSurface,anent,ent);
      if (anent.IsNull()) break;
      IGESSolid_ToolPlaneSurface tool;
      return tool.DirChecker(anent);
    }
    case 13 : {
      DeclareAndCast(IGESSolid_RightAngularWedge,anent,ent);
      if (anent.IsNull()) break;
      IGESSolid_ToolRightAngularWedge tool;
      return tool.DirChecker(anent);
    }
    case 14 : {
      DeclareAndCast(IGESSolid_SelectedComponent,anent,ent);
      if (anent.IsNull()) break;
      IGESSolid_ToolSelectedComponent tool;
      return tool.DirChecker(anent);
    }
    case 15 : {
      DeclareAndCast(IGESSolid_Shell,anent,ent);
      if (anent.IsNull()) break;
      IGESSolid_ToolShell tool;
      return tool.DirChecker(anent);
    }
    case 16 : {
      DeclareAndCast(IGESSolid_SolidAssembly,anent,ent);
      if (anent.IsNull()) break;
      IGESSolid_ToolSolidAssembly tool;
      return tool.DirChecker(anent);
    }
    case 17 : {
      DeclareAndCast(IGESSolid_SolidInstance,anent,ent);
      if (anent.IsNull()) break;
      IGESSolid_ToolSolidInstance tool;
      return tool.DirChecker(anent);
    }
    case 18 : {
      DeclareAndCast(IGESSolid_SolidOfLinearExtrusion,anent,ent);
      if (anent.IsNull()) break;
      IGESSolid_ToolSolidOfLinearExtrusion tool;
      return tool.DirChecker(anent);
    }
    case 19 : {
      DeclareAndCast(IGESSolid_SolidOfRevolution,anent,ent);
      if (anent.IsNull()) break;
      IGESSolid_ToolSolidOfRevolution tool;
      return tool.DirChecker(anent);
    }
    case 20 : {
      DeclareAndCast(IGESSolid_Sphere,anent,ent);
      if (anent.IsNull()) break;
      IGESSolid_ToolSphere tool;
      return tool.DirChecker(anent);
    }
    case 21 : {
      DeclareAndCast(IGESSolid_SphericalSurface,anent,ent);
      if (anent.IsNull()) break;
      IGESSolid_ToolSphericalSurface tool;
      return tool.DirChecker(anent);
    }
    case 22 : {
      DeclareAndCast(IGESSolid_ToroidalSurface,anent,ent);
      if (anent.IsNull()) break;
      IGESSolid_ToolToroidalSurface tool;
      return tool.DirChecker(anent);
    }
    case 23 : {
      DeclareAndCast(IGESSolid_Torus,anent,ent);
      if (anent.IsNull()) break;
      IGESSolid_ToolTorus tool;
      return tool.DirChecker(anent);
    }
    case 24 : {
      DeclareAndCast(IGESSolid_VertexList,anent,ent);
      if (anent.IsNull()) break;
      IGESSolid_ToolVertexList tool;
      return tool.DirChecker(anent);
    }
    default : break;
  }
  return IGESData_DirChecker();    // by default, no specific criterium
}


    Standard_Integer  IGESSolid_GeneralModule::CategoryNumber
  (const Standard_Integer , const Handle(Standard_Transient)& ,
   const Interface_ShareTool& ) const
{
  return Interface_Category::Number("Shape");
}
*/