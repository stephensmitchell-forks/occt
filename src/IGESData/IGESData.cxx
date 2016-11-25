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

// Integration to ensure SCCS base integrity
//#58 rln 28.12.98 changing default values for Global Section
//pdn 11.01.99 including <stdio.h> for compilation on NT
//gka 19.01.99 changing date parameters and  number of IGES version, adding parameter(ApllicationProtocol)
//#65 rln 12.02.99 S4151 (explicitly force YYMMDD.HHMMSS before Y2000 and YYYYMMDD.HHMMSS after Y2000)

#include <IGESData.hxx>
#include <IGESData_DefaultGeneral.hxx>
#include <IGESData_DefaultSpecific.hxx>
#include <IGESData_GlobalSection.hxx>
#include <IGESData_IGESModel.hxx>
#include <IGESData_Protocol.hxx>
#include <Interface_InterfaceModel.hxx>
#include <Interface_Static.hxx>
#include <Interface_Version.hxx>
#include <Message_MsgFile.hxx>
#include <OSD_Process.hxx>
#include <Quantity_Date.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Real.hxx>
#include <TCollection_HAsciiString.hxx>

#include <stdio.h>

static Handle(IGESData_Protocol)  gIGESProto;
static Handle(IGESData_IGESModel) gIGESModel;

static Handle(IGESData_DefaultGeneral)    stmod;
static Handle(IGESData_DefaultSpecific)   speci;

void IGESData::Init ()
{
  if (gIGESProto.IsNull()) gIGESProto = new IGESData_Protocol;
  if (stmod.IsNull()) stmod = new IGESData_DefaultGeneral;
  if (speci.IsNull()) speci = new IGESData_DefaultSpecific;

  //  et modele template "iges"
  if (!gIGESModel.IsNull()) return;

  OSD_Process process;

  // Static Parameters - see also Static Standards
  // -----------------
  Interface_Static::Standards();

  Interface_Static::Init ("XSTEP","read.iges.bspline.approxd1.mode",'e',"");
  Interface_Static::Init ("XSTEP","read.iges.bspline.approxd1.mode",'&',"ematch 0");
  Interface_Static::Init ("XSTEP","read.iges.bspline.approxd1.mode",'&',"eval Off");
  Interface_Static::Init ("XSTEP","read.iges.bspline.approxd1.mode",'&',"eval On");
  Interface_Static::SetIVal ("read.iges.bspline.approxd1.mode",0);

  Interface_Static::Init ("XSTEP","read.iges.bspline.continuity",'i',"1");
  Interface_Static::Init ("XSTEP","read.iges.bspline.continuity",'&',"imin 0");
  Interface_Static::Init ("XSTEP","read.iges.bspline.continuity",'&',"imax 2");

  Interface_Static::Init ("XSTEP","write.iges.header.receiver",'t',"");//58 rln Unknown
  Interface_Static::Init ("XSTEP","write.iges.header.author"  ,'t',process.UserName().ToCString());
  Interface_Static::Init ("XSTEP","write.iges.header.company" ,'t',"");//58 rln Matra Datavision

  Interface_Static::Init ("XSTEP","write.iges.unit",'e',"");
  Interface_Static::Init ("XSTEP","write.iges.unit",'&',"enum 1");
  Interface_Static::Init ("XSTEP","write.iges.unit",'&',"eval INCH");  // 1
  Interface_Static::Init ("XSTEP","write.iges.unit",'&',"eval MM");    // 2
  Interface_Static::Init ("XSTEP","write.iges.unit",'&',"eval ??");    // 3
  Interface_Static::Init ("XSTEP","write.iges.unit",'&',"eval FT");    // 4
  Interface_Static::Init ("XSTEP","write.iges.unit",'&',"eval MI");    // 5
  Interface_Static::Init ("XSTEP","write.iges.unit",'&',"eval M");     // 6
  Interface_Static::Init ("XSTEP","write.iges.unit",'&',"eval KM");    // 7
  Interface_Static::Init ("XSTEP","write.iges.unit",'&',"eval MIL");   // 8
  Interface_Static::Init ("XSTEP","write.iges.unit",'&',"eval UM");    // 9
  Interface_Static::Init ("XSTEP","write.iges.unit",'&',"eval CM");    //10
  Interface_Static::Init ("XSTEP","write.iges.unit",'&',"eval UIN");   //11
  Interface_Static::SetCVal ("write.iges.unit","MM");

  Interface_Static::Init ("XSTEP","write.iges.brep.mode",'e',"");
  Interface_Static::Init ("XSTEP","write.iges.brep.mode",'&',"ematch 0");
  Interface_Static::Init ("XSTEP","write.iges.brep.mode",'&',"eval Faces");
  Interface_Static::Init ("XSTEP","write.iges.brep.mode",'&',"eval BRep");
  Interface_Static::SetIVal ("write.iges.brep.mode",0);

  //S4181 pdn 23.04.99: adding new parameter handling writing of elementary surfaces
  Interface_Static::Init ("XSTEP","write.convertsurface.mode",'e',"");
  Interface_Static::Init ("XSTEP","write.convertsurface.mode",'&',"ematch 0");
  Interface_Static::Init ("XSTEP","write.convertsurface.mode",'&',"eval Off");
  Interface_Static::Init ("XSTEP","write.convertsurface.mode",'&',"eval On");
  Interface_Static::SetIVal ("write.convertsurface.mode",0);

  // abv 15.11.00: ShapeProcessing
  Interface_Static::Init ("XSTEP","write.iges.resource.name",'t',"IGES");
  Interface_Static::Init ("XSTEP","read.iges.resource.name",'t',"IGES");
  Interface_Static::Init ("XSTEP","write.iges.sequence",'t',"ToIGES");
  Interface_Static::Init ("XSTEP","read.iges.sequence",'t',"FromIGES");
  
  // sln 11.06.2002 OCC448 : Initialize "read.onlyvisible" parameter  to control transfering 
  // invisiable sub entities which logicaly depend on the grouping entities
  Interface_Static::Init ("XSTEP","read.iges.onlyvisible",'e',"");
  Interface_Static::Init ("XSTEP","read.iges.onlyvisible",'&',"ematch 0");
  Interface_Static::Init ("XSTEP","read.iges.onlyvisible",'&',"eval Off");
  Interface_Static::Init ("XSTEP","read.iges.onlyvisible",'&',"eval On");
  Interface_Static::SetIVal ("read.iges.onlyvisible",0);

  //gka added parameter for reading failed entities. 19.07
  Interface_Static::Init ("XSTEP","read.iges.faulty.entities",'e',"");
  Interface_Static::Init ("XSTEP","read.iges.faulty.entities",'&',"ematch 0");
  Interface_Static::Init ("XSTEP","read.iges.faulty.entities",'&',"eval Off");
  Interface_Static::Init ("XSTEP","read.iges.faulty.entities",'&',"eval On");
  Interface_Static::SetIVal ("read.iges.faulty.entities",0);

  //ika added parameter for writing planes mode 2.11.2012 
  Interface_Static::Init ("XSTEP","write.iges.plane.mode",'e',"");
  Interface_Static::Init ("XSTEP","write.iges.plane.mode",'&',"ematch 0");
  Interface_Static::Init ("XSTEP","write.iges.plane.mode",'&',"eval Plane");
  Interface_Static::Init ("XSTEP","write.iges.plane.mode",'&',"eval BSpline");
  Interface_Static::SetIVal ("write.iges.plane.mode",0);

   //ika added parameter for writing offset curves like BSplines 12.04.2013 
  Interface_Static::Init ("XSTEP","write.iges.offset.mode",'e',"");
  Interface_Static::Init ("XSTEP","write.iges.offset.mode",'&',"ematch 0");
  Interface_Static::Init ("XSTEP","write.iges.offset.mode",'&',"eval On");
  Interface_Static::Init ("XSTEP","write.iges.offset.mode",'&',"eval Off");
  Interface_Static::SetIVal ("write.iges.offset.mode",0);
  // Message File for IGES
  // -----------------

  Message_MsgFile::LoadFromEnv ("CSF_XSMessage","IGES");

  // Creating the Global Section
  //----------------------------

  IGESData_GlobalSection GS;

  //#58 rln 28.12.98 changing default values for Global Section
  char procver[80];
  sprintf (procver, XSTEP_PROCESSOR_VERSION, "IGES");
  Handle(TCollection_HAsciiString) gsys = new TCollection_HAsciiString (procver);
  Interface_Static::Init ("XSTEP","write.iges.header.product" ,'t',procver);
  
  OSD_Process system;
  Quantity_Date ladate = system.SystemDate(); 
  const Standard_Integer year = ladate.Year();
  Handle(TCollection_HAsciiString) datestr = GS.NewDateString(0,0,0,0,0,0,( year < 2000 ? 0 : -1 )); //#65 rln 12.02.99 S4151 (explicitly force YYYYMMDD.HHMMSS after Y2000)
  GS.SetSeparator (',');
  GS.SetEndMark   (';');
  GS.SetSendName (new TCollection_HAsciiString(Interface_Static::CVal("write.iges.header.product")));
  GS.SetFileName (new TCollection_HAsciiString("Filename.iges"));
  GS.SetSystemId (new TCollection_HAsciiString(XSTEP_SYSTEM_VERSION));//#58 rln "MATRA-DATAVISION EUCLID-QUANTUM"
  GS.SetInterfaceVersion (gsys);
  GS.SetIntegerBits      ( IntegerSize() );
  GS.SetMaxPower10Single ( RealLast10Exp() );
  GS.SetMaxDigitsSingle  ( RealDigits() );
  GS.SetMaxPower10Double ( RealLast10Exp() );
  GS.SetMaxDigitsDouble  ( RealDigits() );
  GS.SetReceiveName (Interface_Static::Static("write.iges.header.receiver")->HStringValue());
  GS.SetScale       (1.0);
  GS.SetUnitFlag    (Interface_Static::IVal("write.iges.unit"));
  GS.SetUnitName    (new TCollection_HAsciiString(Interface_Static::CVal("write.iges.unit")));
  GS.SetLineWeightGrad (1);
  GS.SetMaxLineWeight  (0.01);
  GS.SetDate        (datestr);
  GS.SetResolution  (0.0001);
  GS.SetMaxCoord    (0.0 /*1000.0*/);//22.10.98 rln BUC60081
  GS.SetAuthorName  (Interface_Static::Static("write.iges.header.author")->HStringValue());
  GS.SetCompanyName (Interface_Static::Static("write.iges.header.company")->HStringValue());
  GS.SetIGESVersion (11);         // pour IGES-5.3 //gka 19.01.99
  GS.SetDraftingStandard (0);
  GS.SetLastChangeDate  (datestr);
  GS.SetApplicationProtocol(new TCollection_HAsciiString("")); //gka 19.01.99

  // Creating the Model
  //-------------------

  gIGESModel = new IGESData_IGESModel;
  gIGESModel->SetGlobalSection(GS);
}

const Handle(IGESData_Protocol) & IGESData::Protocol ()
{
  return gIGESProto;
}

Handle(IGESData_IGESModel) IGESData::NewModel ()
{
  Handle(IGESData_IGESModel) newmod;
  if (!gIGESModel.IsNull()) {
    newmod = new IGESData_IGESModel;
    newmod->GetFromAnother (gIGESModel);
  }
  return newmod;
}

Standard_Integer IGESData::UnitNameFlag  (const Standard_CString name)
{
  char* nam = (char *)&name[0];
  if (name[1] == 'H') {
    nam = (char *)&name[2];
  }
  if (!strcmp (nam,"INCH")) return 1;
  if (!strcmp (nam,"IN"))   return 1;
  if (!strcmp (nam,"MM"))   return 2;
  if (!strcmp (nam,"FT"))   return 4;
  if (!strcmp (nam,"MI"))   return 5;
  if (!strcmp (nam,"M"))    return 6;
  if (!strcmp (nam,"KM"))   return 7;
  if (!strcmp (nam,"MIL"))  return 8;
  if (!strcmp (nam,"UM"))   return 9;
  if (!strcmp (nam,"CM"))   return 10;
  if (!strcmp (nam,"UIN"))  return 11;
  return 0;
}

Standard_Real IGESData::UnitFlagValue (const Standard_Integer flag)
{
  switch (flag) {
    case  1 : return 0.0254;
    case  2 : return 0.001;
    case  3 : return 1.;
    case  4 : return 0.3048;
    case  5 : return 1609.27;
    case  6 : return 1.;
    case  7 : return 1000.;
    case  8 : return 0.0000254;
    case  9 : return 0.000001;
    case 10 : return 0.01;
    case 11 : return 0.0000000254;
    default : break;
  }
  return 0.;
}

Standard_CString IGESData::UnitFlagName (const Standard_Integer flag)
{
  Standard_CString name = "";
  switch (flag) {
    case  1 : name = "INCH";  break;
    case  2 : name = "MM";    break;
    case  4 : name = "FT";    break;
    case  5 : name = "MI";    break;
    case  6 : name = "M";     break;
    case  7 : name = "KM";    break;
    case  8 : name = "MIL";   break;
    case  9 : name = "UM";    break;
    case 10 : name = "CM";    break;
    case 11 : name = "UIN";   break;
    default : break;
  }
  return name;
}

Standard_CString IGESData::IGESVersionName (const Standard_Integer flag)
{
  switch (flag) {
    case 1 : return "1.0";
    case 2 : return "ANSI Y14.26M-1981";
    case 3 : return "2.0";
    case 4 : return "3.0";
    case 5 : return "ANSI Y14.26M-1987";
    case 6 : return "4.0";
    case 7 : return "ANSI Y14.26M-1989";
    case 8 : return "5.0";
    case 9 : return "5.1";
    case 10: return "5.2";
    case 11: return "5.3";
    default : break;
  }
  return "";
}

Standard_CString IGESData::DraftingName (const Standard_Integer flag)
{
  switch (flag) {
    case 0 : return "(None)";
    case 1 : return "ISO";
    case 2 : return "AFNOR";
    case 3 : return "ANSI";
    case 4 : return "BSI";
    case 5 : return "CSA";
    case 6 : return "DIN";
    case 7 : return "JIS";
    default : break;
  }
  return "";
}
