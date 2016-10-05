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


#include <Interface_InterfaceModel.hxx>
#include <Interface_Macros.hxx>
#include <TopoDS_HShape.hxx>
#include <TopoDS_Shape.hxx>
#include <Transfer_Process.hxx>
#include <Transfer_SimpleBinderOfTransient.hxx>
#include <Transfer_TransientProcess.hxx>

#include <TransferBRep.hxx>
#include <TransferBRep_ShapeBinder.hxx>
#include <TransferBRep_ShapeMapper.hxx>
#include <TransferBRep_ShapeListBinder.hxx>

static void  ShapeAppend
  (const Handle(Transfer_Binder)& binder,
   const Handle(TopTools_HSequenceOfShape)& shapes)
{
  if (binder.IsNull()) return;
  if (binder->IsKind(STANDARD_TYPE(TransferBRep_ShapeBinder))) {
    DeclareAndCast(TransferBRep_ShapeBinder,shbind,binder);
    if (shbind->HasResult()) shapes->Append (shbind->Result());
  }
  else if (binder->IsKind(STANDARD_TYPE(TransferBRep_ShapeListBinder))) {
    DeclareAndCast(TransferBRep_ShapeListBinder,slbind,binder);
    Standard_Integer i,nb = slbind->Result().Length();
    for (i = 1; i <= nb; i ++) shapes->Append (slbind->Result().Value(i));
  }
  else if (binder->IsKind(STANDARD_TYPE(Transfer_SimpleBinderOfTransient))) {
    DeclareAndCast(Transfer_SimpleBinderOfTransient,trbind,binder);
    DeclareAndCast(TopoDS_HShape,hs,trbind->Result());
    if (!hs.IsNull()) shapes->Append (hs->Shape());
  }
  Handle(Transfer_Binder) nextr = binder->NextResult();
  if (!nextr.IsNull()) ShapeAppend (nextr,shapes);
}


TopoDS_Shape TransferBRep::ShapeResult (const Handle(Transfer_Binder)& binder)
{
  TopoDS_Shape shape;
  Handle(Transfer_Binder) bnd = binder;
  while (!bnd.IsNull()) {
    DeclareAndCast(TransferBRep_ShapeBinder,shb,bnd);
    if (!shb.IsNull()) return shb->Result();
    DeclareAndCast(Transfer_SimpleBinderOfTransient,hsb,bnd);
    if (!hsb.IsNull()) {
      Handle(TopoDS_HShape) hsp = GetCasted(TopoDS_HShape,hsb->Result());
      if (!hsp.IsNull()) return hsp->Shape();
    }
    bnd = bnd->NextResult();
  }
  return shape;
}

TopoDS_Shape TransferBRep::ShapeResult (const Handle(Transfer_TransientProcess)& TP, const Handle(Standard_Transient)& ent)
{
  TopoDS_Shape shape;
  Handle(Transfer_Binder) binder = TP->Find(ent);
  if (binder.IsNull())  binder = GetCasted (Transfer_Binder,ent);
  if (!binder.IsNull()) return TransferBRep::ShapeResult (binder);
  DeclareAndCast(TopoDS_HShape,hsp,ent);
  if (!hsp.IsNull()) return hsp->Shape();
  return shape;
}

void TransferBRep::SetShapeResult (const Handle(Transfer_TransientProcess)& TP, const Handle(Standard_Transient)& ent, const TopoDS_Shape& theShape)
{
  if (theShape.IsNull() || ent.IsNull() || TP.IsNull()) return;
  TP->Bind (ent,new TransferBRep_ShapeBinder(theShape));
}

Handle(TopTools_HSequenceOfShape) TransferBRep::Shapes (const Handle(Transfer_TransientProcess)& TP, const Standard_Boolean roots)
{
  Handle(TopTools_HSequenceOfShape) shapes;
  if (TP.IsNull()) return shapes;
  shapes = new TopTools_HSequenceOfShape();

  Transfer_TransientProcess::Iterator list = (roots ? TP->RootResult() : TP->CompleteResult());
  for (list.Start(); list.More(); list.Next()) {
    const Handle(Transfer_Binder) &binder = list.Value();
    ShapeAppend (binder,shapes);
  }
  return shapes;
}

Handle(TopTools_HSequenceOfShape) TransferBRep::Shapes (const Handle(Transfer_TransientProcess)& TP, const Handle(TColStd_HSequenceOfTransient)& list)
{
  Handle(TopTools_HSequenceOfShape) shapes;
  if (TP.IsNull() && list.IsNull()) return shapes;
  shapes = new TopTools_HSequenceOfShape();

  Standard_Integer ie, ne = list->Length();
  for (ie = 1; ie <= ne; ie ++) {
    Handle(Transfer_Binder) binder = TP->Find(list->Value(ie));
    ShapeAppend (binder,shapes);
  }
  return shapes;
}

Handle(TransferBRep_ShapeMapper) TransferBRep::ShapeMapper (const Handle(Transfer_Process)& FP, const TopoDS_Shape& theShape)
{
  Handle(TransferBRep_ShapeMapper) mapper = new TransferBRep_ShapeMapper(theShape);
  const Standard_Integer index = FP->MapIndex (mapper);
  if (index == 0) return mapper;
  return Handle(TransferBRep_ShapeMapper)::DownCast(FP->Mapped(index));
}

Standard_CString TransferBRep::TypeName (const TopoDS_Shape &theShape)
{
  if (theShape.IsNull()) return "TopoDS_Shape";
  switch (theShape.ShapeType()) {
    case TopAbs_VERTEX     : return "TopoDS_Vertex";
    case TopAbs_EDGE       : return "TopoDS_Edge";
    case TopAbs_WIRE       : return "TopoDS_Wire";
    case TopAbs_FACE       : return "TopoDS_Face";
    case TopAbs_SHELL      : return "TopoDS_Shell";
    case TopAbs_SOLID      : return "TopoDS_Solid";
    case TopAbs_COMPSOLID  : return "TopoDS_CompSolid";
    case TopAbs_COMPOUND   : return "TopoDS_Compound";
    default : break;
  }
  return "TopoDS_Shape";
}
