// Created on: 2016-10-17
// Created by: Sergey ZERCHANINOV
// Copyright (c) 2016 OPEN CASCADE SAS
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

#ifndef _XSDRAW_SessionItems_HeaderFile
#define _XSDRAW_SessionItems_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Dico_DictionaryOfTransient.hxx>
#include <Dico_IteratorOfDictionaryOfTransient.hxx>

class XSDRAW_SessionItems
{
 public:

  DEFINE_STANDARD_ALLOC

  XSDRAW_SessionItems() {}

  //! Records a Session Item, to be added for customisation of the Work Session.
  //! It must have a specific name.
  //! <setapplied> is used if <item> is a GeneralModifier, to decide
  //! If set to true, <item> will be applied to the hook list "send".
  //! Else, it is not applied to any hook list.
  //! Warning : if <name> conflicts, the last recorded item is kept
  void AddItem (const Handle(Standard_Transient)& theItem, const Standard_CString theName)
  {
    if (theItem.IsNull() || theName[0] == '\0') return;
    if (myAdaptorSession.IsNull())
      myAdaptorSession = new Dico_DictionaryOfTransient;
    myAdaptorSession->SetItem (theName,theItem);
  }

  //! Returns an item given its name to record in a Session
  //! If <name> is unknown, returns a Null Handle
  Handle(Standard_Transient) GetItem (const Standard_CString theName) const
  {
    Handle(Standard_Transient) item;
    if (!myAdaptorSession.IsNull())
      myAdaptorSession->GetItem (theName,item);
    return item;
  }

  //! Apply recorded items
  void Apply (const Handle(IFSelect_WorkSession) &theWS) const
  {
    // General
    if (!myAdaptorSession.IsNull()) {
      Dico_IteratorOfDictionaryOfTransient iter(myAdaptorSession);
      for (iter.Start(); iter.More(); iter.Next())
        theWS->AddNamedItem (iter.Name().ToCString(), iter.Value());
    }
  }

 protected:

  Handle(Dico_DictionaryOfTransient) myAdaptorSession;
};

#endif // _XSDRAW_SessionItems_HeaderFile
