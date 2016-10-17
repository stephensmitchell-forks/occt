// Created on: 1996-09-04
// Created by: Christian CAILLET
// Copyright (c) 1996-1999 Matra Datavision
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

#ifndef _Transfer_FinderProcess_HeaderFile
#define _Transfer_FinderProcess_HeaderFile

#include <Transfer_Process.hxx>
#include <Interface_ReturnStatus.hxx>
class Message_Messenger;
class XSControl_WorkSession;

class Transfer_FinderProcess : public Transfer_Process
{
 public:

  //! Constructor, with initial size
  Transfer_FinderProcess(const Standard_Integer nb = 10000)
  : Transfer_Process (nb)
  {}

  //! Destructor
  virtual ~Transfer_FinderProcess ()
  { ClearBinders(); }
  
  //! Prints the transfer status of a transferred item, as beeing the Mapped n0 <num>
  //! Returns True when done, False else (i.e. num out of range)
  Standard_EXPORT virtual Standard_Boolean PrintTransferStatus (const Standard_Integer theNum, const Handle(Message_Messenger)& theMessenger) const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(Transfer_FinderProcess,Transfer_Process)

 protected:
  
  //! Clears binders
  Standard_EXPORT void ClearBinders();
};

#endif // _Transfer_FinderProcess_HeaderFile
