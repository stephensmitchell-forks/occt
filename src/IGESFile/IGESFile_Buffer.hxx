// Created: 2016-12-19 
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

#ifndef _Handle_IGESFile_Buffer_HeaderFile
#define _Handle_IGESFile_Buffer_HeaderFile

#include <NCollection_Buffer.hxx>

class IGESFile_Buffer;
DEFINE_STANDARD_HANDLE(IGESFile_Buffer,NCollection_Buffer)

#endif

#ifndef _IGESFile_Buffer_HeaderFile
#define _IGESFile_Buffer_HeaderFile

#include <Standard_Byte.hxx>
#include <Standard_Size.hxx>
#include <Standard_Character.hxx>

//! Data buffer for ACIS reading
class IGESFile_Buffer : public NCollection_Buffer
{
 public:

  //! Default constructor
  Standard_EXPORT IGESFile_Buffer();

  //! Reads a string
  Standard_EXPORT const Standard_Byte* AddLine(const Standard_Character *theBuffer, const Standard_Size theLength);

  //! Clears the buffer
  void Clear() { myLength = 0; }

  //! Returns the current length of data
  Standard_Size Length() const { return myLength; }

  DEFINE_STANDARD_RTTIEXT(IGESFile_Buffer,NCollection_Buffer)

 protected:

  Standard_EXPORT bool Enlarge();

  Standard_Size myLength; //! Current length of data
};

#endif
