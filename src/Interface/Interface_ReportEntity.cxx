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


#include <Interface_Check.hxx>
#include <Interface_ReportEntity.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Interface_ReportEntity,MMgt_TShared)

//=======================================================================
//function : Interface_ReportEntity
//purpose  : 
//=======================================================================

Interface_ReportEntity::Interface_ReportEntity (const Handle(Standard_Transient)& unknown)
: thecheck(new Interface_Check),
  theconcerned(unknown),
  thecontent(unknown)
{
}


//=======================================================================
//function : Interface_ReportEntity
//purpose  : 
//=======================================================================

Interface_ReportEntity::Interface_ReportEntity (const Handle(Interface_Check)& acheck, const Handle(Standard_Transient)& concerned)
:  thecheck(acheck),
   theconcerned(concerned)
{
  thecheck->SetEntity(concerned);
}


//=======================================================================
//function : HasNewContent
//purpose  : 
//=======================================================================

Standard_Boolean Interface_ReportEntity::HasNewContent () const 
{
  return (!thecontent.IsNull() && thecontent != theconcerned);
}


//=======================================================================
//function : IsError
//purpose  : 
//=======================================================================

Standard_Boolean Interface_ReportEntity::IsError () const
{
  return (thecheck->NbFails() > 0);
}


//=======================================================================
//function : IsUnknown
//purpose  : 
//=======================================================================

Standard_Boolean Interface_ReportEntity::IsUnknown () const
{
  return ((thecheck->NbFails() == 0) && (thecheck->NbWarnings() == 0) && (theconcerned == thecontent));
}
