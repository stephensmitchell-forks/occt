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

#include <IGESData_DirChecker.hxx>
#include <IGESData_Dump.hxx>
#include <IGESData_IGESDumper.hxx>
#include <IGESData_IGESReaderData.hxx>
#include <IGESData_IGESWriter.hxx>
#include <IGESData_ParamCursor.hxx>
#include <IGESData_ParamReader.hxx>
#include <IGESGeom_SplineCurve.hxx>
#include <IGESGeom_ToolSplineCurve.hxx>
#include <Interface_Check.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_Macros.hxx>
#include <Interface_ShareTool.hxx>
#include <Message_Messenger.hxx>
#include <Message_Msg.hxx>
#include <Standard_DomainError.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <TColStd_HArray2OfReal.hxx>

#include <stdio.h>

void IGESGeom_ToolSplineCurve::ReadOwnParams
  (const Handle(IGESGeom_SplineCurve)& ent,
   const Handle(IGESData_IGESReaderData)& /* IR */, IGESData_ParamReader& PR) const
{
  Standard_Integer nbSegments;
  Standard_Integer aType, aDegree, nbDimensions;;
  Handle(TColStd_HArray1OfReal) allBreakPoints;
  Handle(TColStd_HArray2OfReal) allXPolynomials;
  Handle(TColStd_HArray2OfReal) allYPolynomials;
  Handle(TColStd_HArray2OfReal) allZPolynomials;
  Handle(TColStd_HArray1OfReal) allXvalues = new TColStd_HArray1OfReal(1, 4);
  Handle(TColStd_HArray1OfReal) allYvalues = new TColStd_HArray1OfReal(1, 4);
  Handle(TColStd_HArray1OfReal) allZvalues = new TColStd_HArray1OfReal(1, 4);

  if (!PR.ReadInteger(aType)){
    Message_Msg Msg91("XSTEP_91");
    PR.SendFail(Msg91);
  }
  if (!PR.ReadInteger(aDegree)){
    Message_Msg Msg92("XSTEP_92");
    PR.SendFail(Msg92);
  }
  if (!PR.ReadInteger(nbDimensions)){
    Message_Msg Msg93("XSTEP_93");
    PR.SendFail(Msg93);
  }

  if (PR.ReadInteger(nbSegments)) {
    if (nbSegments <= 0){
      Message_Msg Msg94("XSTEP_94");
      PR.SendFail(Msg94);
    }
    else {
      allXPolynomials = new TColStd_HArray2OfReal (1, nbSegments, 1, 4);
      allYPolynomials = new TColStd_HArray2OfReal (1, nbSegments, 1, 4);
      allZPolynomials = new TColStd_HArray2OfReal (1, nbSegments, 1, 4);
      allBreakPoints  = new TColStd_HArray1OfReal (1, (nbSegments + 1));
    }
    Message_Msg Msg95("XSTEP_95");
    PR.ReadReals(PR.CurrentList(nbSegments + 1), Msg95, allBreakPoints); //szv#4:S4163:12Mar99 `st=` not needed
  }
  else{
    Message_Msg Msg94("XSTEP_94");
    PR.SendFail(Msg94);
  }

  Handle(TColStd_HArray1OfReal) temp;
  temp = new TColStd_HArray1OfReal(1,4);

  if (! allXPolynomials.IsNull()) {
    Standard_Integer I;
    for (I = 1; I <= nbSegments; I++)
      {
	if (PR.ReadReals(PR.CurrentList(4),"X-Coordinate Polynomial",temp)) {
	  Standard_Integer J;
	  for (J = 1; J <= 4; J++)
	    allXPolynomials->SetValue(I, J, temp->Value(J));
	}

	if (PR.ReadReals(PR.CurrentList(4),"Y-Coordinate Polynomial",temp)) {
	  Standard_Integer J;
	  for (J = 1; J <= 4; J++)
	    allYPolynomials->SetValue(I, J, temp->Value(J));
	}

	if (PR.ReadReals(PR.CurrentList(4),"Z-Coordinate Polynomial",temp)) {
	  Standard_Integer J;
	  for (J = 1; J <= 4; J++)
	    allZPolynomials->SetValue(I, J, temp->Value(J));
	}
      }
  }

  if (PR.ReadReals(PR.CurrentList(4), "TerminatePoint X-Values", temp)) {
    Standard_Integer J;
    for (J = 1; J <= 4; J++)
      allXvalues->SetValue(J, temp->Value(J));
  }

  if (PR.ReadReals(PR.CurrentList(4), "TerminatePoint Y-Values", temp)) {
    Standard_Integer J;
    for (J = 1; J <= 4; J++)
      allYvalues->SetValue(J, temp->Value(J));
  }

  if (PR.ReadReals(PR.CurrentList(4), "TerminatePoint Z-Values", temp)) {
    Standard_Integer J;
    for (J = 1; J <= 4; J++)
      allZvalues->SetValue(J, temp->Value(J));
  }

  //sln 28.09.2001, BUC61004, If the condition is true function ent->Init is not called in order to avoid exception
  if(allBreakPoints.IsNull() ||  allXPolynomials.IsNull() || allYPolynomials.IsNull() || allZPolynomials.IsNull())
    return;
  else
    {
      DirChecker(ent).CheckTypeAndForm(PR.CCheck(),ent);
      ent->Init
        (aType, aDegree, nbDimensions, allBreakPoints,
         allXPolynomials, allYPolynomials, allZPolynomials,
         allXvalues, allYvalues, allZvalues);
    }
}

void IGESGeom_ToolSplineCurve::WriteOwnParams
  (const Handle(IGESGeom_SplineCurve)& ent, IGESData_IGESWriter& IW)  const
{
  IW.Send(ent->SplineType());
  IW.Send(ent->Degree());
  IW.Send(ent->NbDimensions());
  Standard_Integer nbSegments = ent->NbSegments();
  IW.Send(nbSegments);

  Standard_Integer I;
  for (I = 1; I <= nbSegments + 1; I++)
    IW.Send(ent->BreakPoint(I));
  Standard_Real AX,BX,CX,DX, AY,BY,CY,DY, AZ,BZ,CZ,DZ;
  for (I = 1; I <= nbSegments; I++) {
//no need to declare (hides the same name in an outer scope)
//     Standard_Real AX,BX,CX,DX, AY,BY,CY,DY, AZ,BZ,CZ,DZ;
    ent->XCoordPolynomial(I,AX,BX,CX,DX);
    ent->YCoordPolynomial(I,AY,BY,CY,DY);
    ent->ZCoordPolynomial(I,AZ,BZ,CZ,DZ);
    IW.Send(AX);  IW.Send(BX);  IW.Send(CX);  IW.Send(DX);
    IW.Send(AY);  IW.Send(BY);  IW.Send(CY);  IW.Send(DY);
    IW.Send(AZ);  IW.Send(BZ);  IW.Send(CZ);  IW.Send(DZ);
  }
  ent->XValues(AX,BX,CX,DX);
  ent->YValues(AY,BY,CY,DY);
  ent->ZValues(AZ,BZ,CZ,DZ);
  IW.Send(AX);  IW.Send(BX);  IW.Send(CX);  IW.Send(DX);
  IW.Send(AY);  IW.Send(BY);  IW.Send(CY);  IW.Send(DY);
  IW.Send(AZ);  IW.Send(BZ);  IW.Send(CZ);  IW.Send(DZ);
}

IGESData_DirChecker IGESGeom_ToolSplineCurve::DirChecker
  (const Handle(IGESGeom_SplineCurve)& /* ent */ )   const
{
  IGESData_DirChecker DC(112, 0);
  DC.Structure(IGESData_DefVoid);
  DC.LineFont(IGESData_DefAny);
//  DC.LineWeight(IGESData_DefValue);
  DC.Color(IGESData_DefAny);
  DC.HierarchyStatusIgnored();

  return DC;
}

void IGESGeom_ToolSplineCurve::OwnCheck
  (const Handle(IGESGeom_SplineCurve)& ent,
   const Interface_ShareTool& , Handle(Interface_Check)& ach)  const
{
  // MGE 29/07/98
  // Building of messages

  if (ent->SplineType() < 1 || ent->SplineType() > 6){
    Message_Msg Msg91("XSTEP_91");
    ach->SendFail(Msg91);
  }
  if (ent->NbDimensions() == 2) {
    Standard_Integer nbSegments = ent->NbSegments();
    Standard_Real AZ,BZ,CZ,DZ;
    Message_Msg Msg96("XSTEP_96");
    for (Standard_Integer I = 1; I <= nbSegments; I ++) {
      ent->ZCoordPolynomial(I,AZ,BZ,CZ,DZ);
      if (BZ != 0 || CZ != 0 || DZ != 0) {
	ach->SendFail(Msg96);
      }
    }
  }
  else if (ent->NbDimensions() != 3){
    Message_Msg Msg93("XSTEP_93");
    ach->SendFail(Msg93);
  }
}

void IGESGeom_ToolSplineCurve::OwnDump
  (const Handle(IGESGeom_SplineCurve)& ent, const IGESData_IGESDumper& /* dumper */,
   const Handle(Message_Messenger)& S, const Standard_Integer level)  const
{
  S << "IGESGeom_SplineCurve" << endl;

  Standard_Integer nbSegments = ent->NbSegments();
  S << "Spline Type          : " << ent->SplineType()    << "  ";
  switch (ent->SplineType())
    {
    case 1 : S << "(Linear)"; break;
    case 2 : S << "(Quadratic)"; break;
    case 3 : S << "(Cubic)"; break;
    case 4 : S << "(Wilson-Fowler)"; break;
    case 5 : S << "(Modified Wison-Fowler)"; break;
    case 6 : S << "(B-Spline)"; break;
      default : S << "(Invalid value)"; break;
    }
  S << endl;
  S << "Degree Of Continuity : " << ent->Degree()       << endl;
  S << "Number Of Dimensions : " << ent->NbDimensions() << endl;
  S << "Number Of Segments   : " << ent->NbSegments()   << endl;
  S << "Segment Break Points : ";
  IGESData_DumpVals(S ,level,1, nbSegments+1,ent->BreakPoint);
  if (level <= 4) {
    S << " [ also ask level > 4 for X-Y-Z Polynomials ]" << endl;
    return;
  }
  S << "  --  Polynomial  Values  --" << endl;
  Standard_Real AX,BX,CX,DX, AY,BY,CY,DY, AZ,BZ,CZ,DZ;
  for (Standard_Integer I = 1; I <= nbSegments; I++) {
//no need to declare (hides the same name in an outer scope)
//  Standard_Real AX,BX,CX,DX, AY,BY,CY,DY, AZ,BZ,CZ,DZ;
    ent->XCoordPolynomial(I,AX,BX,CX,DX);
    ent->YCoordPolynomial(I,AY,BY,CY,DY);
    ent->ZCoordPolynomial(I,AZ,BZ,CZ,DZ);
    S << "Segment "<<I<<" :	    X		   Y		Z"<<endl;
    S << " A ...	"<<AX<<"	"<<AY<<"	"<<AZ<<endl;
    S << " B ...	"<<BX<<"	"<<BY<<"	"<<BZ<<endl;
    S << " C ...	"<<CX<<"	"<<CY<<"	"<<CZ<<endl;
    S << " D ...	"<<DX<<"	"<<DY<<"	"<<DZ<<endl;
  }
  ent->XValues(AX,BX,CX,DX);
  ent->YValues(AY,BY,CY,DY);
  ent->ZValues(AZ,BZ,CZ,DZ);
  S << "Terminate Point :	    X		   Y		Z"<<endl;
  S << " Value        	"<<AX<<"	"<<AY<<"	"<<AZ<<endl;
  S << " 1st Derivative	"<<BX<<"	"<<BY<<"	"<<BZ<<endl;
  S << " 2nd Der./2!   	"<<CX<<"	"<<CY<<"	"<<CZ<<endl;
  S << " 3rd Der./3!   	"<<DX<<"	"<<DY<<"	"<<DZ<<endl;
}
