// Created on: 1994-11-04
// Created by: Christian CAILLET
// Copyright (c) 1994-1999 Matra Datavision
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

#ifndef _Transfer_Finder_HeaderFile
#define _Transfer_Finder_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>
#include <MMgt_TShared.hxx>

//! A Finder allows to map any kind of object as a Key for a Map.
//! This works by defining, for a Hash Code, that of the real Key,
//! not of the Finder which acts only as an intermediate.
//! When a Map asks for the HashCode of a Finder, this one returns
//! the code it has determined at creation time
class Transfer_Finder : public MMgt_TShared
{
 public:

  //! Default constructor
  Transfer_Finder() {}

  //! Returns the HashCode which has been stored by SetHashCode
  //! (remark that HashCode could be deferred then be defined by
  //! sub-classes, the result is the same)
  Standard_Integer virtual GetHashCode(const Standard_Integer theUpper) const = 0;
  
  //! Specific test of equality : to be defined by each sub-class,
  //! must be False if Finders have not the same true Type, else
  //! their contents must be compared
  Standard_EXPORT virtual Standard_Boolean Equates (const Handle(Transfer_Finder)& theOther) const = 0;
  
  //! Returns the name of the Type of the Value. Default is name
  //! of ValueType, unless it is for a non-handled object
  Standard_EXPORT virtual Standard_CString ValueTypeName() const = 0;

  DEFINE_STANDARD_RTTIEXT(Transfer_Finder,MMgt_TShared)
};

#endif // _Transfer_Finder_HeaderFile
