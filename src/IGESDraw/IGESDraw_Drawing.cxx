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

#include <gp_Pnt2d.hxx>
#include <gp_XY.hxx>
#include <gp_XYZ.hxx>
#include <IGESData_ViewKindEntity.hxx>
#include <IGESDraw_Drawing.hxx>
#include <IGESDraw_PerspectiveView.hxx>
#include <IGESDraw_View.hxx>
#include <IGESGraph_DrawingSize.hxx>
#include <IGESGraph_DrawingUnits.hxx>
#include <Interface_Macros.hxx>
#include <IGESFile_Reader.hxx>
#include <IGESData_IGESWriter.hxx>
#include <Interface_EntityIterator.hxx>
#include <IGESData_DirChecker.hxx>
#include <Message_Messenger.hxx>
#include <IGESData_IGESDumper.hxx>
#include <IGESData_Dump.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESDraw_Drawing,IGESData_IGESEntity)

Standard_Integer IGESDraw_Drawing::NbViews () const
{
  return (theViews.IsNull() ? 0 : theViews->Length());
}

const Handle(IGESData_ViewKindEntity) & IGESDraw_Drawing::ViewItem (const Standard_Integer ViewIndex) const
{
  return theViews->Value(ViewIndex);
}

gp_Pnt2d IGESDraw_Drawing::ViewOrigin (const Standard_Integer TViewIndex) const
{
  return (gp_Pnt2d (theViewOrigins->Value(TViewIndex)) );
}

Standard_Integer IGESDraw_Drawing::NbAnnotations () const
{
  return (theAnnotations.IsNull() ? 0 : theAnnotations->Length() );
}

const Handle(IGESData_IGESEntity) & IGESDraw_Drawing::Annotation (const Standard_Integer AnnotationIndex) const
{
  return theAnnotations->Value(AnnotationIndex);
}

gp_XY IGESDraw_Drawing::ViewToDrawing (const Standard_Integer NumView, const gp_XYZ& ViewCoords) const
{
  gp_XY         thisOrigin       = theViewOrigins->Value(NumView);
  Standard_Real XOrigin          = thisOrigin.X();
  Standard_Real YOrigin          = thisOrigin.Y();
  Standard_Real theScaleFactor=0.; 

  Handle(IGESData_ViewKindEntity) tempView = theViews->Value(NumView);
  if (tempView->IsKind(STANDARD_TYPE(IGESDraw_View)))
    {
      DeclareAndCast(IGESDraw_View, thisView, tempView);
      theScaleFactor   = thisView->ScaleFactor();
    }
  else if (tempView->IsKind(STANDARD_TYPE(IGESDraw_PerspectiveView)))
    {
      DeclareAndCast(IGESDraw_PerspectiveView, thisView, tempView);
      theScaleFactor   = thisView->ScaleFactor();
    }

  Standard_Real XV               = ViewCoords.X();
  Standard_Real YV               = ViewCoords.Y();

  Standard_Real XD = XOrigin + (theScaleFactor * XV);
  Standard_Real YD = YOrigin + (theScaleFactor * YV);

  return ( gp_XY(XD, YD) );
}

Standard_Boolean IGESDraw_Drawing::DrawingUnit (Standard_Real& val) const
{
  val = 0.;
  Handle(Standard_Type) typunit = STANDARD_TYPE(IGESGraph_DrawingUnits);
  if (NbTypedProperties(typunit) != 1) return Standard_False;
  DeclareAndCast(IGESGraph_DrawingUnits,units,TypedProperty(typunit)); 
  if (units.IsNull()) return Standard_False;
  val = units->UnitValue();
  return Standard_True;
}

Standard_Boolean IGESDraw_Drawing::DrawingSize (Standard_Real& X, Standard_Real& Y) const
{
  X = Y = 0.;
  Handle(Standard_Type) typsize = STANDARD_TYPE(IGESGraph_DrawingSize);
  if (NbTypedProperties(typsize) != 1) return Standard_False;
  DeclareAndCast(IGESGraph_DrawingSize,size,TypedProperty(typsize)); 
  if (size.IsNull()) return Standard_False;
  X = size->XSize();  Y = size->YSize();
  return Standard_True;
}

void IGESDraw_Drawing::OwnRead (IGESFile_Reader &PR)
{ 
  Standard_Integer nbval = 0;
  PR.ReadInteger(nbval,"Count of array of view entities");
  if (nbval > 0)
  {
    theViews       = new IGESDraw_HArray1OfViewKindEntity(1, nbval);
    theViewOrigins = new TColgp_HArray1OfXY(1, nbval);
    for (Standard_Integer i = 1; i <= nbval; i++)
    {
      PR.ReadPointer(theViews->ChangeValue(i),"View Entity",Standard_True);
      PR.ReadXY(theViewOrigins->ChangeValue(i),"array viewOrigins");
    }
  }
  else if (nbval < 0)
    PR.AddFail("Count of view entities : Less than zero");

  nbval = 0;
  PR.ReadInteger(nbval,"Count of array of Annotation entities");
  if (nbval > 0)
  {
    theAnnotations = new IGESData_HArray1OfIGESEntity(1, nbval);
    for (Standard_Integer i = 1; i <= nbval; i++)
    {
      PR.ReadPointer(theAnnotations->ChangeValue(i),"View Entity");
    }
  }
  else if (nbval < 0)
    PR.AddFail("Count of Annotation entities : Less than zero");
}

void IGESDraw_Drawing::OwnWrite (IGESData_IGESWriter &IW) const
{
  Standard_Integer Up = NbViews();
  IW.Send( Up );
  Standard_Integer i;
  for ( i = 1; i <= Up; i++)
  {
    IW.Send( ViewItem(i) );
    IW.Send( ViewOrigin(i).X() );
    IW.Send( ViewOrigin(i).Y() );
  }

  Up = NbAnnotations();
  IW.Send( Up );
  for ( i = 1; i <= Up; i++)
    IW.Send( Annotation(i) );
}

void IGESDraw_Drawing::OwnShared (Interface_EntityIterator &iter) const
{
  Standard_Integer Up = NbViews();
  Standard_Integer i;
  for ( i = 1; i <= Up; i++)
    iter.GetOneItem( ViewItem(i) );

  Up = NbAnnotations();
  for ( i = 1; i <= Up; i++)
    iter.GetOneItem( Annotation(i) );
}

IGESData_DirChecker IGESDraw_Drawing::DirChecker () const
{
  IGESData_DirChecker DC (404, 0);
  DC.Structure(IGESData_DefVoid);
  DC.LineFont(IGESData_DefVoid);
  DC.LineWeight(IGESData_DefVoid);
  DC.Color(IGESData_DefVoid);
  DC.BlankStatusIgnored();
  DC.SubordinateStatusRequired(0);
  DC.UseFlagRequired(1);
  DC.HierarchyStatusIgnored();
  return DC;
}

void IGESDraw_Drawing::OwnCheck (const Interface_ShareTool &, Handle(Interface_Check) &ach) const
{
  Standard_Boolean ianul = Standard_False;
  Standard_Integer i, nb = NbViews();
  for (i = 1; i <= nb; i ++) {
    const Handle(IGESData_ViewKindEntity) &tempView = ViewItem(i);
    if (tempView.IsNull()) ianul = Standard_True;
    else if (tempView->TypeNumber() == 0) ianul = Standard_True;
    if (ianul) {
      ach->AddWarning ("At least one View is Null");
      break;
    }
  }
  nb = NbAnnotations();
  for (i = 1; i <= nb; i ++) {
    const Handle(IGESData_IGESEntity) &ann = Annotation(i);
    if (ann.IsNull()) ianul = Standard_True;
    else if (ann->TypeNumber() == 0) ianul = Standard_True;
    if (ianul) {
      ach->AddWarning ("At least one Annotation is Null");
      break;
    }
  }
}

void IGESDraw_Drawing::OwnDump (const IGESData_IGESDumper &dumper, const Handle(Message_Messenger) &S, const Standard_Integer level) const
{
  const Standard_Integer sublevel = (level <= 4) ? 0 : 1;
  S << "IGESDraw_Drawing" << endl;
  S << "View Entities            : " << endl
    << "Transformed View Origins : ";
  S << "Count = "      << NbViews();
  switch (level)
  {
    case 4 : S << " [ ask level > 4 for content ]" << endl;
      break; // Nothing to be dumped here
    case 5 :        // Presently level 5 and 6 have the same Dump
      S << endl;
    case 6 :
    {
      const Standard_Integer up  = NbViews();
      for (Standard_Integer I = 1; I <= up; I++)
      {
        S << endl << "[" << I << "] ";
        S << "View Entity : ";
        dumper.Dump(ViewItem(I),S,sublevel);
        S << endl;
        S << "Transformed View Origin : ";
        IGESData_DumpXY(S,ViewOrigin(I));
      }
    }
    break;
  }
  S << endl << "Annotation Entities : ";
  IGESData_DumpEntities(S,dumper,level,1,NbAnnotations(),Annotation);
  S << endl;
}
