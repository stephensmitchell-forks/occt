// Created on: 2012-12-19
// Created by: Kirill GAVRILOV
// Copyright (c) 2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

#import "SampleViewer_AppDelegate.hxx"
#import "SampleViewer_View.hxx"

#include <AIS_InteractiveContext.hxx>
#include <AIS_Shape.hxx>
#include <BRep_Builder.hxx>
#include <BRepTools.hxx>
#include <Cocoa_Window.hxx>
#include <Graphic3d_GraphicDevice.hxx>
#include <V3d_View.hxx>
#include <V3d_Viewer.hxx>
#include <OSD_Environment.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_ExtendedString.hxx>

@implementation SampleViewer_AppDelegate

@synthesize window;

- (void )applicationDidFinishLaunching: (NSNotification* )theNotification
{
  TCollection_ExtendedString aNameOfWindow ("Visu3D");
  Handle(Aspect_GraphicDevice) aDevice3d = new Graphic3d_GraphicDevice (getenv ("DISPLAY"), Xw_TOM_READONLY);

  myViewer3d = new V3d_Viewer (aDevice3d, aNameOfWindow.ToExtString());
  myViewer3d->SetDefaultBackgroundColor (Quantity_NOC_BLACK);
  myViewer3d->SetDefaultLights();
  myViewer3d->SetLightOn();

  Handle(V3d_View) aView3d = myViewer3d->CreateView();

  myCtxAis = new AIS_InteractiveContext (myViewer3d);

  SampleViewer_View* aViewNs = [SampleViewer_View alloc];
  [aViewNs initWithView3d: aView3d nsWin: window contextAis: myCtxAis];

  Handle(Cocoa_Window) aWindow = new Cocoa_Window ([window contentView]);

  aView3d->SetWindow (aWindow);

  OSD_Environment aCasRootEnv ("CASROOT");
  TCollection_AsciiString aFolder = aCasRootEnv.Value() + "/data/occ";
  myModelsDir = [[NSURL fileURLWithPath: [NSString stringWithUTF8String: aFolder.ToCString()]] retain];
}

- (BOOL )loadFile
{
  if (myFilePath.IsEmpty() || myCtxAis.IsNull())
  {
    return NO;
  }

  myCtxAis->EraseAll();
  BRep_Builder aBuilder;
  TopoDS_Shape aShape;
  BRepTools::Read (aShape, myFilePath.ToCString(), aBuilder);
  Handle(AIS_Shape) anIO = new AIS_Shape (aShape);
  myCtxAis->Display (anIO, 0, 0, Standard_True);

  SampleViewer_View* aViewNs = [window contentView];
  Handle(V3d_View) aView3d = [aViewNs view3d];
  aView3d->FitAll();
  aView3d->Update();

  return YES;
}

//! Handler to open file.
- (void )openDocument: (id )theSender
{
  NSOpenPanel* aFilePanel = [NSOpenPanel openPanel];
  [aFilePanel setTitle: @"Load model from file"];
  [aFilePanel setDirectoryURL: myModelsDir];

  NSMutableArray* aFilter = [NSMutableArray arrayWithCapacity: 5];
  [aFilter addObject: @"brep"];
  [aFilter addObject: @"rle"];
  [aFilter addObject: @"igs"];
  [aFilter addObject: @"iges"];
  [aFilter addObject: @"stp"];
  [aFilePanel setAllowedFileTypes: aFilter];

  myFilePath.Clear();
  if ([aFilePanel runModal] == NSOKButton)
  {
    // automatically convert filenames from decomposed form used by Mac OS X file systems
    NSString* aFileName = [[aFilePanel filename] precomposedStringWithCanonicalMapping];
    myFilePath = [aFileName UTF8String];

    // remember folder
    [myModelsDir release];
    myModelsDir = [[aFilePanel directoryURL] retain];
    [self loadFile];
  }
}

//! Cocoa interface for opening file with associated application.
- (BOOL )application: (NSApplication* ) theApplication
            openFile: (NSString*      ) theFilename
{
  myFilePath = TCollection_AsciiString ([[theFilename precomposedStringWithCanonicalMapping] UTF8String]);
  if (myFilePath.IsEmpty())
  {
    return NO;
  }
  else if (myCtxAis.IsNull())
  {
    return YES; // application is not yet started
  }

  return [self loadFile];
}

@end
