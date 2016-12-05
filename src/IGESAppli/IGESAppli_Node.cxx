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
#include <IGESAppli_Node.hxx>
#include <IGESData_TransfEntity.hxx>
#include <IGESGeom_TransformationMatrix.hxx>
#include <Interface_Macros.hxx>
#include <Interface_EntityIterator.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESAppli_Node,IGESData_IGESEntity)

void IGESAppli_Node::Init
  (const gp_XYZ& aCoord,
   const Handle(IGESGeom_TransformationMatrix)& aCoordSystem)
{
  theCoord  = aCoord;
  theSystem = aCoordSystem;
  InitTypeAndForm(134,0);
}

gp_Pnt IGESAppli_Node::Coord () const
{
  return  gp_Pnt(theCoord);
}

Handle(IGESData_TransfEntity) IGESAppli_Node::System () const
{
  //if Null, Global Cartesian Coordinate System
  return Handle(IGESData_TransfEntity)(theSystem);
}

Standard_Integer IGESAppli_Node::SystemType () const
{
  if (theSystem.IsNull()) return 0;      // 0 Global Cartesien
  return (theSystem->FormNumber() - 9);  // 1 Cartesien, 2 Cylind. 3 Spher.
}

gp_Pnt IGESAppli_Node::TransformedNodalCoord () const
{
  gp_XYZ tempCoord = Coord().XYZ();
  Handle(IGESData_TransfEntity) temp = System();
  if (!temp.IsNull())    temp->Value().Transforms(tempCoord);
  return gp_Pnt(tempCoord);
}

void IGESAppli_Node::OwnShared(Interface_EntityIterator &theIter) const
{
  theIter.GetOneItem(System());
}

void IGESAppli_Node::OwnCheck (const Interface_ShareTool &, const Handle(Interface_Check) &theCheck) const
{
  if (!HasSubScriptNumber())
    theCheck->AddFail("SubScript Number expected (for Node Number) not present");
  if (!HasTransf())
    theCheck->AddFail("Transformation Matrix expected, not present");
  if (!System().IsNull())
    if (System()->FormNumber() < 10)
      theCheck->AddFail("System : Incorrect FormNumber (not 10-11-12)");
}
