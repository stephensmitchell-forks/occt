// Created on: 1993-01-11
// Created by: CKY / Contract Toubro-Larsen ( Arun MENON )
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

#ifndef _IGESAppli_DrilledHole_HeaderFile
#define _IGESAppli_DrilledHole_HeaderFile

#include <IGESData_IGESEntity.hxx>

//! defines DrilledHole, Type <406> Form <6>
//! in package IGESAppli
//! Identifies an entity representing a drilled hole
//! through a printed circuit board.
class IGESAppli_DrilledHole : public IGESData_IGESEntity
{
 public:

  Standard_EXPORT virtual Standard_Integer TypeNumber() const Standard_OVERRIDE { return 406; }

  Standard_EXPORT virtual Standard_Integer FormNumber() const Standard_OVERRIDE { return 6; }

  IGESAppli_DrilledHole()
  : myDrillDiaSize(0.),
    myFinishDiaSize(0.),
    myPlatingFlag(0),
    myNbLowerLayer(0),
    myNbHigherLayer(0)
  {}

  //! returns the drill diameter size
  Standard_Real DrillDiaSize() const { return myDrillDiaSize; }

  //! returns the finish diameter size
  Standard_Real FinishDiaSize() const { return myFinishDiaSize; }

  //! Returns Plating Status :
  //! False = not plating  /  True  = is plating
  Standard_Boolean IsPlating() const { return (myPlatingFlag != 0); }

  //! returns the lower numbered layer
  Standard_Integer NbLowerLayer() const { return myNbLowerLayer; }

  //! returns the higher numbered layer
  Standard_Integer NbHigherLayer() const { return myNbHigherLayer; }

  Standard_EXPORT virtual void OwnRead (IGESFile_Reader &) Standard_OVERRIDE;
  
  Standard_EXPORT virtual void OwnWrite (IGESData_IGESWriter &) const Standard_OVERRIDE;
  
  Standard_EXPORT virtual IGESData_DirChecker DirChecker () const Standard_OVERRIDE;

  Standard_EXPORT virtual void OwnCheck (const Interface_ShareTool &, const Handle(Interface_Check) &) const Standard_OVERRIDE;

  Standard_EXPORT Standard_Boolean OwnCorrect () Standard_OVERRIDE;

  Standard_EXPORT virtual void OwnDump (const IGESData_IGESDumper &, const Handle(Message_Messenger) &, const Standard_Integer) const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(IGESAppli_DrilledHole,IGESData_IGESEntity)

 private:

  Standard_Real myDrillDiaSize;
  Standard_Real myFinishDiaSize;
  Standard_Integer myPlatingFlag;
  Standard_Integer myNbLowerLayer;
  Standard_Integer myNbHigherLayer;
};

#endif // _IGESAppli_DrilledHole_HeaderFile
