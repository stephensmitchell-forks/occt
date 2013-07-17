// Copyright (c) 1999-2012 OPEN CASCADE SAS
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

#include <XSControl_SelectForTransfer.hxx>
#include <Standard_Type.hxx>
#include <XSControl_TransferReader.hxx>
#include <Transfer_ActorOfTransientProcess.hxx>
#include <Standard_Transient.hxx>
#include <Interface_InterfaceModel.hxx>
#include <TCollection_AsciiString.hxx>
#include <XSControl_SelectForTransfer.hxx>

IMPLEMENT_STANDARD_TYPE(XSControl_SelectForTransfer)
IMPLEMENT_STANDARD_SUPERTYPE_ARRAY()
  STANDARD_TYPE(IFSelect_SelectExtract),
  STANDARD_TYPE(IFSelect_SelectDeduct),
  STANDARD_TYPE(IFSelect_Selection),
  STANDARD_TYPE(MMgt_TShared),
  STANDARD_TYPE(Standard_Transient),

IMPLEMENT_STANDARD_SUPERTYPE_ARRAY_END()
IMPLEMENT_STANDARD_TYPE_END(XSControl_SelectForTransfer)


IMPLEMENT_DOWNCAST(XSControl_SelectForTransfer,Standard_Transient)
IMPLEMENT_STANDARD_RTTI(XSControl_SelectForTransfer)

XSControl_SelectForTransfer::XSControl_SelectForTransfer ()    {  }

    XSControl_SelectForTransfer::XSControl_SelectForTransfer
  (const Handle(XSControl_TransferReader)& TR)
      {  theTR = TR;  }


    void  XSControl_SelectForTransfer::SetReader
  (const Handle(XSControl_TransferReader)& TR)
      {  theTR = TR;  }

    void  XSControl_SelectForTransfer::SetActor
  (const Handle(Transfer_ActorOfTransientProcess)& act)
      {  theAC = act;  }

    Handle(Transfer_ActorOfTransientProcess)  XSControl_SelectForTransfer::Actor () const
      {  return theAC;  }

    Handle(XSControl_TransferReader)  XSControl_SelectForTransfer::Reader () const
      {  return theTR;  }

    Standard_Boolean  XSControl_SelectForTransfer::Sort
  (const Standard_Integer /*rank*/, const Handle(Standard_Transient)& ent,
   const Handle(Interface_InterfaceModel)& /*model*/) const
{
  Handle(Transfer_ActorOfTransientProcess) act = theAC;
  if (act.IsNull() && !theTR.IsNull()) act = theTR->Actor();
  if (!act.IsNull()) return act->Recognize(ent);//,theTR->TransientProcess());//act->Recognize(ent);
  return Standard_False;
}

 
    TCollection_AsciiString  XSControl_SelectForTransfer::ExtractLabel () const
{
  if (!theTR.IsNull()) return TCollection_AsciiString
    ("Recognized for Transfer (current actor)");
  return TCollection_AsciiString("Recognized for Transfer");
}
