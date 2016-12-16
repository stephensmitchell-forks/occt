// Copyright (c) 2016 OPEN CASCADE SAS
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


#include <IGESFile_Reader.hxx>

#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <OSD_OpenFile.hxx>
#include <TCollection_AsciiString.hxx>
#include <Interface_Check.hxx>
#include <Message_Msg.hxx>
#include <IGESData.hxx>
#include <IGESData_Protocol.hxx>
#include <IGESData_IGESModel.hxx>
#include <IGESFile_Buffer.hxx>


//=======================================================================
//function : Constructor
//purpose  : 
//=======================================================================

IGESFile_Reader::IGESFile_Reader (const Handle(IGESData_Protocol)& theProtocol)
: myProtocol(theProtocol),
  myFile(NULL),
  myCurrentLine(0),
  mySectionCode(0)
{
  for (int i = 0; i < sizeof(mySectionLine); i++) mySectionLine[i] = 0;
}

//=======================================================================
//function : Read
//purpose  : 
//=======================================================================

Standard_Integer IGESFile_Reader::Read (const Standard_CString theFileName)
{
  Standard_Size i, aLength;

  myCurrentLine = 0;
  mySectionCode = 0;
  for (i = 0; i < sizeof(mySectionLine); i++) mySectionLine[i] = 0;

  myModel = new IGESData_IGESModel;

  if (myModel->Protocol().IsNull()) myModel->SetProtocol(myProtocol);

  myCheck = new Interface_Check;

  myFile = stdin;
  if (theFileName[0] != '\0') 
    myFile = OSD_OpenFile(theFileName,"r");
  if (myFile == NULL) return -1; // File could not be opened

  Standard_Character aBuffer[100];
  for (i = 0; i < sizeof(aBuffer); i++) aBuffer[i] = 0;

  Standard_Integer aStatus, aDirLine = 0;
  while ((aStatus = ReadLine(aBuffer,aLength)) > 0)
  {
	// Process a line
    switch (mySectionCode)
    {
      case 1: // Start Section
      {
        // Skip ending spaces
        Standard_Integer j = 71;
        for (; j >= 0; j--)
          if (aBuffer[j] > 32) break;
        aBuffer[j+1] = '\0';
        // Add a start section line
        if (j >= 0 || myModel->StartSection()->Length() > 0)
          myModel->StartSection()->Append(new TCollection_HAsciiString(aBuffer));
        break;
      }
      case 2: // Header (Global Section)
      {
        myBuffer->AddLine(aBuffer,aLength);
        break;
      }
      case 3: // Directory
      {
        switch (aDirLine)
        {
          case 0:
          {
            ParseHeader();
            // Then proceed to adding the first directory line
          }
          case 1:
          {
            myBuffer->AddLine(aBuffer,aLength);
            aDirLine = 2;
            break;
          }
          case 2:
          {
            myBuffer->AddLine(aBuffer,aLength);
            ParseDirectory();
            aDirLine = 1;
            break;
          }
          default: break;
        }
        break;
      }
      case 4: // Parameters
      {
        // TODO!!!!
        break;
      }
    }
  }

  // Sending of message : No Terminal Section
  if (mySectionLine[5] == 0)
  {
    Message_Msg amsg("XSTEP_20");
    myCheck->SendWarning(amsg);
  }

  fclose (myFile);

	/*
  LoadModel(amodel);

  if (amodel->Protocol().IsNull()) amodel->SetProtocol (myProtocol);

  //  A present, le check
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

//=======================================================================
//function : LoadModel
//purpose  : 
//=======================================================================

void IGESFile_Reader::LoadModel ()
{
	/*
  //s1:thelist = thereader->Params(0);

  Standard_Integer numr;
  Standard_Integer thenbreps = 0, thenbrep0 = 0;
  Handle(TColStd_HArray1OfTransient) thereports;

  for (numr = myData->FindNextRecord(0);  numr > 0;
       numr = myData->FindNextRecord(numr))
  {
    Handle(Standard_Transient) newent = myProtocol->NewEntity(myData->DirPart(numr).Type());

    if (newent.IsNull()) {
      newent = myProtocol->UnknownEntity();
      if (thereports.IsNull())
        thereports = new TColStd_HArray1OfTransient (1,myData->NbRecords());
      thenbreps ++;  thenbrep0 ++;
      thereports->SetValue (numr,new Interface_ReportEntity(newent));
    }
    myData->BindEntity (numr,newent);
  }

  //====================================
  const Handle(Message_Messenger) &TF = Message::DefaultMessenger(); //this->Messenger();
  //====================================

  Handle(Interface_Check) ach = new Interface_Check;

//  ..            Demarrage : Lecture du Header            ..

  //  ..            Lecture des Entites            ..

  amodel->Reservate (myData->NbEntities());

  numr = myData->FindNextRecord(0);

  Standard_Integer num = numr;
  while (num > 0)
  {
    Standard_Integer ierr = 0;  // erreur sur analyse d une entite
    Handle(Standard_Transient) anent;
    try
    {
      OCC_CATCH_SIGNALS
      for (num = numr;  num > 0; num = myData->FindNextRecord(num))
      {
        numr = num;

        //    Lecture sous protection contre crash
        //    (fait aussi AddEntity mais pas SetReportEntity)
        Handle(Standard_Transient) anent = myData->BoundEntity(num);
        Handle(Interface_Check) ach = new Interface_Check(anent);
        Handle(Interface_ReportEntity) rep;    // entite Report, s il y a lieu
        Standard_Integer irep = 0;
        if (thenbrep0 > 0) {
          rep = Handle(Interface_ReportEntity)::DownCast(thereports->Value(num));
          if (!rep.IsNull()) { irep = num;  ach = rep->Check(); }
        }

        //  ..        Chargement proprement dit : Specifique de la Norme        ..
        thechk = ach;
        thecnum = num;

        DeclareAndCast(IGESData_IGESEntity,ent,anent);
        ent->Clear();
        ent->OwnRead(*this);

        //  ..        Ajout dans le modele de l entite telle quelle        ..
        //            ATTENTION, ReportEntity traitee en bloc apres les Load
        amodel->AddEntity(anent);

        //   Erreur ou Correction : On cree une ReportEntity qui memorise le Check,
        //   l Entite, et en cas d Erreur une UndefinedEntity pour les Parametres

        //   On exploite ici le flag IsLoadError : s il a ete defini (a vrai ou faux)
        //   il a priorite sur les fails du check. Sinon, ce sont les fails qui parlent

        Standard_Integer nbf = ach->NbFails();
        Standard_Integer nbw = ach->NbWarnings();
        if (nbf + nbw > 0)
        {
          amodel->NbEntities();
          rep = new Interface_ReportEntity(ach,anent);
          if (irep == 0)
          {
            if (thereports.IsNull())
              thereports = new TColStd_HArray1OfTransient (1,myData->NbRecords());
            irep = num;
            thenbreps ++;
          }
          thereports->SetValue(irep,rep);
        }
  
        //    Rechargement ? si oui, dans une UnknownEntity fournie par le protocole
        if (nbf > 0)  {
          Handle(Standard_Transient) undef = myProtocol->UnknownEntity();
          GetCasted(IGESData_IGESEntity,undef)->OwnRead(*this);
          rep->SetContent(undef);
        }

        //   ..        Fin Lecture        ..
        if (anent.IsNull())  {
          // Sending of message : Number of ignored Null Entities  
          Message_Msg Msg21("XSTEP_21");
          Msg21.Arg(amodel->NbEntities());
          TF->Send (Msg21, Message_Info);
          continue;
        }
        //      LoadedEntity fait AddEntity MAIS PAS SetReport (en bloc a la fin)

      }    // ---- fin boucle sur entites
      numr = 0;    // plus rien
    }      // ---- fin du try, le catch suit

    //   En cas d erreur NON PREVUE par l analyse, recuperation par defaut
    //   Attention : la recuperation peut elle-meme planter ... (cf ierr)
    catch (Standard_Failure) {
      //      Au passage suivant, on attaquera le record suivant
      numr = myData->FindNextRecord(num); //:g9 abv 28 May 98: tr8_as2_ug.stp - infinite cycle: (0);

      Handle(Standard_Failure) afail = Standard_Failure::Caught();
#ifdef _WIN32
      if (afail.IsNull() || afail->IsKind(STANDARD_TYPE(OSD_Exception))) ierr = 2;
#else
      if (afail.IsNull() || afail->IsKind(STANDARD_TYPE(OSD_Signal))) ierr = 2;
#endif
      anent = myData->BoundEntity(num);
      if (anent.IsNull()) {
        // Sending of message : Number of ignored Null Entities  
        Message_Msg Msg21("XSTEP_21");
        Msg21.Arg(amodel->NbEntities()+1);
        TF->Send (Msg21, Message_Info);
        continue;
      }
      ach = new Interface_Check(anent);
      //: abv 03 Apr 00: trj3_s1-tc-214.stp: generate a message on exception
      Message_Msg Msg278("XSTEP_278");
      Msg278.Arg(amodel->StringLabel(anent));
      ach->SendFail (Msg278); 
      
      if (ierr == 2) {
        // Sending of message : reading of entity failed  
        Message_Msg Msg22("XSTEP_22");
        Msg22.Arg(amodel->StringLabel(anent));
        TF->Send (Msg22, Message_Info); 
        return;
      }

      if (!ierr) {
        ierr = 1;
        // ce qui serait bien ici serait de recuperer le texte de l erreur pour ach ...
        // Sending of message : recovered entity  
        Message_Msg Msg23("XSTEP_23");
        Msg23.Arg(num);
        TF->Send (Msg23, Message_Info); 

        //  Finalement, on charge une Entite Inconnue
        thenbreps ++;
        Handle(Interface_ReportEntity) rep = new Interface_ReportEntity(ach,anent);
        Handle(Standard_Transient) undef = myProtocol->UnknownEntity();
        GetCasted(IGESData_IGESEntity,undef)->OwnRead(*this);
        rep->SetContent(undef);

        if (thereports.IsNull())
          thereports = new TColStd_HArray1OfTransient (1,myData->NbRecords());

        thenbreps ++;
        thereports->SetValue (num,rep);
        amodel->AddEntity (anent);    // pas fait par LoadedEntity ...
      }
      else {
        // Sending of message : reading of entity failed  
        Message_Msg Msg22("XSTEP_22");
        Msg22.Arg(amodel->StringLabel(anent));
        TF->Send (Msg22, Message_Info);
        //  On garde <rep> telle quelle : pas d analyse fichier supplementaire,
        //  Mais la phase preliminaire eventuelle est conservee
        //  (en particulier, on garde trace du Type lu du fichier, etc...)
      }
    }    // -----  fin complete du try/catch
  }      // -----  fin du while

//  ..        Ajout des Reports, silya
  if (!thereports.IsNull()) {
    // Sending of message : report   
    Message_Msg Msg24("XSTEP_24");
    Msg24.Arg(thenbreps);
    TF->Send (Msg24, Message_Info); 
    amodel->Reservate (-thenbreps-10);
    thenbreps = thereports->Upper();
    for (Standard_Integer nr = 1; nr <= thenbreps; nr ++) {
      if (thereports->Value(nr).IsNull()) continue;
      Handle(Standard_Transient) anent = myData->BoundEntity (nr);
      Handle(Interface_ReportEntity) rep =
        Handle(Interface_ReportEntity)::DownCast(thereports->Value(nr));
      amodel->SetReportEntity (-amodel->Number(anent),rep);
    }
  }
  */
}


IGESFile_Reader::ParamStatus IGESFile_Reader::ReadInteger (Standard_Integer &theVal, const Standard_CString theMsg)
{
  const Standard_Character *aBuffer = (const Standard_Character *)myBuffer->ChangeData();

  while (aBuffer[myCurrentPos] == ' ' || aBuffer[myCurrentPos] == '\t') myCurrentPos++;

  if (aBuffer[myCurrentPos] == myHeader.Separator()) {
    myCurrentPos++;
    return ParamEmpty;
  }

  if (aBuffer[myCurrentPos] == myHeader.EndMark())
    return ParamEmpty;

  char *endptr;
  const long int aVal = (Standard_Integer)strtol(&aBuffer[myCurrentPos],&endptr,10);
  const Standard_Size len = endptr - &aBuffer[myCurrentPos];
  myCurrentPos += len;

  while (aBuffer[myCurrentPos] == ' ' || aBuffer[myCurrentPos] == '\t') myCurrentPos++;

  if (aBuffer[myCurrentPos] == myHeader.Separator()) {
    myCurrentPos++;
    theVal = (Standard_Integer)aVal;
    return ParamOK;
  }

  if (aBuffer[myCurrentPos] == myHeader.EndMark()) {
    theVal = (Standard_Integer)aVal;
    return ParamOK;
  }

  return ParamError;
}


IGESFile_Reader::ParamStatus IGESFile_Reader::ReadReal (Standard_Real &theVal, const Standard_CString theMsg)
{
  const Standard_Character *aBuffer = (const Standard_Character *)myBuffer->ChangeData();

  while (aBuffer[myCurrentPos] == ' ' || aBuffer[myCurrentPos] == '\t') myCurrentPos++;

  if (aBuffer[myCurrentPos] == myHeader.Separator()) {
    myCurrentPos++;
    return ParamEmpty;
  }

  if (aBuffer[myCurrentPos] == myHeader.EndMark())
    return ParamEmpty;

  char *endptr;
  const double aVal = (Standard_Real)strtod(&aBuffer[myCurrentPos],&endptr);
  const Standard_Size len = endptr - &aBuffer[myCurrentPos];
  myCurrentPos += len;

  while (aBuffer[myCurrentPos] == ' ' || aBuffer[myCurrentPos] == '\t') myCurrentPos++;

  if (aBuffer[myCurrentPos] == myHeader.Separator()) {
    myCurrentPos++;
    theVal = (Standard_Real)aVal;
    return ParamOK;
  }

  if (aBuffer[myCurrentPos] == myHeader.EndMark()) {
    theVal = (Standard_Real)aVal;
    return ParamOK;
  }

  return ParamError;
}


IGESFile_Reader::ParamStatus IGESFile_Reader::ReadString (TCollection_AsciiString &theVal, const Standard_CString theMsg)
{
  const Standard_Character *aBuffer = (const Standard_Character *)myBuffer->ChangeData();

  while (aBuffer[myCurrentPos] == ' ' || aBuffer[myCurrentPos] == '\t') myCurrentPos++;

  if (aBuffer[myCurrentPos] == myHeader.Separator()) {
    myCurrentPos++;
    return ParamEmpty;
  }

  if (aBuffer[myCurrentPos] == myHeader.EndMark())
    return ParamEmpty;

  char *endptr;
  const long int aLength = strtol(&aBuffer[myCurrentPos],&endptr,10);
  const Standard_Size len = endptr - &aBuffer[myCurrentPos];
  myCurrentPos += len;

  if (aBuffer[myCurrentPos] != 'H' || aBuffer[myCurrentPos] != 'h')
    return ParamError;

  myCurrentPos++;

  // Read characters
  // TODO!!!

  if (aBuffer[myCurrentPos] == myHeader.Separator()) {
    myCurrentPos++;
    return ParamOK;
  }

  if (aBuffer[myCurrentPos] == myHeader.EndMark())
    return ParamOK;

  return ParamError;
}


Standard_Integer IGESFile_Reader::ReadLine (Standard_Character *theBuffer, Standard_Size &theLength)
{
  static const char sects[] = " SGDPT ";

  Standard_Integer i, section_code, section_line;

  for (;;)
  {
    section_code = section_line = 0;

    if (mySectionLine[mySectionCode] == 0)
      theBuffer[72] = theBuffer[79] = ' ';

    myCurrentLine++;

    theBuffer[0] = '\0';

    while ( fgets(theBuffer,2,myFile) && ( theBuffer[0] == '\r' || theBuffer[0] == '\n' ) )
    {}
      
    if (!fgets(&theBuffer[1],80,myFile))
      return 0;

    if (mySectionLine[mySectionCode] == 0 && theBuffer[72] != 'S' && theBuffer[79] == ' ')
    {
      // CASE OF FNES : Skip the first line
      theBuffer[0] = '\0';
      
      while ( fgets(theBuffer,2,myFile) && ( theBuffer[0] == '\r' || theBuffer[0] == '\n' ) )
      {}

      if (!fgets(&theBuffer[1],80,myFile))
        return 0;
    }

    if (feof(myFile))
      return 0;

    {//0x1A is END_OF_FILE for OS DOS and WINDOWS. For other OS we set this rule forcefully.
      char *fc = strchr(theBuffer, 0x1A);
      if(fc != 0)
      {
        fc[0] = '\0';
        return 0;
      }
    }

    if (theBuffer[0] == '\0' || theBuffer[0] == '\n' || theBuffer[0] == '\r')
      continue;

    if (sscanf(&theBuffer[73],"%d",&section_line) != 0)
    {
      switch (theBuffer[72])
      {
        case 'S' : section_code = 1; break;
        case 'G' : section_code = 2; break;
        case 'D' : section_code = 3; break;
        case 'P' : section_code = 4; break;
        case 'T' : section_code = 5; break;
        default  :
          // the column 72 is empty, try to check the neighbour
          if (strlen(theBuffer) == 80 && (theBuffer[79] == '\n' || theBuffer[79] == '\r') && (theBuffer[0] >= '0' && theBuffer[0] <= '9'))
          {
            // check if the case of losted
            for (i = 1; theBuffer[i] >= '0' && theBuffer[i] <= '9'; i++);
            if (theBuffer[i]=='D' || theBuffer[i]=='d') {
              for (i = 79; i > 0; i--)
                theBuffer[i] = theBuffer[i-1];
              theBuffer[0]='.';
              switch (theBuffer[72])
              {
                case 'S' : section_code = 1; break;
                case 'G' : section_code = 2; break;
                case 'D' : section_code = 3; break;
                case 'P' : section_code = 4; break;
                case 'T' : section_code = 5; break;
                default  :;
              }
            }
          }
      }
      if (section_code > 0)
      {
        theBuffer[72] = '\0';
        theLength = 72;
        if (section_code >= mySectionCode) break;
      }
    }

    if (section_code == 0)
    {
      // the line does not conform to standard, try to read it (if there are some missing spaces)
      // find the number end
      const int len = (int)strlen(theBuffer);
      i = len;
      while ((theBuffer[i] == '\0' || theBuffer[i] == '\n' || theBuffer[i] == '\r' || theBuffer[i] == ' ') && i > 0)
        i--;
      if (i != len)
        theBuffer[i + 1] = '\0';
      // find the number start
      while (theBuffer[i] >= '0' && theBuffer[i] <= '9' && i > 0)
        i--;

      if (sscanf(&theBuffer[i + 1],"%d",&section_line) != 0)
	  {
        // find type of line
        while (theBuffer[i] == ' ' && i > 0)
          i--;
        switch (theBuffer[i])
        {
          case 'S' : section_code = 1; break;
          case 'G' : section_code = 2; break;
          case 'D' : section_code = 3; break;
          case 'P' : section_code = 4; break;
          case 'T' : section_code = 5; break;
          default  :;
        }
        if (section_code > 0)
        {
          theBuffer[i] = '\0';
          theLength = i;
          if (section_code >= mySectionCode) break;
        }
	  }
    }

    // Sending of message : Syntax error
    char str[2];
    str[1] = '\0';
    str[0] = sects[mySectionCode];
    Message_Msg amsg("XSTEP_18");
    amsg.Arg(myCurrentLine);
    amsg.Arg(str);
    myCheck->SendFail(amsg);
    
    if (mySectionCode == 0) return -1;

    mySectionLine[mySectionCode]++;
  }

  mySectionCode = section_code;
  mySectionLine[mySectionCode]++;

  if (section_line != mySectionLine[mySectionCode])
  {
    // Sending of message : Syntax error
    char str[2];
    str[1] = '\0';
    str[0] = sects[mySectionCode];
    Message_Msg amsg("XSTEP_19");
    amsg.Arg(myCurrentLine);
    amsg.Arg(str);
    myCheck->SendFail(amsg);
  }

  return section_code;
}


Standard_Boolean IGESFile_Reader::ParseHeader ()
{
  myCurrentPos = 0;

  // Set default delimiters
  myHeader.SetSeparator(',');
  myHeader.SetEndMark(';');

  Standard_Integer ival;
  Standard_Real rval;
  TCollection_AsciiString sval;

  // Parameter Delimiter Character
  switch (ReadString(sval)) {
    case ParamOK:    myHeader.SetSeparator(sval.ToCString()[0]); break;
    case ParamEmpty: myHeader.SetSeparator(','); break;
    default: return Standard_False;
  }
  // Record Delimiter
  switch (ReadString(sval)) {
    case ParamOK:    myHeader.SetEndMark(sval.ToCString()[0]); break;
    case ParamEmpty: myHeader.SetEndMark(';'); break;
    default: return Standard_False;
  }
  // Product Identification From Sender
  switch (ReadString(sval)) {
    case ParamOK:    myHeader.SetSendName(new TCollection_HAsciiString(sval)); break;
    case ParamEmpty: myHeader.SetSendName(NULL); break;
    default: return Standard_False;
  }
  // File Name
  switch (ReadString(sval)) {
    case ParamOK:    myHeader.SetFileName(new TCollection_HAsciiString(sval)); break;
    case ParamEmpty: myHeader.SetFileName(NULL); break;
    default: return Standard_False;
  }
  // Native System ID
  switch (ReadString(sval)) {
    case ParamOK:    myHeader.SetSystemId(new TCollection_HAsciiString(sval)); break;
    case ParamEmpty: myHeader.SetSystemId(NULL); break;
    default: return Standard_False;
  }
  // Preprocessor Version
  switch (ReadString(sval)) {
    case ParamOK:    myHeader.SetInterfaceVersion(new TCollection_HAsciiString(sval)); break;
    case ParamEmpty: myHeader.SetInterfaceVersion(NULL); break;
    default: return Standard_False;
  }
  // Number of Binary Bits for Integer Representation
  switch (ReadInteger(ival)) {
    case ParamOK:    myHeader.SetIntegerBits(ival); break;
    //case ParamEmpty: myHeader.SetIntegerBits(32); break;
    default: return Standard_False;
  }

  switch (ReadInteger(ival)) {
    case ParamOK:    myHeader.SetMaxPower10Single(ival); break;
    //case ParamEmpty: myHeader.SetMaxPower10Single(38); break;
    default: return Standard_False;
  }

  switch (ReadInteger(ival)) {
    case ParamOK:    myHeader.SetMaxDigitsSingle(ival); break;
    //case ParamEmpty: myHeader.SetMaxDigitsSingle(6); break;
    default: return Standard_False;
  }

  switch (ReadInteger(ival)) {
    case ParamOK:    myHeader.SetMaxPower10Double(ival); break;
    //case ParamEmpty: myHeader.SetMaxPower10Double(308); break;
    default: return Standard_False;
  }

  switch (ReadInteger(ival)) {
    case ParamOK:    myHeader.SetMaxDigitsDouble(ival); break;
    //case ParamEmpty: myHeader.SetMaxDigitsDouble(15); break;
    default: return Standard_False;
  }

  switch (ReadString(sval)) {
    case ParamOK:    myHeader.SetReceiveName(new TCollection_HAsciiString(sval)); break;
    case ParamEmpty: myHeader.SetReceiveName(myHeader.SendName()); break;
    default: return Standard_False;
  }

  switch (ReadReal(rval)) {
    case ParamOK:    myHeader.SetScale(rval); break;
    case ParamEmpty: myHeader.SetScale(1.); break;
    default: return Standard_False;
  }

  switch (ReadInteger(ival)) {
    case ParamOK:    myHeader.SetUnitFlag(ival); break;
    case ParamEmpty: myHeader.SetUnitFlag(1); break;
    default: return Standard_False;
  }

  switch (ReadString(sval)) {
    case ParamOK:    myHeader.SetUnitName(new TCollection_HAsciiString(sval)); break;
    case ParamEmpty: myHeader.SetUnitName(new TCollection_HAsciiString("IN")); break;
    default: return Standard_False;
  }

  switch (ReadInteger(ival)) {
    case ParamOK:    myHeader.SetLineWeightGrad(ival); break;
    case ParamEmpty: myHeader.SetLineWeightGrad(1); break;
    default: return Standard_False;
  }

  switch (ReadReal(rval)) {
    case ParamOK:    myHeader.SetMaxLineWeight(rval); break;
    //case ParamEmpty: myHeader.SetMaxLineWeight(0.); break;
    default: return Standard_False;
  }

  switch (ReadString(sval)) {
    case ParamOK:    myHeader.SetDate(new TCollection_HAsciiString(sval)); break;
    //case ParamEmpty: myHeader.SetDate(NULL); break;
    default: return Standard_False;
  }

  switch (ReadReal(rval)) {
    case ParamOK:    myHeader.SetResolution(rval); break;
    //case ParamEmpty: myHeader.SetResolution(0.); break;
    default: return Standard_False;
  }

  switch (ReadReal(rval)) {
    case ParamOK:    myHeader.SetMaxCoord(rval); break; //hasMaxCoord = Standard_True;
    //case ParamEmpty: myHeader.SetMaxCoord(0.); break;
    default: return Standard_False;
  }

  switch (ReadString(sval)) {
    case ParamOK:    myHeader.SetAuthorName(new TCollection_HAsciiString(sval)); break;
    case ParamEmpty: myHeader.SetAuthorName(NULL); break;
    default: return Standard_False;
  }

  switch (ReadString(sval)) {
    case ParamOK:    myHeader.SetCompanyName(new TCollection_HAsciiString(sval)); break;
    case ParamEmpty: myHeader.SetCompanyName(NULL); break;
    default: return Standard_False;
  }

  switch (ReadInteger(ival)) {
    case ParamOK:    myHeader.SetIGESVersion(ival); break;
    case ParamEmpty: myHeader.SetIGESVersion(11); break; //3?
    default: return Standard_False;
  }

  switch (ReadInteger(ival)) {
    case ParamOK:    myHeader.SetDraftingStandard(ival); break;
    case ParamEmpty: myHeader.SetDraftingStandard(0); break;
    default: return Standard_False;
  }

  //theLastChangeDate.Nullify();  // nouveaute 5.1 (peut etre absente)
  //theAppliProtocol.Nullify();   // nouveaute 5.3 (peut etre absente)

  //:45 by abv 11.12.97: if UnitFlag is not defined in the file, 
  // restore it from UnitName. Repris par CKY 13-FEV-1998
  if ( myHeader.UnitFlag() == 0 || myHeader.UnitFlag() == 3 ) {
    const Standard_Integer corrected = myHeader.UnitName().IsNull()? 1 : IGESData::UnitNameFlag(myHeader.UnitName()->ToCString());
    if (corrected > 0) myHeader.SetUnitFlag(corrected);
    else if (myHeader.UnitFlag() == 3) {
      Message_Msg Msg49 ("XSTEP_49");
      myCheck->SendWarning (Msg49);
    }
    else {
      Message_Msg Msg48 ("XSTEP_48");
      myCheck->SendFail (Msg48);
    }
  }

  myModel->SetGlobalSection(myHeader);

  /* themaxweight  = gs.MaxLineWeight();
     thegradweight = gs.LineWeightGrad();
     if (thegradweight > 0)
     { themaxweight = themaxweight/thegradweight; thegradweight = 1; }
     thedefweight = myData->DefaultLineWeight();*/

  return Standard_True;
}
