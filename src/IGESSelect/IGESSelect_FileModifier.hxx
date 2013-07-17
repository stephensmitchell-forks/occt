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

#ifndef _IGESSelect_FileModifier_HeaderFile
#define _IGESSelect_FileModifier_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineHandle.hxx>
#include <IFSelect_GeneralModifier.hxx>

class IGESData_IGESWriter;
class IFSelect_ContextWrite;
class Standard_Transient;
class Handle(Standard_Type);
class Handle(IFSelect_GeneralModifier);
class IGESSelect_FileModifier;

DEFINE_STANDARD_HANDLE(IGESSelect_FileModifier,IFSelect_GeneralModifier)

class IGESSelect_FileModifier : public IFSelect_GeneralModifier
{

public:
  Standard_EXPORT   virtual  void Perform(IFSelect_ContextWrite& ctx,IGESData_IGESWriter& writer) const = 0;

  DEFINE_STANDARD_RTTI(IGESSelect_FileModifier)
protected:

  Standard_EXPORT IGESSelect_FileModifier ()
    :  IFSelect_GeneralModifier (Standard_False)
  { }

};
#endif
