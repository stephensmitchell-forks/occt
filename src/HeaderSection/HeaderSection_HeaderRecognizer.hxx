// Copyright (c) 2013 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

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
