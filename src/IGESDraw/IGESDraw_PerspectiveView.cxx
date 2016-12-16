// Created by: CKY / Contract Toubro-Larsen
// Copyright (c) 1993-1999 Matra Datavision
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

//--------------------------------------------------------------------
//--------------------------------------------------------------------

#include <gp_GTrsf.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec.hxx>
#include <gp_XY.hxx>
#include <gp_XYZ.hxx>
#include <IGESData_TransfEntity.hxx>
#include <IGESDraw_PerspectiveView.hxx>
#include <IGESFile_Reader.hxx>
#include <IGESData_IGESWriter.hxx>
#include <Interface_EntityIterator.hxx>
#include <IGESData_DirChecker.hxx>
#include <Message_Messenger.hxx>
#include <IGESData_IGESDumper.hxx>
#include <IGESData_Dump.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESDraw_PerspectiveView,IGESData_ViewKindEntity)

Standard_Boolean IGESDraw_PerspectiveView::IsSingle () const
{
  return Standard_True;
}

Standard_Integer IGESDraw_PerspectiveView::NbViews () const
{
  return 1;
}

Handle(IGESData_ViewKindEntity) IGESDraw_PerspectiveView::ViewItem (const Standard_Integer) const
{
  return Handle(IGESData_ViewKindEntity)(this);
}

gp_Vec IGESDraw_PerspectiveView::ViewNormalVector () const
{
  gp_Vec tempRes(theViewNormalVector);
  return tempRes;
}

gp_Pnt IGESDraw_PerspectiveView::ViewReferencePoint () const
{
  gp_Pnt tempRes(theViewReferencePoint);
  return tempRes;
}

gp_Pnt IGESDraw_PerspectiveView::CenterOfProjection () const
{
  gp_Pnt tempRes(theCenterOfProjection);
  return tempRes;
}

gp_Vec IGESDraw_PerspectiveView::ViewUpVector () const
{
  gp_Vec tempRes(theViewUpVector);
  return tempRes;
}

gp_Pnt2d IGESDraw_PerspectiveView::TopLeft () const
{
  gp_Pnt2d tempRes(theTopLeft);
  return tempRes;
}

gp_Pnt2d IGESDraw_PerspectiveView::BottomRight () const
{
  gp_Pnt2d tempRes(theBottomRight);
  return tempRes;
}

gp_XYZ IGESDraw_PerspectiveView::ModelToView (const gp_XYZ& coords) const
{
  gp_XYZ tempCoords = coords;
  Location().Transforms(tempCoords);
  return (tempCoords);
}

void IGESDraw_PerspectiveView::OwnRead (IGESFile_Reader &PR)
{
  PR.ReadInteger(theViewNumber,"View Number");
  PR.ReadReal(theScaleFactor,"Scale Number");
  PR.ReadXYZ(theViewNormalVector,"View Plane Normal Vector");
  PR.ReadXYZ(theViewReferencePoint,"View Reference Point");
  PR.ReadXYZ(theCenterOfProjection,"Center Of Projection");
  PR.ReadXYZ(theViewUpVector,"View Up Vector");
  PR.ReadReal(theViewPlaneDistance,"View Plane Distance");

  Standard_Real r = 0.;
  PR.ReadReal(r,"Left Side Of Clipping Window");
  theTopLeft.SetX(r);

  r = 0.;
  PR.ReadReal(r,"Right Side Of Clipping Window");
  theBottomRight.SetX(r);

  r = 0.;
  PR.ReadReal(r,"Bottom Of Clipping Window");
  theBottomRight.SetY(r);

  r = 0.;
  PR.ReadReal(r,"Top Of Clipping Window");
  theTopLeft.SetY(r);

  PR.ReadInteger(theDepthClip,"Depth Clipping Indicator");
  PR.ReadReal(theBackPlaneDistance,"Back Plane Distance");
  PR.ReadReal(theFrontPlaneDistance,"Front Plane Distance");
}

void IGESDraw_PerspectiveView::OwnWrite (IGESData_IGESWriter &IW) const
{
  IW.Send(theViewNumber);
  IW.Send(theScaleFactor);
  IW.Send(theViewNormalVector.X());
  IW.Send(theViewNormalVector.Y());
  IW.Send(theViewNormalVector.Z());
  IW.Send(theViewReferencePoint.X());
  IW.Send(theViewReferencePoint.Y());
  IW.Send(theViewReferencePoint.Z());
  IW.Send(theCenterOfProjection.X());
  IW.Send(theCenterOfProjection.Y());
  IW.Send(theCenterOfProjection.Z());
  IW.Send(theViewUpVector.X());
  IW.Send(theViewUpVector.Y());
  IW.Send(theViewUpVector.Z());
  IW.Send(theViewPlaneDistance);
  IW.Send(theTopLeft.X());
  IW.Send(theBottomRight.X());
  IW.Send(theBottomRight.Y());
  IW.Send(theTopLeft.Y());
  IW.Send(theDepthClip);
  IW.Send(theBackPlaneDistance);
  IW.Send(theFrontPlaneDistance);
}

IGESData_DirChecker IGESDraw_PerspectiveView::DirChecker () const
{
  IGESData_DirChecker DC(410, 1);
  DC.Structure(IGESData_DefVoid);
  DC.LineFont(IGESData_DefVoid);
  DC.LineWeight(IGESData_DefVoid);
  DC.Color(IGESData_DefVoid);
  DC.BlankStatusIgnored();
  DC.UseFlagRequired(1);
  DC.HierarchyStatusIgnored();
  return DC;
}

void IGESDraw_PerspectiveView::OwnCheck (const Interface_ShareTool &, Handle(Interface_Check) &ach) const
{
  if ((theDepthClip < 0) || (theDepthClip > 3))
    ach->AddFail("DepthClip has invalid value");
  if (HasTransf()) {
    if (Transf()->FormNumber() != 0)
      ach->AddFail("Associated Matrix has not Form Number 0");
  }
}

void IGESDraw_PerspectiveView::OwnDump (const IGESData_IGESDumper &, const Handle(Message_Messenger) &S, const Standard_Integer level) const
{
  S << "IGESDraw_PerspectiveView" << endl;
  S << "View Number  : " << theViewNumber  << "  ";
  S << "Scale Factor : " << theScaleFactor << endl;
  S         << "View Plane Normal Vector : ";
  IGESData_DumpXYZL(S,level,theViewNormalVector,Location());
  S << endl << "View Reference Point     : ";
  IGESData_DumpXYZL(S,level,theViewReferencePoint,Location());
  S << endl << "Center Of Projection     : ";
  IGESData_DumpXYZL(S,level,theCenterOfProjection,Location());
  S << endl << "View Up Vector           : ";
  IGESData_DumpXYZL(S,level,theViewUpVector,Location());
  S << endl << "View Plane Distance      : " << theViewPlaneDistance << endl;
  S << "Left   Side Of Clipping Window : " << theTopLeft.X()     << endl;
  S << "Right  Side Of Clipping Window : " << theBottomRight.X() << endl;
  S << "Bottom Side Of Clipping Window : " << theBottomRight.Y() << endl;
  S << "Top    Side Of Clipping Window : " << theTopLeft.Y()     << endl;
  S << "Depth Clipping : " << theDepthClip;
  switch (theDepthClip) {
    case 0 :  S << " (No Depth Clipping)" << endl;                    break;
    case 1 :  S << " (Back Clipping Plane ON)" << endl;               break;
    case 2 :  S << " (Front Clipping Plane ON)" << endl;              break;
    case 3 :  S << " (Front and Back Clipping Planes ON)" << endl;    break;
    default : S << " (Invalid Value)" << endl;                        break;
  }
  S << "Back Plane Distance  : " << theBackPlaneDistance  << "  ";
  S << "Front Plane Distance : " << theFrontPlaneDistance << endl;
  S << endl;
}
