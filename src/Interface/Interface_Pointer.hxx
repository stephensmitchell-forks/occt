// Created: 2016-09-08 
// Created by: Sergey ZERCHANINOV
// Copyright (c) 2001-2016 OPEN CASCADE SAS 
// 
// This file is part of commercial software by OPEN CASCADE SAS. 
// 
// This software is furnished in accordance with the terms and conditions 
// of the contract and with the inclusion of this copyright notice. 
// This software or any other copy thereof may not be provided or otherwise 
// be made available to any third party. 
// No ownership title to the software is transferred hereby. 
// 
// OPEN CASCADE SAS makes no representation or warranties with respect to the 
// performance of this software, and specifically disclaims any responsibility 
// for any damages, special or consequential, connected with its use. 

#ifndef _Interface_Pointer_HeaderFile
#define _Interface_Pointer_HeaderFile

#include <Standard_Type.hxx>

//! Root class for pointer fields
class Interface_PointerBase
{
 public:

  //! Default constructor
  Interface_PointerBase() {}

  //! Sets the entity of this pointer
  Standard_EXPORT virtual void SetEntity(const Handle(Standard_Transient) &) = 0;

  //! Gets the entity of this pointer
  Standard_EXPORT virtual const Handle(Standard_Transient) & GetEntity() const = 0;
};

//! Template class for pointer fields
template <class TheItemType>
class Interface_Pointer : public Interface_PointerBase, public Handle(TheItemType)
{
 public:

  DEFINE_STANDARD_ALLOC

  //! Default constructor
  Interface_Pointer() {}

  //! Constructor from handle
  Interface_Pointer(const Handle(TheItemType)& theHandle) : Handle(TheItemType)(theHandle) {}

  //! Constructor from pointer
  Interface_Pointer(TheItemType* thePointer) : Handle(TheItemType)(thePointer) {}

  //! Sets the entity of this pointer
  Standard_EXPORT virtual void SetEntity(const Handle(Standard_Transient) &) Standard_OVERRIDE;

  //! Gets the entity of this pointer
  Standard_EXPORT virtual const Handle(Standard_Transient) & GetEntity() const Standard_OVERRIDE;
};

#define IMPLEMENT_INTERFACE_POINTER(C1) \
void Interface_Pointer<C1>::SetEntity(const Handle(Standard_Transient) &theEntity) { *this = Handle(C1)::DownCast(theEntity); } \
const Handle(Standard_Transient) & Interface_Pointer<C1>::GetEntity() const { return *this; }

#endif
