// Created on: 1992-10-28
// Created by: Christian CAILLET
// Copyright (c) 1992-1999 Matra Datavision
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

#ifndef _Transfer_Iterator_HeaderFile
#define _Transfer_Iterator_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

class Transfer_HSequenceOfBinder;
class TColStd_HSequenceOfInteger;

//! Defines an Iterator on the result of a Transfer
//! Available for Normal Results or not (Erroneous Transfer)
//! It gives several kinds of Informations, and allows to consider
//! various criteria (criteria are cumulative)
class Transfer_Iterator 
{
public:

  DEFINE_STANDARD_ALLOC
  
  //! Creates an empty Iterator
  Standard_EXPORT Transfer_Iterator();
  
  //! Returns count of Binders to be iterated
  Standard_EXPORT Standard_Integer Number() const;
  
  //! Clears Iteration in progress, to allow it to be restarted
  void Start() { thecurr = 0; Next(); }
  
  //! Returns True if there are other Items to iterate
  Standard_EXPORT Standard_Boolean More();
  
  //! Sets Iteration to the next Item
  Standard_EXPORT void Next();
  
  //! Returns the current Binder
  Standard_EXPORT const Handle(Transfer_Binder)& Value() const;

 protected:
  
  //! Adds a Binder to the iteration list (construction)
  Standard_EXPORT void AddItem (const Handle(Transfer_Binder)& atr);
  
  //! Selects/Unselect (according to <keep> an item designated by
  //! its rank <num> in the list
  //! Used by sub-classes which have specific criteria
  Standard_EXPORT void SelectItem (const Standard_Integer num, const Standard_Boolean keep);

  Standard_Integer thecurr;

 private:

  Handle(Transfer_HSequenceOfBinder) theitems;
  Handle(TColStd_HSequenceOfInteger) theselect;
};

#endif // _Transfer_Iterator_HeaderFile
