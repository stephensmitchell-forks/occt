// Created on: 1998-09-07
// Created by: Christian CAILLET
// Copyright (c) 1998-1999 Matra Datavision
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

#ifndef _IGESControl_ActorWrite_HeaderFile
#define _IGESControl_ActorWrite_HeaderFile

#include <Transfer_ActorOfFinderProcess.hxx>

class IGESControl_ActorWrite;
DEFINE_STANDARD_HANDLE(IGESControl_ActorWrite, Transfer_ActorOfFinderProcess)

//! Actor to write Shape to IGES
class IGESControl_ActorWrite : public Transfer_ActorOfFinderProcess
{
 public:
  
  IGESControl_ActorWrite() {}
  
  //! Recognizes a source entity
  Standard_EXPORT virtual Standard_Boolean Recognize (const Handle(Standard_Transient)& theSource) Standard_OVERRIDE;
  
  //! Transfers a source entity to IGES Entities
  Standard_EXPORT virtual Handle(Transfer_Binder) Transferring (const Handle(Standard_Transient)& theSource, const Handle(Transfer_Process)& FP) Standard_OVERRIDE;
  
  //! Provides transfer mode bounds
  Standard_EXPORT virtual Standard_Boolean TransferModeBounds (Standard_Integer& theMin, Standard_Integer& theMax) const Standard_OVERRIDE;
  
  //! Returns the help string recorded for a given transfer mode
  Standard_EXPORT virtual Standard_CString TransferModeHelp (const Standard_Integer theMode) const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(IGESControl_ActorWrite,Transfer_ActorOfFinderProcess)
};

#endif // _IGESControl_ActorWrite_HeaderFile
