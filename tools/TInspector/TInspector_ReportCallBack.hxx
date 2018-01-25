// Created on: 2017-12-01
// Created by: Natalia Ermolaeva
// Copyright (c) 2017 OPEN CASCADE SAS
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

#ifndef _TInspector_ReportCallBack_HeaderFile
#define _TInspector_ReportCallBack_HeaderFile

#include <Message_ReportCallBack.hxx>

#include "TInspector_Window.hxx"

class TInspector_Window;
DEFINE_STANDARD_HANDLE(TInspector_ReportCallBack, Message_ReportCallBack)

//! Class to update tree view model by message report events.
class TInspector_ReportCallBack : public Message_ReportCallBack
{
public:

  //! Constructor
  TInspector_ReportCallBack (TInspector_Window* theWindow) : Message_ReportCallBack(), myWindow (theWindow) {}

  //! Destructor
  ~TInspector_ReportCallBack() {}

  // Interface to be implemented in the child to process the message report event
  virtual void Update() { myWindow->UpdateContent(); }

  // OCCT RTTI
  DEFINE_STANDARD_RTTIEXT(TInspector_ReportCallBack, Message_ReportCallBack)

private:
  TInspector_Window* myWindow; //< window
};

#endif // _TInspector_ReportCallBack_HeaderFile
