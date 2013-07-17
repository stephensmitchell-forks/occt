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

#ifndef _XSDRAW_Functions_HeaderFile
#define _XSDRAW_Functions_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Macro.hxx>

//! Defines additionnal commands for XSDRAW to : <br>
//!           - control of initialisation (xinit, xnorm, newmodel) <br>
//!           - analyse of the result of a transfer (recorded in a <br>
//!             TransientProcess for Read, FinderProcess for Write) : <br>
//!             statistics, various lists (roots,complete,abnormal), what <br>
//!             about one specific entity, producing a model with the <br>
//!             abnormal result <br>
//! <br>
//!           This appendix of XSDRAW is compiled separately to distinguish <br>
//!           basic features from user callable forms <br>
class XSDRAW_Functions
{
public:

  DEFINE_STANDARD_ALLOC

  //! Defines and loads all basic functions for XSDRAW (as ActFunc) <br>
  Standard_EXPORT   static  void Init() ;
};
#endif
