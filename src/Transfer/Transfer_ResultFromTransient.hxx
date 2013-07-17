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

#ifndef _Transfer_ResultFromTransient_HeaderFile
#define _Transfer_ResultFromTransient_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineHandle.hxx>
#include <Handle_Standard_Transient.hxx>
#include <Handle_Transfer_Binder.hxx>
#include <Handle_TColStd_HSequenceOfTransient.hxx>
#include <MMgt_TShared.hxx>
#include <Standard_Boolean.hxx>
#include <Handle_Interface_Check.hxx>
#include <Interface_CheckStatus.hxx>
#include <Standard_Integer.hxx>
#include <Transfer_TransientProcess.hxx>

class Standard_Transient;
class Transfer_Binder;
class TColStd_HSequenceOfTransient;
class Interface_Check;
class TColStd_IndexedMapOfTransient;
class Transfer_TransientProcess;
class Standard_Transient;
class Handle(Standard_Type);
class Handle(MMgt_TShared);
class Transfer_ResultFromTransient;

DEFINE_STANDARD_HANDLE(Transfer_ResultFromTransient,MMgt_TShared)

//! This class, in conjunction with ResultFromModel, allows to <br>
//!           record the result of a transfer initially stored in a <br>
//!           TransientProcess. <br>
//! <br>
//!           A ResultFromTransient records a couple (Transient,Binder for <br>
//!           the result and checks) plus a list of "sub-results", which <br>
//!           have been recorded in the TrabsientProcess, under scope <br>
//!           attached to the starting transient. <br>

class Transfer_ResultFromTransient : public MMgt_TShared
{

public:

  //! Creates a ResultFromTransient, empty <br>
  Standard_EXPORT   Transfer_ResultFromTransient();
  //! Sets starting entity <br>
  Standard_EXPORT     void SetStart(const Handle(Standard_Transient)& start) ;
  //! Sets Binder (for result plus individual check) <br>
  Standard_EXPORT     void SetBinder(const Handle(Transfer_Binder)& binder) ;
  //! Returns the starting entity <br>
  Standard_EXPORT     Handle_Standard_Transient Start() const;
  //! Returns the binder <br>
  Standard_EXPORT     Handle_Transfer_Binder Binder() const;
  //! Returns True if a result is recorded <br>
  Standard_EXPORT     Standard_Boolean HasResult() const;
  //! Returns the check (or an empty one if no binder) <br>
  Standard_EXPORT    const Handle_Interface_Check Check() const;
  //! Returns the check status <br>
  Standard_EXPORT     Interface_CheckStatus CheckStatus() const;
  //! Clears the list of (immediate) sub-results <br>
  Standard_EXPORT     void ClearSubs() ;
  //! Adds a sub-result <br>
  Standard_EXPORT     void AddSubResult(const Handle(Transfer_ResultFromTransient)& sub) ;
  //! Returns the count of recorded sub-results <br>
  Standard_EXPORT     Standard_Integer NbSubResults() const;
  //! Returns a sub-result, given its rank <br>
  Standard_EXPORT     Handle_Transfer_ResultFromTransient SubResult(const Standard_Integer num) const;
  //! Returns the ResultFromTransient attached to a given starting <br>
  //!           entity (the key). Returns a null handle if not found <br>
  Standard_EXPORT     Handle_Transfer_ResultFromTransient ResultFromKey(const Handle(Standard_Transient)& key) const;
  //! This method is used by ResultFromModel to collate the list of <br>
  //!           ResultFromTransient, avoiding duplications with a map <br>
  //!           Remark : <me> is already in the map and has not to be bound <br>
  Standard_EXPORT     void FillMap(TColStd_IndexedMapOfTransient& map) const;
  //! Fills from a TransientProcess, with the starting entity which <br>
  //!           must have been set before. It works with scopes, calls Fill <br>
  //!           on each of its sub-results <br>
  Standard_EXPORT     void Fill(const Handle(Transfer_TransientProcess)& TP) ;
  //! Clears some data attached to binders used by TransientProcess, <br>
  //!           which become useless once the transfer has been done : <br>
  //!           the list of sub-scoped binders, which is now recorded as <br>
  //!           sub-results <br>
  Standard_EXPORT     void Strip() ;
  //! Fills back a TransientProcess with definition of a <br>
  //!           ResultFromTransient, respectfully to its structuration in <br>
  //!           scopes <br>
  Standard_EXPORT     void FillBack(const Handle(Transfer_TransientProcess)& TP) const;

  DEFINE_STANDARD_RTTI(Transfer_ResultFromTransient)

private: 

  Handle_Standard_Transient thestart;
  Handle_Transfer_Binder thebinder;
  Handle_TColStd_HSequenceOfTransient thesubs;

};
#endif
