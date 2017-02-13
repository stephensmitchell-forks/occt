// Created on: 2017-02-13
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

#ifndef _XCAFDoc_NoteComment_HeaderFile
#define _XCAFDoc_NoteComment_HeaderFile

#include <XCAFDoc_Note.hxx>

class XCAFDoc_NoteComment;
DEFINE_STANDARD_HANDLE(XCAFDoc_NoteComment, XCAFDoc_Note)

class XCAFDoc_NoteComment : public XCAFDoc_Note
{
public:

  DEFINE_STANDARD_RTTIEXT(XCAFDoc_NoteComment, XCAFDoc_Note)

  Standard_EXPORT static const Standard_GUID& GetID();

  Standard_EXPORT static Standard_Boolean IsMine(const TDF_Label& theLabel);

  Standard_EXPORT static Handle(XCAFDoc_NoteComment) Set(const TDF_Label&                           theLabel,
                                                         const Handle(TCollection_HExtendedString)& theUserName,
                                                         const Handle(TCollection_HExtendedString)& theTimeStamp,
                                                         const Handle(TCollection_HExtendedString)& theComment);

  Standard_EXPORT XCAFDoc_NoteComment();

  Standard_EXPORT void Set(const Handle(TCollection_HExtendedString)& theComment);

  Standard_EXPORT Handle(TCollection_HExtendedString) Comment() const;

public:

  Standard_EXPORT const Standard_GUID& ID() const Standard_OVERRIDE;

  Standard_EXPORT Handle(TDF_Attribute) NewEmpty() const Standard_OVERRIDE;

  Standard_EXPORT void Restore(const Handle(TDF_Attribute)& theAttrFrom) Standard_OVERRIDE;

  Standard_EXPORT void Paste(const Handle(TDF_Attribute)&       theAttrInto,
                             const Handle(TDF_RelocationTable)& theRT) const Standard_OVERRIDE;

  Standard_EXPORT Standard_OStream& Dump(Standard_OStream& theOS) const Standard_OVERRIDE;

protected:

  Handle(TCollection_HExtendedString) myComment;
};

#endif // _XCAFDoc_NoteComment_HeaderFile
