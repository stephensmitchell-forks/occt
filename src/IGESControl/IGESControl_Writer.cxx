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

//cky 16.01.99 Remove couts.
//rln 28.12.98 CCI60005

#include <Bnd_Box.hxx>
#include <BndLib_Add3dCurve.hxx>
#include <BndLib_AddSurface.hxx>
#include <BRepBndLib.hxx>
#include <BRepToIGES_BREntity.hxx>
#include <BRepToIGESBRep_Entity.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Geometry.hxx>
#include <Geom_Surface.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <GeomToIGES_GeomCurve.hxx>
#include <GeomToIGES_GeomSurface.hxx>
#include <gp_XYZ.hxx>
#include <IGESControl_Controller.hxx>
#include <IGESControl_Writer.hxx>
#include <IGESData.hxx>
#include <IGESData_DirChecker.hxx>
#include <IGESData_IGESEntity.hxx>
#include <IGESData_IGESModel.hxx>
#include <IGESData_IGESWriter.hxx>
#include <IGESData_Protocol.hxx>
#include <IGESData_GeneralModule.hxx>
#include <Interface_Macros.hxx>
#include <Interface_Static.hxx>
#include <Interface_Graph.hxx>
#include <Interface_EntityIterator.hxx>
#include <Message_ProgressIndicator.hxx>
#include <OSD_OpenFile.hxx>
#include <ShapeAnalysis_ShapeTolerance.hxx>
#include <Standard_Stream.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS_Shape.hxx>
#include <Transfer_FinderProcess.hxx>
#include <XSAlgo.hxx>
#include <XSAlgo_AlgoContainer.hxx>

#include <errno.h>
IGESControl_Writer::IGESControl_Writer ()
: myTP (new Transfer_FinderProcess(10000)),
  myIsComputed (Standard_False)
{
  InitProtocol();
  myWriteMode = Interface_Static::IVal ("write.iges.brep.mode");
  myModel = IGESData::NewModel();
  SetUnitName(Interface_Static::CVal ("write.iges.unit"));
}

IGESControl_Writer::IGESControl_Writer (const Standard_CString unit, const Standard_Integer modecr)
: myTP (new Transfer_FinderProcess(10000)),
  myWriteMode (modecr), myIsComputed (Standard_False)
{
  InitProtocol();
  myModel = IGESData::NewModel();
  SetUnitName(unit);
}

IGESControl_Writer::IGESControl_Writer (const Handle(IGESData_IGESModel)& model, const Standard_Integer modecr)
: myTP (new Transfer_FinderProcess(10000)),
  myModel (model), 
  myWriteMode (modecr), myIsComputed (Standard_False)
{
  InitProtocol();
}

Standard_Boolean IGESControl_Writer::AddShape (const TopoDS_Shape& theShape)
{
  if (theShape.IsNull()) return Standard_False;
  
  // for progress indication
  Handle(Message_ProgressIndicator) progress = myTP->GetProgress();
  if ( ! progress.IsNull() ) { 
    Standard_Integer nbfaces=0;
    for( TopExp_Explorer exp(theShape,TopAbs_FACE); exp.More(); exp.Next() )
      nbfaces++;
    progress->SetScale ( "Faces", 0, nbfaces, 1 );
  }
  
  XSAlgo::AlgoContainer()->PrepareForTransfer();
  
  //  modified by NIZHNY-EAP Tue Aug 29 11:16:54 2000 ___BEGIN___
  Handle(Standard_Transient) info;
  Standard_Real Tol = Interface_Static::RVal("write.precision.val");
  Standard_Real maxTol = Interface_Static::RVal("read.maxprecision.val");
  TopoDS_Shape Shape = XSAlgo::AlgoContainer()->ProcessShape( theShape, Tol, maxTol, 
                                                              "write.iges.resource.name", 
                                                              "write.iges.sequence", info,
                                                              progress );
  //  modified by NIZHNY-EAP Tue Aug 29 11:17:01 2000 ___END___
  BRepToIGES_BREntity   B0;  B0.SetTransferProcess(myTP); B0.SetModel(myModel);
  BRepToIGESBRep_Entity B1;  B1.SetTransferProcess(myTP); B1.SetModel(myModel);
  Handle(IGESData_IGESEntity) ent = myWriteMode? B1.TransferShape(Shape) : B0.TransferShape(Shape);

  if(ent.IsNull())
    return Standard_False;
//  modified by NIZHNY-EAP Tue Aug 29 11:37:18 2000 ___BEGIN___
  XSAlgo::AlgoContainer()->MergeTransferInfo(myTP, info);
//  modified by NIZHNY-EAP Tue Aug 29 11:37:25 2000 ___END___
  
  //22.10.98 gka BUC60080

  Standard_Integer oldnb = myModel->NbEntities();
  Standard_Boolean aent = AddEntity (ent);
  Standard_Integer newnb = myModel->NbEntities();

  Standard_Real oldtol = myModel->GlobalSection().Resolution(), newtol;
  
  Standard_Integer tolmod = Interface_Static::IVal("write.precision.mode");
  if (tolmod == 2)
    newtol = Interface_Static::RVal("write.precision.val");
  else {
    ShapeAnalysis_ShapeTolerance stu; 
    Standard_Real Tolv = stu.Tolerance (Shape, tolmod, TopAbs_VERTEX);
    Standard_Real Tole = stu.Tolerance (Shape, tolmod, TopAbs_EDGE); 

    if (tolmod == 0 ) {   //Average
      Standard_Real Tol1 = (Tolv + Tole) / 2;
      newtol = (oldtol * oldnb + Tol1 * (newnb - oldnb)) / newnb;
    }
    else if (tolmod < 0) {//Least
      newtol = Min (Tolv, Tole);
      if (oldnb > 0) newtol = Min (oldtol, newtol);
    }
    else {                //Greatest
      newtol = Max (Tolv, Tole);
      if (oldnb > 0) newtol = Max (oldtol, newtol);
    }
  }
  
  IGESData_GlobalSection gs = myModel->GlobalSection();
  gs.SetResolution (newtol / gs.UnitValue());//rln 28.12.98 CCI60005

  //#34 22.10.98 rln BUC60081
  Bnd_Box box;
  BRepBndLib::Add (Shape, box);
  if (!(box.IsVoid() || box.IsOpenXmax() || box.IsOpenYmax() || box.IsOpenZmax() || box.IsOpenXmin() || box.IsOpenYmin() || box.IsOpenZmin())){
    Standard_Real aXmin, aYmin, aZmin, aXmax, aYmax, aZmax;
    box.Get (aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);
    gs.MaxMaxCoords (gp_XYZ (aXmax / gs.UnitValue(),
	     		     aYmax / gs.UnitValue(),
	  		     aZmax / gs.UnitValue()));
    gs.MaxMaxCoords (gp_XYZ (aXmin / gs.UnitValue(),
 			     aYmin / gs.UnitValue(),
			     aZmin / gs.UnitValue()));
  }

  myModel->SetGlobalSection(gs);

  return aent;
}

Standard_Boolean IGESControl_Writer::AddGeom (const Handle(Standard_Transient)& geom)
{
  if (geom.IsNull() || !geom->IsKind (STANDARD_TYPE (Geom_Geometry)))
    return Standard_False;
  DeclareAndCast(Geom_Curve,Curve,geom);
  DeclareAndCast(Geom_Surface,Surf,geom);
  Handle(IGESData_IGESEntity) ent; 

//  On reconnait : Curve et Surface de Geom
//   quid de Point; Geom2d ?

//  GeomToIGES_GeomPoint GP;
  GeomToIGES_GeomCurve GC;    GC.SetModel(myModel);
  GeomToIGES_GeomSurface GS;  GS.SetModel(myModel);

  //#34 22.10.98 rln BUC60081
  IGESData_GlobalSection gs = myModel->GlobalSection();
  Bnd_Box box;

  if (!Curve.IsNull()) {
    ent = GC.TransferCurve(Curve,Curve->FirstParameter(),Curve->LastParameter());
    BndLib_Add3dCurve::Add (GeomAdaptor_Curve (Curve), 0, box); }
  else if (!Surf.IsNull()) {
    Standard_Real U1,U2,V1,V2;
    Surf->Bounds(U1,U2,V1,V2);
    ent = GS.TransferSurface(Surf,U1,U2,V1,V2);
    BndLib_AddSurface::Add (GeomAdaptor_Surface (Surf), 0, box);
  }

  Standard_Real aXmin, aYmin, aZmin, aXmax, aYmax, aZmax;
  box.Get (aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);
  gs.MaxMaxCoords (gp_XYZ (aXmax / gs.UnitValue(),
			   aYmax / gs.UnitValue(),
			   aZmax / gs.UnitValue()));
  gs.MaxMaxCoords (gp_XYZ (aXmin / gs.UnitValue(),
			   aYmin / gs.UnitValue(),
			   aZmin / gs.UnitValue()));
  myModel->SetGlobalSection(gs);
  return AddEntity (ent);
}

Standard_Boolean IGESControl_Writer::AddEntity (const Handle(IGESData_IGESEntity)& ent)
{
  if (ent.IsNull()) return Standard_False;
  myModel->AddWithRefs(ent,IGESControl_Controller::DefineProtocol());
  myIsComputed = Standard_False;
  return Standard_True;
}

void IGESControl_Writer::ComputeModel ()
{
  if (!myIsComputed) {
    ComputeStatus();
    AutoCorrectModel();
    myIsComputed = Standard_True;
  }
}

Standard_Boolean IGESControl_Writer::Write (Standard_OStream& S, const Standard_Boolean fnes)
{
  if (!S) return Standard_False;
  ComputeModel();
  Standard_Integer nbEnt = myModel->NbEntities();
  if(!nbEnt)
    return Standard_False;
  IGESData_IGESWriter IW (myModel);
//  ne pas oublier le mode fnes ... a transmettre a IW
  IW.SendModel ();
  if (fnes) IW.WriteMode() = 10;
  return IW.Print(S);
}

Standard_Boolean IGESControl_Writer::Write (const Standard_CString file, const Standard_Boolean fnes)
{
  ofstream fout;
  OSD_OpenStream(fout,file,ios::out);
  if (!fout) return Standard_False;

  const Standard_Boolean res = Write (fout,fnes);

  errno = 0;
  fout.close();
  return (fout.good() && res && !errno);
}

void IGESControl_Writer::InitProtocol ()
{
  IGESControl_Controller::Init();
  const Handle(IGESData_Protocol) &aProtocol = IGESControl_Controller::DefineProtocol();
  myGLib = Interface_GeneralLib (aProtocol);
}

void IGESControl_Writer::SetUnitName (const Standard_CString name)
{
  if (myModel.IsNull()) return;

  const Standard_Integer flag = IGESData::UnitNameFlag (name);
  IGESData_GlobalSection GS = myModel->GlobalSection();
  if (GS.UnitFlag() == 3) {
    char* nam = (char *)name;
    if (name[1] == 'H') nam = (char *)&name[2];
    GS.SetUnitName (new TCollection_HAsciiString(nam));
    myModel->SetGlobalSection (GS);
  }
  else if (flag >= 1 && flag <= 11) {
    //! Set a new unit from its flag (param 14 of Global Section)
    //! Returns True if done, False if <flag> is incorrect
    Handle(TCollection_HAsciiString) name = GS.UnitName();
    Standard_CString nam = IGESData::UnitFlagName (flag);
    if (nam[0] != '\0') name = new TCollection_HAsciiString (nam);
    GS.SetUnitFlag (flag);
    GS.SetUnitName (name);
    myModel->SetGlobalSection (GS);

    Standard_Real unit = GS.UnitValue();
    if (unit <= 0.) return;
    if (unit != 1.) {
      GS.SetMaxLineWeight (GS.MaxLineWeight () / unit);
      GS.SetResolution    (GS.Resolution    () / unit);
      GS.SetMaxCoord      (GS.MaxCoord      () / unit);
      myModel->SetGlobalSection (GS);
    }
  }
}

void IGESControl_Writer::ComputeStatus ()
{
  if (myModel.IsNull()) return;
  const Standard_Integer nb = myModel->NbEntities();
  if (nb == 0) return;
  TColStd_Array1OfInteger subs (0,nb); subs.Init(0); // gere Subordinate Status
  Interface_Graph G (myModel);    // gere & memorise UseFlag
  G.ResetStatus();

//  2 phases : d abord on fait un calcul d ensemble. Ensuite on applique
//             Tout le modele est traite, pas de jaloux

//  Chaque entite va donner une contribution sur ses descendents propres :
//  pour Subordinate (1 ou 2 cumulables), pour UseFlag (1 a 6 exclusifs)
//    (6 depuis IGES-5.2)

//  Pour Subordinate : Drawing et 402 (sauf p-e dimensioned geometry ?) donnent
//   Logical, le reste implique Physical (sur descendants directs propres)

//  Pour UseFlag, un peu plus complique :
//  D une part, les UseFlag se propagent aux descendants directs ou non
//  D autre part les cas sont plus compliques (et pas aussi clairs)

//  ATTENTION, on ne peut traiter que ce qui se deduit du graphe en s appuyant
//  sur les "IGES Type Number", on n a pas le droit ici d acceder a la
//  description specifique des differents types : traites par AutoCorrect.
//  Exemple : une courbe est 3D ou parametrique 2D(UV), non seulement selon son
//  ascendant, mais selon le role qu elle y joue (ex. pour CurveOnSurface :
//  CurveUV/Curve3D)
//  Traites actuellement (necessaires) :
//  1(Annotation), aussi 4(pour maillage). 5(ParamUV) traite par AutoCorrect

  Standard_Integer i; // svv Jan11 2000 : porting on DEC
  for (i = 1; i <= nb; i ++) {
//  Subordinate (sur directs en propre seulement)
    Handle(IGESData_IGESEntity) ent = myModel->Entity(i);
    const Standard_Integer igt = ent->TypeNumber();
    Interface_EntityIterator sh;
    ent->OwnShared (sh);
    for (sh.Start(); sh.More(); sh.Next()) {
      const Standard_Integer nums = myModel->Number(sh.Value());
      if (igt == 402 || igt == 404) subs.SetValue (nums,subs.Value(nums) | 2);
      else subs.SetValue (nums,subs.Value(nums) | 1);
    }
//  UseFlag (a propager)
    if (igt / 100 == 2) {
      G.GetFromEntity(ent,Standard_True,1);               // Annotation
      G.GetFromEntity(ent,Standard_False,ent->UseFlag());
    } else if (igt == 134 || igt == 116 || igt == 132) {
      Interface_EntityIterator sh = G.Sharings(ent);      // Maillage ...
      if (sh.NbEntities() > 0) G.GetFromEntity(ent,Standard_True,4);
    }
  }

//  A present, on va appliquer tout cela "de force"
//  Seule exception : des UseFlags non nuls deja en place sont laisses

  for (i = 1; i <= nb; i ++) {
    Handle(IGESData_IGESEntity) ent = myModel->Entity(i);
    Standard_Integer bl = ent->BlankStatus();
    Standard_Integer uf = ent->UseFlag();
    if (uf == 0) uf = G.Status(i);
    Standard_Integer hy = ent->HierarchyStatus();
    ent->InitStatus(bl,subs.Value(i),uf,hy);
  }
}

Standard_Boolean IGESControl_Writer::AutoCorrect (const Handle(IGESData_IGESEntity)& ent)
{
  if (ent.IsNull()) return Standard_False;

  Standard_Boolean done = Standard_False;

//    Corrections dans les Assocs (les Props restent attachees a l Entite)
  Interface_EntityIterator iter = ent->Associativities();
  for (iter.Start(); iter.More(); iter.Next())
  {
    const Handle(Standard_Transient) &aent = iter.Value();
    if (!aent.IsNull() && myModel->Number(aent) == 0)
    {
      DeclareAndCast(IGESData_IGESEntity,subent,aent);
      subent->Dissociate(ent);
      done = Standard_True;
    }
  }

//    Corrections specifiques
  //szv_c1:done |= ent->DirChecker().Correct(ent); should be called from OwnCorrect

  done |= ent->OwnCorrect();

  return done;
}

Standard_Integer IGESControl_Writer::AutoCorrectModel ()
{
  if (myModel.IsNull()) return 0;

  Standard_Integer res = 0;
  const Standard_Integer nb = myModel->NbEntities();
  for (Standard_Integer i = 1; i <= nb; i ++) {
    if (AutoCorrect (myModel->Entity(i))) res ++;
  }
  return res;
}
