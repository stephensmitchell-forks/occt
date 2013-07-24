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

#ifndef _Transfer_IteratorOfProcessForTransient_HeaderFile
#define _Transfer_IteratorOfProcessForTransient_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Macro.hxx>
#include <Handle_TColStd_HSequenceOfTransient.hxx>
#include <Transfer_Iterator.hxx>
#include <Transfer_TransferIterator.hxx>
#include <Handle_Standard_Transient.hxx>
#include <Transfer_ActorOfTransientProcess.hxx>
#include <Standard_Boolean.hxx>
#include <Handle_Transfer_Binder.hxx>
#include <TColStd_HSequenceOfTransient.hxx>
#include <Standard_NoSuchObject.hxx>
#include <Standard_Transient.hxx>
#include <TColStd_MapTransientHasher.hxx>
#include <Transfer_Binder.hxx>
#include <NCollection_IndexedDataMap.hxx>

class TColStd_HSequenceOfTransient;
class Standard_NoSuchObject;
class Standard_Transient;
class TColStd_MapTransientHasher;
class Transfer_TransientProcess;
class Transfer_ActorOfTransientProcess;
class Transfer_Binder;

typedef NCollection_IndexedDataMap <Handle(Standard_Transient),
                                    Handle(Transfer_Binder),
                                    TColStd_MapTransientHasher>
        Transfer_TransferMapOfProcessForTransient;


class Transfer_IteratorOfProcessForTransient  : public Transfer_Iterator <Handle(Standard_Transient),
                                                                          TColStd_HSequenceOfTransient,
                                                                          Handle(TColStd_HSequenceOfTransient),
                                                                          Transfer_TransferMapOfProcessForTransient>
{
public:
  Standard_EXPORT   Transfer_IteratorOfProcessForTransient(const Standard_Boolean theWithStarts)
  : Transfer_Iterator <Handle(Standard_Transient),
                      TColStd_HSequenceOfTransient,
                      Handle(TColStd_HSequenceOfTransient),
                      Transfer_TransferMapOfProcessForTransient> (theWithStarts)
  { }

};
#endif
