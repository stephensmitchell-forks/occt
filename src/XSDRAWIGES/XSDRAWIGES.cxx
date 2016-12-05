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

//gka 06.01.99 S3767 new function TPSTAT (first version)
//pdn 11.01.99 putting "return" statement for compilation on NT

#include <DBRep.hxx>
#include <Draw_Appli.hxx>
#include <Draw_ProgressIndicator.hxx>
#include <DrawTrSurf.hxx>
#include <Dico_DictionaryOfInteger.hxx>
#include <Dico_DictionaryOfTransient.hxx>
#include <Dico_IteratorOfDictionaryOfInteger.hxx>
#include <Dico_IteratorOfDictionaryOfTransient.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Surface.hxx>
#include <IGESControl_Controller.hxx>
#include <IGESControl_Reader.hxx>
#include <IGESControl_Writer.hxx>
#include <IGESData.hxx>
#include <IGESData_IGESEntity.hxx>
#include <IGESData_IGESModel.hxx>
#include <IGESData_Protocol.hxx>
#include <IGESToBRep.hxx>
#include <Interface_Check.hxx>
#include <Interface_CheckIterator.hxx>
#include <Interface_InterfaceModel.hxx>
#include <Interface_Macros.hxx>
#include <Interface_Static.hxx>
#include <Message.hxx>
#include <Message_Msg.hxx>
#include <Message_Messenger.hxx>
#include <Message_ProgressSentry.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_HAsciiString.hxx>
#include <TColStd_HSequenceOfTransient.hxx>
#include <TColStd_MapIteratorOfMapOfTransient.hxx>
#include <TColStd_MapOfTransient.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <Transfer_FinderProcess.hxx>
#include <Transfer_TransientProcess.hxx>
#include <IFSelect_WorkSession.hxx>
#include <XSDRAW.hxx>
#include <XSDRAW_Commands.hxx>
#include <XSDRAW_SelectFunctions.hxx>
#include <XSDRAWIGES.hxx>

#include <stdio.h>

//--------------------------------------------------------------
// Function : igesbrep
//--------------------------------------------------------------
static Standard_Integer igesbrep (Draw_Interpretor& di, Standard_Integer argc, const char** argv) 
{
  DeclareAndCast(IGESControl_Controller,ctl,XSDRAW::Controller());
  if (ctl.IsNull()) XSDRAW::SetNorm("IGES");

  // Progress indicator
  Handle(Draw_ProgressIndicator) progress = new Draw_ProgressIndicator ( di, 1 );
  progress->SetScale ( 0, 100, 1 );
  progress->Show();

  const Handle(IFSelect_WorkSession) &WS = XSDRAW::Session();

  IGESControl_Reader Reader (WS,Standard_False);
  WS->SetModeStat(Standard_True);
  if (ctl.IsNull())
    ctl=Handle(IGESControl_Controller)::DownCast(XSDRAW::Controller());

  TCollection_AsciiString fnom,rnom;

  Standard_Boolean modfic = XSDRAW::FileAndVar
    (argv[1],argv[2],"IGESBREP",fnom,rnom);
  if (modfic) di<<" File IGES to read : "<<fnom.ToCString()<<"\n";
  else        di<<" Model taken from the session : "<<fnom.ToCString()<<"\n";
  di<<" -- Names of variables BREP-DRAW prefixed by : "<<rnom.ToCString()<<"\n";
  Interface_ReturnStatus readstat = Interface_RetVoid;

#ifdef CHRONOMESURE
  OSD_Timer Chr; Chr.Reset();
  IDT_SetLevel(3);
#endif


// Reading the file
  progress->NewScope ( 20, "Loading" ); // On average loading takes 20% 
  progress->Show();

  if (modfic) readstat = Reader.ReadFile (fnom.ToCString());
  else  if (XSDRAW::Session()->NbStartingEntities() > 0) readstat = Interface_RetDone;

  progress->EndScope();
  progress->Show();

  if (readstat != Interface_RetDone) {
    if (modfic) di<<"Could not read file "<<fnom.ToCString()<<" , abandon\n";
    else di<<"No model loaded\n";
    return 1;
  }
// Choice of treatment
  Standard_Boolean fromtcl = (argc > 3);
  Standard_Integer modepri = 1, nent, nbs;
  if (fromtcl) modepri = 4;

  while (modepri) {
    //Roots for transfer are defined before setting mode ALL or OnlyVisible - gka 
    //mode OnlyVisible does not work.
    // nent = Reader.NbRootsForTransfer();
    if (!fromtcl) {
      cout<<"Mode (0 End, 1 Visible Roots, 2 All Roots, 3 Only One Entity, 4 Selection) :"<<flush;
      modepri = -1;
      
// amv 26.09.2003 : this is used to avoid error of enter's simbol        
      char str[80];                                                             
      cin>>str;                                                                 
      modepri = Draw::Atoi(str);   
    }

    if (modepri == 0) {  //fin
      di << "Bye and good luck! \n";
      break;
    } 

    else if (modepri <= 2) {  // 1 : Visible Roots, 2 : All Roots
      di << "All Geometry Transfer\n";
      di<<"spline_continuity (read) : "<<Interface_Static::IVal("read.iges.bspline.continuity")<<" (0 : no modif, 1 : C1, 2 : C2)\n";
      di<<"  To modify : command  param read.iges.bspline.continuity\n";
      XSDRAW::SetTransferProcess (WS->ReaderProcess());
      progress->NewScope ( 80, "Translation" );
      progress->Show();
      WS->ReaderProcess()->SetProgress ( progress );
      
      if (modepri == 1) Reader.SetReadVisible (Standard_True);
      Reader.TransferRoots();
      
      WS->ReaderProcess()->SetProgress ( 0 );
      progress->EndScope();
      progress->Show();
      // result in only one shape for all the roots
      //        or in one shape for one root.
      di<<"Count of shapes produced : "<<Reader.NbShapes()<<"\n";
      Standard_Integer answer = 1;
      if (Reader.NbShapes() > 1) {
	cout << " pass(0)  one shape for all (1)\n or one shape per root (2)\n + WriteBRep (one for all : 3) (one per root : 4) : " << flush;
        answer = -1;
        //amv 26.09.2003                                                        
        char str_a[80];                                                         
        cin >> str_a;                                                           
        answer = Draw::Atoi(str_a);    
      }
      if ( answer == 0) continue;
      if ( answer == 1 || answer == 3) {
	TopoDS_Shape shape = Reader.OneShape();
	// save the shape
	if (shape.IsNull()) { di<<"No Shape produced\n"; continue; }
	char fname[110];
	Sprintf(fname, "%s", rnom.ToCString());
	di << "Saving shape in variable Draw : " << fname << "\n";
	if (answer == 3) IGESToBRep::WriteShape (shape,1);
	try {
	  OCC_CATCH_SIGNALS
	  DBRep::Set(fname,shape);
	}
	catch(Standard_Failure) {
	  di << "** Exception : ";
	  di << Standard_Failure::Caught()->GetMessageString();
	  di<<" ** Skip\n";
	  di << "Saving shape in variable Draw : " << fname << "\n";
	  IGESToBRep::WriteShape (shape,1);
	}
      }
	
      else if (answer == 2 || answer == 4) {
	Standard_Integer numshape = Reader.NbShapes();
	for (Standard_Integer inum = 1; inum <= numshape; inum++) {
	  // save all the shapes
	  TopoDS_Shape shape = Reader.Shape(inum);
	  if (shape.IsNull()) { di<<"No Shape produced\n"; continue; }
	  char fname[110];
	  Sprintf(fname, "%s_%d", rnom.ToCString(),inum);
	  di << "Saving shape in variable Draw : " << fname << "\n";
	  if (answer == 4) IGESToBRep::WriteShape (shape,inum);
	  try {
	    OCC_CATCH_SIGNALS
	    DBRep::Set(fname,shape);
	  }
	  catch(Standard_Failure) {
	    di << "** Exception : ";
	    di << Standard_Failure::Caught()->GetMessageString();
	    di<<" ** Skip\n";
	  }
	}
      }
      else return 0;
    }

    else if (modepri == 3) {  // One Entity
      cout << "Only One Entity"<<endl;
      cout<<"spline_continuity (read) : "<<Interface_Static::IVal("read.iges.bspline.continuity")<<" (0 : no modif, 1 : C1, 2 : C2)"<<endl;
      cout<<"  To modify : command  param read.iges.bspline.continuity"<<endl;
      cout << " give the number of the Entity : " << flush;
      nent = XSDRAW::GetEntityNumber();

      if (!Reader.TransferEntity (WS->StartingEntity(nent))) di<<"Transfer entity n0 "<<nent<<" : no result\n";
      else {
	nbs = Reader.NbShapes();
	char shname[30];  Sprintf (shname,"%s_%d",rnom.ToCString(),nent);
	di<<"Transfer entity n0 "<<nent<<" OK  -> DRAW Shape: "<<shname<<"\n";
	di<<"Now, "<<nbs<<" Shapes produced\n";
	TopoDS_Shape sh = Reader.Shape(nbs);
	DBRep::Set (shname,sh);
      }
    }

    else if (modepri == 4) {   // Selection
      Standard_Integer answer = 1;
      Handle(TColStd_HSequenceOfTransient)  list;

//  Selection, nommee ou via tcl. tcl : raccourcis admis
//   * donne iges-visible + xst-transferrable-roots
//   *r donne xst-model-roots (TOUTES racines)

      if( fromtcl && argv[3][0]=='*' && argv[3][1]=='\0' ) {         
        di << "All Geometry Transfer\n";
        di<<"spline_continuity (read) : "<<Interface_Static::IVal("read.iges.bspline.continuity")<<" (0 : no modif, 1 : C1, 2 : C2)\n";
        di<<"  To modify : command  param read.iges.bspline.continuity\n";
        XSDRAW::SetTransferProcess (WS->ReaderProcess());
        progress->NewScope ( 80, "Translation" );
        progress->Show();
        WS->ReaderProcess()->SetProgress ( progress );
      
        Reader.SetReadVisible (Standard_True);
        Reader.TransferRoots();
      
        WS->ReaderProcess()->SetProgress ( 0 );
        progress->EndScope();
        progress->Show();
        
        // result in only one shape for all the roots
        TopoDS_Shape shape = Reader.OneShape();
        // save the shape
        char fname[110];
        Sprintf(fname, "%s", rnom.ToCString());
        di << "Saving shape in variable Draw : " << fname << "\n";
        try {
          OCC_CATCH_SIGNALS
          DBRep::Set(fname,shape);
        }
        catch(Standard_Failure) {
          di << "** Exception : ";
	  di << Standard_Failure::Caught()->GetMessageString();
	  di<<" ** Skip\n";
          di << "Saving shape in variable Draw : " << fname << "\n";
          IGESToBRep::WriteShape (shape,1);
        }                                                                             
        return 0;
      }
   
      if(fromtcl) {
	modepri = 0;    // d office, une seule passe
	if (argv[3][0] == '*' && argv[3][1] == 'r' && argv[3][2] == '\0') {
	  di<<"All Roots : ";
	  list = XSDRAW::GetList ("xst-model-roots");
	}
        else {
	  TCollection_AsciiString compart = XSDRAW_CommandPart (argc,argv,3);
	  di<<"List given by "<<compart.ToCString()<<" : ";
	  list = XSDRAW::GetList (compart.ToCString());
	}
	if (list.IsNull()) {
          di<<"No list defined. Give a selection name or * for all visible transferrable roots\n";
          continue;
        }
      }
      else {
	cout<<"Name of Selection :"<<flush;
	list = XSDRAW::GetList();
	if (list.IsNull()) { cout<<"No list defined"<<endl; continue; }
      }

      Standard_Integer nbl = list->Length();
      di<<"Nb entities selected : "<<nbl<<"\n";
      if (nbl == 0) continue;
      while (answer) {
	if (!fromtcl) {
	  cout<<"Choice: 0 abandon  1 transfer all  2 with confirmation  3 list n0s ents :"<<flush;
          answer = -1;
          // anv 26.09.2003                                                     
          char str_answer[80];                                                  
          cin>>str_answer;                                                      
          answer = Draw::Atoi(str_answer);    
	}
	if (answer <= 0 || answer > 3) continue;
	if (answer == 3) {
	  for (Standard_Integer ill = 1; ill <= nbl; ill ++) {
	    Handle(Standard_Transient) ent = list->Value(ill);
	    di<<"  ";// model->Print(ent,di);
	  }
	  di<<"\n";
	}
	if (answer == 1 || answer == 2) {
	  Standard_Integer nbt = 0;
	
	  XSDRAW::SetTransferProcess (WS->ReaderProcess());
          progress->NewScope ( 80, "Translation" );
          progress->Show();
          WS->ReaderProcess()->SetProgress ( progress );

          Message_ProgressSentry PSentry ( progress, "Root", 0, nbl, 1 );
	  for (Standard_Integer ill = 1; ill <= nbl && PSentry.More(); ill ++, PSentry.Next()) {
	  
	    nent = Reader.Model()->Number(list->Value(ill));
	    if (nent == 0) continue;
	    if (!Reader.TransferEntity (WS->StartingEntity(nent))) di<<"Transfer entity n0 "<<nent<<" : no result\n";
	    else {
	      nbs = Reader.NbShapes();
	      char shname[30];  Sprintf (shname,"%s_%d",rnom.ToCString(),nbs);
	      di<<"Transfer entity n0 "<<nent<<" OK  -> DRAW Shape: "<<shname<<"\n";
	      di<<"Now, "<<nbs<<" Shapes produced\n";
	      TopoDS_Shape sh = Reader.Shape(nbs);
	      DBRep::Set (shname,sh);
              nbt++;
	    }
	  }
	  WS->ReaderProcess()->SetProgress ( 0 );
          progress->EndScope();
          progress->Show();
	  di<<"Nb Shapes successfully produced : "<<nbt<<"\n";
	  answer = 0;  // on ne reboucle pas
	}
      }
    }
    else di<<"Unknown mode n0 "<<modepri<<"\n";
  }
  return 0;
}

//--------------------------------------------------------------
// Function : testreadiges
//
//--------------------------------------------------------------
static Standard_Integer testread (Draw_Interpretor& di, Standard_Integer argc, const char** argv) 
{
  if (argc != 3)                                                                                      
    {                                                                                             
      di << "ERROR in " << argv[0] << "Wrong Number of Arguments.\n";                     
      di << " Usage : " << argv[0] <<" file_name shape_name\n";                          
      return 1;                                                                                 
    }  
  IGESControl_Reader Reader;
  Standard_CString filename = argv[1];
  Interface_ReturnStatus readstat =  Reader.ReadFile(filename);
  di<<"Status from reading IGES file "<<filename<<" : ";  
  switch(readstat) {                                                              
    case Interface_RetVoid  : { di<<"empty file\n"; return 1; }            
    case Interface_RetDone  : { di<<"file read\n";    break; }             
    case Interface_RetError : { di<<"file not found\n";   return 1; }      
    case Interface_RetFail  : { di<<"error during read\n";  return 1; }    
    default  :  { di<<"failure\n";   return 1; }                          
  }       
  Reader.TransferRoots();
  TopoDS_Shape shape = Reader.OneShape();
  DBRep::Set(argv[2],shape); 
  di<<"Count of shapes produced : "<<Reader.NbShapes()<<"\n";
  return 0;  
}
 
//--------------------------------------------------------------
// Function : brepiges
//
//--------------------------------------------------------------

static Standard_Integer brepiges (Draw_Interpretor& di, Standard_Integer n, const char** a) 
{
  XSDRAW::SetNorm("IGES");
  // ecriture dans le model d'une entite :
  //    -  model_AddEntity(ent)             : ecriture de l`entite seule
  //    -  model->AddWithRefs(ent, protocol): ecriture de l`entite et eventuellement 
  //                                          . de sa matrice de transformation 
  //                                          . de ses sous-elements

  IGESControl_Writer ICW (Interface_Static::CVal("write.iges.unit"),
			  Interface_Static::IVal("write.iges.brep.mode"));
  di<<"unit (write) : "<<Interface_Static::CVal("write.iges.unit")<<"\n";
  di<<"mode  write  : "<<Interface_Static::CVal("write.iges.brep.mode")<<"\n";
  di<<"  To modifiy : command  param\n";

//  Mode d emploi (K4B ->) : brepiges shape [+shape][ +shape] nomfic
//   c a d tant qu il y a des + on ajoute ce qui suit
  const char* nomfic = NULL;
  Standard_Integer npris = 0;

  Handle(Draw_ProgressIndicator) progress = new Draw_ProgressIndicator ( di, 1 );
  progress->NewScope(90,"Translating");
  progress->Show();
  ICW.TransferProcess()->SetProgress(progress);

  for ( Standard_Integer i = 1; i < n; i++) {
    const char* nomvar = a[i];
    if (a[i][0] == '+') nomvar = &(a[i])[1];
    else if (i > 1)  {  nomfic = a[i];  break;  }
    TopoDS_Shape Shape = DBRep::Get(nomvar);
    if      (ICW.AddShape (Shape)) npris ++;
    else if (ICW.AddGeom (DrawTrSurf::GetCurve   (nomvar)) ) npris ++;
    else if (ICW.AddGeom (DrawTrSurf::GetSurface (nomvar)) ) npris ++;
  }
  ICW.ComputeModel();
  XSDRAW::SetModel(ICW.Model());
  XSDRAW::SetTransferProcess (ICW.TransferProcess());
    
  ICW.TransferProcess()->SetProgress(0);
  progress->EndScope();
  progress->Show();
  progress->NewScope(10,"Writing");
  progress->Show();

  di<<npris<<" Shapes translated, giving "<<XSDRAW::Model()->NbEntities()<<" Entities\n";

  if ( ! nomfic ) // delayed write
  {
    di<<" Now, the model is ready\n";
    return 0;
  }

  // write file
  if (! ICW.Write(nomfic)) di<<" Error: could not write file " << nomfic;
  else                     di<<" File " << nomfic << " written";

  progress->EndScope();
  progress->Show();

  return 0;
}

//--------------------------------------------------------------
// Function : testwriteiges
//
//--------------------------------------------------------------

static Standard_Integer testwrite (Draw_Interpretor& di, Standard_Integer n, const char** a) 
{
  if (n != 3)                                                                                      
    {                                                                                             
      di << "ERROR in " << a[0] << "Wrong Number of Arguments.\n";                     
      di << " Usage : " << a[0] <<" file_name shape_name\n";                          
      return 1;                                                                                 
    }
  IGESControl_Writer Writer;
  Standard_CString filename = a[1];
  TopoDS_Shape shape = DBRep::Get(a[2]); 
  Standard_Boolean ok = Writer.AddShape(shape);
  if(!ok){
    di<<"Shape not add\n";
    return 1;
  }
  
  if(!(Writer.Write(filename))){
    di<<"Error on writing file\n";
    return 1;
  }
  di<<"File Is Written\n"; 
  return 0;
}
//--------------------------------------------------------------
// Function : igesparam
//
//--------------------------------------------------------------


static Standard_Integer igesparam (Draw_Interpretor& di, Standard_Integer , const char** ) 
{
//  liste des parametres
  di<<"List of parameters which control IGES :\n";
  di<<"  unit : write.iges.unit\n  mode write : write.iges.brep.mode\n  spline_continuity (read) : read.iges.bspline.continuity\nSee definition by  defparam, read/edit value by  param\n";
  di<<"unit (write) : "<<Interface_Static::CVal("write.iges.unit")<<"\n";
  di<<"mode  write  : "<<Interface_Static::CVal("write.iges.brep.mode")<<"\n";
  di<<"spline_continuity (read) : "<<Interface_Static::IVal("read.iges.bspline.continuity")<<" (0 : no modif, 1 : C1, 2 : C2)\n";
  di<<"\n To modifier, param nom_param new_val\n";
  return 0;
}


//--------------------------------------------------------------
// Function : tplosttrim
//
//--------------------------------------------------------------

static Standard_Integer XSDRAWIGES_tplosttrim (Draw_Interpretor& di, Standard_Integer n, const char** a) 
{
  Standard_Integer narg = n;

  const Handle(IFSelect_WorkSession) &WS = XSDRAW::Session();
  const Handle(Transfer_TransientProcess) &TP = WS->ReaderProcess();

  TColStd_Array1OfAsciiString strarg(1, 3);
  TColStd_Array1OfAsciiString typarg(1, 3);
  strarg.SetValue(1,"xst-type(CurveOnSurface)");
  strarg.SetValue(2,"xst-type(Boundary)");
  strarg.SetValue(3,"xst-type(Loop)");
  typarg.SetValue(1,"IGESGeom_TrimmedSurface");
  typarg.SetValue(2,"IGESGeom_BoundedSurface");
  typarg.SetValue(3,"IGESSolid_Face");
  if (TP.IsNull()) { di<<"No Transfer Read\n"; return 1; }
  Standard_Integer nbFaces = 0, totFaces = 0 ;
  Transfer_TransientProcess::Iterator itrp = TP->AbnormalResult();
  Standard_Integer k=0;
  if(narg > 1) {
    TCollection_AsciiString Arg(a[1]);
    for(k=1 ; k<=3;k++ ) {
      if(typarg.Value(k).Location(Arg,1,typarg.Value(k).Length()) != 0) break;
    }
  }   
  if( k == 4) {di<< "Invalid argument\n"; return 0; }
  for(Standard_Integer j = 1 ; j <= 3; j++) {
    TColStd_MapOfTransient aMap;
    if(narg == 1) k=j;
    Handle(TColStd_HSequenceOfTransient) list = XSDRAW_SelectFunctions::GiveList(WS,strarg.Value(k).ToCString());
    if (!list.IsNull()) itrp.Filter (list);
    else {
      di << "No untrimmed faces\n";
      return 0;
    }
    for (itrp.Start(); itrp.More(); itrp.Next()) {
      const Handle(Standard_Transient) &ent = itrp.SourceObject();
      Handle(TColStd_HSequenceOfTransient) super = WS->Sharings (ent);
      if (!super.IsNull()) {
	Standard_Integer nb = super->Length();
	if (nb > 0) {
	    for (Standard_Integer i = 1; i <= nb; i++)
	      if (super->Value(i)->IsKind (typarg.Value(k).ToCString())) {
		if(aMap.Add(super->Value(i))) nbFaces++;
	      }
	}
      }
    }
    if(nbFaces != 0) {
      if( j == 1 ) di << "Number of untrimmed faces: \n";
      switch(k){
      case 1:  
	di << "Trimmed Surface: \n"; break;
      case 2:
	di << "Bounded Surface: \n"; break;
      case 3:
	di << "Face: \n"; break;
      }

      TColStd_MapIteratorOfMapOfTransient itmap;
      for(itmap.Initialize(aMap); itmap.More(); itmap.Next()) {
        XSDRAW::Model()->Print (itmap.Key(), Message::DefaultMessenger());
        di << "  ";
      }
      di << "\n";
      di << "\nNumber:"<< nbFaces << "\n";
      totFaces += nbFaces;
    }
    if(narg > 1) break;
    nbFaces = 0;
  }
  
  if(totFaces == 0) di << "No untrimmed faces\n";
  else              di << "Total number :" << totFaces << "\n";
  return 0;
}
//-------------------------------------------------------------------
//  ####    Reliquat de methodes a reprendre    ####

//=======================================================================
// Function : PrintTransferInfo
// Purpose  : Print statistics information on transfer using MoniTool message management
// Created  : 18/01/98 DCE for S3767
// Modified : 
//=======================================================================
static void PrintTransferInfo (IGESControl_Reader &reader, const IFSelect_PrintCount mode)
{
  Standard_Integer nbWarn = 0, nbFail= 0, nbEntities =0, nbRoots = 0, nbResults = 0;  
  const Handle(Transfer_TransientProcess) &TP = reader.WS()->ReaderProcess();
  Handle(Message_Messenger) TF = TP->Messenger();
  const Handle(Interface_InterfaceModel) &model = TP->Model();
  if (! model.IsNull()) {
    nbEntities = model->NbEntities();
    nbRoots = TP->NbRoots();
    Transfer_TransientProcess::Iterator iterTrans = TP->RootResult(Standard_True);
    Handle(Dico_DictionaryOfInteger) dicoCountResult = new Dico_DictionaryOfInteger;
    Handle(Dico_DictionaryOfInteger) dicoCountMapping = new Dico_DictionaryOfInteger;
    for (iterTrans.Start(); iterTrans.More() ; iterTrans.Next() ) {
      nbResults++;
      // Init for dicoCountResult for IFSelect_ResultCount
      if ( mode == IFSelect_ResultCount ) {
	char mess[300];
	const Handle(Transfer_Binder) aBinder = iterTrans.Value();
	sprintf(mess,"\t%s",aBinder->ResultTypeName());
	Standard_Boolean deja;
	Standard_Integer& nb = dicoCountResult->NewItem(mess,deja);
	if (!deja) nb = 0;
	nb ++;	
      }
      // Init for dicoCountMapping for IFSelect_Mapping
      else if ( mode == IFSelect_Mapping ) {
	char mess[300];
	const Handle(Transfer_Binder) aBinder = iterTrans.Value();
	DeclareAndCast(IGESData_IGESEntity,igesEnt,iterTrans.SourceObject());
	
	sprintf(mess,"%d\t%d\t%s\t%s", igesEnt->TypeNumber(), igesEnt->FormNumber(),
		"%d", aBinder->ResultTypeName());
	//cout << mess << endl;
	Standard_Boolean deja;
	Standard_Integer& nb = dicoCountMapping->NewItem(mess,deja);
	if (!deja) nb = 0;
	nb ++;
      } 
    }

    Interface_CheckIterator checkIterator = TP->CheckList(Standard_False);
    Handle(Dico_DictionaryOfInteger) dicoCount = new Dico_DictionaryOfInteger;
    Handle(Dico_DictionaryOfTransient) dicoList = new Dico_DictionaryOfTransient;
    // Init the dicoCount dicoList and nbWarn ,nb Fail.
    for(checkIterator.Start(); checkIterator.More(); checkIterator.Next() ) {
      char mess[300];
      const Handle(Interface_Check) aCheck = checkIterator.Value(); 
      Handle(Standard_Transient) ent = model->Value(checkIterator.Number());
      DeclareAndCast(IGESData_IGESEntity,igesEnt,ent);
      Standard_Integer type = igesEnt->TypeNumber(), form = igesEnt->FormNumber();
      Standard_Integer nw = aCheck->NbWarnings(), nf = aCheck->NbFails(), i;
      for(i = 1; i <= nw; i++) {
	sprintf(mess,"\t W\t%d\t%d\t%s",type,form,aCheck->CWarning(i));
	Standard_Boolean deja;
	Standard_Integer& nb = dicoCount->NewItem(mess,deja);
	if (!deja) nb = 0;
	nb ++;
	Handle(Standard_Transient)& anitem = dicoList->NewItem(mess,deja);
	DeclareAndCast(TColStd_HSequenceOfInteger,alist,anitem);
	if (!deja) { alist = new TColStd_HSequenceOfInteger(); anitem = alist;  }
	alist->Append(model->Number(igesEnt)*2-1);
      }
      for(i = 1; i<= nf; i++) {
	sprintf(mess,"\t F\t%d\t%d\t%s",type,form,aCheck->CFail(i));
	// TF << mess << endl;
	Standard_Boolean deja;
	Standard_Integer& nb = dicoCount->NewItem(mess,deja);
	if (!deja) nb = 0;
	nb ++;
	Handle(Standard_Transient)& anitem = dicoList->NewItem(mess,deja);
	DeclareAndCast(TColStd_HSequenceOfInteger,alist,anitem);
	if (!deja) { alist = new TColStd_HSequenceOfInteger(); anitem = alist;  }
	alist->Append(model->Number(igesEnt)*2-1);
      }
      nbWarn += nw;
      nbFail += nf;
    }
    Message_Msg msg3000("IGES_3000");  // *************************
    TF->Send (msg3000, Message_Info); //smh#14
    
    switch (mode) {
    case IFSelect_GeneralInfo : {
      Message_Msg msg3005("IGES_3005");TF->Send(msg3005, Message_Info);
      Message_Msg msg3010("IGES_3010");msg3010.Arg(nbEntities);TF->Send(msg3010, Message_Info);
      Message_Msg msg3011("IGES_3011");msg3011.Arg(nbRoots);TF->Send(msg3011, Message_Info);      
      Message_Msg msg3015("IGES_3015");msg3015.Arg(nbResults);TF->Send(msg3015, Message_Info);
      Message_Msg msg3020("IGES_3020");msg3020.Arg(nbWarn);TF->Send(msg3020, Message_Info);
      Message_Msg msg3025("IGES_3025");msg3025.Arg(nbFail);TF->Send(msg3025, Message_Info);
      break;
    }
    case IFSelect_CountByItem : 
    case IFSelect_ListByItem : {
      Message_Msg msg3030("IGES_3030");
      TF->Send(msg3030, Message_Info);
      Dico_IteratorOfDictionaryOfInteger dicoCountIter(dicoCount);
      Dico_IteratorOfDictionaryOfTransient dicoListIter(dicoList);
      for(dicoCountIter.Start(),dicoListIter.Start(); 
	  dicoCountIter.More() && dicoListIter.More();
	  dicoCountIter.Next(),dicoListIter.Next()) {
	TF << dicoCountIter.Value() << dicoCountIter.Name() << endl;
	if (mode == IFSelect_ListByItem) {
	  DeclareAndCast(TColStd_HSequenceOfInteger, entityList, dicoListIter.Value());
	  Standard_Integer length = entityList->Length();
	  Message_Msg msg3035("IGES_3035");
	  TF->Send(msg3035, Message_Info);
	  char line[80];
	  sprintf(line,"\t\t\t");
	  TF << line ;
	  Standard_Integer nbInLine =0;
	  for(Standard_Integer i = 1; i <= length ; i++ ) {
	    // IDT_Out << (entityList->Value(i)) << " ";
	    sprintf(line,"\t %d", entityList->Value(i));
	    TF << line ;
	    if (++nbInLine == 6) {
	      nbInLine = 0;
	      sprintf(line,"\n\t\t\t");
	      TF << line ;
	    }
	  }
	  TF << endl ;
	}
      }
      break;
    }
    case IFSelect_ResultCount : { 
      Message_Msg msg3040("IGES_3040");TF->Send(msg3040, Message_Info);
      Message_Msg msg3011("IGES_3011");msg3011.Arg(nbRoots);TF->Send(msg3011, Message_Info);      
      Message_Msg msg3015("IGES_3015");msg3015.Arg(nbResults);TF->Send(msg3015, Message_Info);
      Message_Msg msg3045("IGES_3045");TF->Send(msg3045, Message_Info);
      Dico_IteratorOfDictionaryOfInteger dicoCountIter(dicoCountResult);
      for(dicoCountIter.Start(); dicoCountIter.More(); dicoCountIter.Next()) {
	TF << dicoCountIter.Value() << dicoCountIter.Name() << endl;
      }
      break;
    }
    case IFSelect_Mapping : { 
      Message_Msg msg3040("IGES_3050");TF->Send(msg3040, Message_Info);
      Message_Msg msg3011("IGES_3011");msg3011.Arg(nbRoots);TF->Send(msg3011, Message_Info);      
      Message_Msg msg3015("IGES_3015");msg3015.Arg(nbResults);TF->Send(msg3015, Message_Info);
      Message_Msg msg3045("IGES_3055");TF->Send(msg3045, Message_Info);
      // Add failed entities in dicoCountMapping
      if (nbRoots!=nbResults) {
	for( Standard_Integer i = 1; i <= nbRoots ; i++) {
	  DeclareAndCast(IGESData_IGESEntity, root, TP->Root(i));
	  if (!TP->IsBound(root)) {
	    char mess[300];
    
	    sprintf(mess,"%d\t%d \t%s\t%s", root->TypeNumber(), root->FormNumber(),
		    "%d", "Failed");
	    //cout << mess << endl;
	    Standard_Boolean deja;
	    Standard_Integer& nb = dicoCountMapping->NewItem(mess,deja);
	if (!deja) nb = 0;
	    nb ++;	    
	  }
	}
      }
      Dico_IteratorOfDictionaryOfInteger dicoCountIter(dicoCountMapping);
      for(dicoCountIter.Start(); dicoCountIter.More(); dicoCountIter.Next()) {
	char mess[80];
	sprintf(mess, dicoCountIter.Name().ToCString() , dicoCountIter.Value());
	TF << mess << endl; //dicoCountIter.Value() << dicoCountIter.Name() << endl;
      }
      break;
    }
    default: break;
    }
  }
}

//--------------------------------------------------------------
// Function : TPSTAT
//
//--------------------------------------------------------------
static Standard_Integer XSDRAWIGES_TPSTAT(Draw_Interpretor& di,Standard_Integer n, const char** a)
{
  Standard_Integer argc = n;
  const Standard_CString arg1 = a[1];
  const Handle(Transfer_TransientProcess) &TP = XSDRAW::Session()->ReaderProcess();

  IGESControl_Reader read;
//        ****    tpent        ****
  Handle(Interface_InterfaceModel) model = TP->Model();
  if (model.IsNull()) {di<<"No Transfer Read\n"; return -1;}
  Handle(XSControl_WorkSession) thesession = read.WS();
  thesession->SetReaderProcess(TP);
  Standard_Integer mod1 = 0;
  if (argc > 1) {
    char a2 = arg1[1]; if (a2 == '\0') a2 = '!';
    switch (arg1[0]) {
    case 'g' : PrintTransferInfo(read,IFSelect_GeneralInfo);break;
    case 'c' : PrintTransferInfo(read,IFSelect_CountByItem); break;
    case 'C' : PrintTransferInfo(read,IFSelect_ListByItem); break;
    case 'r' : PrintTransferInfo(read,IFSelect_ResultCount);break;
    case 's' : PrintTransferInfo(read,IFSelect_Mapping);break;
    case '?' : mod1 = -1; break;
    default  : mod1 = -2; break;
    }
  }
  if (mod1 < -1) di<<"Unknown Mode\n";
  if (mod1 < 0) {
    di<<"Modes available :\n"
      <<"g : general    c : checks (count)  C (list)\n"
      <<"r : number of CasCade resulting shapes\n"
      <<"s : mapping between IGES entities and CasCade shapes\n";
    if (mod1 < -1) return -1;
    return 0;
  }
  return 0;
}

static Standard_Integer etest(Draw_Interpretor& di, Standard_Integer argc, const char** a)
{
  if(argc < 3) {
    di<<"etest igesfile shape\n";
    return 0;
  }
  IGESControl_Reader aReader;
  aReader.ReadFile(a[1]);
  aReader.SetReadVisible(Standard_True);
  aReader.TransferRoots();
  TopoDS_Shape shape = aReader.OneShape();
  DBRep::Set(a[2],shape);
  return 0;
}

extern "C" {
static void cleanpilot ()
{
  XSDRAW::Session()->ClearData(1);
}
}


//--------------------------------------------------------------
// Function : Init(s)
//
//--------------------------------------------------------------

#include <IFSelect_SelectModelEntities.hxx>
#include <IFSelect_SelectModelRoots.hxx>
#include <XSSelect_SelectForTransfer.hxx>
#include <IGESSelect_SelectVisibleStatus.hxx>
#include <IGESSelect_SelectSubordinate.hxx>
#include <IGESSelect_SelectBypassGroup.hxx>
#include <IGESSelect_SelectBypassSubfigure.hxx>
#include <IGESSelect_SelectBasicGeom.hxx>
#include <IGESSelect_SelectFaces.hxx>
#include <IGESSelect_SelectPCurves.hxx>
#include <IFSelect_SelectType.hxx>
#include <IGESSelect_IGESTypeForm.hxx>
#include <IGESSelect_SignStatus.hxx>
#include <IFSelect_SignMultiple.hxx>
#include <IFSelect_SignCounter.hxx>
#include <IGESSelect_CounterOfLevelNumber.hxx>
#include <IGESSelect_SignLevelNumber.hxx>
#include <IGESSelect_IGESName.hxx>
#include <IGESSelect_SignColor.hxx>
#include <IGESBasic_SubfigureDef.hxx>
#include <IFSelect_SignType.hxx>

void XSDRAWIGES::InitSelect ()
{
  IGESControl_Controller::Init();
  Handle(XSControl_Controller) aCntl = XSControl_Controller::Recorded("IGES");

  XSDRAW::SetController (aCntl);

  const Handle(IFSelect_WorkSession) &WS = XSDRAW::Session();

  //   ---  SELECTIONS, SIGNATURES, COMPTEURS, EDITEURS
  //   --   BypassGroup / xst-model-roots

  // Should be already set by the above call to Customise
  Handle(IFSelect_SelectModelEntities) xma;
  Handle(Standard_Transient) xma1 = WS->NamedItem("xst-model-all");
  if (xma1.IsNull()) xma = new IFSelect_SelectModelEntities;
  else {
    xma = Handle(IFSelect_SelectModelEntities)::DownCast(xma1);
    WS->AddNamedItem ("xst-model-all",xma);
  }
  
  Handle(IFSelect_SelectModelRoots) xmr;
  Handle(Standard_Transient) xmr1 = WS->NamedItem("xst-model-roots");
  if (!xmr1.IsNull())
     xmr = Handle(IFSelect_SelectModelRoots)::DownCast(xmr1);
  else  {
    xmr = new IFSelect_SelectModelRoots;
    WS->AddNamedItem ("xst-model-roots",xmr);
  }

  Handle(XSSelect_SelectForTransfer) xtr;
  Handle(Standard_Transient) xtr1 = WS->NamedItem("xst-transferrable-roots");
  if (!xtr1.IsNull())
    xtr = Handle(XSSelect_SelectForTransfer)::DownCast(xtr1);
  else {
    xtr = new XSSelect_SelectForTransfer(WS->ReaderProcess());
    WS->AddNamedItem ("xst-transferrable-roots",xtr);
  }

  if (!xmr.IsNull()) {
    Handle(IGESSelect_SelectVisibleStatus) visa = new IGESSelect_SelectVisibleStatus;
    visa->SetInput(xmr);
    WS->AddNamedItem ("iges-visible-roots",visa);
    Handle(IGESSelect_SelectVisibleStatus) vist = new IGESSelect_SelectVisibleStatus;
    vist->SetInput(xtr);
    WS->AddNamedItem ("iges-visible-transf-roots",vist);
    Handle(IGESSelect_SelectVisibleStatus) blka = new IGESSelect_SelectVisibleStatus;
    blka->SetDirect (Standard_False);
    blka->SetInput(xmr);
    WS->AddNamedItem ("iges-blanked-roots",blka);
    Handle(IGESSelect_SelectVisibleStatus) blkt = new IGESSelect_SelectVisibleStatus;
    blkt->SetDirect (Standard_False);
    blkt->SetInput(xtr);
    WS->AddNamedItem ("iges-blanked-transf-roots",blkt);
    Handle(IGESSelect_SelectSubordinate) indp = new IGESSelect_SelectSubordinate (0);
    indp->SetInput (xma);
    WS->AddNamedItem ("iges-status-independant",indp);

    Handle(IGESSelect_SelectBypassGroup) sb = new IGESSelect_SelectBypassGroup;
    sb->SetInput(xmr);
    WS->AddNamedItem ("iges-bypass-group",sb);
    Handle(IGESSelect_SelectBypassSubfigure) sfi = new IGESSelect_SelectBypassSubfigure;
    sfi->SetInput(xmr);
    WS->AddNamedItem ("iges-bypass-subfigure",sfi);
    Handle(IGESSelect_SelectBypassGroup) sfb = new IGESSelect_SelectBypassGroup;
    sfb->SetInput(sfi);
    WS->AddNamedItem ("iges-bypass-group-subfigure",sfb);
    Handle(IGESSelect_SelectBasicGeom) sc3d = new IGESSelect_SelectBasicGeom(1);
    sc3d->SetInput(sfi);
    WS->AddNamedItem ("iges-curves-3d",sc3d);
    Handle(IGESSelect_SelectBasicGeom) sb3d = new IGESSelect_SelectBasicGeom(2);
    sb3d->SetInput(sfi);
    WS->AddNamedItem ("iges-basic-curves-3d",sb3d);
    Handle(IGESSelect_SelectBasicGeom) sbg = new IGESSelect_SelectBasicGeom(0);
    sbg->SetInput(sfi);
    WS->AddNamedItem ("iges-basic-geom",sbg);
    Handle(IGESSelect_SelectBasicGeom) srf = new IGESSelect_SelectBasicGeom(-1);
    srf->SetInput(sfi);
    WS->AddNamedItem ("iges-surfaces",srf);
    Handle(IGESSelect_SelectFaces) sfa = new IGESSelect_SelectFaces;
    sfa->SetInput(sfi);
    WS->AddNamedItem ("iges-faces",sfa );
    Handle(IGESSelect_SelectPCurves) spc = new IGESSelect_SelectPCurves(Standard_True);
    spc->SetInput(sfa);
    WS->AddNamedItem ("iges-pcurves",spc);

    Handle(IFSelect_SelectType) snosub = new IFSelect_SelectType
      (STANDARD_TYPE(IGESBasic_SubfigureDef));
    snosub->SetDirect(Standard_False);
    snosub->SetInput(xmr);
    WS->AddNamedItem ("iges-no-indep-subfigure-def",snosub);

    Handle(IGESSelect_IGESTypeForm) itf = new IGESSelect_IGESTypeForm(Standard_True);
    WS->AddNamedItem ("iges-type",itf);

    Handle(IGESSelect_SignStatus) sigst = new IGESSelect_SignStatus;
    Handle(IFSelect_SignMultiple) typsta = new IFSelect_SignMultiple
      ("IGES Type Form    Status");
    typsta->Add (itf,15);
    typsta->Add (sigst);
    WS->AddNamedItem ("iges-type-status",typsta);

    Handle(IFSelect_SignMultiple) typnam = new IFSelect_SignMultiple
      ("IGES Type Form  TypeName");
    typnam->Add (itf,4);
    typnam->Add (new IFSelect_SignType(Standard_True));
    WS->AddNamedItem ("iges-type-name",typnam);

    Handle(IFSelect_SignCounter) itfs = new IFSelect_SignCounter
      (itf,Standard_False,Standard_True);
    WS->AddNamedItem ("iges-types",itfs);
    Handle(IGESSelect_CounterOfLevelNumber) levs =
      new IGESSelect_CounterOfLevelNumber;
    WS->AddNamedItem ("iges-levels",levs);

    Handle(IGESSelect_SignLevelNumber) slev = new IGESSelect_SignLevelNumber(Standard_False);
    WS->AddNamedItem ("iges-level-number",slev);
    Handle(IGESSelect_IGESName) igna = new IGESSelect_IGESName;
    WS->AddNamedItem ("iges-name",igna);

    Handle(IGESSelect_SignColor) scol1 = new IGESSelect_SignColor (1);
    WS->AddNamedItem ("iges-color-number",scol1);
    Handle(IGESSelect_SignColor) scol2 = new IGESSelect_SignColor (2);
    WS->AddNamedItem ("iges-color-name",scol2);
    Handle(IGESSelect_SignColor) scol3 = new IGESSelect_SignColor (3);
    WS->AddNamedItem ("iges-color-rgb",scol3);
    Handle(IGESSelect_SignColor) scol4 = new IGESSelect_SignColor (4);
    WS->AddNamedItem ("iges-color-red",scol4);
    Handle(IGESSelect_SignColor) scol5 = new IGESSelect_SignColor (5);
    WS->AddNamedItem ("iges-color-green",scol5);
    Handle(IGESSelect_SignColor) scol6 = new IGESSelect_SignColor (6);
    WS->AddNamedItem ("iges-color-blue",scol6);

    WS->SetSignType( typnam );
  }

  atexit (cleanpilot);
}


//=======================================================================
//function : InitToBRep
//purpose  : 
//=======================================================================

void  XSDRAWIGES::InitToBRep (Draw_Interpretor& theCommands)
{
  const char* g = "DE: IGES";
  theCommands.Add("igesbrep",     "igesbrep [file else already loaded model] [name DRAW]",     __FILE__, igesbrep,              g);
  theCommands.Add("testreadiges", "testreadiges [file else already loaded model] [name DRAW]", __FILE__, testread,              g);
  theCommands.Add("igesread",     "igesread [file else already loaded model] [name DRAW]",     __FILE__, igesbrep,              g);
  theCommands.Add("igesparam",    "igesparam ->list, + name ->one param, + name val->change",  __FILE__, igesparam,             g);
  theCommands.Add("TPSTAT",       " ",                                                         __FILE__, XSDRAWIGES_TPSTAT,     g);
  theCommands.Add("tplosttrim",   "number of untrimmed faces during last transfer",            __FILE__, XSDRAWIGES_tplosttrim, g);
  theCommands.Add("etest",        "test of eviewer",                                           __FILE__, etest,                 g);

}


//=======================================================================
//function : InitFromBRep
//purpose  : 
//=======================================================================

void  XSDRAWIGES::InitFromBRep (Draw_Interpretor& theCommands)
{
  const char* g = "DE: IGES";
  theCommands.Add("brepiges",      "brepiges sh1 [+sh2 [+sh3 ..]] filename.igs", __FILE__, brepiges,  g);
  theCommands.Add("testwriteiges", "testwriteiges filename.igs shape",           __FILE__, testwrite, g);
}
