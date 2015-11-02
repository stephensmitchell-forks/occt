// Created on: 1993-09-07
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

#ifndef _IGESSolid_SpecificModule_HeaderFile
#define _IGESSolid_SpecificModule_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <IGESData_SpecificModule.hxx>
#include <Standard_Integer.hxx>
class IGESData_IGESEntity;
class IGESData_IGESDumper;
class Message_Messenger;


class IGESSolid_SpecificModule;
DEFINE_STANDARD_HANDLE(IGESSolid_SpecificModule, IGESData_SpecificModule)

//! Defines Services attached to IGES Entities : Dump, for IGESSolid
class IGESSolid_SpecificModule : public IGESData_SpecificModule
{

public:

  
  //! Creates a SpecificModule from IGESSolid & puts it into SpecificLib
  Standard_EXPORT IGESSolid_SpecificModule();
  
  //! Specific Dump (own parameters) for IGESSolid
  Standard_EXPORT void OwnDump (const Standard_Integer CN, const Handle(IGESData_IGESEntity)& ent, const IGESData_IGESDumper& dumper, const Handle(Message_Messenger)& S, const Standard_Integer own) const;




  DEFINE_STANDARD_RTTI(IGESSolid_SpecificModule,IGESData_SpecificModule)

protected:




private:




};







#endif // _IGESSolid_SpecificModule_HeaderFile