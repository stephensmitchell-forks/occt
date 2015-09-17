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

#ifndef _Storage_IODevice_HeaderFile
#define _Storage_IODevice_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>
#include <Storage_OpenMode.hxx>
#include <Storage_Error.hxx>
#include <Storage_Position.hxx>
#include <Storage_SeekMode.hxx>

#include <MMgt_TShared.hxx>
#include <TCollection_ExtendedString.hxx>
#include <TCollection_AsciiString.hxx>


class Storage_IODevice;
DEFINE_STANDARD_HANDLE(Storage_IODevice, MMgt_TShared)


//! Root any input output devices. A device define a way
//! to store or retrieve a data, for instance a file.
class Storage_IODevice : public MMgt_TShared 
{
public:

  Standard_EXPORT Storage_IODevice();

  Standard_EXPORT void Delete();

  Standard_EXPORT virtual ~Storage_IODevice() {Delete();}

  Standard_EXPORT virtual TCollection_ExtendedString Name() const = 0;
  
  Standard_EXPORT virtual Storage_Error Open (const Storage_OpenMode aMode) = 0;
  
  Standard_EXPORT void SetOpenMode( const Storage_OpenMode aMode );

  //! returns True if we are at end of the device data.
  Standard_EXPORT virtual Standard_Boolean IsEnd () const = 0;
    
  //! returns position in the device. Return -1 upon error.
  Standard_EXPORT virtual Storage_Position Tell () = 0;
  
  //! Set new absolute position within the stream
  Standard_EXPORT virtual Standard_Boolean Seek(const Storage_Position& aPos, 
                                                const Storage_SeekMode aMode = Storage_SMBegin ) = 0;
    
  Standard_EXPORT virtual Standard_Boolean Close () = 0;

  Standard_EXPORT virtual Standard_Boolean CanRead() const = 0;

  Standard_EXPORT virtual Standard_Boolean CanWrite()  const = 0;

  Standard_EXPORT virtual Standard_Size Read(const Standard_Address aBuffer,
                                             const Standard_Size aSize) = 0;
    
  Standard_EXPORT virtual Standard_Size Write(const Standard_Address aBuffer,
                                              const Standard_Size aSize) = 0;

  Standard_EXPORT virtual TCollection_AsciiString Signature() const = 0;

  Standard_EXPORT virtual Standard_OStream& Print( Standard_OStream& anOStream ) const;

  Standard_EXPORT void ReadLine( Standard_CString& aBuffer, const Standard_Integer aSize, const Standard_Character anEndSymbol );
  
  Standard_EXPORT TCollection_AsciiString ReadLine( const Standard_Character anEndSymbol );

  Standard_EXPORT void WriteLine( const Standard_CString aLine );

  Standard_EXPORT void WriteLine( const TCollection_AsciiString& aLine );

  Standard_EXPORT void WriteLine( const Standard_CString aLine, const Standard_Character anEndSymbol );
  
  Standard_EXPORT void WriteLine( const TCollection_AsciiString& aLine, const Standard_Character anEndSymbol );

  Standard_EXPORT Storage_OpenMode OpenMode() const;


private:

  Storage_OpenMode myOpenMode;
  
};

inline Standard_EXPORT Standard_OStream& operator<< (Standard_OStream& anOStream, const Handle(Storage_IODevice)& theDevice) { return theDevice->Print(anOStream);}


#include <Storage_IODevice.lxx>



#endif // _Storage_IODevice_HeaderFile
