// Copyright (c) 1999-2013 OPEN CASCADE SAS
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

#ifndef _IGESSelect_AddFileComment_HeaderFile
#define _IGESSelect_AddFileComment_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineHandle.hxx>
#include <Handle_TColStd_HSequenceOfHAsciiString.hxx>
#include <IGESSelect_FileModifier.hxx>
#include <Standard_CString.hxx>
#include <Standard_Integer.hxx>

class TColStd_HSequenceOfHAsciiString;
class IFSelect_ContextWrite;
class IGESData_IGESWriter;
class TCollection_AsciiString;
class Standard_Transient;
class Handle(Standard_Type);
class Handle(IGESSelect_FileModifier);
class IGESSelect_AddFileComment;

DEFINE_STANDARD_HANDLE(IGESSelect_AddFileComment,IGESSelect_FileModifier)

//! This class allows to add comment lines on writing an IGES File <br>
//!           These lines are added to Start Section, instead of the only <br>
//!           one blank line written by default. <br>
class IGESSelect_AddFileComment : public IGESSelect_FileModifier
{

public:

  //! Creates a new emoty AddFileComment. Use AddLine to complete it <br>
  Standard_EXPORT   IGESSelect_AddFileComment();
  //! Clears the list of file comment lines already stored <br>
  Standard_EXPORT     void Clear() ;
  //! Adds a line for file comment <br>
  //!  Remark : Lines are limited to 72 useful char.s . A line of more than <br>
  //!           72 char.s will be splited into several ones of 72 max each. <br>
  Standard_EXPORT     void AddLine(const Standard_CString line) ;
  //! Adds a list of lines for file comment <br>
  //!           Each of them must comply with demand of AddLine <br>
  Standard_EXPORT     void AddLines(const Handle(TColStd_HSequenceOfHAsciiString)& lines) ;
  //! Returns the count of stored lines <br>
  Standard_EXPORT     Standard_Integer NbLines() const;
  //! Returns a stored line given its rank <br>
  Standard_EXPORT     Standard_CString Line(const Standard_Integer num) const;
  //! Returns the complete list of lines in once <br>
  Standard_EXPORT     Handle_TColStd_HSequenceOfHAsciiString Lines() const;
  //! Sends the comment lines to the file (Start Section) <br>
  Standard_EXPORT     void Perform(IFSelect_ContextWrite& ctx,IGESData_IGESWriter& writer) const;
  //! Returns specific Label, which is <br>
  //!           "Add <nn> Comment Lines (Start Section)" <br>
  Standard_EXPORT     TCollection_AsciiString Label() const;


  DEFINE_STANDARD_RTTI(IGESSelect_AddFileComment)

private: 
  Handle_TColStd_HSequenceOfHAsciiString thelist;
};
#endif
