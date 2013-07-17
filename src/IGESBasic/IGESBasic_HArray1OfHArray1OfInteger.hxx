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

#ifndef _IGESBasic_HArray1OfHArray1OfInteger_HeaderFile
#define _IGESBasic_HArray1OfHArray1OfInteger_HeaderFile

#include <Standard_DefineHandle.hxx>
#include <Interface_JaggedArray.hxx>
#include <Handle_TColStd_HArray1OfInteger.hxx>

class TColStd_HArray1OfInteger;

class IGESBasic_HArray1OfHArray1OfInteger : public Interface_JaggedArray <Handle(TColStd_HArray1OfInteger)>
{
public:
  Standard_EXPORT IGESBasic_HArray1OfHArray1OfInteger
                                        (const Standard_Integer theLowerIndex,
                                         const Standard_Integer theUpperIndex)
  :Interface_JaggedArray<Handle(TColStd_HArray1OfInteger)>(theLowerIndex, theUpperIndex)
  {}
  DEFINE_STANDARD_RTTI(IGESBasic_HArray1OfHArray1OfInteger);
};

DEFINE_STANDARD_HANDLE(IGESBasic_HArray1OfHArray1OfInteger, MMgt_TShared)

#endif
