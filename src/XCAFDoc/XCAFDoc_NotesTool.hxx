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
#include <TDF_LabelSequence.hxx>

class OSD_File;
class Standard_GUID;
class TCollection_AsciiString;
class TCollection_ExtendedString;
class TColStd_HArray1OfByte;
class TDF_RelocationTable;
class XCAFDoc_Note;

class XCAFDoc_NotesTool;
DEFINE_STANDARD_HANDLE(XCAFDoc_NotesTool, TDF_Attribute)

class XCAFDoc_NotesTool : public TDF_Attribute
{
public:

  DEFINE_STANDARD_RTTIEXT(XCAFDoc_NotesTool, TDF_Attribute)

  Standard_EXPORT static const Standard_GUID& GetID();

  Standard_EXPORT static Handle(XCAFDoc_NotesTool) Set(const TDF_Label& theLabel);

  Standard_EXPORT XCAFDoc_NotesTool();

  Standard_EXPORT Standard_Integer NbNotes() const;

  Standard_EXPORT void GetNotes(TDF_LabelSequence& theNoteLabels) const;

  Standard_EXPORT Handle(XCAFDoc_Note) AddComment(const TCollection_ExtendedString& theUserName,
                                                  const TCollection_ExtendedString& theTimeStamp,
                                                  const TCollection_ExtendedString& theComment);

  Standard_EXPORT Handle(XCAFDoc_Note) AddBinData(const TCollection_ExtendedString& theUserName,
                                                  const TCollection_ExtendedString& theTimeStamp,
                                                  const TCollection_ExtendedString& theTitle,
                                                  const TCollection_AsciiString&    theMIMEtype,
                                                  OSD_File&                         theFile);

  Standard_EXPORT Handle(XCAFDoc_Note) AddBinData(const TCollection_ExtendedString&    theUserName,
                                                  const TCollection_ExtendedString&    theTimeStamp,
                                                  const TCollection_ExtendedString&    theTitle,
                                                  const TCollection_AsciiString&       theMIMEtype,
                                                  const Handle(TColStd_HArray1OfByte)& theData);

  Standard_EXPORT Standard_Boolean HasAttachedNotes(const TDF_Label& theLabel) const;

  Standard_EXPORT void GetAttachedNotes(const TDF_Label&   theLabel,
                                        TDF_LabelSequence& theNoteLabels) const;
  Standard_EXPORT Standard_Integer DetachAllNotes(const TDF_Label& theLabel) const;

  Standard_EXPORT Standard_Boolean RemoveNote(const TDF_Label& theNoteLabel);
  Standard_EXPORT Standard_Integer RemoveNotes(TDF_LabelSequence& theNoteLabels);
  Standard_EXPORT Standard_Integer RemoveDetachedNotes();
  Standard_EXPORT Standard_Integer RemoveAllNotes();

public:

  // Overrides TDF_Attribute pure virtuals
  Standard_EXPORT const Standard_GUID& ID() const Standard_OVERRIDE;
  Standard_EXPORT Handle(TDF_Attribute) NewEmpty() const Standard_OVERRIDE;
  Standard_EXPORT void Restore(const Handle(TDF_Attribute)& theAttrFrom) Standard_OVERRIDE;
  Standard_EXPORT void Paste(const Handle(TDF_Attribute)&       theAttrInto, 
                             const Handle(TDF_RelocationTable)& theRT) const Standard_OVERRIDE;

};

#endif // _XCAFDoc_NotesTool_HeaderFile
