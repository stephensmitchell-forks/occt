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

#ifndef _Transfer_ActorOfTransientProcess_HeaderFile
#define _Transfer_ActorOfTransientProcess_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineHandle.hxx>
#include <Handle_Transfer_Binder.hxx>
#include <Standard_Transient.hxx>
#include <Transfer_TransientProcess_Handle.hxx>
#include <Transfer_Actor.hxx>

DEFINE_STANDARD_HANDLE(Transfer_ActorOfTransientProcess, MMgt_TShared)

class Transfer_ActorOfTransientProcess : public Transfer_Actor < Handle(Transfer_ActorOfTransientProcess),
                                                                  Handle(Standard_Transient),
                                                                  Handle(Transfer_TransientProcess) >
{
public:
  
  Standard_EXPORT   Transfer_ActorOfTransientProcess();
  
  Standard_EXPORT   virtual  Handle_Transfer_Binder Transferring(const Handle(Standard_Transient)& start,const Handle(Transfer_TransientProcess)& TP) ;
  
  Standard_EXPORT   virtual  Handle_Transfer_Binder Transfer(const Handle(Standard_Transient)& start,const Handle(Transfer_TransientProcess)& TP) ;
  
  Standard_EXPORT   virtual  Handle_Standard_Transient TransferTransient(const Handle(Standard_Transient)& start,const Handle(Transfer_TransientProcess)& TP) ;


  DEFINE_STANDARD_RTTI(Transfer_ActorOfTransientProcess)

};

#endif