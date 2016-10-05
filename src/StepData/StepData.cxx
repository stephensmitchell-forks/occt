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

//    abv 09.04.99 S4136: eliminate parameter step.readaccept.void
//    svv #2    23.02.00: porting on SIL

#include <StepData.hxx>
#include <Interface_Macros.hxx>
#include <HeaderSection.hxx>
#include <HeaderSection_Protocol.hxx>
#include <StepData_Protocol.hxx>

//szv_c1:StaticHandle(StepData_Protocol,proto);
static Handle(StepData_Protocol) proto;

//szv_c1:StaticHandleA(StepData_Protocol,theheader);
static Handle(StepData_Protocol) theheader;

const Handle(StepData_Protocol) & StepData::Protocol ()
{
  //szv_c1:InitHandleVoid(StepData_Protocol,proto);// svv #2
  if (proto.IsNull()) proto = new StepData_Protocol;
  return proto;
}

const Handle(StepData_Protocol) & StepData::HeaderProtocol ()
{
  /*szv_c1:UseHandle(StepData_Protocol,theheader);
  return theheader;*/
  return HeaderSection::Protocol();
}
