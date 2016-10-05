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


#include <BRepToIGES_BREntity.hxx>
#include <BRepToIGESBRep_Entity.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Surface.hxx>
#include <GeomToIGES_GeomCurve.hxx>
#include <GeomToIGES_GeomSurface.hxx>
#include <IGESControl_ActorWrite.hxx>
#include <IGESData_IGESEntity.hxx>
#include <IGESData_IGESModel.hxx>
#include <Interface_Macros.hxx>
#include <Interface_Static.hxx>
#include <Standard_Type.hxx>
#include <Transfer_Binder.hxx>
#include <Transfer_Finder.hxx>
#include <Transfer_FinderProcess.hxx>
#include <Transfer_SimpleBinderOfTransient.hxx>
#include <TransferBRep_ShapeMapper.hxx>
#include <XSAlgo.hxx>
#include <XSAlgo_AlgoContainer.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESControl_ActorWrite,Transfer_ActorOfFinderProcess)

Standard_Boolean IGESControl_ActorWrite::Recognize (const Handle(Standard_Transient)& start)
{
  DeclareAndCast(TransferBRep_ShapeMapper,shmap,start);
  if (!shmap.IsNull()) return Standard_True;
  DeclareAndCast(Geom_Curve,Curve,start);
  DeclareAndCast(Geom_Surface,Surf,start);
  if (!Curve.IsNull() || !Surf.IsNull()) return Standard_True;
  return Standard_False;
}

Handle(Transfer_Binder) IGESControl_ActorWrite::Transferring (const Handle(Standard_Transient)& start, const Handle(Transfer_Process)& TP)
{
  XSAlgo::AlgoContainer()->PrepareForTransfer();
    
  DeclareAndCast(IGESData_IGESModel,modl,TP->Model());
  if (modl.IsNull()) return NULL;
  if (themodetrans < 0 || themodetrans > 1) return NULL;
  Handle(Standard_Transient) ent;

  DeclareAndCast(TransferBRep_ShapeMapper,shmap,start);
  if (!shmap.IsNull()) {
    TopoDS_Shape shape = shmap->Shape();
    if (shape.IsNull()) return NULL;
    Handle(Standard_Transient) info;
    Standard_Real Tol = Interface_Static::RVal("write.precision.val");
    Standard_Real maxTol = Interface_Static::RVal("read.maxprecision.val");
    shape = XSAlgo::AlgoContainer()->ProcessShape( shape, Tol, maxTol, 
                                                   "write.iges.resource.name", 
                                                   "write.iges.sequence", info,
                                                   TP->GetProgress() );

	Handle(Transfer_FinderProcess) FP = Handle(Transfer_FinderProcess)::DownCast(TP);

    BRepToIGES_BREntity   BR0; BR0.SetModel(modl);  BR0.SetTransferProcess(FP);
    BRepToIGESBRep_Entity BR1; BR1.SetModel(modl);  BR1.SetTransferProcess(FP);

    if (themodetrans == 0) ent = BR0.TransferShape(shape);
    if (themodetrans == 1) ent = BR1.TransferShape(shape);
    XSAlgo::AlgoContainer()->MergeTransferInfo(FP, info);
    if (!ent.IsNull()) return new Transfer_SimpleBinderOfTransient(ent);
  }

  DeclareAndCast(Geom_Curve,Curve,start);
  DeclareAndCast(Geom_Surface,Surf,start);
 
//  On reconnait : Curve et Surface de Geom
//   quid de Point; Geom2d ?
 
  GeomToIGES_GeomCurve GC;    GC.SetModel(modl);
  GeomToIGES_GeomSurface GS;  GS.SetModel(modl);
  if (!Curve.IsNull())
    ent = GC.TransferCurve(Curve,Curve->FirstParameter(),Curve->LastParameter());
  else if (!Surf.IsNull()) {
    Standard_Real U1,U2,V1,V2;
    Surf->Bounds(U1,U2,V1,V2);
    ent = GS.TransferSurface(Surf,U1,U2,V1,V2);
  }
  if (!ent.IsNull()) return new Transfer_SimpleBinderOfTransient(ent);

  return NULL;
}
