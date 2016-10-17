// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.


#include <Dico_DictionaryOfTransient.hxx>
#include <Dico_IteratorOfDictionaryOfTransient.hxx>
#include <Interface_InterfaceModel.hxx>
#include <Interface_HArray1OfHAsciiString.hxx>
#include <Interface_Macros.hxx>
#include <Interface_Protocol.hxx>
#include <Interface_Static.hxx>
#include <Message.hxx>
#include <Message_Messenger.hxx>
#include <Standard_DomainError.hxx>
#include <TCollection_HAsciiString.hxx>
#include <TColStd_HSequenceOfHAsciiString.hxx>
#include <TColStd_IndexedMapOfTransient.hxx>
#include <Transfer_FinderProcess.hxx>
#include <XSControl_Controller.hxx>
#include <XSControl_WorkSession.hxx>

IMPLEMENT_STANDARD_RTTIEXT(XSControl_Controller,MMgt_TShared)

namespace {
static class RecordedAdaptors
{
  Handle(Dico_DictionaryOfTransient) myDictionary;
 public:
  RecordedAdaptors() : myDictionary(new Dico_DictionaryOfTransient) {}
  void Record (const TCollection_AsciiString &theName, const Handle(XSControl_Controller) &theCtl) const
  {
    Standard_Boolean isAlreadyRegistered = Standard_False;
    Handle(Standard_Transient)& newadapt = myDictionary->NewItem(theName,isAlreadyRegistered);
    if (isAlreadyRegistered) {
      if (newadapt->IsKind(theCtl->DynamicType()))
        return;
      if (!(theCtl->IsKind(newadapt->DynamicType())) && theCtl != newadapt)
        Standard_DomainError::Raise("XSControl_Controller : Attempt to overwrite a more specific controller");
    }
    newadapt = theCtl;
  }
  Handle(XSControl_Controller) Recorded (const Standard_CString theName) const
  {
    Handle(Standard_Transient) recorded;
    return (myDictionary->GetItem(theName,recorded)?
      Handle(XSControl_Controller)::DownCast(recorded) :
      Handle(XSControl_Controller)());
  }
} gAdaptors;
}

//=======================================================================
//function : AutoRecord
//purpose  : 
//=======================================================================

void XSControl_Controller::AutoRecord () const
{
  gAdaptors.Record (myName,this);
}

//=======================================================================
//function : XSControl_Controller
//purpose  : Constructor
//=======================================================================

XSControl_Controller::XSControl_Controller (const Standard_CString theName)
: myName(theName),
  thelevdef(0)
{
  // Standard parameters
  Interface_Static::Standards();
  TraceStatic ("read.precision.mode" , 5);
  TraceStatic ("read.precision.val"  , 5);
  TraceStatic ("write.precision.mode" , 6);
  TraceStatic ("write.precision.val"  , 6);
}

//=======================================================================
//function : Recorded
//purpose  : 
//=======================================================================

Handle(XSControl_Controller) XSControl_Controller::Recorded (const Standard_CString theName)
{
  return gAdaptors.Recorded(theName);
}

//    ####    DEFINITION    ####

//=======================================================================
//function : ActorRead
//purpose  : 
//=======================================================================

Handle(Transfer_ActorOfTransientProcess) XSControl_Controller::ActorRead (const Handle(Interface_InterfaceModel)&) const
{
  return myAdaptorRead;
}

// ###########################
//  Cutomisation ! On enregistre des Items pour une WorkSession
//     (annule et remplace)
//     Ensuite, on les remet en place a la demande

//=======================================================================
//function : Customise
//purpose  : 
//=======================================================================

void XSControl_Controller::Customise (Handle(XSControl_WorkSession)&)
{
}

//=======================================================================
//function : DumpEntity
//purpose  : 
//=======================================================================

void XSControl_Controller::DumpEntity
  (const Handle(Interface_InterfaceModel)& model,
   const Handle(Interface_Protocol)& protocol,
   const Handle(Standard_Transient)& entity,
   const Handle(Message_Messenger)& S) const
{
  DumpEntity (model,protocol,entity,S,(thelevhlp.IsNull()? 0 : thelevdef));
}

//=======================================================================
//function : SetDumpLevels
//purpose  : 
//=======================================================================

void XSControl_Controller::SetDumpLevels
  (const Standard_Integer def, const Standard_Integer max)
{
  thelevdef = def;
  if (max >= 0) thelevhlp = new Interface_HArray1OfHAsciiString (0,max);
  else thelevhlp.Nullify();
}

//=======================================================================
//function : DumpLevels
//purpose  : 
//=======================================================================

void XSControl_Controller::DumpLevels
  (Standard_Integer& def, Standard_Integer& max) const
{
  def = thelevdef;
  if (thelevhlp.IsNull()) {  def = 0;  max = -1;  }
  else max = thelevhlp->Upper();
}

//=======================================================================
//function : SetDumpHelp
//purpose  : 
//=======================================================================

void XSControl_Controller::SetDumpHelp (const Standard_Integer level, const Standard_CString help)
{
  if (thelevhlp.IsNull()) return;
  if (level < 0 || level > thelevhlp->Upper()) return;
  Handle(TCollection_HAsciiString) str = new TCollection_HAsciiString (help);
  thelevhlp->SetValue (level,str);
}

//=======================================================================
//function : DumpHelp
//purpose  : 
//=======================================================================

Standard_CString XSControl_Controller::DumpHelp (const Standard_Integer level) const
{
  if (thelevhlp.IsNull()) return "";
  if (level < 0 || level > thelevhlp->Upper()) return "";
  Handle(TCollection_HAsciiString) str = thelevhlp->Value (level);
  if (str.IsNull()) return "";
  return str->ToCString();
}

//=======================================================================
//function : TraceStatic
//purpose  : 
//=======================================================================

void XSControl_Controller::TraceStatic (const Standard_CString theName, const Standard_Integer theUse)
{
  Handle(Interface_Static) val = Interface_Static::Static(theName);
  if (val.IsNull()) return;
  myParams.Append (val);
  myParamUses.Append(theUse);
}
