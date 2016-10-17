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

// pdn 26.02.99 added initializing of compound in function OneShape
//:   gka 14.04.99: S4136: apply scaling

#include <BRep_Builder.hxx>
#include <Interface_Check.hxx>
#include <Interface_InterfaceModel.hxx>
#include <Interface_ShareFlags.hxx>
#include <Interface_Static.hxx>
#include <Message_ProgressSentry.hxx>
#include <ShapeExtend_Explorer.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Shape.hxx>
#include <Transfer_Binder.hxx>
#include <Transfer_TransientProcess.hxx>
#include <XSControl_Controller.hxx>
#include <XSControl_Reader.hxx>
#include <XSControl_WorkSession.hxx>

//=======================================================================
//function : XSControl_Reader
//purpose  : 
//=======================================================================

XSControl_Reader::XSControl_Reader ()
: therootsta(Standard_False)
{
  SetWS (new XSControl_WorkSession);
}


//=======================================================================
//function : SetNorm
//purpose  : 
//=======================================================================

Standard_Boolean XSControl_Reader::SetNorm (const Standard_CString norm)
{
  if (thesession.IsNull()) SetWS(new XSControl_WorkSession);
  Standard_Boolean stat = thesession->SelectNorm(norm);
  if (stat) {
    thesession->InitTransferReader(0);
    thesession->InitTransferReader(4);
  }
  return stat;
}


//=======================================================================
//function : SetWS
//purpose  : 
//=======================================================================

void XSControl_Reader::SetWS(const Handle(XSControl_WorkSession)& WS, const Standard_Boolean WithNewModel)
{
  therootsta = Standard_False;
  theroots.Clear();
  thesession = WS;
  //  Il doit y avoir un Controller ...  Sinon onverra plus tard (apres SetNorm)
  if (thesession->NormAdaptor().IsNull()) return;
  Handle(Interface_InterfaceModel) model = thesession->Model ();
  if (WithNewModel || model.IsNull())   model = thesession->NewModel ();
  thesession->InitTransferReader(0);
  thesession->InitTransferReader(4);
}


//=======================================================================
//function : ReadFile
//purpose  : 
//=======================================================================

Interface_ReturnStatus XSControl_Reader::ReadFile (const Standard_CString filename)
{
  Interface_ReturnStatus stat = thesession->ReadFile(filename);
  thesession->InitTransferReader(4);
  return stat;
}


//=======================================================================
//function : Model
//purpose  : 
//=======================================================================

Handle(Interface_InterfaceModel) XSControl_Reader::Model () const
{
  return thesession->Model();
}


//=======================================================================
//function : NbRootsForTransfer
//purpose  : 
//=======================================================================

Standard_Integer XSControl_Reader::NbRootsForTransfer ()
{
  if (!therootsta) {
    therootsta = Standard_True;
    Interface_ShareFlags sf (thesession->Graph());
    Standard_Integer i, nbr = sf.NbRoots();
    for (i = 1; i <= nbr; i ++) {
      //    on filtre les racines qu on sait transferer
      Handle(Standard_Transient) start = sf.Root(i);
      if (thesession->ReaderProcess()->GetActor()->Recognize(start)) theroots.Append(start);
    }
  }
  return theroots.Length();
}


//=======================================================================
//function : RootForTransfer
//purpose  : 
//=======================================================================

Handle(Standard_Transient) XSControl_Reader::RootForTransfer (const Standard_Integer num)
{
  Handle(Standard_Transient) voidroot;
  Standard_Integer nbr = NbRootsForTransfer();
  if (num < 1 || num > nbr) return voidroot;
  return theroots.Value(num);
}


//  ####        TRANSFERT        ####


//=======================================================================
//function : TransferOneRoot
//purpose  : 
//=======================================================================

Standard_Boolean XSControl_Reader::TransferOneRoot(const Standard_Integer num)
{
  return TransferEntity (RootForTransfer (num));
}


//=======================================================================
//function : TransferEntity
//purpose  : 
//=======================================================================

Standard_Boolean XSControl_Reader::TransferEntity (const Handle(Standard_Transient)& start)
{
  if (start.IsNull()) return Standard_False;
  const Handle(Transfer_TransientProcess) &TP = thesession->ReaderProcess();
  TP->BeginTransfer();
  if (TP->TransferOne (start) == 0) return Standard_False;
  TopoDS_Shape sh = TP->ShapeResult(start);
  theshapes.Append(sh);
  return Standard_True;
}


//=======================================================================
//function : TransferList
//purpose  : 
//=======================================================================

Standard_Integer XSControl_Reader::TransferList (const Handle(TColStd_HSequenceOfTransient)& list)
{
  if (list.IsNull()) return 0;
  Standard_Integer nbt = 0;
  Standard_Integer i, nb = list->Length();
  const Handle(Transfer_TransientProcess) &TP = thesession->ReaderProcess();
  TP->BeginTransfer();
  ClearShapes();
  ShapeExtend_Explorer STU;
  for (i = 1; i <= nb; i ++) {
    Handle(Standard_Transient) start = list->Value(i);
    if (TP->TransferOne (start) == 0) continue;
    TopoDS_Shape sh = TP->ShapeResult(start);
    if (STU.ShapeType(sh,Standard_True) == TopAbs_SHAPE) continue;  // nulle-vide
    theshapes.Append(sh);
    nbt ++;
  }
  return nbt;
}


//=======================================================================
//function : TransferRoots
//purpose  : 
//=======================================================================

Standard_Integer XSControl_Reader::TransferRoots ()
{
  NbRootsForTransfer();
  Standard_Integer nbt = 0;
  Standard_Integer i, nb = theroots.Length();
  const Handle(Transfer_TransientProcess) &TP = thesession->ReaderProcess();
   
  TP->BeginTransfer();
  ClearShapes();
  ShapeExtend_Explorer STU;
  Message_ProgressSentry PS ( TP->GetProgress(), "Root", 0, nb, 1 );
  for (i = 1; i <= nb && PS.More(); i ++,PS.Next()) {
    Handle(Standard_Transient) start = theroots.Value(i);
    if (TP->TransferOne (start) == 0) continue;
    TopoDS_Shape sh = TP->ShapeResult(start);
    if (STU.ShapeType(sh,Standard_True) == TopAbs_SHAPE) continue;  // nulle-vide
    theshapes.Append(sh);
    nbt ++;
  }
  return nbt;
}

//=======================================================================
//function : OneShape
//purpose  : 
//=======================================================================

TopoDS_Shape XSControl_Reader::OneShape () const
{
  TopoDS_Shape sh;
  Standard_Integer i,nb = theshapes.Length();
  if (nb == 0) return sh;
  if (nb == 1) return theshapes.Value(1);
  TopoDS_Compound C;
  BRep_Builder B;
  //pdn 26.02.99 testing S4133
  B.MakeCompound(C);
  for (i = 1; i <= nb; i ++)  B.Add (C,theshapes.Value(i));
  return C;
}
