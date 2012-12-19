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

#include <AIS_InteractiveContext.hxx>
#include <Cocoa_Window.hxx>
#include <V3d_View.hxx>

#include "SampleViewer_View.hxx"

@implementation SampleViewer_View

- (id ) initWithView3d: (Handle_V3d_View& )               theView3d
                 nsWin: (NSWindow* )                      theNsWin
            contextAis: (Handle_AIS_InteractiveContext& ) theCtxAis
{
  NSRect aBounds = [[theNsWin contentView] bounds];
  self = [super initWithFrame: aBounds];
  if (self == NULL)
  {
    return NULL;
  }
  myView3d = theView3d;
  myCtxAis = theCtxAis;

  // replace content view in the window
  [theNsWin setContentView: self];

  // make view as first responder in winow to capture all useful events
  [theNsWin makeFirstResponder: self];
  //if (!myCtxAis.IsNull())
  [theNsWin setAcceptsMouseMovedEvents: YES];

  return self;
}

- (Handle_V3d_View )view3d
{
  return myView3d;
}

- (void )setFrameSize: (NSSize )theNewSize
{
  [super setFrameSize: theNewSize];

  myView3d->MustBeResized();
  myView3d->Update();
}

- (void )drawRect: (NSRect )theDirtyRect
{
  myView3d->Redraw();
}

- (void )mouseMoved: (NSEvent* )theEvent
{
  NSPoint aMouseLoc = [self convertPoint: [theEvent locationInWindow] fromView: nil];
  NSRect  aBounds   = [self bounds];
  if (!myCtxAis.IsNull())
  {
    // AIS_InteractiveContext (0,0) coordinates are in upper-left corner
    myCtxAis->MoveTo (Standard_Integer(aMouseLoc.x),
                      Standard_Integer(aBounds.size.height - aMouseLoc.y),
                      myView3d);
  }
}

- (BOOL )acceptsFirstResponder
{
  return YES;
}

- (void )mouseDown: (NSEvent* )theEvent
{
  //CGEventRef anEvent = CGEventCreate (NULL);
  //CGPoint    aCursor = CGEventGetLocation (anEvent);
  //CFRelease (anEvent);
  //myCtxAis->Select();
}

- (void )mouseUp: (NSEvent* )theEvent
{
  myCtxAis->Select();
}

- (void ) scrollWheel: (NSEvent* ) theEvent
{
  if (Abs ([theEvent deltaY]) < 0.001)
  {
    return;
  }
}

- (void )keyDown: (NSEvent* )theEvent
{
  /*NSString* aStringNs = [theEvent characters];
  if (aStringNs == NULL || [aStringNs length] == 0)
  {
    return;
  }
  const Standard_CString aString = [aStringNs UTF8String];*/
  switch ([theEvent keyCode])
  {
    case 1: // s
    {
      myCtxAis->SetDisplayMode (AIS_Shaded);
      break;
    }
    case 13: // w
    {
      myCtxAis->SetDisplayMode (AIS_WireFrame);
      break;
    }
    case 3: // f
    {
      myView3d->FitAll();
      break;
    }
    default: break;
  }
}

- (void ) keyUp: (NSEvent* ) theEvent
{
  //
}

@end
