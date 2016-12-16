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


#include <gp_GTrsf.hxx>
#include <IGESData_ColorEntity.hxx>
#include <IGESData_IGESEntity.hxx>
#include <IGESData_IGESType.hxx>
#include <IGESData_LabelDisplayEntity.hxx>
#include <IGESData_LevelListEntity.hxx>
#include <IGESData_LineFontEntity.hxx>
#include <IGESData_NameEntity.hxx>
#include <IGESData_SingleParentEntity.hxx>
#include <IGESData_TransfEntity.hxx>
#include <IGESData_ViewKindEntity.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_EntityList.hxx>
#include <Interface_InterfaceError.hxx>
#include <Interface_Macros.hxx>
#include <Standard_NoSuchObject.hxx>
#include <Standard_PCharacter.hxx>
#include <TCollection_HAsciiString.hxx>
#include <IGESData_DirChecker.hxx>
#include <Message_Msg.hxx>
#include <Interface_ParamList.hxx>
#include <Interface_FileParameter.hxx>
#include <IGESData_ParamCursor.hxx>

#include <stdio.h>
IMPLEMENT_STANDARD_RTTIEXT(IGESData_IGESEntity,MMgt_TShared)

#define ThisEntity  Handle(IGESData_IGESEntity)::DownCast(This())

namespace
{
  static const Standard_Integer IGESFlagAssocs  = 131072;
  static const Standard_Integer IGESFlagProps   = 262144;
  static const Standard_Integer IGESFourStatus  = 65535;
  static const Standard_Integer IGESStatusField = 15;
  static const Standard_Integer IGESShiftSubord = 4;
  static const Standard_Integer IGESShiftUse    = 8;
  static const Standard_Integer IGESShiftHier   = 12;
}

//=======================================================================
//function : IGESData_IGESEntity
//purpose  : Default constructor.
//=======================================================================
IGESData_IGESEntity::IGESData_IGESEntity()
: theDefLevel   (0),
  theStatusNum  (0),
  theLWeightNum (0),
  theLWeightVal (0.0),
  theSubScriptN (0)
{
  theRes1[0] = theRes2[0] = '\0';
}

//=======================================================================
//function : Clear
//purpose  : 
//=======================================================================
void IGESData_IGESEntity::Clear ()
{
//  Handle et DefSwitch
  theStructure.Nullify();
  theDefLineFont.SetVoid();  theLineFont.Nullify();
  theDefColor.SetVoid();     theColor.Nullify();
  theDefLevel   = 0;         theLevelList.Nullify();
  theView.Nullify();         theTransf.Nullify();      theLabDisplay.Nullify();
  theSubScriptN = 0;         theShortLabel.Nullify();

//  Autres Valeurs, Listes
  theStatusNum  = theLWeightNum = 0;
  theLWeightVal = 0.;
  theProps.Clear(); theAssocs.Clear();
}


//  #########################################################################
//  ....                Definition IGES : Directory Entry                ....

    IGESData_DefType IGESData_IGESEntity::DefLineFont () const
      {  return theDefLineFont.DefType();  }

    Standard_Integer IGESData_IGESEntity::RankLineFont () const
      {  return theDefLineFont.Value();  }

    Handle(IGESData_LineFontEntity) IGESData_IGESEntity::LineFont () const
      {  return GetCasted(IGESData_LineFontEntity,theLineFont);  }

    IGESData_DefList IGESData_IGESEntity::DefLevel () const
{
  if (theDefLevel >  0) return IGESData_DefOne;
  if (theDefLevel <  0) return IGESData_DefSeveral;
  return IGESData_DefNone;
}

    Handle(IGESData_LevelListEntity) IGESData_IGESEntity::LevelList () const
      {  return GetCasted(IGESData_LevelListEntity,theLevelList);  }


    IGESData_DefList IGESData_IGESEntity::DefView () const
{
  if (View().IsNull())    return IGESData_DefNone;
  if (View()->IsSingle()) return IGESData_DefOne;
  else                    return IGESData_DefSeveral;
}

    Handle(IGESData_ViewKindEntity) IGESData_IGESEntity::View () const
      {  return GetCasted(IGESData_ViewKindEntity,theView);  }

    Handle(IGESData_ViewKindEntity) IGESData_IGESEntity::SingleView () const
{
  Handle(IGESData_ViewKindEntity) nulvue;
  if (DefView() != IGESData_DefOne) return nulvue;
  return View();
}

    Handle(IGESData_ViewKindEntity) IGESData_IGESEntity::ViewList () const
{
  Handle(IGESData_ViewKindEntity) nulvue;
  if (DefView() != IGESData_DefSeveral) return nulvue;
  return View();
}


Handle(IGESData_TransfEntity) IGESData_IGESEntity::Transf () const
{  return GetCasted(IGESData_TransfEntity,theTransf);  }


Handle(IGESData_LabelDisplayEntity) IGESData_IGESEntity::LabelDisplay () const
{  return GetCasted(IGESData_LabelDisplayEntity,theLabDisplay);  }

// Status : un Integer pour BlankStatus,SubrodinateStatus,USeFlag,HierarchySt.
// Decoupage : 4 bits chacun (BlankStatus tout a droite, etc)

Standard_Integer IGESData_IGESEntity::BlankStatus () const
{  return (theStatusNum & IGESStatusField);  }

Standard_Integer IGESData_IGESEntity::SubordinateStatus () const
{  return ((theStatusNum >> IGESShiftSubord) & IGESStatusField);  }

Standard_Integer IGESData_IGESEntity::UseFlag () const
{  return ((theStatusNum >> IGESShiftUse) & IGESStatusField);  }

Standard_Integer IGESData_IGESEntity::HierarchyStatus () const
{  return ((theStatusNum >> IGESShiftHier) & IGESStatusField);  }

IGESData_DefType IGESData_IGESEntity::DefColor () const
{  return theDefColor.DefType();  }

Standard_Integer IGESData_IGESEntity::RankColor () const
{  return theDefColor.Value();  }

Handle(IGESData_ColorEntity) IGESData_IGESEntity::Color () const
{  return GetCasted(IGESData_ColorEntity,theColor);  }


//=======================================================================
//function : CResValues
//purpose  : 
//=======================================================================

Standard_Boolean IGESData_IGESEntity::CResValues (const Standard_CString res1, 
						  const Standard_CString res2) const
{
  Standard_Boolean res = Standard_False;
  Standard_PCharacter pres1, pres2;
  //
  pres1=(Standard_PCharacter)res1;
  pres2=(Standard_PCharacter)res2;
  //
  for (Standard_Integer i = 0; i < 8; i ++) {
    pres1[i] = theRes1[i]; 
    pres2[i] = theRes2[i];
    if (theRes1[i] > ' ' || theRes2[i] > ' ') {
      res = Standard_True;
    }
  }
  pres1[8] = '\0'; 
  pres2[8] = '\0';
  //
  return res;
}

Standard_Boolean IGESData_IGESEntity::HasSubScriptNumber () const
{  return (theSubScriptN >= 0);  }  // =0 nul mais defini, <0 absent

//  ....                (Re)Initialisation du Directory                 ....

void  IGESData_IGESEntity::InitTransf (const Handle(IGESData_TransfEntity)& ent)
{  theTransf = ent;  }

void  IGESData_IGESEntity::InitView (const Handle(IGESData_ViewKindEntity)& ent)
{  theView = ent;  }

void  IGESData_IGESEntity::InitLineFont (const Handle(IGESData_LineFontEntity)& ent, const Standard_Integer rank)
{  theDefLineFont.SetRank((ent.IsNull() ? rank : -1));  theLineFont = ent;  }

void  IGESData_IGESEntity::InitLevel (const Handle(IGESData_LevelListEntity)& ent, const Standard_Integer val)
{  theLevelList = ent;  theDefLevel = (ent.IsNull() ? val : -1);  }

void  IGESData_IGESEntity::InitColor (const Handle(IGESData_ColorEntity)& ent, const Standard_Integer rank)
{  theDefColor.SetRank((ent.IsNull() ? rank : -1));  theColor = ent;  }

void  IGESData_IGESEntity::InitStatus
  (const Standard_Integer blank,   const Standard_Integer subordinate,
   const Standard_Integer useflag, const Standard_Integer hierarchy)
{
  theStatusNum  = (theStatusNum & (!IGESFourStatus));
  theStatusNum += (blank       & IGESStatusField) |
                ((subordinate  & IGESStatusField) << IGESShiftSubord) |
                ((useflag      & IGESStatusField) << IGESShiftUse) |
                ((hierarchy    & IGESStatusField) << IGESShiftHier);
}

void  IGESData_IGESEntity::InitMisc
  (const Handle(IGESData_IGESEntity)& str,
   const Handle(IGESData_LabelDisplayEntity)& lab,
   const Standard_Integer weightnum)
{
  theStructure = str;  theLabDisplay = lab;
  if (theLWeightNum != 0)  theLWeightVal *= (weightnum/theLWeightNum);
  else if (weightnum == 0) theLWeightVal  = 0;
  theLWeightNum = weightnum;
}

//  ....                  Notions derivees importantes                  ....


//  SingleParent : ici on ne traite que l Associativity SingleParent
//  Pour considerer le partage implicite, il faut remonter au Modele ...

    Standard_Boolean IGESData_IGESEntity::HasOneParent () const
{
  return (NbTypedProperties(STANDARD_TYPE(IGESData_SingleParentEntity)) == 1);
}

    Handle(IGESData_IGESEntity) IGESData_IGESEntity::UniqueParent () const
{
  if (NbTypedProperties(STANDARD_TYPE(IGESData_SingleParentEntity)) != 1)
    Interface_InterfaceError::Raise ("IGESEntity : UniqueParent");
  else {
    DeclareAndCast(IGESData_SingleParentEntity,PP,
		   TypedProperty(STANDARD_TYPE(IGESData_SingleParentEntity)));
    return PP->SingleParent();
  }
  return this;  // ne rime a rien (cf exception) mais calme le compilateur
}


    gp_GTrsf IGESData_IGESEntity::Location () const
{
  if (!HasTransf()) return gp_GTrsf();    // Identite
  Handle(IGESData_TransfEntity) trsf = Transf(); 
  return (trsf.IsNull())? gp_GTrsf() : trsf->Value(); 
}

    gp_GTrsf IGESData_IGESEntity::VectorLocation () const
{
  if (!HasTransf()) return gp_GTrsf();    // Identite
//    Prendre Location et anuler TranslationPart
  gp_GTrsf loca = Transf()->Value();      // c-a-d Compoound
  loca.SetTranslationPart (gp_XYZ(0.,0.,0.));
  return loca;
}

    gp_GTrsf IGESData_IGESEntity::CompoundLocation () const
{
  gp_GTrsf loca = Location();
  if (!HasOneParent()) return loca;
  gp_GTrsf locp = UniqueParent()->CompoundLocation();
  loca.PreMultiply(locp);
  return loca;
}


    Standard_Boolean IGESData_IGESEntity::HasName () const
{
  if (HasShortLabel()) return Standard_True;
  return (NbTypedProperties(STANDARD_TYPE(IGESData_NameEntity)) == 1);
}

Handle(TCollection_HAsciiString) IGESData_IGESEntity::NameValue () const
{
  Handle(TCollection_HAsciiString) nom;  // au depart vide
  //   Question : concatene-t-on le SubScript ?  Oui, forme label(subscript)
  Standard_Integer nbname = NbTypedProperties(STANDARD_TYPE(IGESData_NameEntity));
  if (nbname == 0) {
    if (!HasShortLabel()) return nom;
    if (theSubScriptN < 0) return theShortLabel;
    char lenom[50];
    sprintf (lenom,"%s(%d)",theShortLabel->ToCString(),theSubScriptN);
    nom = new TCollection_HAsciiString (lenom);
  }
  else if (nbname > 0) {
    DeclareAndCast(IGESData_NameEntity,name,
		   TypedProperty(STANDARD_TYPE(IGESData_NameEntity), 1));
    nom = name->Value();
  }

  return nom;
}


//  ....            Listes d'infos Optionnelles (Assocs,Props)            ....

    Standard_Boolean IGESData_IGESEntity::ArePresentAssociativities () const
{
  if (!theAssocs.IsEmpty()) return Standard_True;
  return (theStatusNum & IGESFlagAssocs) != 0;
}

Standard_Integer IGESData_IGESEntity::NbAssociativities () const
{
  return (theAssocs.IsEmpty()? 0 : theAssocs.NbEntities());
}

Interface_EntityIterator IGESData_IGESEntity::Associativities () const
{
  Interface_EntityIterator iter;
  theAssocs.FillIterator(iter);
  return iter;
}

Standard_Integer IGESData_IGESEntity::NbTypedAssociativities (const Handle(Standard_Type)& atype) const
{  return theAssocs.NbTypedEntities(atype);  }

Handle(IGESData_IGESEntity) IGESData_IGESEntity::TypedAssociativity (const Handle(Standard_Type)& atype) const
{  return GetCasted(IGESData_IGESEntity,theAssocs.TypedEntity(atype));  }

void IGESData_IGESEntity::Associate (const Handle(IGESData_IGESEntity)& ent) const
{
  if (!ent.IsNull()) {
    Handle(IGESData_IGESEntity) aThis(this);
    ent->theAssocs.Append(aThis);
  }
}

void IGESData_IGESEntity::Dissociate (const Handle(IGESData_IGESEntity)& ent) const
{
  if (!ent.IsNull()) {
    Handle(IGESData_IGESEntity) aThis(this);
    ent->theAssocs.Remove(aThis);
  }
}

Standard_Boolean IGESData_IGESEntity::ArePresentProperties () const
{
  if (!theProps.IsEmpty()) return Standard_True;
  return (theStatusNum & IGESFlagProps) != 0;
}

Standard_Integer IGESData_IGESEntity::NbProperties () const
{
  return (theProps.IsEmpty()? 0 : theProps.NbEntities());
}

Interface_EntityIterator IGESData_IGESEntity::Properties () const
{
  Interface_EntityIterator iter;
  theProps.FillIterator(iter);
  return iter;
}

Standard_Integer IGESData_IGESEntity::NbTypedProperties (const Handle(Standard_Type)& atype) const
{  return theProps.NbTypedEntities(atype);  }

Handle(IGESData_IGESEntity) IGESData_IGESEntity::TypedProperty (const Handle(Standard_Type)& atype, const Standard_Integer anum) const
{  return GetCasted(IGESData_IGESEntity,theProps.TypedEntity(atype, anum));  }

void IGESData_IGESEntity::AddProperty (const Handle(IGESData_IGESEntity)& ent)
{  theProps.Append(ent);  }

void IGESData_IGESEntity::RemoveProperty (const Handle(IGESData_IGESEntity)& ent)
{  theProps.Remove(ent);  }

// ....                     Actions liees au Transfert                     ....

void IGESData_IGESEntity::SetLineWeight (const Standard_Real defw, const Standard_Real maxw, const Standard_Integer gradw)
{
  if (theLWeightNum == 0) theLWeightVal = defw;
  else if (gradw == 1) theLWeightVal = maxw*theLWeightNum;
  else theLWeightVal = (maxw*theLWeightNum)/gradw;
}

//=======================================================================
//function : OwnRead
//purpose  : 
//=======================================================================

#include <IGESData_IGESReaderTool.hxx>
#include <IGESData_IGESReaderData.hxx>
#include <IGESData_DirPart.hxx>
#include <IGESData_ParamReader.hxx>
#include <IGESData_UndefinedEntity.hxx>

Standard_Boolean IGESData_IGESEntity::OwnRead (IGESData_IGESReaderTool &theReader)
{
  const Standard_Integer entity_number = theReader.EntityNumber();

  Handle(IGESData_IGESReaderData) IR = theReader.ReaderData();

  Handle(Interface_Check) &ach = theReader.Check();

  IGESData_DirPart DP = IR->DirPart(entity_number);    // qui le copie ...

//  UndefinedEntity : une pre-analyse est faite
  DeclareAndCast(IGESData_UndefinedEntity,undent,this);
  /*szv_c1:if (!undent.IsNull())
    undent->ReadDir (igesdat,DP,ach);*/

  //! Reads directory part componants from file; DP is the litteral
  //! directory part, IR detains entities referenced by DP
  Standard_Integer v[17];
  Standard_Character nom[9], snum[9], theRes1[9], theRes2[9];

  DP.Values(v[0],v[1],v[2],v[3],v[4],v[5],v[6],v[7],v[8],v[9],v[10],v[11],
            v[12],v[13],v[14],v[15],v[16],theRes1,theRes2,nom,snum);

  //s1:this->InitTypeAndForm (v[0] , v[16]);

  Handle(IGESData_IGESEntity) fieldent, Structure, fieldlab;
  if (v[2] <  0) Structure  = GetCasted(IGESData_IGESEntity,IR->BoundEntity( (1-v[2])/2 ));

  Handle(IGESData_LineFontEntity) Lnf;
  if (v[3] <  0) {
    fieldent = GetCasted(IGESData_IGESEntity, IR->BoundEntity( (1-v[3])/2 ));
    Lnf = GetCasted(IGESData_LineFontEntity, fieldent);
    if (Lnf.IsNull()) {
      // Sending of message : Incorrect Line Font Pattern
      Message_Msg Msg29 ("XSTEP_29");
      Msg29.Arg(entity_number);
      Msg29.Arg(TypeNumber());
      ach->SendWarning(Msg29);
      this->theLineFont = fieldent;
    }
    else InitLineFont (Lnf);
  }
  else InitLineFont(Lnf,v[3]);    // ici Lnf Null

  Handle(IGESData_LevelListEntity) Lvs;
  if (v[4] <  0) {
    fieldent = GetCasted(IGESData_IGESEntity, IR->BoundEntity( (1-v[4])/2 ));
    Lvs = GetCasted(IGESData_LevelListEntity, fieldent);
    if (Lvs.IsNull()) {
      // Sending of message : Incorrect Line Font Pattern
      Message_Msg Msg30 ("XSTEP_30");
      Msg30.Arg(entity_number);
      Msg30.Arg(TypeNumber());
      ach->SendWarning(Msg30);
      this->theLevelList = fieldent;
    }
    else InitLevel(Lvs,-1);
  }
  else InitLevel(Lvs,v[4]);       // ici Lvs Null

  if (v[5] != 0) {
    fieldent = GetCasted(IGESData_IGESEntity,IR->BoundEntity( (1+v[5])/2 ));
    DeclareAndCast(IGESData_ViewKindEntity,View,fieldent);
    if (View.IsNull()) {
      // Sending of message : Incorrect View 
      Message_Msg Msg31 ("XSTEP_31");
      Msg31.Arg(entity_number);
      Msg31.Arg(TypeNumber());
      ach->SendWarning(Msg31);
      this->theView = fieldent;
    }
    else this->InitView(View);
  }

  if (v[6] != 0) {
    fieldent = GetCasted(IGESData_IGESEntity,IR->BoundEntity( (1+v[6])/2 ));
    DeclareAndCast(IGESData_TransfEntity,Transf,fieldent);
    if (Transf.IsNull()) {
      // Sending of message : Incorrect Transformation Matrix 
      Message_Msg Msg32 ("XSTEP_32");
      Msg32.Arg(entity_number);
      Msg32.Arg(TypeNumber());
      ach->SendWarning(Msg32);
      this->theTransf = fieldent;
    }
    else this->InitTransf(Transf);
  }

  Handle(IGESData_LabelDisplayEntity) Lbd;
  if (v[7] != 0) {
    fieldlab = GetCasted(IGESData_IGESEntity,IR->BoundEntity( (1+v[7])/2 ));
    Lbd = GetCasted(IGESData_LabelDisplayEntity,fieldent);
    if (Lbd.IsNull()) {
      // Sending of message : Incorrect Label Display 
      Message_Msg Msg33 ("XSTEP_33");
      Msg33.Arg(entity_number);
      Msg33.Arg(this->TypeNumber());
      ach->SendWarning(Msg33);
    }
  }

  this->InitStatus (v[8] , v[9] , v[10] , v[11]);

  Standard_Integer LWeightNum  = v[13];

  Handle(IGESData_ColorEntity) Color;
  if (v[14] <  0) {
    fieldent = GetCasted(IGESData_IGESEntity,IR->BoundEntity( (1-v[14])/2 ));
    Color = GetCasted(IGESData_ColorEntity, fieldent);
    if (Color.IsNull()) {
      // Sending of message : Incorrect Color Number 
      Message_Msg Msg34 ("XSTEP_34");
      Msg34.Arg(entity_number);
      Msg34.Arg(this->TypeNumber());
      ach->SendWarning(Msg34);
      this->theColor = fieldent;
    }
    else this->InitColor(Color);
  }
  else this->InitColor(Color,v[14]);

  this->InitMisc (Structure,Lbd,LWeightNum);
  this->theLabDisplay = fieldlab;

// ignores : 1(type),2(ptrPsect),13(type),16(lignesPsect),17(form)
// type et forme sont lus directement du DirPart; autres infos recalculees

//    Restent a analyser nom (short label) et snum (subscript number)
  Handle(TCollection_HAsciiString) ShortLabel;
  Standard_Integer SubScriptN = -1;
  Standard_Integer iacar = 0;
  Standard_Integer i; // svv Jan11 2000 : porting on DEC
  for (i = 0; i < 8; i ++) { if (nom[i] > ' ') iacar = 1;  }
  if (iacar > 0) ShortLabel = new TCollection_HAsciiString(nom);
  iacar = 0;
  for (i = 0; i < 8; i ++)
    {  if (snum[i] > ' ') iacar = 1; if(snum[i] == 0) break;  }
  if (iacar > 0) SubScriptN = atoi(snum);
  this->SetLabel(ShortLabel,SubScriptN);

//    Enfin, SetLineWeight, tenant compte du defaut
  SetLineWeight (IR->DefaultLineWeight(),theReader.MaxWeight(),theReader.GradWeight());

//   Liste de Parametres : controle de son entete
  const Standard_Integer nbpar = IR->NbParams(entity_number);
  const Standard_Integer n0par = (entity_number == 1 ? 1 : (IR->ParamFirstRank(entity_number-1) +1));
  if (nbpar < 1) {
//   Liste vide non admise, sauf si Undefined (par exemple type nul)
    if (!undent.IsNull()) return Standard_True;
    // Sending of message : DE : no parameter
    Message_Msg Msg27 ("XSTEP_27");
    Msg27.Arg(entity_number);
    ach->SendFail(Msg27);
    return Standard_False;
  }

  Handle(Interface_ParamList) thelist = IR->Params();
  const Interface_FileParameter& FP = thelist->Value(n0par);
  if ((FP.ParamType() != Interface_ParamInteger) || (atoi(FP.CValue()) != this->TypeNumber()))
  { 
    // Sending of message : DE : Incorrect type 
    Message_Msg Msg28 ("XSTEP_28");
    Msg28.Arg(entity_number);
    ach->SendFail(Msg28);  
    return Standard_False; 
  }

  IGESData_ParamReader PR (thelist,ach,n0par,nbpar,entity_number);

  IGESData_ReadStage thestep;

  OwnReadParams(IR,PR);

  if ((thestep = PR.Stage()) == IGESData_ReadOwn) PR.NextStage();
  if (thestep == IGESData_ReadEnd) {
    if (!PR.IsCheckEmpty()) ach = PR.Check();
    return (!ach->HasFailed());
  }

  Standard_Integer ncur, nbp;

  //! Reads Associativity List, if there is (if not, does nothing)
  //! criterium is : current parameter of PR remains inside params list, and Stage is "Own"
  //! Same conditions as above; in addition, no parameter must be let after the list once read
  //! Note that "Associated" entities are not declared "Shared"
  //Handle(Interface_Check) ach = new Interface_Check;
  Message_Msg Msg37 ("XSTEP_37");
  Msg37.Arg(entity_number);
  Msg37.Arg(TypeNumber());
  if (PR.Stage() != IGESData_ReadAssocs)
    ach->SendFail(Msg37);
  ncur = PR.CurrentNumber(); 
  nbp  = PR.NbParams();
  if (ncur == nbp + 1) PR.EndAll();
  else
  {
    if (ncur > nbp || ncur == 0)
	  ach->SendWarning(Msg37);

    if (PR.DefinedElseSkip()) {
      Standard_Integer nbassocs = 0;
      if (!PR.ReadInteger(nbassocs)) {
        Message_Msg Msg220 ("XSTEP_220");
        PR.SendFail(Msg220);
      }
      else if (nbassocs > 0) {
        Interface_EntityList assocs;
        if (PR.ReadEntList(IR,PR.CurrentList(nbassocs),Msg37,assocs,Standard_False)) {
          theAssocs = assocs;  theStatusNum = (theStatusNum | IGESFlagAssocs);
        }
      }
	}
  }

  if ((thestep = PR.Stage()) == IGESData_ReadAssocs) PR.NextStage();
  if (thestep == IGESData_ReadEnd) {
    if (!PR.IsCheckEmpty()) ach = PR.Check();
    return (!ach->HasFailed());
  }

  //! Reads Property List, if there is (if not, does nothing)
  //! criterium is : current parameter of PR remains inside params
  //! list, and Stage is "Own"
  //! Current parameter must be a positive integer, which value
  //! gives the length of the list; else, a Fail is produced (into
  //! Check of PR) and reading process is stopped
  //Handle(Interface_Check) ach = new Interface_Check;
  Message_Msg Msg38 ("XSTEP_38");
  Msg38.Arg(entity_number);
  Msg38.Arg(this->TypeNumber());
  if (PR.Stage() != IGESData_ReadProps)
    ach->SendFail(Msg38);
  ncur = PR.CurrentNumber();
  nbp  = PR.NbParams();
  if (ncur == nbp + 1) PR.EndAll();
  else {
    if (ncur > nbp || ncur == 0)
      ach->SendWarning(Msg38);

    if (PR.DefinedElseSkip()) {
      PR.SetCurrentNumber(ncur);
      Standard_Integer nbprops = 0;
      if (!PR.ReadInteger(nbprops)) {
        Message_Msg Msg221 ("XSTEP_221");
        PR.SendFail(Msg221);
      }
      else if (nbprops > 0) {
        Interface_EntityList props;
        if (PR.ReadEntList(IR,PR.CurrentList(nbprops),Msg38,props,Standard_False)) {
          theProps = props;    theStatusNum = (theStatusNum | IGESFlagProps);
        }
      }
    }
  }

  if (!PR.IsCheckEmpty()) ach = PR.Check();
  return (!ach->HasFailed());
}
  
//=======================================================================
//function : OwnWrite
//purpose  : 
//=======================================================================

void IGESData_IGESEntity::OwnWrite (IGESData_IGESWriter &IW) const
{
  OwnWriteParams(IW);
}

//=======================================================================
//function : DirChecker
//purpose  : 
//=======================================================================

IGESData_DirChecker IGESData_IGESEntity::DirChecker () const
{
  return IGESData_DirChecker();
};

//=======================================================================
//function : OwnCheck
//purpose  : 
//=======================================================================

void IGESData_IGESEntity::OwnCheck (const Interface_ShareTool &, const Handle(Interface_Check) &ach) const
{
  IGESData_DirChecker DC = DirChecker();

  /*if (DC.thetype != 0)
  {
    // Sending of message : Entity Type Number field is incorrect.
    if (TypeNumber() != DC.thetype) {
      Message_Msg Msg58 ("XSTEP_58");
      ach->SendFail(Msg58);
    }

    // Sending of message : Form Number field is incorrect.
    if (DC.theform1 <= DC.theform2) {
      if (FormNumber() < DC.theform1 || FormNumber() > DC.theform2) {
        Message_Msg Msg71 ("XSTEP_71");
        ach->SendFail(Msg71);
      }
    }
  }*/
 
  // Sending of message : Structure field is undefined.
  if (DC.thestructure == IGESData_DefReference && theStructure.IsNull()) {
    Message_Msg Msg59 ("XSTEP_59");
    ach->SendFail (Msg59);
  }

  if (DC.thegraphier == -1 || DC.thegraphier == HierarchyStatus()) { }
  else
  {
    IGESData_DefType df = DefLineFont();
   
    // Sending of message : Line Font Pattern field is incorrect
    if (df == IGESData_ErrorVal || df == IGESData_ErrorRef) {
      Message_Msg Msg60 ("XSTEP_60");
      ach->SendFail (Msg60);
    }
    else if (DC.thelinefont == IGESData_DefValue && df != IGESData_DefValue) {
      Message_Msg Msg60 ("XSTEP_60");
      ach->SendWarning (Msg60);
    }

    Standard_Integer dlw = LineWeightNumber();
    Message_Msg Msg69 ("XSTEP_69");
    // Sending of message : Line Weight Number is undefined.
    if (DC.thelineweig == IGESData_DefValue && dlw == 0) {
      ach->SendWarning (Msg69);
    }

    df = DefColor();

    // Sending of message : Color Number field is incorrect.
    if (df == IGESData_ErrorVal || df == IGESData_ErrorRef) {
      ach->SendFail (Msg69);
    }
    else if (DC.thecolor == IGESData_DefValue && df != IGESData_DefValue) {
      ach->SendWarning (Msg69);
    }
  }

  Standard_Integer st = BlankStatus();

  // Sending of message : Blank Status field is incorrect.
  if (st < 0 || st > 1) {
    Message_Msg Msg65 ("XSTEP_65");
    ach->SendFail (Msg65);
  }

  st = SubordinateStatus();
  
  // Sending of message : Subordinate Entity Switch field is incorrect.
  if (st < 0 || st > 3) {
    Message_Msg Msg66 ("XSTEP_66");
    ach->SendFail(Msg66);
  }

  st = UseFlag();

  // Send of message : Entity Use Flag is incorrect.
  if (st < 0 || st > 5) {
    Message_Msg Msg67 ("XSTEP_67");
    ach->SendFail(Msg67);
  }

  st = HierarchyStatus();

  //Sending of message : Hierarchy field is incorrect.
  if (st < 0 || st > 2) {
    Message_Msg Msg68 ("XSTEP_68");
    ach->SendFail(Msg68);
  }
}

//=======================================================================
//function : OwnCorrect
//purpose  : 
//=======================================================================

Standard_Boolean IGESData_IGESEntity::OwnCorrect ()
{
  IGESData_DirChecker DC = DirChecker();

  Standard_Boolean done = Standard_False;
  /*Standard_Integer type = TypeNumber();
  Standard_Integer form = FormNumber();
  if (DC.thetype != 0)
  {
    if (DC.theform1 >= 0 && DC.theform1 == DC.theform2 && DC.theform1 != form)
    {
      InitTypeAndForm (DC.thetype,DC.theform1);  done = Standard_True;
    }
    else if (DC.thetype != type)
    {
      InitTypeAndForm (DC.thetype,form);      done = Standard_True;
    }
  }*/

  Handle(IGESData_IGESEntity)      structure;    // par defaut Nulle
  if (DC.thestructure != IGESData_DefVoid) structure = Structure();
  Handle(IGESData_ViewKindEntity)  nulview;
  Handle(IGESData_LineFontEntity)  nulfont;
  Handle(IGESData_LevelListEntity) nulevel;
  Handle(IGESData_ColorEntity)     nulcolor;
  Handle(IGESData_LabelDisplayEntity) label;    // par defaut Nulle
  if (DC.thegraphier != -1) label = LabelDisplay();
  Standard_Integer linew = 0;
  if (DC.thegraphier != -1 && DC.thelineweig  != IGESData_DefVoid)
    linew = LineWeightNumber();

  if (DC.thegraphier == -1 || (RankLineFont() != 0 && DC.thelinefont  == IGESData_DefVoid) )
  {  InitLineFont(nulfont);    done = Standard_True;  }
  if (DC.thegraphier == -1 || (RankColor() != 0    && DC.thecolor     == IGESData_DefVoid) )
  {  InitColor(nulcolor);      done = Standard_True;  }
  if (DC.thegraphier == -1 && (!View().IsNull() || Level() != 0) )
  {  InitView(nulview);  InitLevel(nulevel);  done = Standard_True;  }
  if ((DC.thegraphier == -1 && (!LabelDisplay().IsNull() || LineWeightNumber() != 0)) || (!theStructure.IsNull() && DC.thestructure == IGESData_DefVoid) ) // combines :
  {  InitMisc (structure,label,linew);       done = Standard_True;  }

  Standard_Boolean force = Standard_False;
  Standard_Integer stb = BlankStatus();
  Standard_Integer sts = SubordinateStatus();
  Standard_Integer stu = UseFlag();
  Standard_Integer sth = HierarchyStatus();
  if (DC.theblankst  >= 0 && DC.theblankst != stb)
  {  force = Standard_True;  stb = DC.theblankst;   }
  if (DC.thesubordst >= 0 && DC.thesubordst != sts)
  {  force = Standard_True;  sts = DC.thesubordst;  }
  if (DC.theuseflag  >= 0 && DC.theuseflag != stu)
  {  force = Standard_True;  stu = DC.theuseflag;   }
  if (DC.thehierst   >= 0 && DC.thehierst != sth)
  {  force = Standard_True;  sth = DC.thehierst;  }
  if (force) {  InitStatus(stb,sts,stu,sth);      done = Standard_True;  }
  return done;
}
