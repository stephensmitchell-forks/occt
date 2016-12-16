// Created on: 1992-10-26
// Created by: Christian CAILLET
// Copyright (c) 1992-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#ifndef _IGESData_ParamCursor_HeaderFile
#define _IGESData_ParamCursor_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>
class Interface_InterfaceError;


//! Auxiliary class for ParamReader.
//! It stores commands for a ParamReader to manage the current
//! parameter number. Used by methods Read... from ParamReader.
//! It allows to define the following commands :
//! - read a parameter specified by a precise Number (basic case)
//! - read a parameter then set Current Number to follow its number
//! - read the current parameter (with Current Number) then
//! advance Current Number by one
//! - idem with several : read "nb" parameters from one specified,
//! included, with or without setting Current Number to follow
//! last parameter read
//! - read several parameter from the current one, then advance
//! Current Number to follow the last one read
//! - Read several parameters (as above) but in interlaced lists,
//! i.e. from complex items (each one including successively for
//! instance, an Integer, a Real, an Entity ...)
//!
//! If commands to advance Current Number are not set, it must be
//! set by the user (with method SetCurrent from ParamReader)
//! ParamReader offers methods which create most useful cases
class IGESData_ParamCursor 
{
 public:

  DEFINE_STANDARD_ALLOC

  //! Creates a Cursor to read a precise parameter of ParamReader,
  //! identified by its number, then set Current Number to "num + 1"
  //! (this constructor allows to simply give a Number to a method
  //! Read... from ParamReader, which will be translated into a
  //! ParamCursor by compiler)
  IGESData_ParamCursor (const Standard_Integer num)
  : thestart(num),
    thelimit(num+1),
    thecount(1),
    thesize(1)
  {}

  //! Creates a Cursor to read a list of parameters (count "nb")
  //! starting from a precise one (number "num") included, then
  //! set Current Number of ParamNumber to the first following one
  //! ("num + nb")
  //! If size is given, it means that each parameter is made of more
  //! than one term. One term is the normal (default) case : for
  //! instance, a Parameter comprises one Integer, or one Entity ...
  //! Size gives the complete size of each Item if it is complex.
  //! To be used ONLY IF it is constant
  IGESData_ParamCursor (const Standard_Integer num, const Standard_Integer nb, const Standard_Integer size = 1)
  : thestart(num),
    thelimit(num + (nb*size)),
    thecount(nb),
    thesize(size)
  {}
  
  //! Returns (included) starting number for reading parameters
  Standard_Integer Start() const { return thestart; }

  //! Returns (excluded) upper limit number for reading parameters
  Standard_Integer Limit() const { return thelimit; }

  //! Returns required count of items to be read
  Standard_Integer Count() const { return thecount; }

  //! Returns length of item (count of parameters per item)
  Standard_Integer Size() const { return thesize; }

 private:

  Standard_Integer thestart;
  Standard_Integer thelimit;
  Standard_Integer thecount;
  Standard_Integer thesize;
};

#endif // _IGESData_ParamCursor_HeaderFile
