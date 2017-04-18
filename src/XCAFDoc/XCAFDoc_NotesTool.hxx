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
class XCAFDoc_AssemblyItemId;
class XCAFDoc_AssemblyItemRef;

class XCAFDoc_NotesTool;
DEFINE_STANDARD_HANDLE(XCAFDoc_NotesTool, TDF_Attribute)

class XCAFDoc_NotesTool : public TDF_Attribute
{
public:

  DEFINE_STANDARD_RTTIEXT(XCAFDoc_NotesTool, TDF_Attribute)

  Standard_EXPORT static const Standard_GUID& GetID();

  Standard_EXPORT static Handle(XCAFDoc_NotesTool) Set(const TDF_Label& theLabel);

  Standard_EXPORT XCAFDoc_NotesTool();

  Standard_EXPORT TDF_Label GetNotesLabel() const;
  Standard_EXPORT TDF_Label GetAnnotatedItemsLabel() const;

  Standard_EXPORT Standard_Integer NbNotes() const;
  Standard_EXPORT Standard_Integer NbAnnotatedItems() const;

  Standard_EXPORT void GetNotes(TDF_LabelSequence& theNoteLabels) const;
  Standard_EXPORT void GetAnnotatedItems(TDF_LabelSequence& theLabels) const;

  Standard_EXPORT Standard_Boolean IsAnnotatedItem(const XCAFDoc_AssemblyItemId& theItemId) const;
  Standard_EXPORT TDF_Label FindAnnotatedItem(const XCAFDoc_AssemblyItemId& theItemId) const;
  Standard_EXPORT TDF_Label FindAnnotatedItemAttr(const XCAFDoc_AssemblyItemId& theItemId,
                                                  const Standard_GUID&          theGUID) const;
  Standard_EXPORT TDF_Label FindAnnotatedItemSubshape(const XCAFDoc_AssemblyItemId& theItemId,
                                                      Standard_Integer              theSubshapeIndex) const;

  Standard_EXPORT Handle(XCAFDoc_Note) CreateComment(const TCollection_ExtendedString& theUserName,
                                                     const TCollection_ExtendedString& theTimeStamp,
                                                     const TCollection_ExtendedString& theComment);

  Standard_EXPORT Handle(XCAFDoc_Note) CreateBinData(const TCollection_ExtendedString& theUserName,
                                                     const TCollection_ExtendedString& theTimeStamp,
                                                     const TCollection_ExtendedString& theTitle,
                                                     const TCollection_AsciiString&    theMIMEtype,
                                                     OSD_File&                         theFile);

  Standard_EXPORT Handle(XCAFDoc_Note) CreateBinData(const TCollection_ExtendedString&    theUserName,
                                                     const TCollection_ExtendedString&    theTimeStamp,
                                                     const TCollection_ExtendedString&    theTitle,
                                                     const TCollection_AsciiString&       theMIMEtype,
                                                     const Handle(TColStd_HArray1OfByte)& theData);

  Standard_EXPORT Standard_Integer GetNotes(const XCAFDoc_AssemblyItemId& theItemId,
                                            TDF_LabelSequence&            theNoteLabels) const;
  Standard_EXPORT Standard_Integer GetAttrNotes(const XCAFDoc_AssemblyItemId& theItemId,
                                                const Standard_GUID&          theGUID,
                                                TDF_LabelSequence&            theNoteLabels) const;
  Standard_EXPORT Standard_Integer GetSubshapeNotes(const XCAFDoc_AssemblyItemId& theItemId,
                                                    Standard_Integer              theSubshapeIndex,
                                                    TDF_LabelSequence&            theNoteLabels) const;

  Standard_EXPORT Handle(XCAFDoc_AssemblyItemRef) AddNote(const TDF_Label&              theNoteLabel,
                                                          const XCAFDoc_AssemblyItemId& theItemId);
  Standard_EXPORT Handle(XCAFDoc_AssemblyItemRef) AddNoteToAttr(const TDF_Label&              theNoteLabel,
                                                                const XCAFDoc_AssemblyItemId& theItemId,
                                                                const Standard_GUID&          theGUID);
  Standard_EXPORT Handle(XCAFDoc_AssemblyItemRef) AddNoteToSubshape(const TDF_Label&              theNoteLabel,
                                                                    const XCAFDoc_AssemblyItemId& theItemId,
                                                                    Standard_Integer              theSubshapeIndex);

  Standard_EXPORT Standard_Boolean RemoveNote(const TDF_Label&              theNoteLabel,
                                              const XCAFDoc_AssemblyItemId& theItemId,
                                              Standard_Boolean              theDelIfOrphan = Standard_False);
  Standard_EXPORT Standard_Boolean RemoveAllNotes(const XCAFDoc_AssemblyItemId& theItemId,
                                                  Standard_Boolean              theDelIfOrphan = Standard_False);

  Standard_EXPORT Standard_Boolean DeleteNote(const TDF_Label& theNoteLabel);
  Standard_EXPORT Standard_Integer DeleteNotes(TDF_LabelSequence& theNoteLabels);
  Standard_EXPORT Standard_Integer DeleteAllNotes();

  Standard_EXPORT Standard_Integer NbOrphanNotes() const;
  Standard_EXPORT void GetOrphanNotes(TDF_LabelSequence& theNoteLabels) const;
  Standard_EXPORT Standard_Integer DeleteOrphanNotes();

public:

  // Overrides TDF_Attribute pure virtuals
  Standard_EXPORT const Standard_GUID& ID() const Standard_OVERRIDE;
  Standard_EXPORT Handle(TDF_Attribute) NewEmpty() const Standard_OVERRIDE;
  Standard_EXPORT void Restore(const Handle(TDF_Attribute)& theAttrFrom) Standard_OVERRIDE;
  Standard_EXPORT void Paste(const Handle(TDF_Attribute)&       theAttrInto, 
                             const Handle(TDF_RelocationTable)& theRT) const Standard_OVERRIDE;
  Standard_EXPORT Standard_OStream& Dump(Standard_OStream& theOS) const Standard_OVERRIDE;

};

#endif // _XCAFDoc_NotesTool_HeaderFile
