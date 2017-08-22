// Created on: 2017-08-22
// Created by: Sergey SLYADNEV
// Copyright (c) 2017 OPEN CASCADE SAS
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

#ifndef _XDEDRAW_Assemblies_HeaderFile
#define _XDEDRAW_Assemblies_HeaderFile

#include <Draw_Interpretor.hxx>

//! Draw commands for new XDE interface dedicated to assemblies.
class XDEDRAW_Assemblies
{
public:

  DEFINE_STANDARD_ALLOC

  Standard_EXPORT static void InitCommands(Draw_Interpretor& theCommands);

};

#endif // _XDEDRAW_Assemblies_HeaderFile
