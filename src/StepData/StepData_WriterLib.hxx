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

#ifndef _StepData_WriterLib_HeaderFile
#define _StepData_WriterLib_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Macro.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>
#include <Standard_DefineHandle.hxx>
#include <Standard_Transient.hxx>
#include <Handle_Standard_Transient.hxx>
#include <LibCtl_Library.hxx>
#include <Handle_StepData_ReadWriteModule.hxx>
#include <Handle_StepData_Protocol.hxx>
#include <MMgt_TShared.hxx>
#include <StepData_Protocol.hxx>


class StepData_NodeOfWriterLib;
class Standard_NoSuchObject;
class Standard_Transient;
class StepData_ReadWriteModule;
class StepData_Protocol;

typedef LibCtl_Library <Handle(Standard_Transient),
                        Handle(StepData_ReadWriteModule),
                        Handle(StepData_Protocol)>
        LibCtl_StepWriterLib;

//template <>
//NCollection_Handle <LibCtl_GlobalNode<Handle(StepData_ReadWriteModule),
//                                      Handle(StepData_Protocol)> >
//LibCtl_StepWriterLib::theglobal;
//
//template <>
//Handle(StepData_Protocol)
//LibCtl_StepWriterLib::theprotocol;
//
//template <>
//NCollection_Handle <LibCtl_Node<Handle(StepData_ReadWriteModule),
//                                Handle(StepData_Protocol)> >
//LibCtl_StepWriterLib::thelast;

class StepData_WriterLib : public LibCtl_StepWriterLib
{
public:
  DEFINE_STANDARD_ALLOC
  Standard_EXPORT   StepData_WriterLib()
    : LibCtl_StepWriterLib() { }
  
  Standard_EXPORT   StepData_WriterLib(const Handle(StepData_Protocol)& theProtocol);

  Standard_EXPORT void SetComplete ();

  Standard_EXPORT void AddProtocol (const Handle(Standard_Transient)& theProtocol);

  Standard_EXPORT static void SetGlobal (const Handle(StepData_ReadWriteModule)& theModule,
                                         const Handle(StepData_Protocol)& theProtocol);

};

#endif