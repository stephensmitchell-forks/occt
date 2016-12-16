// Created on: 1993-09-08
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

#ifndef _IGESData_DefaultGeneral_HeaderFile
#define _IGESData_DefaultGeneral_HeaderFile

#include <IGESData_GeneralModule.hxx>


class IGESData_DefaultGeneral;
DEFINE_STANDARD_HANDLE(IGESData_DefaultGeneral, IGESData_GeneralModule)

//! Processes the specific case of UndefinedEntity from IGESData
//! (Case Number 1)
class IGESData_DefaultGeneral : public IGESData_GeneralModule
{
 public:

  //! Creates a DefaultGeneral and puts it into GeneralLib,
  //! bound with a Protocol from IGESData
  Standard_EXPORT IGESData_DefaultGeneral();
  
  //! Returns a DirChecker, specific for each type of Entity
  //! Here, Returns an empty DirChecker (no constraint to check)
  Standard_EXPORT IGESData_DirChecker DirChecker (const Standard_Integer CN, const Handle(IGESData_IGESEntity)& ent) const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(IGESData_DefaultGeneral,IGESData_GeneralModule)
};

#endif // _IGESData_DefaultGeneral_HeaderFile
