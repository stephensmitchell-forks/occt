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
#include <IGESAppli_PartNumber.hxx>
#include <IGESAppli_PinNumber.hxx>
#include <IGESAppli_PipingFlow.hxx>
#include <IGESAppli_Protocol.hxx>
#include <IGESAppli_PWBArtworkStackup.hxx>
#include <IGESAppli_PWBDrilledHole.hxx>
#include <IGESAppli_ReferenceDesignator.hxx>
#include <IGESAppli_RegionRestriction.hxx>


#include <IGESGeom.hxx>
#include <IGESGeom_Protocol.hxx>
#include <IGESDefs.hxx>
#include <IGESDefs_Protocol.hxx>
#include <IGESDraw.hxx>
#include <IGESDraw_Protocol.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESAppli_Protocol,IGESData_Protocol)

/*static Handle(Standard_Type) atype01,atype02,atype03,atype04,atype05,atype06,
  atype07,atype08,atype09,atype10,atype11,atype12,atype13,atype14,atype15,
  atype16,atype17,atype18,atype19;*/
static Handle(Standard_Type) gTypes[43+1];

IGESAppli_Protocol::IGESAppli_Protocol ()
{
  static int deja = 0;
  if (!deja) {
    deja = 1;
    // For IGESSolid {
    gTypes[ 1] = STANDARD_TYPE(IGESSolid_Block);
    gTypes[ 2] = STANDARD_TYPE(IGESSolid_BooleanTree);
    gTypes[ 3] = STANDARD_TYPE(IGESSolid_ConeFrustum);
    gTypes[ 4] = STANDARD_TYPE(IGESSolid_ConicalSurface);
    gTypes[ 5] = STANDARD_TYPE(IGESSolid_Cylinder);
    gTypes[ 6] = STANDARD_TYPE(IGESSolid_CylindricalSurface);
    gTypes[ 7] = STANDARD_TYPE(IGESSolid_EdgeList);
    gTypes[ 8] = STANDARD_TYPE(IGESSolid_Ellipsoid);
    gTypes[ 9] = STANDARD_TYPE(IGESSolid_Face);
    gTypes[10] = STANDARD_TYPE(IGESSolid_Loop);
    gTypes[11] = STANDARD_TYPE(IGESSolid_ManifoldSolid);
    gTypes[12] = STANDARD_TYPE(IGESSolid_PlaneSurface);
    gTypes[13] = STANDARD_TYPE(IGESSolid_RightAngularWedge);
    gTypes[14] = STANDARD_TYPE(IGESSolid_SelectedComponent);
    gTypes[15] = STANDARD_TYPE(IGESSolid_Shell);
    gTypes[16] = STANDARD_TYPE(IGESSolid_SolidAssembly);
    gTypes[17] = STANDARD_TYPE(IGESSolid_SolidInstance);
    gTypes[18] = STANDARD_TYPE(IGESSolid_SolidOfLinearExtrusion);
    gTypes[19] = STANDARD_TYPE(IGESSolid_SolidOfRevolution);
    gTypes[20] = STANDARD_TYPE(IGESSolid_Sphere);
    gTypes[21] = STANDARD_TYPE(IGESSolid_SphericalSurface);
    gTypes[22] = STANDARD_TYPE(IGESSolid_ToroidalSurface);
    gTypes[23] = STANDARD_TYPE(IGESSolid_Torus);
    gTypes[24] = STANDARD_TYPE(IGESSolid_VertexList);
    // }
    gTypes[25] = STANDARD_TYPE(IGESAppli_DrilledHole);
    gTypes[26] = STANDARD_TYPE(IGESAppli_ElementResults);
    gTypes[27] = STANDARD_TYPE(IGESAppli_FiniteElement);
    gTypes[28] = STANDARD_TYPE(IGESAppli_Flow);
    gTypes[29] = STANDARD_TYPE(IGESAppli_FlowLineSpec);
    gTypes[30] = STANDARD_TYPE(IGESAppli_LevelFunction);
    gTypes[31] = STANDARD_TYPE(IGESAppli_LevelToPWBLayerMap);
    gTypes[32] = STANDARD_TYPE(IGESAppli_LineWidening);
    gTypes[33] = STANDARD_TYPE(IGESAppli_NodalConstraint);
    gTypes[34] = STANDARD_TYPE(IGESAppli_NodalDisplAndRot);
    gTypes[35] = STANDARD_TYPE(IGESAppli_NodalResults);
    gTypes[36] = STANDARD_TYPE(IGESAppli_Node);
    gTypes[37] = STANDARD_TYPE(IGESAppli_PWBArtworkStackup);
    gTypes[38] = STANDARD_TYPE(IGESAppli_PWBDrilledHole);
    gTypes[39] = STANDARD_TYPE(IGESAppli_PartNumber);
    gTypes[40] = STANDARD_TYPE(IGESAppli_PinNumber);
    gTypes[41] = STANDARD_TYPE(IGESAppli_PipingFlow);
    gTypes[42] = STANDARD_TYPE(IGESAppli_ReferenceDesignator);
    gTypes[43] = STANDARD_TYPE(IGESAppli_RegionRestriction);
  }
}

Standard_Integer IGESAppli_Protocol::NbResources () const
{  return 3;  }

Handle(Interface_Protocol) IGESAppli_Protocol::Resource (const Standard_Integer num) const
{
  if (num == 1) return IGESGeom::Protocol();
  if (num == 2) return IGESDefs::Protocol();
  if (num == 3) return IGESDraw::Protocol();
  return NULL;
}

Standard_Integer IGESAppli_Protocol::TypeNumber (const Handle(Standard_Type)& atype) const
{
  for (Standard_Integer i = 1; i < sizeof(gTypes) / sizeof(Handle(Standard_Type)); i++)
  {
    if (atype == gTypes[i]) return i;
  }
  /*
  // For IGESSolid {
  if      (atype == atype01) return  1;
  else if (atype == atype02) return  2;
  else if (atype == atype03) return  3;
  else if (atype == atype04) return  4;
  else if (atype == atype05) return  5;
  else if (atype == atype06) return  6;
  else if (atype == atype07) return  7;
  else if (atype == atype08) return  8;
  else if (atype == atype09) return  9;
  else if (atype == atype10) return 10;
  else if (atype == atype11) return 11;
  else if (atype == atype12) return 12;
  else if (atype == atype13) return 13;
  else if (atype == atype14) return 14;
  else if (atype == atype15) return 15;
  else if (atype == atype16) return 16;
  else if (atype == atype17) return 17;
  else if (atype == atype18) return 18;
  else if (atype == atype19) return 19;
  else if (atype == atype20) return 20;
  else if (atype == atype21) return 21;
  else if (atype == atype22) return 22;
  else if (atype == atype23) return 23;
  else if (atype == atype24) return 24;
  // }
  else if (atype == atype01) return 25;
  else if (atype == atype02) return 26;
  else if (atype == atype03) return 27;
  else if (atype == atype04) return 28;
  else if (atype == atype05) return 29;
  else if (atype == atype06) return 30;
  else if (atype == atype07) return 31;
  else if (atype == atype08) return 32;
  else if (atype == atype09) return 33;
  else if (atype == atype10) return 34;
  else if (atype == atype11) return 35;
  else if (atype == atype12) return 36;
  else if (atype == atype13) return 37;
  else if (atype == atype14) return 38;
  else if (atype == atype15) return 39;
  else if (atype == atype16) return 40;
  else if (atype == atype17) return 41;
  else if (atype == atype18) return 42;
  else if (atype == atype19) return 43;*/
  return 0;
}
