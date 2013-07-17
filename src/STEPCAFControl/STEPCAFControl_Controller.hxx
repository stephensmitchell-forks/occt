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

#ifndef _STEPCAFControl_Controller_HeaderFile
#define _STEPCAFControl_Controller_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineHandle.hxx>
#include <STEPControl_Controller.hxx>
#include <Standard_Boolean.hxx>
#include <Standard.hxx>
#include <Standard_DefineHandle.hxx>

class Standard_Transient;
class Handle(Standard_Type);
class Handle(STEPControl_Controller);
class STEPCAFControl_Controller;

DEFINE_STANDARD_HANDLE(STEPCAFControl_Controller,STEPControl_Controller)

//! Extends Controller from STEPControl in order to provide <br>
//!          ActorWrite adapted for writing assemblies from DECAF <br>
//!          Note that ActorRead from STEPControl is used for reading <br>
//!          (inherited automatically) <br>
class STEPCAFControl_Controller : public STEPControl_Controller
{

public:

  //! Initializes the use of STEP Norm (the first time) <br>
  Standard_EXPORT   STEPCAFControl_Controller();
  
  //! Standard Initialisation. It creates a Controller for STEP-XCAF <br>
  //!           and records it to various names, available to select it later <br>
  //!           Returns True when done, False if could not be done <br>
  Standard_EXPORT   static  Standard_Boolean Init() ;

  DEFINE_STANDARD_RTTI(STEPCAFControl_Controller)
};
#endif
