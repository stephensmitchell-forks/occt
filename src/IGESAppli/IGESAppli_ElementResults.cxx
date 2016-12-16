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
// UNFINISHED & UNSTARTED

#include <TColStd_HArray1OfInteger.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <IGESAppli_ElementResults.hxx>
#include <IGESAppli_FiniteElement.hxx>
#include <IGESBasic_HArray1OfHArray1OfInteger.hxx>
#include <IGESBasic_HArray1OfHArray1OfReal.hxx>
#include <IGESDimen_GeneralNote.hxx>
#include <Standard_DimensionMismatch.hxx>
#include <Standard_OutOfRange.hxx>
#include <Interface_EntityIterator.hxx>
#include <IGESFile_Reader.hxx>
#include <IGESData_IGESWriter.hxx>
#include <IGESData_IGESDumper.hxx>
#include <IGESData_DirChecker.hxx>
#include <IGESData_Dump.hxx>
#include <Message_Messenger.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESAppli_ElementResults,IGESData_IGESEntity)

Standard_Integer IGESAppli_ElementResults::NbElements () const
{
  return myElements->Length();
}

Standard_Integer IGESAppli_ElementResults::ElementIdentifier (const Standard_Integer Index) const
{
  return myElementIdentifiers->Value(Index);
}

const Handle(IGESAppli_FiniteElement) & IGESAppli_ElementResults::Element (const Standard_Integer Index) const
{
  return myElements->Value(Index);
}

Standard_Integer IGESAppli_ElementResults::ElementTopologyType (const Standard_Integer Index) const
{
  return myElementTopologyTypes->Value(Index);
}

Standard_Integer IGESAppli_ElementResults::NbLayers (const Standard_Integer Index) const
{
  return myNbLayers->Value(Index);
}

Standard_Integer IGESAppli_ElementResults::DataLayerFlag (const Standard_Integer Index) const
{
  return myDataLayerFlags->Value(Index);
}

Standard_Integer IGESAppli_ElementResults::NbResultDataLocs (const Standard_Integer Index) const
{
  return myNbResultDataLocs->Value(Index);
}

//  ?? VERIFIER

Standard_Integer IGESAppli_ElementResults::ResultDataLoc (const Standard_Integer NElem, const Standard_Integer NLoc) const
{
  return myResultDataLocs->Value(NElem)->Value(NLoc);
}

Standard_Integer IGESAppli_ElementResults::NbResults (const Standard_Integer Index) const
{
  return myResultData->Value(Index)->Length();
}

Standard_Real IGESAppli_ElementResults::ResultData (const Standard_Integer NElem, const Standard_Integer num) const
{
  return myResultData->Value(NElem)->Value(num);
}

Standard_Integer IGESAppli_ElementResults::ResultRank
  (const Standard_Integer NElem, const Standard_Integer NVal,
   const Standard_Integer NLay, const Standard_Integer NLoc) const
{
  Standard_Integer num = NVal + myNbResultValues *
    (NLay + myNbLayers->Value(NElem) * NLoc);
  return num;
}

Standard_Real IGESAppli_ElementResults::ResultData
  (const Standard_Integer NElem, const Standard_Integer NVal,
   const Standard_Integer NLay, const Standard_Integer NLoc) const
{
  return myResultData->Value(NElem)->Value(ResultRank(NElem,NVal,NLay,NLoc));
}

Handle(TColStd_HArray1OfReal) IGESAppli_ElementResults::ResultList (const Standard_Integer NElem) const
{
  return myResultData->Value(NElem);
}

void IGESAppli_ElementResults::OwnRead (IGESFile_Reader &theReader)
{
  Standard_Integer num = 0;

  theReader.ReadPointer(myNote,"General Note");
  theReader.ReadInteger(mySubcaseNumber,"Subcase Number");
  theReader.ReadReal(myTime,"Time");
  theReader.ReadInteger(myNbResultValues,"Number of Result Values");
  theReader.ReadInteger(myResultReportFlag,"Result Report Flag");
  theReader.ReadInteger(num,"Number of Finite Elements");

  if (num > 0)
  {
    myElementIdentifiers   = new TColStd_HArray1OfInteger(1, num);
    myElements             = new IGESAppli_HArray1OfFiniteElement(1, num);
    myElementTopologyTypes = new TColStd_HArray1OfInteger(1, num);
    myNbLayers             = new TColStd_HArray1OfInteger(1, num);
    myDataLayerFlags       = new TColStd_HArray1OfInteger(1, num);
    myNbResultDataLocs     = new TColStd_HArray1OfInteger(1, num);
    myResultDataLocs       = new IGESBasic_HArray1OfHArray1OfInteger(1, num);
    myResultData           = new IGESBasic_HArray1OfHArray1OfReal(1, num);

    for (Standard_Integer i = 1; i <= num; i++)
    {
      Standard_Integer nl = 0, nrl = 0, numv = 0;
	  theReader.ReadInteger(myElementIdentifiers->ChangeValue(i),"FEM Element Identifier");
      theReader.ReadPointer(myElements->ChangeValue(i),"FEM Element Entity");
      theReader.ReadInteger(myElementTopologyTypes->ChangeValue(i),"FEM Element Topology Type");
      if (theReader.ReadInteger(nl,"Nb. of layers"))
        myNbLayers->SetValue(i,nl);
      theReader.ReadInteger(myDataLayerFlags->ChangeValue(i),"Data Layer Flag");

      if (theReader.ReadInteger(nrl,"Nb. of result data locations"))
        myNbResultDataLocs->SetValue(i,nrl);

      if (nrl > 0) {
        Handle(TColStd_HArray1OfInteger) rdrl = new TColStd_HArray1OfInteger(1,nrl);
        if (theReader.ReadInteger(rdrl->ChangeFirst(),nrl,"Result data locations"))
          myResultDataLocs->SetValue(i,rdrl);
	  }

      theReader.ReadInteger(numv,"Nb. of result data locations");
      if (numv > 0) {
        Handle(TColStd_HArray1OfReal) vres = new TColStd_HArray1OfReal(1,numv);
		if (theReader.ReadReal(vres->ChangeFirst(),numv,"Result Data"))
          myResultData->SetValue(i,vres);
      }
    }
  }
  else theReader.AddFail("Number of Finite Elements: Not Positive");
}

void IGESAppli_ElementResults::OwnWrite (IGESData_IGESWriter &IW) const
{
  const Standard_Integer num = NbElements();
  IW.Send(myNote);
  IW.Send(mySubcaseNumber);
  IW.Send(myTime);
  IW.Send(myNbResultValues);
  IW.Send(myResultReportFlag);
  IW.Send(num);
  Standard_Integer i,j;
  for ( i = 1; i <= num; i++ ) {
    const Standard_Integer nl   = NbLayers(i);
    const Standard_Integer nrl  = NbResultDataLocs(i);
    const Standard_Integer numv = NbResults(i);
    IW.Send(ElementIdentifier(i));
    IW.Send(Element(i));
    IW.Send(ElementTopologyType(i));
    IW.Send(nl);
    IW.Send(DataLayerFlag(i));
    IW.Send(nrl);
    for (j = 1; j <= nrl;  j ++) IW.Send(ResultDataLoc(i,j));
    IW.Send(numv);
    for (j = 1; j <= numv; j ++) IW.Send(ResultData(i,j));
  }
}

void IGESAppli_ElementResults::OwnShared(Interface_EntityIterator &theIter) const
{
  theIter.GetOneItem(Note());
  const Standard_Integer num = NbElements();
  for ( Standard_Integer i = 1; i <= num; i++ )
    theIter.GetOneItem(Element(i));
}

IGESData_DirChecker IGESAppli_ElementResults::DirChecker () const
{
  IGESData_DirChecker DC(148, 0, 34);
  DC.Structure(IGESData_DefVoid);
  DC.GraphicsIgnored();
  DC.LineFont(IGESData_DefVoid);
  DC.LineWeight(IGESData_DefVoid);
  DC.Color(IGESData_DefAny);
  DC.BlankStatusIgnored();
  DC.UseFlagRequired(3);
  DC.HierarchyStatusIgnored();
  return DC;
}

void IGESAppli_ElementResults::OwnCheck (const Interface_ShareTool &, const Handle(Interface_Check) &theCheck) const
{
  if (myResultReportFlag < 0 || myResultReportFlag > 3)
    theCheck->AddFail("Result Report Flag not in [0-3]");

  Standard_Boolean OK = Standard_True;
  switch (myForm) {
    case  0 : if (myNbResultValues <  0) OK = Standard_False;  break;
    case  1 : if (myNbResultValues != 1) OK = Standard_False;  break;
    case  2 : if (myNbResultValues != 1) OK = Standard_False;  break;
    case  3 : if (myNbResultValues != 3) OK = Standard_False;  break;
    case  4 : if (myNbResultValues != 6) OK = Standard_False;  break;
    case  5 : if (myNbResultValues != 3) OK = Standard_False;  break;
    case  6 : if (myNbResultValues != 3) OK = Standard_False;  break;
    case  7 : if (myNbResultValues != 3) OK = Standard_False;  break;
    case  8 : if (myNbResultValues != 3) OK = Standard_False;  break;
    case  9 : if (myNbResultValues != 3) OK = Standard_False;  break;
    case 10 : if (myNbResultValues != 1) OK = Standard_False;  break;
    case 11 : if (myNbResultValues != 1) OK = Standard_False;  break;
    case 12 : if (myNbResultValues != 3) OK = Standard_False;  break;
    case 13 : if (myNbResultValues != 1) OK = Standard_False;  break;
    case 14 : if (myNbResultValues != 1) OK = Standard_False;  break;
    case 15 : if (myNbResultValues != 3) OK = Standard_False;  break;
    case 16 : if (myNbResultValues != 1) OK = Standard_False;  break;
    case 17 : if (myNbResultValues != 3) OK = Standard_False;  break;
    case 18 : if (myNbResultValues != 3) OK = Standard_False;  break;
    case 19 : if (myNbResultValues != 3) OK = Standard_False;  break;
    case 20 : if (myNbResultValues != 3) OK = Standard_False;  break;
    case 21 : if (myNbResultValues != 3) OK = Standard_False;  break;
    case 22 : if (myNbResultValues != 3) OK = Standard_False;  break;
    case 23 : if (myNbResultValues != 6) OK = Standard_False;  break;
    case 24 : if (myNbResultValues != 6) OK = Standard_False;  break;
    case 25 : if (myNbResultValues != 6) OK = Standard_False;  break;
    case 26 : if (myNbResultValues != 6) OK = Standard_False;  break;
    case 27 : if (myNbResultValues != 6) OK = Standard_False;  break;
    case 28 : if (myNbResultValues != 6) OK = Standard_False;  break;
    case 29 : if (myNbResultValues != 9) OK = Standard_False;  break;
    case 30 : if (myNbResultValues != 9) OK = Standard_False;  break;
    case 31 : if (myNbResultValues != 9) OK = Standard_False;  break;
    case 32 : if (myNbResultValues != 9) OK = Standard_False;  break;
    case 33 : if (myNbResultValues != 9) OK = Standard_False;  break;
    case 34 : if (myNbResultValues != 9) OK = Standard_False;  break;
    default : theCheck->AddFail("Incorrect Form Number");    break;
  }
  if (!OK)
    theCheck->AddFail("Incorrect count of real values in array V for FEM node");

  const Standard_Integer ne = NbElements();
  for (Standard_Integer i = 1; i <= ne; i ++) {
    char mess[100];
    Standard_Integer dlf = DataLayerFlag(i);
    Standard_Integer nl  = NbLayers(i);
    Standard_Integer nrl = NbResultDataLocs(i);
    if (dlf < 0 || dlf > 4)
      theCheck->AddFail("One of the Data Layer Flags not in [0-4]");
    if (dlf < 4 && NbLayers(i) != 1) {
      sprintf(mess,"Nb. of Layers n0.%d not ONE while Data Layer Flag is in [0-3]",i);
      theCheck->AddFail(mess);
    }
    if (myResultReportFlag == 1 || myResultReportFlag == 2)
      if (nrl != 1 || ResultDataLoc(i,1) != 0) {
        sprintf(mess,"Result Data Locs n0.%d incorrect for Result Report = 1 or 2",i);
        theCheck->AddFail(mess);
      }
    if (NbResults(i) != (myNbResultValues*nl*nrl)) {
      sprintf(mess,"Nb. of results for Element n0.%d incorrect, should be %d",i,myNbResultValues*nl*nrl);
      theCheck->AddFail(mess);
    }
  }
}
  
void IGESAppli_ElementResults::OwnDump (const IGESData_IGESDumper &dumper, const Handle(Message_Messenger) &S, const Standard_Integer level) const
{
  Standard_Integer sublevel = (level > 4) ? 1 : 0;
  S << "IGESAppli_ElementResults" << endl;
  S << "General Note   : ";
  dumper.Dump(myNote,S,sublevel);
  S << endl;
  S << "Subcase Number : " << mySubcaseNumber << endl;
  S << "Time           : " << myTime << endl;
  S << "Number of Result Values : " << myNbResultValues << endl;
  S << "Result Report Flag  : " << myResultReportFlag << endl;
  S << "Element Identifiers : ";
  IGESData_DumpVals(S,level,1,NbElements(),ElementIdentifier);
  S << endl << "Elements : ";
  IGESData_DumpEntities(S,dumper,level,1,NbElements(),Element);
  S << endl << "Element Topology Types : ";
  IGESData_DumpVals(S,level,1,NbElements(),ElementTopologyType);
  S << endl << "Numbers of Layers      : ";
  IGESData_DumpVals(S,level,1,NbElements(),NbLayers);
  S << endl << "Data Layer Flags       : ";
  IGESData_DumpVals(S,level,1,NbElements(),DataLayerFlag);
  S << endl << "Numbers of Result Data Locations : ";
  IGESData_DumpVals(S,level,1,NbElements(),NbResultDataLocs);
  S << endl << "Result Data Locations : ";  S << " TO BE DONE  ";

//  ??  A VERIFIER DE PRES, pas du tout sur que ce soit bon
//      cf aussi Write et Copy
  if (level <= 4) S << " [ ask level > 4 for more, > 5 for complete ]" << endl;
  else {
    Standard_Integer i;// svv Jan 10 2000 : porting on DEC 
    for (i = 1; i <= NbElements(); i ++) {
      Standard_Integer nloc = NbResultDataLocs(i);
      S << " ["<<i<<":NbLoc="<<nloc<<"]:";
      for (Standard_Integer j = 1; j <= nloc; j ++)
        S << " " << ResultDataLoc (i,j);
      S << endl;
    }
    S << "Result Data : ";

    for (i = 1; i <= NbElements(); i ++) {
      Standard_Integer nres = NbResults(i);
      S << " ["<<i<<":NbRes="<<nres<<"]:";
      if (level <= 5) S << " [ ask level > 5 for complete Data ]" << endl;
      else {
        for (Standard_Integer j = 1; j <= nres; j ++)
          S << " " << ResultData(i,j);    // ?? is it all ?? UNFINISHED
      }
    }
  }
}
