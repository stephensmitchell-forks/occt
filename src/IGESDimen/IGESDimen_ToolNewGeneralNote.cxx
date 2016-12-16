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

#include <gp_XYZ.hxx>
#include <IGESData_DirChecker.hxx>
#include <IGESData_Dump.hxx>
#include <IGESData_HArray1OfIGESEntity.hxx>
#include <IGESData_IGESDumper.hxx>
#include <IGESData_IGESEntity.hxx>
#include <IGESData_IGESReaderData.hxx>
#include <IGESData_IGESWriter.hxx>
#include <IGESData_ParamCursor.hxx>
#include <IGESData_ParamReader.hxx>
#include <IGESDimen_NewGeneralNote.hxx>
#include <IGESDimen_ToolNewGeneralNote.hxx>
#include <Interface_Check.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_HArray1OfHAsciiString.hxx>
#include <Interface_Macros.hxx>
#include <Interface_ShareTool.hxx>
#include <Message_Messenger.hxx>
#include <Standard_DomainError.hxx>
#include <TColgp_HArray1OfXYZ.hxx>
#include <TCollection_HAsciiString.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <TColStd_HArray1OfReal.hxx>

#include <stdio.h>

void  IGESDimen_ToolNewGeneralNote::ReadOwnParams
  (const Handle(IGESDimen_NewGeneralNote)& ent,
   const Handle(IGESData_IGESReaderData)& IR, IGESData_ParamReader& PR) const
{ 
  //Standard_Boolean st; //szv#4:S4163:12Mar99 moved down

  Standard_Integer nbval;
  Standard_Real width; 
  Standard_Real height; 
  Standard_Integer justifyCode;
  gp_XYZ areaLoc; 
  Standard_Real areaRotationAngle; 
  gp_XYZ baseLinePos;
  Standard_Real normalInterlineSpace; 
  Handle(TColStd_HArray1OfInteger) charDisplays;
  Handle(TColStd_HArray1OfReal) charWidths; 
  Handle(TColStd_HArray1OfReal) charHeights; 
  Handle(TColStd_HArray1OfReal) interCharSpaces;
  Handle(TColStd_HArray1OfReal) interlineSpaces; 
  Handle(TColStd_HArray1OfInteger) fontStyles;
  Handle(TColStd_HArray1OfReal) charAngles; 
  Handle(Interface_HArray1OfHAsciiString) controlCodeStrings;
  Handle(TColStd_HArray1OfInteger) nbChars; 
  Handle(TColStd_HArray1OfReal) boxWidths; 
  Handle(TColStd_HArray1OfReal) boxHeights;
  Handle(TColStd_HArray1OfInteger) charSetCodes; 
  Handle(IGESData_HArray1OfIGESEntity) charSetEntities;
  Handle(TColStd_HArray1OfReal) slantAngles; 
  Handle(TColStd_HArray1OfReal) rotationAngles;
  Handle(TColStd_HArray1OfInteger) mirrorFlags; 
  Handle(TColStd_HArray1OfInteger) rotateFlags;
  Handle(TColgp_HArray1OfXYZ) startPoints; 
  Handle(Interface_HArray1OfHAsciiString) texts;

  PR.ReadReal(width,"Text Width");
  PR.ReadReal(height,"Text Height");
  PR.ReadInteger(justifyCode,"Justification Code");
  PR.ReadXYZ(areaLoc,"Area Location Point");
  PR.ReadReal(areaRotationAngle,"Area Rotation Angle");
  PR.ReadXYZ(baseLinePos,"Base Line Position");
  PR.ReadReal(normalInterlineSpace,"NormalInterline Spacing");

  Standard_Boolean st = PR.ReadInteger(nbval,"Number of Text Strings");
  if (st && nbval > 0)
    {
      charDisplays         = new TColStd_HArray1OfInteger(1, nbval);
      charWidths           = new TColStd_HArray1OfReal(1, nbval);
      charHeights          = new TColStd_HArray1OfReal(1, nbval);
      interCharSpaces      = new TColStd_HArray1OfReal(1, nbval);
      interlineSpaces      = new TColStd_HArray1OfReal(1, nbval);
      fontStyles           = new TColStd_HArray1OfInteger(1, nbval);
      charAngles           = new TColStd_HArray1OfReal(1, nbval);
      controlCodeStrings   = new Interface_HArray1OfHAsciiString(1, nbval);
      nbChars              = new TColStd_HArray1OfInteger(1, nbval);
      boxWidths            = new TColStd_HArray1OfReal(1, nbval);
      boxHeights           = new TColStd_HArray1OfReal(1, nbval);
      charSetCodes         = new TColStd_HArray1OfInteger(1, nbval);
      charSetEntities      = new IGESData_HArray1OfIGESEntity(1,nbval);
      slantAngles          = new TColStd_HArray1OfReal(1, nbval);
      rotationAngles       = new TColStd_HArray1OfReal(1, nbval);
      mirrorFlags          = new TColStd_HArray1OfInteger(1, nbval);
      rotateFlags          = new TColStd_HArray1OfInteger(1, nbval);
      startPoints          = new TColgp_HArray1OfXYZ(1, nbval);
      texts                = new Interface_HArray1OfHAsciiString(1, nbval);
    }
  else  PR.AddFail("Number of Text Strings: Not Positive");

  if (! charDisplays.IsNull())
    for (Standard_Integer i = 1; i <= nbval; i++)
      {
	Standard_Integer charDisplay;
	Standard_Real charWidth;
	Standard_Real charHeight;
	Standard_Real interCharSpace;
	Standard_Real interlineSpace;
	Standard_Integer fontStyle;
	Standard_Real charAngle;
	Handle(TCollection_HAsciiString) controlCodeString;
	Standard_Integer nbChar;
	Standard_Real boxWidth;
	Standard_Real boxHeight;
	Standard_Integer charSetCode;
	Handle(IGESData_IGESEntity) charSetEntity;
	Standard_Real slantAngle;
	Standard_Real rotationAngle;
	Standard_Integer mirrorFlag;
	Standard_Integer rotateFlag;
	gp_XYZ startPoint;
	Handle(TCollection_HAsciiString) text;

	if (PR.ReadInteger(charDisplay,"Character Display"))
	  charDisplays->SetValue(i, charDisplay);

	if (PR.ReadReal(charWidth,"Character Width"))
	  charWidths->SetValue(i, charWidth);

	if (PR.ReadReal(charHeight,"Character Height"))
	  charHeights->SetValue(i, charHeight);

	if (PR.ReadReal(interCharSpace,"Inter-character space"))
	  interCharSpaces->SetValue(i, interCharSpace);

	if (PR.ReadReal(interlineSpace,"Interline space"))
	  interlineSpaces->SetValue(i, interlineSpace);

	if (PR.ReadInteger(fontStyle,"Font Style"))
	  fontStyles->SetValue(i, fontStyle);

	if (PR.ReadReal(charAngle,"Character Angle"))
	  charAngles->SetValue(i, charAngle);

	if (PR.ReadText("Control Code String", controlCodeString))
	  controlCodeStrings->SetValue(i, controlCodeString);

	if (PR.ReadInteger(nbChar,"Number of Characters"))
	  nbChars->SetValue(i, nbChar);

	if (PR.ReadReal(boxWidth,"Box Width"))
	  boxWidths->SetValue(i, boxWidth);

	if (PR.ReadReal(boxHeight,"Box Height"))
	  boxHeights->SetValue(i, boxHeight);

	Standard_Integer curnum = PR.CurrentNumber();
	if (PR.DefinedElseSkip())
	  {
	    // Reading fontCode(Integer, must be positive)
	    PR.ReadInteger(charSetCode,"Character Set Interpretation Code");
	    // Reading charSetEnt
	    if (charSetCode < 0) {
	      charSetEntity = PR.ParamEntity (IR,curnum);
	      if (charSetEntity.IsNull())
		PR.AddFail("Char Set Interpretation Entity : incorrect reference");
	      charSetEntities->SetValue(i, charSetEntity);
	      charSetCodes->SetValue(i, -1);
	    } else {
	      charSetCodes->SetValue(i, charSetCode);
	    }
	  }

	else
	  charSetCodes->SetValue(i, 1);

	if (PR.DefinedElseSkip())
          {
	    if (PR.ReadReal(slantAngle,"Slant Angle"))
	      slantAngles->SetValue(i, slantAngle);
          }
	else
	  slantAngles->SetValue(i, M_PI/2);

	if (PR.ReadReal(rotationAngle,"Rotation Angle"))
	  rotationAngles->SetValue(i, rotationAngle);

	if (PR.ReadInteger(mirrorFlag,"Mirror Flag"))
	  mirrorFlags->SetValue(i, mirrorFlag);

	if (PR.ReadInteger(rotateFlag,"Rotate Flag"))
	  rotateFlags->SetValue(i, rotateFlag);

	if (PR.ReadXYZ(startPoint,"Start Point"))
	  startPoints->SetValue(i, startPoint);

	if (PR.ReadText("Text String", text))
	  texts->SetValue(i, text);
      }

  DirChecker(ent).CheckTypeAndForm(PR.CCheck(),ent);
  ent->Init
    ( width, height, justifyCode, areaLoc, areaRotationAngle,baseLinePos,
     normalInterlineSpace, charDisplays, charWidths, charHeights, 
     interCharSpaces, interlineSpaces, fontStyles, charAngles, 
     controlCodeStrings, nbChars, boxWidths, boxHeights, charSetCodes, 
     charSetEntities, slantAngles, rotationAngles, mirrorFlags, rotateFlags,
     startPoints, texts);
}

void  IGESDimen_ToolNewGeneralNote::WriteOwnParams
  (const Handle(IGESDimen_NewGeneralNote)& ent, IGESData_IGESWriter& IW) const 
{ 
  IW.Send(ent->TextWidth());
  IW.Send(ent->TextHeight());
  IW.Send(ent->JustifyCode());
  IW.Send(ent->AreaLocation().X());
  IW.Send(ent->AreaLocation().Y());
  IW.Send(ent->AreaLocation().Z());
  IW.Send(ent->AreaRotationAngle());
  IW.Send(ent->BaseLinePosition().X());
  IW.Send(ent->BaseLinePosition().Y());
  IW.Send(ent->BaseLinePosition().Z());
  IW.Send(ent->NormalInterlineSpace());
  Standard_Integer nbval = ent->NbStrings();
  IW.Send(nbval);

  for (Standard_Integer i = 1; i <= nbval; i++)
    {
      IW.Send(ent->CharacterDisplay(i));
      IW.Send(ent->CharacterWidth(i));
      IW.Send(ent->CharacterHeight(i));
      IW.Send(ent->InterCharacterSpace(i));
      IW.Send(ent->InterlineSpace(i));
      IW.Send(ent->FontStyle(i));
      IW.Send(ent->CharacterAngle(i));
      IW.Send(ent->ControlCodeString(i));
      IW.Send(ent->NbCharacters(i));
      IW.Send(ent->BoxWidth(i));
      IW.Send(ent->BoxHeight(i));
      if (ent->IsCharSetEntity(i))
	IW.Send(ent->CharSetEntity(i),Standard_True);  // negative
      else
	IW.Send(ent->CharSetCode(i));
      IW.Send(ent->SlantAngle(i));
      IW.Send(ent->RotationAngle(i));
      IW.Send(ent->MirrorFlag(i));
      IW.Send(ent->RotateFlag(i));
      IW.Send((ent->StartPoint(i)).X());
      IW.Send((ent->StartPoint(i)).Y());
      IW.Send((ent->StartPoint(i)).Z());
      IW.Send(ent->Text(i));
    }
}

void  IGESDimen_ToolNewGeneralNote::OwnShared
  (const Handle(IGESDimen_NewGeneralNote)& ent, Interface_EntityIterator& iter) const
{
  Standard_Integer nbval = ent->NbStrings();
  for (Standard_Integer i = 1; i <= nbval; i++)
    {

      if (ent->IsCharSetEntity(i))
	iter.GetOneItem(ent->CharSetEntity(i));
    }
}

IGESData_DirChecker  IGESDimen_ToolNewGeneralNote::DirChecker
  (const Handle(IGESDimen_NewGeneralNote)& /* ent */ ) const 
{ 
  IGESData_DirChecker DC (213, 0);
  DC.Structure(IGESData_DefVoid);
  DC.LineFont(IGESData_DefValue);
  DC.LineWeight(IGESData_DefValue);
  DC.Color(IGESData_DefAny);
  DC.UseFlagRequired(1);
  DC.HierarchyStatusIgnored();
  return DC;
}

void  IGESDimen_ToolNewGeneralNote::OwnCheck
  (const Handle(IGESDimen_NewGeneralNote)& ent,
   const Interface_ShareTool& , Handle(Interface_Check)& ach) const 
{

  Standard_Integer jcode = ent->JustifyCode();
  if ((jcode < 0) || (jcode > 3))
    ach->AddFail("Justify Code != 0, 1, 2, 3");
  Standard_Integer upper = ent->NbStrings();
  for (Standard_Integer i = 1; i <= upper; i ++)
    {
      if (ent->NbCharacters(i) != ent->Text(i)->Length())
	{
	  char mess[80];
	  sprintf(mess,"%d : Number of Characters != Length of Text String",i);
          ach->AddFail(mess);
	}

      Standard_Integer charcode = ent->CharSetCode(i);
      if ( (charcode >= 0) && (charcode != 1) &&
	  ((charcode < 1001) || (charcode > 1003)))
	{
	  char mess[80];
	  sprintf(mess, "%d : Character Set Code != 1, 1001, 1002, 1003",i);
          ach->AddFail(mess);
	}

      Standard_Integer chardisp = ent->CharacterDisplay(i);
      if ((chardisp < 0) || (chardisp > 1))
	{
	  char mess[80];
	  sprintf(mess, "%d : Character Display != 0, 1",i);
          ach->AddFail(mess);
	}

      Standard_Integer mflag = ent->MirrorFlag(i);
      if ((mflag < 0) || (mflag > 2))
	{
	  char mess[80];
	  sprintf(mess, "%d : Mirror flag != 0, 1, 2",i);
          ach->AddFail(mess);
	}

      Standard_Integer rflag = ent->RotateFlag(i);
      if ((rflag < 0) || (rflag > 1))
	{
	  char mess[80];
	  sprintf(mess, "%d : Rotate flag != 0, 1",i);
	  ach->AddFail(mess);
	}
    }
}

void  IGESDimen_ToolNewGeneralNote::OwnDump
  (const Handle(IGESDimen_NewGeneralNote)& ent, const IGESData_IGESDumper& dumper,
   const Handle(Message_Messenger)& S, const Standard_Integer level) const 
{ 
  Standard_Integer sublevel = (level > 4) ? 1 : 0;
  Standard_Integer nbval = ent->NbStrings();

  S << "IGESDimen_NewGeneralNote" << endl;
  S << "Text Area  : Width : " << ent->TextWidth() << "  ";
  S << "Height : " << ent->TextHeight() << "  ";
  S << "Justification Code  : " << ent->JustifyCode() << endl;
  S << "Text Area Location Point : ";
  IGESData_DumpXYZL(S,level, ent->AreaLocation(), ent->Location());
  S << "Rotation Angle of Text : " << ent->AreaRotationAngle() << endl;
  S << "Base Line Position : ";
  IGESData_DumpXYZL(S,level, ent->BaseLinePosition(), ent->Location());
  S << "Normal Interline Spacing : " << ent->NormalInterlineSpace() << endl;
  S << "Number of Text Strings : " << nbval << endl;

  S << "Character Display : " << endl; 
  S << "Character Width : " << endl; 
  S << "Character Height : " << endl; 
  S << "Inter Character Spacing : " << endl; 
  S << "Interline Spacing : " << endl; 
  S << "Font Styles : " << endl; 
  S << "Character Angle : " << endl; 
  S << "Control Code String : " << endl; 
  S << "Number of Characters : " << endl; 
  S << "Box Widths : " << endl; 
  S << "Box Heights : " << endl; 
  S << "Character Set Codes : " << endl; 
  S << "Character Set Entities : " << endl; 
  S << "Slant Angles : " << endl; 
  S << "Rotation Angles : " << endl; 
  S << "Mirror Flags : " << endl; 
  S << "Rotate Flags : " << endl; 
  S << "Start Points : " << endl; 
  S << "Texts : "; 
  IGESData_DumpVals(S,-level,1, nbval, ent->NbCharacters);
  S << endl;
  if (level > 4)
    {
      S << "Details of each String" << endl;
      for ( Standard_Integer i = 1; i <= nbval; i++)
	{
          S << "[" << i << "]: " << endl;
          S << "Character Display : " << ent->CharacterDisplay(i) << "  ";
          S << "Character Width   : " << ent->CharacterWidth(i)   << "  ";
          S << "Character Height  : " << ent->CharacterHeight(i)  << endl;
          S << "Inter Character Spacing : "<<ent->InterCharacterSpace(i)<<"  ";
          S << "Interline Spacing : " << ent->InterlineSpace(i) << endl;
          S << "Font Styles       : " << ent->FontStyle(i) << endl;
          S << "Character Angle   : " << ent->CharacterAngle(i) << endl;
          S << "Control Code String : " ;
          IGESData_DumpString(S,ent->ControlCodeString(i));
          S << endl;
          S << "Number of Characters : " << ent->NbCharacters(i) << "  ";
          S << "Box Width  : " << ent->BoxWidth(i)  << "  ";
          S << "Box Height : " << ent->BoxHeight(i) << endl;
          if (ent->IsCharSetEntity(i))
	    {
              S << "Character Set Entity : ";
              dumper.Dump (ent->CharSetEntity(i),S, sublevel);
              S << endl;
	    }
          else
	    S << "Character Set Code : " << ent->CharSetCode(i) << endl;

          S << "Slant Angle : "    << ent->SlantAngle(i) << "  ";
          S << "Rotation Angle : " << ent->RotationAngle(i) << "  ";
          S << "Mirror Flag : "    << ent->MirrorFlag(i) << "  ";
          S << "Rotate Flag : "    << ent->RotateFlag(i) << endl;
          S << "Start Point : ";
          IGESData_DumpXYZL(S,level, ent->StartPoint(i), ent->Location());
          S << "Text : ";
          IGESData_DumpString(S,ent->Text(i));
          S << endl;
	}
    }
  else S << endl;
}
