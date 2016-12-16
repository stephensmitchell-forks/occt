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


#include <IGESData_ColorEntity.hxx>
#include <IGESData_DirPart.hxx>
#include <IGESData_IGESReaderData.hxx>
#include <IGESData_IGESWriter.hxx>
#include <IGESData_IGESDumper.hxx>
#include <IGESData_LabelDisplayEntity.hxx>
#include <IGESData_LevelListEntity.hxx>
#include <IGESData_LineFontEntity.hxx>
#include <IGESData_ParamReader.hxx>
#include <IGESData_TransfEntity.hxx>
#include <IGESData_UndefinedEntity.hxx>
#include <IGESData_ViewKindEntity.hxx>
#include <Interface_Check.hxx>
#include <Interface_Macros.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_UndefinedContent.hxx>
#include <Message_Msg.hxx>
#include <Message_Messenger.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESData_UndefinedEntity,IGESData_IGESEntity)

// MGE 23/07/98
//=======================================================================
//function : IGESData_UndefinedEntity
//purpose  : 
//=======================================================================
IGESData_UndefinedEntity::IGESData_UndefinedEntity ()
{
  thecont = new Interface_UndefinedContent;
}


//  ....           (Re)definitions specifiques a UndefinedEntity           ....


//=======================================================================
//function : DefLineFont
//purpose  : 
//=======================================================================

IGESData_DefType IGESData_UndefinedEntity::DefLineFont () const
{
  Standard_Integer st = ((thedstat/4) & 3);
  if (st == 0) return IGESData_IGESEntity::DefLineFont();
  else if (st == 1) return IGESData_ErrorVal;
  else return IGESData_ErrorRef;
}


//=======================================================================
//function : DefLevel
//purpose  : 
//=======================================================================

IGESData_DefList IGESData_UndefinedEntity::DefLevel () const
{
  Standard_Integer st = ((thedstat/16) & 3);
  if (st == 0) return IGESData_IGESEntity::DefLevel();
  else if (st == 1) return IGESData_ErrorOne;
  else return IGESData_ErrorSeveral;
}


//=======================================================================
//function : DefView
//purpose  : 
//=======================================================================

IGESData_DefList IGESData_UndefinedEntity::DefView () const
{
  Standard_Integer st = ((thedstat/64) & 3);
  if (st == 0) return IGESData_IGESEntity::DefView();
  else if (st == 1) return IGESData_ErrorOne;
  else return IGESData_ErrorSeveral;
}


//=======================================================================
//function : DefColor
//purpose  : 
//=======================================================================

IGESData_DefType IGESData_UndefinedEntity::DefColor () const
{
  Standard_Integer st = ((thedstat/256) & 3);
  if (st == 0) return IGESData_IGESEntity::DefColor();
  else if (st == 1) return IGESData_ErrorVal;
  else return IGESData_ErrorRef;
}


//=======================================================================
//function : HasSubScriptNumber
//purpose  : 
//=======================================================================

Standard_Boolean IGESData_UndefinedEntity::HasSubScriptNumber () const
{
  Standard_Integer st = ((thedstat/1024) & 1);
  if (st == 0) return IGESData_IGESEntity::HasSubScriptNumber();
  else return Standard_False;
}


//   ReadDir verifie les donnees, s il y a des erreurs les note (status),
//   genere un nouveau DirPart sans ces erreurs, et appelle ReadDir de base


//=======================================================================
//function : ReadDir
//purpose  : 
//=======================================================================

Standard_Boolean IGESData_UndefinedEntity::ReadDir
  (const Handle(IGESData_IGESReaderData)& IR, IGESData_DirPart& DP,
   Handle(Interface_Check)& ach)
{
  // MGE 23/07/98
  // =====================================
  //Message_Msg Msg60 ("XSTEP_60");
  //Message_Msg Msg61 ("XSTEP_61");
  //Message_Msg Msg62 ("XSTEP_62");
  //Message_Msg Msg63 ("XSTEP_63");
  //Message_Msg Msg64 ("XSTEP_64");
  //Message_Msg Msg70 ("XSTEP_70");
  //Message_Msg Msg72 ("XSTEP_72");
  // =====================================

  Standard_Integer v[17]; Standard_Character res1[9],res2[9],lab[9],subs[9];
  Standard_Integer max = 2 * IR->NbRecords();    // valeur maxi pour DSectNum
  thedstat = 0;

  Handle(IGESData_IGESEntity) anent; Standard_Boolean iapb;
  DP.Values(v[0],v[1],v[2],v[3],v[4],v[5],v[6],v[7],v[8],v[9],v[10],
	    v[11],v[12],v[13],v[14],v[15],v[16],res1,res2,lab,subs);

  iapb = Standard_False;
  if (v[3] < -max) iapb = Standard_True;
  else if (v[3] < 0) {
    anent = GetCasted(IGESData_IGESEntity,IR->BoundEntity((-1-v[3])/2));
    if (!anent->IsKind(STANDARD_TYPE(IGESData_LineFontEntity))) iapb = Standard_True;
  }
  // Sending of message : Line Font Pattern field is incorrect.
  if (iapb) { 
    Message_Msg Msg60 ("XSTEP_60");
    ach->SendFail(Msg60);
    thedstat += 8;
    v[3] = 0;
  }

  iapb = Standard_False;
  if (v[4] < -max) iapb = Standard_True;
  else if (v[4] < 0) {
    anent = GetCasted(IGESData_IGESEntity,IR->BoundEntity((-1-v[4])/2));
    if (!anent->IsKind(STANDARD_TYPE(IGESData_LevelListEntity))) iapb = Standard_True;
  }

  // Sending of message : Level field is incorrect.
  if (iapb) { 
    Message_Msg Msg61 ("XSTEP_61");
    ach->SendFail(Msg61);
    thedstat += 32;
    v[3] = 0;
  }

  iapb = Standard_False;
  if (v[5] < 0 || v[5] > max) iapb = Standard_True;
  else if (v[5] > 0) {
    anent = GetCasted(IGESData_IGESEntity,IR->BoundEntity((1+v[5])/2));
    if (!anent->IsKind(STANDARD_TYPE(IGESData_ViewKindEntity))) iapb = Standard_True;
  }

  // Sending of message : View field is incorrect.
  if (iapb) { 
    Message_Msg Msg62 ("XSTEP_62");
    ach->SendFail(Msg62);
    thedstat += 128;
    v[5] = 0;
  }

  iapb = Standard_False;
  if (v[6] < 0 || v[6] > max) iapb = Standard_True;
  else if (v[6] > 0) {
    anent = GetCasted(IGESData_IGESEntity,IR->BoundEntity((1+v[6])/2));
    if (!anent->IsKind(STANDARD_TYPE(IGESData_TransfEntity))) iapb = Standard_True;
  }
 
  // Sending of message : Transformation Matrix field is incorrect
  if (iapb) { 
    Message_Msg Msg63 ("XSTEP_63");
    ach->SendFail(Msg63);
    thedstat |= 1;
    v[6] = 0;
  }

  iapb = Standard_False;
  if (v[7] < 0 || v[7] > max) iapb = Standard_True;
  else if (v[7] < 0) {
    anent = GetCasted(IGESData_IGESEntity,IR->BoundEntity((1+v[7])/2));
    if (!anent->IsKind(STANDARD_TYPE(IGESData_LabelDisplayEntity))) iapb = Standard_True;
  }

  // Sending of message : Label Display Entity  field is incorrect.
  if (iapb) { 
    Message_Msg Msg64 ("XSTEP_64");
    ach->SendFail(Msg64);
    thedstat |= 1;
    v[7] = 0;
  }

  iapb = Standard_False;
  if (v[14] < 0 || v[14] > max) iapb = Standard_True;
  else if (v[14] < 0) {
    anent = GetCasted(IGESData_IGESEntity,IR->BoundEntity((1+v[14])/2));
    if (!anent->IsKind(STANDARD_TYPE(IGESData_ColorEntity)))
      {  thedstat += 512; v[14] = 0;  }
  }
  
  // Sending of message : Color Number field is incorrect.
  if (iapb) { 
    Message_Msg Msg70 ("XSTEP_70");
    ach->SendFail(Msg70);
    thedstat += 512;
    v[14] = 0;
  }

  iapb = Standard_False;
  Standard_Integer i; // svv Jan11 2000 : porting on DEC
  for (i = 0; i < 8; i ++) {
    if (subs[i] == '\0') break;  // fin de ligne
    if (subs[i] != ' ' && (subs[i] < 48 || subs[i] > 57)) iapb = Standard_True;
  }

  // Sending of message : Entity Subscript Number field is incorrect.
  if (iapb) { 
    Message_Msg Msg72 ("XSTEP_72");
    ach->SendFail(Msg72);
    thedstat += 1024;
    for (i = 0; i < 8; i ++) subs[i] = ' ';
  }

//  ...  Fin de cette analyse : si necessaire on reconstruit DP  ...
  if (thedstat == 0) return Standard_True;
  else {
    DP.Init(v[0],v[1],v[2],v[3],v[4],v[5],v[6],v[7],v[8],v[9],v[10],
	     v[11],v[12],v[13],v[14],v[15],v[16],res1,res2,lab,subs);
    return Standard_False;
  }
}


//   Parametres indifferencies : assocs et props ignores

//=======================================================================
//function : ReadOwnParams
//purpose  : 
//=======================================================================

void IGESData_UndefinedEntity::ReadOwnParams
  (const Handle(IGESData_IGESReaderData)& /*IR*/, IGESData_ParamReader& PR)
{
  const Standard_Integer nb = PR.NbParams();
  thecont->Reservate(nb,nb);
  for (Standard_Integer i = 1; i <= nb; i ++) {
    Interface_ParamType ptyp = PR.ParamType(i);
    thecont->AddLiteral (ptyp,new TCollection_HAsciiString(PR.ParamValue(i)));
  }
  PR.SetCurrentNumber(nb+1);
}


//=======================================================================
//function : WriteOwnParams
//purpose  : 
//=======================================================================

void IGESData_UndefinedEntity::WriteOwnParams(IGESData_IGESWriter& IW) const
{
  Standard_Integer nb = thecont->NbParams();
  for (Standard_Integer i = 1; i <= nb; i ++) {
    Interface_ParamType ptyp = thecont->ParamType(i);
    if (ptyp == Interface_ParamVoid) IW.SendVoid();
    else if (thecont->IsParamEntity(i)) {
      DeclareAndCast(IGESData_IGESEntity,anent,thecont->ParamEntity(i));
      IW.Send(anent);
    }
    else IW.SendString (thecont->ParamValue(i));
  }
}


//=======================================================================
//function : OwnShared
//purpose  : 
//=======================================================================

void IGESData_UndefinedEntity::OwnShared (Interface_EntityIterator& theIter) const
{
  const Standard_Integer nb = thecont->NbParams();
  for (Standard_Integer i = 1; i <= nb; i ++) {
    if (thecont->IsParamEntity(i))
      theIter.GetOneItem (thecont->ParamEntity(i));
  }
}


//=======================================================================
//function : OwnDump
//purpose  : 
//=======================================================================

void IGESData_UndefinedEntity::OwnDump (const IGESData_IGESDumper &dumper, const Handle(Message_Messenger) &S, const Standard_Integer) const
{
  const Standard_Integer dstat = thedstat;
  if (dstat != 0)
    S << " --  Directory Entry Error Status = " << dstat << "  --" << endl;
  Handle(Interface_UndefinedContent) cont = UndefinedContent();
  const Standard_Integer nb = cont->NbParams();
  S << " UNDEFINED ENTITY ...\n" << nb
    <<" Parameters (WARNING : Odd Integer Values Interpreted as Entities)\n";
  for (Standard_Integer i = 1; i <= nb; i ++) {
    Interface_ParamType ptyp = cont->ParamType(i);
    if (ptyp == Interface_ParamVoid) S<<"	["<<i<<":Void]";
    else if (cont->IsParamEntity(i)) {
      DeclareAndCast(IGESData_IGESEntity,anent,cont->ParamEntity(i));
      S<<"	["<<i<<":IGES]=";  
      dumper.PrintDNum(anent,S);
    }
    else {  S<<"	["<<i<<"]=" << cont->ParamValue(i);  }
    if ( i == (i%5)*5) S << endl;
  }
  S << endl;
}
