// Created on: 1993-02-02
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

#ifndef _Interface_GeneralModule_HeaderFile
#define _Interface_GeneralModule_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <MMgt_TShared.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>
class Interface_InterfaceModel;
class Standard_Transient;
class Interface_EntityIterator;
class Interface_ShareTool;
class Interface_Check;
class TCollection_HAsciiString;


class Interface_GeneralModule;
DEFINE_STANDARD_HANDLE(Interface_GeneralModule, MMgt_TShared)

//! This class defines general services, which must be provided
//! for each type of Entity (i.e. of Transient Object processed
//! by an Interface) : Shared List, Check, Copy, Delete, Category
//!
//! To optimise processing (e.g. firstly bind an Entity to a Module
//! then calls  Module), each recognized Entity Type corresponds
//! to a Case Number, determined by the Protocol each class of
//! GeneralModule belongs to.
class Interface_GeneralModule : public MMgt_TShared
{
 public:

  //! Specific filling of the list of Entities shared by an Entity
  //! <ent>, according a Case Number <CN> (formerly computed by
  //! CaseNum), considered in the context of a Model <model>
  //! Default calls FillSharedCase (i.e., ignores the model)
  //! Can be redefined to use the model for working
  Standard_EXPORT virtual void FillShared (const Handle(Interface_InterfaceModel)& model, const Standard_Integer CN, const Handle(Standard_Transient)& ent, Interface_EntityIterator& iter) const;
  
  //! Specific filling of the list of Entities shared by an Entity
  //! <ent>, according a Case Number <CN> (formerly computed by
  //! CaseNum). Can use the internal utility method Share, below
  Standard_EXPORT virtual void FillSharedCase (const Standard_Integer CN, const Handle(Standard_Transient)& ent, Interface_EntityIterator& iter) const = 0;
  
  //! Adds an Entity to a Shared List (uses GetOneItem on <iter>)
  Standard_EXPORT void Share (Interface_EntityIterator& iter, const Handle(Standard_Transient)& shared) const;
  
  //! List the Implied References of <ent> considered in the context
  //! of a Model <model> : i.e. the Entities which are Referenced
  //! while not considered as Shared (not copied if <ent> is,
  //! references not renewed by CopyCase but by ImpliedCase, only
  //! if referenced Entities have been Copied too)
  //! FillShared + ListImplied give the complete list of References
  //! Default calls ListImpliedCase (i.e. ignores the model)
  //! Can be redefined to use the model for working
  Standard_EXPORT virtual void ListImplied (const Handle(Interface_InterfaceModel)& model, const Standard_Integer CN, const Handle(Standard_Transient)& ent, Interface_EntityIterator& iter) const;
  
  //! List the Implied References of <ent> (see above)
  //! are Referenced while not considered as Shared (not copied if
  //! <ent> is, references not renewed by CopyCase but by
  //! ImpliedCase, only if referenced Entities have been Copied too)
  //! FillSharedCase + ListImpliedCase give the complete list of
  //! Referenced Entities
  //! The provided default method does nothing (Implied References
  //! are specific of a little amount of Entity Classes).
  Standard_EXPORT virtual void ListImpliedCase (const Standard_Integer CN, const Handle(Standard_Transient)& ent, Interface_EntityIterator& iter) const;
  
  //! Specific Checking of an Entity <ent>
  //! Can check context queried through a ShareTool, as required
  Standard_EXPORT virtual void CheckCase (const Standard_Integer CN, const Handle(Standard_Transient)& ent, const Interface_ShareTool& shares, Handle(Interface_Check)& ach) const = 0;
  
  //! Creates a new void entity <entto> according to a Case Number
  //! This entity remains to be filled, by reading from a file or
  //! by copying from another entity of same type (see CopyCase)
  Standard_EXPORT virtual Standard_Boolean NewVoid (const Standard_Integer CN, Handle(Standard_Transient)& entto) const = 0;
 
  //! Returns a category number which characterizes an entity
  //! Category Numbers are managed by the class Category
  //! <shares> can be used to evaluate this number in the context
  //! Default returns 0 which means "unspecified"
  Standard_EXPORT virtual Standard_Integer CategoryNumber (const Standard_Integer CN, const Handle(Standard_Transient)& ent, const Interface_ShareTool& shares) const;
  
  //! Determines if an entity brings a Name (or widerly, if a Name
  //! can be attached to it, through the ShareTool
  //! By default, returns a Null Handle (no name can be produced)
  //! Can be redefined
  //!
  //! Warning : While this string may be edited on the spot, if it is a read
  //! field, the returned value must be copied before.
  Standard_EXPORT virtual Handle(TCollection_HAsciiString) Name (const Standard_Integer CN, const Handle(Standard_Transient)& ent, const Interface_ShareTool& shares) const;

  DEFINE_STANDARD_RTTIEXT(Interface_GeneralModule,MMgt_TShared)
};

#endif // _Interface_GeneralModule_HeaderFile
