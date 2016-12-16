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

#include <IGESGraph_NominalSize.hxx>
#include <TCollection_HAsciiString.hxx>
#include <IGESFile_Reader.hxx>
#include <IGESData_IGESWriter.hxx>
#include <IGESData_DirChecker.hxx>
#include <Message_Messenger.hxx>
#include <IGESData_IGESDumper.hxx>
#include <IGESData_Dump.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESGraph_NominalSize,IGESData_IGESEntity)

void IGESGraph_NominalSize::OwnRead (IGESFile_Reader &PR)
{ 
  Standard_Integer aNbPropertyValues = 0;
  PR.ReadInteger(aNbPropertyValues,"No. of property values");
  if ( (aNbPropertyValues != 2) && (aNbPropertyValues != 3) )
    PR.AddFail("No. of Property values : Value is not 2/3");

  PR.ReadReal(myNominalSizeValue,"Nominal size value");
  PR.ReadText(myNominalSizeName,"Nominal size name");
  if (aNbPropertyValues > 2)
    PR.ReadText(myStandardName,"Name of relevant engg. standard");
}

void IGESGraph_NominalSize::OwnWrite (IGESData_IGESWriter &IW) const
{ 
  const Standard_Integer aNbPropertyValues = myStandardName.IsNull()? 2 : 3;
  IW.Send(aNbPropertyValues);
  IW.Send(myNominalSizeValue);
  IW.Send(myNominalSizeName);
  if (aNbPropertyValues > 2)
    IW.Send(myStandardName);
}

IGESData_DirChecker IGESGraph_NominalSize::DirChecker () const
{ 
  IGESData_DirChecker DC (406, 13);
  DC.Structure(IGESData_DefVoid);
  DC.LineFont(IGESData_DefVoid);
  DC.LineWeight(IGESData_DefVoid);
  DC.Color(IGESData_DefVoid);
  DC.BlankStatusIgnored();
  DC.UseFlagIgnored();
  DC.HierarchyStatusIgnored();
  return DC;
}

void IGESGraph_NominalSize::OwnDump (const IGESData_IGESDumper &, const Handle(Message_Messenger) &S, const Standard_Integer) const
{
  S << "IGESGraph_NominalSize" << endl;
  const Standard_Integer aNbPropertyValues = myStandardName.IsNull()? 2 : 3;
  S << "No. of property values : " << aNbPropertyValues << endl;
  S << "Nominal size value : "     << myNominalSizeValue << endl;
  S << "Nominal size name  : ";
  IGESData_DumpString(S,myNominalSizeName);
  S << endl;
  S << "Name of relevant engineering standard : ";
  IGESData_DumpString(S,myStandardName);
  S << endl;
}
