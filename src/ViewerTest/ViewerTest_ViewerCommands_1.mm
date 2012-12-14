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

#if defined(__APPLE__) && !defined(MACOSX_USE_GLX)

#import <Cocoa/Cocoa.h>

#include <Cocoa_Window.hxx>
#include <ViewerTest.hxx>
#include <Xw_Window.hxx>
#include <TCollection_ExtendedString.hxx>

#include <V3d_Viewer.hxx>
#include <AIS_InteractiveContext.hxx>
#include <AIS_Shape.hxx>
#include <BRep_Builder.hxx>
#include <BRepTools.hxx>
#include <Graphic3d_GraphicDevice.hxx>
#include <V3d_View.hxx>
#include <OSD_Environment.hxx>
#include <NIS_View.hxx>
#include <ViewerTest_CocoaEventManagerView.hxx>

static Handle(Cocoa_Window) ViewerTest_myCocoaWindow;
static Handle(Aspect_GraphicDevice) ViewerTest_myDevice3d;

extern const Handle(NIS_InteractiveContext)& TheNISContext();

void ViewerTest::ViewerInit (const Standard_Integer thePxLeft,  const Standard_Integer thePxTop,
                             const Standard_Integer thePxWidth, const Standard_Integer thePxHeight)
{
  static Standard_Boolean isFirst = Standard_True;

  Standard_Integer aPxLeft   = 20;
  Standard_Integer aPxTop    = 40;
  Standard_Integer aPxWidth  = 409;
  Standard_Integer aPxHeight = 409;
  if (thePxWidth != 0 && thePxHeight != 0)
  {
    aPxLeft   = thePxLeft;
    aPxTop    = thePxTop;
    aPxWidth  = thePxWidth;
    aPxHeight = thePxHeight;
  }

  if (isFirst)
  {

    NSRect aRect = NSMakeRect (aPxLeft, aPxTop, aPxWidth, aPxHeight);
    NSWindow * aWin = [NSWindow alloc];
    [aWin initWithContentRect: aRect
                    styleMask: NSTitledWindowMask | NSClosableWindowMask | NSResizableWindowMask
                      backing: NSBackingStoreBuffered
                        defer: NO];

    [aWin setTitle: @"Test3d"];
    [aWin makeKeyAndOrderFront: NSApp];
    TCollection_ExtendedString aNameOfWindow ("Visu3D");
    ViewerTest_myDevice3d = new Graphic3d_GraphicDevice (getenv ("DISPLAY"), Xw_TOM_READONLY);

    Handle(V3d_Viewer) a3DViewer = new V3d_Viewer (ViewerTest_myDevice3d, aNameOfWindow.ToExtString());
    a3DViewer->SetDefaultBackgroundColor (Quantity_NOC_BLACK);
    Handle(V3d_View) aView3d = a3DViewer->CreateView();

    Handle(AIS_InteractiveContext) aCtxAis = new AIS_InteractiveContext (a3DViewer);

    ViewerTest_CocoaEventManagerView* aViewNs = [ViewerTest_CocoaEventManagerView alloc];
    [aViewNs initWithView3d: aView3d nsWin: aWin contextAis: aCtxAis];

    ViewerTest_myCocoaWindow = new Cocoa_Window ([aWin contentView]);

    aView3d->SetWindow (ViewerTest_myCocoaWindow);

    Handle(NIS_View) aView = Handle(NIS_View)::DownCast (ViewerTest::CurrentView());
    if (aView.IsNull())
    {
      aView = new NIS_View (a3DViewer, ViewerTest_myCocoaWindow);
      ViewerTest::CurrentView (aView);
      TheNISContext()->AttachView (aView);
    }

    if (ViewerTest::GetAISContext().IsNull())
    {
      ViewerTest::SetAISContext (aCtxAis);
    }

    Handle(V3d_View) aCurrentView = ViewerTest::CurrentView();
    aCurrentView->SetDegenerateModeOn();

    aCurrentView->SetZClippingDepth (0.5);
    aCurrentView->SetZClippingWidth (0.5);

    a3DViewer->SetDefaultLights();
    a3DViewer->SetLightOn();

    isFirst = Standard_False;
  }

  ViewerTest_myCocoaWindow->Map();

}

void ViewerTest_InitViewerTest (const Handle(AIS_InteractiveContext)& context)
{

}

int ViewerMainLoop(Standard_Integer argc, const char** argv)
{
  return 0;
}

#endif
