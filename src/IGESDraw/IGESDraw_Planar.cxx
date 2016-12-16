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

#include <IGESDraw_Planar.hxx>
#include <IGESGeom_TransformationMatrix.hxx>
#include <IGESFile_Reader.hxx>
#include <IGESData_IGESWriter.hxx>
#include <Interface_EntityIterator.hxx>
#include <IGESData_DirChecker.hxx>
#include <Message_Messenger.hxx>
#include <IGESData_IGESDumper.hxx>
#include <IGESData_Dump.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESDraw_Planar,IGESData_IGESEntity)

Standard_Integer IGESDraw_Planar::NbEntities () const
{
  return ( theEntities.IsNull()? 0 : theEntities->Length() );
}

const Handle(IGESData_IGESEntity) & IGESDraw_Planar::Entity (const Standard_Integer EntityIndex) const
{
  return theEntities->Value(EntityIndex);
}

void IGESDraw_Planar::OwnRead (IGESFile_Reader &PR)
{ 
  theNbMatrices = 1;
  PR.ReadInteger(theNbMatrices,"No. of Transformation matrices");
  if ( theNbMatrices != 1 )
    PR.AddFail("No. of Transformation matrices != 1");

  Standard_Integer nbval = 0;
  PR.ReadInteger(nbval,"No. of Entities in this plane");
  if (nbval <= 0) PR.AddFail ("No. of Entities in this plane : Not Positive");

  // Reading transformationMatrix(Instance of TransformationMatrix or Null)
  PR.ReadPointer(theTransformationMatrix,"Instance of TransformationMatrix",Standard_True);

  if (nbval > 0)
  {
    theEntities = new IGESData_HArray1OfIGESEntity(1, nbval);
    for (Standard_Integer i = 1; i <= nbval; i++)
      PR.ReadPointer(theEntities->ChangeValue(i),"Plane entity");
  }
}

void IGESDraw_Planar::OwnWrite (IGESData_IGESWriter &IW) const
{ 
  IW.Send( theNbMatrices );
  Standard_Integer Up = NbEntities();
  IW.Send( Up );
  IW.Send( theTransformationMatrix );
  for ( Standard_Integer i = 1; i <= Up; i++)
    IW.Send( Entity(i) );
}

void IGESDraw_Planar::OwnShared (Interface_EntityIterator &iter) const
{
  Standard_Integer Up = NbEntities();
  iter.GetOneItem( theTransformationMatrix );
  for ( Standard_Integer i = 1; i <= Up; i++)
    iter.GetOneItem( Entity(i) );
}

IGESData_DirChecker IGESDraw_Planar::DirChecker () const
{ 
  IGESData_DirChecker DC (402, 16);
  DC.Structure(IGESData_DefVoid);
  DC.LineFont(IGESData_DefVoid);
  DC.LineWeight(IGESData_DefVoid);
  DC.Color(IGESData_DefVoid);
  DC.BlankStatusIgnored();
  DC.UseFlagRequired(5);
  DC.HierarchyStatusIgnored();
  return DC;
}

void IGESDraw_Planar::OwnDump (const IGESData_IGESDumper &dumper, const Handle(Message_Messenger) &S, const Standard_Integer level) const
{
  const Standard_Integer sublevel = (level <= 4) ? 0 : 1;
  S << "IGESDraw_Planar" << endl;
  S << "No. of Transformation Matrices : " << theNbMatrices << "  ";
  S << "i.e. : ";
  if ( theTransformationMatrix.IsNull() )
    S << "Null Handle";
  else
    theTransformationMatrix->OwnDump(dumper,S,sublevel);
  S << endl;
  S << "Array of Entities on the specified plane : ";
  IGESData_DumpEntities(S,dumper,level,1,NbEntities(),Entity);
  S << endl;
}
