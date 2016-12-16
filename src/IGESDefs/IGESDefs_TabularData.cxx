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
// UNFINISHED
// The last field (myDependentValues) not resolved. Queried to mdtv

#include <IGESBasic_HArray1OfHArray1OfReal.hxx>
#include <IGESDefs_TabularData.hxx>
#include <Standard_DimensionMismatch.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_Type.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <IGESFile_Reader.hxx>
#include <IGESData_IGESWriter.hxx>
#include <IGESData_DirChecker.hxx>
#include <Message_Messenger.hxx>
#include <IGESData_IGESDumper.hxx>
#include <IGESData_Dump.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESDefs_TabularData,IGESData_IGESEntity)

Standard_Integer IGESDefs_TabularData::NbDependents () const
{
  return myDependentValues->Length();
}

Standard_Integer IGESDefs_TabularData::NbIndependents () const
{
  return myTypeOfIndependentVariables->Length();
}

Standard_Integer IGESDefs_TabularData::TypeOfIndependents (const Standard_Integer num) const
{
  return myTypeOfIndependentVariables->Value(num);
}

Standard_Integer IGESDefs_TabularData::NbValues (const Standard_Integer num) const
{
  return myNbValues->Value(num);
}

Standard_Real IGESDefs_TabularData::IndependentValue (const Standard_Integer variablenum, const Standard_Integer valuenum) const
{
  return (myIndependentValues->Value(variablenum))->Value(valuenum);
}

Handle(TColStd_HArray1OfReal) IGESDefs_TabularData::DependentValues (const Standard_Integer num) const
{
  return myDependentValues->Value(num);
}

// UNFINISHED
// Array limits not sure.
Standard_Real IGESDefs_TabularData::DependentValue (const Standard_Integer /*variablenum*/, 
                                                    const Standard_Integer /*valuenum*/) const
{
  Standard_Real val = 0.;
  return val;
}

void IGESDefs_TabularData::OwnRead (IGESFile_Reader &theReader)
{
  theReader.ReadInteger(myNbPropertyValues,"Number of Property values");
  theReader.ReadInteger(myPropertyType,"Property type");

  Standard_Integer nbDeps = 0;
  theReader.ReadInteger(nbDeps,"No. of dependent variables");
  if (nbDeps > 0)
    myDependentValues = new IGESBasic_HArray1OfHArray1OfReal(1, nbDeps);

  Standard_Integer i, nbIndeps = 0;
  theReader.ReadInteger(nbIndeps,"No. of Independent variables");
  if (nbIndeps > 0)
  {
    myTypeOfIndependentVariables = new TColStd_HArray1OfInteger(1, nbIndeps);
    myNbValues = new TColStd_HArray1OfInteger(1, nbIndeps);
    myIndependentValues = new IGESBasic_HArray1OfHArray1OfReal(1, nbIndeps);

    theReader.ReadInteger(myTypeOfIndependentVariables->ChangeFirst(),nbIndeps,"Type of independent variables");
    theReader.ReadInteger(myNbValues->ChangeFirst(),nbIndeps,"No. of values of independent variables");

    for (i=1; i<=nbIndeps; i++)
    {
      const Standard_Integer nb = myNbValues->Value(i);
      if (nb > 0)
      {
        Handle(TColStd_HArray1OfReal) tarr = new TColStd_HArray1OfReal(1, nb);
        for (Standard_Integer j=1; j<= nb; j++)
          theReader.ReadReal(tarr->ChangeValue(j),"Value of independent variable");
        myIndependentValues->SetValue(i, tarr);
      }
    }
  }

// ??  for (i=1; i<=nbDeps; i++) {  }
//  Dependents : definition pas limpide, on accumule tout sur un seul
//  HArray1OfReal, mis en 1re position du HArray1OfHArray1OfReal
//  On y met tous les flottants qui restent
  while (theReader.HasData())
    theReader.ReadAny();
  theReader.AddWarning("Don't know exactly how to read dependant values ...");
}

void IGESDefs_TabularData::OwnWrite (IGESData_IGESWriter& IW) const
{
  Standard_Integer i, nbIndeps = NbIndependents();
  Standard_Integer j, nbDeps = NbDependents();
  IW.Send(NbPropertyValues());
  IW.Send(PropertyType());
  IW.Send(nbDeps);
  IW.Send(nbIndeps);
  for (i=1; i<=nbIndeps; i++)
    IW.Send(TypeOfIndependents(i));
  for (i=1; i<=nbIndeps; i++)
    IW.Send(NbValues(i));
  for (i=1; i<=nbIndeps; i++)
    for (j=1; j<=NbValues(i); j++)
      IW.Send(IndependentValue(i,j));
  // UNFINISHED
  if (nbDeps == 0) return;
  Handle(TColStd_HArray1OfReal) deps = DependentValues(1);
  for (i = 1; i <= deps->Length(); i ++) IW.Send(deps->Value(i));
}

IGESData_DirChecker IGESDefs_TabularData::DirChecker () const
{
  IGESData_DirChecker DC(406, 11);
  DC.Structure  (IGESData_DefVoid);
  DC.LineFont   (IGESData_DefVoid);
  DC.LineWeight (IGESData_DefVoid);
  DC.Color      (IGESData_DefVoid);

  DC.BlankStatusIgnored ();
  DC.UseFlagIgnored ();
  DC.HierarchyStatusIgnored ();
  return DC;
}

void IGESDefs_TabularData::OwnDump (const IGESData_IGESDumper &, const Handle(Message_Messenger) &S, const Standard_Integer level) const
{
  Standard_Integer nbIndeps = NbIndependents();
  Standard_Integer nbDeps = NbDependents();

  S << "IGESDefs_TabularData" << endl;
  S << "No. of property values : " << NbPropertyValues() << endl;
  S << "Property type : " << PropertyType() << endl;
  S << "No. of Dependent variables    : " << nbDeps << endl;
  S << "No. of Independent variables  : " << nbIndeps << endl;
  S << "Type of independent variables : ";
  IGESData_DumpVals(S,level,1,nbIndeps,TypeOfIndependents);
  S << endl << "Number of values of independent variables : ";
  IGESData_DumpVals(S,level,1,nbIndeps,NbValues);
// ?? JAGGED ??
  S << endl << "Values of the independent variable : ";
  if (level < 5) S << " [ask level > 4]";
  else {
    for (Standard_Integer ind = 1; ind <= nbIndeps; ind ++) {
      S << endl << "[" << ind << "]:";
      Standard_Integer nbi = NbValues(ind);
      for (Standard_Integer iv = 1; iv <= nbi; iv ++)
	S << " " << IndependentValue(ind,iv);
    }
  }
//  IGESData_DumpVals(S,level,1,nbIndeps,IndependentValue);
  S << endl << "Values of the dependent variable : ";
//  IGESData_DumpVals(S,level,1,nbDeps,DependentValue);
  S << "  TO BE DONE";
  S << endl;
}
