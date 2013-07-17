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

#ifndef _Interface_JaggedArray_H__
#define _Interface_JaggedArray_H__

#include <MMgt_TShared.hxx>
#include <NCollection_Array1.hxx>

template <class KeyType>
class Interface_JaggedArray: public MMgt_TShared
{
private:
  NCollection_Array1<Handle(Standard_Transient)> myList;
public:
  Standard_EXPORT Interface_JaggedArray (const Standard_Integer theLowerIndex,
                                         const Standard_Integer theUpperIndex)
  : myList (theLowerIndex, theUpperIndex)
  {
    Handle(Standard_Transient) aNullObj;
    myList.Init(aNullObj);
  }

  Standard_EXPORT Standard_Integer Lower() const
  {
    return myList.Lower();
  }

  Standard_EXPORT Standard_Integer Upper() const
  {
    return myList.Upper();
  }

  Standard_EXPORT Standard_Integer Length() const
  {
    return myList.Length();
  }

  Standard_EXPORT void SetValue (const Standard_Integer theNum,
                 const KeyType& theVal)
  {
    myList.SetValue(theNum, theVal);
  }

  Standard_EXPORT KeyType Value (const Standard_Integer theNum) const
  {
    return KeyType::DownCast(myList.Value(theNum));
  }
};
#endif