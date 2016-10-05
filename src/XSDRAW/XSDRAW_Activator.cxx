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

#include <XSDRAW_Activator.hxx>

IMPLEMENT_STANDARD_RTTIEXT(XSDRAW_Activator,IFSelect_Activator)

static TCollection_AsciiString thedefgr, thedefil;

void XSDRAW_Activator::SetGroup (const Standard_CString group, const Standard_CString file)
{
  thedefgr.Clear();  if (group[0] != '\0') thedefgr.AssignCat(group);
  thedefil.Clear();  if (file [0] != '\0') thedefil.AssignCat(file);
}

void XSDRAW_Activator::AddFunc (const Standard_CString name, const Standard_CString help,  const XSDRAW_ActFunc  func)
{
  Handle(XSDRAW_Activator) act = new XSDRAW_Activator (name,help,func);
  if (thedefgr.Length() > 0) act->SetForGroup (thedefgr.ToCString());
  act->Add    (1,name);
}

void XSDRAW_Activator::AddFSet (const Standard_CString name, const Standard_CString help, const XSDRAW_ActFunc  func)
{
  Handle(XSDRAW_Activator) act = new XSDRAW_Activator (name,help,func);
  if (thedefgr.Length() > 0)
    act->SetForGroup (thedefgr.ToCString(),thedefil.ToCString());
  act->AddSet (1,name);
}
