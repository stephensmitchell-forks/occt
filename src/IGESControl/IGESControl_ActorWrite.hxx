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

#ifndef _IGESControl_ActorWrite_HeaderFile
#define _IGESControl_ActorWrite_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineHandle.hxx>
#include <Transfer_ActorOfFinderProcess.hxx>
#include <Standard_Boolean.hxx>
#include <Handle_Transfer_Finder.hxx>
#include <Handle_Transfer_Binder.hxx>
#include <Transfer_FinderProcess.hxx>

class Transfer_Finder;
class Transfer_Binder;
class Transfer_FinderProcess;
class Standard_Transient;
class Handle(Standard_Type);
class Handle(Transfer_ActorOfFinderProcess);
class IGESControl_ActorWrite;

DEFINE_STANDARD_HANDLE(IGESControl_ActorWrite,Transfer_ActorOfFinderProcess)

//! Actor to write Shape to IGES <br>
class IGESControl_ActorWrite : public Transfer_ActorOfFinderProcess {

public:

  Standard_EXPORT   IGESControl_ActorWrite();
  
  //! Recognizes a ShapeMapper <br>
  Standard_EXPORT   virtual  Standard_Boolean Recognize(const Handle(Transfer_Finder)& start);
  
  //! Transfers Shape to IGES Entities <br>
  //! <br>
  //!           ModeTrans may be : 0 -> groups of Faces <br>
  //!           or 1 -> BRep <br>
  Standard_EXPORT   virtual  Handle_Transfer_Binder Transfer(const Handle(Transfer_Finder)& start,
                                                             const Handle(Transfer_FinderProcess)& FP);


  DEFINE_STANDARD_RTTI(IGESControl_ActorWrite)

};
#endif
