/*
 Copyright (c) 1999-2014 OPEN CASCADE SAS

 This file is part of Open CASCADE Technology software library.

 This library is free software; you can redistribute it and/or modify it under
 the terms of the GNU Lesser General Public License version 2.1 as published
 by the Free Software Foundation, with special exception defined in the file
 OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
 distribution for complete text of the license and disclaimer of any warranty.

 Alternatively, this file may be used under the terms of Open CASCADE
 commercial license or contractual agreement.
*/

/* pdn PRO16162: do restart in order to restore after possible crash or wrong data
*/ 
/*rln 10.01.99 - transmission of define's into this file
*/ 
/**
*/ 

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include "recfile.ph"
#include <OSD_OpenFile.hxx>
#include "scanner.hpp"

/*    StepFile_Error.c

      Ce programme substitue au yyerror standard, qui fait "exit" (brutal !)
      une action plus adaptee a un fonctionnement en process :

      Affichage de la ligne qui a provoque l' erreur,
      Preparation d'un eventuel appel suivant (vu qu on ne fait plus exit),
      en pour le retour, on s'arrange pour lever une exception
      (c-a-d qu on provoque un plantage)

      Adaptation pour flex (flex autorise d avoir plusieurs lex dans un meme
      executable) : les fonctions et variables sont renommees; et la
      continuation a change
*/

void rec_debfile();
void rec_finfile();

/*   But de ce mini-programme : appeler yyparse et si besoin preciser un
     fichier d'entree
     StepFile_Error  redefinit yyerror pour ne pas stopper (s'y reporter)
*/

/*   Designation d'un fichier de lecture
    (par defaut, c'est l'entree standard)

    Appel :  iflag = stepread_setinput ("...") ou (char[] ...) ;
                     stepread_setinput ("") [longueur nulle] laisse en standard
     iflag retourne vaut 0 si c'est OK, 1 sinon
*/

void stepread_setinput(std::ifstream& stream, char* nomfic)
{
  if (strlen(nomfic) == 0) return;
  OSD_OpenStream(stream, nomfic, std::ios_base::in | std::ios_base::binary);
}

void stepread_endinput (std::ifstream& stream, char* nomfic)
{
  if (!stream) return;
  if (strlen(nomfic) == 0) return;
  stream.close();
}

/*  Lecture d'un fichier ia grammaire lex-yacc
    Appel : i = stepread() ;  i est la valeur retournee par yyparse
    (0 si OK, 1 si erreur)
*/
int stepread(std::istream* stream)
{
  int letat = 0;
  rec_debfile();
  yy::scanner scanner(stream);
  scanner.yyrestart(stream);
  yy::parser parser(&scanner);
  letat = parser.parse();
  rec_finfile();
  return letat;
}