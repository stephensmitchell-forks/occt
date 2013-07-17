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

#ifndef _IGESBasic_HArray1OfHArray1OfReal_HeaderFile
#define _IGESBasic_HArray1OfHArray1OfReal_HeaderFile

#include <Standard_DefineHandle.hxx>
#include <Interface_JaggedArray.hxx>
#include <Handle_TColStd_HArray1OfReal.hxx>
class TColStd_HArray1OfReal;

class IGESBasic_HArray1OfHArray1OfReal : public Interface_JaggedArray <Handle(TColStd_HArray1OfReal)>
{
public:
  Standard_EXPORT IGESBasic_HArray1OfHArray1OfReal
                                        (const Standard_Integer theLowerIndex,
                                         const Standard_Integer theUpperIndex)
  :Interface_JaggedArray <Handle(TColStd_HArray1OfReal)>(theLowerIndex, theUpperIndex)
  {}
  DEFINE_STANDARD_RTTI(IGESBasic_HArray1OfHArray1OfReal);
};

DEFINE_STANDARD_HANDLE(IGESBasic_HArray1OfHArray1OfReal, MMgt_TShared)

#endif