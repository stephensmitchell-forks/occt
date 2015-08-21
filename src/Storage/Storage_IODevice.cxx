// File:      Storage_IODevice.cxx
// Created:   18.05.15 12:41:53
// Author:    stv@NETEX
// Copyright: Open CASCADE 2015

#include <Storage_IODevice.hxx>

//=======================================================================
//function : Storage_IODevice
//purpose  : Constructor
//=======================================================================

Storage_IODevice::Storage_IODevice()
: myOpenMode(Storage_VSNone)
{
}

void Storage_IODevice::Delete()
{
}
  
void Storage_IODevice::SetOpenMode( const Storage_OpenMode aMode )
{
  myOpenMode = aMode;
}

Standard_OStream& Storage_IODevice::Print( Standard_OStream& anOStream ) const
{
    return anOStream;
}

void Storage_IODevice::ReadLine( Standard_CString& aBuffer, const Standard_Integer aSize, const Standard_Character anEndSymbol )
{
  Standard_Integer i = 0;
  Standard_Boolean aFin = Standard_False;
  Standard_Character* aBuf = (Standard_Character*)aBuffer;
  while ( CanRead() && !IsEnd() && i < aSize && !aFin )
  {
    Standard_Character c;
    Read( (Standard_Address)&c, sizeof( Standard_Character ) );
    aFin = ( c == anEndSymbol );
    if ( aFin )
      aBuf[i++] = '\0';
    else
      aBuf[i++] = c;
  }
}
  
TCollection_AsciiString Storage_IODevice::ReadLine( const Standard_Character anEndSymbol )
{
  TCollection_AsciiString aLine;
  Standard_Boolean aFin = Standard_False;
  while ( CanRead() && !IsEnd() && !aFin )
  {
    Standard_Character c;
    Read( (Standard_Address)&c, sizeof( Standard_Character ) );
    aFin = ( c == anEndSymbol );
    if ( !aFin )
      aLine += c;
  }
  return aLine;
}

void Storage_IODevice::WriteLine( const Standard_CString aLine )
{
  WriteLine( aLine, '\n' );
}

void Storage_IODevice::WriteLine( const TCollection_AsciiString& aLine )
{
  WriteLine( aLine, '\n' );
}

void Storage_IODevice::WriteLine( const Standard_CString aLine, const Standard_Character anEndSymbol )
{
  if ( !CanWrite() )
    return;

  Write( (Standard_Address)aLine, strlen( aLine ) );
  Write( (Standard_Address)&anEndSymbol, sizeof( Standard_Character ) );
}
  
void Storage_IODevice::WriteLine( const TCollection_AsciiString& aLine, const Standard_Character anEndSymbol )
{
  if ( !CanWrite() )
    return;

  Write( (Standard_Address)aLine.ToCString(), aLine.Length() );
  Write( (Standard_Address)&anEndSymbol, sizeof( Standard_Character ) );
}
