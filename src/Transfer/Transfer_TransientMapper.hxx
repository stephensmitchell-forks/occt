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

#ifndef _Transfer_TransientMapper_HeaderFile
#define _Transfer_TransientMapper_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineHandle.hxx>
#include <Handle_Standard_Transient.hxx>
#include <Transfer_Finder.hxx>
#include <Standard_Boolean.hxx>
#include <Handle_Transfer_Finder.hxx>
#include <Handle_Standard_Type.hxx>
#include <Transfer_Mapper.hxx>
#include <Transfer_DataInfo.hxx>
#include <TColStd_MapTransientHasher.hxx>

class Standard_Transient;
class TColStd_MapTransientHasher;
class Transfer_DataInfo;
class Transfer_Finder;
class Standard_Type;
class Standard_Transient;
class Handle(Standard_Type);
class Handle(Transfer_Finder);
class Transfer_TransientMapper;

DEFINE_STANDARD_HANDLE(Transfer_TransientMapper,Transfer_Finder)

class Transfer_TransientMapper : public Transfer_Mapper<Handle(Standard_Transient),
                                                         TColStd_MapTransientHasher,
                                                         Transfer_DataInfo,
                                                         Handle(Transfer_TransientMapper)>
 {
   public:
     Standard_EXPORT Transfer_TransientMapper(const Handle(Standard_Transient) theKey)
     : Transfer_Mapper<Handle(Standard_Transient),
                       TColStd_MapTransientHasher,
                       Transfer_DataInfo,
                       Handle(Transfer_TransientMapper)>(theKey)
     { }
     DEFINE_STANDARD_RTTI(Transfer_TransientMapper)
     
 };

#endif