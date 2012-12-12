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

#import "Cocoa_AppDelegate.hxx"

// This is a draft version of application delegate for the Cocoa application.

@implementation Cocoa_AppDelegate

// Method applicationDidFinishLaunching is the main method of application.
// It is automatically called after [NSApp run] or NSApplicationMain if
// an instance of Cocoa_AppDelegate was set as application delegate.
- (void)applicationDidFinishLaunching:(NSNotification *)theNotification
{

/*
  NSTextView *accessory = [[NSTextView alloc] initWithFrame:NSMakeRect(0,0,200,15)];
  NSFont *font = [NSFont systemFontOfSize:[NSFont systemFontSize]];
  NSDictionary *textAttributes = [NSDictionary dictionaryWithObject:font forKey:NSFontAttributeName];
  [accessory insertText:[[NSAttributedString alloc] initWithString:@"Text in accessory view"
                                                        attributes:textAttributes]];
  [accessory setEditable:NO];
  [accessory setDrawsBackground:NO];

  NSAlert *alert = [[NSAlert alloc] init];
  [alert setMessageText:@"Message text"];
  [alert setInformativeText:@"Informative text"];
  [alert setAccessoryView:accessory];
  [alert runModal];
  [alert release];
*/

  NSWindow* aWindow;
  aWindow = [[NSWindow alloc] initWithContentRect: NSMakeRect(10, 100, 150, 200)
                                        styleMask: (NSResizableWindowMask | NSClosableWindowMask | NSTitledWindowMask)
                                          backing: NSBackingStoreBuffered
                                            defer: NO];

  [aWindow setTitle: @"Test window"];
  [aWindow makeKeyAndOrderFront:self];

}

@end
