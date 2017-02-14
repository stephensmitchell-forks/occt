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

#ifndef _XCAFDoc_Note_HeaderFile
#define _XCAFDoc_Note_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>
#include <TCollection_ExtendedString.hxx>
#include <OSD_File.hxx>
#include <TDF_Attribute.hxx>
#include <TDF_LabelSequence.hxx>

class Standard_GUID;
class TDF_RelocationTable;

class XCAFDoc_Note;
DEFINE_STANDARD_HANDLE(XCAFDoc_Note, TDF_Attribute)

class XCAFDoc_Note : public TDF_Attribute
{
public:

  DEFINE_STANDARD_RTTIEXT(XCAFDoc_Note, TDF_Attribute)

  Standard_EXPORT static Standard_Boolean IsMine(const TDF_Label& theLabel);

  Standard_EXPORT static Handle(XCAFDoc_Note) Get(const TDF_Label& theLabel);

  Standard_EXPORT void Set(const TCollection_ExtendedString& theUserName,
                           const TCollection_ExtendedString& theTimeStamp);

  Standard_EXPORT const TCollection_ExtendedString& UserName() const;

  Standard_EXPORT const TCollection_ExtendedString& TimeStamp() const;

  Standard_EXPORT Standard_Boolean IsAttached() const;

  Standard_EXPORT void Attach(const TDF_LabelSequence& theLabels);

  Standard_EXPORT void Detach(const TDF_LabelSequence& theLabels);
  Standard_EXPORT void DetachAll();

public:

  Standard_EXPORT void Restore(const Handle(TDF_Attribute)& theAttrFrom) Standard_OVERRIDE;

  Standard_EXPORT void Paste(const Handle(TDF_Attribute)&       theAttrInto,
                             const Handle(TDF_RelocationTable)& theRT) const Standard_OVERRIDE;

  Standard_EXPORT Standard_OStream& Dump(Standard_OStream& theOS) const Standard_OVERRIDE;

protected:

  Standard_EXPORT XCAFDoc_Note();

private:

  TCollection_ExtendedString myUserName;
  TCollection_ExtendedString myTimeStamp;
};

#endif // _XCAFDoc_Note_HeaderFile
