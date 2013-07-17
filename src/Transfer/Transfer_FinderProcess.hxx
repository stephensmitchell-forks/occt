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

#ifndef _Transfer_FinderProcess_HeaderFile
#define _Transfer_FinderProcess_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineHandle.hxx>
#include <Standard_Integer.hxx>
#include <Standard_CString.hxx>
#include <Transfer_TransferProcess.hxx>
#include <Transfer_TransientMapper.hxx>
#include <Handle_Standard_Transient.hxx>
#include <Handle_Transfer_Finder.hxx>
#include <Handle_Message_Messenger.hxx>
#include <Transfer_FindHasher.hxx>
#include <Transfer_ActorOfFinderProcess.hxx>
#include <Transfer_HSequenceOfFinder.hxx>
#include <Transfer_Binder.hxx>
#include <NCollection_IndexedDataMap.hxx>
#include <Transfer_IteratorOfProcessForFinder.hxx>
#include <MMgt_TShared.hxx>
#include <Transfer_FinderProcess_Handle.hxx>

class Interface_InterfaceModel;
class Transfer_TransientMapper;
class Standard_Transient;
class Transfer_Finder;
class Message_Messenger;
class Standard_Transient;
class Handle(Standard_Type);
class Handle(MMgt_TShared);
class Transfer_FinderProcess;

class Transfer_FinderProcess 
: public Transfer_TransferProcess < Handle(Transfer_ActorOfFinderProcess),
                                   Transfer_TransferMapOfProcessForFinder,
                                   Handle(Transfer_Finder),
                                   Transfer_FindHasher,
                                   Handle(Transfer_HSequenceOfFinder),
                                   Transfer_IteratorOfProcessForFinder >
{

public:

  //! Sets FinderProcess at initial state, with an initial size <br>
  Standard_EXPORT   Transfer_FinderProcess(const Standard_Integer theNb = 10000);
  //! Sets an InterfaceModel, which can be used during transfer <br>
  //!           for instance if a context must be managed, it is in the Model <br>
  Standard_EXPORT     void SetModel(const Handle(Interface_InterfaceModel)& theModel) ;
  //! Returns the Model which can be used for context <br>
  Standard_EXPORT     Handle_Interface_InterfaceModel Model() const;
  //! In the list of mapped items (between 1 and NbMapped), <br>
  //!           searches for the first mapped item which follows <num0> <br>
  //!           (not included) and which has an attribute named <name> <br>
  //!           The considered Attributes are those brought by Finders,i.e. <br>
  //!           by Input data. <br>
  //!           While NextItemWithAttribute works on Result data (Binders) <br>
  //! <br>
  //!           Hence, allows such an iteration <br>
  //! <br>
  //!           for (num = FP->NextMappedWithAttribute(name,0); <br>
  //!                num > 0; <br>
  //!                num = FP->NextMappedWithAttribute(name,num) { <br>
  //!                .. process mapped item <num> <br>
  //!           } <br>
  Standard_EXPORT     Standard_Integer NextMappedWithAttribute (const Standard_CString theName,
                                                                const Standard_Integer theNum0) const;
  //! Returns a TransientMapper for a given Transient Object <br>
  //!           Either <obj> is already mapped, then its Mapper is returned <br>
  //!           Or it is not, then a new one is created then returned, BUT <br>
  //!             it is not mapped here (use Bind or FindElseBind to do this) <br>
  Standard_EXPORT     Handle_Transfer_TransientMapper TransientMapper (const Handle(Standard_Transient)& theObj) const;
  //! Specific printing to trace a Finder (by its method ValueType) <br>
  Standard_EXPORT   virtual  void PrintTrace (const Handle(Transfer_Finder)& theStart,
                                              const Handle(Message_Messenger)& theMessenger) const;
  //! Prints statistics on a given output, according mode <br>
  Standard_EXPORT     void PrintStats(const Standard_Integer mode,const Handle(Message_Messenger)& theMessenger) const;

  Standard_EXPORT Handle(Transfer_Binder) TransferProduct (const Handle(Transfer_Finder)& theStart);

   Standard_EXPORT Handle(Transfer_Binder) Transferring (const Handle(Transfer_Finder)& theStart);

   Standard_EXPORT Standard_Boolean Transfer(const Handle(Transfer_Finder)& theStart);

  DEFINE_STANDARD_RTTI(Transfer_FinderProcess)

private: 

  Handle_Interface_InterfaceModel myModel;

};
#endif
