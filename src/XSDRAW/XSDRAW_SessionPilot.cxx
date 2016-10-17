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

#include <XSDRAW_SessionPilot.hxx>

#include <Standard_Stream.hxx>
#include <TCollection_AsciiString.hxx>
#include <TColStd_HSequenceOfAsciiString.hxx>
#include <OSD_OpenFile.hxx>
#include <Message.hxx>
#include <Message_Messenger.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_InterfaceModel.hxx>
#include <Interface_Macros.hxx>
#include <IFSelect_Selection.hxx>
#include <IFSelect_SignCounter.hxx>
#include <IFSelect_WorkSession.hxx>
#include <XSDRAW_Vars.hxx>

#include <stdio.h>
IMPLEMENT_STANDARD_RTTIEXT(XSDRAW_SessionPilot,XSDRAW_Activator)

#define MAXWORDS 200
#define MAXCARS 1000

static TCollection_AsciiString nulword;

//#define DEBUG_TRACE

// Nb Maxi de words : cf thewords et method SetCommandLine

const Handle(XSDRAW_SessionPilot) & XSDRAW_SessionPilot::Pilot ()
{
  static Handle(XSDRAW_SessionPilot) gPilot;
  if (gPilot.IsNull())
    gPilot = new XSDRAW_SessionPilot;
  return gPilot;
}

XSDRAW_SessionPilot::XSDRAW_SessionPilot ()
: theprompt ("XSTEP-DRAW>"), thenbwords(0),
  thewords (0,MAXWORDS-1) , thewordeb (0,MAXWORDS-1),
  therecord (Standard_False), thenumrec (0)
{
  Add (1,"x");
  Add (1,"exit");
  Add (2,"?");
  Add (2,"xhelp");
  Add (3,"xcommand");
  Add (4,"xsource");
  Add (5,"xstep");
  Add (6,"xnew");

  Handle(IFSelect_WorkSession) WS = new IFSelect_WorkSession;
  WS->SetVars (new XSDRAW_Vars);
  thesession = WS;
}


    void  XSDRAW_SessionPilot::SetCommandLine
  (const TCollection_AsciiString& command)
{
  Standard_Integer lc = command.Length();
  if (lc > 200) cout<<" Commande TRES LONGUE : "<<lc<<" caracteres :"<<endl
    <<command.ToCString()<<endl;
  thecommand = command;
  if (thecommand.Value(lc) <= ' ')  {  thecommand.Remove(lc);  lc --;  }
  thenbwords = 0;
  Standard_Integer i, nc = 0;
  char unarg[MAXCARS];
  for (i = 1; i <= lc; i ++) {
    char val = command.Value(i);
    if (val <= ' ') {
      if (nc == 0) continue;
      if (thenbwords >= MAXWORDS) {  unarg[nc] = val;  nc ++;  continue;  }
      unarg[nc] = '\0';
      thewords(thenbwords).Clear();  thewords(thenbwords).AssignCat(unarg);
#ifdef DEBUG_TRACE
      cout<<"thewords("<<thenbwords<<") ="<<unarg<<endl;
#endif
      thenbwords ++; nc = 0;
      continue;
    }
    if (nc == 0) thewordeb.SetValue (thenbwords,i);
    if (nc > MAXCARS) {  cout<<"Arg."<<thenbwords<<" > "<<MAXCARS<<" car.s, tronque"<<endl; continue;  }
    unarg[nc] = val;  nc ++;
  }
  if (nc > 0) {
    unarg[nc] = '\0'; thewords(thenbwords).Clear();
    thewords(thenbwords).AssignCat(unarg);
#ifdef DEBUG_TRACE
    cout<<"thewords("<<thenbwords<<")="<<unarg<<endl<<" .. Fin avec thenbwords="<<thenbwords+1<<endl;
#endif
    thenbwords ++;
  }
  thenumrec = 0;
  theobjrec.Nullify();
}

    Standard_CString  XSDRAW_SessionPilot::CommandPart
  (const Standard_Integer numarg) const
{
  if (numarg <= 0) return thecommand.ToCString();
  if (numarg >= thenbwords) return "";
  return &(thecommand.ToCString())[thewordeb(numarg)-1];
}

    const TCollection_AsciiString&  XSDRAW_SessionPilot::Word
  (const Standard_Integer num) const 
      {  if (num < thenbwords) return thewords(num);  return nulword;  }

    Standard_CString  XSDRAW_SessionPilot::Arg
  (const Standard_Integer num) const 
      {  return Word(num).ToCString();  }

    Standard_Boolean  XSDRAW_SessionPilot::RemoveWord
  (const Standard_Integer num)
{
  if (num < 0 || num > thenbwords) return Standard_False;
  Standard_Integer i; // svv Jan11 2000 : porting on DEC
  for (i = num; i < thenbwords; i ++) {
    thewords(i).Clear();
    thewords(i).AssignCat(thewords(i+1).ToCString());
  }
  thewords(thenbwords).Clear();
  thenbwords --;
//  Et refaire thecommand. Si num = 0, on supprime le debut (facile)
  if (num == 0) {
    thecommand.Remove(1,thewordeb(1));
  } else {
//   Sinon, reconstituer, a partir des words
    thecommand.Clear();
    for (i = 0; i < thenbwords; i ++) {
      if (i > 0) thecommand.AssignCat(" ");
      thecommand.AssignCat(thewords(i));
    }
  }

  return Standard_True;
}

    Interface_ReturnStatus  XSDRAW_SessionPilot::RecordItem
  (const Handle(Standard_Transient)& item)
{
  theobjrec = item;
  return (item.IsNull() ? Interface_RetFail : Interface_RetDone);
}

//  #######################################################################
//  ########        CONTROLE D EXECUTION


    Interface_ReturnStatus  XSDRAW_SessionPilot::ReadScript
  (const Standard_CString file)
{
  FILE* fic; int lefic = 0;
  if (file != NULL && file[0] != '\0') {
    fic = OSD_OpenFile (file,"r");
    if (fic) lefic = 1;
    else { cout<<" ...   Script File "<<file<<" not found"<<endl; return Interface_RetFail; }
    cout << " ...   Reading Script File " << file << endl;
  }
  else fic = stdin;
  Interface_ReturnStatus stat = Interface_RetVoid;

  for (;;) {
    char ligne[100];
    if (!lefic) std::cout << theprompt.ToCString();
    ligne[0] = '\0';
    if (fgets(ligne,100,fic) == NULL
     || feof(fic) != 0)
    {
      break;
    }
    if (ligne[0] == '\0') continue;
//    On interprete cette commande
    TCollection_AsciiString command(ligne);
    if (lefic) cout<<file<<":"<<command;  // le return est dans la ligne ... !
    stat = Execute(command);
    if (stat == Interface_RetStop) break;
    if ((stat == Interface_RetError || stat == Interface_RetFail) && lefic)
      { cout << " ...   Error in Script File, abandon"<<endl;  break; }
  }
  if (!lefic) return Interface_RetStop;
  fclose(fic);
  cout<<"End of Reading Script File " << file << endl;
  if (stat == Interface_RetError || stat == Interface_RetFail) return stat;
  return Interface_RetVoid;        // fin fichier : depiler
}


//  On boucle sur la lecture jusqu a une commande de fin ou un EOF

    Interface_ReturnStatus  XSDRAW_SessionPilot::Perform ()
{
  Interface_ReturnStatus stat = Interface_RetVoid;
  if (thenbwords == 0) return stat;
  if (thewords(0).Value(1) == '#') return stat;  // commentaire

  theobjrec.Nullify();
//  Est-ce un nom ?

//  Commande pour un Acteur
  Handle(XSDRAW_Activator) actor;  Standard_Integer num;
  if (XSDRAW_Activator::Select(thewords(0).ToCString(),num,actor)) {
    stat = actor->Do(num,this);
//  Prise en compte des commandes a resultat
//  Ici, resultat non nomme;  Resultat nomme par commande x (plus loin)
    if (!theobjrec.IsNull()) {
      thesession->RemoveItem(theobjrec);  //// depannage ?
      Standard_Integer addws = thesession->AddItem(theobjrec);
      if (addws == 0) { cout<<"Could not add item to session, sorry"<<endl; return Interface_RetFail; }
    }

    if (stat == Interface_RetVoid || stat == Interface_RetDone) {
      if (therecord) thecomlist.Append(thecommand);
    }
    else if (stat == Interface_RetError) cout<<"Error in Command : "<<thecommand<<endl;
    else if (stat == Interface_RetFail) cout << "Execution Failure for : " <<thecommand<<endl;
    return stat;
  }
  cout << " Command : " << thewords(0) << " unknown" << endl;
  return Interface_RetError;    // pas reconnu donc incorrect
}

    Interface_ReturnStatus  XSDRAW_SessionPilot::Execute
  (const TCollection_AsciiString& command)
{
  SetCommandLine(command);
  return Perform();
}

    Interface_ReturnStatus  XSDRAW_SessionPilot::ExecuteCounter
  (const Handle(IFSelect_SignCounter)& counter, const Standard_Integer numword,
   const IFSelect_PrintCount mode)
{
  if (counter.IsNull()) return Interface_RetError;
  counter->Clear();
  if (NbWords() <= numword) counter->AddModel (thesession->Model());
  else {
//   on demande un givelist
    Handle(TColStd_HSequenceOfTransient) list = thesession->GiveList (CommandPart(numword));
    if (list.IsNull()) {
      cout<<"Nothing selected from : "<<CommandPart(numword)<<endl;
      return Interface_RetError;
    }
    counter->AddWithGraph (list,thesession->Graph());
  }
  counter->PrintList(Message::DefaultMessenger(),thesession->Model(),mode);
  return Interface_RetVoid;
}

    Standard_Integer  XSDRAW_SessionPilot::Number
  (const Standard_CString val) const
{
  Standard_Integer num = thesession->NumberFromLabel (val);
  if (num < 0)  cout<<" Label:"<<val<<" ->"<<-num<<" ent.s, refus"<<endl;
  return num;
}


//  #########################################################################
//  ########        ACTIONS SPECIFIQUES DU PILOTE

#define MAXCOMPERLINE  5
#define LENGTHFORCOM  15

    Interface_ReturnStatus  XSDRAW_SessionPilot::Do
  (const Standard_Integer number,
   const Handle(XSDRAW_SessionPilot)& session)
{
//                  Commandes Propres : x, exit, undo, redo, ?, help
  Interface_ReturnStatus  stat = Interface_RetVoid;
  Standard_Integer       argc = NbWords();
  const Standard_CString arg1 = Word(1).ToCString();
  Standard_Integer       modhelp = -1;
  switch (number) {
    case -1 :                                 //        ****     HELP-XSNEW
      modhelp = 1;
      cout<<"  --  Commands candidate for  xsnew  --"<<endl;
//  HELP : soit complet (par defaut)  soit limite a xsnew
    case  0 : {                               //        ****     HELP
      Handle(TColStd_HSequenceOfAsciiString) list;
//    Help complet : on donne la liste des commandes, sans plus (deja pas mal)
      if (thenbwords <= 1) {
	list = XSDRAW_Activator::Commands(modhelp);
	Standard_Integer nbcom = 0;
	Standard_Integer nb = list->Length();
	cout << " -- Liste des Commands Disponibles --"<<endl;
	for (Standard_Integer i = 1; i <= nb; i ++) {
	  const TCollection_AsciiString& uncom = list->Value(i);
	  Standard_Integer loncom = uncom.Length();
	  nbcom ++;
	  if (nbcom > MAXCOMPERLINE) { cout<<endl; nbcom = 1; }
	  cout<<" "<<uncom;
	  if (nbcom == MAXCOMPERLINE) continue;
	  for (Standard_Integer j = loncom; j < LENGTHFORCOM; j ++) cout<<" ";
	}
	if (nbcom > 0) cout<<endl;
	cout<<"\nhelp *  liste toutes les commandes avec un help sur chacune\n"
	  <<"help <com> liste la ou les commande debutant par <com>"
	  <<" avec un help sur chacune"<<endl;

//    Un Help particulier
      } else {
	if (thewords(1).IsEqual("*")) list = XSDRAW_Activator::Commands(modhelp);

	else list = XSDRAW_Activator::Commands(modhelp,thewords(1).ToCString());

	Standard_Integer nb = list->Length();
	for (Standard_Integer i = 1; i <= nb; i ++) {
	  Handle(XSDRAW_Activator) actor;  Standard_Integer num;
	  if (XSDRAW_Activator::Select
	      (list->Value(i).ToCString(),num,actor)) {
	    if (XSDRAW_Activator::Mode (list->Value(i).ToCString()) == 1)
	      cout<<"[xsnew name] ";
	    cout << list->Value(i) << "	: " << actor->Help(num) << endl;
	  }
	}
	if (nb == 0 && thenbwords > 1) cout<<" Command "<<Word(1)<<" unknown. "
	  << " help (without command) lists all the commands" << endl;
      }
      return Interface_RetVoid;
    }
    case  1 : return Interface_RetStop;        //        ****     Fin de session
    case  2 : {                               //        ****     HELP
      return Do(0,this);
    }
    case  3 : {                               //        ****     COMMAND
      if (argc < 2) { cout << "Donner une option :\n"
	 <<"a : analyse une ligne  r : toggle record mode\n"
	 <<"l : list recorded  c : clear  f nom : sauver dans fichier de nom"
	 << endl; return Interface_RetVoid; }
      switch (arg1[0]) {
        case 'a' : {                          //        ****    command analyse
	  cout<<"Command n0 " << number <<" : "<< session->CommandLine()<<endl;
	  cout<<"Nb Words : " << argc-2 << " :\n";
	  for (Standard_Integer i = 2; i < argc; i ++) {
	    cout << " Word." << i-1 << " : " << session->Word(i) <<endl;
	  }
	  break;
	}
	case 'c' : session->Clear();  break;  //        ****    command clear
	case 'f' : {
	  if (argc < 3) { cout<<"Donner nom de fichier"<<endl; return Interface_RetError; }
	  Standard_Integer nb = session->NbCommands();
	  if (nb == 0) { cout<<"Aucune commande enregistree"<<endl; break; }
	  cout << "Nb Commandes enregistrees : " << nb <<endl;
	  ofstream fout(Word(2).ToCString(),ios::out);
	  for (Standard_Integer i = 1; i <= nb; i ++)
	    fout<<session->Command(i)<<endl;
	  break;
	}
	case 'l' : {                          //        ****    command list
	  if (session->RecordMode()) cout<<"  -- Record Mode Actif"<<endl;
	  else                       cout<<"  -- Record Mode Inactif"<<endl;
	  Standard_Integer nb = session->NbCommands();
	  cout << "Nb Commandes enregistrees : " << nb << " :"<<endl;
	  for (Standard_Integer i = 1; i <= nb; i ++) {
	    cout<<"  "<<i<<"	"<<session->Command(i)<<endl;
	  }
	  break;
	}
	case 'r' : {                          //        ****    command record
	  Standard_Boolean mode = session->RecordMode();
	  if (mode) cout << " -- Record Mode a present Inactif" <<endl;
	  else      cout << " -- Record Mode a present Actif"   <<endl;
	  session->SetRecordMode(!mode);
	  break;
	}
	default  : cout << "Option de controle de commande non comprise"<<endl;
      }
      return Interface_RetVoid;
    }

    case  4 : {                               //        ****     FILE
      if (argc < 2) { cout<<"Donner nom de fichier"<<endl; return Interface_RetError; }
      return session->ReadScript
	(TCollection_AsciiString(session->Word(1)).ToCString());
//          On recopie la string parce que Word(1) change tout le temps !
    }

    case  5 : {                               //        ****     XSTEP
      if (argc < 2) {
	cout<<"xstep : prefixe neutre pour toute commande xstep-draw"<<endl
	  <<"xstep command args  equivaut a  command args"<<endl;
	return Do(2,this);
      } else {
	RemoveWord(0);
	return Perform();
      }
    }
    case  6 : {                               //        ****    XSNEW(variable)
      if (argc < 3) {
	cout<<"xsnew nomvar command [args]   creates an item"<<endl
	  <<"  nomvar : name of item (must be a new name) in the session"<<endl;
	return Do (-1,this);
      } else {

	theobjrec.Nullify();
	TCollection_AsciiString name = Word(1);
//  Le nom ne doit pas etre deja pris !
    if (thesession.IsNull()) { cout<<"Command with a Name and no Session defined !"<<endl; return Interface_RetFail; }
	RemoveWord(0);  RemoveWord(0);

//  Commande pour un Acteur
	Handle(XSDRAW_Activator) actor;  Standard_Integer num;
	if (XSDRAW_Activator::Select(thewords(0).ToCString(),num,actor)) {
	  theobjrec.Nullify();
	  stat = actor->Do(num,this);
//  Prise en compte des commandes a resultat
	  if (!theobjrec.IsNull()) {
	    thesession->RemoveItem(theobjrec);  //// depannage ?
	    Standard_Integer addws =
	      thesession->AddNamedItem(name.ToCString(),theobjrec);
	    theobjrec.Nullify();
	    if (addws == 0) { cout<<"Could not add named item:"<<name<<", sorry"<<endl; return Interface_RetFail; }
	  }
	  else cout<<"Remark : xsnew with name:"<<name<<" and no result"<<endl;

	  return stat;
	}
	cout << " Command : " << thewords(0) << " unknown" << endl;
	return Interface_RetError;    // pas reconnu donc incorrect
      }
    }
    default : return Interface_RetError;
  }
}



    Standard_CString  XSDRAW_SessionPilot::Help
  (const Standard_Integer number) const
{
  switch (number) {
    case  1 : return "exit ou x : Fin de session";
    case  2 : return "Liste les commandes. ? <titre> : commandes debutant par <titre>";
    case  3 : return "controle de commande. command tout court pour help complet";
    case  4 : return "lit les commandes depuis un fichier";
    case  5 : return "prefixe neutre pour xstep-draw";
    case  6 : return "creation item : donner nom_item puis commande args";
    default : return "";
  }
}
