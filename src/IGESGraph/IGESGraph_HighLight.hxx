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

#ifndef _IGESGraph_HighLight_HeaderFile
#define _IGESGraph_HighLight_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Integer.hxx>
#include <IGESData_IGESEntity.hxx>
#include <Standard_Boolean.hxx>


class IGESGraph_HighLight;
DEFINE_STANDARD_HANDLE(IGESGraph_HighLight, IGESData_IGESEntity)

//! defines IGESHighLight, Type <406> Form <20>
//! in package IGESGraph
//!
//! Attaches information that an entity is to be
//! displayed in some system dependent manner
class IGESGraph_HighLight : public IGESData_IGESEntity
{
 public:

  Standard_EXPORT virtual Standard_Integer TypeNumber() const Standard_OVERRIDE { return 406; }

  Standard_EXPORT virtual Standard_Integer FormNumber() const Standard_OVERRIDE { return 20; }

  IGESGraph_HighLight()
  : myHighLight(0)
  {}

  //! returns 0 if <me> is not highlighted(default),
  //! 1 if <me> is highlighted
  Standard_Integer HighLightStatus() const { return myHighLight; }

  //! returns True if entity is highlighted
  Standard_Boolean IsHighLighted() const { return (myHighLight != 0); }

  Standard_EXPORT virtual void OwnRead (IGESFile_Reader &) Standard_OVERRIDE;
  
  Standard_EXPORT virtual void OwnWrite (IGESData_IGESWriter &) const Standard_OVERRIDE;

  Standard_EXPORT virtual IGESData_DirChecker DirChecker () const Standard_OVERRIDE;

  Standard_EXPORT virtual void OwnDump (const IGESData_IGESDumper &, const Handle(Message_Messenger) &, const Standard_Integer) const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(IGESGraph_HighLight,IGESData_IGESEntity)

 private:

  Standard_Integer myHighLight;
};

#endif // _IGESGraph_HighLight_HeaderFile
