// Created on: 2017-02-10
// Created by: Sergey NIKONOV
// Copyright (c) 2000-2017 OPEN CASCADE SAS
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

#ifndef _XCAFDoc_NotesTool_HeaderFile
#define _XCAFDoc_NotesTool_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>
#include <TDF_Attribute.hxx>

class Standard_GUID;
class TDF_RelocationTable;

class XCAFDoc_NotesTool;
DEFINE_STANDARD_HANDLE(XCAFDoc_NotesTool, TDF_Attribute)

class XCAFDoc_NotesTool : public TDF_Attribute
{
public:

  DEFINE_STANDARD_RTTIEXT(XCAFDoc_NotesTool, TDF_Attribute)

  Standard_EXPORT static const Standard_GUID& GetID();

  //! Create (if not exist) NotesTool from XCAFDoc on <L>.
  Standard_EXPORT static Handle(XCAFDoc_NotesTool) Set(const TDF_Label& theLabel);

  //! Creates an empty tool
  Standard_EXPORT XCAFDoc_NotesTool();

public:

  Standard_EXPORT const Standard_GUID& ID() const Standard_OVERRIDE;

  Standard_EXPORT Handle(TDF_Attribute) NewEmpty() const Standard_OVERRIDE;

  Standard_EXPORT void Restore(const Handle(TDF_Attribute)& theAttrFrom) Standard_OVERRIDE;

  Standard_EXPORT void Paste(const Handle(TDF_Attribute)&       theAttrInto, 
                             const Handle(TDF_RelocationTable)& theRT) const Standard_OVERRIDE;

};

#endif // _XCAFDoc_NotesTool_HeaderFile
