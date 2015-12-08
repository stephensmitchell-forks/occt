// Created on: 2000-05-30
// Created by: Sergey MOZOKHIN
// Copyright (c) 2000-2014 OPEN CASCADE SAS
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


#include <AIS_InteractiveContext.hxx>
#include <Aspect_TypeOfMarker.hxx>
#include <Bnd_Box.hxx>
#include <DBRep.hxx>
#include <Draw.hxx>
#include <Draw_Interpretor.hxx>
#include <Draw_PluginMacro.hxx>
#include <Draw_ProgressIndicator.hxx>
#include <Graphic3d_MaterialAspect.hxx>
#include <MeshVS_DataMapOfIntegerAsciiString.hxx>
#include <MeshVS_DeformedDataSource.hxx>
#include <MeshVS_Drawer.hxx>
#include <MeshVS_DrawerAttribute.hxx>
#include <MeshVS_ElementalColorPrsBuilder.hxx>
#include <MeshVS_Mesh.hxx>
#include <MeshVS_MeshEntityOwner.hxx>
#include <MeshVS_MeshPrsBuilder.hxx>
#include <MeshVS_NodalColorPrsBuilder.hxx>
#include <MeshVS_PrsBuilder.hxx>
#include <MeshVS_TextPrsBuilder.hxx>
#include <MeshVS_VectorPrsBuilder.hxx>
#include <OSD_Path.hxx>
#include <Quantity_Color.hxx>
#include <Quantity_HArray1OfColor.hxx>
#include <Quantity_NameOfColor.hxx>
#include <RWStl.hxx>
#include <SelectMgr_SelectionManager.hxx>
#include <Standard_ErrorHandler.hxx>
#include <StdSelect_ViewerSelector3d.hxx>
#include <StlAPI.hxx>
#include <StlAPI_Writer.hxx>
#include <StlMesh_Mesh.hxx>
#include <StlMesh_SequenceOfMeshTriangle.hxx>
#include <TColgp_SequenceOfXYZ.hxx>
#include <TCollection_AsciiString.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_HPackedMapOfInteger.hxx>
#include <TColStd_MapIteratorOfPackedMapOfInteger.hxx>
#include <TopoDS_Shape.hxx>
#include <V3d_View.hxx>
#include <ViewerTest.hxx>
#include <ViewerTest_DoubleMapOfInteractiveAndName.hxx>
#include <VrmlAPI.hxx>
#include <VrmlAPI_Writer.hxx>
#include <VrmlData_DataMapOfShapeAppearance.hxx>
#include <VrmlData_Scene.hxx>
#include <VrmlData_ShapeConvert.hxx>
#include <XSDRAW.hxx>
#include <XSDRAWIGES.hxx>
#include <XSDRAWSTEP.hxx>
#include <XSDRAWSTLVRML.hxx>
#include <XSDRAWSTLVRML_DataSource.hxx>
#include <XSDRAWSTLVRML_DataSource3D.hxx>
#include <XSDRAWSTLVRML_DrawableMesh.hxx>

// avoid warnings on 'extern "C"' functions returning C++ classes
#ifdef _MSC_VER
#pragma warning(4:4190)
#endif

#ifndef _STDIO_H
#include <stdio.h>
#endif

extern Standard_Boolean VDisplayAISObject (const TCollection_AsciiString& theName,
                                           const Handle(AIS_InteractiveObject)& theAISObj,
                                           Standard_Boolean theReplaceIfExists = Standard_True);
extern ViewerTest_DoubleMapOfInteractiveAndName& GetMapOfAIS();

static Standard_Integer writestl
(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 3 || argc > 4) {
    di << "Use: " << argv[0]
    << " shape file [ascii/binary (0/1) : 1 by default]\n";
  } else {
    TopoDS_Shape aShape = DBRep::Get(argv[1]);
    Standard_Boolean isASCIIMode = Standard_False;
    if (argc == 4) {
      isASCIIMode = (Draw::Atoi(argv[3]) == 0);
    }
    StlAPI_Writer aWriter;
    aWriter.ASCIIMode() = isASCIIMode;
    StlAPI_ErrorStatus aStatus = aWriter.Write (aShape, argv[2]);

    switch (aStatus)
    {
    case StlAPI_MeshIsEmpty: di << "** Error **: Mesh is empty. Please, compute triangulation before."; break;
    case StlAPI_CannotOpenFile: di << "** Error **: Cannot create/open a file with the passed name."; break;
    case StlAPI_StatusOK: default: break;
    }
  }
  return 0;
}

static Standard_Integer readstl
(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc<3) di << "wrong number of parameters"    << "\n";
  else {
    TopoDS_Shape aShape ;
    StlAPI::Read(aShape,argv[2]);
    DBRep::Set(argv[1],aShape);
  }
  return 0;
}

static Standard_Integer writevrml
(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 3 || argc > 5) 
  {
    di << "wrong number of parameters\n";
    return 0;
  }

  TopoDS_Shape aShape = DBRep::Get(argv[1]);

  // Get the optional parameters
  Standard_Integer aVersion = 2;
  Standard_Integer aType = 1;
  if (argc >= 4)
  {
    aVersion = Draw::Atoi(argv[3]);
    if (argc == 5)
      aType = Draw::Atoi(argv[4]);
  }

  // Bound parameters
  aVersion = Max(1, aVersion);
  aVersion = Min(2, aVersion);
  aType = Max(0, aType);
  aType = Min(2, aType);

  VrmlAPI_Writer writer;

  switch (aType)
  {
  case 0: writer.SetRepresentation(VrmlAPI_ShadedRepresentation); break;
  case 1: writer.SetRepresentation(VrmlAPI_WireFrameRepresentation); break;
  case 2: writer.SetRepresentation(VrmlAPI_BothRepresentation); break;
  }

  writer.Write(aShape, argv[2], aVersion);

  return 0;
}

//=======================================================================
//function : loadvrml
//purpose  :
//=======================================================================

static Standard_Integer loadvrml
(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc<3) di << "wrong number of parameters"    << "\n";
  else {
    TopoDS_Shape aShape ;
    VrmlData_DataMapOfShapeAppearance aShapeAppMap;

    //-----------------------------------------------------------
    filebuf aFic;
    istream aStream (&aFic);

    if (aFic.open(argv[2], ios::in)) {

      // Get path of the VRML file.
      OSD_Path aPath(argv[2]);
      TCollection_AsciiString aVrmlDir(".");
      TCollection_AsciiString aDisk = aPath.Disk();
      TCollection_AsciiString aTrek = aPath.Trek();
      if (!aTrek.IsEmpty())
      {
        if (!aDisk.IsEmpty())
          aVrmlDir = aDisk;
        else
          aVrmlDir.Clear();
        aTrek.ChangeAll('|', '/');
        aVrmlDir += aTrek;
      }

      VrmlData_Scene aScene;

      aScene.SetVrmlDir (aVrmlDir);
      aScene << aStream;
      const char * aStr = 0L;
      switch (aScene.Status()) {

      case VrmlData_StatusOK:
        {
          aShape = aScene.GetShape(aShapeAppMap);
          break;
        }
      case VrmlData_EmptyData:            aStr = "EmptyData"; break;
      case VrmlData_UnrecoverableError:   aStr = "UnrecoverableError"; break;
      case VrmlData_GeneralError:         aStr = "GeneralError"; break;
      case VrmlData_EndOfFile:            aStr = "EndOfFile"; break;
      case VrmlData_NotVrmlFile:          aStr = "NotVrmlFile"; break;
      case VrmlData_CannotOpenFile:       aStr = "CannotOpenFile"; break;
      case VrmlData_VrmlFormatError:      aStr = "VrmlFormatError"; break;
      case VrmlData_NumericInputError:    aStr = "NumericInputError"; break;
      case VrmlData_IrrelevantNumber:     aStr = "IrrelevantNumber"; break;
      case VrmlData_BooleanInputError:    aStr = "BooleanInputError"; break;
      case VrmlData_StringInputError:     aStr = "StringInputError"; break;
      case VrmlData_NodeNameUnknown:      aStr = "NodeNameUnknown"; break;
      case VrmlData_NonPositiveSize:      aStr = "NonPositiveSize"; break;
      case VrmlData_ReadUnknownNode:      aStr = "ReadUnknownNode"; break;
      case VrmlData_NonSupportedFeature:  aStr = "NonSupportedFeature"; break;
      case VrmlData_OutputStreamUndefined:aStr = "OutputStreamUndefined"; break;
      case VrmlData_NotImplemented:       aStr = "NotImplemented"; break;
      default:
        break;
      }
      if (aStr) {
        di << " ++ VRML Error: " << aStr << " in line "
          << aScene.GetLineError() << "\n";
      }
      else {
        DBRep::Set(argv[1],aShape);
      }
    }
    else {
      di << "cannot open file\n";
    }


    //-----------------------------------------------------------
  }
  return 0;
}

//-----------------------------------------------------------------------------
static Standard_Integer createmesh
(Draw_Interpretor& di, Standard_Integer argc, const char** argv )
{
  if (argc<3)
  {
    di << "Wrong number of parameters\n";
    di << "Use: " << argv[0] << " <mesh name> <stl file>\n";
    return 0;
  }

  Handle(AIS_InteractiveContext) aContext = ViewerTest::GetAISContext();
  if (aContext.IsNull())
  {
    di << "No active view. Please call 'vinit' first\n";
    return 0;
  }

  // Progress indicator
  OSD_Path aFile( argv[2] );
  Handle(Draw_ProgressIndicator) aProgress = new Draw_ProgressIndicator (di, 1);
  Handle(StlMesh_Mesh) aSTLMesh = RWStl::ReadFile (aFile, aProgress);

  di << "Reading OK...\n";
  Handle( XSDRAWSTLVRML_DataSource ) aDS = new XSDRAWSTLVRML_DataSource( aSTLMesh );
  di << "Data source is created successful\n";
  Handle( MeshVS_Mesh ) aMesh = new MeshVS_Mesh();
  di << "MeshVS_Mesh is created successful\n";

  aMesh->SetDataSource( aDS );
  aMesh->AddBuilder( new MeshVS_MeshPrsBuilder( aMesh.operator->() ), Standard_True );

  aMesh->GetDrawer()->SetColor( MeshVS_DA_EdgeColor, Quantity_NOC_YELLOW );

  // Hide all nodes by default
  Handle(TColStd_HPackedMapOfInteger) aNodes = new TColStd_HPackedMapOfInteger();
  Standard_Integer aLen = aSTLMesh->Vertices().Length();
  for ( Standard_Integer anIndex = 1; anIndex <= aLen; anIndex++ )
    aNodes->ChangeMap().Add( anIndex );
  aMesh->SetHiddenNodes( aNodes );
  aMesh->SetSelectableNodes ( aNodes );

  VDisplayAISObject(argv[1], aMesh);
  aContext->Deactivate( aMesh );

  Draw::Set( argv[1], new XSDRAWSTLVRML_DrawableMesh( aMesh ) );
  Handle( V3d_View ) aView = ViewerTest::CurrentView();
  if ( !aView.IsNull() )
    aView->FitAll();

  return 0;
}
//-----------------------------------------------------------------------------

static Standard_Integer create3d
(Draw_Interpretor& di, Standard_Integer argc, const char** argv )
{
  if (argc<2)
  {
    di << "Wrong number of parameters\n";
    di << "Use: " << argv[0] << " <mesh name>\n";
    return 0;
  }

  Handle(AIS_InteractiveContext) aContext = ViewerTest::GetAISContext();
  if (aContext.IsNull())
  {
    di << "No active view. Please call 'vinit' first\n";
    return 0;
  }

  Handle( XSDRAWSTLVRML_DataSource3D ) aDS = new XSDRAWSTLVRML_DataSource3D();
  di << "Data source is created successful\n";
  Handle( MeshVS_Mesh ) aMesh = new MeshVS_Mesh();
  di << "MeshVS_Mesh is created successful\n";

  aMesh->SetDataSource( aDS );
  aMesh->AddBuilder( new MeshVS_MeshPrsBuilder( aMesh.operator->() ), Standard_True );

  aMesh->GetDrawer()->SetColor( MeshVS_DA_EdgeColor, Quantity_NOC_YELLOW );

  // Hide all nodes by default
  Handle(TColStd_HPackedMapOfInteger) aNodes = new TColStd_HPackedMapOfInteger();
  Standard_Integer aLen = aDS->GetAllNodes().Extent();
  for ( Standard_Integer anIndex = 1; anIndex <= aLen; anIndex++ )
    aNodes->ChangeMap().Add( anIndex );
  aMesh->SetHiddenNodes( aNodes );
  aMesh->SetSelectableNodes ( aNodes );

  VDisplayAISObject(argv[1], aMesh);
  aContext->Deactivate( aMesh );

  Draw::Set( argv[1], new XSDRAWSTLVRML_DrawableMesh( aMesh ) );
  Handle( V3d_View ) aView = ViewerTest::CurrentView();
  if ( !aView.IsNull() )
    aView->FitAll();

  return 0;
}

Handle( MeshVS_Mesh ) getMesh( const char* theName, Draw_Interpretor& di)
{
  Handle( XSDRAWSTLVRML_DrawableMesh ) aDrawMesh =
    Handle( XSDRAWSTLVRML_DrawableMesh )::DownCast( Draw::Get( theName ) );

  if( aDrawMesh.IsNull() )
  {
    di << "There is no such object\n";
    return NULL;
  }
  else
  {
    Handle( MeshVS_Mesh ) aMesh = aDrawMesh->GetMesh();
    if( aMesh.IsNull() )
    {
      di << "There is invalid mesh\n";
      return NULL;
    }
    else
      return aMesh;
  }
}

//-----------------------------------------------------------------------------
static Standard_Integer setcolor
(Draw_Interpretor& di, Standard_Integer argc, const char** argv, Standard_Integer theParam )
{
  if (argc<5)
    di << "Wrong number of parameters\n";
  else
  {
    Handle( MeshVS_Mesh ) aMesh = getMesh( argv[1], di );
    if( !aMesh.IsNull() )
    {
      Standard_Real aRed = Draw::Atof (argv[2]);
      Standard_Real aGreen = Draw::Atof (argv[3]);
      Standard_Real aBlue = Draw::Atof (argv[4]);
      aMesh->GetDrawer()->SetColor( (MeshVS_DrawerAttribute)theParam,
                                    Quantity_Color( aRed, aGreen, aBlue, Quantity_TOC_RGB ) );

      Handle( AIS_InteractiveContext ) aContext = ViewerTest::GetAISContext();

      if( aContext.IsNull() )
        di << "The context is null\n";
      else
        aContext->Redisplay( aMesh );
    }
  }
  return 0;
}
//-----------------------------------------------------------------------------
static Standard_Integer meshcolor
(Draw_Interpretor& theInterp, Standard_Integer argc, const char** argv )
{
  return setcolor( theInterp, argc, argv, MeshVS_DA_InteriorColor );
}
//-----------------------------------------------------------------------------
static Standard_Integer linecolor
(Draw_Interpretor& theInterp, Standard_Integer argc, const char** argv )
{
  return setcolor( theInterp, argc, argv, MeshVS_DA_EdgeColor );
}
//-----------------------------------------------------------------------------
static Standard_Integer meshmat
(Draw_Interpretor& di, Standard_Integer argc, const char** argv )
{
  if (argc<3)
    di << "Wrong number of parameters\n";
  else
  {
    Handle( MeshVS_Mesh ) aMesh = getMesh( argv[1], di );
    if( !aMesh.IsNull() )
    {
      Standard_Integer aMaterial = Draw::Atoi (argv[2]);

      Graphic3d_MaterialAspect aMatAsp =
        (Graphic3d_MaterialAspect)(Graphic3d_NameOfMaterial)aMaterial;

      if (argc == 4)
      {
        Standard_Real aTransparency = Draw::Atof(argv[3]);
        aMatAsp.SetTransparency(aTransparency);
      }
      aMesh->GetDrawer()->SetMaterial( MeshVS_DA_FrontMaterial, aMatAsp );
      aMesh->GetDrawer()->SetMaterial( MeshVS_DA_BackMaterial, aMatAsp );

      Handle( AIS_InteractiveContext ) aContext = ViewerTest::GetAISContext();

      if( aContext.IsNull() )
        di << "The context is null\n";
      else
        aContext->Redisplay( aMesh );
    }
  }
  return 0;
}
//-----------------------------------------------------------------------------
static Standard_Integer shrink
(Draw_Interpretor& di, Standard_Integer argc, const char** argv )
{
  if (argc<3)
    di << "Wrong number of parameters\n";
  else
  {
    Handle( MeshVS_Mesh ) aMesh = getMesh( argv[1], di );
    if( !aMesh.IsNull() )
    {
      Standard_Real aShrinkCoeff = Draw::Atof (argv[2]);
      aMesh->GetDrawer()->SetDouble( MeshVS_DA_ShrinkCoeff, aShrinkCoeff );

      Handle( AIS_InteractiveContext ) aContext = ViewerTest::GetAISContext();

      if( aContext.IsNull() )
        di << "The context is null\n";
      else
        aContext->Redisplay( aMesh );
    }
  }
  return 0;
}

//-----------------------------------------------------------------------------
static Standard_Integer closed (Draw_Interpretor& theDI, Standard_Integer theArgc, const char** theArgv)
{
  if (theArgc < 3)
  {
    theDI << "Wrong number of parameters.\n";
  }
  else
  {
    Handle(MeshVS_Mesh) aMesh = getMesh (theArgv[1], theDI);
    if (!aMesh.IsNull())
    {
      Standard_Integer aFlag = Draw::Atoi (theArgv[2]);
      aMesh->GetDrawer()->SetBoolean (MeshVS_DA_SupressBackFaces, aFlag);

      Handle( AIS_InteractiveContext ) aContext = ViewerTest::GetAISContext();
      if (aContext.IsNull())
      {
        theDI << "The context is null\n";
      }
      else
      {
        aContext->Redisplay (aMesh);
      }
    }
  }
  return 0;
}

//-----------------------------------------------------------------------------

static Standard_Integer mdisplay
(Draw_Interpretor& di, Standard_Integer argc, const char** argv )
{
  if (argc<2)
    di << "Wrong number of parameters\n";
  else
  {
    Handle( MeshVS_Mesh ) aMesh = getMesh( argv[1], di );
    if( !aMesh.IsNull() )
    {
      Handle( AIS_InteractiveContext ) aContext = ViewerTest::GetAISContext();

      if( aContext.IsNull() )
        di << "The context is null\n";
      else
      {
        if( aContext->HasOpenedContext() )
          aContext->CloseLocalContext();

        aContext->Display( aMesh );
      }
    }
  }
  return 0;
}
//-----------------------------------------------------------------------------
static Standard_Integer merase
(Draw_Interpretor& di, Standard_Integer argc, const char** argv )
{
  if (argc<2)
    di << "Wrong number of parameters\n";
  else
  {
    Handle( MeshVS_Mesh ) aMesh = getMesh( argv[1], di );
    if( !aMesh.IsNull() )
    {
      Handle( AIS_InteractiveContext ) aContext = ViewerTest::GetAISContext();

      if( aContext.IsNull() )
        di << "The context is null\n";
      else
      {
        if( aContext->HasOpenedContext() )
          aContext->CloseLocalContext();

        aContext->Erase( aMesh );
      }
    }
    else
      di << "Mesh is null\n";
  }
  return 0;
}
//-----------------------------------------------------------------------------
static Standard_Integer hidesel
(Draw_Interpretor& di, Standard_Integer argc, const char** argv )
{
  if (argc<2)
  {
    di << "Wrong number of parameters\n";
    di << "Use: " << argv[0] << " <mesh name>\n";
    return 0;
  }

  Handle( AIS_InteractiveContext ) aContext = ViewerTest::GetAISContext();
  Handle( MeshVS_Mesh ) aMesh = getMesh( argv[1], di );
  if( aMesh.IsNull() )
  {
    di << "The mesh is invalid\n";
    return 0;
  }

  if( aContext.IsNull() )
    di << "The context is null\n";
  else
  {
    Handle(TColStd_HPackedMapOfInteger) aHiddenNodes = aMesh->GetHiddenNodes();
    if (aHiddenNodes.IsNull())
    {
      aHiddenNodes = new TColStd_HPackedMapOfInteger();
    }
    Handle(TColStd_HPackedMapOfInteger) aHiddenElements = aMesh->GetHiddenElems();
    if (aHiddenElements.IsNull())
    {
      aHiddenElements = new TColStd_HPackedMapOfInteger();
    }
    for( aContext->InitSelected(); aContext->MoreSelected(); aContext->NextSelected() )
    {
      Handle( MeshVS_MeshEntityOwner ) anOwner =
        Handle( MeshVS_MeshEntityOwner )::DownCast( aContext->SelectedOwner() );
      if( !anOwner.IsNull() )
      {
        if( anOwner->Type()==MeshVS_ET_Node )
        {
          aHiddenNodes->ChangeMap().Add( anOwner->ID() );
        }
        else
        {
          aHiddenElements->ChangeMap().Add( anOwner->ID() );
        }
      }
    }
    aContext->ClearSelected();
    aMesh->SetHiddenNodes( aHiddenNodes );
    aMesh->SetHiddenElems( aHiddenElements );
    aContext->Redisplay( aMesh );
  }

  return 0;
}
//-----------------------------------------------------------------------------
static Standard_Integer showonly
(Draw_Interpretor& di, Standard_Integer argc, const char** argv )
{
  if (argc<2)
  {
    di << "Wrong number of parameters\n";
    di << "Use: " << argv[0] << " <mesh name>\n";
    return 0;
  }


  Handle( AIS_InteractiveContext ) aContext = ViewerTest::GetAISContext();
  Handle( MeshVS_Mesh ) aMesh = getMesh( argv[1], di );
  if( aMesh.IsNull() )
  {
    di << "The mesh is invalid\n";
    return 0;
  }

  if( aContext.IsNull() )
    di << "The context is null\n";
  else
  {
    Handle(TColStd_HPackedMapOfInteger) aHiddenNodes =
      new TColStd_HPackedMapOfInteger(aMesh->GetDataSource()->GetAllNodes());
    Handle(TColStd_HPackedMapOfInteger) aHiddenElements =
      new TColStd_HPackedMapOfInteger(aMesh->GetDataSource()->GetAllElements());
    for( aContext->InitSelected(); aContext->MoreSelected(); aContext->NextSelected() )
    {
      Handle( MeshVS_MeshEntityOwner ) anOwner =
        Handle( MeshVS_MeshEntityOwner )::DownCast( aContext->SelectedOwner() );
      if( !anOwner.IsNull() )
      {
        if( anOwner->Type() == MeshVS_ET_Node )
        {
          aHiddenNodes->ChangeMap().Remove( anOwner->ID() );
        }
        else
        {
          aHiddenElements->ChangeMap().Remove( anOwner->ID() );
        }
      }
    }
    aMesh->SetHiddenNodes( aHiddenNodes );
    aMesh->SetHiddenElems( aHiddenElements );
    aContext->Redisplay( aMesh );
  }

  return 0;
}
//-----------------------------------------------------------------------------
static Standard_Integer showall
(Draw_Interpretor& di, Standard_Integer argc, const char** argv )
{
  if (argc<2)
  {
    di << "Wrong number of parameters\n";
    di << "Use: " << argv[0] << " <mesh name>\n";
    return 0;
  }

  Handle( AIS_InteractiveContext ) aContext = ViewerTest::GetAISContext();
  Handle( MeshVS_Mesh ) aMesh = getMesh( argv[1], di );
  if( aMesh.IsNull() )
  {
    di << "The mesh is invalid\n";
    return 0;
  }

  if( aContext.IsNull() )
    di << "The context is null\n";
  else
  {
    aMesh->SetHiddenNodes( new TColStd_HPackedMapOfInteger() );
    aMesh->SetHiddenElems( new TColStd_HPackedMapOfInteger() );
    aContext->Redisplay( aMesh );
  }

  return 0;
}

//-----------------------------------------------------------------------------
static Standard_Integer meshcolors( Draw_Interpretor& di,
                                    Standard_Integer argc,
                                    const char** argv )
{
  try
  {
    OCC_CATCH_SIGNALS
      if ( argc < 4 )
      {
        di << "Wrong number of parameters\n";
        di << "Use : meshcolors <mesh name> <mode> <isreflect>\n";
        di << "mode : {elem1|elem2|nodal|nodaltex|none}\n";
        di << "       elem1 - different color for each element\n";
        di << "       elem2 - one color for one side\n";
        di << "       nodal - different color for each node\n";
        di << "       nodaltex - different color for each node with texture interpolation\n";
        di << "       none  - clear\n";
        di << "isreflect : {0|1} \n";

        return 0;
      }

      Handle( MeshVS_Mesh ) aMesh = getMesh( argv[ 1 ], di );

      if ( aMesh.IsNull() )
      {
        di << "Mesh not found\n";
        return 0;
      }
      Handle(AIS_InteractiveContext) anIC = ViewerTest::GetAISContext();
      if ( anIC.IsNull() )
      {
        di << "The context is null\n";
        return 0;
      }
      if( !aMesh.IsNull() )
      {
        TCollection_AsciiString aMode = TCollection_AsciiString (argv[2]);
        Quantity_Color aColor1( (Quantity_NameOfColor)( Quantity_NOC_BLUE1 ) );
        Quantity_Color aColor2( (Quantity_NameOfColor)( Quantity_NOC_RED1 ) );
        if( aMode.IsEqual("elem1") || aMode.IsEqual("elem2") || aMode.IsEqual("nodal") || aMode.IsEqual("nodaltex") || aMode.IsEqual("none") )
        {
          Handle(MeshVS_PrsBuilder) aTempBuilder;
          Standard_Integer aReflection = Draw::Atoi(argv[3]);

          for (Standard_Integer aCount = 0 ; aCount < aMesh->GetBuildersCount(); aCount++ ){
            aTempBuilder = aMesh->FindBuilder("MeshVS_ElementalColorPrsBuilder");
            if( !aTempBuilder.IsNull())
              aMesh->RemoveBuilderById(aTempBuilder->GetId());

            aTempBuilder = aMesh->FindBuilder("MeshVS_NodalColorPrsBuilder");
            if( !aTempBuilder.IsNull())
              aMesh->RemoveBuilderById(aTempBuilder->GetId());
          }

          if( aMode.IsEqual("elem1") || aMode.IsEqual("elem2") )
          {
            Handle(MeshVS_ElementalColorPrsBuilder) aBuilder = new MeshVS_ElementalColorPrsBuilder(
                aMesh, MeshVS_DMF_ElementalColorDataPrs | MeshVS_DMF_OCCMask );
              // Color
            const TColStd_PackedMapOfInteger& anAllElements = aMesh->GetDataSource()->GetAllElements();
            TColStd_MapIteratorOfPackedMapOfInteger anIter( anAllElements );

            if( aMode.IsEqual("elem1") )
              for ( ; anIter.More(); anIter.Next() )
              {
                Quantity_Color aColor( (Quantity_NameOfColor)( anIter.Key() % Quantity_NOC_WHITE ) );
                aBuilder->SetColor1( anIter.Key(), aColor );
              }
            else
              for ( ; anIter.More(); anIter.Next() )
                aBuilder->SetColor2( anIter.Key(), aColor1, aColor2 );

            aMesh->AddBuilder( aBuilder, Standard_True );
          }


          if( aMode.IsEqual("nodal") )
          {
            Handle(MeshVS_NodalColorPrsBuilder) aBuilder = new MeshVS_NodalColorPrsBuilder(
                aMesh, MeshVS_DMF_NodalColorDataPrs | MeshVS_DMF_OCCMask );
            aMesh->AddBuilder( aBuilder, Standard_True );

            // Color
            const TColStd_PackedMapOfInteger& anAllNodes =
              aMesh->GetDataSource()->GetAllNodes();
            TColStd_MapIteratorOfPackedMapOfInteger anIter( anAllNodes );
            for ( ; anIter.More(); anIter.Next() )
            {
              Quantity_Color aColor( (Quantity_NameOfColor)(
                anIter.Key() % Quantity_NOC_WHITE ) );
              aBuilder->SetColor( anIter.Key(), aColor );
            }
            aMesh->AddBuilder( aBuilder, Standard_True );
          }

          if(aMode.IsEqual("nodaltex"))
          {
            // assign nodal builder to the mesh
            Handle(MeshVS_NodalColorPrsBuilder) aBuilder = new MeshVS_NodalColorPrsBuilder(
                   aMesh, MeshVS_DMF_NodalColorDataPrs | MeshVS_DMF_OCCMask);
            aMesh->AddBuilder(aBuilder, Standard_True);
            aBuilder->UseTexture(Standard_True);

            // prepare color map for texture
            Aspect_SequenceOfColor aColorMap;
            aColorMap.Append((Quantity_NameOfColor) Quantity_NOC_RED);
            aColorMap.Append((Quantity_NameOfColor) Quantity_NOC_YELLOW);
            aColorMap.Append((Quantity_NameOfColor) Quantity_NOC_BLUE1);

            // prepare scale map for mesh - it will be assigned to mesh as texture coordinates
            // make mesh color interpolated from minimum X coord to maximum X coord
            Handle(MeshVS_DataSource) aDataSource = aMesh->GetDataSource();
            Standard_Real aMinX, aMinY, aMinZ, aMaxX, aMaxY, aMaxZ;

            // get bounding box for calculations
            aDataSource->GetBoundingBox().Get(aMinX, aMinY, aMinZ, aMaxX, aMaxY, aMaxZ);
            Standard_Real aDelta = aMaxX - aMinX;

            // assign color scale map values (0..1) to nodes
            TColStd_DataMapOfIntegerReal aScaleMap;
            TColStd_Array1OfReal aCoords(1, 3);
            Standard_Integer     aNbNodes;
            MeshVS_EntityType    aType;

            // iterate nodes
            const TColStd_PackedMapOfInteger& anAllNodes =
                  aMesh->GetDataSource()->GetAllNodes();
            TColStd_MapIteratorOfPackedMapOfInteger anIter(anAllNodes);
            for (; anIter.More(); anIter.Next())
            {
              //get node coordinates to aCoord variable
              aDataSource->GetGeom(anIter.Key(), Standard_False, aCoords, aNbNodes, aType);

              Standard_Real aScaleValue;
              try {
                OCC_CATCH_SIGNALS
                aScaleValue = (aCoords.Value(1) - (Standard_Real) aMinX) / aDelta;
              } catch(Standard_Failure) {
                aScaleValue = 0;
              }

              aScaleMap.Bind(anIter.Key(), aScaleValue);
            }

            //set color map for builder and a color for invalid scale value
            aBuilder->SetColorMap(aColorMap);
            aBuilder->SetInvalidColor(Quantity_NOC_BLACK);
            aBuilder->SetTextureCoords(aScaleMap);
            aMesh->AddBuilder(aBuilder, Standard_True);

            //set viewer to display texures
            const Handle(V3d_Viewer)& aViewer = anIC->CurrentViewer();
            for (aViewer->InitActiveViews(); aViewer->MoreActiveViews(); aViewer->NextActiveViews())
                 aViewer->ActiveView()->SetSurfaceDetail(V3d_TEX_ALL);
          }

          aMesh->GetDrawer()->SetBoolean ( MeshVS_DA_ColorReflection, Standard_Boolean(aReflection) );

          anIC->Redisplay( aMesh );
        }
        else
        {
          di << "Wrong mode name\n";
          return 0;
        }
      }
  }
  catch ( Standard_Failure )
  {
    di << "Error\n";
  }

  return 0;
}
//-----------------------------------------------------------------------------
static Standard_Integer meshvectors( Draw_Interpretor& di,
                                     Standard_Integer argc,
                                     const char** argv )
{
  if ( argc < 3 )
  {
    di << "Wrong number of parameters\n";
    di << "Use : meshvectors <mesh name> < -mode {elem|nodal|none} > [-maxlen len] [-color name] [-arrowpart ratio] [-issimple {1|0}]\n";
    di << "Supported mode values:\n";
    di << "       elem  - vector per element\n";
    di << "       nodal - vector per node\n";
    di << "       none  - clear\n";

    return 0;
  }

  Handle( MeshVS_Mesh ) aMesh = getMesh( argv[ 1 ], di );

  if ( aMesh.IsNull() )
  {
    di << "Mesh not found\n";
    return 0;
  }
  Handle(AIS_InteractiveContext) anIC = ViewerTest::GetAISContext();
  if ( anIC.IsNull() )
  {
    di << "The context is null\n";
    return 0;
  }

  TCollection_AsciiString aParam;
  TCollection_AsciiString aMode("none");
  Standard_Real           aMaxlen(1.0);
  Quantity_Color          aColor(Quantity_NOC_ORANGE);
  Standard_Real           anArrowPart(0.1);
  Standard_Boolean        isSimplePrs(Standard_False);

  for (Standard_Integer anIdx = 2; anIdx < argc; anIdx++)
  {
    if (!aParam.IsEmpty())
    {
      if (aParam == "-mode")
      {
        aMode       = argv[anIdx];
      }
      else if (aParam == "-maxlen")
      {
        aMaxlen     = Draw::Atof(argv[anIdx]);
      }
      else if (aParam == "-color")
      {
        aColor      = ViewerTest::GetColorFromName(argv[anIdx]);
      }
      else if (aParam == "-arrowpart")
      {
        anArrowPart = Draw::Atof(argv[anIdx]);
      }
      else if (aParam == "-issimple")
      {
        isSimplePrs = Draw::Atoi(argv[anIdx]);
      }
      aParam.Clear();
    }
    else if (argv[anIdx][0] == '-')
    {
      aParam = argv[anIdx];
    }
  }

  if( !aMode.IsEqual("elem") && !aMode.IsEqual("nodal") && !aMode.IsEqual("none") )
  {
    di << "Wrong mode name\n";
    return 0;
  }

  Handle(MeshVS_PrsBuilder) aTempBuilder;

  aTempBuilder = aMesh->FindBuilder("MeshVS_VectorPrsBuilder");
  if( !aTempBuilder.IsNull())
    aMesh->RemoveBuilderById(aTempBuilder->GetId());

  if( !aMode.IsEqual("none") )
  {
    Handle(MeshVS_VectorPrsBuilder) aBuilder = new MeshVS_VectorPrsBuilder( aMesh.operator->(), 
                                                                            aMaxlen,
                                                                            aColor,
                                                                            MeshVS_DMF_VectorDataPrs,
                                                                            0,
                                                                            -1,
                                                                            MeshVS_BP_Vector,
                                                                            isSimplePrs);

    Standard_Boolean anIsElement = aMode.IsEqual("elem");
    const TColStd_PackedMapOfInteger& anAllIDs = anIsElement ? aMesh->GetDataSource()->GetAllElements() :
                                                               aMesh->GetDataSource()->GetAllNodes();

    Standard_Integer aNbNodes;
    MeshVS_EntityType aEntType;

    TColStd_MapIteratorOfPackedMapOfInteger anIter( anAllIDs );
    for ( ; anIter.More(); anIter.Next() )
    {
      TColStd_Array1OfReal aCoords(1, 3);
      if (anIsElement)
        aMesh->GetDataSource()->GetNormal(anIter.Key(), 3, aCoords.ChangeValue(1), aCoords.ChangeValue(2), aCoords.ChangeValue(3));
      else
        aMesh->GetDataSource()->GetGeom(anIter.Key(), Standard_False, aCoords, aNbNodes, aEntType);

      gp_Vec aNorm = gp_Vec(aCoords.Value(1), aCoords.Value(2), aCoords.Value(3));
      if( !aNorm.Magnitude() )
        aNorm = gp_Vec(0,0,1);
      aBuilder->SetVector(anIsElement, anIter.Key(), aNorm.Normalized());
    }

    aMesh->AddBuilder( aBuilder, Standard_False );
    aMesh->GetDrawer()->SetDouble ( MeshVS_DA_VectorArrowPart, anArrowPart );
  }

  anIC->Redisplay( aMesh );

  return 0;
}
//-----------------------------------------------------------------------------

static Standard_Integer meshtext( Draw_Interpretor& di,
                                  Standard_Integer argc,
                                  const char** argv )
{
  if ( argc < 2 )
  {
    di << "Wrong number of parameters\n";
    di << "Use : meshtext <mesh name>\n";
    return 0;
  }

  Handle( MeshVS_Mesh ) aMesh = getMesh( argv[ 1 ], di );

  if ( aMesh.IsNull() )
  {
    di << "Mesh not found\n";
    return 0;
  }

  Handle(AIS_InteractiveContext) anIC = ViewerTest::GetAISContext();
  if ( anIC.IsNull() )
  {
    di << "The context is null\n";
    return 0;
  }

  // Prepare triangle labels
  MeshVS_DataMapOfIntegerAsciiString aLabels;
  Standard_Integer aLen = aMesh->GetDataSource()->GetAllElements().Extent();
  for ( Standard_Integer anIndex = 1; anIndex <= aLen; anIndex++ ){
    aLabels.Bind( anIndex, TCollection_AsciiString( anIndex ) );
  }

  Handle(MeshVS_TextPrsBuilder) aTextBuilder = new MeshVS_TextPrsBuilder( aMesh.operator->(), 20., Quantity_NOC_YELLOW );
  aTextBuilder->SetTexts( Standard_True, aLabels );
  aMesh->AddBuilder( aTextBuilder );

  return 0;
}

static Standard_Integer meshdeform( Draw_Interpretor& di,
                                    Standard_Integer argc,
                                    const char** argv )
{
  if ( argc < 3 )
  {
    di << "Wrong number of parameters\n";
    di << "Use : meshdeform <mesh name> < -mode {on|off} > [-scale scalefactor]\n";
    return 0;
  }

  Handle( MeshVS_Mesh ) aMesh = getMesh( argv[ 1 ], di );

  if ( aMesh.IsNull() )
  {
    di << "Mesh not found\n";
    return 0;
  }
  Handle(AIS_InteractiveContext) anIC = ViewerTest::GetAISContext();
  if ( anIC.IsNull() )
  {
    di << "The context is null\n";
    return 0;
  }

  TCollection_AsciiString aParam;
  TCollection_AsciiString aMode("off");
  Standard_Real           aScale(1.0);

  for (Standard_Integer anIdx = 2; anIdx < argc; anIdx++)
  {
    if (!aParam.IsEmpty())
    {
      if (aParam == "-mode")
      {
        aMode = argv[anIdx];
      }
      else if (aParam == "-scale")
      {
        aScale = Draw::Atof(argv[anIdx]);
      }
      aParam.Clear();
    }
    else if (argv[anIdx][0] == '-')
    {
      aParam = argv[anIdx];
    }
  }

  if(!aMode.IsEqual("on") && !aMode.IsEqual("off"))
  {
    di << "Wrong mode name\n";
    return 0;
  }

  Handle ( MeshVS_DeformedDataSource ) aDefDS =
    new MeshVS_DeformedDataSource( aMesh->GetDataSource() , aScale );

  const TColStd_PackedMapOfInteger& anAllIDs = aMesh->GetDataSource()->GetAllNodes();

  Standard_Integer aNbNodes;
  MeshVS_EntityType aEntType;

  TColStd_MapIteratorOfPackedMapOfInteger anIter( anAllIDs );
  for ( ; anIter.More(); anIter.Next() )
  {
    TColStd_Array1OfReal aCoords(1, 3);
    aMesh->GetDataSource()->GetGeom(anIter.Key(), Standard_False, aCoords, aNbNodes, aEntType);

    gp_Vec aNorm = gp_Vec(aCoords.Value(1), aCoords.Value(2), aCoords.Value(3));
    if( !aNorm.Magnitude() )
      aNorm = gp_Vec(0,0,1);
    aDefDS->SetVector(anIter.Key(), aNorm.Normalized());
  }

  aMesh->SetDataSource(aDefDS);

  anIC->Redisplay( aMesh );

  Handle( V3d_View ) aView = ViewerTest::CurrentView();
  if ( !aView.IsNull() )
    aView->FitAll();

  return 0;
}

static Standard_Integer mesh_edge_width( Draw_Interpretor& di,
                                        Standard_Integer argc,
                                        const char** argv )
{
  try
  {
    OCC_CATCH_SIGNALS
      if ( argc < 3 )
      {
        di << "Wrong number of parameters\n";
        di << "Use : mesh_edge_width <mesh name> <width>\n";
        return 0;
      }

      Handle(MeshVS_Mesh) aMesh = getMesh( argv[ 1 ], di );
      if ( aMesh.IsNull() )
      {
        di << "Mesh not found\n";
        return 0;
      }

      const char* aWidthStr = argv[ 2 ];
      if ( aWidthStr == 0 || Draw::Atof( aWidthStr ) <= 0 )
      {
        di << "Width must be real value more than zero\n";
        return 0;
      }

      double aWidth = Draw::Atof( aWidthStr );

      Handle(AIS_InteractiveContext) anIC = ViewerTest::GetAISContext();
      if ( anIC.IsNull() )
      {
        di << "The context is null\n";
        return 0;
      }

      Handle(MeshVS_Drawer) aDrawer = aMesh->GetDrawer();
      if ( aDrawer.IsNull() )
      {
        di << "The drawer is null\n";
        return 0;
      }

      aDrawer->SetDouble( MeshVS_DA_EdgeWidth, aWidth );
      anIC->Redisplay( aMesh );
  }
  catch ( Standard_Failure )
  {
    di << "Error\n";
  }

  return 0;
}

//-----------------------------------------------------------------------------

static Standard_Integer meshinfo(Draw_Interpretor& di,
                                 Standard_Integer argc,
                                 const char** argv)
{
  if ( argc != 2 )
  {
    di << "Wrong number of parameters. Use : meshinfo mesh\n";
    return 0;
  }

  Handle(MeshVS_Mesh) aMesh = getMesh(argv[ 1 ], di);
  if ( aMesh.IsNull() )
  {
    di << "Mesh not found\n";
    return 0;
  }

  Handle(XSDRAWSTLVRML_DataSource) stlMeshSource = Handle(XSDRAWSTLVRML_DataSource)::DownCast(aMesh->GetDataSource());
  if (!stlMeshSource.IsNull())
  {
    const TColStd_PackedMapOfInteger& nodes = stlMeshSource->GetAllNodes();
    const TColStd_PackedMapOfInteger& tris  = stlMeshSource->GetAllElements();

    di << "Nb nodes = " << nodes.Extent() << "\n";
    di << "Nb triangles = " << tris.Extent() << "\n";
  }

  return 0;
}

#include <AIS_InteractiveObject.hxx>
#include <BRepBuilderAPI_Copy.hxx>
#include <BRepMesh_IncrementalMesh.hxx>
#include <BRepTools.hxx>
#include <MeshVS_LODDataSource.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <ViewerTest_AutoUpdater.hxx>
//=======================================================================
//function : VGenLods
//purpose  : Generates sequence of LODs for the shape given
//=======================================================================
static int MeshGenLods (Draw_Interpretor& theDI,
                        Standard_Integer  theArgNum,
                        const char**      theArgs)
{
  if (theArgNum < 3)
  {
    std::cout << "Error! Wrong number of arguments. See usage:\n";
    theDI.PrintHelp (theArgs[0]);
    return 1;
  }

  Handle(AIS_InteractiveContext) aCtx = ViewerTest::GetAISContext();
  if (aCtx.IsNull())
  {
    std::cout << "Error! No opened viewer." << std::endl;
    return 1;
  }

  Standard_CString aShapeName = theArgs[1];
  const TopoDS_Shape aShape = DBRep::Get (aShapeName);
  if (aShape.IsNull())
  {
    std::cout << "Error! No shape with the name " << aShapeName << "." << std::endl;
    return 1;
  }
  const Standard_Integer aLodsNb = Draw::Atoi (theArgs[2]);
  if (aLodsNb <= 0)
  {
    std::cout << "Error! Incorrect amount of LODs passed: " << aLodsNb << "." << std::endl;
    std::cout << "Must be greater than zero." << std::endl;
    return 1;
  }
  Standard_Boolean toPrintInfo = Standard_False;
  TCollection_AsciiString aSavePath = "";
  Handle(MeshVS_Mesh) aMesh = NULL;
  if (theArgNum > 3)
  {
    for (Standard_Integer anArgIdx = 3; anArgIdx < theArgNum; ++anArgIdx)
    {
      TCollection_AsciiString anArg (theArgs[anArgIdx]);
      anArg.LowerCase();
      if (anArg == "-debug")
      {
        toPrintInfo = Standard_True;
      }
      else if (anArg == "-mesh")
      {
        aMesh = Handle(MeshVS_Mesh)::DownCast (GetMapOfAIS().Find2 (theArgs[++anArgIdx]));
      }
      else if (anArg == "-save")
      {
        aSavePath = TCollection_AsciiString (theArgs[++anArgIdx]);
      }
    }
  }

  // Determine max deflection of a shape
  Handle(Poly_Triangulation) aFaceTrg;
  TopLoc_Location aFaceLoc;
  Standard_Real aMaxDefl = 0.0;
  for (TopExp_Explorer anExplorer (aShape, TopAbs_FACE); anExplorer.More(); anExplorer.Next())
  {
    TopoDS_Face aCurFace = TopoDS::Face (anExplorer.Current());
    aFaceTrg = BRep_Tool::Triangulation (aCurFace, aFaceLoc);
    if (!aFaceTrg.IsNull())
    {
      if (aFaceTrg->Deflection() > aMaxDefl)
        aMaxDefl = aFaceTrg->Deflection();
    }
  }

  BRepTools::Clean (aShape);
  BRepMesh_FastDiscret::Parameters aMeshParams;
  aMeshParams.Deflection = aMaxDefl;
  aMeshParams.Angle = 0.5;
  aMeshParams.Relative =  Standard_False;
  aMeshParams.InParallel = Standard_False;
  aMeshParams.MinSize = Precision::Confusion();
  aMeshParams.InternalVerticesMode = Standard_True;
  aMeshParams.ControlSurfaceDeflection = Standard_True;
  aMeshParams.AdaptiveMin = Standard_False;
  BRepMesh_IncrementalMesh aBaseMesher (aShape, aMeshParams);
  aBaseMesher.Perform();
  ViewerTest_AutoUpdater anAutoUpd (aCtx, ViewerTest::CurrentView());


  // compute the shape for each LOD
  TopoDS_Shape* aLodShapes = new TopoDS_Shape[aLodsNb];
  BRepBuilderAPI_Copy aMainCopyAlgo;
  aMainCopyAlgo.Perform (aShape, Standard_True, Standard_True);
  aLodShapes[0] = aMainCopyAlgo.Shape();
  Standard_Real aDeflDecrFactor = aMaxDefl / aLodsNb;
  for (Standard_Integer aLodIdx = 1; aLodIdx < aLodsNb; ++aLodIdx)
  {
    BRepBuilderAPI_Copy aCopyAlgo;
    aCopyAlgo.Perform (aShape, Standard_True, Standard_True);
    aLodShapes[aLodIdx] = aCopyAlgo.Shape();
    BRepTools::Clean (aLodShapes[aLodIdx]);
    BRepMesh_FastDiscret::Parameters aParams;
    aParams.Deflection = aMaxDefl - aLodIdx * aDeflDecrFactor;
    aParams.Angle = 0.5;
    aParams.Relative =  Standard_False;
    aParams.InParallel = Standard_False;
    aParams.MinSize = Precision::Confusion();
    aParams.InternalVerticesMode = Standard_True;
    aParams.ControlSurfaceDeflection = Standard_True;
    aParams.AdaptiveMin = Standard_False;
    BRepMesh_IncrementalMesh aMesher (aLodShapes[aLodIdx], aParams);
    aMesher.Perform();
    if (toPrintInfo)
    {
      Standard_Integer aStatus = aMesher.GetStatusFlags();
      std::cout << "LOD #" << aLodIdx << " meshing status: ";
      if (!aStatus)
        std::cout << " OK" << std::endl;
      else
      {
        for (Standard_Integer aBitIdx = 0; aBitIdx < 4; ++aBitIdx)
        {
          if ((aStatus >> aBitIdx) & 1)
          {
            switch (aBitIdx + 1)
            {
            case 1:
              std::cout << " OpenWire ";
              break;
            case 2:
              std::cout << " SelfIntersectingWire ";
              break;
            case 3:
              std::cout << " Failure ";
              break;
            case 4:
              std::cout << " ReMesh ";
              break;
            }
          }
        }
        std::cout << std::endl;
      }
    }
  }

  if (toPrintInfo)
  {
    std::cout << std::endl;
    for (Standard_Integer aLodIdx = 0; aLodIdx < aLodsNb; ++aLodIdx)
    {
      std::cout << "LOD #" << aLodIdx + 1 << " info:" << std::endl;
      if (aLodShapes[aLodIdx].IsNull())
      {
        std::cout << "The shape is null!" << std::endl;
      }

      Handle(Poly_Triangulation) aCurTrg;
      TopLoc_Location aCurLoc;
      Standard_Real aDefl = 0.0;
      Standard_Integer aTrgsNb = 0, aNodesNb = 0;
      for (TopExp_Explorer aShapeExp (aLodShapes[aLodIdx], TopAbs_FACE); aShapeExp.More(); aShapeExp.Next())
      {
        TopoDS_Face aCurFace = TopoDS::Face (aShapeExp.Current());
        aCurTrg = BRep_Tool::Triangulation (aCurFace, aCurLoc);
        if (!aCurTrg.IsNull())
        {
          aTrgsNb += aCurTrg->NbTriangles();
          aNodesNb += aCurTrg->NbNodes();
          if (aCurTrg->Deflection() > aDefl)
            aDefl = aCurTrg->Deflection();
        }
      }
      std::cout << "deflection  : " << aDefl << std::endl;
      std::cout << "triangles nb: " << aTrgsNb << std::endl;
      std::cout << "nodes nb    : " << aNodesNb << std::endl;
      std::cout << std::endl;
    }
  }

  if (!aMesh.IsNull() && aSavePath != "")
  {
    if (aSavePath.Value (aSavePath.Length()) != '/')
    {
      aSavePath = aSavePath + "/";
    }
    aSavePath = aSavePath + aShapeName + "_LOD";
    for (Standard_Integer aLodIdx = 0; aLodIdx < aLodsNb; ++aLodIdx)
    {
      StlAPI_Writer aWriter;
      aWriter.ASCIIMode() = Standard_False;
      TCollection_AsciiString aPath = aSavePath + aLodIdx + ".stl";
      StlAPI_ErrorStatus aStatus = aWriter.Write (aLodShapes[aLodIdx], aPath.ToCString());
      switch (aStatus)
      {
      case StlAPI_CannotOpenFile:
        std::cout << "Error! Cannot create/open a file with the name: " << aPath << std::endl;
        break;
      case StlAPI_StatusOK:
      default:
        std::cout << "LOD" << aLodIdx << " was written sucessfully to the file " << aPath << std::endl;
      }
    }
  }

  return 0;
}

#include <MeshVS_LODBuilder.hxx>
static int MeshLod (Draw_Interpretor& theDI,
                    Standard_Integer  theArgNum,
                    const char**      theArgs)
{
  struct DetailLevelData
  {
  public:
    DetailLevelData()
      : myMesh (NULL),
        myFrom (-DBL_MAX),
        myTo (DBL_MAX) {}

  public:
    Handle(StlMesh_Mesh) myMesh;
    Standard_Real        myFrom;
    Standard_Real        myTo;
  };

  if (theArgNum < 3)
  {
    std::cout << "Error! Wrong number of arguments. See usage:\n";
    theDI.PrintHelp (theArgs[0]);
    return 1;
  }

  Handle(AIS_InteractiveContext) aCtx = ViewerTest::GetAISContext();
  if (aCtx.IsNull())
  {
    std::cout << "Error! No opened viewer." << std::endl;
    return 1;
  }

  Standard_CString aShapeName = theArgs[1];
  if (GetMapOfAIS().IsBound2 (aShapeName))
  {
    std::cout << "Error! The context already has an interactive object with the name " << aShapeName << "." << std::endl;
    return 1;
  }
  const TCollection_AsciiString aPathToLODInfo = Draw::Atoi (theArgs[2]);
  if (aPathToLODInfo == "")
  {
    std::cout << "Error! Path to LOD info must not be empty!" << std::endl;
    return 1;
  }

  std::ifstream aLODInfoFile (theArgs[2]);
  NCollection_List<DetailLevelData> myLODDataList;
  Handle(StlMesh_Mesh) aLargestMesh;
  for (std::string aLODInfoStr; getline (aLODInfoFile, aLODInfoStr);)
  {
    DetailLevelData aData;
    std::istringstream aStream (aLODInfoStr);
    std::vector<std::string> aTokens;
    std::copy (std::istream_iterator<std::string> (aStream),
               std::istream_iterator<std::string>(),
               std::back_inserter (aTokens));
    for (Standard_Integer aTokenIdx = 0; aTokenIdx < aTokens.size(); ++aTokenIdx)
    {
      if (aTokens[aTokenIdx] == "-path")
      {
        OSD_Path aFile (aTokens[++aTokenIdx].c_str());
        Handle(Draw_ProgressIndicator) aProgress = new Draw_ProgressIndicator (theDI, 1);
        Handle(StlMesh_Mesh) aSTLMesh = RWStl::ReadFile (aFile, aProgress);
        if (aSTLMesh.IsNull())
        {
          std::cout << "Error! Can not read LOD located at the path: " << aTokens[aTokenIdx] << std::endl;
          return 1;
        }
        aData.myMesh = aSTLMesh;
        if (aLargestMesh.IsNull() || aSTLMesh->Triangles().Length() > aLargestMesh->Triangles().Length())
        {
          aLargestMesh = aSTLMesh;
        }
      }
      else if (aTokens[aTokenIdx] == "-from")
      {
        aData.myFrom = Draw::Atof (aTokens[++aTokenIdx].c_str());
      }
      else if (aTokens[aTokenIdx] == "-to")
      {
        aData.myTo = Draw::Atof (aTokens[++aTokenIdx].c_str());
      }
    }
    myLODDataList.Append (aData);
  }

  if (aLargestMesh.IsNull())
  {
    std::cout << "Error! No meshes found in lod info file!" << std::endl;
    return 1;
  }

  Handle(MeshVS_Mesh) anOriginMesh = new MeshVS_Mesh();
  Handle(XSDRAWSTLVRML_DataSource) anOriginDataSource = new XSDRAWSTLVRML_DataSource (aLargestMesh);
  anOriginMesh->SetDataSource (anOriginDataSource);
  Handle(MeshVS_MeshPrsBuilder) anOriginBuilder = new MeshVS_MeshPrsBuilder (anOriginMesh.operator->());
  anOriginMesh->AddBuilder (anOriginBuilder, Standard_True);
  anOriginMesh->GetDrawer()->SetColor (MeshVS_DA_EdgeColor, Quantity_NOC_YELLOW);

  for (NCollection_List<DetailLevelData>::Iterator aLodDataIter (myLODDataList); aLodDataIter.More(); aLodDataIter.Next())
  {
    Handle(MeshVS_LODDataSource) aLod = new MeshVS_LODDataSource (aLodDataIter.Value().myMesh);
    anOriginMesh->AddDataSource (aLod);
    Handle(MeshVS_LODBuilder) aLODBuilder = new MeshVS_LODBuilder (anOriginMesh.operator->());
    aLODBuilder->SetDataSource (aLod);
    aLODBuilder->SetDrawer (anOriginBuilder->GetDrawer());
    anOriginMesh->AddBuilder (aLODBuilder);
  }

  // Hide all nodes by default
  Handle(TColStd_HPackedMapOfInteger) aNodes = new TColStd_HPackedMapOfInteger();
  Standard_Integer aLen = aLargestMesh->Vertices().Length();
  for (Standard_Integer anIndex = 1; anIndex <= aLen; ++anIndex)
    aNodes->ChangeMap().Add (anIndex);
  anOriginMesh->SetHiddenNodes (aNodes);
  anOriginMesh->SetSelectableNodes (aNodes);

  VDisplayAISObject (aShapeName, anOriginMesh);
  aCtx->Deactivate (anOriginMesh);

  Standard_Integer aLodIdx = 0;
  for (NCollection_List<DetailLevelData>::Iterator aLodDataIter (myLODDataList); aLodDataIter.More(); aLodDataIter.Next())
  {
    anOriginMesh->Presentation()->SetDetailLevelRange (aLodIdx, aLodDataIter.Value().myFrom, aLodDataIter.Value().myTo);
  }

  Draw::Set (aShapeName, new XSDRAWSTLVRML_DrawableMesh (anOriginMesh));
  Handle(V3d_View) aView = ViewerTest::CurrentView();
  if (!aView.IsNull())
    aView->FitAll();

  return 0;
}

//-----------------------------------------------------------------------------

void  XSDRAWSTLVRML::InitCommands (Draw_Interpretor& theCommands)
{
  const char* g = "XSTEP-STL/VRML";  // Step transfer file commands
  //XSDRAW::LoadDraw(theCommands);

  theCommands.Add ("writevrml", "shape file [version VRML#1.0/VRML#2.0 (1/2): 2 by default] [representation shaded/wireframe/both (0/1/2): 1 by default]",__FILE__,writevrml,g);
  theCommands.Add ("writestl",  "shape file [ascii/binary (0/1) : 1 by default] [InParallel (0/1) : 0 by default]",__FILE__,writestl,g);
  theCommands.Add ("readstl",   "shape file",__FILE__,readstl,g);
  theCommands.Add ("loadvrml" , "shape file",__FILE__,loadvrml,g);

  theCommands.Add ("meshfromstl",     "creates MeshVS_Mesh from STL file",            __FILE__, createmesh,      g );
  theCommands.Add ("mesh3delem",      "creates 3d element mesh to test",              __FILE__, create3d,        g );
  theCommands.Add ("meshshadcolor",   "change MeshVS_Mesh shading color",             __FILE__, meshcolor,       g );
  theCommands.Add ("meshlinkcolor",   "change MeshVS_Mesh line color",                __FILE__, linecolor,       g );
  theCommands.Add ("meshmat",         "change MeshVS_Mesh material and transparency", __FILE__, meshmat,         g );
  theCommands.Add ("meshshrcoef",     "change MeshVS_Mesh shrink coeff",              __FILE__, shrink,          g );
  theCommands.Add ("meshclosed",      "meshclosed meshname (0/1) \nChange MeshVS_Mesh drawing mode. 0 - not closed object, 1 - closed object", __FILE__, closed, g);
  theCommands.Add ("meshshow",        "display MeshVS_Mesh object",                   __FILE__, mdisplay,        g );
  theCommands.Add ("meshhide",        "erase MeshVS_Mesh object",                     __FILE__, merase,          g );
  theCommands.Add ("meshhidesel",     "hide selected entities",                       __FILE__, hidesel,         g );
  theCommands.Add ("meshshowsel",     "show only selected entities",                  __FILE__, showonly,        g );
  theCommands.Add ("meshshowall",     "show all entities",                            __FILE__, showall,         g );
  theCommands.Add ("meshcolors",      "display color presentation",                   __FILE__, meshcolors,      g );
  theCommands.Add ("meshvectors",     "display sample vectors",                       __FILE__, meshvectors,     g );
  theCommands.Add ("meshtext",        "display text labels",                          __FILE__, meshtext,        g );
  theCommands.Add ("meshdeform",      "display deformed mesh",                        __FILE__, meshdeform,      g );
  theCommands.Add ("mesh_edge_width", "set width of edges",                           __FILE__, mesh_edge_width, g );
  theCommands.Add ("meshinfo",        "displays the number of nodes and triangles",   __FILE__, meshinfo,        g );
  theCommands.Add ("meshgenlods",
                   "meshgenlods shapeName lodsNb [-save pathToFolder] [-mesh meshName] [-debug]"
                   "\n\t\t: Generates sequence of LODs for the shape with shapeName in amount of lodsNb."
                   "\n\t\t: lodsNb means the exact amount of physical LODs to generate, e.g. the main"
                   "\n\t\t: object's presentation is not considered as a separate level of detail here."
                   "\n\t\t: [-debug] enables printing of meshing status and each LOD's info.",
                   __FILE__, MeshGenLods, g);
  theCommands.Add ("meshlod",
                   "meshlod shapeName pathToFileWithLodInfo"
                   "\n\t\t: Creates an object with the name shapeName that has LODs described in file stored at"
                   "\n\t\t: pathToFileWithLodInfo. Each string in the file is a full description of LOD. It must"
                   "\n\t\t: be filled using the following form:"
                   "\n\t\t: -path pathToLOD [-from fromRange] [-to toRange], where"
                   "\n\t\t: pathToLOD is a path to STL file with LOD mesh, fromRange and toRange define a depth"
                   "\n\t\t: range where the LOD will be visible. Each of range parameters is optional, but in this"
                   "\n\t\t: case, it will be equal to positive and negative max double correspondingly."
                   "\n\t\t: It is assumed that ranges are non-overlapping segments of the real axis."
                   __FILE__, MeshLod, g);
}

//==============================================================================
// XSDRAWSTLVRML::Factory
//==============================================================================
void XSDRAWSTLVRML::Factory(Draw_Interpretor& theDI)
{
  XSDRAWIGES::InitSelect();
  XSDRAWIGES::InitToBRep(theDI);
  XSDRAWIGES::InitFromBRep(theDI);
  XSDRAWSTEP::InitCommands(theDI);
  XSDRAWSTLVRML::InitCommands(theDI);
  XSDRAW::LoadDraw(theDI);
#ifdef OCCT_DEBUG
  theDI << "Draw Plugin : All TKXSDRAW commands are loaded\n";
#endif
}

// Declare entry point PLUGINFACTORY
DPLUGIN(XSDRAWSTLVRML)

