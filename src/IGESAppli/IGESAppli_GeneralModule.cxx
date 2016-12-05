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

#include <IGESSolid_Block.hxx>
#include <IGESSolid_BooleanTree.hxx>
#include <IGESSolid_ConeFrustum.hxx>
#include <IGESSolid_ConicalSurface.hxx>
#include <IGESSolid_Cylinder.hxx>
#include <IGESSolid_CylindricalSurface.hxx>
#include <IGESSolid_EdgeList.hxx>
#include <IGESSolid_Ellipsoid.hxx>
#include <IGESSolid_Face.hxx>
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
#include <IGESSolid_ToroidalSurface.hxx>
#include <IGESSolid_Torus.hxx>
#include <IGESSolid_VertexList.hxx>
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


#include <IGESAppli_DrilledHole.hxx>
#include <IGESAppli_ElementResults.hxx>
#include <IGESAppli_FiniteElement.hxx>
#include <IGESAppli_Flow.hxx>
#include <IGESAppli_FlowLineSpec.hxx>
#include <IGESAppli_GeneralModule.hxx>
#include <IGESAppli_LevelFunction.hxx>
#include <IGESAppli_LevelToPWBLayerMap.hxx>
#include <IGESAppli_LineWidening.hxx>
#include <IGESAppli_NodalConstraint.hxx>
#include <IGESAppli_NodalDisplAndRot.hxx>
#include <IGESAppli_NodalResults.hxx>
#include <IGESAppli_Node.hxx>
#include <IGESAppli_PartNumber.hxx>
#include <IGESAppli_PinNumber.hxx>
#include <IGESAppli_PipingFlow.hxx>
#include <IGESAppli_PWBArtworkStackup.hxx>
#include <IGESAppli_PWBDrilledHole.hxx>
#include <IGESAppli_ReferenceDesignator.hxx>
#include <IGESAppli_RegionRestriction.hxx>
#include <IGESAppli_ToolDrilledHole.hxx>
#include <IGESAppli_ToolElementResults.hxx>
#include <IGESAppli_ToolFiniteElement.hxx>
#include <IGESAppli_ToolFlow.hxx>
#include <IGESAppli_ToolFlowLineSpec.hxx>
#include <IGESAppli_ToolLevelFunction.hxx>
#include <IGESAppli_ToolLevelToPWBLayerMap.hxx>
#include <IGESAppli_ToolLineWidening.hxx>
#include <IGESAppli_ToolNodalConstraint.hxx>
#include <IGESAppli_ToolNodalDisplAndRot.hxx>
#include <IGESAppli_ToolNodalResults.hxx>
#include <IGESAppli_ToolNode.hxx>
#include <IGESAppli_ToolPartNumber.hxx>
#include <IGESAppli_ToolPinNumber.hxx>
#include <IGESAppli_ToolPipingFlow.hxx>
#include <IGESAppli_ToolPWBArtworkStackup.hxx>
#include <IGESAppli_ToolPWBDrilledHole.hxx>
#include <IGESAppli_ToolReferenceDesignator.hxx>
#include <IGESAppli_ToolRegionRestriction.hxx>

#include <IGESData_DirChecker.hxx>
#include <IGESData_IGESEntity.hxx>
#include <Interface_Category.hxx>
#include <Interface_Check.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_Macros.hxx>
#include <Interface_ShareTool.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESAppli_GeneralModule,IGESData_GeneralModule)

void IGESAppli_GeneralModule::OwnSharedCase (const Standard_Integer, const Handle(IGESData_IGESEntity)& ent, Interface_EntityIterator& iter) const
{
  ent->OwnShared(iter);
}


IGESData_DirChecker IGESAppli_GeneralModule::DirChecker (const Standard_Integer CN, const Handle(IGESData_IGESEntity)& ent) const
{
  // For IGESSolid {
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
  // }
    case 25 : {
      DeclareAndCast(IGESAppli_DrilledHole,anent,ent);
      if (anent.IsNull()) break;
      IGESAppli_ToolDrilledHole tool;
      return tool.DirChecker(anent);
    }
    case 26 : {
      DeclareAndCast(IGESAppli_ElementResults,anent,ent);
      if (anent.IsNull()) break;
      IGESAppli_ToolElementResults tool;
      return tool.DirChecker(anent);
    }
    case 27 : {
      DeclareAndCast(IGESAppli_FiniteElement,anent,ent);
      if (anent.IsNull()) break;
      IGESAppli_ToolFiniteElement tool;
      return tool.DirChecker(anent);
    }
    case 28 : {
      DeclareAndCast(IGESAppli_Flow,anent,ent);
      if (anent.IsNull()) break;
      IGESAppli_ToolFlow tool;
      return tool.DirChecker(anent);
    }
    case 29 : {
      DeclareAndCast(IGESAppli_FlowLineSpec,anent,ent);
      if (anent.IsNull()) break;
      IGESAppli_ToolFlowLineSpec tool;
      return tool.DirChecker(anent);
    }
    case 30 : {
      DeclareAndCast(IGESAppli_LevelFunction,anent,ent);
      if (anent.IsNull()) break;
      IGESAppli_ToolLevelFunction tool;
      return tool.DirChecker(anent);
    }
    case 31 : {
      DeclareAndCast(IGESAppli_LevelToPWBLayerMap,anent,ent);
      if (anent.IsNull()) break;
      IGESAppli_ToolLevelToPWBLayerMap tool;
      return tool.DirChecker(anent);
    }
    case 32 : {
      DeclareAndCast(IGESAppli_LineWidening,anent,ent);
      if (anent.IsNull()) break;
      IGESAppli_ToolLineWidening tool;
      return tool.DirChecker(anent);
    }
    case 33 : {
      DeclareAndCast(IGESAppli_NodalConstraint,anent,ent);
      if (anent.IsNull()) break;
      IGESAppli_ToolNodalConstraint tool;
      return tool.DirChecker(anent);
    }
    case 34 : {
      DeclareAndCast(IGESAppli_NodalDisplAndRot,anent,ent);
      if (anent.IsNull()) break;
      IGESAppli_ToolNodalDisplAndRot tool;
      return tool.DirChecker(anent);
    }
    case 35 : {
      DeclareAndCast(IGESAppli_NodalResults,anent,ent);
      if (anent.IsNull()) break;
      IGESAppli_ToolNodalResults tool;
      return tool.DirChecker(anent);
    }
    case 36 : {
      DeclareAndCast(IGESAppli_Node,anent,ent);
      if (anent.IsNull()) break;
      IGESAppli_ToolNode tool;
      return tool.DirChecker(anent);
    }
    case 37 : {
      DeclareAndCast(IGESAppli_PWBArtworkStackup,anent,ent);
      if (anent.IsNull()) break;
      IGESAppli_ToolPWBArtworkStackup tool;
      return tool.DirChecker(anent);
    }
    case 38 : {
      DeclareAndCast(IGESAppli_PWBDrilledHole,anent,ent);
      if (anent.IsNull()) break;
      IGESAppli_ToolPWBDrilledHole tool;
      return tool.DirChecker(anent);
    }
    case 39 : {
      DeclareAndCast(IGESAppli_PartNumber,anent,ent);
      if (anent.IsNull()) break;
      IGESAppli_ToolPartNumber tool;
      return tool.DirChecker(anent);
    }
    case 40 : {
      DeclareAndCast(IGESAppli_PinNumber,anent,ent);
      if (anent.IsNull()) break;
      IGESAppli_ToolPinNumber tool;
      return tool.DirChecker(anent);
    }
    case 41 : {
      DeclareAndCast(IGESAppli_PipingFlow,anent,ent);
      if (anent.IsNull()) break;
      IGESAppli_ToolPipingFlow tool;
      return tool.DirChecker(anent);
    }
    case 42 : {
      DeclareAndCast(IGESAppli_ReferenceDesignator,anent,ent);
      if (anent.IsNull()) break;
      IGESAppli_ToolReferenceDesignator tool;
      return tool.DirChecker(anent);
    }
    case 43 : {
      DeclareAndCast(IGESAppli_RegionRestriction,anent,ent);
      if (anent.IsNull()) break;
      IGESAppli_ToolRegionRestriction tool;
      return tool.DirChecker(anent);
    }
    default : break;
  }
  return IGESData_DirChecker();    // by default, no specific criterium
}


void IGESAppli_GeneralModule::OwnCheckCase (const Standard_Integer, const Handle(IGESData_IGESEntity)& ent, const Interface_ShareTool& shares, Handle(Interface_Check)& ach) const
{
  ent->OwnCheck(shares,ach);
}


Standard_Boolean IGESAppli_GeneralModule::NewVoid (const Standard_Integer CN, Handle(Standard_Transient)& ent) const
{
  // For IGESSolid {
  switch (CN) {
    case  1 : ent = new IGESSolid_Block;	break;
    case  2 : ent = new IGESSolid_BooleanTree;	break;
    case  3 : ent = new IGESSolid_ConeFrustum;	break;
    case  4 : ent = new IGESSolid_ConicalSurface;	break;
    case  5 : ent = new IGESSolid_Cylinder;	break;
    case  6 : ent = new IGESSolid_CylindricalSurface;	break;
    case  7 : ent = new IGESSolid_EdgeList;	break;
    case  8 : ent = new IGESSolid_Ellipsoid;	break;
    case  9 : ent = new IGESSolid_Face;		break;
    case 10 : ent = new IGESSolid_Loop;		break;
    case 11 : ent = new IGESSolid_ManifoldSolid;	break;
    case 12 : ent = new IGESSolid_PlaneSurface;	break;
    case 13 : ent = new IGESSolid_RightAngularWedge;	break;
    case 14 : ent = new IGESSolid_SelectedComponent;	break;
    case 15 : ent = new IGESSolid_Shell;	break;
    case 16 : ent = new IGESSolid_SolidAssembly;	break;
    case 17 : ent = new IGESSolid_SolidInstance;	break;
    case 18 : ent = new IGESSolid_SolidOfLinearExtrusion;	break;
    case 19 : ent = new IGESSolid_SolidOfRevolution;	break;
    case 20 : ent = new IGESSolid_Sphere;	break;
    case 21 : ent = new IGESSolid_SphericalSurface;	break;
    case 22 : ent = new IGESSolid_ToroidalSurface;	break;
    case 23 : ent = new IGESSolid_Torus;	break;
    case 24 : ent = new IGESSolid_VertexList;	break;
  // }
    case 25 : ent = new IGESAppli_DrilledHole;	break;
    case 26 : ent = new IGESAppli_ElementResults;	break;
    case 27 : ent = new IGESAppli_FiniteElement;	break;
    case 28 : ent = new IGESAppli_Flow;	break;
    case 29 : ent = new IGESAppli_FlowLineSpec;	break;
    case 30 : ent = new IGESAppli_LevelFunction;	break;
    case 31 : ent = new IGESAppli_LevelToPWBLayerMap;	break;
    case 32 : ent = new IGESAppli_LineWidening;	break;
    case 33 : ent = new IGESAppli_NodalConstraint;	break;
    case 34 : ent = new IGESAppli_NodalDisplAndRot;	break;
    case 35 : ent = new IGESAppli_NodalResults;	break;
    case 36 : ent = new IGESAppli_Node;	break;
    case 37 : ent = new IGESAppli_PWBArtworkStackup;	break;
    case 38 : ent = new IGESAppli_PWBDrilledHole;	break;
    case 39 : ent = new IGESAppli_PartNumber;	break;
    case 40 : ent = new IGESAppli_PinNumber;	break;
    case 41 : ent = new IGESAppli_PipingFlow;	break;
    case 42 : ent = new IGESAppli_ReferenceDesignator;	break;
    case 43 : ent = new IGESAppli_RegionRestriction;	break;
    default : return Standard_False;    // by default, Failure on Recognize
  }
  return Standard_True;
}

Standard_Integer IGESAppli_GeneralModule::CategoryNumber (const Standard_Integer CN, const Handle(Standard_Transient)& /*ent*/, const Interface_ShareTool& ) const
{
  // For IGESSolid {
  if (CN >= 1 && CN <= 24) return Interface_Category::Number("Shape");
  // }
  if (CN == 28 || CN == 29 || CN == 41) return Interface_Category::Number("Piping");
  if (CN == 26 || CN == 27 || (CN >= 33 && CN <= 36)) return Interface_Category::Number("FEA");
  return Interface_Category::Number("Professional");
}
