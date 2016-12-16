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
#include <gp_XYZ.hxx>
#include <IGESDraw_ConnectPoint.hxx>
#include <IGESDraw_NetworkSubfigure.hxx>
#include <IGESDraw_NetworkSubfigureDef.hxx>
#include <IGESGraph_TextDisplayTemplate.hxx>
#include <TCollection_HAsciiString.hxx>
#include <IGESFile_Reader.hxx>
#include <IGESData_IGESWriter.hxx>
#include <Interface_EntityIterator.hxx>
#include <IGESData_DirChecker.hxx>
#include <Message_Messenger.hxx>
#include <IGESData_IGESDumper.hxx>
#include <IGESData_Dump.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESDraw_NetworkSubfigure,IGESData_IGESEntity)

gp_XYZ IGESDraw_NetworkSubfigure::TransformedTranslation () const
{
  gp_XYZ TempXYZ = theTranslation;
  if (HasTransf()) Location().Transforms(TempXYZ);
  return ( TempXYZ );
}

Standard_Integer IGESDraw_NetworkSubfigure::NbConnectPoints () const
{
  return (theConnectPoints.IsNull() ? 0 : theConnectPoints->Length() );
}

const Handle(IGESDraw_ConnectPoint) & IGESDraw_NetworkSubfigure::ConnectPoint (const Standard_Integer Index) const
{
  return theConnectPoints->Value(Index);
}

void IGESDraw_NetworkSubfigure::OwnRead (IGESFile_Reader &PR)
{
  PR.ReadPointer(theSubfigureDefinition,"Instance of NetworkSubfigureDef");
  PR.ReadXYZ(theTranslation,"Translation data");

  Standard_Real scaleX = 1.0; // Default Value
  PR.ReadReal(scaleX,"Scale factors(X)");

  Standard_Real scaleY = scaleX; // Default Value
  PR.ReadReal(scaleY,"Scale factors(Y)");

  Standard_Real scaleZ = scaleX; // Default Value
  PR.ReadReal(scaleZ,"Scale factors(Z)");

  theScaleFactor.SetCoord(scaleX, scaleY, scaleZ);

  theTypeFlag = 0; // Default Value
  PR.ReadInteger(theTypeFlag,"Type flag");

  if (PR.ReadText(theDesignator,"Primary reference designator") == IGESFile_Reader::ParamEmpty)
    PR.AddWarning("Primary reference designator : Null definition");

  PR.ReadPointer(theDesignatorTemplate,"Instance of TextDisplayTemplate",Standard_True);

  Standard_Integer nbval = 0;
  PR.ReadInteger(nbval,"Count of Connect Points");
  if (nbval > 0)
  {
    // Reading connectPoints(HArray1OfConnectPoint)
    theConnectPoints = new IGESDraw_HArray1OfConnectPoint(1, nbval);
    for (Standard_Integer i = 1; i <= nbval; i++)
      PR.ReadPointer(theConnectPoints->ChangeValue(i),"ConnectPoint entity",Standard_True);
  }
  else if (nbval < 0)
    PR.AddFail ("Count of Connect point entities : Less than Zero");
}

void IGESDraw_NetworkSubfigure::OwnWrite (IGESData_IGESWriter &IW) const
{
  IW.Send( theSubfigureDefinition );
  IW.Send( theTranslation.X() );
  IW.Send( theTranslation.Y() );
  IW.Send( theTranslation.Z() );
  IW.Send( theScaleFactor.X() );
  IW.Send( theScaleFactor.Y() );
  IW.Send( theScaleFactor.Z() );
  IW.Send( theTypeFlag );
  IW.Send( theDesignator );
  IW.Send( theDesignatorTemplate );

  const Standard_Integer Up = NbConnectPoints();
  IW.Send( Up );
  for ( Standard_Integer i = 1; i <= Up; i++)
    IW.Send( ConnectPoint(i) );
}

void IGESDraw_NetworkSubfigure::OwnShared (Interface_EntityIterator &iter) const
{
  iter.GetOneItem( theSubfigureDefinition );
  iter.GetOneItem( theDesignatorTemplate );
  const Standard_Integer Up = NbConnectPoints();
  for ( Standard_Integer i = 1; i <= Up; i++)
    iter.GetOneItem( ConnectPoint(i) );
}

IGESData_DirChecker IGESDraw_NetworkSubfigure::DirChecker () const
{ 
  IGESData_DirChecker DC (420, 0);
  DC.Structure(IGESData_DefVoid);
  DC.LineFont(IGESData_DefAny);
  DC.LineWeight(IGESData_DefValue);
  DC.Color(IGESData_DefAny);
  return DC;
}

void IGESDraw_NetworkSubfigure::OwnCheck (const Interface_ShareTool &, Handle(Interface_Check) &ach) const
{
  if ( theTypeFlag != 0 &&
       theTypeFlag != 1 &&
       theTypeFlag != 2 )
    ach->AddFail("Type Flag : Value != 0/1/2");
  if (NbConnectPoints() != SubfigureDefinition()->NbPointEntities())
    ach->AddFail("Count of associated Connect Points inconsistent with Definition");
  if (theDesignator.IsNull())
    ach->AddFail("Primary Reference Designator : not defined");
}

void IGESDraw_NetworkSubfigure::OwnDump (const IGESData_IGESDumper &dumper, const Handle(Message_Messenger) &S, const Standard_Integer level) const
{
  const Standard_Integer sublevel = (level <= 4) ? 0 : 1;
  S << "IGESDraw_NetworkSubfigure" << endl;
  S << "Network Subfigure Definition Entity : ";
  dumper.Dump(theSubfigureDefinition,S,sublevel);
  S << endl << "Translation Data : ";
  IGESData_DumpXYZL(S,level,theTranslation,Location());
  S << endl << "Scale Factors    : ";
  IGESData_DumpXYZ(S,theScaleFactor);
  S << endl << "Type Flag : " << theTypeFlag << endl;
  S << "Primary Reference Designator : ";
  IGESData_DumpString(S,theDesignator);
  S << endl << "Text Display Template Entity : ";
  dumper.Dump(theDesignatorTemplate,S,sublevel);
  S << endl << "Connect Points  : ";
  IGESData_DumpEntities(S,dumper,level,1,NbConnectPoints(),ConnectPoint);
  S << endl;
}
