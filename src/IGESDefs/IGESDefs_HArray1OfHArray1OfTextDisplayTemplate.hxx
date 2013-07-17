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

#ifndef _IGESDefs_HArray1OfHArray1OfTextDisplayTemplate_H__
#define _IGESDefs_HArray1OfHArray1OfTextDisplayTemplate_H__

#include <Standard_DefineHandle.hxx>
#include <Interface_JaggedArray.hxx>
#include <Handle_IGESGraph_HArray1OfTextDisplayTemplate.hxx>

class IGESGraph_HArray1OfTextDisplayTemplate;

class IGESDefs_HArray1OfHArray1OfTextDisplayTemplate : public Interface_JaggedArray <Handle(IGESGraph_HArray1OfTextDisplayTemplate)>
{
public:
  Standard_EXPORT IGESDefs_HArray1OfHArray1OfTextDisplayTemplate
                                        (const Standard_Integer theLowerIndex,
                                         const Standard_Integer theUpperIndex)
  :Interface_JaggedArray <Handle(IGESGraph_HArray1OfTextDisplayTemplate)> (theLowerIndex, theUpperIndex)
  {}
  DEFINE_STANDARD_RTTI(IGESDefs_HArray1OfHArray1OfTextDisplayTemplate);
};

DEFINE_STANDARD_HANDLE(IGESDefs_HArray1OfHArray1OfTextDisplayTemplate, MMgt_TShared)

#endif
