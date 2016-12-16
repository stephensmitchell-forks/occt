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


#include <IGESData_DirPart.hxx>
#include <IGESData_GlobalSection.hxx>
#include <IGESData_IGESEntity.hxx>
#include <IGESData_IGESModel.hxx>
#include <IGESData_IGESReaderData.hxx>
#include <IGESData_IGESType.hxx>
#include <IGESData_UndefinedEntity.hxx>
#include <Interface_Check.hxx>
#include <Interface_FileParameter.hxx>
#include <Interface_Macros.hxx>
#include <Interface_ParamList.hxx>
#include <Interface_ParamSet.hxx>
#include <Interface_ParamType.hxx>
#include <Standard_Type.hxx>
#include <TCollection_HAsciiString.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <Message_Msg.hxx>

#include <OSD_OpenFile.hxx>

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////#include <igesread.h>

/*  structiges : */
struct parlist {
  struct oneparam *first, *last;
  int nbparam;
};

struct dirpart {
  int typ,poi,pdef,tra,niv,vue,trf,aff,blk,sub,use,her;  /* ligne 1 */
  int typ2,epa,col,nbl,form;                             /* ligne 2 */
  char res1[10],res2[10],nom[10],num[10];
  struct parlist list;                                   /* liste Psect */
  int numpart;                                           /* n0 en Dsect */
};

int igesread   (char* nomfic,int lesect[6],int modefnes);

#ifdef __cplusplus
extern "C" {
#endif

  /*  structiges : */
  int  iges_lirpart
   (int* *tabval,char* *res1,char* *res2,char* *nom,char* *num,int* nbparam);
  void iges_stats    (int* nbpart, int* nbparam);
  void iges_setglobal ();
  void iges_nextpart ();
  int  iges_lirparam (int* typarg,char* *parval);
  void iges_finfile  (int mode);
  struct dirpart *iges_get_curp (void);
  void iges_curpart (int dnum);

  void iges_initfile();
  void iges_newparam(int typarg,int longval, char *parval);
  void iges_param(int *Pstat,char *ligne,char c_separ,char c_fin,int lonlin);
  void iges_Psect(int numsec,char ligne[80]);

  /* MGE 20/07/98 */
  void IGESFile_Check2 (int mode,char * code, int num, char * str);
  void IGESFile_Check3 (int mode,char * code);

#ifdef __cplusplus
}
#endif

/*  Definition des types de parametres de l'analyseur de base IGES */
#define ArgVide 0
#define ArgQuid 1
#define ArgChar 2
#define ArgInt  3   /* Entier non signe : peut evoluer vers Real ou Char */
#define ArgSign 4   /* Entier signe : peut evoluer vers Real */
#define ArgReal 5
#define ArgExp  6   /* Real + lettre E : attendre confirmation */
#define ArgRexp 7   /* Real + Exposant : se ramene a Real */
#define ArgMexp 8   /* Real + Exposant INCOMPLET (pas de point decimal) */

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////#include <igesread.h>

//  Correspondance entre types igesread et types Interface_ParamFile ...
static Interface_ParamType LesTypes[10];

// decoupage interne pour faciliter les recuperations d erreurs
static Handle(Interface_Check)& checkread()
{
  static Handle(Interface_Check) chrd = new Interface_Check;
  return chrd;
}

void IGESFile_Check2 (int mode,char * code, int num, char * str)
{
  // MGE 20/07/98
  Message_Msg amsg (code);
  amsg.Arg(num);
  amsg.Arg(str);

  switch (mode)
   {
    case 0 : checkread()->SendFail (amsg); break;
    case 1 : checkread()->SendWarning (amsg); break;
    case 2 : checkread()->SendMsg (amsg); break;
    default : checkread()->SendMsg (amsg); 
   }
}

void IGESFile_Check3 (int mode,char * code)
{
  // MGE 20/07/98
  Message_Msg amsg (code);
  switch (mode)
   {
    case 0 : checkread()->SendFail (amsg); break;
    case 1 : checkread()->SendWarning (amsg); break;
    case 2 : checkread()->SendMsg (amsg); break;
    default : checkread()->SendMsg (amsg);
   }
}

/*    Lecture section D
      Chaque entite tient sur deux lignes

      Dstat demarre a zero "on lit une nouvelle entite"
      et bascule avec un   "deuxieme ligne de l'entite"
*/

/*      DECODAGE : parce que scanf ne fait pas vraiment ce qu'il faut     */
/*      Utilitaire decodant un nombre en format fixe dans une ligne
	Il part de "depuis" inclus (debut ligne = 0) et prend "tant" caracteres
	Valeur lue en retour de fonction   */
static bool IGES_decode (const char* theLine, const int theStart, const int theLength, int &theVal)
{
  static const int bases[] = { 1,10,100,1000,10000,100000,1000000, 10000000,100000000,1000000000 };
  const int s = theStart+theLength-1;
  theVal = 0;
  for (int i = 0; i < theLength; i ++)
  {
    const char c = theLine[s-i];
    switch (c) {
      case ' ': return true;
      case '-': theVal = -theVal;
      case '+':
      case '0': break;
      case '1': theVal += 1*bases[i]; break;
      case '2': theVal += 2*bases[i]; break;
      case '3': theVal += 3*bases[i]; break;
      case '4': theVal += 4*bases[i]; break;
      case '5': theVal += 5*bases[i]; break;
      case '6': theVal += 6*bases[i]; break;
      case '7': theVal += 7*bases[i]; break;
      case '8': theVal += 8*bases[i]; break;
      case '9': theVal += 9*bases[i]; break;
      default: return false;
    }
  }
  return true;
}

/*   Recopie d'une chaine de caracteres de longueur fixe (close par \0)  */
void IGES_copstr (const char *ligne, const int start, const int length, char* dans)
{
  for (int i = 0; i < length; i ++) { dans[i] = ligne[start+i]; }
  dans[length] = '\0';
}


IMPLEMENT_STANDARD_RTTIEXT(IGESData_IGESReaderData,MMgt_TShared)

//=======================================================================
//function : Contructor
//purpose  : 
//=======================================================================

IGESData_IGESReaderData::IGESData_IGESReaderData ()
: theparh (new Interface_ParamSet(30)),
  thechk (new Interface_Check),
  myFile(NULL)
{}

//=======================================================================
//function : Read
//purpose  : 
//=======================================================================

Standard_Integer IGESData_IGESReaderData::Read (const Standard_CString theFileName, const Standard_Boolean theModeFnes)
{
  //====================================
  Message_Msg Msg1("XSTEP_1");
  Message_Msg Msg15("XSTEP_15");
  //====================================

  // Sending of message : Beginning of the reading
  checkread()->SendMsg(Msg1);

  checkread()->Clear();

  //Init()
  {
  iges_initfile();

  myModeFnes = theModeFnes;

  myFile = stdin;
  if (theFileName[0] != '\0') 
    myFile = OSD_OpenFile(theFileName,"r");
  if (myFile == NULL) return -1; // File could not be opened

  for (int i = 0; i < sizeof(myBuffer); i++) myBuffer[i] = 0;

  // Prepare variables for start section
  Standard_Integer aNbStartLines = 0;
  myStartSection = new TColStd_HSequenceOfHAsciiString();
  }

  //int result = igesread((char*)theFileName,lesect,modefnes);
  //int igesread (char* nomfic, int lesect[6], int modefnes)
  //{
  static const char sects[] = " SGDPT ";

  char str[2];
  int Pstat = 0; char c_separ = ','; char c_fin = ';';
  int aNumLine = 0, numl = 0, i, i0 = 0;

  int lesect[6];
  for (i = 1; i < 6; i++) lesect[i] = 0;

  // Prepare variables for directory section
  struct dirpart {
    int typ,poi,pdef,tra,niv,vue,trf,aff,blk,sub,use,her;  // ligne 1
    int typ2,epa,col,nbl,form;                             // ligne 2
    char res1[10],res2[10],nom[10],num[10];
    struct parlist list;                                   // liste Psect
    int numpart;                                           // n0 en Dsect
  } curp_s;
  bool isDirLineEnd = false;

  for(;;) {
    numl ++;
    const Standard_Integer cur_section = ReadLine(aNumLine);
    if (cur_section <= 0 || cur_section < i0) {
      if (cur_section == 0) break;
      // Sending of message : Syntax error
      {
        str[1] = '\0';
        str[0] = sects[i0];
        IGESFile_Check2 (0,"XSTEP_18",numl,str);
      }
    
      if (i0 == 0) return -1;
      lesect[i0] ++;
      continue;
    }
    lesect[i]++;  i0 = cur_section;

    if (aNumLine != lesect[i]) {
      // Sending of message : Syntax error
      str[1] = '\0';
      str[0] = sects[i0];
      IGESFile_Check2 (0,"XSTEP_19",numl,str);
    }

    if (cur_section == 1) // Start Section (comm.)
    {
      // Skip ending spaces
      Standard_Integer j = 71;
      for (; j >= 0; j--)
        if (myBuffer[j] > 32) break;
      myBuffer[j+1] = '\0';
      // Add a start section line
	  if (j >= 0 || myStartSection->Length() > 0)
        myStartSection->Append (new TCollection_HAsciiString(myBuffer));
    }
    else if (cur_section == 2) // Header (Global sect)
    {
      iges_setglobal();
      for (;;) {
        // Record specific separators
        if (lesect[cur_section] == 1) {
          int n0 = 0;
          if (myBuffer[0] != ',') {  c_separ = myBuffer[2]; n0 = 3;  }
          if (myBuffer[n0+1] != c_separ) { c_fin = myBuffer[n0+3]; }
        }
        iges_param(&Pstat,myBuffer,c_separ,c_fin,72);
        if (Pstat != 2) break;
		/* TODO!!!!!!!!!!!!!!!!!!!!!!!!!
        char* parval;
        int typarg;
        while (iges_lirparam(&typarg,&parval) != 0)
          theparh->Append(parval,(int)strlen(parval),LesTypes[typarg],0);*/
      }
    }
    else if (cur_section == 3) // Directory  (Dsect)
    {
      struct dirpart *curp = &curp_s;
      if (!isDirLineEnd) {
        IGES_decode(myBuffer, 0,8,curp->typ);
        IGES_decode(myBuffer, 8,8,curp->poi);
        IGES_decode(myBuffer,16,8,curp->pdef);
        IGES_decode(myBuffer,24,8,curp->tra);
        IGES_decode(myBuffer,32,8,curp->niv);
        IGES_decode(myBuffer,40,8,curp->vue);
        IGES_decode(myBuffer,48,8,curp->trf);
        IGES_decode(myBuffer,56,8,curp->aff);
        IGES_decode(myBuffer,64,2,curp->blk);
        IGES_decode(myBuffer,66,2,curp->sub);
        IGES_decode(myBuffer,68,2,curp->use);
        IGES_decode(myBuffer,70,2,curp->her);
        isDirLineEnd = true;
      }
      else {
        IGES_decode(myBuffer, 0,8,curp->typ2);
        IGES_decode(myBuffer, 8,8,curp->epa);
        IGES_decode(myBuffer,16,8,curp->col);
        IGES_decode(myBuffer,24,8,curp->nbl);
        IGES_decode(myBuffer,32,8,curp->form);
        IGES_copstr(myBuffer,40,8,curp->res1);
        IGES_copstr(myBuffer,48,8,curp->res2);
        IGES_copstr(myBuffer,56,8,curp->nom);
        IGES_copstr(myBuffer,64,8,curp->num);
        isDirLineEnd = false;
        // Send directory to entity - TODO!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      }
    }
    else if (cur_section == 4) // Parametres (Psect)
    {
      //iges_Psect(aNumLine,myBuffer);
      // Lecture section P : preanalyse
      // Extraction du numero D et troncature a 64 caracteres
      //void iges_Psect (int numsec, char myBuffer[80])
      const int dnum = atoi(&myBuffer[65]); //directory number
      myBuffer[64] = '\0';
      iges_curpart(dnum);
      for (;;) {
        iges_param(&Pstat,myBuffer,c_separ,c_fin,64);
        if (Pstat != 2) break;
      }
    }
  }

  // Sending of message : No Terminal Section
  if (lesect[5] == 0)
    IGESFile_Check3 (1, "XSTEP_20");

  fclose (myFile);

  //return 0;
  //}
  //if (result != 0) return result;

//  Chargement des resultats dans un IGESReader

  LesTypes[ArgVide] = Interface_ParamVoid;
  LesTypes[ArgQuid] = Interface_ParamMisc;
  LesTypes[ArgChar] = Interface_ParamText;
  LesTypes[ArgInt]  = Interface_ParamInteger;
  LesTypes[ArgSign] = Interface_ParamInteger;
  LesTypes[ArgReal] = Interface_ParamReal;
  LesTypes[ArgExp ] = Interface_ParamMisc;  // exposant pas termine
  LesTypes[ArgRexp] = Interface_ParamReal;       // exposant complet
  LesTypes[ArgMexp] = Interface_ParamEnum;       // exposant mais pas de point

  int nbparts, nbparams;
  iges_stats(&nbparts,&nbparams);    // et fait les Initialisations necessaires

  //! creates IGESReaderData correctly dimensionned (for arrays)
  //! <nbe> count of entities, that is, half nb of directory lines
  //! <nbp> : count of parameters
  {
  const int nbe = (lesect[3]+1)/2;
  thenumpar = new TColStd_HArray1OfInteger(0,nbe);
  thenumpar->Init(0);
  theparams = new Interface_ParamSet (nbparams);

  theents = new TColStd_HArray1OfTransient(0,nbe);
  thedirs = new IGESData_HArray1OfDirPart(0,nbe);
  }

  try {
    OCC_CATCH_SIGNALS

    //  d abord les start lines (commentaires)
    char* parval;
    int typarg;
    /*while (iges_lirparam(&typarg,&parval) != 0) {
      Standard_Integer j; // svv Jan11 2000 : porting on DEC
      for (j = 72; j >= 0; j--)
        if (parval[j] > 32) break;
      parval[j+1] = '\0';
      if (j >= 0 || l > 0)
        thestar->Append (new TCollection_HAsciiString(parval));
      l++;
    }*/
    //  puis la Global Section
    iges_setglobal();
    while (iges_lirparam(&typarg,&parval) != 0)
      theparh->Append(parval,(int)strlen(parval),LesTypes[typarg],0);
    thehead.Init(theparh,thechk);
  }    // fin essai 1 (global)
  catch (Standard_Failure) {
    // Sending of message : Internal error during the header reading 
    const Message_Msg Msg11("XSTEP_11");
    checkread()->SendWarning (Msg11);
  }

  if (nbparts > 0)
  {
    /*static*/ Standard_Integer recupne = 0,recupnp = 0;  // pour affichage en cas de pepin

    char *res1, *res2, *nom, *num, *parval;
    int *v; int typarg;
    int nbparam;
    
    Standard_Integer nn=0;
    int ns;

    try {
      OCC_CATCH_SIGNALS

      while ( (ns = iges_lirpart(&v,&res1,&res2,&nom,&num,&nbparam)) != 0) {
        nn++;
        recupnp = 0;
        recupne = (ns+1)/2;  // numero entite
        thedirs->ChangeValue(recupne).Init(v[0],v[1],v[2],v[3],v[4],v[5],v[6],v[7],v[8],v[9],v[10],v[11],v[12],v[13],v[14],v[15],v[16],res1,res2,nom,num);
        while (iges_lirparam(&typarg,&parval) != 0) {
          recupnp ++;
          Standard_Integer nument = 0;
          if (typarg == ArgInt || typarg == ArgSign) {
            nument = atoi(parval);
            if (nument < 0) nument = -nument;
            if (nument & 1) nument = (nument+1)/2;
            else nument = 0;
          }
          theparams->Append(parval,-1,LesTypes[typarg],nument);
        }
        // Record current number of parameters
        thenumpar->SetValue(recupne,theparams->NbParams());
        iges_nextpart();
      }

      // Sending of message : Loaded data
    }    // fin essai 2 (entites)
    catch (Standard_Failure) {
      // Sending of message : Internal error during the content reading
      if (recupnp == 0) {
        Message_Msg Msg13("XSTEP_13");
        Msg13.Arg(recupne);
        checkread()->SendWarning (Msg13);
      }
      else {
        Message_Msg Msg14("XSTEP_14");
        Msg14.Arg(recupne);
        Msg14.Arg(recupnp);
        checkread()->SendWarning (Msg14);
      }
    }
  }
  
  Standard_Integer nbr = NbRecords();
  // Sending of message : Number of total loaded entities 
  Msg15.Arg(nbr);
  checkread()->SendMsg(Msg15);

  iges_finfile(1);

  /*
  // A present, le check
  // Nb warning in global section.
  Standard_Integer nbWarn = checkread()->NbWarnings(), nbFail = checkread()->NbFails();
  if (nbWarn + nbFail > 0) {
    const Handle(Interface_Check)& oldglob = amodel->GlobalCheck();
    checkread()->GetMessages (oldglob);
    amodel->SetGlobalCheck (checkread());
  }

  checkread()->Trace(0,1);

  iges_finfile(2);*/
 
  return 0;
}

const IGESData_DirPart & IGESData_IGESReaderData::DirPart (const Standard_Integer num) const
{ return thedirs->Value(num); }

Standard_Integer IGESData_IGESReaderData::NbRecords () const
{  return thedirs->Upper();/*thenumpar.Upper();*/  }

Standard_Integer IGESData_IGESReaderData::NbEntities () const
{  return thedirs->Upper();  }

Standard_Integer IGESData_IGESReaderData::FindNextRecord (const Standard_Integer num) const
{
  return (num >= thedirs->Upper())? 0 : (num + 1);
}

Standard_Integer IGESData_IGESReaderData::NbParams (const Standard_Integer num) const
{
  if (num > 1) return (thenumpar->Value(num) - thenumpar->Value(num-1));
  if (num ==1) return thenumpar->Value(num);
  return theparams->NbParams();
}

Standard_EXPORT Handle(Interface_ParamList) IGESData_IGESReaderData::Params () const
{
  return theparams->Params(0,0);  // complet
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////liriges.c

/*    A routine to read a line from an IGES File

   This routine returns:
   - status (function return): section no .: S, G, D, P, T (for 73) or
     0 (EOF) or -1 (jump stain)
   - a line number in the section (numbers 74 to 80)
   - the truncated line has 72 characters (0 in the 73rd)
   It is necessary to provide it a line buffer reserved to 81 characters
*/

Standard_Integer IGESData_IGESReaderData::ReadLine (Standard_Integer &theNumLine)
{
  if (theNumLine == 0)
    myBuffer[72] = myBuffer[79] = ' ';

  myBuffer[0] = '\0';

  if (myModeFnes)
  {
    if (fgets(myBuffer,99,myFile) == NULL) // kept for compatibility with fnes
      return 0;
  }
  else
  {
    while ( fgets(myBuffer,2,myFile) && ( myBuffer[0] == '\r' || myBuffer[0] == '\n' ) )
    {}
      
    if (fgets(&myBuffer[1],80,myFile) == NULL)
      return 0;
  }
    
  if (theNumLine == 0 && myBuffer[72] != 'S' && myBuffer[79] == ' ')
  {
    // CASE OF FNES : Skip the first line
    myBuffer[0] = '\0';
      
    if (myModeFnes)
    {
      if (fgets(myBuffer,99,myFile) == NULL) // kept for compatibility with fnes
        return 0;
    }
    else
    {
      while ( fgets(myBuffer,2,myFile) && ( myBuffer[0] == '\r' || myBuffer[0] == '\n' ) )
      {}

      if (fgets(&myBuffer[1],80,myFile) == NULL)
        return 0;
    }
  }

  if (myModeFnes && (myBuffer[0] & 128))
  {
    for (int i = 0; i < 80; i ++)
      myBuffer[i] = (char)(myBuffer[i] ^ (150 + (i & 3)));
  }

  if (feof(myFile))
    return 0;

  {//0x1A is END_OF_FILE for OS DOS and WINDOWS. For other OS we set this rule forcefully.
    char *fc = strchr(myBuffer, 0x1A);
    if(fc != 0)
    {
      fc[0] = '\0';
      return 0;
    }
  }

  if (myBuffer[0] == '\0' || myBuffer[0] == '\n' || myBuffer[0] == '\r')
    return ReadLine(theNumLine);

  int i, result;

  if (sscanf(&myBuffer[73],"%d",&result) != 0)
  {
    theNumLine = result;
    char typesec = myBuffer[72];
    switch (typesec)
    {
      case 'S' :  myBuffer[72] = '\0'; return (1);
      case 'G' :  myBuffer[72] = '\0'; return (2);
      case 'D' :  myBuffer[72] = '\0'; return (3);
      case 'P' :  myBuffer[72] = '\0'; return (4);
      case 'T' :  myBuffer[72] = '\0'; return (5);
      default  :;
    }
    // the column 72 is empty, try to check the neighbour
    if (strlen(myBuffer)==80 && (myBuffer[79]=='\n' || myBuffer[79]=='\r') && (myBuffer[0]<='9' && myBuffer[0]>='0'))
    {
       // check if the case of losted
       int index;
       for(index = 1; myBuffer[index]<='9' && myBuffer[index]>='0'; index++);
       if (myBuffer[index]=='D' || myBuffer[index]=='d') {
         for(index = 79; index > 0; index--)
           myBuffer[index] = myBuffer[index-1];
         myBuffer[0]='.';
       }
       char typesec = myBuffer[72];
       switch (typesec)
       {
         case 'S' :  myBuffer[72] = '\0'; return (1);
         case 'G' :  myBuffer[72] = '\0'; return (2);
         case 'D' :  myBuffer[72] = '\0'; return (3);
         case 'P' :  myBuffer[72] = '\0'; return (4);
         case 'T' :  myBuffer[72] = '\0'; return (5);
         default  :;
      }
    }
  }

  // the line does not conform to standard, try to read it (if there are some missing spaces)
  // find the number end
  i = (int)strlen(myBuffer);
  while ((myBuffer[i] == '\0' || myBuffer[i] == '\n' || myBuffer[i] == '\r' || myBuffer[i] == ' ') && i > 0)
    i--;
  if (i != (int)strlen(myBuffer))
    myBuffer[i + 1] = '\0';
  // find the number start
  while (myBuffer[i] >= '0' && myBuffer[i] <= '9' && i > 0)
    i--;
  if (sscanf(&myBuffer[i + 1],"%d",&result) == 0)
    return -1;
  theNumLine = result;
  // find type of line
  while (myBuffer[i] == ' ' && i > 0)
    i--;
  char typesec = myBuffer[i];
  switch (typesec) {
    case 'S' :  myBuffer[i] = '\0'; return (1);
    case 'G' :  myBuffer[i] = '\0'; return (2);
    case 'D' :  myBuffer[i] = '\0'; return (3);
    case 'P' :  myBuffer[i] = '\0'; return (4);
    case 'T' :  myBuffer[i] = '\0'; return (5);
    default  :;
  }
  return -1;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////liriges.c
