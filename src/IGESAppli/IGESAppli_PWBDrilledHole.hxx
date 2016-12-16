// Created on: 1993-01-11
// Created by: CKY / Contract Toubro-Larsen ( Anand NATRAJAN )
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

#ifndef _IGESAppli_PWBDrilledHole_HeaderFile
#define _IGESAppli_PWBDrilledHole_HeaderFile

#include <IGESData_IGESEntity.hxx>

class IGESAppli_PWBDrilledHole;
DEFINE_STANDARD_HANDLE(IGESAppli_PWBDrilledHole, IGESData_IGESEntity)

//! defines PWBDrilledHole, Type <406> Form <26>
//! in package IGESAppli
//! Used to identify an entity that locates a drilled hole
//! and to specify the characteristics of the drilled hole
class IGESAppli_PWBDrilledHole : public IGESData_IGESEntity
{
 public:

  Standard_EXPORT virtual Standard_Integer TypeNumber() const Standard_OVERRIDE { return 406; }

  Standard_EXPORT virtual Standard_Integer FormNumber() const Standard_OVERRIDE { return 26; }

  IGESAppli_PWBDrilledHole()
  : myDrillDiameter(0.),
    myFinishDiameter(0.),
    myFunctionCode(0)
  {}

  //! returns Drill diameter size
  Standard_Real DrillDiameterSize() const { return myDrillDiameter; }

  //! returns Finish diameter size
  Standard_Real FinishDiameterSize() const { return myFinishDiameter; }

  //! returns Function code for drilled hole
  //! is 0, 1, 2, 3, 4, 5 or 5001-9999
  Standard_Integer FunctionCode() const { return myFunctionCode; }

  Standard_EXPORT virtual void OwnRead (IGESFile_Reader &) Standard_OVERRIDE;
  
  Standard_EXPORT virtual void OwnWrite (IGESData_IGESWriter &) const Standard_OVERRIDE;

  Standard_EXPORT virtual IGESData_DirChecker DirChecker () const Standard_OVERRIDE;

  Standard_EXPORT virtual void OwnCheck (const Interface_ShareTool &, const Handle(Interface_Check) &) const Standard_OVERRIDE;

  Standard_EXPORT virtual void OwnDump (const IGESData_IGESDumper &, const Handle(Message_Messenger) &, const Standard_Integer) const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(IGESAppli_PWBDrilledHole,IGESData_IGESEntity)

 private:

  Standard_Real myDrillDiameter;
  Standard_Real myFinishDiameter;
  Standard_Integer myFunctionCode;
};

#endif // _IGESAppli_PWBDrilledHole_HeaderFile
