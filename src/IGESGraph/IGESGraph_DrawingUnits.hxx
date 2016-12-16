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

#ifndef _IGESGraph_DrawingUnits_HeaderFile
#define _IGESGraph_DrawingUnits_HeaderFile

#include <IGESData_IGESEntity.hxx>
class TCollection_HAsciiString;


//! defines IGESDrawingUnits, Type <406> Form <17>
//! in package IGESGraph
//!
//! Specifies the drawing space units as outlined
//! in the Drawing entity
class IGESGraph_DrawingUnits : public IGESData_IGESEntity
{
 public:

  Standard_EXPORT virtual Standard_Integer TypeNumber() const Standard_OVERRIDE { return 406; }

  Standard_EXPORT virtual Standard_Integer FormNumber() const Standard_OVERRIDE { return 17; }

  IGESGraph_DrawingUnits()
  : myFlag(1)
  {}

  //! returns the drawing space units of <me>
  Standard_Integer Flag() const { return myFlag; }

  //! returns the name of the drawing space units of <me>
  const Handle(TCollection_HAsciiString) & Unit() const { return myUnit; }

  //! Computes the value of the unit, in meters, according Flag
  //! (same values as for GlobalSection from IGESData)
  Standard_EXPORT Standard_Real UnitValue() const;

  Standard_EXPORT virtual void OwnRead (IGESFile_Reader &) Standard_OVERRIDE;
  
  Standard_EXPORT virtual void OwnWrite (IGESData_IGESWriter &) const Standard_OVERRIDE;

  Standard_EXPORT virtual IGESData_DirChecker DirChecker () const Standard_OVERRIDE;

  Standard_EXPORT virtual void OwnCheck (const Interface_ShareTool &, Handle(Interface_Check) &) const Standard_OVERRIDE;

  Standard_EXPORT virtual Standard_Boolean OwnCorrect () Standard_OVERRIDE;

  Standard_EXPORT virtual void OwnDump (const IGESData_IGESDumper &, const Handle(Message_Messenger) &, const Standard_Integer) const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(IGESGraph_DrawingUnits,IGESData_IGESEntity)

 private:

  Standard_Integer myFlag;
  Handle(TCollection_HAsciiString) myUnit;
};

#endif // _IGESGraph_DrawingUnits_HeaderFile
