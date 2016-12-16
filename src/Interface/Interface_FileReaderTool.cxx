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

// dce 21/01/99 S3767 : Suppression of general messages

#include <Interface_Check.hxx>
#include <Interface_FileReaderData.hxx>
#include <Interface_FileReaderTool.hxx>
#include <Interface_GeneralLib.hxx>
#include <Interface_GeneralModule.hxx>
#include <Interface_InterfaceMismatch.hxx>
#include <Interface_InterfaceModel.hxx>
#include <Interface_Protocol.hxx>
#include <Interface_ReaderModule.hxx>
#include <Interface_ReportEntity.hxx>
#include <Message.hxx>
#include <Message_Messenger.hxx>
#include <Message_Msg.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <Standard_NoSuchObject.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_Transient.hxx>
#include <Standard_TypeMismatch.hxx>
#include <TColStd_HArray1OfTransient.hxx>

#ifdef _WIN32
#include <OSD_Exception.hxx>
#else
#include <OSD_Signal.hxx>
#endif
#include <stdio.h>

#include <Message_Msg.hxx>
#include <TCollection_HAsciiString.hxx>

// Failure pour recuperer erreur en lecture fichier,
// TypeMismatch pour message d erreur circonstancie (cas particulier important)


//  Gere le chargement d un Fichier, prealablement transforme en FileReaderData
//  (de la bonne norme), dans un Modele


//=======================================================================
//function : Interface_FileReaderTool
//purpose  : 
//=======================================================================

Interface_FileReaderTool::Interface_FileReaderTool (const Handle(Interface_Protocol)& protocol)
: theglib(protocol),
  therlib(protocol),
  themessenger(Message::DefaultMessenger())
{
}

//=======================================================================
//function : EndRead
//purpose  : 
//=======================================================================

void Interface_FileReaderTool::EndRead(const Handle(Interface_InterfaceModel)& )
{
}


//  ....               (Sa Majeste le) CHARGEMENT DU MODELE               ....


//=======================================================================
//function : LoadModel
//purpose  : 
//=======================================================================

void Interface_FileReaderTool::LoadModel (const Handle(Interface_InterfaceModel)& amodel)
//
//   Methode generale de lecture d un fichier : il est lu via un FileReaderData
//   qui doit y donner acces de la facon la plus performante possible
//   chaque interface definit son FileHeader avec ses methodes, appelees ici
{
  Prepare ();

  Standard_Integer numr;
  Standard_Integer thenbreps = 0, thenbrep0 = 0;
  Handle(TColStd_HArray1OfTransient) thereports;

  for (numr = thereader->FindNextRecord(0);  numr > 0;
       numr = thereader->FindNextRecord(numr))
  {
    Handle(Standard_Transient) newent;
    const Standard_Boolean res = Recognize (numr,newent);
    if (!res) {
      newent = theproto->UnknownEntity();
      if (thereports.IsNull())
        thereports = new TColStd_HArray1OfTransient (1,thereader->NbRecords());
      thenbreps ++;  thenbrep0 ++;
      thereports->SetValue (numr,new Interface_ReportEntity(newent));
    }
    thereader->BindEntity (numr,newent);
  }

  //====================================
  const Handle(Message_Messenger) &TF = Messenger();
  //====================================
  Handle(Interface_Check) ach = new Interface_Check;

  themodel = amodel;

//  ..            Demarrage : Lecture du Header            ..
  try {
    OCC_CATCH_SIGNALS
    BeginRead(themodel);  // selon la norme
  }
  catch (Standard_Failure) {
    // Sendinf of message : Internal error during the header reading
    Message_Msg Msg11("XSTEP_11");
    TF->Send (Msg11, Message_Info); 
  }

  //  ..            Lecture des Entites            ..

  themodel->Reservate (thereader->NbEntities());

  numr = thereader->FindNextRecord(0);

  Standard_Integer num = numr;
  while (num > 0)
  {
    Standard_Integer ierr = 0;  // erreur sur analyse d une entite
    Handle(Standard_Transient) anent;
    try
    {
      OCC_CATCH_SIGNALS
      for (num = numr;  num > 0; num = thereader->FindNextRecord(num))
      {
        numr = num;

        //    Lecture sous protection contre crash
        //    (fait aussi AddEntity mais pas SetReportEntity)
        Handle(Standard_Transient) anent = thereader->BoundEntity(num);
        Handle(Interface_Check) ach = new Interface_Check(anent);
        Handle(Interface_ReportEntity) rep;    // entite Report, s il y a lieu
        Standard_Integer irep = 0;
        if (thenbrep0 > 0) {
          rep = Handle(Interface_ReportEntity)::DownCast(thereports->Value(num));
          if (!rep.IsNull()) { irep = num;  ach = rep->Check(); }
        }

        //  ..        Chargement proprement dit : Specifique de la Norme        ..
        AnalyseRecord(num,anent,ach);

        //  ..        Ajout dans le modele de l entite telle quelle        ..
        //            ATTENTION, ReportEntity traitee en bloc apres les Load
        themodel->AddEntity(anent);

        //   Erreur ou Correction : On cree une ReportEntity qui memorise le Check,
        //   l Entite, et en cas d Erreur une UndefinedEntity pour les Parametres

        //   On exploite ici le flag IsLoadError : s il a ete defini (a vrai ou faux)
        //   il a priorite sur les fails du check. Sinon, ce sont les fails qui parlent

        Standard_Integer nbf = ach->NbFails();
        Standard_Integer nbw = ach->NbWarnings();
        if (nbf + nbw > 0)
        {
          themodel->NbEntities();
          rep = new Interface_ReportEntity(ach,anent);
          if (irep == 0)
          {
            if (thereports.IsNull())
              thereports = new TColStd_HArray1OfTransient (1,thereader->NbRecords());
            irep = num;
            thenbreps ++;
          }
          thereports->SetValue(irep,rep);
        }
  
        //    Rechargement ? si oui, dans une UnknownEntity fournie par le protocole
        if (nbf > 0)  {
          Handle(Standard_Transient) undef = theproto->UnknownEntity();
          AnalyseRecord(num,undef,ach);
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
      numr = thereader->FindNextRecord(num); //:g9 abv 28 May 98: tr8_as2_ug.stp - infinite cycle: (0);

      Handle(Standard_Failure) afail = Standard_Failure::Caught();
#ifdef _WIN32
      if (afail.IsNull() || afail->IsKind(STANDARD_TYPE(OSD_Exception))) ierr = 2;
#else
      if (afail.IsNull() || afail->IsKind(STANDARD_TYPE(OSD_Signal))) ierr = 2;
#endif
//:abv 03Apr00: anent is actually a previous one:      if (anent.IsNull()) 
      anent = thereader->BoundEntity(num);
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
        Handle(Interface_ReportEntity) rep =
          new Interface_ReportEntity(ach,anent);
        Handle(Standard_Transient) undef = theproto->UnknownEntity();
        AnalyseRecord(num,undef,ach);
        rep->SetContent(undef);

        if (thereports.IsNull())
          thereports = new TColStd_HArray1OfTransient (1,thereader->NbRecords());
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
      Handle(Standard_Transient) anent = thereader->BoundEntity (nr);
      Handle(Interface_ReportEntity) rep =
        Handle(Interface_ReportEntity)::DownCast(thereports->Value(nr));
      amodel->SetReportEntity (-amodel->Number(anent),rep);
    }
  }

//   Conclusion : peut ne rien faire : selon necessite
  try {
    OCC_CATCH_SIGNALS
    EndRead(amodel);  // selon la norme
  }
  catch (Standard_Failure) {
    // Sendinf of message : Internal error during the header reading
    Message_Msg Msg11("XSTEP_11");
    TF->Send (Msg11, Message_Info); 
  }
}


//=======================================================================
//function : RecognizeByLib
//purpose  : 
//=======================================================================

Standard_Boolean Interface_FileReaderTool::RecognizeByLib(const Standard_Integer num,
                                                          Handle(Standard_Transient)& ent) const
{
  Handle(Interface_GeneralModule) gmod;
  Handle(Interface_ReaderModule)  rmod;
  Handle(Interface_Protocol) proto;
  Standard_Integer CN = 0;
//   Chercher dans ReaderLib : Reconnaissance de cas -> CN , proto
  for (therlib.Start(); therlib.More(); therlib.Next()) {
    rmod = therlib.Module();
    if (rmod.IsNull()) continue;
    CN = rmod->CaseNum(thereader,num);
    if (CN > 0)  {  proto = therlib.Protocol();  break;  }
  }
  if (CN <= 0 || proto.IsNull()) return Standard_False;
//   Se recaler dans GeneralLib : Creation de l entite vide
  Handle(Standard_Type) typrot = proto->DynamicType();
  for (theglib.Start(); theglib.More(); theglib.Next()) {
    proto = theglib.Protocol();
    if (proto.IsNull()) continue;
    if (proto->DynamicType() != typrot) continue;
    Standard_Boolean res = theglib.Module()->NewVoid(CN,ent);
    if (res) return res;
  }
  return Standard_False;
}
