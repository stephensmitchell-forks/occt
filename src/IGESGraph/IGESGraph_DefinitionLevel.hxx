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

#ifndef _IGESGraph_DefinitionLevel_HeaderFile
#define _IGESGraph_DefinitionLevel_HeaderFile

#include <IGESData_LevelListEntity.hxx>
class TColStd_HArray1OfInteger;


//! defines IGESDefinitionLevel, Type <406> Form <1>
//! in package IGESGraph
//!
//! Indicates the no. of levels on which an entity is
//! defined
class IGESGraph_DefinitionLevel : public IGESData_LevelListEntity
{
 public:

  Standard_EXPORT virtual Standard_Integer TypeNumber() const Standard_OVERRIDE { return 406; }

  Standard_EXPORT virtual Standard_Integer FormNumber() const Standard_OVERRIDE { return 1; }

  IGESGraph_DefinitionLevel() {}
  
  //! returns the number of property values in <me>
  Standard_EXPORT Standard_Integer NbPropertyValues() const;
  
  //! Must return the count of levels (== NbPropertyValues)
  Standard_EXPORT Standard_Integer NbLevelNumbers() const Standard_OVERRIDE;
  
  //! returns the Level Number of <me> indicated by <LevelIndex>
  //! raises an exception if LevelIndex is <= 0 or
  //! LevelIndex > NbPropertyValues
  Standard_EXPORT Standard_Integer LevelNumber (const Standard_Integer LevelIndex) const Standard_OVERRIDE;

  Standard_EXPORT virtual void OwnRead (IGESFile_Reader &) Standard_OVERRIDE;
  
  Standard_EXPORT virtual void OwnWrite (IGESData_IGESWriter &) const Standard_OVERRIDE;

  Standard_EXPORT virtual IGESData_DirChecker DirChecker () const Standard_OVERRIDE;

  Standard_EXPORT virtual void OwnDump (const IGESData_IGESDumper &, const Handle(Message_Messenger) &, const Standard_Integer) const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(IGESGraph_DefinitionLevel,IGESData_LevelListEntity)

 private:

  Handle(TColStd_HArray1OfInteger) myLevelNumbers;
};

#endif // _IGESGraph_DefinitionLevel_HeaderFile
