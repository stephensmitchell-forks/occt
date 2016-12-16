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
#include <IGESData_IGESEntity.hxx>
#include <IGESData_IGESModel.hxx>
#include <IGESData_IGESReaderData.hxx>
#include <IGESData_IGESReaderTool.hxx>
#include <IGESData_LabelDisplayEntity.hxx>
#include <IGESData_LevelListEntity.hxx>
#include <IGESData_LineFontEntity.hxx>
#include <IGESData_ParamCursor.hxx>
#include <IGESData_ParamReader.hxx>
#include <IGESData_Protocol.hxx>
#include <IGESData_TransfEntity.hxx>
#include <IGESData_UndefinedEntity.hxx>
#include <IGESData_ViewKindEntity.hxx>
#include <Interface_Check.hxx>
#include <Interface_FileParameter.hxx>
#include <Interface_InterfaceModel.hxx>
#include <Interface_Macros.hxx>
#include <Interface_ParamList.hxx>
#include <Interface_ReaderModule.hxx>
#include <Message.hxx>
#include <Message_Msg.hxx>
#include <Message_Messenger.hxx>
#include <TCollection_HAsciiString.hxx>
#include <TColStd_HArray1OfTransient.hxx>
#include <Interface_ReportEntity.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <OSD_Exception.hxx>

#include <stdio.h>

//=======================================================================
//function : LoadModel
//purpose  : 
//=======================================================================

Standard_Integer IGESData_IGESReaderTool::Read (const Standard_CString theFileName, const Handle(IGESData_IGESModel)& amodel, const Standard_Boolean modefnes)
{
  myData = new IGESData_IGESReaderData; //((lesect[3]+1)/2, gNbParams);

  int result = myData->Read(theFileName, modefnes);

  if (result != 0) return result;

  LoadModel(amodel);

  if (amodel->Protocol().IsNull()) amodel->SetProtocol (myProtocol);

  /*
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

void IGESData_IGESReaderTool::LoadModel (const Handle(Interface_InterfaceModel)& amodel)
{
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

  //themodel = amodel;

//  ..            Demarrage : Lecture du Header            ..
  try {
    OCC_CATCH_SIGNALS

    DeclareAndCast(IGESData_IGESModel,amod,amodel);
    const IGESData_GlobalSection& gs = myData->GlobalSection();
    amod->SetStartSection  (myData->StartSection());
    amod->SetGlobalSection (gs);
    const Handle(Interface_Check) &glob = amod->GlobalCheck();
    glob->GetMessages (myData->GlobalCheck());
    //not needed:amod->SetGlobalCheck   (glob);

    themaxweight  = gs.MaxLineWeight();
    thegradweight = gs.LineWeightGrad();
    if (thegradweight > 0)
    { themaxweight = themaxweight/thegradweight; thegradweight = 1; }
    thedefweight = myData->DefaultLineWeight();
  }
  catch (Standard_Failure) {
    // Sendinf of message : Internal error during the header reading
    Message_Msg Msg11("XSTEP_11");
    TF->Send (Msg11, Message_Info); 
  }

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
}
