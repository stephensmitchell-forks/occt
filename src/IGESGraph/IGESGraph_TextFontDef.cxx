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

#include <TColgp_HArray1OfXY.hxx>
#include <IGESBasic_HArray1OfHArray1OfInteger.hxx>
#include <IGESGraph_TextFontDef.hxx>
#include <TCollection_HAsciiString.hxx>
#include <IGESFile_Reader.hxx>
#include <IGESData_IGESWriter.hxx>
#include <Interface_EntityIterator.hxx>
#include <IGESData_DirChecker.hxx>
#include <Message_Messenger.hxx>
#include <IGESData_IGESDumper.hxx>
#include <IGESData_Dump.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESGraph_TextFontDef,IGESData_IGESEntity)

Standard_Integer IGESGraph_TextFontDef::NbCharacters () const
{
  return ( myASCIICodes->Length() );
}

Standard_Integer IGESGraph_TextFontDef::ASCIICode (const Standard_Integer Chnum) const
{
  return ( myASCIICodes->Value(Chnum) ); 
}

void IGESGraph_TextFontDef::NextCharOrigin (const Standard_Integer Chnum, Standard_Integer& NX, Standard_Integer& NY) const
{
  NX = myNextCharOriginX->Value(Chnum);
  NY = myNextCharOriginY->Value(Chnum);
}

Standard_Integer IGESGraph_TextFontDef::NbPenMotions (const Standard_Integer Chnum) const
{
  return ( myNbPenMotions->Value(Chnum) ); 
}

Standard_Boolean IGESGraph_TextFontDef::IsPenUp (const Standard_Integer Chnum, const Standard_Integer Motionnum) const
{
  Handle(TColStd_HArray1OfInteger) MotionArr = myPenMotions->Value(Chnum);
  Standard_Integer PenStatus =     MotionArr->Value(Motionnum);
  return ( PenStatus == 1 );
}

void IGESGraph_TextFontDef::NextPenPosition (const Standard_Integer Chnum, const Standard_Integer Motionnum, Standard_Integer& IX, Standard_Integer& IY) const
{
  IX = myPenMovesToX->Value(Chnum)->Value(Motionnum);
  IY = myPenMovesToY->Value(Chnum)->Value(Motionnum);
}

void IGESGraph_TextFontDef::OwnRead (IGESFile_Reader &PR)
{ 
  PR.ReadInteger(myFontCode,"Font Code");
  PR.ReadText(myFontName,"Font Name");

  mySupersededFontCode = 1;
  PR.ReadInteger(mySupersededFontCode,"No. of superseded font");
  if (mySupersededFontCode < 0)
  {
    PR.GetPointer(mySupersededFontEntity,-mySupersededFontCode);
  }

  PR.ReadInteger(myScale,"Grid units eqvt to one text height");

  // Reading nbval(Integer)
  Standard_Integer nbval = 0;
  PR.ReadInteger(nbval,"No. of characters in this defn");
  if (nbval > 0)
  {
    myASCIICodes = new TColStd_HArray1OfInteger(1, nbval);
    myNextCharOriginX = new TColStd_HArray1OfInteger(1, nbval);
    myNextCharOriginY = new TColStd_HArray1OfInteger(1, nbval);
    myNbPenMotions = new TColStd_HArray1OfInteger(1, nbval);
    myPenMotions   = new IGESBasic_HArray1OfHArray1OfInteger(1, nbval);
    myPenMovesToX  = new IGESBasic_HArray1OfHArray1OfInteger(1, nbval);
    myPenMovesToY  = new IGESBasic_HArray1OfHArray1OfInteger(1, nbval);
      
    for ( Standard_Integer i = 1; i <= nbval; i++ )
    {
      PR.ReadInteger(myASCIICodes->ChangeValue(i),"array ASCIICodes");
      PR.ReadInteger(myNextCharOriginX->ChangeValue(i),"array nextChar X");
      PR.ReadInteger(myNextCharOriginY->ChangeValue(i),"array nextChar Y");
	  
      Standard_Integer tempMotion = 0;
      PR.ReadInteger(tempMotion,"array penMotions");
      myNbPenMotions->SetValue(i, tempMotion);
      if (tempMotion > 0)
      {
        Handle(TColStd_HArray1OfInteger) intarray, xarray, yarray;

        intarray = new TColStd_HArray1OfInteger(1, tempMotion);
        xarray   = new TColStd_HArray1OfInteger(1, tempMotion);
        yarray   = new TColStd_HArray1OfInteger(1, tempMotion);

        for ( Standard_Integer j = 1; j <= tempMotion; j++ )
        {
          Standard_Integer tempFlag = 0;
          PR.ReadInteger(tempFlag,"array penFlags");
          intarray->SetValue(j, tempFlag);
          PR.ReadInteger(xarray->ChangeValue(j),"array movePenTo X");
          PR.ReadInteger(yarray->ChangeValue(j),"array movePenTo Y");
        }

        myPenMotions->SetValue(i, intarray);
        myPenMovesToX->SetValue(i, xarray);
        myPenMovesToY->SetValue(i, yarray);
      }
      else PR.AddFail("Count of Pen motions : Not Positive");
    }
  }
  else PR.AddFail ("Count of characters in this defn : Not Positive");
}

void IGESGraph_TextFontDef::OwnWrite (IGESData_IGESWriter &IW) const
{
  Standard_Integer IX,IY;
  IW.Send(myFontCode);
  IW.Send(myFontName);

  if (!mySupersededFontEntity.IsNull())
    IW.Send(mySupersededFontEntity, Standard_True);  // negative
  else
    IW.Send(mySupersededFontCode);

  IW.Send(myScale);

  const Standard_Integer Up = NbCharacters();
  IW.Send(Up);
  for ( Standard_Integer i = 1; i <= Up; i++ )
  {
    IW.Send( ASCIICode(i) );
    NextCharOrigin (i,IX,IY);
    IW.Send( IX );
    IW.Send( IY );
    IW.Send( NbPenMotions(i) );
    for ( Standard_Integer j = 1; j <= NbPenMotions(i); j ++)
    {
      IW.SendBoolean( IsPenUp(i,j) );
      NextPenPosition (i,j, IX,IY);
      IW.Send( IX );
      IW.Send( IY );
    }
  }
} 
 
void IGESGraph_TextFontDef::OwnShared (Interface_EntityIterator &theIter) const
{
  if (!mySupersededFontEntity.IsNull())
    theIter.GetOneItem( mySupersededFontEntity );
}

IGESData_DirChecker IGESGraph_TextFontDef::DirChecker () const
{ 
  IGESData_DirChecker DC (310, 0);
  DC.Structure(IGESData_DefVoid);
  DC.LineFont(IGESData_DefVoid);
  DC.LineWeight(IGESData_DefVoid);
  DC.Color(IGESData_DefVoid);
  DC.BlankStatusIgnored();
  DC.SubordinateStatusRequired(0);
  DC.UseFlagRequired(2);
  DC.HierarchyStatusIgnored();
  return DC;
}

void IGESGraph_TextFontDef::OwnDump (const IGESData_IGESDumper &dumper, const Handle(Message_Messenger) &S, const Standard_Integer level) const
{
  Standard_Integer nbchars  = NbCharacters();

  S << "IGESGraph_TextFontDef" << endl;

  S << "Font Code : " << myFontCode << endl;
  S << "Font Name : ";
  IGESData_DumpString(S,myFontName);
  S << endl;
  if (!mySupersededFontEntity.IsNull()) {
    S << "Text Definition Entity : ";
    const Standard_Integer sublevel = (level <= 4) ? 0 : 1;
    dumper.Dump(mySupersededFontEntity,S,sublevel);
  }
  else  S << "Superseding Font Number : " << mySupersededFontCode;
  S << endl; 
  S << "No. of Grid Units eqvt to 1 Text Height : " << myScale << endl;
  S << "ASCII Codes                              : " << endl
    << "Grid Locations of next character origins : " << endl
    << "Pen Motions                              : " << endl
    << "Pen Positions                            : " << endl
    << "Grid Locations the pen moves to          : ";
  S << "Count = "      << nbchars << endl;
  IGESData_DumpVals(S,-level,1,nbchars,ASCIICode);
  S << endl;
  if (level > 4)
  {
    Handle(TColgp_HArray1OfXY) arrXY;
    Standard_Integer I, J, nbmotions;
    for (I = 1; I <= nbchars; I++)
    {
      Standard_Integer IX,IY;
      S << "[" << I << "]: ";
      S << "ASCII Code : " << ASCIICode(I) << endl;
      S << "Grid Location of next character's origin : ";
      NextCharOrigin(I,IX,IY);
      S << "X=" << IX << " Y=" << IY;
      nbmotions = NbPenMotions(I);
      S << "  No. of Pen Motions : " << nbmotions;
      if (level <= 5) S << " [ ask level > 5 for Details ]" << endl;
      else {
        S << endl;
        for (J = 1; J <= nbmotions; J++)
        {
          S << "Pen up(1) / down(0) flag : " << (Standard_Integer)IsPenUp(I,J);
          S << " Next Pen Position : ";
          NextPenPosition(I,J, IX,IY);
          S << " X="<<IX<<" Y="<<IY;
          S << endl;
        }
      }
    }
  }
  S << endl;
}
