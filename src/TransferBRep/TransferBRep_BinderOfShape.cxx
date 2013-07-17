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

#include <TransferBRep_BinderOfShape.hxx>

#include <Standard_Type.hxx>
#include <Transfer_TransferFailure.hxx>
#include <TopoDS_Shape.hxx>
#include <TransferBRep_ShapeInfo.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_TYPE(TransferBRep_BinderOfShape)
IMPLEMENT_STANDARD_SUPERTYPE_ARRAY()
  STANDARD_TYPE(Transfer_Binder),
  STANDARD_TYPE(MMgt_TShared),
  STANDARD_TYPE(Standard_Transient),

IMPLEMENT_STANDARD_SUPERTYPE_ARRAY_END()
IMPLEMENT_STANDARD_TYPE_END(TransferBRep_BinderOfShape)

IMPLEMENT_DOWNCAST(TransferBRep_BinderOfShape,Standard_Transient)
IMPLEMENT_STANDARD_RTTI(TransferBRep_BinderOfShape)

TransferBRep_BinderOfShape::TransferBRep_BinderOfShape ()
{ }

TransferBRep_BinderOfShape::TransferBRep_BinderOfShape (const TopoDS_Shape& res)
 : theres (res)
{
  SetResultPresent();
}

Handle(Standard_Type)  TransferBRep_BinderOfShape::ResultType () const
{
  return  TransferBRep_ShapeInfo::Type (theres);
}  // corresponds a "STANDARD_TYPE(TopoDS_Shape)"

Standard_CString  TransferBRep_BinderOfShape::ResultTypeName () const
{
  return  TransferBRep_ShapeInfo::TypeName (theres);
}  // corresponds a "STANDARD_TYPE(TopoDS_Shape)"


void  TransferBRep_BinderOfShape::SetResult (const TopoDS_Shape& res)
{
  SetResultPresent();
  theres = res;
}

const TopoDS_Shape&  TransferBRep_BinderOfShape::Result () const
{
  return theres;
}

TopoDS_Shape&  TransferBRep_BinderOfShape::CResult ()
{
  SetResultPresent(); return theres;
}