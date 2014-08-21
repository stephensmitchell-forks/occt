// Created on: 2014-08-13
// Created by: Maxim GLIBIN
// Copyright (c) 2014 OPEN CASCADE SAS
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

#include <AIS_PointCloud.hxx>
#include <AIS_Drawer.hxx>

#include <Prs3d_Root.hxx>
#include <Prs3d_Presentation.hxx>
#include <Prs3d_PointAspect.hxx>

#include <PrsMgr_Presentations.hxx>
#include <PrsMgr_ModedPresentation.hxx>

#include <Graphic3d_Group.hxx>
#include <Graphic3d_AspectMarker3d.hxx>

IMPLEMENT_STANDARD_HANDLE(AIS_PointCloud, AIS_InteractiveObject)
IMPLEMENT_STANDARD_RTTIEXT(AIS_PointCloud, AIS_InteractiveObject)

IMPLEMENT_HARRAY1(AIS_ArrayOfPnt)

//==================================================
// Function: AIS_PointCloud
// Purpose : Constructor
//==================================================
AIS_PointCloud::AIS_PointCloud()
: AIS_InteractiveObject()
{
}

//=======================================================================
//function : SetPoints
//purpose  :
//=======================================================================
bool AIS_PointCloud::SetPoints (const Handle(Graphic3d_ArrayOfPoints)& thePoints)
{
  if (thePoints.IsNull() || !thePoints->IsValid()) return false;
  myPoints = thePoints;
  return true;
}

//=======================================================================
//function : SetPoints
//purpose  : 
//=======================================================================
bool AIS_PointCloud::SetPoints (const Handle(AIS_ArrayOfPnt)& theCoords,
                                const Handle(AIS_ArrayOfPnt)& theColors,
                                const Standard_Boolean hasColors)
{
  if (theCoords.IsNull() || theColors.IsNull()) return false;
  if (theCoords->Size() != theColors->Size()) return false;

  Standard_Integer aNumPoints = theCoords->Size();

  Handle(Graphic3d_ArrayOfPoints) anArrayOfPoints = new Graphic3d_ArrayOfPoints (aNumPoints, hasColors);
  Standard_Integer iter = 1;
  for (; iter <= aNumPoints; iter++)
  {
    anArrayOfPoints->AddVertex (theCoords->Value(iter));

    gp_Pnt aColor = theColors->Value(iter);
    anArrayOfPoints->SetVertexColor (anArrayOfPoints->VertexNumber(),
                                     aColor.X(), aColor.Y(), aColor.Z());
  }
  myPoints = anArrayOfPoints;
  return true;
}

void AIS_PointCloud::SetColor (const Quantity_NameOfColor theColor)
{
  SetColor (Quantity_Color(theColor));
}

void AIS_PointCloud::SetColor (const Quantity_Color &theColor)
{
  if (!myDrawer->HasPointAspect())
  {
    myDrawer->SetPointAspect (new Prs3d_PointAspect (Aspect_TOM_POINT, theColor, 1.0));
    *myDrawer->PointAspect()->Aspect() = *myDrawer->Link()->PointAspect()->Aspect();
  }
  myDrawer->PointAspect()->SetColor (theColor);

  hasOwnColor = Standard_True;
  myOwnColor = theColor;

  Handle(Graphic3d_AspectMarker3d) aPointAspect = myDrawer->PointAspect()->Aspect();

  const Handle(Prs3d_Presentation)& aPrs = Presentations().Value (1).Presentation()->Presentation();
  
  // Set aspect for presentation
  aPrs->SetPrimitivesAspect (aPointAspect);

  const Handle(Graphic3d_Group)& aGroup = aPrs->Groups().First();

  // Check if aspect of given type is set for the group, 
  // because setting aspect for group with no already set aspect
  // can lead to loss of presentation data
  if (aGroup->IsGroupPrimitivesAspectSet (Graphic3d_ASPECT_MARKER))
  {
    aGroup->SetGroupPrimitivesAspect (aPointAspect);
  }
}

//=======================================================================
//function : Compute
//purpose  : 
//=======================================================================
void AIS_PointCloud::Compute(const Handle(PrsMgr_PresentationManager3d)& /*aPresentationManager*/,
                             const Handle(Prs3d_Presentation)& aPresentation,
                             const Standard_Integer /*aMode*/)
{
  aPresentation->Clear();
  if (myPoints.IsNull() || !myPoints->IsValid()) return;

  Handle(Graphic3d_Group) aGroup = Prs3d_Root::CurrentGroup (aPresentation);

  /************************************
  static Handle(Graphic3d_AspectMarker3d) PtA = new Graphic3d_AspectMarker3d();
  PtA->SetType(Aspect_TOM_POINT);
  PtA->SetScale(1.);
  Handle(Graphic3d_Group) TheGroup = Prs3d_Root::CurrentGroup(aPresentation);
  TheGroup->SetPrimitivesAspect(PtA);
  Handle(Graphic3d_ArrayOfPoints) aPoint = new Graphic3d_ArrayOfPoints (1);
  aPoint->AddVertex (myComponent->X(),myComponent->Y(),myComponent->Z());
  TheGroup->AddPrimitiveArray (aPoint);
  ******************************/

  if (Attributes()->HasPointAspect())
  {
    aGroup->SetPrimitivesAspect (Attributes()->PointAspect()->Aspect());
  }
  aGroup->AddPrimitiveArray (myPoints);
}

void AIS_PointCloud::ComputeSelection(const Handle(SelectMgr_Selection)& /*aSelection*/,
                                      const Standard_Integer /*aMode*/)
{
}
