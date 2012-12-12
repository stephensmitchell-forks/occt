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

static NSWindow * myWin;
static Handle(Cocoa_Window) myWindow;
static Handle(Aspect_GraphicDevice) myDevice3d;

extern const Handle(NIS_InteractiveContext)& TheNISContext();

void ViewerTest::ViewerInit (const Standard_Integer thePxLeft,  const Standard_Integer thePxTop,
                             const Standard_Integer thePxWidth, const Standard_Integer thePxHeight)
{
  static Standard_Boolean isFirst = Standard_True;

  // Default position and dimension of the viewer window.
  // Note that left top corner is set to be sufficiently small to have
  // window fit in the small screens (actual for remote desktops, see #23003).
  // The position corresponds to the window's client area, thus some
  // gap is added for window frame to be visible.
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

    NSRect aRect = NSMakeRect(256, 256, 512, 512);
    myWin = [NSWindow alloc];
    [myWin initWithContentRect: aRect
                    styleMask: NSTitledWindowMask | NSClosableWindowMask | NSResizableWindowMask
     //styleMask: NSBorderlessWindowMask
                      backing: NSBackingStoreBuffered
                        defer: NO];

    [myWin setTitle: @"Test3d"];
    [myWin makeKeyAndOrderFront: NSApp];
    TCollection_ExtendedString aNameOfWindow ("Visu3D");
    myDevice3d = new Graphic3d_GraphicDevice (getenv ("DISPLAY"), Xw_TOM_READONLY);

    Handle(V3d_Viewer) aViewer3d = new V3d_Viewer (myDevice3d, aNameOfWindow.ToExtString());
    aViewer3d->SetDefaultBackgroundColor (Quantity_NOC_BLACK);


    myWindow = new Cocoa_Window ([myWin contentView]);
    Handle(V3d_View) aView3d = aViewer3d->CreateView();
    aView3d->SetWindow (myWindow);

    Handle(NIS_View) aView = Handle(NIS_View)::DownCast (ViewerTest::CurrentView());
    if (aView.IsNull())
    {
      aView = new NIS_View (aViewer3d, myWindow);
      ViewerTest::CurrentView (aView);
      TheNISContext()->AttachView (aView);
    }

    if (ViewerTest::GetAISContext().IsNull())
    {
      Handle(AIS_InteractiveContext) aCtxAis = new AIS_InteractiveContext (aViewer3d);
      ViewerTest::SetAISContext(aCtxAis);
    }

    //aView3d->SetZClippingDepth(0.5);
    //aView3d->SetZClippingWidth(ZCLIPWIDTH/2.);
    aViewer3d->SetDefaultLights();
    aViewer3d->SetLightOn();

    //CocoaSample_View* aViewNs = [CocoaSample_View alloc];
    //[aViewNs initWithView3d: aView3d nsWin: aWin contextAis: myCtxAis]

    isFirst = Standard_False;
  }

  myWindow->Map();

}

void ViewerTest_InitViewerTest (const Handle(AIS_InteractiveContext)& context)
{

}

int ViewerMainLoop(Standard_Integer argc, const char** argv)
{
  return 0;
}

#endif
