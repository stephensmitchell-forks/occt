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

#ifndef _STEPSelections_SelectForTransfer_HeaderFile
#define _STEPSelections_SelectForTransfer_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineHandle.hxx>
#include <XSControl_SelectForTransfer.hxx>
#include <XSControl_TransferReader.hxx>
class XSControl_TransferReader;
class Interface_EntityIterator;
class Interface_Graph;
class Standard_Transient;
class Handle(Standard_Type);
class Handle(XSControl_SelectForTransfer);
class STEPSelections_SelectForTransfer;

DEFINE_STANDARD_HANDLE(STEPSelections_SelectForTransfer,XSControl_SelectForTransfer)

class STEPSelections_SelectForTransfer : public XSControl_SelectForTransfer {

public:

  
  Standard_EXPORT   STEPSelections_SelectForTransfer();
  
  Standard_EXPORT   STEPSelections_SelectForTransfer(const Handle(XSControl_TransferReader)& TR);
  
  Standard_EXPORT   virtual  Interface_EntityIterator RootResult(const Interface_Graph& G) const;




  DEFINE_STANDARD_RTTI(STEPSelections_SelectForTransfer)
};
#endif
