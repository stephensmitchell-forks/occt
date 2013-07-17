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

#ifndef _IGESToBRep_Actor_HeaderFile
#define _IGESToBRep_Actor_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineHandle.hxx>
#include <Handle_Interface_InterfaceModel.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Real.hxx>
#include <Transfer_ActorOfTransientProcess.hxx>
#include <Standard_Boolean.hxx>
#include <Handle_Standard_Transient.hxx>
#include <Handle_Transfer_Binder.hxx>
#include <Transfer_TransientProcess.hxx>

class Interface_InterfaceModel;
class Standard_Transient;
class Transfer_Binder;
class Transfer_TransientProcess;
class Standard_Transient;
class Handle(Standard_Type);
class Handle(Transfer_ActorOfTransientProcess);
class IGESToBRep_Actor;

DEFINE_STANDARD_HANDLE(IGESToBRep_Actor,Transfer_ActorOfTransientProcess)

//! This class performs the transfer of an Entity from <br>
//!           IGESToBRep <br>
//! <br>
//!           I.E. for each type of Entity, it invokes the appropriate Tool <br>
//!           then returns the Binder which contains the Result <br>
class IGESToBRep_Actor : public Transfer_ActorOfTransientProcess {

public:

  Standard_EXPORT   IGESToBRep_Actor();
  
  Standard_EXPORT     void SetModel(const Handle(Interface_InterfaceModel)& model) ;
  //!---Purpose   By default continuity = 0 <br>
  //!           if continuity = 1 : try C1 <br>
  //!           if continuity = 2 : try C2 <br>
  Standard_EXPORT     void SetContinuity(const Standard_Integer continuity = 0);
  
  //! Return "thecontinuity" <br>
  Standard_EXPORT     Standard_Integer GetContinuity() const;
  
  Standard_EXPORT   virtual  Standard_Boolean Recognize(const Handle(Standard_Transient)& start);
  
  Standard_EXPORT   virtual  Handle_Transfer_Binder Transfer(const Handle(Standard_Transient)& start,
                                                             const Handle(Transfer_TransientProcess)& TP);
                                                             
  //! Returns the tolerance which was actually used, either from <br>
  //!           the file or from statics <br>
  Standard_EXPORT     Standard_Real UsedTolerance() const;

  DEFINE_STANDARD_RTTI(IGESToBRep_Actor)

private: 


Handle_Interface_InterfaceModel themodel;
Standard_Integer thecontinuity;
Standard_Real theeps;


};
#endif
