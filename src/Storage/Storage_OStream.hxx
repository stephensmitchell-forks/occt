// Created on: 2015-05-18
// Created by: Sergey TELKOV
// Copyright (c) 2015 OPEN CASCADE SAS
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

#ifndef _Storage_OStream_HeaderFile
#define _Storage_OStream_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>
#include <Storage_IODevice.hxx>
#include <Storage_OpenMode.hxx>
#include <Storage_Error.hxx>
#include <Storage_Position.hxx>
#include <Storage_SeekMode.hxx>
#include <Standard_OStreamPtr.hxx>

#include <TCollection_ExtendedString.hxx>
#include <TCollection_AsciiString.hxx>


class Storage_OStream;
DEFINE_STANDARD_HANDLE(Storage_OStream, Storage_IODevice)


//! Root any input output devices. A device define a way
//! to store or retrieve a data, for instance a file.
class Storage_OStream : public Storage_IODevice 
{
public:

  Standard_EXPORT Storage_OStream( Standard_OStream& theStream );

  Standard_EXPORT void Delete();

  Standard_EXPORT virtual ~Storage_OStream() {Delete();}

  Standard_EXPORT virtual TCollection_ExtendedString Name() const;
  
  Standard_EXPORT virtual Storage_Error Open (const Storage_OpenMode aMode);
  
  //! returns True if we are at end of the device data.
  Standard_EXPORT virtual Standard_Boolean IsEnd () const;
    
  //! returns position in the device. Return -1 upon error.
  Standard_EXPORT virtual Storage_Position Tell ();
  
  //! Set new absolute position within the stream
  Standard_EXPORT virtual Standard_Boolean Seek(const Storage_Position& aPos, 
                                                const Storage_SeekMode aMode = Storage_SMBegin );
    
  Standard_EXPORT virtual Standard_Boolean Close ();

  Standard_EXPORT virtual Standard_Boolean CanRead() const;

  Standard_EXPORT virtual Standard_Boolean CanWrite() const;

  Standard_EXPORT virtual Standard_Size Read(const Standard_Address aBuffer,
                                             const Standard_Size aSize);
    
  Standard_EXPORT virtual Standard_Size Write(const Standard_Address aBuffer,
                                              const Standard_Size aSize);

  Standard_EXPORT virtual TCollection_AsciiString Signature() const;

  Standard_EXPORT virtual Standard_OStream& Print( Standard_OStream& anOStream ) const;


private:

  Standard_SStream   myBuffer;
  Standard_OStreamPtr myStream;
  
};




#endif // _Storage_OStream_HeaderFile
