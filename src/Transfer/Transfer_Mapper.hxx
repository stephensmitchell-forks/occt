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

#ifndef _Transfer_Mapper_HeaderFile
#define _Transfer_Mapper_HeaderFile

#include <Transfer_Finder.hxx>
#include <Standard_CString.hxx>

  //!  Mapper defines a Finder for a specific input class
  //!  its definition includes the value of the Key to be mapped,
  //!  and the HashCoder associated to the class of the Key
  //!  TheKey defines the class to be keyed
  //!  TheHasher is the associated Hasher
  //!  TheInfo   is an additionnal class which helps to provide
  //!  informations on the value (template : see DataInfo)
template <class TheKey, class TheHasher, class TheInfo, class MapperHandle>
class Transfer_Mapper : public Transfer_Finder
{
private:
  TheKey theval;
public:
  Standard_EXPORT Transfer_Mapper (const TheKey& akey)
  : theval (akey)
  {
    SetHashCode ( TheHasher::HashCode (akey, IntegerLast() ) );
  }
  
  Standard_EXPORT const TheKey&  Value () const
  {  return theval;  }

  Standard_EXPORT Standard_Boolean Equates (const Handle(Transfer_Finder)& other) const
  {
    if (other.IsNull()) return Standard_False;
    if (GetHashCode() != other->GetHashCode()) return Standard_False;
    if (other->DynamicType() != DynamicType()) return Standard_False;
    MapperHandle another = MapperHandle::DownCast(other);
    return  TheHasher::IsEqual (theval,another->Value());
  }

  Standard_EXPORT Handle(Standard_Type) ValueType () const
  {  return TheInfo::Type(theval);  }

  Standard_EXPORT Standard_CString ValueTypeName () const
  {  return TheInfo::TypeName(theval);  }
};
#endif