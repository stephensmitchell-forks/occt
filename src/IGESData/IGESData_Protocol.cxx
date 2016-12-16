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


#include <IGESData_IGESType.hxx>
#include <IGESData_IGESEntity.hxx>
#include <IGESData_IGESModel.hxx>
#include <IGESData_Protocol.hxx>
#include <IGESData_UndefinedEntity.hxx>

#include <IGESAppli_DrilledHole.hxx>
#include <IGESAppli_ElementResults.hxx>
#include <IGESAppli_FiniteElement.hxx>
#include <IGESAppli_Flow.hxx>
#include <IGESAppli_FlowLineSpec.hxx>
#include <IGESAppli_LevelFunction.hxx>
#include <IGESAppli_LevelToPWBLayerMap.hxx>
#include <IGESAppli_LineWidening.hxx>
#include <IGESAppli_NodalConstraint.hxx>
#include <IGESAppli_NodalDisplAndRot.hxx>
#include <IGESAppli_NodalResults.hxx>
#include <IGESAppli_Node.hxx>
#include <IGESAppli_PWBArtworkStackup.hxx>
#include <IGESAppli_PWBDrilledHole.hxx>
#include <IGESAppli_PartNumber.hxx>
#include <IGESAppli_PinNumber.hxx>
#include <IGESAppli_PipingFlow.hxx>
#include <IGESAppli_ReferenceDesignator.hxx>
#include <IGESAppli_RegionRestriction.hxx>

#include <IGESBasic_AssocGroupType.hxx>
#include <IGESBasic_ExternalRefFile.hxx>
#include <IGESBasic_ExternalRefFileIndex.hxx>
#include <IGESBasic_ExternalRefFileName.hxx>
#include <IGESBasic_ExternalRefLibName.hxx>
#include <IGESBasic_ExternalRefName.hxx>
#include <IGESBasic_ExternalReferenceFile.hxx>
#include <IGESBasic_Group.hxx>
#include <IGESBasic_GroupWithoutBackP.hxx>
#include <IGESBasic_Hierarchy.hxx>
#include <IGESBasic_Name.hxx>
#include <IGESBasic_OrderedGroup.hxx>
#include <IGESBasic_OrderedGroupWithoutBackP.hxx>
#include <IGESBasic_SingleParent.hxx>
#include <IGESBasic_SingularSubfigure.hxx>
#include <IGESBasic_SubfigureDef.hxx>

#include <IGESDefs_AssociativityDef.hxx>
#include <IGESDefs_AttributeDef.hxx>
#include <IGESDefs_AttributeTable.hxx>
#include <IGESDefs_GenericData.hxx>
#include <IGESDefs_MacroDef.hxx>
#include <IGESDefs_TabularData.hxx>
#include <IGESDefs_UnitsData.hxx>

#include <IGESDimen_AngularDimension.hxx>
#include <IGESDimen_BasicDimension.hxx>
#include <IGESDimen_CenterLine.hxx>
#include <IGESDimen_CurveDimension.hxx>
#include <IGESDimen_DiameterDimension.hxx>
#include <IGESDimen_DimensionDisplayData.hxx>
#include <IGESDimen_DimensionTolerance.hxx>
#include <IGESDimen_DimensionUnits.hxx>
#include <IGESDimen_DimensionedGeometry.hxx>
#include <IGESDimen_FlagNote.hxx>
#include <IGESDimen_GeneralLabel.hxx>
#include <IGESDimen_GeneralNote.hxx>
#include <IGESDimen_GeneralSymbol.hxx>
#include <IGESDimen_LeaderArrow.hxx>
#include <IGESDimen_LinearDimension.hxx>
#include <IGESDimen_NewDimensionedGeometry.hxx>
#include <IGESDimen_NewGeneralNote.hxx>
#include <IGESDimen_OrdinateDimension.hxx>
#include <IGESDimen_PointDimension.hxx>
#include <IGESDimen_RadiusDimension.hxx>
#include <IGESDimen_Section.hxx>
#include <IGESDimen_SectionedArea.hxx>
#include <IGESDimen_WitnessLine.hxx>

#include <IGESDraw_CircArraySubfigure.hxx>
#include <IGESDraw_ConnectPoint.hxx>
#include <IGESDraw_Drawing.hxx>
#include <IGESDraw_DrawingWithRotation.hxx>
#include <IGESDraw_LabelDisplay.hxx>
#include <IGESDraw_NetworkSubfigure.hxx>
#include <IGESDraw_NetworkSubfigureDef.hxx>
#include <IGESDraw_PerspectiveView.hxx>
#include <IGESDraw_Planar.hxx>
#include <IGESDraw_RectArraySubfigure.hxx>
#include <IGESDraw_SegmentedViewsVisible.hxx>
#include <IGESDraw_View.hxx>
#include <IGESDraw_ViewsVisible.hxx>
#include <IGESDraw_ViewsVisibleWithAttr.hxx>

#include <IGESGeom_BSplineCurve.hxx>
#include <IGESGeom_BSplineSurface.hxx>
#include <IGESGeom_Boundary.hxx>
#include <IGESGeom_BoundedSurface.hxx>
#include <IGESGeom_CircularArc.hxx>
#include <IGESGeom_CompositeCurve.hxx>
#include <IGESGeom_ConicArc.hxx>
#include <IGESGeom_CopiousData.hxx>
#include <IGESGeom_CurveOnSurface.hxx>
#include <IGESGeom_Direction.hxx>
#include <IGESGeom_Flash.hxx>
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
#include <IGESGeom_TransformationMatrix.hxx>
#include <IGESGeom_TrimmedSurface.hxx>

#include <IGESGraph_Color.hxx>
#include <IGESGraph_DefinitionLevel.hxx>
#include <IGESGraph_DrawingSize.hxx>
#include <IGESGraph_DrawingUnits.hxx>
#include <IGESGraph_HighLight.hxx>
#include <IGESGraph_IntercharacterSpacing.hxx>
#include <IGESGraph_LineFontDefPattern.hxx>
#include <IGESGraph_LineFontPredefined.hxx>
#include <IGESGraph_LineFontDefTemplate.hxx>
#include <IGESGraph_NominalSize.hxx>
#include <IGESGraph_Pick.hxx>
#include <IGESGraph_TextDisplayTemplate.hxx>
#include <IGESGraph_TextFontDef.hxx>
#include <IGESGraph_UniformRectGrid.hxx>

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


IMPLEMENT_STANDARD_RTTIEXT(IGESData_Protocol,Interface_Protocol)

//  TypeNumber : Ici, on reconnait UndefinedEntity (faut bien quelqu un)

Standard_Integer IGESData_Protocol::TypeNumber (const Handle(Standard_Type)& atype) const
{
  if (atype->SubType(STANDARD_TYPE(IGESData_UndefinedEntity))) return 1;
  return 0;
}

Handle(Interface_InterfaceModel) IGESData_Protocol::NewModel () const
{
  return new IGESData_IGESModel;
}

Handle(Standard_Transient) IGESData_Protocol::UnknownEntity () const
{
  return new IGESData_UndefinedEntity;
}

Standard_Boolean IGESData_Protocol::IsUnknownEntity (const Handle(Standard_Transient)& ent) const
{
  return ent->IsKind(STANDARD_TYPE(IGESData_UndefinedEntity));
}

Handle(IGESData_IGESEntity) IGESData_Protocol::NewEntity (const IGESData_IGESType &theType) const
{
  const Standard_Integer aType = theType.Type();
  const Standard_Integer aForm = theType.Form();

  switch (aType) {
    case 100 : return new IGESGeom_CircularArc;
    case 102 : return new IGESGeom_CompositeCurve;
    case 104 : return new IGESGeom_ConicArc;
    case 106 :
      switch (aForm) {
        case 20: return new IGESDimen_CenterLine;
        case 21: return new IGESDimen_CenterLine;
        case 40: return new IGESDimen_WitnessLine;
        default:
          if (aForm < 20 || aForm > 40) return new IGESGeom_CopiousData;
          if (aForm > 30 && aForm < 40) return new IGESDimen_Section;
          break;
      }
      break;
    case 108 : return new IGESGeom_Plane;
    case 110 : return new IGESGeom_Line;
    case 112 : return new IGESGeom_SplineCurve;
    case 114 : return new IGESGeom_SplineSurface;
    case 116 : return new IGESGeom_Point;
    case 118 : return new IGESGeom_RuledSurface;
    case 120 : return new IGESGeom_SurfaceOfRevolution;
    case 122 : return new IGESGeom_TabulatedCylinder;
    case 123 : return new IGESGeom_Direction;
    case 124 : return new IGESGeom_TransformationMatrix;
    case 125 : return new IGESGeom_Flash;
    case 126 : return new IGESGeom_BSplineCurve;
    case 128 : return new IGESGeom_BSplineSurface;
    case 130 : return new IGESGeom_OffsetCurve;
    case 132 : return new IGESDraw_ConnectPoint; //+
    case 134 : return new IGESAppli_Node; //+
    case 136 : return new IGESAppli_FiniteElement; //+
    case 138 : return new IGESAppli_NodalDisplAndRot; //+
    case 140 : return new IGESGeom_OffsetSurface;
    case 141 : return new IGESGeom_Boundary;
    case 142 : return new IGESGeom_CurveOnSurface;
    case 143 : return new IGESGeom_BoundedSurface;
    case 144 : return new IGESGeom_TrimmedSurface;
    case 146 : return new IGESAppli_NodalResults(aForm); //+
    case 148 : return new IGESAppli_ElementResults(aForm); //+
    case 150 : return new IGESSolid_Block;
    case 152 : return new IGESSolid_RightAngularWedge;
    case 154 : return new IGESSolid_Cylinder;
    case 156 : return new IGESSolid_ConeFrustum;
    case 158 : return new IGESSolid_Sphere;
    case 160 : return new IGESSolid_Torus;
    case 162 : return new IGESSolid_SolidOfRevolution;
    case 164 : return new IGESSolid_SolidOfLinearExtrusion;
    case 168 : return new IGESSolid_Ellipsoid;
    case 180 : return new IGESSolid_BooleanTree;
    case 182 : return new IGESSolid_SelectedComponent;
    case 184 : return new IGESSolid_SolidAssembly;
    case 186 : return new IGESSolid_ManifoldSolid;
    case 190 : return new IGESSolid_PlaneSurface;
    case 192 : return new IGESSolid_CylindricalSurface;
    case 194 : return new IGESSolid_ConicalSurface;
    case 196 : return new IGESSolid_SphericalSurface;
    case 198 : return new IGESSolid_ToroidalSurface;
    case 202 : return new IGESDimen_AngularDimension;
    case 204 : return new IGESDimen_CurveDimension;
    case 206 : return new IGESDimen_DiameterDimension;
    case 208 : return new IGESDimen_FlagNote;
    case 210 : return new IGESDimen_GeneralLabel;
    case 212 : return new IGESDimen_GeneralNote;
    case 213 : return new IGESDimen_NewGeneralNote;
    case 214 : return new IGESDimen_LeaderArrow;
    case 216 : return new IGESDimen_LinearDimension;
    case 218 : return new IGESDimen_OrdinateDimension;
    case 220 : return new IGESDimen_PointDimension;
    case 222 : return new IGESDimen_RadiusDimension;
    case 228 : return new IGESDimen_GeneralSymbol;
    case 230 : return new IGESDimen_SectionedArea;
    case 302 : return new IGESDefs_AssociativityDef(aForm); //+
    case 304 :
      switch (aForm) {
        case  1 : return new IGESGraph_LineFontDefTemplate; //+
        case  2 : return new IGESGraph_LineFontDefPattern; //+
        default : break;
      }
      break;
    case 306 : return new IGESDefs_MacroDef; //+
    case 308 : return new IGESBasic_SubfigureDef;
    case 310 : return new IGESGraph_TextFontDef; //+
    case 312 : return new IGESGraph_TextDisplayTemplate(aForm); //+
    case 314 : return new IGESGraph_Color; //+
    case 316 : return new IGESDefs_UnitsData; //+
    case 320 : return new IGESDraw_NetworkSubfigureDef; //+
    case 322 : return new IGESDefs_AttributeDef(aForm); //+
    case 402 :
      switch (aForm) {
        case  1 : return new IGESBasic_Group;
        case  3 : return new IGESDraw_ViewsVisible;
        case  4 : return new IGESDraw_ViewsVisibleWithAttr;
        case  5 : return new IGESDraw_LabelDisplay; //+
        case  7 : return new IGESBasic_GroupWithoutBackP;
        case  9 : return new IGESBasic_SingleParent;
        case 12 : return new IGESBasic_ExternalRefFileIndex;
        case 13 : return new IGESDimen_DimensionedGeometry;
        case 14 : return new IGESBasic_OrderedGroup;
        case 15 : return new IGESBasic_OrderedGroupWithoutBackP;
        case 16 : return new IGESDraw_Planar; //+
        case 18 : return new IGESAppli_Flow; //+
        case 19 : return new IGESDraw_SegmentedViewsVisible; //+
        case 20 : return new IGESAppli_PipingFlow; //+
        case 21 : return new IGESDimen_NewDimensionedGeometry;
        default : break;
      }
      break;
    case 404 :
      switch (aForm) {
        case  0 : return new IGESDraw_Drawing; //+
        case  1 : return new IGESDraw_DrawingWithRotation; //+
        default : break;
      }
      break;
    case 406 :
      switch (aForm) {
        case  1 : return new IGESGraph_DefinitionLevel; //+
        case  2 : return new IGESAppli_RegionRestriction; //+
        case  3 : return new IGESAppli_LevelFunction; //+
        case  5 : return new IGESAppli_LineWidening; //+
        case  6 : return new IGESAppli_DrilledHole; //+
        case  7 : return new IGESAppli_ReferenceDesignator; //+
        case  8 : return new IGESAppli_PinNumber; //+
        case  9 : return new IGESAppli_PartNumber; //+
        case 10 : return new IGESBasic_Hierarchy;
        case 11 : return new IGESDefs_TabularData; //+
        case 12 : return new IGESBasic_ExternalReferenceFile;
        case 13 : return new IGESGraph_NominalSize; //+
        case 14 : return new IGESAppli_FlowLineSpec; //+
        case 15 : return new IGESBasic_Name;
        case 16 : return new IGESGraph_DrawingSize; //+
        case 17 : return new IGESGraph_DrawingUnits; //+
        case 18 : return new IGESGraph_IntercharacterSpacing; //+
        case 19 : return new IGESGraph_LineFontPredefined; //+
        case 20 : return new IGESGraph_HighLight; //+
        case 21 : return new IGESGraph_Pick; //+
        case 22 : return new IGESGraph_UniformRectGrid; //+
        case 23 : return new IGESBasic_AssocGroupType;
        case 24 : return new IGESAppli_LevelToPWBLayerMap; //+
        case 25 : return new IGESAppli_PWBArtworkStackup; //+
        case 26 : return new IGESAppli_PWBDrilledHole; //+
        case 27 : return new IGESDefs_GenericData; //+
        case 28 : return new IGESDimen_DimensionUnits;
        case 29 : return new IGESDimen_DimensionTolerance;
        case 30 : return new IGESDimen_DimensionDisplayData;
        case 31 : return new IGESDimen_BasicDimension;
        default : break;
      }
      break;
    case 408 : return new IGESBasic_SingularSubfigure;
    case 410 :
      switch (aForm) {
        case  0 : return new IGESDraw_View; //+
        case  1 : return new IGESDraw_PerspectiveView; //+
        default : break;
      }
      break;
    case 412 : return new IGESDraw_RectArraySubfigure; //+
    case 414 : return new IGESDraw_CircArraySubfigure; //+
    case 416 :
      switch (aForm) {
        case  0 : return new IGESBasic_ExternalRefFileName;
        case  1 : return new IGESBasic_ExternalRefFile;
        case  2 : return new IGESBasic_ExternalRefFileName;
        case  3 : return new IGESBasic_ExternalRefName;
        case  4 : return new IGESBasic_ExternalRefLibName;
        default : break;
      }
      break;
    case 418 : return new IGESAppli_NodalConstraint; //+
    case 420 : return new IGESDraw_NetworkSubfigure; //+
    case 422 : return new IGESDefs_AttributeTable(aForm); //+
    case 430 : return new IGESSolid_SolidInstance;
    case 502 : return new IGESSolid_VertexList;
    case 504 : return new IGESSolid_EdgeList;
    case 508 : return new IGESSolid_Loop;
    case 510 : return new IGESSolid_Face;
    case 514 : return new IGESSolid_Shell;
    default : break;
  }

  /*szv_c1:Standard_Boolean  IGESData_DefaultGeneral::NewVoid (const Standard_Integer CN, Handle(Standard_Transient)& entto) const
  if (CN == 1) entto = new IGESData_UndefinedEntity;
  if (CN == 2) entto = new IGESData_FreeFormatEntity;*/

  return Handle(IGESData_IGESEntity)();
}
