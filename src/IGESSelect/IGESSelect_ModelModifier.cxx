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

#include <IFSelect_ContextModif.hxx>
#include <Interface_Check.hxx>
#include <IGESSelect_ModelModifier.hxx>
#include <IGESData_IGESModel.hxx>

 IMPLEMENT_STANDARD_HANDLE(IGESSelect_ModelModifier, IFSelect_Modifier)
 IMPLEMENT_STANDARD_RTTIEXT(IGESSelect_ModelModifier, IFSelect_Modifier)
 
IGESSelect_ModelModifier::IGESSelect_ModelModifier (const Standard_Boolean mayChangeGraph)
    : IFSelect_Modifier (mayChangeGraph)
{}

void  IGESSelect_ModelModifier::Perform
               (IFSelect_ContextModif& theContext,
               const Handle(Interface_InterfaceModel)& theTarget,
               const Handle(Interface_Protocol)& theProtocol,
               Interface_CopyTool& theCopyTool) const
{
  theContext.TraceModifier(this);
  Handle(IGESData_IGESModel) aTarget = Handle(IGESData_IGESModel)::DownCast(theTarget);
  Handle(IGESData_Protocol) aProtocol
    = Handle(IGESData_Protocol)::DownCast(theProtocol);
  if (aTarget.IsNull())
  {
    theContext.CCheck()->AddFail("Model to Modify : unproper type");
    return;
  }
  PerformProtocol (theContext,aTarget,aProtocol,theCopyTool);
}

void  IGESSelect_ModelModifier::PerformProtocol
          (IFSelect_ContextModif& theContext,
           const Handle(IGESData_IGESModel)& theTarget,
           const Handle(IGESData_Protocol)& theProtocol,
           Interface_CopyTool& theCopyTool) const
{
  theContext.SetProtocol(theProtocol);
  Performing (theContext,theTarget,theCopyTool);
}
