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

#include <IGESData_HArray1OfIGESEntity.hxx>
#include <IGESData_IGESEntity.hxx>
#include <IGESDefs_AttributeDef.hxx>
#include <IGESDefs_AttributeTable.hxx>
#include <Interface_HArray1OfHAsciiString.hxx>
#include <Interface_Macros.hxx>
#include <Standard_DimensionMismatch.hxx>
#include <Standard_NullObject.hxx>
#include <Standard_OutOfRange.hxx>
#include <TCollection_HAsciiString.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <TColStd_HArray2OfTransient.hxx>
#include <IGESFile_Reader.hxx>
#include <IGESData_IGESWriter.hxx>
#include <Interface_EntityIterator.hxx>
#include <IGESData_DirChecker.hxx>
#include <Message_Messenger.hxx>
#include <IGESData_IGESDumper.hxx>
#include <IGESData_Dump.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESDefs_AttributeTable,IGESData_IGESEntity)

void IGESDefs_AttributeTable::SetDefinition (const Handle(IGESDefs_AttributeDef)& def)
{
  InitMisc (def,LabelDisplay(),LineWeightNumber());
}

Handle(IGESDefs_AttributeDef) IGESDefs_AttributeTable::Definition () const
{
  return GetCasted(IGESDefs_AttributeDef,Structure());
}

Standard_Integer IGESDefs_AttributeTable::NbRows () const
{
  return myAttributes->UpperCol();
}

Standard_Integer IGESDefs_AttributeTable::NbAttributes () const
{
  return myAttributes->UpperRow();
}

Standard_Integer IGESDefs_AttributeTable::DataType (const Standard_Integer Atnum) const
{
  return Definition()->AttributeType(Atnum);
}

Standard_Integer IGESDefs_AttributeTable::ValueCount (const Standard_Integer Atnum) const
{
  return Definition()->AttributeValueCount(Atnum);
}

Handle(Standard_Transient) IGESDefs_AttributeTable::AttributeList (const Standard_Integer Atnum, const Standard_Integer Rownum) const
{
  return myAttributes->Value(Atnum,Rownum);
}

Standard_Integer IGESDefs_AttributeTable::AttributeAsInteger (const Standard_Integer Atnum, const Standard_Integer Rownum, const Standard_Integer Valuenum) const
{
  return GetCasted(TColStd_HArray1OfInteger,myAttributes->Value(Atnum,Rownum))->Value(Valuenum);
}

Standard_Real IGESDefs_AttributeTable::AttributeAsReal (const Standard_Integer Atnum, const Standard_Integer Rownum, const Standard_Integer Valuenum) const
{
  return GetCasted(TColStd_HArray1OfReal,myAttributes->Value(Atnum,Rownum))->Value(Valuenum);
}

Handle(TCollection_HAsciiString) IGESDefs_AttributeTable::AttributeAsString (const Standard_Integer Atnum, const Standard_Integer Rownum, const Standard_Integer Valuenum) const
{
  return GetCasted(Interface_HArray1OfHAsciiString,myAttributes->Value(Atnum,Rownum))->Value(Valuenum);
}

Handle(IGESData_IGESEntity) IGESDefs_AttributeTable::AttributeAsEntity (const Standard_Integer Atnum, const Standard_Integer Rownum, const Standard_Integer Valuenum) const
{
  return GetCasted(IGESData_HArray1OfIGESEntity,myAttributes->Value(Atnum,Rownum))->Value(Valuenum);
}

Standard_Boolean IGESDefs_AttributeTable::AttributeAsLogical (const Standard_Integer Atnum, const Standard_Integer Rownum, const Standard_Integer Valuenum) const
{
  return (AttributeAsInteger(Atnum,Rownum,Valuenum) != 0);    // raccourci
}

void IGESDefs_AttributeTable::OwnRead (IGESFile_Reader &theReader)
{ 
  Handle(IGESDefs_AttributeDef) ab = Definition();  // formerly loaded
  if (ab.IsNull()) {
    theReader.AddFail("No Attribute Definition as Structure");
    return;
  }

  const Standard_Integer na = ab->NbAttributes();

  Standard_Integer j, nr = 1;
  if (myForm == 1)
    if (theReader.ReadInteger(nr,"No. of rows") == IGESFile_Reader::ParamError) nr = 0;

  if (na > 0 && nr > 0)
    myAttributes = new TColStd_HArray2OfTransient(1,na,1,nr);

  //  AttributeDef repeated once (Form 0) or <nr> times (Form 1)
  for (Standard_Integer k = 1; k <= nr; k ++)
  {
    for (Standard_Integer i = 1; i <= na; i ++)
    {
      const Standard_Integer avc   = ab->AttributeValueCount(i);
      const Standard_Integer atype = ab->AttributeValueDataType(i);
      switch (atype)
      {
        case 0 : 
          for (j = 1; j <= avc; j ++)
            theReader.ReadAny();
          break;
        case 1 : {
          Handle(TColStd_HArray1OfInteger) attrInt = new TColStd_HArray1OfInteger(1,avc);
          myAttributes->SetValue(i,k,attrInt);
          for (j = 1; j <= avc; j ++)
            theReader.ReadInteger(attrInt->ChangeValue(j),"Value");
        }
        break;
        case 2 : {
          Handle(TColStd_HArray1OfReal) attrReal = new TColStd_HArray1OfReal(1,avc);
          myAttributes->SetValue(i,k,attrReal);
          for (j = 1; j <= avc; j ++)
            theReader.ReadReal(attrReal->ChangeValue(j),"Value");
        }
        break;
        case 3 : {
          Handle(Interface_HArray1OfHAsciiString) attrStr =	new Interface_HArray1OfHAsciiString(1,avc);
          myAttributes->SetValue(i,k,attrStr);
          for (j = 1; j <= avc; j ++)
            theReader.ReadText(attrStr->ChangeValue(j),"Value");
        }
        break;
        case 4 : {
          Handle(IGESData_HArray1OfIGESEntity) attrEnt = new IGESData_HArray1OfIGESEntity(1,avc);
          myAttributes->SetValue(i,k,attrEnt);
          for (j = 1; j <= avc; j ++)
            theReader.ReadPointer(attrEnt->ChangeValue(j),"Value");
        }
        break;
        case 5 :
          for (j = 1; j <= avc; j ++)
            theReader.ReadAny();
          break;
        case 6 : {    // Here item takes value 0 or 1
          Handle(TColStd_HArray1OfInteger) attrInt  = new TColStd_HArray1OfInteger(1,avc);
          myAttributes->SetValue(i,k,attrInt);
          for (j = 1; j <= avc; j ++)
            theReader.ReadInteger(attrInt->ChangeValue(j),"Value");
        }
        break;
        default : break;
      }
    }
  }
}

void IGESDefs_AttributeTable::OwnWrite (IGESData_IGESWriter& IW) const
{
  Handle(IGESDefs_AttributeDef) ab = Definition();

  const Standard_Integer nr = NbRows();
  const Standard_Integer na = NbAttributes();
  if (myForm == 1) IW.Send(nr);
  for (Standard_Integer k = 1; k <= nr; k ++)
  {
    for (Standard_Integer i = 1; i <= na; i ++)
	{
      const Standard_Integer count = ab->AttributeValueCount(i);
      for (Standard_Integer j = 1;j <= count; j++)
      {
        switch(ab->AttributeValueDataType(i))
        {
          case 0 : IW.SendVoid();  break;
          case 1 : IW.Send(AttributeAsInteger(i,k,j));  break;
          case 2 : IW.Send(AttributeAsReal   (i,k,j));  break;
          case 3 : IW.Send(AttributeAsString (i,k,j));  break;
          case 4 : IW.Send(AttributeAsEntity (i,k,j));  break;
          case 5 : IW.SendVoid();  break;
          case 6 : IW.SendBoolean(AttributeAsLogical(i,k,j)); break;
          default : break;
        }
      }
    }
  }
}

void IGESDefs_AttributeTable::OwnShared (Interface_EntityIterator& iter) const
{
  Handle(IGESDefs_AttributeDef) ab = Definition();
  const Standard_Integer na = NbAttributes();
  const Standard_Integer nr = NbRows();
  for (Standard_Integer k = 1; k <= nr; k ++)
  {
    for (Standard_Integer i = 1; i <= na; i ++)
    {
      if (ab->AttributeValueDataType(i) != 4) continue;
      const Standard_Integer avc = ab->AttributeValueCount(i);
      for (Standard_Integer j = 1; j <= avc; j ++)
        iter.GetOneItem(AttributeAsEntity(i,k,j));
    }
  }
}

IGESData_DirChecker IGESDefs_AttributeTable::DirChecker () const
{ 
  IGESData_DirChecker DC(422,0,1);
  DC.Structure(IGESData_DefReference);
  DC.GraphicsIgnored();
  DC.BlankStatusIgnored();
  DC.HierarchyStatusIgnored();
  return DC;
}

void IGESDefs_AttributeTable::OwnCheck (const Interface_ShareTool &, Handle(Interface_Check) &ach) const
{
  if (Definition().IsNull()) {
    if (!Structure().IsNull())
      ach->AddFail("Structure in Directory Entry is not an Attribute Definition Table");
    else
      ach->AddFail("No Attribute Definition defined");
  }
  if (myForm == 0 && NbRows() != 1)
    ach->AddFail("Form 0 with several Rows");
  if (NbAttributes() != Definition()->NbAttributes())
    ach->AddFail("Mismatch between Definition (Structure) and Content");
}

void IGESDefs_AttributeTable::OwnDump (const IGESData_IGESDumper &dumper, const Handle(Message_Messenger) &S, const Standard_Integer level) const
{ 
  S << "IGESDefs_AttributeTable" << endl;

  Handle(IGESDefs_AttributeDef) ab = Definition();

  const Standard_Integer na = NbAttributes();
  const Standard_Integer nr = NbRows();
  if (myForm == 1)
    S << "Number of Rows (i.e. complete sets of Attributes) : " << nr << endl;
  else S << "One set of Attributes" << endl;
  S << "Number of defined Attributes : " << na << endl;
  if (level <= 4) S <<
    " [ structure : see Structure in Directory Entry; content : level > 4 ]" <<endl;
  else
    for (Standard_Integer k = 1; k <= nr; k ++)
    {
      for (Standard_Integer i = 1; i <= na; i ++)
      {
        const Standard_Integer avc = ab->AttributeValueCount(i);
        S << "[At.no."<<i<<" Row:"<<k<<"]";
        switch (ab->AttributeValueDataType(i)) {
	      case 0 : S << "  (Void) ";   break;
	      case 1 : S << "  Integer";  break;
	      case 2 : S << "  Real   ";  break;
	      case 3 : S << "  String ";  break;
	      case 4 : S << "  Entity ";  break;
	      case 5 : S << " (Not used)"; break;
	      case 6 : S << "  Logical";  break;
	      default : break;
	    }
	    S << " :";
	    for (Standard_Integer j = 1;j <= avc; j++) {
	      S << "  ";
          switch(ab->AttributeValueDataType(i)) {
            case 1 : S << AttributeAsInteger(i,k,j);  break;
            case 2 : S << AttributeAsReal   (i,k,j);  break;
            case 3 : IGESData_DumpString(S,AttributeAsString (i,k,j)); break;
            case 4 : dumper.Dump(AttributeAsEntity (i,k,j),S,level-5); break;
            case 6 : S << (AttributeAsLogical(i,k,j) ? "True" : "False"); break;
            default : break;
	      }
	    }
	    S << endl;
	  }
    }
  S << endl;
}
