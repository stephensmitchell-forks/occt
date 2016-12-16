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

#include <gp_Pnt.hxx>
#include <IGESData_ViewKindEntity.hxx>
#include <IGESDimen_LeaderArrow.hxx>
#include <IGESDraw_LabelDisplay.hxx>
#include <IGESDraw_PerspectiveView.hxx>
#include <IGESDraw_View.hxx>
#include <Interface_Macros.hxx>
#include <IGESFile_Reader.hxx>
#include <IGESData_IGESWriter.hxx>
#include <Interface_EntityIterator.hxx>
#include <IGESData_DirChecker.hxx>
#include <Message_Messenger.hxx>
#include <IGESData_IGESDumper.hxx>
#include <IGESData_Dump.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESDraw_LabelDisplay,IGESData_LabelDisplayEntity)

Standard_Integer IGESDraw_LabelDisplay::NbLabels () const
{
  return (theViews->Length());
}

const Handle(IGESData_ViewKindEntity) & IGESDraw_LabelDisplay::ViewItem (const Standard_Integer ViewIndex) const
{
  return theViews->Value(ViewIndex);
}

gp_Pnt IGESDraw_LabelDisplay::TextLocation (const Standard_Integer ViewIndex) const
{
  return ( gp_Pnt (theTextLocations->Value(ViewIndex)) );
}

const Handle(IGESDimen_LeaderArrow) & IGESDraw_LabelDisplay::LeaderEntity (const Standard_Integer ViewIndex) const
{
  return theLeaderEntities->Value(ViewIndex);
}

Standard_Integer IGESDraw_LabelDisplay::LabelLevel (const Standard_Integer ViewIndex) const
{
  return theLabelLevels->Value(ViewIndex);
}

const Handle(IGESData_IGESEntity) & IGESDraw_LabelDisplay::DisplayedEntity (const Standard_Integer EntityIndex) const
{
  return theDisplayedEntities->Value(EntityIndex);
}

gp_Pnt IGESDraw_LabelDisplay::TransformedTextLocation (const Standard_Integer ViewIndex) const
{
  gp_XYZ retXYZ;
  gp_XYZ tempXYZ = theTextLocations->Value(ViewIndex);

  Handle(IGESData_ViewKindEntity) tempView = theViews->Value(ViewIndex);
  if (tempView->IsKind(STANDARD_TYPE(IGESDraw_View)))
    {
      DeclareAndCast(IGESDraw_View, thisView, tempView);
      retXYZ = thisView->ModelToView( tempXYZ );
    }
  else if (tempView->IsKind(STANDARD_TYPE(IGESDraw_PerspectiveView)))
    {
      DeclareAndCast(IGESDraw_PerspectiveView, thisView, tempView);
      retXYZ = thisView->ModelToView( tempXYZ );
    }
  return ( gp_Pnt(retXYZ) );
}

void IGESDraw_LabelDisplay::OwnRead (IGESFile_Reader &PR)
{ 
  Standard_Integer nbval = 0;
  PR.ReadInteger(nbval,"No. of Label placements");
  if (nbval > 0)
  {
    theViews             = new IGESDraw_HArray1OfViewKindEntity(1, nbval);
    theTextLocations     = new TColgp_HArray1OfXYZ(1, nbval);
    theLeaderEntities    = new IGESDimen_HArray1OfLeaderArrow(1, nbval);
    theLabelLevels       = new TColStd_HArray1OfInteger(1, nbval);
    theDisplayedEntities = new IGESData_HArray1OfIGESEntity(1, nbval);
      
    Handle(IGESData_ViewKindEntity) tempView;
    Handle(IGESDimen_LeaderArrow)   tempLeaderArrow;
    Handle(IGESData_IGESEntity)     tempDisplayedEntity;
      
    for (Standard_Integer i = 1; i <= nbval; i++)
	{
	  PR.ReadPointer(theViews->ChangeValue(i),"Instance of views");
	  PR.ReadXYZ(theTextLocations->ChangeValue(i),"array textLocations");
	  PR.ReadPointer(theLeaderEntities->ChangeValue(i),"Instance of LeaderArrow");
	  PR.ReadInteger(theLabelLevels->ChangeValue(i),"array labelLevels");
	  PR.ReadPointer(theDisplayedEntities->ChangeValue(i),"displayedEntities entity");
	}
  }
  else  PR.AddFail("No. of Label placements : Not Positive");
}

void IGESDraw_LabelDisplay::OwnWrite (IGESData_IGESWriter &IW) const
{
  Standard_Integer Up = NbLabels();
  IW.Send( Up );
  for ( Standard_Integer i = 1; i <= Up; i++)
  {
    IW.Send( ViewItem(i) );
    IW.Send( TextLocation(i).X() );
    IW.Send( TextLocation(i).Y() );
    IW.Send( TextLocation(i).Z() );
    IW.Send( LeaderEntity(i) );
    IW.Send( LabelLevel(i) );
    IW.Send( DisplayedEntity(i) );
  }
}

void IGESDraw_LabelDisplay::OwnShared (Interface_EntityIterator &iter) const
{
  Standard_Integer Up = NbLabels();
  for ( Standard_Integer i = 1; i <= Up; i++)
  {
    iter.GetOneItem( ViewItem(i) );
    iter.GetOneItem( LeaderEntity(i) );
    iter.GetOneItem( DisplayedEntity(i) );
  }
}

IGESData_DirChecker IGESDraw_LabelDisplay::DirChecker () const
{ 
  IGESData_DirChecker DC (402, 5);
  DC.Structure(IGESData_DefVoid);
  DC.HierarchyStatusIgnored();
  DC.BlankStatusIgnored();
  return DC;
}

void IGESDraw_LabelDisplay::OwnDump (const IGESData_IGESDumper &dumper, const Handle(Message_Messenger) &S, const Standard_Integer level) const
{
  const Standard_Integer sublevel = (level <= 4) ? 0 : 1;
  S << "IGESDraw_LabelDisplay" << endl;
  S << "View Entities       : " << endl
    << "Text Locations      : " << endl
    << "Leader Entities     : " << endl
    << "Label Level Numbers : " << endl
    << "Displayed Entities  : ";
  S << "Count = "      << NbLabels() << endl;
  if (level > 4)  // Level = 4 : no Dump. Level = 5 & 6 : same Dump
  {
    const Standard_Integer up = NbLabels();
    for (Standard_Integer I = 1; I <= up; I ++)
    {
      S << "[" << I << "]:" << endl;
      S << "View Entity : ";
      dumper.Dump (ViewItem(I),S,sublevel);
      S << endl;
      S << "Text Location in View : ";
      IGESData_DumpXYZL(S,level,TextLocation(I),Location());
      S << "  Leader Entity in View : ";
      dumper.Dump(LeaderEntity(I),S,sublevel);
      S << endl;
      S << "Entity Label Level Number : ";
      S << LabelLevel(I) << "  ";
      S << "Displayed Entity : ";
      dumper.Dump(DisplayedEntity(I),S,sublevel);
      S << endl;
    }
  }
  S << endl;
}
