// Copyright (c) 1998-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and / or modify it
// under the terms of the GNU Lesser General Public version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#ifndef _HeaderSection_HeaderRecognizer_HeaderFile
#define _HeaderSection_HeaderRecognizer_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineHandle.hxx>
#include <TCollection_AsciiString.hxx>
#include <StepData_FileRecognizer.hxx>

class TCollection_AsciiString;

//! Recognizes STEP Standard Header Entities <br>
//!           (FileName, FileDescription, FileSchema) <br>
class HeaderSection_HeaderRecognizer : public StepData_FileRecognizer
{
public:
  Standard_EXPORT   HeaderSection_HeaderRecognizer();

  DEFINE_STANDARD_RTTI(HeaderSection_HeaderRecognizer)

protected:

  //! Recognizes data types of Header STEP Standard <br>
  Standard_EXPORT     void Eval(const TCollection_AsciiString& key);

};

 DEFINE_STANDARD_HANDLE(HeaderSection_HeaderRecognizer, StepData_FileRecognizer)

#endif
