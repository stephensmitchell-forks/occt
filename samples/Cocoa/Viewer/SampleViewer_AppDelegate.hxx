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

#import <Cocoa/Cocoa.h>

#include <Handle_AIS_InteractiveContext.hxx>
#include <Handle_V3d_Viewer.hxx>
#include <TCollection_AsciiString.hxx>

@interface SampleViewer_AppDelegate : NSObject <NSApplicationDelegate>
{
  Handle_V3d_Viewer             myViewer3d;
  Handle_AIS_InteractiveContext myCtxAis;
  TCollection_AsciiString       myFilePath;
  NSURL*                        myModelsDir;
  NSWindow*                     window;
}

@property (assign) IBOutlet NSWindow* window;

//! Load model from specified file myFilePath.
- (BOOL )loadFile;

- (void )openDocument: (id )theSender;

@end
