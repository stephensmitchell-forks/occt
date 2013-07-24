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

#ifndef _Transfer_ActorOfFinderProcess_HeaderFile
#define _Transfer_ActorOfFinderProcess_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineHandle.hxx>
#include <Standard_Integer.hxx>
#include <Transfer_Binder.hxx>
#include <Handle_Transfer_Binder.hxx>
#include <Handle_Transfer_Finder.hxx>
#include <Transfer_FinderProcess_Handle.hxx>
#include <Handle_Standard_Transient.hxx>
#include <Transfer_Actor.hxx>

DEFINE_STANDARD_HANDLE(Transfer_ActorOfFinderProcess, MMgt_TShared)

class Transfer_ActorOfFinderProcess : public Transfer_Actor < Handle(Transfer_ActorOfFinderProcess),
                                                            Handle(Transfer_Finder),
                                                            Handle(Transfer_FinderProcess) >
{
public:
  Standard_EXPORT   Transfer_ActorOfFinderProcess();
  //! Returns the Transfer Mode, modifiable <br>
  Standard_EXPORT     Standard_Integer& ModeTrans() ;
  
  Standard_EXPORT   virtual  Handle(Transfer_Binder) Transferring(const Handle(Transfer_Finder)& theFinder,
                                                                  const Handle(Transfer_FinderProcess)& theFP);
  
  Standard_EXPORT   virtual  Handle(Transfer_Binder) Transfer(const Handle(Transfer_Finder)& theFinder,
                                                              const Handle(Transfer_FinderProcess)& theFP);
  
  Standard_EXPORT   virtual  Handle(Standard_Transient) TransferTransient(const Handle(Standard_Transient)& theEnt,
                                                                          const Handle(Transfer_FinderProcess)& theFP);

  DEFINE_STANDARD_RTTI(Transfer_ActorOfFinderProcess)

protected:
  Standard_Integer myModeTrans;
};
#endif