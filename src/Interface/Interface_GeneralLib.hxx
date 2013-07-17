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

#ifndef _Interface_GeneralLib_HeaderFile
#define _Interface_GeneralLib_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Macro.hxx>
#include <Standard_DefineHandle.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Transient.hxx>
#include <LibCtl_Library.hxx>
#include <Handle_Standard_Transient.hxx>
#include <Handle_Interface_GeneralModule.hxx>
#include <Interface_Protocol.hxx>
#include <MMgt_TShared.hxx>


class Standard_NoSuchObject;
class Standard_Transient;
class Interface_GeneralModule;
class Interface_Protocol;

typedef LibCtl_Library <Handle(Standard_Transient),
                        Handle(Interface_GeneralModule),
                        Handle(Interface_Protocol)>
        Interface_GeneralLib;

#endif
