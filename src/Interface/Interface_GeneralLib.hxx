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
        LibCtl_GeneralLib;

//template <>
//NCollection_Handle <LibCtl_GlobalNode<Handle(Interface_GeneralModule),
//                                      Handle(Interface_Protocol)> >
//LibCtl_GeneralLib::theglobal;
//
//template <>
//Handle(Interface_Protocol)
//LibCtl_GeneralLib::theprotocol;
//
//template <>
//NCollection_Handle <LibCtl_Node<Handle(Interface_GeneralModule),
//                                Handle(Interface_Protocol)> >
//LibCtl_GeneralLib::thelast;

class Interface_GeneralLib : public LibCtl_GeneralLib
{
public:
  DEFINE_STANDARD_ALLOC
  Standard_EXPORT   Interface_GeneralLib()
  : LibCtl_GeneralLib ()
  { }
  
  Standard_EXPORT   Interface_GeneralLib(const Handle(Interface_Protocol)& theProtocol);
  
      Standard_EXPORT const Handle(Interface_Protocol)& Protocol () const
  {return  LibCtl_GeneralLib::Protocol();}

  Standard_EXPORT const Handle(Interface_GeneralModule)& Module () const
  {return  LibCtl_GeneralLib::Module();}

  Standard_EXPORT void Next ()
  { LibCtl_GeneralLib::Next();}

  Standard_EXPORT Standard_Boolean More () const
  {return  LibCtl_GeneralLib::More();}

  Standard_EXPORT void Start ()
  { LibCtl_GeneralLib::Start();}

  Standard_EXPORT Standard_Boolean Select (const Handle(Standard_Transient)& obj,
                                           Handle(Interface_GeneralModule)& module,
                                           Standard_Integer& CN) const
  {return  LibCtl_GeneralLib::Select(obj,module,CN);}

  Standard_EXPORT void SetComplete ();

  Standard_EXPORT void AddProtocol (const Handle(Standard_Transient)& theProtocol);

  Standard_EXPORT static void SetGlobal (const Handle(Interface_GeneralModule)& theModule,
                                         const Handle(Interface_Protocol)& theProtocol);
};

#endif
