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

#include <AIS_Drawer.hxx>
#include <AIS_PointCloud.hxx>

#include <Graphic3d_Group.hxx>
#include <Graphic3d_AspectMarker3d.hxx>

#include <Prs3d_PointAspect.hxx>
#include <Prs3d_Presentation.hxx>
#include <Prs3d_Root.hxx>

#include <PrsMgr_ModedPresentation.hxx>
#include <PrsMgr_Presentations.hxx>

IMPLEMENT_STANDARD_HANDLE(AIS_PointCloud, AIS_InteractiveObject)
IMPLEMENT_STANDARD_RTTIEXT(AIS_PointCloud, AIS_InteractiveObject)

//==================================================
// Function: AIS_PointCloud
// Purpose : Constructor
//==================================================
AIS_PointCloud::AIS_PointCloud()
: AIS_InteractiveObject()
{
  SetHilightMode (0);
}

//=======================================================================
//function : SetPoints
//purpose  :
//=======================================================================
void AIS_PointCloud::SetPoints (const Handle(Graphic3d_ArrayOfPoints)& thePoints)
{
  myPoints.Nullify();

  if (thePoints.IsNull())
    return;

  myPoints = thePoints;
}

//=======================================================================
//function : SetPoints
//purpose  : 
//=======================================================================
void AIS_PointCloud::SetPoints (const Handle(TColgp_HArray1OfPnt)&     theCoords,
                                const Handle(Quantity_HArray1OfColor)& theColors)
{
  myPoints.Nullify();

  if (theCoords.IsNull())
    return;

  Standard_Integer aNumPoints = theCoords->Length();
  Standard_Boolean hasColors = !theColors.IsNull() && aNumPoints == theColors->Length();

  myPoints = new Graphic3d_ArrayOfPoints (aNumPoints, hasColors);
  for (Standard_Integer aPntIter = theCoords->Lower(); aPntIter <= theCoords->Upper(); aPntIter++)
  {
    myPoints->AddVertex (theCoords->Value (aPntIter));
  }

  if (hasColors)
  {
    Standard_Integer aNumVertex = 1;
    for(Standard_Integer aColorIter = theColors->Lower(); aColorIter <= theColors->Upper(); aColorIter++)
    {
      myPoints->SetVertexColor (aNumVertex, theColors->Value (aColorIter));
      aNumVertex++;
    }
  }
}

//=======================================================================
//function : GetPoints
//purpose  : 
//=======================================================================
const Handle(Graphic3d_ArrayOfPoints)& AIS_PointCloud::GetPoints() const
{
  return myPoints;
}

//=======================================================================
//function : SetColor
//purpose  : 
//=======================================================================
void AIS_PointCloud::SetColor (const Quantity_NameOfColor theColor)
{
  SetColor (Quantity_Color (theColor));
}

//=======================================================================
//function : SetColor
//purpose  :
//=======================================================================
void AIS_PointCloud::SetColor (const Quantity_Color& theColor)
{
  if (!myDrawer->HasPointAspect())
  {
    myDrawer->SetPointAspect (new Prs3d_PointAspect (Aspect_TOM_POINT, theColor, 1.0));
    *myDrawer->PointAspect()->Aspect() = *myDrawer->Link()->PointAspect()->Aspect();
  }
  myDrawer->PointAspect()->SetColor (theColor);

  hasOwnColor = Standard_True;
  myOwnColor = theColor;
}

//=======================================================================
//function : Compute
//purpose  : 
//=======================================================================
void AIS_PointCloud::Compute(const Handle(PrsMgr_PresentationManager3d)& /*thePresentationManager*/,
                             const Handle(Prs3d_Presentation)& thePresentation,
                             const Standard_Integer /*theMode*/)
{
  thePresentation->Clear();

  const Handle(Graphic3d_ArrayOfPoints)& aPoints = GetPoints();
  if (aPoints.IsNull() || !aPoints->IsValid())
    return;

  Handle(Graphic3d_AspectMarker3d) aMarkerAspect = myDrawer->PointAspect()->Aspect();
  if (!myDrawer->HasPointAspect())
  {
    aMarkerAspect->SetType (Aspect_TOM_POINT);
  }

  Handle(Graphic3d_Group) aGroup = Prs3d_Root::CurrentGroup (thePresentation);
  aGroup->SetGroupPrimitivesAspect (aMarkerAspect);
  aGroup->AddPrimitiveArray (aPoints);
}

//=======================================================================
//function : ComputeSelection
//purpose  : 
//=======================================================================
void AIS_PointCloud::ComputeSelection(const Handle(SelectMgr_Selection)& /*theSelection*/,
                                      const Standard_Integer /*theMode*/)
{
}
