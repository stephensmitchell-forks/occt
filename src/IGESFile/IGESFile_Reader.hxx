// Created on: 2016-12-15
// Created by: Sergey ZERCHANINOV
// Copyright (c) 2016 OPEN CASCADE SAS
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

#ifndef _IGESFile_Reader_HeaderFile
#define _IGESFile_Reader_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <IGESData_GlobalSection.hxx>
class TCollection_AsciiString;
class TCollection_HAsciiString;
class gp_XY;
class gp_XYZ;
class Interface_Check;
class Interface_PointerBase;
class IGESData_Protocol;
class IGESData_IGESModel;
class IGESFile_Buffer;

class IGESFile_Reader
{
 public:

  enum ParamStatus
  {
    ParamOK,
    ParamEmpty,
    ParamError
  };

 public:

  DEFINE_STANDARD_ALLOC

  //! Constructor
  Standard_EXPORT IGESFile_Reader (const Handle(IGESData_Protocol)& theProtocol);

  //! ...
  Standard_EXPORT Standard_Boolean HasData () const;

  //! ...
  Standard_EXPORT Standard_Integer Read (const Standard_CString theFileName);

  //! Reads an integer value
  Standard_EXPORT ParamStatus ReadBoolean (Standard_Boolean &theVal, const Standard_CString theMsg = NULL);

  //! Reads an integer value
  ParamStatus ReadInteger (Standard_Integer &theVal, const Standard_CString theMsg = NULL) { return ReadInteger(theVal,1,theMsg); }

  //! Reads an array of integer values
  Standard_EXPORT ParamStatus ReadInteger (Standard_Integer &theVal, const Standard_Integer theNum, const Standard_CString theMsg = NULL);

  //! Reads a double value
  ParamStatus ReadReal (Standard_Real &theVal, const Standard_CString theMsg = NULL) { return ReadReal(theVal,1,theMsg); }

  //! Reads an array of double values
  Standard_EXPORT ParamStatus ReadReal (Standard_Real &theVal, const Standard_Integer theNum, const Standard_CString theMsg = NULL);

  //! Reads an XYZ value
  Standard_EXPORT ParamStatus ReadXY (gp_XY &theVal, const Standard_CString theMsg = NULL);

  //! Reads an XYZ value
  Standard_EXPORT ParamStatus ReadXYZ (gp_XYZ &theVal, const Standard_CString theMsg = NULL);

  //! ...
  Standard_EXPORT ParamStatus ReadString (TCollection_AsciiString &theVal, const Standard_CString theMsg = NULL);

  //! ...
  Standard_EXPORT ParamStatus ReadText (Handle(TCollection_HAsciiString) &theVal, const Standard_CString theMsg = NULL);

  //! ...
  Standard_EXPORT ParamStatus ReadPointer (Interface_PointerBase &thePointer, const Standard_CString theMsg = NULL);

  //! Skips a parameter
  Standard_EXPORT ParamStatus ReadAny (const Standard_CString theMsg = NULL);

  //! ...
  Standard_EXPORT ParamStatus GetPointer (Interface_PointerBase &thePointer, const Standard_Integer theIndex);

  //! ...
  const Handle(IGESData_IGESModel) & Model () { return myModel; }

  Standard_EXPORT void AddWarning (const Standard_CString afail, const Standard_CString bfail = "");

  Standard_EXPORT void AddFail (const Standard_CString afail, const Standard_CString bfail = "");

 private:

  Standard_EXPORT Standard_Integer ReadLine (Standard_Character *theBuffer, Standard_Size &theLength);

  Standard_EXPORT Standard_Boolean ParseHeader ();

  Standard_EXPORT Standard_Boolean ParseDirectory ();

  //! ...
  Standard_EXPORT void LoadModel ();

  Handle(IGESData_Protocol) myProtocol;
  Handle(IGESData_IGESModel) myModel;
  Handle(Interface_Check) myCheck;

  FILE* myFile;
  Standard_Integer myCurrentLine;
  Standard_Integer mySectionCode;
  Standard_Integer mySectionLine[6];

  Handle(IGESFile_Buffer) myBuffer;
  Standard_Size myCurrentPos;

  IGESData_GlobalSection myHeader;
};

#endif // _IGESFile_Reader_HeaderFile
