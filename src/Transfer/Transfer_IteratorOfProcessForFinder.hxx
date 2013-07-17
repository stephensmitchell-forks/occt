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

#ifndef _Transfer_IteratorOfProcessForFinder_HeaderFile
#define _Transfer_IteratorOfProcessForFinder_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Macro.hxx>
#include <Handle_Transfer_HSequenceOfFinder.hxx>
#include <Handle_Transfer_Finder.hxx>
#include <Transfer_FinderProcess_Handle.hxx>
#include <Transfer_ActorOfFinderProcess.hxx>
#include <Standard_Boolean.hxx>
#include <Handle_Transfer_Binder.hxx>
#include <Transfer_Iterator.hxx>
#include <Transfer_HSequenceOfFinder.hxx>
#include <Standard_NoSuchObject.hxx>
#include <Transfer_Finder.hxx>
#include <Transfer_FindHasher.hxx>
#include <Transfer_Binder.hxx>
#include <NCollection_IndexedDataMap.hxx>

class Transfer_HSequenceOfFinder;
class Standard_NoSuchObject;
class Transfer_Finder;
class Transfer_FindHasher;
class Transfer_FinderProcess;
class Transfer_ActorOfFinderProcess;
class Transfer_Binder;

typedef NCollection_IndexedDataMap < Handle(Transfer_Finder),
                                    Handle(Transfer_Binder),
                                    Transfer_FindHasher >
        Transfer_TransferMapOfProcessForFinder;

class Transfer_IteratorOfProcessForFinder  : public Transfer_Iterator <Handle(Transfer_Finder),
                                                                       Transfer_HSequenceOfFinder,
                                                                       Handle(Transfer_HSequenceOfFinder),
                                                                       Transfer_TransferMapOfProcessForFinder>
{
public:
  Standard_EXPORT   Transfer_IteratorOfProcessForFinder(const Standard_Boolean withstarts)
  : Transfer_Iterator <Handle(Transfer_Finder),
                       Transfer_HSequenceOfFinder,
                       Handle(Transfer_HSequenceOfFinder),
                       Transfer_TransferMapOfProcessForFinder>(withstarts)
  { }

};
#endif
