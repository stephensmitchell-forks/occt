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


#include <BRep_Builder.hxx>
#include <BRepTools.hxx>
#include <Geom_Geometry.hxx>
#include <Interface_CheckIterator.hxx>
#include <Interface_InterfaceModel.hxx>
#include <Interface_Macros.hxx>
#include <IFSelect_Vars.hxx>
#include <IFSelect_CheckCounter.hxx>
#include <Message.hxx>
#include <Message_Messenger.hxx>
#include <ShapeExtend_Explorer.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <TCollection_AsciiString.hxx>
#include <TColStd_HSequenceOfTransient.hxx>
#include <TColStd_SequenceOfInteger.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopTools_HSequenceOfShape.hxx>
#include <Transfer_Binder.hxx>
#include <Transfer_Finder.hxx>
#include <Transfer_FinderProcess.hxx>
#include <Transfer_ResultFromModel.hxx>
#include <Transfer_ResultFromTransient.hxx>
#include <Transfer_SimpleBinderOfTransient.hxx>
#include <Transfer_TransientListBinder.hxx>
#include <Transfer_TransientProcess.hxx>
#include <TransferBRep.hxx>
#include <TransferBRep_ShapeBinder.hxx>
#include <TransferBRep_ShapeListBinder.hxx>
#include <TransferBRep_ShapeMapper.hxx>
#include <XSControl_Controller.hxx>

#include <IFSelect_WorkSession.hxx>
#include <XSControl_Writer.hxx>
#include <XSSelect_ConnectedShapes.hxx>
#include <XSDRAW.hxx>
#include <XSDRAW_SelectFunctions.hxx>
#include <XSDRAW_ShapeFunctions.hxx>
#include <XSDRAW_SessionPilot.hxx>

#include <stdio.h>

//=======================================================================
//function : XSControl_tpdraw
//purpose  : 
//=======================================================================
static Handle(IFSelect_Vars) GetXSControlVars (const Handle(XSDRAW_SessionPilot)& pilot)
{
  static const Handle(IFSelect_Vars) avars;
  const Handle(IFSelect_WorkSession) &WS = pilot->Session();
  return (WS.IsNull()? avars : WS->Vars());
}

//  ######################################################################
//  ####                                                              ####
//  ####                           COMMANDS                           ####
//  ####                                                              ####
//  ######################################################################

//=======================================================================
//function : XSControl_tpdraw
//purpose  : 
//=======================================================================
static Interface_ReturnStatus XSControl_tpdraw
  (const Handle(XSDRAW_SessionPilot)& pilot)
{
  Standard_Integer argc = pilot->NbWords();
  const Standard_CString arg1 = pilot->Arg(1);
  const Standard_CString arg2 = pilot->Arg(2);
  const Standard_CString arg3 = pilot->Arg(3);
  const Handle(Transfer_TransientProcess) &TP = XSDRAW::Session(pilot)->ReaderProcess();
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  if (TP.IsNull()) { sout<<"No Transfer Read"<<endl; return Interface_RetError;}
  //        ****    tpdraw        ****
  if (argc < 2) {
    sout<<"Donner [mode facultatif : item ou root] , NUMERO , nom DRAW facultatif"<<endl;
    sout<<"  mode si present : item ou root, sinon n0 d entite modele"<<endl;
    sout<<"  NUMERO entier : d entite, d item transfert ou de root transfert\n"
      <<  "    ou * pour dire tous"<<endl;
    return Interface_RetError;
  }
  Standard_Integer mode = 0, num=0;
  if      (arg1[0] == 'i') mode = 1;
  else if (arg1[0] == 'r') mode = 2;
  Standard_Boolean tout = Standard_False;
  if (mode == 0) {
    if (argc < 2) { sout<<"Donner au moins un NUMERO ou *"<<endl; return Interface_RetError; }
    if (arg1[0] == '*') tout = Standard_True;
    else num = XSDRAW_SelectFunctions::GiveEntityNumber(XSDRAW::Session(pilot),arg1);
  } else {
    if (arg2[0] == '*') tout = Standard_True;
    else num = XSDRAW_SelectFunctions::GiveEntityNumber(XSDRAW::Session(pilot),arg2);
  }

  Standard_Integer nbvar = 0;
  Handle(Transfer_Binder) binder;
  Handle(Standard_Transient) ent;
  TopoDS_Shape sh;    char nomvar[40];
  //  Standard_Boolean moderoot =  (pilot->Word(0).Value(3) == 'r');

  Standard_Integer n1, n2, i, max=0, index=0;
  Handle(Interface_InterfaceModel) model = TP->Model();
  if (model.IsNull()) {
    if (mode == 0) {
      sout<<"Pas de modele, preciser n0 d item de transfert"<<endl;
      return Interface_RetError;
    }
  }
  if (mode == 0) { sout<<"Entite de modele";    max = model->NbEntities(); }
  if (mode == 1) { sout<<"Item de transfert";   max = TP->NbMapped(); }
  if (mode == 2) { sout<<"Racine de transfert"; max = TP->NbRoots(); }
  if (tout) {
    n1 = 1;  n2 = max;
    sout<<", listage de 1 a "<<max<<endl;
  }
  else if (num <= 0 || num > max) {
    sout<<" - Num="<<num<<" hors limite (de 1 a "<<max<<")"<<endl;
    return Interface_RetError;
  } else {
    n1 = n2 = num;  nbvar = -1;  // nbvar : 1ere shape simple = pas de n0
    sout<<", n0 "<<num<<endl;
  }

  for (i = n1; i <= n2; i ++) {
    if        (mode == 0) {
      ent = model->Value(i);
      num = i;
      index = TP->MapIndex(ent);
    } else if (mode == 1) {
      ent = TP->Mapped(i);
      if (model.IsNull()) num = 0;
      else num = model->Number(ent);
      index = i;
    } else if (mode == 2) {
      ent = TP->Root(i);
      if (model.IsNull()) num = 0;
      else num = model->Number(ent);
      index = TP->MapIndex(ent);
    }

    if (index > 0) binder = TP->MapItem (index);
    if (binder.IsNull()) index = 0;
    if (index == 0) {
      if (!tout) sout<<"Entite n0 "<<num<<" : non repertoriee"<<endl;
      continue;
    }
    if (!binder->HasResult()) {
      if (!tout) sout<<"Entite n0 "<<num<<" : pas de resultat"<<endl;
      continue;
    }
    sh = TransferBRep::ShapeResult (binder);
    //DeclareAndCast(TransferBRep_ShapeBinder,shb,binder);
    if (!sh.IsNull()) {
      //sh = shb->Result();
      nbvar ++;
      if (sh.IsNull()) { sout<<" (no Shape recorded)"<<endl; continue; }
      if (tout) sout<<"[ "<<i<<" ]:";
      if (num == 0) sout<<" pas dans le modele";
      else sout<<" ent.n0 "<<num;
      sout<<", item transfert n0 "<<index;
      if (nbvar == 0) {
	if      (argc > 3 && mode  > 0) sprintf (nomvar,"%s",arg3);
	else if (argc > 2 && mode == 0) sprintf (nomvar,"%s",arg2);
	else                            sprintf (nomvar,"tp_%d",i);
      } else {
	if      (argc > 3 && mode  > 0) sprintf (nomvar,"%s_%d",arg3,nbvar);
	else if (argc > 2 && mode == 0) sprintf (nomvar,"%s_%d",arg2,nbvar);
	else                            sprintf (nomvar,"tp_%d",i);
      }
      sout<<" -> 1 DRAW Shape: "<<nomvar<<endl;
      GetXSControlVars(pilot)->SetShape(nomvar,sh);
      continue;
    }
    DeclareAndCast(TransferBRep_ShapeListBinder,slb,binder);
    if (!slb.IsNull()) {
      const Standard_Integer nbs = slb->Result().Length();
      if (tout) sout<<"[ "<<i<<" ]:";
      if (num == 0) sout<<" pas dans le modele";
      else sout<<" ent.n0 "<<num;
      sout<<", item transfert n0 "<<index;
      sout<<" -> "<<nbs<<" DRAW Shapes :";
      for (Standard_Integer j = 1; j <= nbs; j ++)  {
        sh = slb->Result().Value(j);  if (nbvar < 0) nbvar = 0;  nbvar ++;
        if (sh.IsNull()) { sout<<" (no Shape recorded)"<<endl; continue; }
        if      (argc > 3 && mode  > 0) sprintf (nomvar,"%s_%d",arg3,nbvar);
        else if (argc > 2 && mode == 0) sprintf (nomvar,"%s_%d",arg2,nbvar);
        else                        sprintf (nomvar,"tp_%d_%d",i,nbvar);
        sout<<" "<<nomvar;
        GetXSControlVars(pilot)->SetShape(nomvar,sh);
      }
      sout<<endl;
      continue;
    }
    DeclareAndCast(Transfer_SimpleBinderOfTransient,trb,binder);
    if (!trb.IsNull()) {
      Handle(Standard_Transient) resu = trb->Result();
      if (resu.IsNull()) {
	sout<<"Entite n0 "<<num<<" : pas de resultat"<<endl;
	continue;
      }
      DeclareAndCast(Geom_Geometry,geom,resu);
      sout<<"Entite n0 "<<num<<" : resultat "<<resu->DynamicType()->Name();
      if (geom.IsNull()) { sout<<endl; continue; }
      nbvar ++;
      if (nbvar == 0) {
	if      (argc > 3 && mode  > 0) sprintf (nomvar,"%s",arg3);
	else if (argc > 2 && mode == 0) sprintf (nomvar,"%s",arg2);
	else                            sprintf (nomvar,"tp_%d",i);
      } else {
	if      (argc > 3 && mode  > 0) sprintf (nomvar,"%s_%d",arg3,nbvar);
	else if (argc > 2 && mode == 0) sprintf (nomvar,"%s_%d",arg2,nbvar);
	else                            sprintf (nomvar,"tp_%d",i);
      }
      char* nomv = nomvar;
      GetXSControlVars(pilot)->Set (nomv,geom);
      sout<<" -> DRAW Geom : "<<nomvar<<endl;
      continue;
    }

    if (sh.IsNull() && trb.IsNull())
      if (!tout) sout<<"Entite n0 "<<num<<" : resultat pas une Shape mais "<<binder->ResultTypeName()<<endl;
  }

  if (sh.IsNull()) sout<<" (No Shape)"<<endl;
  return Interface_RetDone;
}

//=======================================================================
//function : XSControl_tpcompound
//purpose  : 
//=======================================================================
static Interface_ReturnStatus XSControl_tpcompound
  (const Handle(XSDRAW_SessionPilot)& pilot)
{
  Standard_Integer argc = pilot->NbWords();
  const Standard_CString arg1 = pilot->Arg(1);
  const Handle(Transfer_TransientProcess) &TP = XSDRAW::Session(pilot)->ReaderProcess();
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  if (TP.IsNull()) { sout<<"No Transfer Read"<<endl; return Interface_RetError;}
  //        ****    tpcompound        ****
  if (argc < 2) { sout<<"Give a NAME for the Compound  + optional givelist, else roots are taken"<<endl; return Interface_RetError; }
  Handle(TopTools_HSequenceOfShape) list;
  if (argc == 2) list = TransferBRep::Shapes(TP);
  else {
    Handle(TColStd_HSequenceOfTransient) lise = XSDRAW_SelectFunctions::GiveList(pilot->Session(),pilot->CommandPart(2));
    if (lise.IsNull()) { sout<<"Not a valid entity list : "<<pilot->CommandPart(2)<<endl; return Interface_RetError; }
    list = TransferBRep::Shapes (TP,lise);
    sout<<lise->Length()<<" Entities, ";
  }
  if (list.IsNull()) { sout<<"No Shape listed"<<endl; return Interface_RetError; }
  Standard_Integer nb = list->Length();
  sout<<nb<<" Shape(s) listed"<<endl;
  TopoDS_Compound C;
  BRep_Builder B;
  B.MakeCompound(C);
  for (Standard_Integer i = 1; i <= nb; i ++)  B.Add (C,list->Value(i));
  GetXSControlVars(pilot)->SetShape (arg1,C);
  return Interface_RetDone;
}



//=======================================================================
//function : XSControl_traccess
//purpose  : 
//=======================================================================
static Interface_ReturnStatus XSControl_traccess
  (const Handle(XSDRAW_SessionPilot)& pilot)
{
  Standard_Integer argc = pilot->NbWords();
  const Standard_CString arg1 = pilot->Arg(1);
  const Standard_CString arg2 = pilot->Arg(2);
  //        ****    trdraw : ReaderProcess        **** 26
  //        ****    trsave : ReaderProcess        **** 27
  //        ****    trcomp  (comp -> DRAW)         **** 28
  //        ****    trscomp (comp -> save)         **** 29
  Standard_Boolean cascomp = (pilot->Word(0).Location(1,'o',1,5) > 0);
  Standard_Boolean cassave = (pilot->Word(0).Location(1,'s',1,5) > 0);
  char nomsh[100], noms[100];
  const Handle(Transfer_TransientProcess) &TP = XSDRAW::Session(pilot)->ReaderProcess();
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  if (TP.IsNull()) { sout<<" manque init"<<endl; return Interface_RetError; }
  const Handle(Interface_InterfaceModel) &mdl = TP->Model();
  if (mdl.IsNull()) { sout<<" modele absent"<<endl; return Interface_RetError; }
  Standard_Integer num = (argc > 1 ? XSDRAW_SelectFunctions::GiveEntityNumber(XSDRAW::Session(pilot),arg1) : 0);

  if (argc > 1) strcpy (nomsh,arg1);
  else    strcpy (nomsh,(cascomp ? "TREAD_COMP" : "TREAD_LIST"));
  if (cassave) sout<<" save shapes -> current directory"<<endl;

  if (num == 0 || cascomp) {
    TopoDS_Compound C;        // pour cas compound
    BRep_Builder B;
    B.MakeCompound(C);

    Handle(TopTools_HSequenceOfShape) list = new TopTools_HSequenceOfShape();
    Handle(TColStd_HSequenceOfTransient) li = TP->RecordedList();
    Standard_Integer i, nb = mdl->NbEntities();
    TopoDS_Shape sh;
    for (i = 1; i <= nb; i ++) {
      sh = TP->ShapeResult (mdl->Value(i));
      if (!sh.IsNull()) list->Append(sh);
    }

    nb = list->Length();
    sout<<" TOUS RESULTATS par ShapeResultList, soit "<<nb<<endl;
    for (i = 1; i <= nb; i ++) {
      sprintf (noms,"%s_%d",nomsh,i);
      if      ( (i%1000) == 0) sout<<"("<<i<<")"<<endl;
      else if ( (i%100)  == 0) sout<<"*";
      else if ( (i%10)   == 0) sout<<"0";
      else                     sout<<".";
      if (list->Value(i).IsNull()) continue;
      if      (!cascomp && !cassave) GetXSControlVars(pilot)->SetShape(noms,list->Value(i));
      else if (!cascomp &&  cassave) BRepTools::Write (list->Value(i),noms);
      else if (cascomp) B.Add (C,list->Value(i));
    }
    sout<<endl;
    if      (cascomp && !cassave) GetXSControlVars(pilot)->SetShape(nomsh,C);
    else if (cascomp &&  cassave) BRepTools::Write (C,nomsh);
  } else {
    if (num < 1 || num > mdl->NbEntities()) { sout<<" incorrect:"<<arg1<<endl; return Interface_RetError; }
    TopoDS_Shape sh = TP->ShapeResult(mdl->Value(num));
    if (sh.IsNull()) { sout<<" Pas de resultat pour "<<arg1<<endl; return Interface_RetError; }
    if (argc > 2) sprintf (nomsh,"%s",arg2);
    else sprintf (nomsh,"TREAD_%d",num);
    if      (!cascomp && !cassave) GetXSControlVars(pilot)->SetShape(nomsh,sh);
    else if (!cascomp &&  cassave) BRepTools::Write (sh,nomsh);
    else sout<<"Option non comprise"<<endl;
  }
  return Interface_RetDone;
}

//=======================================================================
//function : XSControl_IsEqualSubShape
//purpose  : 
//=======================================================================
// PTV 23.08.2000 Added for checking where are an entity from.
static Standard_Boolean XSControl_IsEqualSubShape (const TopoDS_Shape& Shape,
                                                   TopoDS_Shape& sh, Standard_Integer aLevel)
{
  if ( sh.IsSame(Shape) ) return Standard_True;
  if (aLevel > 0) {
    TopoDS_Shape varShape;
    aLevel--;
    TopoDS_Iterator it(sh);
    for (; it.More(); it.Next() ) {
      varShape = it.Value();
      if ( XSControl_IsEqualSubShape(Shape, varShape, aLevel) ) return Standard_True;
    }
  }
  return Standard_False; 
}

//=======================================================================
//function : XSControl_fromshape
//purpose  : 
//=======================================================================
static Interface_ReturnStatus XSControl_fromshape
  (const Handle(XSDRAW_SessionPilot)& pilot)
{
  Standard_Integer argc = pilot->NbWords();
  const Standard_CString arg1 = pilot->Arg(1);
  //        ****    fromshape (tread)         ****
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  if (argc < 2) {
    sout<<"Give name of a DRAW Shape"<<endl;
    return Interface_RetError;
  }
  const char* a1 = (char *)arg1;
  TopoDS_Shape Shape = GetXSControlVars(pilot)->GetShape(a1);
  if (Shape.IsNull()) {
    sout<<"Not a DRAW Shape:"<<arg1<<endl;
    return Interface_RetError;
  }
  Standard_Boolean yena = Standard_False;
  Standard_Integer aLevel = 1;
  if (argc >=3 )
    aLevel = atoi(pilot->Arg(2));
  Standard_Boolean silent = Standard_False;
  if (aLevel <0 ) {
    silent = Standard_True;
    aLevel = -aLevel;
  }

  //    IMPORT
  const Handle(Transfer_TransientProcess) &TP = XSDRAW::Session(pilot)->ReaderProcess();
  if (TP.IsNull()) { }
  else {
    yena = Standard_True;
    if ( ! silent ) sout<<"Shape "<<arg1<<" : ";
    Standard_Integer modrec = 1;
    Handle(Standard_Transient) ent = TP->EntityFromShapeResult (Shape,modrec);
    if (ent.IsNull()) {
      modrec = -1;
      ent = TP->EntityFromShapeResult (Shape,modrec);
    }
    if (ent.IsNull()) {
      modrec = 2;
      TopoDS_Shape S0 = Shape;
      TopLoc_Location L;
      S0.Location ( L );
      Standard_Integer i, nb = TP->NbMapped();
      if ( ! silent ) sout<<"searching in map among "<<nb<<" ...";
      for (i = 1; i <= nb; i ++) {
        ent = TP->Mapped(i);
        TopoDS_Shape sh = TransferBRep::ShapeResult(TP,ent);
        if (sh.IsNull()) {
          ent.Nullify();
          continue;
        }
        if (XSControl_IsEqualSubShape(Shape, sh, aLevel)) break;
        modrec = -2;
        sh.Location ( L );
        if (XSControl_IsEqualSubShape(S0, sh, aLevel)) break;
        ent.Nullify();
        modrec = 2;
      }
    }
    if ( ! ent.IsNull() ) {
      if ( silent ) sout << "Shape " << arg1 << ": ";
      if (modrec <0) sout<<"(moved from origin) ";
    }
    //  on affiche
    if (ent.IsNull()) {
      if ( ! silent ) sout<<" unknown as imported";
      // skl 11.05.2004
      // if Shape is a compound try to make "fromshape" for its subshapes
      if(Shape.ShapeType()==TopAbs_COMPOUND) {
        sout<<endl<<"Subshapes imported from entities:";
        TopoDS_Iterator Iter(Shape);
        for (; Iter.More(); Iter.Next()) {
          TopoDS_Shape subsh = Iter.Value();
          Handle(Standard_Transient) subent = TP->EntityFromShapeResult(subsh,1);
          if (subent.IsNull())
            subent = TP->EntityFromShapeResult(subsh,-1);
          if (!subent.IsNull()) {
            sout<<"  "<<XSDRAW::Session(pilot)->Model()->Number(subent); 
          }
        }
      }
    }
    else { 
      sout<<"imported from entity "; 
      XSDRAW::Session(pilot)->Model()->Print(ent,sout); 
      if ( silent ) sout << " in file " << XSDRAW::Session(pilot)->LoadedFile() << endl;
    }
    if ( ! silent ) sout<<endl;
  }

  //   ET EN EXPORT ?
  const Handle(Transfer_FinderProcess) &FP = XSDRAW::Session(pilot)->WriterProcess();
  if (FP.IsNull()) { }
  else {
    yena = Standard_True;
    Handle(Transfer_Finder) fnd = TransferBRep::ShapeMapper (FP,Shape);
    Handle(Standard_Transient) ent;
    if (!fnd.IsNull()) ent = FP->FindTransient (fnd);
    if (!ent.IsNull()) {
      sout<<"Shape "<<arg1<<": exported to entity ";
      XSDRAW::Session(pilot)->Model()->Print(ent,sout);
      if ( silent ) sout << " in file " << XSDRAW::Session(pilot)->LoadedFile();
      sout<<endl;
    }
    // abv 31.08.00: treat case of splitted shape (several results)
    // it is supposed that results are of the same type and lie in one-level comp
    else {
      Handle(Transfer_Binder) bnd = FP->Find ( fnd );
      if ( ! bnd.IsNull() ) {
	Handle(Transfer_TransientListBinder) TransientListBinder =
	  Handle(Transfer_TransientListBinder)::DownCast( bnd ); //skl
	if (! TransientListBinder.IsNull() ) {
      Standard_Integer i = 1, nb = TransientListBinder->Result().Length();
	  if (nb > 0) sout<<"Shape "<<arg1<<": exported to entities ";
	  for (; i<=nb; i++) {
        XSDRAW::Session(pilot)->Model()->Print( TransientListBinder->Result().Value(i), sout );
	    if (i < nb) sout<<", ";
	  }
	  if (nb > 0) {
	    if ( silent ) sout << " in file " << XSDRAW::Session(pilot)->LoadedFile();
	    sout<<endl;
	  }
	}
      }
    }
  }
  if (!yena) sout<<"No transfer (either import or export) recorded"<<endl;

  return Interface_RetVoid;
}

//=======================================================================
//function : XSControl_trconnexentities
//purpose  : 
//=======================================================================
static Interface_ReturnStatus XSControl_trconnexentities
  (const Handle(XSDRAW_SessionPilot)& pilot)
{
  Standard_Integer argc = pilot->NbWords();
  const Standard_CString arg1 = pilot->Arg(1);
  //        ****    connected entities (last transfer)         ****
  const Handle(Transfer_TransientProcess) &TP = XSDRAW::Session(pilot)->ReaderProcess();
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  if (TP.IsNull()) { sout<<"no transfer map"<<endl; return Interface_RetVoid; }
  if (argc < 2) { 
    sout<<"Give name of a DRAW Shape + optional shape type v-e-w-f(D)-s"<<endl; 
    return Interface_RetError; 
  }
  const char* a1 = (const char *)arg1;
  TopoDS_Shape Shape = GetXSControlVars(pilot)->GetShape(a1);
  if (Shape.IsNull()) { sout<<"Not a DRAW Shape:"<<arg1<<endl; return Interface_RetError; }
  sout<<"Shape "<<arg1<<" : ";

  Handle(TColStd_HSequenceOfTransient) list =
    XSSelect_ConnectedShapes::AdjacentEntities (Shape,TP,TopAbs_FACE);
  Standard_Integer i, nb = list->Length();
  sout<<nb<<" Entities produced Connected Shapes :"<<endl;
  const Handle(Interface_InterfaceModel) &model = XSDRAW::Session(pilot)->Model();
  sout<<"(";
  for (i = 1; i <= nb; i ++) {
    if (i > 1) sout<<",";
    sout<<model->Number(list->Value(i));
  }
  sout<<")"<<endl;
  return Interface_RetDone;
}
  
//=======================================================================
//function : XSControl_trimport
//purpose  : 
//=======================================================================
static Interface_ReturnStatus XSControl_trimport
  (const Handle(XSDRAW_SessionPilot)& pilot)
{
  //  FileName ou . (pour courant)  VarName  GiveList (obligatoire)
  //    GiveList : * pour xst-transferrable-roots
  const Handle(IFSelect_WorkSession) &WS = pilot->Session();

  Standard_Integer argc = pilot->NbWords();
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  if (argc < 4) {
    sout<<"Give : filename or . for current model;  varname or . to take fileroot\n  GiveList, * for all transferrable roots"<<endl;
    return Interface_RetError;
  }
  const Standard_CString arg1 = pilot->Arg(1);
  const Standard_CString arg2 = pilot->Arg(2);
  const Standard_CString arg3 = pilot->Arg(3);

  //  File Name and Variable (root) Name

  TCollection_AsciiString fnom,rnom;
  Standard_Boolean modfic = XSDRAW_ShapeFunctions::FileAndVar
    (WS,arg1,arg2,"IMPORT",fnom,rnom);
  if (modfic) sout<<" File to read : "<<fnom<<endl;
  else        sout<<" Model taken from the session : "<<fnom<<endl;
  sout<<" -- Names of variables BREP-DRAW prefixed by : "<<rnom<<endl;

  //  keep the current command, because sub-commands will be called
  TCollection_AsciiString compart = pilot->CommandPart (3);

  //  Reading file if required

  if (modfic) {
    TCollection_AsciiString comload ("xload ");
    comload.AssignCat(arg1);
    Interface_ReturnStatus status = pilot->Execute(comload);
    if (status != Interface_RetDone)
      { sout<<"Abandon import"<<endl; return status; }
  } else {
    sout<<"Currently Loaded Model"<<endl;
  }

  //  Selecting Entities

  Handle(TColStd_HSequenceOfTransient)  list;
  if (arg3[0] == '*' && arg3[1] == '\0') {
    list = WS->GiveList ("xst-transferrable-roots");
    sout<<"All Transferrable Roots : ";
  } else {
    sout<<"List given by "<<compart.ToCString()<<" : ";
    list = WS->GiveList (compart.ToCString());
  }
  if (list.IsNull()) { sout<<"No list defined. Abandon"<<endl; return Interface_RetError; }
  Standard_Integer nbl = list->Length();
  sout<<"Nb entities selected : "<<nbl<<endl;

  //  Starting Transfer

  WS->InitTransferReader (0);
  WS->InitTransferReader (4);

  const Handle(Transfer_TransientProcess) &TP = WS->ReaderProcess();
  if (TP.IsNull()) { sout<<" init not done or failed"<<endl; return Interface_RetError; }

  //  Transferring
  Standard_Integer nbt = WS->TransferList(list);
  sout<<"Nb Entities Selected : "<<nbl<<" have given "<<nbt<<" results"<<endl;

  //  Filling VARS. one compound (trimpcomp) or one shape per ent (trimport)
  Standard_Boolean iscomp = (pilot->Arg(0)[5] == 'c');
  Standard_Integer nbs = 0;
  TopoDS_Shape sh;
  TopoDS_Compound C;
  BRep_Builder B;
  B.MakeCompound (C);
  const Handle(Interface_InterfaceModel) &mdl = TP->Model();
  if (mdl.IsNull()) { sout<<" modele absent"<<endl; return Interface_RetError; }
  for (Standard_Integer il= 1; il <= nbl; il ++) {
    Handle(Standard_Transient) ent = list->Value(il);
    sh = TP->ShapeResult(ent);
    if (sh.IsNull()) continue;
    nbs ++;
    if (iscomp) B.Add (C,sh);
    else {
      char nomsh[50];
      sprintf (nomsh,"%s_%d",rnom.ToCString(),nbs);
      GetXSControlVars(pilot)->SetShape(nomsh,sh);
    }
  }
  if (nbs == 0) sout<<"No Shape produced"<<endl;
  else if (nbs == 1) {
    sout<<"One Shape produced, named "<<rnom.ToCString()<<endl;
    GetXSControlVars(pilot)->SetShape(rnom.ToCString(),sh);
  } else if (iscomp) {
    sout<<"One compound made of "<<nbs<<" Shapes, named "<<rnom.ToCString()<<endl;
    GetXSControlVars(pilot)->SetShape(rnom.ToCString(),C);
  } else {  // several individual shapes
    sout<<nbs<<" Shapes, named "<<rnom.ToCString()<<"_1 to "<<rnom.ToCString()<<"_"<<nbs<<endl;
  }

  return Interface_RetDone;
}

//=======================================================================
//function : twmode
//=======================================================================
static Standard_Integer gWriteMode = 0;
static Interface_ReturnStatus XSControl_twmode(const Handle(XSDRAW_SessionPilot)& pilot)
{
  Standard_Integer argc = pilot->NbWords();
  const Standard_CString arg1 = pilot->Arg(1);
  //        ****    twmode         ****
  Handle(Transfer_Process::Actor) anActor = pilot->Session()->WriterProcess()->GetActor();
  if (anActor.IsNull())
  {
    const Handle(XSControl_Controller) &aCtl = pilot->Session()->NormAdaptor();
    if (aCtl.IsNull()) return Interface_RetError;

    anActor = aCtl->NewActorWrite();
    pilot->Session()->WriterProcess()->SetActor(anActor);
  }
  Standard_Integer modemin,modemax;
  const Standard_Boolean bset = anActor->TransferModeBounds(modemin,modemax);
  const Handle(Message_Messenger) &sout = Message::DefaultMessenger();
  if (bset) {
    sout<<"Write Mode : allowed values  "<<modemin<<" to "<<modemax<<endl;
    for (Standard_Integer modd = modemin; modd <= modemax; modd++)
      sout<<modd<<" : "<<anActor->TransferModeHelp(modd)<<endl;
  }
  sout<<"Write Mode : actual = "<<gWriteMode<<endl;
  if (argc <= 1) return Interface_RetVoid;
  Standard_Integer mod = atoi(arg1);
  sout<<"New value -> "<<arg1<<endl;
  gWriteMode = mod;
  if (bset && (gWriteMode < modemin || gWriteMode > modemax)) sout<<"Warning : this new value is not supported"<<endl;
  return Interface_RetDone;
}

//=======================================================================
//function : XSControl_twrite
//purpose  : 
//=======================================================================
static Interface_ReturnStatus XSControl_twrite
  (const Handle(XSDRAW_SessionPilot)& pilot)
{
  Standard_Integer argc = pilot->NbWords();
  const Standard_CString arg1 = pilot->Arg(1);
  //        ****    twrite         ****
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  if (argc < 2) { sout<<" donner nom de shape draw"<<endl; return Interface_RetError; }
  sout<<"Attention, on alimente le modele courant ..."<<endl;

  XSControl_Writer aWriter(XSDRAW::Session(pilot));
  // Shape
  for (Standard_Integer i = 1; i < argc; i ++) {
    const char* ai = (const char *)pilot->Arg(i);
    TopoDS_Shape Shape = GetXSControlVars(pilot)->GetShape(ai);
    if (Shape.IsNull()) { sout<<"pas un nom de shape draw:"<<arg1<<endl; continue; }
    sout<<"Pour Shape : "<<ai;
    Interface_ReturnStatus stat = aWriter.TransferShape(Shape,Standard_False);
    sout<<" Transfer Write Status = "<<stat<<endl;
  }
  pilot->Session()->ComputeGraph();
  // Transient ? (Geom) : ignore
  return Interface_RetDone;
}

//  ######################################################################
//  ####                                                              ####
//  ####                    Initialising Commands                     ####
//  ####                                                              ####
//  ######################################################################

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void XSDRAW_ShapeFunctions::Init ()
{
  static int initactor = 0;
  if (initactor) return;  initactor = 1;

  XSDRAW::SetGroup("DE: General");

  XSDRAW::AddFunc ("tpdraw","[mode:item or root]  num|*  [nomvar] Passes an ITEM to Shape Draw (Start or Result)",XSControl_tpdraw);

  XSDRAW::AddFunc ("tpcompound","name:cstring [givelist] : -> compound with Shapes Root or from givelist",XSControl_tpcompound);

  XSDRAW::AddFunc ("trdraw","results ->DRAW : all;  or num [name] : from ent.num -> DRAW [name/tread_num]",XSControl_traccess);
  XSDRAW::AddFunc ("trsave","results ->files : all;  or num [name] : from ent.num -> DRAW [name/tread_num]",XSControl_traccess);
  XSDRAW::AddFunc ("trcomp","results -> 1 compound -> DRAW + name optional",XSControl_traccess);
  XSDRAW::AddFunc ("trscomp","results -> 1 compound -> file + name optional",XSControl_traccess);

  XSDRAW::AddFunc ("fromshape","shape [level=1]: imported/exported entity (when known)",XSControl_fromshape);
  XSDRAW::AddFunc ("trconnexent","name of draw shape : entities -> connected shapes (when known)",XSControl_trconnexentities);

  XSDRAW::AddFunc ("trimport","filename or .  varname  givelist  -> 1 shape per entity",XSControl_trimport);
  XSDRAW::AddFunc ("trimpcomp","filename or .  varname  givelist -> one xcompound",XSControl_trimport);

  XSDRAW::AddFunc ("twmode","displays mode transfer write, + num  changes it",XSControl_twmode);
  XSDRAW::AddFunc ("twrite","shape : transfer write for this shape, AFTER newmodel !",XSControl_twrite);
}

//  ######################################################################
//  ####                                                              ####
//  ####                      Additional Methods                      ####
//  ####                                                              ####
//  ######################################################################


//=======================================================================
//function : FileAndVar
//purpose  : 
//=======================================================================

Standard_Boolean  XSDRAW_ShapeFunctions::FileAndVar
  (const Handle(XSControl_WorkSession)& session,
   const Standard_CString file, const Standard_CString var,
   const Standard_CString def,
   TCollection_AsciiString& resfile,   TCollection_AsciiString& resvar)
{
  Standard_Boolean iafic = Standard_True;
  resfile.Clear();  resvar.Clear();
  if (file)
    if ( file[0] == '\0' ||
        (file[0] == '.' && file[1] == '\0')) iafic = Standard_False;
  if (!iafic) resfile.AssignCat (session->LoadedFile());
  else        resfile.AssignCat (file);
 
  if (var && var[0] != '\0' && (var[0] != '.' || var[1] != '\0') )
    resvar.AssignCat (var);
  else if (resfile.Length() == 0) resvar.AssignCat (def);
  else {
    Standard_Integer nomdeb, nomfin;
    nomdeb = resfile.SearchFromEnd ("/");
    if (nomdeb <= 0) nomdeb = resfile.SearchFromEnd("\\");  // pour NT
    if (nomdeb <  0) nomdeb = 0;
    nomfin = resfile.SearchFromEnd (".");
    if (nomfin < nomdeb) nomfin = resfile.Length() + 1;
    resvar = resfile.SubString(nomdeb+1,nomfin-1);
  }
  return iafic;
}
