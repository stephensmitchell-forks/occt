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

#include <IGESData_LineFontEntity.hxx>
#include <IGESDraw_SegmentedViewsVisible.hxx>
#include <IGESGraph_Color.hxx>
#include <IGESFile_Reader.hxx>
#include <IGESData_IGESWriter.hxx>
#include <Interface_EntityIterator.hxx>
#include <IGESData_DirChecker.hxx>
#include <Message_Messenger.hxx>
#include <IGESData_IGESDumper.hxx>
#include <IGESData_Dump.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESDraw_SegmentedViewsVisible,IGESData_ViewKindEntity)

Standard_Boolean IGESDraw_SegmentedViewsVisible::IsSingle () const
{
  return Standard_False;
}

Standard_Integer IGESDraw_SegmentedViewsVisible::NbViews () const
{
  return theViews->Length();
}

Standard_Integer IGESDraw_SegmentedViewsVisible::NbSegmentBlocks () const
{
  return theViews->Length();
}

Handle(IGESData_ViewKindEntity) IGESDraw_SegmentedViewsVisible::ViewItem (const Standard_Integer ViewIndex) const
{
  return theViews->Value(ViewIndex);
}

Standard_Real IGESDraw_SegmentedViewsVisible::BreakpointParameter (const Standard_Integer BreakpointIndex) const
{
  return theBreakpointParameters->Value(BreakpointIndex);
}

Standard_Integer IGESDraw_SegmentedViewsVisible::DisplayFlag (const Standard_Integer FlagIndex) const
{
  return theDisplayFlags->Value(FlagIndex);
}

Standard_Boolean IGESDraw_SegmentedViewsVisible::IsColorDefinition (const Standard_Integer ColorIndex) const
{
  return ( !theColorDefinitions->Value(ColorIndex).IsNull() );
}

Standard_Integer IGESDraw_SegmentedViewsVisible::ColorValue (const Standard_Integer ColorIndex) const
{
  return theColorValues->Value(ColorIndex);
}

Handle(IGESGraph_Color) IGESDraw_SegmentedViewsVisible::ColorDefinition (const Standard_Integer ColorIndex) const
{
  return theColorDefinitions->Value(ColorIndex);
}

Standard_Boolean IGESDraw_SegmentedViewsVisible::IsFontDefinition (const Standard_Integer FontIndex) const
{
  return ( !theLineFontDefinitions->Value(FontIndex).IsNull() );
}

Standard_Integer IGESDraw_SegmentedViewsVisible::LineFontValue (const Standard_Integer FontIndex) const
{
  return theLineFontValues->Value(FontIndex);
}

Handle(IGESData_LineFontEntity)  IGESDraw_SegmentedViewsVisible::LineFontDefinition (const Standard_Integer FontIndex) const
{
  return theLineFontDefinitions->Value(FontIndex);
}

Standard_Integer IGESDraw_SegmentedViewsVisible::LineWeightItem (const Standard_Integer WeightIndex) const
{
  return theLineWeights->Value(WeightIndex);
}

void IGESDraw_SegmentedViewsVisible::OwnRead (IGESFile_Reader &PR)
{
  Standard_Integer nbval = 0;
  PR.ReadInteger(nbval,"No. of View/segment blocks");
  if (nbval > 0)
  {
    theViews                = new IGESDraw_HArray1OfViewKindEntity(1, nbval);
    theBreakpointParameters = new TColStd_HArray1OfReal(1, nbval);
    theDisplayFlags         = new TColStd_HArray1OfInteger(1, nbval);
    theColorValues          = new TColStd_HArray1OfInteger(1, nbval);
    theColorDefinitions     = new IGESGraph_HArray1OfColor(1, nbval);
    theLineFontValues       = new TColStd_HArray1OfInteger(1, nbval);
    theLineFontDefinitions  = new IGESBasic_HArray1OfLineFontEntity(1, nbval);
    theLineWeights          = new TColStd_HArray1OfInteger(1, nbval);
    
    for (Standard_Integer i = 1; i <= nbval; i++)
    {
      PR.ReadPointer(theViews->ChangeValue(i),"Instance of views");
      PR.ReadReal(theBreakpointParameters->ChangeValue(i),"array breakpointParameters");
      PR.ReadInteger(theDisplayFlags->ChangeValue(i),"array displayFlags");

      Standard_Integer tempValue = 0;
      PR.ReadInteger(tempValue,"array colorValues");
      if (tempValue < 0) {
        tempValue = -1;
        PR.GetPointer(theColorDefinitions->ChangeValue(i),-tempValue);
      }
      theColorValues->SetValue(i, tempValue);

      tempValue = 0;
      PR.ReadInteger(tempValue,"array lineFontValues");
      if (tempValue < 0) {
        tempValue = -1;
        PR.GetPointer(theLineFontDefinitions->ChangeValue(i),-tempValue);
      }
      theLineFontValues->SetValue(i, tempValue);
	
      PR.ReadInteger(theLineWeights->ChangeValue(i),"array lineWeights");
    }
  }
  else
    PR.AddFail("No. of View/segment blocks : Not Positive");
}

void IGESDraw_SegmentedViewsVisible::OwnWrite (IGESData_IGESWriter &IW) const
{
  const Standard_Integer Up = NbSegmentBlocks();
  IW.Send( Up );
  for ( Standard_Integer i = 1; i <= Up; i++)
  {
    IW.Send( ViewItem(i) );
    IW.Send( BreakpointParameter(i) );
    IW.Send( DisplayFlag(i) );

    if (IsColorDefinition(i))
      IW.Send( ColorDefinition(i), Standard_True );     // negative
    else
      IW.Send( ColorValue(i) );

    if (IsFontDefinition(i))
      IW.Send( LineFontDefinition(i), Standard_True );  // negative
    else
      IW.Send( LineFontValue(i) );

    IW.Send( LineWeightItem(i) );
  }
}

void IGESDraw_SegmentedViewsVisible::OwnShared (Interface_EntityIterator &iter) const
{
  const Standard_Integer Up = NbSegmentBlocks();
  for ( Standard_Integer i = 1; i <= Up; i++)
  {
    iter.GetOneItem( ViewItem(i) );
    if (IsColorDefinition(i))
      iter.GetOneItem( ColorDefinition(i) );
    if (!IsFontDefinition(i))
      iter.GetOneItem( LineFontDefinition(i) );
  }
}

IGESData_DirChecker IGESDraw_SegmentedViewsVisible::DirChecker () const
{ 
  IGESData_DirChecker DC (402, 19);
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

void IGESDraw_SegmentedViewsVisible::OwnDump (const IGESData_IGESDumper &dumper, const Handle(Message_Messenger) &S, const Standard_Integer level) const
{
  const Standard_Integer sublevel = (level <= 4) ? 0 : 1;
  S << "IGESDraw_SegmentedViewsVisible" << endl;
  S << "View Entities            : " << endl
    << "Breakpoint parameters    : " << endl
    << "Display flags            : " << endl
    << "Color Values             : " << endl
    << "Color Definitions        : " << endl
    << "LineFont Values          : " << endl
    << "LineFont Definitions     : " << endl
    << "Line Weights : " << endl;
  S << "Count Of Blocks = "      << NbSegmentBlocks() << endl;
  switch (level)
  {
    case 4 : S << " [ for content, ask level > 4 ]" << endl;
      break; // Nothing to be dumped here
    case 5 :        // Presently level 5 and 6 have the same Dump
    case 6 :
      {
	Standard_Integer I;
	Standard_Integer up  = NbSegmentBlocks();
	for (I = 1; I <= up; I++)
          {
	    S << "[" << I << "]:" << endl;
	    S << "View Entity : ";
	    dumper.Dump (ViewItem(I),S, sublevel);
	    S << endl;
	    S << "Breakpoint parameter : " << BreakpointParameter(I)<<endl;
	    S << "Display Flag : " << DisplayFlag(I) << endl;
	    if ( (ColorDefinition(I)).IsNull() )
              {
		S << "Color Value : ";
		S << ColorValue(I);
              }
	    else
              {
		S << "Color Definition : ";
		dumper.Dump (ColorDefinition(I),S, sublevel);
              }
	    S << endl;
	    if ( (LineFontDefinition(I)).IsNull() )
              {
		S << "LineFont Value : " << LineFontValue(I);
              }
	    else
              {
		S << "LineFont Definition : ";
		dumper.Dump (LineFontDefinition(I),S, sublevel);
              }
	    S << endl;
	    S << "Line Weight : " << LineWeightItem(I) << endl;
          }
      }
      break;
    }
  S << endl;
}
