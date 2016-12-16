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


#include <IGESData.hxx>
#include <IGESData_DefaultGeneral.hxx>
#include <IGESData_DirChecker.hxx>
#include <IGESData_FreeFormatEntity.hxx>
#include <IGESData_IGESEntity.hxx>
#include <IGESData_Protocol.hxx>
#include <IGESData_UndefinedEntity.hxx>
#include <Interface_Check.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_GeneralLib.hxx>
#include <Interface_Macros.hxx>
#include <Interface_ShareTool.hxx>
#include <Interface_UndefinedContent.hxx>
#include <TColStd_HSequenceOfInteger.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESData_DefaultGeneral,IGESData_GeneralModule)

IGESData_DefaultGeneral::IGESData_DefaultGeneral ()
{  Interface_GeneralLib::SetGlobal(this, IGESData::Protocol());  }


    IGESData_DirChecker  IGESData_DefaultGeneral::DirChecker
  (const Standard_Integer , const Handle(IGESData_IGESEntity)& ) const 
{  IGESData_DirChecker dc; return dc;  }  // aucun critere specifique
