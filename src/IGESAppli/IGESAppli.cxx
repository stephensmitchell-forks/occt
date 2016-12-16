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


#include <IGESAppli.hxx>
#include <IGESAppli_GeneralModule.hxx>
#include <IGESAppli_Protocol.hxx>
#include <IGESDefs.hxx>
#include <IGESDraw.hxx>
#include <Interface_GeneralLib.hxx>

#include <IGESData.hxx>
#include <IGESBasic.hxx>
#include <IGESGeom.hxx>
#include <IGESGraph.hxx>
#include <IGESDimen.hxx>
//#include <IGESSolid.hxx>

void IGESAppli::Init ()
{
  //Instead of IGESSolid::Init(); {
  //Instead of IGESGeom::Init(); {
  //Instead of IGESBasic::Init(); {
  IGESData::Init();      // usefull for Undefined Type or Erroneous Entities
  IGESBasic::Protocol();
  //}
  IGESGeom::Protocol();
  //}
  //IGESSolid::Protocol();
  //}
  //Instead of IGESDefs::Init(); {
  //Instead of IGESGraph::Init(); {
  //IGESBasic::Init();
  IGESGraph::Protocol();
  //}
  IGESDefs::Protocol();
  //}
  //Instead of IGESDraw::Init(); {
  //Instead of IGESDimen::Init(); {
  //IGESGeom::Init();
  //IGESGraph::Init();
  IGESDimen::Protocol();
  //}
  IGESDraw::Protocol();
  //}
  IGESAppli::Protocol();
}

const Handle(IGESAppli_Protocol) & IGESAppli::Protocol ()
{
  static Handle(IGESAppli_Protocol) protocol;
  if (protocol.IsNull()) {
    protocol = new IGESAppli_Protocol;
    /*Interface_GeneralLib::SetGlobal (new IGESSolid_GeneralModule,  protocol);*/
    Interface_GeneralLib::SetGlobal (new IGESAppli_GeneralModule,  protocol);
  }
  return protocol;
}
