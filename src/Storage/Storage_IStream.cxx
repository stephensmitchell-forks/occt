// File:      Storage_OStream.cxx
// Created:   20.05.15
// Author:    
// Copyright: Open CASCADE 2015

#include <Storage_IStream.ixx>

#include <Standard_SStream.hxx>

#include <TCollection_AsciiString.hxx>
#include <TCollection_ExtendedString.hxx>

//=======================================================================
//function : Storage_IStream
//purpose  : Constructor
//=======================================================================
Storage_IStream::Storage_IStream (Standard_IStream& theStream)
  : Storage_IODevice(),
    myStream( &theStream ),
    myBuffer( std::ios::in | std::ios::binary )
{
}

//=======================================================================
//function : Delete
//purpose  : 
//=======================================================================
void Storage_IStream::Delete()
{
}

//=======================================================================
//function : Name
//purpose  : 
//=======================================================================
TCollection_ExtendedString Storage_IStream::Name() const
{
  return "";
}

//=======================================================================
//function : Open
//purpose  : 
//=======================================================================
Storage_Error Storage_IStream::Open (const Storage_OpenMode theMode)
{
  if (theMode != Storage_VSRead)
  {
    return Storage_VSOpenError;
  }
  
  Storage_Error anOpenResult = Storage_VSOk;
  
  if (OpenMode() == Storage_VSNone)
  {
    if ( !myBuffer.good() ) // not good for eof
      anOpenResult = Storage_VSOpenError;
    else
    {
      SetOpenMode (theMode);

      // clear flags and set the position where the next character is to be inserted 
      myBuffer.clear();
      Seek( 0 );
    }
  }
  else
  {
    anOpenResult = Storage_VSAlreadyOpen;
  }

  return anOpenResult;
}

//=======================================================================
//function : IsEnd
//purpose  : 
//=======================================================================
Standard_Boolean Storage_IStream::IsEnd() const
{
  return myBuffer.eof() && myStream->eof();
}

//=======================================================================
//function : Tell
//purpose  : 
//=======================================================================
Storage_Position Storage_IStream::Tell()
{
  return Storage_Position( myBuffer.tellg() );
}

//=======================================================================
//function : Seek
//purpose  : 
//=======================================================================
Standard_Boolean Storage_IStream::Seek (const Storage_Position& thePos, const Storage_SeekMode aMode )
{
  if ( aMode == Storage_SMEnd )
  {
    fillBuffer();
    myBuffer.seekg( thePos, ios::end );
  }
  else
  {
    Standard_Size aCur = myBuffer.tellg();
    Standard_Size aPos = aMode == Storage_SMBegin ? thePos : aCur + thePos;
    if ( aPos > aCur )
    {
      myBuffer.seekg( 0, ios::end );
      Standard_Size aLast = myBuffer.tellg();
      if ( aLast < aPos )
      {
        Standard_Size aCount = aPos - aLast;
        char* aBuf = (char*)malloc( aCount );
        myStream->read( aBuf, aCount );
        Standard_Size aNum = (Standard_Size)myStream->gcount();
        std::string aStr = myBuffer.str();
        aStr.append( (char*)aBuf, aNum );
        myBuffer.str( aStr );
        free( aBuf );
        aPos = aLast + aNum;
      }
    }
    if ( aPos != aCur )
      myBuffer.seekg( aPos );
  }

  return !myBuffer.fail();
}

//=======================================================================
//function : Close
//purpose  : 
//=======================================================================
Standard_Boolean Storage_IStream::Close()
{
  SetOpenMode( Storage_VSNone );
  myBuffer.clear();

  return Standard_True;
}

//=======================================================================
//function : CanRead
//purpose  : 
//=======================================================================
Standard_Boolean Storage_IStream::CanRead() const
{
    return myBuffer.good() || myStream->good();
}

//=======================================================================
//function : CanWrite
//purpose  : 
//=======================================================================
Standard_Boolean Storage_IStream::CanWrite() const
{
  return Standard_False;
}

//=======================================================================
//function : Read
//purpose  : 
//=======================================================================
Standard_Size Storage_IStream::Read( const Standard_Address theBuffer, const Standard_Size theSize )
{
  myBuffer.read((char*)theBuffer, theSize);
  Standard_Size aCount = (Standard_Size)myBuffer.gcount();
  if ( aCount < theSize )
  {
    myStream->read((char*)theBuffer + aCount, theSize - aCount  );
    Standard_Size aNum = (Standard_Size)myStream->gcount();

    if ( aNum > 0 )
    {
      std::string aStr = myBuffer.str();
      aStr.append( (char*)theBuffer + aCount, aNum );
      myBuffer.str( aStr );
      aCount += aNum;

      myBuffer.clear();
      myBuffer.seekg( 0, std::ios::end );
    }
  }

  return aCount;
}

//=======================================================================
//function : Write
//purpose  : 
//=======================================================================
Standard_Size Storage_IStream::Write (const Standard_Address /*theBuffer*/, const Standard_Size /*theSize*/)
{  
  return Standard_Size(0);
}

//=======================================================================
//function : Signature
//purpose  : 
//=======================================================================
TCollection_AsciiString Storage_IStream::Signature() const
{
  return TCollection_AsciiString();
}

//=======================================================================
//function : Print
//purpose  : 
//=======================================================================
Standard_OStream& Storage_IStream::Print (Standard_OStream& theOStream) const
{
  return theOStream;
}

//=======================================================================
//function : fillBuffer
//purpose  : 
//=======================================================================
void Storage_IStream::fillBuffer()
{
  Standard_Size aCur = myBuffer.tellg();

  Standard_Size aSize = 8000;
  char* aBuf = (char*)malloc( aSize );
  while ( !myStream->eof() )
  {
    myStream->read( aBuf, aSize );
    Standard_Size aNum = (Standard_Size)myStream->gcount();
    std::string aStr = myBuffer.str();
    aStr.append( (char*)aBuf, aNum );
    myBuffer.str( aStr );
  }
  free( aBuf );

  myBuffer.seekg( aCur );
}
