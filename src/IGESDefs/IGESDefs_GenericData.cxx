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

#include <IGESData_HArray1OfIGESEntity.hxx>
#include <IGESData_IGESEntity.hxx>
#include <IGESDefs_GenericData.hxx>
#include <Interface_HArray1OfHAsciiString.hxx>
#include <Interface_Macros.hxx>
#include <Standard_DimensionMismatch.hxx>
#include <Standard_NullObject.hxx>
#include <Standard_OutOfRange.hxx>
#include <TCollection_HAsciiString.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <TColStd_HArray1OfTransient.hxx>
#include <IGESFile_Reader.hxx>
#include <IGESData_IGESWriter.hxx>
#include <Interface_EntityIterator.hxx>
#include <IGESData_DirChecker.hxx>
#include <Message_Messenger.hxx>
#include <IGESData_IGESDumper.hxx>
#include <IGESData_Dump.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESDefs_GenericData,IGESData_IGESEntity)

Standard_Integer IGESDefs_GenericData::NbTypeValuePairs () const
{
  return myTypes->Length();
}

Standard_Integer IGESDefs_GenericData::Type (const Standard_Integer Index) const
{
  return myTypes->Value(Index);
}

Handle(Standard_Transient) IGESDefs_GenericData::Value (const Standard_Integer Index) const
{
  return myValues->Value(Index);
}

Standard_Integer IGESDefs_GenericData::ValueAsInteger (const Standard_Integer Index) const
{
  return GetCasted(TColStd_HArray1OfInteger,myValues->Value(Index))->Value(1);
}

Standard_Real IGESDefs_GenericData::ValueAsReal (const Standard_Integer Index) const
{
  return GetCasted(TColStd_HArray1OfReal,myValues->Value(Index))->Value(1);
}

Handle(TCollection_HAsciiString) IGESDefs_GenericData::ValueAsString (const Standard_Integer Index) const
{
  return GetCasted(TCollection_HAsciiString,myValues->Value(Index));
}

Handle(IGESData_IGESEntity) IGESDefs_GenericData::ValueAsEntity (const Standard_Integer Index) const
{
  return GetCasted(IGESData_IGESEntity,myValues->Value(Index));
}

Standard_Boolean IGESDefs_GenericData::ValueAsLogical (const Standard_Integer Index) const
{
  return (GetCasted(TColStd_HArray1OfInteger,myValues->Value(Index))->Value(1) != 0);
}

void IGESDefs_GenericData::OwnRead (IGESFile_Reader &theReader)
{
  theReader.ReadInteger(myNbPropertyValues,"Number of property values");
  theReader.ReadText(myName,"Property Name");

  Standard_Integer num = 0;
  theReader.ReadInteger(num,"Number of TYPE/VALUEs");
  if (num > 0)
  {
    myTypes = new TColStd_HArray1OfInteger(1, num);
    myValues = new TColStd_HArray1OfTransient(1, num);

    for ( Standard_Integer i = 1; i <= num; i++ )
    {
      Standard_Integer tempTyp;
      theReader.ReadInteger(tempTyp,"Type code");
      myTypes->SetValue(i, tempTyp);
      switch (tempTyp)
      {
        case 0: theReader.ReadAny(); break;
        case 1: // Integer
	    {
          Handle(TColStd_HArray1OfInteger) tempObj = new TColStd_HArray1OfInteger(1,1);
          if (theReader.ReadInteger(tempObj->ChangeFirst(),"Integer value") == IGESFile_Reader::ParamOK)
            myValues->SetValue(i, tempObj);
	    }
        break;
        case 2: // Real
        {
          Handle(TColStd_HArray1OfReal) tempObj = new TColStd_HArray1OfReal(1,1);
          if (theReader.ReadReal(tempObj->ChangeFirst(),"Real value") == IGESFile_Reader::ParamOK)
            myValues->SetValue(i, tempObj);
        }
        break;
        case 3: // Character string
        {
          Handle(TCollection_HAsciiString) tempObj;
          if (theReader.ReadText(tempObj,"String value") == IGESFile_Reader::ParamOK)
            myValues->SetValue(i, tempObj);
        }
        break;
        case 4: // Pointer
        {
          Interface_Pointer<IGESData_IGESEntity> tempEntity;
          if (theReader.ReadPointer(tempEntity,"Entity value") == IGESFile_Reader::ParamOK)
            myValues->SetValue(i, tempEntity);
        }
        break;
        case 5: theReader.ReadAny();
        break;
        case 6: // Logical
	    {
          Handle(TColStd_HArray1OfInteger) tempObj = new TColStd_HArray1OfInteger(1,1);
          Standard_Boolean tempBool;
          if (theReader.ReadBoolean(tempBool,"Boolean value")) {
            tempObj->SetValue(1, (tempBool ? 1 : 0));
            myValues->SetValue(i, tempObj);
	      }
        }
        break;
      }
    }
  }
  else  theReader.AddFail("Number of TYPE/VALUEs: Not Positive");
}

void IGESDefs_GenericData::OwnWrite (IGESData_IGESWriter& IW) const
{
  IW.Send(myNbPropertyValues);
  IW.Send(myName);
  const Standard_Integer num = NbTypeValuePairs();
  IW.Send(num);
  for ( Standard_Integer i = 1; i <= num; i++ )
  {
    const Standard_Integer typ = Type(i);
    IW.Send(typ);
    switch (typ)
    {
      case 0 : IW.SendVoid();	  break;
      case 1 : IW.Send(ValueAsInteger(i));  break;
      case 2 : IW.Send(ValueAsReal(i));     break;
      case 3 : IW.Send(ValueAsString(i));   break;
      case 4 : IW.Send(ValueAsEntity(i));   break;
      case 5 : IW.SendVoid();   break;
      case 6 : IW.SendBoolean(ValueAsLogical(i));  break;
      default : break;
    }
  }
}

void IGESDefs_GenericData::OwnShared (Interface_EntityIterator& iter) const
{
  const Standard_Integer num = NbTypeValuePairs();
  for ( Standard_Integer i = 1; i <= num; i++ )
  {
    if (Type(i) == 4)
      iter.GetOneItem(ValueAsEntity(i));
  }
}

IGESData_DirChecker IGESDefs_GenericData::DirChecker () const
{
  IGESData_DirChecker DC(406, 27);
  DC.Structure(IGESData_DefVoid);
  DC.GraphicsIgnored();
  DC.LineFont(IGESData_DefVoid);
  DC.LineWeight(IGESData_DefVoid);
  DC.Color(IGESData_DefVoid);
  DC.BlankStatusIgnored();
  DC.SubordinateStatusRequired(1);
  DC.UseFlagRequired(2);
  DC.HierarchyStatusIgnored();
  return DC;
}

void IGESDefs_GenericData::OwnCheck (const Interface_ShareTool &, Handle(Interface_Check) &ach) const
{
  if (myNbPropertyValues != NbTypeValuePairs()*2 + 2)
    ach->AddFail("Nb. of Property Values not consistent with Nb. of Type/value Pairs");
}

void IGESDefs_GenericData::OwnDump (const IGESData_IGESDumper &dumper, const Handle(Message_Messenger) &S, const Standard_Integer level) const
{
  S << "IGESDefs_GenericData" << endl;
  S << "Number of property values : " << myNbPropertyValues << endl;
  S << "Property Name : ";
  IGESData_DumpString(S,myName);
  S << endl;
  switch (level)
  {
    case 4:
      S << "Types  : " << endl;
      S << "Values : Count = " << NbTypeValuePairs() << endl;
      S << "      [ as level > 4 for content ]" << endl;
      break;
    case 5:
    case 6:
    {
      Standard_Integer i, num;
      S << "Types & Values : " << endl;
      for ( num = NbTypeValuePairs(), i = 1; i <= num; i++ )
      {
	    S << "[" << i << "]: ";
	    S << "Type : " << Type(i);
	    switch (Type(i)) {
	      case 0 : S << "  (Void)";   break;
	      case 1 : S << "  Integer, Value : " << ValueAsInteger(i); break;
	      case 2 : S << "  Real   , Value : " << ValueAsReal(i); break;
	      case 3 : S << "  String , Value : "; IGESData_DumpString(S,ValueAsString(i));    break;
	      case 4 : S << "  Entity , Value : "; dumper.Dump(ValueAsEntity(i),S,level-1);    break;
	      case 5 : S << " (Not used)";  break;
	      case 6 : S << "  Logical, Value : " << (ValueAsLogical(i) ? "True" : "False");  break;
	      default : break;
        }
	    S << endl;
      }
    }
  }
  S << endl;
}
