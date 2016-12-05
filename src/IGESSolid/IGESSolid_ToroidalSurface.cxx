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
#include <IGESGeom_Direction.hxx>
#include <IGESGeom_Point.hxx>
#include <IGESSolid_ToroidalSurface.hxx>
#include <Interface_EntityIterator.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESSolid_ToroidalSurface,IGESData_IGESEntity)

void IGESSolid_ToroidalSurface::Init
  (const Handle(IGESGeom_Point)& aCenter,
   const Handle(IGESGeom_Direction)& anAxis,
   const Standard_Real majRadius, const Standard_Real minRadius,
   const Handle(IGESGeom_Direction)& Refdir)
{
  theCenter      = aCenter;
  theAxis        = anAxis;
  theMajorRadius = majRadius;
  theMinorRadius = minRadius;
  theRefDir      = Refdir;
  InitTypeAndForm(198, (theRefDir.IsNull() ? 0 : 1) );
}

gp_Pnt IGESSolid_ToroidalSurface::TransformedCenter () const
{
  if (!HasTransf())
    return theCenter->Value();

  gp_XYZ tmp = theCenter->Value().XYZ();
  Location().Transforms(tmp);
  return gp_Pnt(tmp);
}

void IGESSolid_ToroidalSurface::OwnShared(Interface_EntityIterator &theIter) const
{
  theIter.GetOneItem(Center());
  theIter.GetOneItem(Axis());
  theIter.GetOneItem(ReferenceDir());
}

void IGESSolid_ToroidalSurface::OwnCheck (const Interface_ShareTool &, const Handle(Interface_Check) &theCheck) const
{
  if (MajorRadius() <= 0.0)
    theCheck->AddFail("Major Radius : Not Positive");
  if (MinorRadius() <= 0.0)
    theCheck->AddFail("Minor Radius : Not Positive");
  if (MinorRadius() >= MajorRadius())
    theCheck->AddFail("Minor Radius : Value not < Major radius");
  const Standard_Integer fn = (IsParametrised()? 1 : 0);
  if (fn != FormNumber()) theCheck->AddFail("Parametrised Status Mismatches with Form Number");
}
