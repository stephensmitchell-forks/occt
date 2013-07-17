// Copyright (c) 1999-2013 OPEN CASCADE SAS
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

#ifndef _TransferBRep_ShapeMapper_HeaderFile
#define _TransferBRep_ShapeMapper_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineHandle.hxx>
#include <Transfer_Mapper.hxx>
#include <TopoDS_Shape.hxx>
#include <Transfer_Finder.hxx>
#include <Standard_Boolean.hxx>
#include <Handle_Transfer_Finder.hxx>
#include <Handle_Standard_Type.hxx>
#include <Standard_CString.hxx>
#include <TransferBRep_ShapeInfo.hxx>
#include <TopTools_ShapeMapHasher.hxx>

class TopoDS_Shape;
class TopTools_ShapeMapHasher;
class TransferBRep_ShapeInfo;
class Transfer_Finder;
class Standard_Type;
class Standard_Transient;
class Handle(Standard_Type);
class Handle(Transfer_Finder);

DEFINE_STANDARD_HANDLE(TransferBRep_ShapeMapper,Transfer_Finder)


class TransferBRep_ShapeMapper : public Transfer_Mapper<TopoDS_Shape,
                                                   TopTools_ShapeMapHasher,
                                                   TransferBRep_ShapeInfo,
                                                   Handle(TransferBRep_ShapeMapper)>
 {
public:
  Standard_EXPORT TransferBRep_ShapeMapper(const TopoDS_Shape& theShape)
    : Transfer_Mapper<TopoDS_Shape,
                      TopTools_ShapeMapHasher,
                      TransferBRep_ShapeInfo,
                      Handle(TransferBRep_ShapeMapper)> (theShape)
  { }

  DEFINE_STANDARD_RTTI(TransferBRep_ShapeMapper)
 };
#endif
