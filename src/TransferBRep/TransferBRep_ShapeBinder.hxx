// Created on: 1994-10-03
// Created by: Assim
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

#ifndef _TransferBRep_ShapeBinder_HeaderFile
#define _TransferBRep_ShapeBinder_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TopoDS_Shape.hxx>
#include <Transfer_Binder.hxx>

//! Allows direct binding between a starting Object and the Result of its transfer when it is Unique.
//! The Result itself is defined as a formal parameter <Shape from TopoDS>
//! Warning : While it is possible to instantiate ShapeBinder with any Type for the Result,
//! it is not advisable to instantiate it with Transient Classes,
//! because such Results are directly known and managed by TransferProcess & Co,
//! through SimpleBinderOfTransient : this class looks like instantiation of ShapeBinder,
//! but its method ResultType is adapted (reads DynamicType of the Result)
class TransferBRep_ShapeBinder : public Transfer_Binder
{
 public:
  
  //! Creates an empty ShapeBinder
  TransferBRep_ShapeBinder() {}

  //! Constructor which in the same time defines the result
  //! Returns True if a starting object is bound with SEVERAL
  //! results : Here, returns allways False
  //! But it can have next results
  TransferBRep_ShapeBinder(const TopoDS_Shape& theResult) : myResult (theResult) { SetResultPresent(); }
  
  //! Returns the Type permitted for the Result, i.e. the Type
  //! of the Parameter Class <Shape from TopoDS> (statically defined)
  Standard_EXPORT Handle(Standard_Type) ResultType() const Standard_OVERRIDE;
  
  //! Returns the Type Name computed for the Result (dynamic)
  Standard_EXPORT Standard_CString ResultTypeName() const Standard_OVERRIDE;
  
  //! Defines the Result
  void SetResult (const TopoDS_Shape &theResult) { SetResultPresent(); myResult = theResult; }
  
  //! Returns the defined Result, if there is one
  const TopoDS_Shape & Result() const { return myResult; }

  DEFINE_STANDARD_RTTIEXT(TransferBRep_ShapeBinder,Transfer_Binder)

 private:

  TopoDS_Shape myResult;
};

#endif // _TransferBRep_ShapeBinder_HeaderFile
