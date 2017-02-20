// Created on: 2017-02-16
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

#ifndef _XCAFDoc_AssemblyItemRef_HeaderFile
#define _XCAFDoc_AssemblyItemRef_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>
#include <TDF_Attribute.hxx>
#include <XCAFDoc_AssemblyItemId.hxx>

class Standard_GUID;
class TDF_RelocationTable;

class XCAFDoc_AssemblyItemRef;
DEFINE_STANDARD_HANDLE(XCAFDoc_AssemblyItemRef, TDF_Attribute)

class XCAFDoc_AssemblyItemRef : public TDF_Attribute
{

public:

  DEFINE_STANDARD_RTTIEXT(XCAFDoc_AssemblyItemRef, TDF_Attribute);

  Standard_EXPORT static const Standard_GUID& GetID();

  Standard_EXPORT static Handle(XCAFDoc_AssemblyItemRef) Get(const TDF_Label& theLabel);

  Standard_EXPORT static Handle(XCAFDoc_AssemblyItemRef) Set(const TDF_Label&              theLabel,
                                                             const XCAFDoc_AssemblyItemId& theItemId);

  Standard_EXPORT XCAFDoc_AssemblyItemRef();

  Standard_EXPORT Standard_Boolean IsOrphan() const;

  Standard_EXPORT const XCAFDoc_AssemblyItemId& Get() const;
  
  Standard_EXPORT void Set(const XCAFDoc_AssemblyItemId& theItemId);
  Standard_EXPORT void Set(const TColStd_ListOfAsciiString& thePath);
  Standard_EXPORT void Set(const TCollection_AsciiString& theString);

public:

  Standard_EXPORT const Standard_GUID& ID() const Standard_OVERRIDE;

  Standard_EXPORT Handle(TDF_Attribute) NewEmpty() const Standard_OVERRIDE;

  Standard_EXPORT void Restore(const Handle(TDF_Attribute)& theAttrFrom) Standard_OVERRIDE;

  Standard_EXPORT void Paste(const Handle(TDF_Attribute)&       theAttrInto,
                             const Handle(TDF_RelocationTable)& theRT) const Standard_OVERRIDE;

  Standard_EXPORT Standard_OStream& Dump(Standard_OStream& theOS) const Standard_OVERRIDE;

private:

  XCAFDoc_AssemblyItemId myItemId;

};

#endif // _XCAFDoc_AssemblyItemRef_HeaderFile
