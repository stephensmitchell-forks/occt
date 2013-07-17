// Copyright (c) 2013 OPEN CASCADE SAS
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

#ifndef _XSDRAW_Shape_HeaderFile
#define _XSDRAW_Shape_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Macro.hxx>

class Draw_Interpretor;


//! Defines functions to control shapes (in way useful for XSTEP), <br>
//!           additional features which should be basic, or call tools which <br>
//!           are bound with transfer needs. <br>
//!           But these functions work on shapes, geometry, nothing else <br>
//!           (no file, no model, no entity) <br>
class XSDRAW_Shape
{
public:

  DEFINE_STANDARD_ALLOC

  //! Defines and loads all basic functions for XSDRAW on Shapes <br>
  Standard_EXPORT   static  void InitCommands(Draw_Interpretor& theCommands);

};

#endif
