// Created on: 1993-06-10
// Created by: Christian CAILLET
// Copyright (c) 1993-1999 Matra Datavision
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

#ifndef _Transfer_SimpleBinderOfTransient_HeaderFile
#define _Transfer_SimpleBinderOfTransient_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Transfer_Binder.hxx>


class Transfer_SimpleBinderOfTransient;
DEFINE_STANDARD_HANDLE(Transfer_SimpleBinderOfTransient, Transfer_Binder)

//! An adapted instantiation of SimpleBinder for Transient Result,
//! i.e. ResultType can be computed from the Result itself,
//! instead of being static
class Transfer_SimpleBinderOfTransient : public Transfer_Binder
{
 public:
  
  //! Default constructor
  Transfer_SimpleBinderOfTransient() {}
  
  //! Constructor from a result
  Transfer_SimpleBinderOfTransient(const Handle(Standard_Transient) &theResult) : myResult(theResult) { SetResultPresent(); }
  
  //! Returns the Effective (Dynamic) Type of the Result
  //! (Standard_Transient if no Result is defined)
  Standard_EXPORT Handle(Standard_Type) ResultType() const Standard_OVERRIDE;
  
  //! Returns the Effective Name of (Dynamic) Type of the Result
  //! (void) if no result is defined
  Standard_EXPORT Standard_CString ResultTypeName() const Standard_OVERRIDE;
  
  //! Defines the Result
  void SetResult (const Handle(Standard_Transient)& theResult) { SetResultPresent(); myResult = theResult; }

  //! Returns the defined Result, if there is one
  const Handle(Standard_Transient) & Result() const { return myResult; }
  
  //! Returns a transient result according to its type (IsKind)
  //! i.e. the result itself if IsKind(atype), else searches in
  //! NextResult, until first found, then returns True
  //! If not found, returns False (res is NOT touched)
  //!
  //! This syntactic form avoids to do DownCast : if a result is
  //! found with the good type, it is loaded in <res> and can be
  //! immediately used, well initialised
  Standard_EXPORT static Standard_Boolean GetTypedResult (const Handle(Transfer_Binder)& bnd, const Handle(Standard_Type)& atype, Handle(Standard_Transient)& res);
  
  //! Prepares and Returns a Binder for a Transient Result
  //! Returns a Null Handle if <res> is itself Null
  static Handle(Transfer_SimpleBinderOfTransient) TransientResult (const Handle(Standard_Transient)& res)
  { return (res.IsNull()? NULL : new Transfer_SimpleBinderOfTransient(res)); }

  DEFINE_STANDARD_RTTIEXT(Transfer_SimpleBinderOfTransient,Transfer_Binder)

 private:

  Handle(Standard_Transient) myResult;
};

#endif // _Transfer_SimpleBinderOfTransient_HeaderFile
