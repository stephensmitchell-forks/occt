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

#ifndef _IGESSelect_FloatFormat_HeaderFile
#define _IGESSelect_FloatFormat_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineHandle.hxx>
#include <Standard_Boolean.hxx>
#include <TCollection_AsciiString.hxx>
#include <Standard_Real.hxx>
#include <IGESSelect_FileModifier.hxx>
#include <Standard_Integer.hxx>
#include <Standard_CString.hxx>

class TCollection_AsciiString;
class IFSelect_ContextWrite;
class IGESData_IGESWriter;
class Standard_Transient;
class Handle(Standard_Type);
class Handle(IGESSelect_FileModifier);
class IGESSelect_FloatFormat;

DEFINE_STANDARD_HANDLE(IGESSelect_FloatFormat,IGESSelect_FileModifier)

//! This class gives control out format for floatting values : <br>
//!           ZeroSuppress or no, Main Format, Format in Range (for values <br>
//!           around 1.), as IGESWriter allows to manage it. <br>
//!           Formats are given under C-printf form <br>
class IGESSelect_FloatFormat : public IGESSelect_FileModifier
{

public:

  //! Creates a new FloatFormat, with standard options : <br>
  //!           ZeroSuppress, Main Format = %E, <br>
  //!           Format between 0.001 and 1000. = %f <br>
  Standard_EXPORT   IGESSelect_FloatFormat();
  //! Sets FloatFormat to default value (see Create) but if <digits> <br>
  //!           is given positive, it commands Formats (main and range) to <br>
  //!           ensure <digits> significant digits to be displayed <br>
  Standard_EXPORT     void SetDefault(const Standard_Integer digits = 0) ;
  //! Sets ZeroSuppress mode to a new value <br>
  Standard_EXPORT     void SetZeroSuppress(const Standard_Boolean mode) ;
  //! Sets Main Format to a new value <br>
  //!           Remark : SetFormat, SetZeroSuppress and SetFormatForRange are <br>
  //!           independant <br>
  Standard_EXPORT     void SetFormat(const Standard_CString format = "%E") ;
  //! Sets Format for Range to a new value with its range of <br>
  //!           application. <br>
  //!           To cancel it, give format as "" (empty string) <br>
  //!           Remark that if the condition (0. < Rmin < Rmax)  is not <br>
  //!           verified, this secondary format will be ignored. <br>
  //!           Moreover, this secondary format is intended to be used in a <br>
  //!           range around 1. <br>
  Standard_EXPORT     void SetFormatForRange(const Standard_CString format = "%f",const Standard_Real Rmin = 0.1,const Standard_Real Rmax = 1000.0) ;
  //! Returns all recorded parameters : <br>
  //!           zerosup  : ZeroSuppress status <br>
  //!           mainform : Main Format (which applies out of the range, or <br>
  //!                       for every real if no range is set) <br>
  //!           hasrange : True if a FormatInRange is set, False else <br>
  //!                      (following parameters do not apply if it is False) <br>
  //!           forminrange : Secondary Format (it applies inside the range) <br>
  //!           rangemin, rangemax : the range in which the secondary format <br>
  //!                                applies <br>
  Standard_EXPORT     void Format(Standard_Boolean& zerosup,TCollection_AsciiString& mainform,Standard_Boolean& hasrange,TCollection_AsciiString& forminrange,Standard_Real& rangemin,Standard_Real& rangemax) const;
  //! Sets the Floatting Formats of IGESWriter to the recorded <br>
  //!           parameters <br>
  Standard_EXPORT     void Perform(IFSelect_ContextWrite& ctx,IGESData_IGESWriter& writer) const;
  //! Returns specific Label : for instance, <br>
  //!           "Float Format [ZeroSuppress] %E [, in range R1-R2 %f]" <br>
  Standard_EXPORT     TCollection_AsciiString Label() const;

  DEFINE_STANDARD_RTTI(IGESSelect_FloatFormat)

private: 

  Standard_Boolean thezerosup;
  TCollection_AsciiString themainform;
  TCollection_AsciiString theformrange;
  Standard_Real therangemin;
  Standard_Real therangemax;
};
#endif
