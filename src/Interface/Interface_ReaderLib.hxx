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

#ifndef _Interface_ReaderLib_HeaderFile
#define _Interface_ReaderLib_HeaderFile

#include <Standard_DefineAlloc.hxx>
#include <Standard_Macro.hxx>
#include <Standard_DefineHandle.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>
#include <LibCtl_Library.hxx>
#include <MMgt_TShared.hxx>
#include <Handle_Interface_ReaderModule.hxx>
#include <Interface_Protocol.hxx>
#include <Standard_Transient.hxx>

class Interface_ReaderModule;
class Interface_Protocol;
class Standard_NoSuchObject;

typedef LibCtl_Library <Handle(Standard_Transient),
                Handle(Interface_ReaderModule),
                Handle(Interface_Protocol)>
        LibCtl_ReaderLib;

//template <>
//NCollection_Handle <LibCtl_GlobalNode<Handle(Interface_ReaderModule),
//                                      Handle(Interface_Protocol)> >
//LibCtl_ReaderLib::theglobal;
//
//template <>
//Handle(Interface_Protocol)
//LibCtl_ReaderLib::theprotocol;
//
//template <>
//NCollection_Handle <LibCtl_Node<Handle(Interface_ReaderModule),
//                                Handle(Interface_Protocol)> >
//LibCtl_ReaderLib::thelast;

// Library
class Interface_ReaderLib : public LibCtl_ReaderLib
{
public:
  DEFINE_STANDARD_ALLOC
  Standard_EXPORT   Interface_ReaderLib()
  : LibCtl_ReaderLib ()
  { }
  
  Standard_EXPORT   Interface_ReaderLib(const Handle(Interface_Protocol)& theProtocol);

  Standard_EXPORT void SetComplete ();

  Standard_EXPORT void AddProtocol (const Handle(Standard_Transient)& theProtocol);

  Standard_EXPORT static void SetGlobal (const Handle(Interface_ReaderModule)& theModule,
                                         const Handle(Interface_Protocol)& theProtocol);
};

#endif