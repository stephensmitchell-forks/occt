// Created on: 2017-02-17
// Created by: Nikolai BUKHALOV
// Copyright (c) 1999-2017 OPEN CASCADE SAS
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

#include <TopClass_GeomVertex.hxx>

#include <BRep_Tool.hxx>
#include <TopoDS_Vertex.hxx>

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================
void TopClass_GeomVertex::Init(const TopoDS_Vertex &theV,
                               const TopoDS_Edge   &theE)
{
  if (theV.IsNull())
    return;

  // If theV.IsNull() == TRUE then BRep_Tool::Parameter(...)
  // throws exception.
  Init(theV, BRep_Tool::Parameter(theV, theE));
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================
void TopClass_GeomVertex::Init(const TopoDS_Vertex &theV,
                               const Standard_Real theParam)
{
  if (theV.IsNull())
    return;

  // If theV.IsNull() == TRUE then BRep_Tool::Tolerance(...) and
  // BRep_Tool::Pnt(...) throw exception.

  myEdgeParameter = theParam;
  myToler = BRep_Tool::Tolerance(theV);
  myPoint = BRep_Tool::Pnt(theV);  
}
