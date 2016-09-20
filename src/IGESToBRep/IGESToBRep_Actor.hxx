// Created on: 1994-11-03
// Created by: Marie Jose MARTZ
// Copyright (c) 1994-1999 Matra Datavision
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

#ifndef _IGESToBRep_Actor_HeaderFile
#define _IGESToBRep_Actor_HeaderFile

#include <Transfer_ActorOfTransientProcess.hxx>

class IGESToBRep_Actor;
DEFINE_STANDARD_HANDLE(IGESToBRep_Actor, Transfer_ActorOfTransientProcess)

//! This class performs the transfer of an Entity from
//! IGESToBRep
//!
//! I.E. for each type of Entity, it invokes the appropriate Tool
//! then returns the Binder which contains the Result
class IGESToBRep_Actor : public Transfer_ActorOfTransientProcess
{
 public:

  IGESToBRep_Actor() : thecontinuity (0), theeps (0.0001) {}
  
  Standard_EXPORT virtual Standard_Boolean Recognize (const Handle(Standard_Transient)& start) Standard_OVERRIDE;
  
  Standard_EXPORT virtual Handle(Transfer_Binder) Transferring (const Handle(Standard_Transient)& start, const Handle(Transfer_ProcessForTransient)& TP) Standard_OVERRIDE;
  
  Standard_EXPORT void SetModel (const Handle(Interface_InterfaceModel)& model);
  
  //! ---Purpose   By default continuity = 0
  //! if continuity = 1 : try C1
  //! if continuity = 2 : try C2
  void SetContinuity (const Standard_Integer continuity = 0) { thecontinuity = continuity; }
  
  Standard_Integer GetContinuity() const { return thecontinuity; }
  
  //! Returns the tolerance which was actually used, either from
  //! the file or from statics
  Standard_Real UsedTolerance() const { return theeps; }

  DEFINE_STANDARD_RTTIEXT(IGESToBRep_Actor,Transfer_ActorOfTransientProcess)

 private:

  Handle(Interface_InterfaceModel) themodel;
  Standard_Integer thecontinuity;
  Standard_Real theeps;
};

#endif // _IGESToBRep_Actor_HeaderFile
