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

#ifndef _TransferBRep_BinderOfShape_HeaderFile
#define _TransferBRep_BinderOfShape_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineHandle.hxx>
#include <TopoDS_Shape.hxx>
#include <Transfer_Binder.hxx>
#include <Handle_Standard_Type.hxx>
#include <Standard_CString.hxx>

class Transfer_TransferFailure;
class TopoDS_Shape;
class TransferBRep_ShapeInfo;
class Standard_Type;
class Standard_Transient;
class Handle(Standard_Type);
class Handle(Transfer_Binder);
class TransferBRep_BinderOfShape;

DEFINE_STANDARD_HANDLE(TransferBRep_BinderOfShape,Transfer_Binder)

class TransferBRep_BinderOfShape : public Transfer_Binder
{
public:
  Standard_EXPORT   TransferBRep_BinderOfShape();
  
  Standard_EXPORT   TransferBRep_BinderOfShape(const TopoDS_Shape& res);
  
  Standard_EXPORT     Handle_Standard_Type ResultType() const;
  
  Standard_EXPORT     Standard_CString ResultTypeName() const;
  
  Standard_EXPORT     void SetResult(const TopoDS_Shape& res) ;
  
  Standard_EXPORT    const TopoDS_Shape& Result() const;
  
  Standard_EXPORT     TopoDS_Shape& CResult() ;

  DEFINE_STANDARD_RTTI(TransferBRep_BinderOfShape)

private: 
  TopoDS_Shape theres;
};
#endif