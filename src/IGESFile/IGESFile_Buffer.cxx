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

#include <IGESFile_Buffer.hxx>

#include <NCollection_AlignedAllocator.hxx>
#include <TCollection_AsciiString.hxx>


IMPLEMENT_STANDARD_RTTIEXT(IGESFile_Buffer,NCollection_Buffer)

// The size of a memory block
static const Standard_Size gBufferBlockSize = 512;
// The default allocator for memory blocks
static const Handle(NCollection_BaseAllocator) gBufferBlockAllocator(new NCollection_AlignedAllocator(16));

//=======================================================================
//function : Read
//purpose  :
//=======================================================================

IGESFile_Buffer::IGESFile_Buffer()
: NCollection_Buffer(gBufferBlockAllocator,gBufferBlockSize),
  myLength(0)
{}


//=======================================================================
//function : ReadLine
//purpose  :
//=======================================================================

const Standard_Byte* IGESFile_Buffer::AddLine(const Standard_Character *theBuffer, const Standard_Size theLength)
{
  // Enlarge the buffer
  const Standard_Size aNewLength = myLength+theLength;
  while (mySize < aNewLength)
    if (!Enlarge())
      return NULL;
  // Read data from the stream
  Standard_Byte* aStart = &myData[myLength];
  memcpy(aStart,theBuffer,theLength);
  myLength = aNewLength-1;
  return aStart;
}


//=======================================================================
//function : Enlarge
//purpose  :
//=======================================================================

bool IGESFile_Buffer::Enlarge()
{
  const Standard_Size aNewSize = mySize+gBufferBlockSize;
  Standard_Byte *aData = (Standard_Byte*)myAllocator->Allocate(aNewSize);
  if (aData == NULL)
    return false;
  memcpy (aData, myData, mySize);
  Free();
  mySize = aNewSize;
  myData = aData;
  return true;
}
