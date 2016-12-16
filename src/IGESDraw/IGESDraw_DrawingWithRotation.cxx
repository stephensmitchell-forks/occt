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
#include <IGESDraw_DrawingWithRotation.hxx>
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

IMPLEMENT_STANDARD_RTTIEXT(IGESDraw_DrawingWithRotation,IGESData_IGESEntity)

Standard_Integer IGESDraw_DrawingWithRotation::NbViews () const
{
  return (theViews->Length());
}

const Handle(IGESData_ViewKindEntity) & IGESDraw_DrawingWithRotation::ViewItem (const Standard_Integer Index) const
{
  return theViews->Value(Index);
}

gp_Pnt2d IGESDraw_DrawingWithRotation::ViewOrigin (const Standard_Integer Index) const
{
  return ( gp_Pnt2d (theViewOrigins->Value(Index)) );
}

Standard_Real IGESDraw_DrawingWithRotation::OrientationAngle (const Standard_Integer Index) const
{
  return theOrientationAngles->Value(Index);
}

Standard_Integer IGESDraw_DrawingWithRotation::NbAnnotations () const
{
  return (theAnnotations.IsNull() ? 0 : theAnnotations->Length() );
}

const Handle(IGESData_IGESEntity) & IGESDraw_DrawingWithRotation::Annotation (const Standard_Integer Index) const
{
  return ( theAnnotations->Value(Index) );
}

gp_XY IGESDraw_DrawingWithRotation::ViewToDrawing (const Standard_Integer NumView, const gp_XYZ& ViewCoords) const
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

  Standard_Real theta            = theOrientationAngles->Value(NumView);

  Standard_Real XD = 
    XOrigin + theScaleFactor * ( XV * Cos(theta) - YV * Sin(theta) );
  Standard_Real YD = 
    YOrigin + theScaleFactor * ( XV * Sin(theta) + YV * Cos(theta) );

  return ( gp_XY(XD, YD) );
}

Standard_Boolean IGESDraw_DrawingWithRotation::DrawingUnit (Standard_Real& val) const
{
  val = 0.;
  Handle(Standard_Type) typunit = STANDARD_TYPE(IGESGraph_DrawingUnits);
  if (NbTypedProperties(typunit) != 1) return Standard_False;
  DeclareAndCast(IGESGraph_DrawingUnits,units,TypedProperty(typunit)); 
  if (units.IsNull()) return Standard_False;
  val = units->UnitValue();
  return Standard_True;
}

Standard_Boolean IGESDraw_DrawingWithRotation::DrawingSize (Standard_Real& X, Standard_Real& Y) const
{
  X = Y = 0.;
  Handle(Standard_Type) typsize = STANDARD_TYPE(IGESGraph_DrawingSize);
  if (NbTypedProperties(typsize) != 1) return Standard_False;
  DeclareAndCast(IGESGraph_DrawingSize,size,TypedProperty(typsize)); 
  if (size.IsNull()) return Standard_False;
  X = size->XSize();  Y = size->YSize();
  return Standard_True;
}

void IGESDraw_DrawingWithRotation::OwnRead (IGESFile_Reader &PR)
{ 
  Standard_Integer nbval = 0;
  PR.ReadInteger(nbval,"count of array views");
  if (nbval > 0)
  {
    theViews             = new IGESDraw_HArray1OfViewKindEntity(1, nbval);
    theViewOrigins       = new TColgp_HArray1OfXY(1, nbval);
    theOrientationAngles = new TColStd_HArray1OfReal(1, nbval);

    for (Standard_Integer i = 1; i <= nbval; i++)
	{
      PR.ReadPointer(theViews->ChangeValue(i),"Instance of views",Standard_True);
	  PR.ReadXY(theViewOrigins->ChangeValue(i),"array viewOrigins");
	  
      Standard_Real tempOrient = 0.;
      PR.ReadReal(tempOrient,"array viewOrigins");
      theOrientationAngles->SetValue(i, tempOrient);
	}
  }
  else if (nbval <= 0)
    PR.AddFail("Count of view entities : Not Positive");

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

void IGESDraw_DrawingWithRotation::OwnWrite (IGESData_IGESWriter &IW) const
{ 
  Standard_Integer Up = NbViews();
  IW.Send( Up );
  Standard_Integer i;
  for ( i = 1; i <= Up; i++)
  {
    IW.Send( ViewItem(i) );
    IW.Send( ViewOrigin(i).X() );
    IW.Send( ViewOrigin(i).Y() );
    IW.Send( OrientationAngle(i) );
  }

  Up = NbAnnotations();
  IW.Send( Up );
  for ( i = 1; i <= Up; i++)
    IW.Send( Annotation(i) );
}

void IGESDraw_DrawingWithRotation::OwnShared (Interface_EntityIterator &iter) const
{
  Standard_Integer Up = NbViews();
  Standard_Integer i;
  for ( i = 1; i <= Up; i++)
    iter.GetOneItem( ViewItem(i) );
  Up = NbAnnotations();
  for ( i = 1; i <= Up; i++)
    iter.GetOneItem( Annotation(i) );
}

IGESData_DirChecker IGESDraw_DrawingWithRotation::DirChecker () const
{ 
  IGESData_DirChecker DC (404, 1);
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

void IGESDraw_DrawingWithRotation::OwnCheck (const Interface_ShareTool &, Handle(Interface_Check) &ach) const
{
  Standard_Boolean ianul = Standard_False;
  Standard_Integer i, nb = NbViews();
  for (i = 1; i <= nb; i ++) {
    Handle(IGESData_ViewKindEntity) tempView = ViewItem(i);
    if (tempView.IsNull()) ianul = Standard_True;
    else if (tempView->TypeNumber() == 0) ianul = Standard_True;
    if (ianul) {
      ach->AddWarning ("At least one View is Null");
      break;
    }
  }
  nb = NbAnnotations();
  for (i = 1; i <= nb; i ++) {
    Handle(IGESData_IGESEntity) ann = Annotation(i);
    if (ann.IsNull()) ianul = Standard_True;
    else if (ann->TypeNumber() == 0) ianul = Standard_True;
    if (ianul) {
      ach->AddWarning ("At least one Annotation is Null");
      break;
    }
  }
}

void IGESDraw_DrawingWithRotation::OwnDump (const IGESData_IGESDumper &dumper, const Handle(Message_Messenger) &S, const Standard_Integer level) const
{
  const Standard_Integer sublevel = (level <= 4) ? 0 : 1;
  S << "IGESDraw_DrawingWithRotation" << endl;
  S << "View Entities            : " << endl
    << "Transformed View Origins : " << endl
    << "Orientation Angles : ";
  S << "Count = "      << NbViews() << endl;
  if (level > 4)    // Level = 4 : no Dump. Level = 5 & 6 have same Dump
  {
    const Standard_Integer up = NbViews();
    for (Standard_Integer I = 1; I <= up; I++)
	{
      S << "[" << I << "]:" << endl;
      S << "View Entity : ";
      dumper.Dump (ViewItem(I),S,sublevel);
      S << endl;
      S << "Transformed View Origin : ";
      IGESData_DumpXY(S,ViewOrigin(I));
      S << "  Orientation Angle : " << OrientationAngle(I) << endl;
    }
  }
  S << "Annotation Entities : ";
  IGESData_DumpEntities(S,dumper,level,1,NbAnnotations(),Annotation);
  S << endl;
}
