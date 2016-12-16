// Created on: 1993-01-09
// Created by: CKY / Contract Toubro-Larsen ( TCD )
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

#ifndef _IGESGraph_Pick_HeaderFile
#define _IGESGraph_Pick_HeaderFile

#include <IGESData_IGESEntity.hxx>


//! defines IGESPick, Type <406> Form <21>
//! in package IGESGraph
//!
//! Attaches information that an entity may be picked
//! by whatever pick device is used in the receiving
//! system
class IGESGraph_Pick : public IGESData_IGESEntity
{
 public:

  Standard_EXPORT virtual Standard_Integer TypeNumber() const Standard_OVERRIDE { return 406; }

  Standard_EXPORT virtual Standard_Integer FormNumber() const Standard_OVERRIDE { return 21; }

  IGESGraph_Pick()
  : myPick(0)
  {}

  //! returns 0 if <me> is pickable(default),
  //! 1 if <me> is not pickable.
  Standard_Integer PickFlag() const { return myPick; }

  //! returns True if thePick is 0.
  Standard_Boolean IsPickable() const { return (myPick == 0); }

  Standard_EXPORT virtual void OwnRead (IGESFile_Reader &) Standard_OVERRIDE;
  
  Standard_EXPORT virtual void OwnWrite (IGESData_IGESWriter &) const Standard_OVERRIDE;

  Standard_EXPORT virtual IGESData_DirChecker DirChecker () const Standard_OVERRIDE;

  Standard_EXPORT virtual void OwnDump (const IGESData_IGESDumper &, const Handle(Message_Messenger) &, const Standard_Integer) const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(IGESGraph_Pick,IGESData_IGESEntity)

 private:

  Standard_Integer myPick;
};

#endif // _IGESGraph_Pick_HeaderFile
