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

#include <TCollection_HAsciiString.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <TColStd_HArray1OfTransient.hxx>
#include <IGESData_HArray1OfIGESEntity.hxx>
#include <IGESData_IGESEntity.hxx>
#include <IGESDefs_AttributeDef.hxx>
#include <IGESDefs_HArray1OfHArray1OfTextDisplayTemplate.hxx>
#include <IGESGraph_HArray1OfTextDisplayTemplate.hxx>
#include <IGESGraph_TextDisplayTemplate.hxx>
#include <Interface_HArray1OfHAsciiString.hxx>
#include <Interface_Macros.hxx>
#include <Standard_DimensionMismatch.hxx>
#include <Standard_NullObject.hxx>
#include <Standard_OutOfRange.hxx>
#include <IGESFile_Reader.hxx>
#include <IGESData_IGESWriter.hxx>
#include <Interface_EntityIterator.hxx>
#include <IGESData_DirChecker.hxx>
#include <Message_Messenger.hxx>
#include <IGESData_IGESDumper.hxx>
#include <IGESData_Dump.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESDefs_AttributeDef,IGESData_IGESEntity)

Standard_Integer IGESDefs_AttributeDef::NbAttributes () const
{
  return theAttrTypes->Length();
}

Standard_Integer IGESDefs_AttributeDef::AttributeType (const Standard_Integer num) const
{
  return theAttrTypes->Value(num);
}

Standard_Integer IGESDefs_AttributeDef::AttributeValueDataType (const Standard_Integer num) const
{
  return theAttrValueDataTypes->Value(num);
}

Standard_Integer IGESDefs_AttributeDef::AttributeValueCount (const Standard_Integer num) const
{
  return theAttrValueCounts->Value(num);
}

Handle(IGESGraph_TextDisplayTemplate) IGESDefs_AttributeDef::AttributeTextDisplay (const Standard_Integer AttrNum, const Standard_Integer PointerNum) const
{
  Handle(IGESGraph_TextDisplayTemplate)  res;
  if (HasTextDisplay())
    res = theAttrValuePointers->Value(AttrNum)->Value(PointerNum);
  return res;
}

Handle(Standard_Transient) IGESDefs_AttributeDef::AttributeList (const Standard_Integer AttrNum) const
{
  Handle(Standard_Transient) nulres;
  if (!HasValues()) return nulres;
  return theAttrValues->Value(AttrNum);
}

Standard_Integer IGESDefs_AttributeDef::AttributeAsInteger (const Standard_Integer AttrNum, const Standard_Integer ValueNum) const
{
  return GetCasted(TColStd_HArray1OfInteger,theAttrValues->Value(AttrNum))->Value(ValueNum);
}

Standard_Real IGESDefs_AttributeDef::AttributeAsReal (const Standard_Integer AttrNum, const Standard_Integer ValueNum) const
{
  return GetCasted(TColStd_HArray1OfReal,theAttrValues->Value(AttrNum))->Value(ValueNum);
}

Handle(TCollection_HAsciiString) IGESDefs_AttributeDef::AttributeAsString (const Standard_Integer AttrNum, const Standard_Integer ValueNum) const
{
  return GetCasted(Interface_HArray1OfHAsciiString,theAttrValues->Value(AttrNum))->Value(ValueNum);
}

Handle(IGESData_IGESEntity) IGESDefs_AttributeDef::AttributeAsEntity (const Standard_Integer AttrNum, const Standard_Integer ValueNum) const
{
  return GetCasted(IGESData_HArray1OfIGESEntity,theAttrValues->Value(AttrNum))->Value(ValueNum);
}

Standard_Boolean IGESDefs_AttributeDef::AttributeAsLogical (const Standard_Integer AttrNum, const Standard_Integer ValueNum) const
{
  return (GetCasted(TColStd_HArray1OfInteger,theAttrValues->Value(AttrNum))->Value(ValueNum) != 0);
}

void IGESDefs_AttributeDef::OwnRead (IGESFile_Reader& theReader)
{
  theReader.ReadText(theName,"Attribute Table Name");
  theReader.ReadInteger(theListType,"Attribute List Type");

  Standard_Integer nbval = 0;
  theReader.ReadInteger(nbval,"Number of Attributes");
  if (nbval > 0)
  {
    theAttrTypes          = new TColStd_HArray1OfInteger(1, nbval);
    theAttrValueDataTypes = new TColStd_HArray1OfInteger(1, nbval);
    theAttrValueCounts    = new TColStd_HArray1OfInteger(1, nbval);
    if (myForm > 0)
      theAttrValues = new TColStd_HArray1OfTransient(1, nbval);
    if (myForm > 1)
      theAttrValuePointers = new IGESDefs_HArray1OfHArray1OfTextDisplayTemplate(1, nbval);

    for (Standard_Integer i = 1; i <= nbval; i++)
    {
      theReader.ReadInteger(theAttrTypes->ChangeValue(i),"Attribute Type");

      Standard_Integer attrValueDataType = 0;
      theReader.ReadInteger(attrValueDataType,"Attribute Data Type");
      theAttrValueDataTypes->SetValue(i, attrValueDataType);

      Standard_Integer avc = 0;
      if (theReader.ReadInteger(avc,"Attribute Value Count") == IGESFile_Reader::ParamEmpty) avc = 1;
      theAttrValueCounts->SetValue(i, avc);

      Handle(IGESGraph_HArray1OfTextDisplayTemplate) attrValuePointer;
      if (myForm > 1 && avc > 0) attrValuePointer = new IGESGraph_HArray1OfTextDisplayTemplate(1, avc);

      if (myForm > 0)
      {
        Handle(TColStd_HArray1OfInteger)        attrInt;
        Handle(TColStd_HArray1OfReal)           attrReal;
        Handle(Interface_HArray1OfHAsciiString) attrStr;
        Handle(IGESData_HArray1OfIGESEntity)    attrEnt;
        switch (attrValueDataType)
        {
          case 1 :
          case 6 : attrInt  = new TColStd_HArray1OfInteger       (1,avc); theAttrValues->SetValue(i,attrInt);  break;
          case 2 : attrReal = new TColStd_HArray1OfReal          (1,avc); theAttrValues->SetValue(i,attrReal); break;
          case 3 : attrStr  = new Interface_HArray1OfHAsciiString(1,avc); theAttrValues->SetValue(i,attrStr);  break;
          case 4 : attrEnt  = new IGESData_HArray1OfIGESEntity   (1,avc); theAttrValues->SetValue(i,attrEnt);  break;
          default : break;
        }
        for (Standard_Integer j = 1; j <= avc; j++)
        {
          switch (attrValueDataType)
          {
            case 0: theReader.ReadAny(); break;
            case 1: theReader.ReadInteger(attrInt->ChangeValue(j),"Attribute Value"); break;
            case 2: theReader.ReadReal(attrReal->ChangeValue(j),"Attribute Value"); break;
            case 3: theReader.ReadText(attrStr->ChangeValue(j),"Attribute Value"); break;
            case 4: theReader.ReadPointer(attrEnt->ChangeValue(j),"Attribute Value"); break;
            case 5: theReader.ReadAny(); break;
            case 6:
            {
              Standard_Boolean temp = Standard_False;
              theReader.ReadBoolean(temp,"Attribute Value");
              attrInt->SetValue(j, (temp ? 1 : 0));
            }
            break;
          }
          if (!attrValuePointer.IsNull())
            theReader.ReadPointer(attrValuePointer->ChangeValue(j),"Attribute Val. Pointer");
        }
        if (!attrValuePointer.IsNull())
          theAttrValuePointers->SetValue(i, attrValuePointer);
	  }
    }
  }
  else theReader.AddFail("Number of Attributes: Not Positive");
}

void IGESDefs_AttributeDef::OwnWrite (IGESData_IGESWriter& IW) const
{
  if (HasTableName()) IW.Send(TableName());
  else                IW.SendVoid();
  IW.Send(ListType());
  const Standard_Integer upper = NbAttributes();
  IW.Send(upper);

  for (Standard_Integer i = 1; i <= upper; i++)
  {
    const Standard_Integer check = AttributeValueDataType(i);
    const Standard_Integer count = AttributeValueCount(i);
    IW.Send(AttributeType(i));
    IW.Send(check);
    IW.Send(count);
    if (FormNumber() > 0)
    {
      for (Standard_Integer j = 1; j <= count; j++)
      {
        switch (check) {
          case 0 : IW.SendVoid();  break;
          case 1 : IW.Send(AttributeAsInteger(i,j));  break;
          case 2 : IW.Send(AttributeAsReal(i,j));     break;
          case 3 : IW.Send(AttributeAsString(i,j));   break;
          case 4 : IW.Send(AttributeAsEntity(i,j));   break;
          case 5 : IW.SendVoid();  break;
          case 6 : IW.SendBoolean(AttributeAsLogical(i,j)); break;
          default : break;
        }
        if (FormNumber() == 2)
          IW.Send(AttributeTextDisplay(i,j));
      }
    }
  }
}

void IGESDefs_AttributeDef::OwnShared (Interface_EntityIterator& iter) const
{
  const Standard_Integer upper = NbAttributes();
  for (Standard_Integer i = 1; i <= upper; i++)
  {
    const Standard_Integer check = AttributeValueDataType(i);
    const Standard_Integer count = AttributeValueCount(i);
    if (FormNumber() > 0)
    {
      for (Standard_Integer j = 1; j <= count; j++)
      {
        if ( check == 4 )
          iter.GetOneItem(AttributeAsEntity(i,j));
        if ( FormNumber() == 2 )
          iter.GetOneItem(AttributeTextDisplay(i,j));
      }
    }
  }
}

IGESData_DirChecker IGESDefs_AttributeDef::DirChecker () const
{
  IGESData_DirChecker DC (322, 0, 2);
  DC.Structure(IGESData_DefVoid);
  DC.LineFont(IGESData_DefAny);
  DC.LineWeight(IGESData_DefValue);
  DC.Color(IGESData_DefAny);
  DC.BlankStatusIgnored();
  DC.SubordinateStatusRequired(0);
  DC.UseFlagRequired(2);
  DC.GraphicsIgnored(1);
  return DC;
}

void IGESDefs_AttributeDef::OwnCheck (const Interface_ShareTool &, Handle(Interface_Check) & ach) const
{
  const Standard_Integer nb = NbAttributes();
  const Standard_Integer fn = FormNumber();
  for (Standard_Integer i = 1; i <= nb; i ++) {
    char mess[80];
    if (AttributeType(i) < 0 || AttributeType(i) > 9999) {
      sprintf(mess,"Attribute Type n0.%d not in <0 - 9999>", AttributeType(i));
      ach->AddFail(mess);
    }
    const Standard_Integer aty = AttributeValueDataType(i);
    if (aty < 0 || aty > 6) {
      sprintf(mess,"Attribute Value Data Type n0.%d not in <0 - 6>", aty);
      ach->AddFail(mess);
    }
    if (AttributeValueCount(i) <= 0) continue;
    Handle(Standard_Transient) list = AttributeList(i);
    if (fn > 0 && list.IsNull()) {
      if (aty == 0 || aty == 5) continue;
      sprintf(mess,"Form Number > 0 and Attribute Value List n0.%d undefined", aty);
      ach->AddFail(mess);
      continue;
    }
    else if (fn == 0) continue;
    mess[0] = '\0';
    switch (aty) {
      case 1 : if (!list->IsKind(STANDARD_TYPE(TColStd_HArray1OfInteger))) sprintf(mess,"Attribute List n0.%d (Integers) badly defined", aty); break;
      case 2 : if (!list->IsKind(STANDARD_TYPE(TColStd_HArray1OfReal))) sprintf(mess,"Attribute List n0.%d (Reals) badly defined", aty); break;
      case 3 : if (!list->IsKind(STANDARD_TYPE(Interface_HArray1OfHAsciiString))) sprintf(mess,"Attribute List n0.%d (Strings) badly defined", aty); break;
      case 4 : if (!list->IsKind(STANDARD_TYPE(IGESData_HArray1OfIGESEntity))) sprintf(mess,"Attribute List n0.%d (IGES Pointers) badly defined", aty); break;
      case 6 : if (!list->IsKind(STANDARD_TYPE(TColStd_HArray1OfInteger))) sprintf(mess,"Attribute List n0.%d (Logicals i.e. Integers) badly defined", aty); break;
      default : break;
    }
    if (mess[0] != '\0') ach->AddFail(mess);
  }
}

void IGESDefs_AttributeDef::OwnDump (const IGESData_IGESDumper &dumper, const Handle(Message_Messenger) &S, const Standard_Integer level) const
{
  Standard_Integer sublevel = (level > 4) ? 1 : 0;

  S << "IGESDefs_AttributeDef" << endl;
  S << "Attribute Table Name: ";
  IGESData_DumpString(S,TableName());
  S << endl;
  S << "Attribute List Type  : " << ListType() << endl;
  S << "Number of Attributes : " << NbAttributes() << endl;
  S << "Attribute Types : " << endl;
  S << "Attribute Value Data Types : " << endl;
  S << "Attribute Value Counts : " << endl;
  if (HasValues())       S << "Attribute Values : " << endl;
  if (HasTextDisplay())  S << "Attribute Value Entities : " << endl;
  IGESData_DumpVals(S,-level,1,NbAttributes(),AttributeType);
  S << endl;
  if (level > 4)
  {
    const Standard_Integer upper = NbAttributes();
    for (Standard_Integer i = 1; i <= upper ; i ++)
	{
      const Standard_Integer avc = AttributeValueCount(i);
      Standard_Integer typ = AttributeValueDataType(i);
      S << "[" << i << "]:  ";
      S << "Attribute Type : " << AttributeType(i) << "  ";
      S << "Value Data Type : " << typ;
      switch (typ) {
        case 0 : S << "  (Void)";     break;
        case 1 : S << " : Integer ";  break;
        case 2 : S << " : Real    ";  break;
        case 3 : S << " : String  ";  break;
        case 4 : S << " : Entity  ";  break;
        case 5 : S << " (Not Used)";  break;
        case 6 : S << " : Logical ";  break;
        default :   break;
      }
      S << "   Count : " << avc << endl;
      if (HasValues())
      {
        if (level <= 5) {
          S << " [ content (Values) : ask level > 5 ]" << endl;
          continue;
        }
        for (Standard_Integer j = 1; j <= avc; j ++)
        {
          S << "[" << j << "]: ";
          switch(AttributeValueDataType(i))
          {
            case 0:  S << "(Void) "; break;
            case 1:  S << AttributeAsInteger(i,j); break;
            case 2:  S << AttributeAsReal(i,j); break;
            case 3:  IGESData_DumpString(S,AttributeAsString(i,j)); break;
            case 4: dumper.Dump(AttributeAsEntity(i,j),S,level-5); break;
            case 5:  S << "(Not Used)"; break;
            case 6: S << (AttributeAsLogical(i,j) ? "True" : "False"); break;
            default : break;
          }
          if (HasTextDisplay())
          {
            S << "  Attribute Value Pointer : ";
            dumper.Dump(AttributeTextDisplay(i,j),S, sublevel);
          }
          S << endl;
        }
      }
    }
  }
  S << endl;
}
