// File:      Storage_OStream.cxx
// Created:   20.05.15
// Author:    
// Copyright: Open CASCADE 2015

#include <Storage_OStream.ixx>

#include <TCollection_AsciiString.hxx>
#include <TCollection_ExtendedString.hxx>

//=======================================================================
//function : Storage_OStream
//purpose  : Constructor
//=======================================================================
Storage_OStream::Storage_OStream (Standard_OStream& theStream)
  : Storage_IODevice(),
    myStream (&theStream)
{
}

//=======================================================================
//function : Delete
//purpose  : 
//=======================================================================
void Storage_OStream::Delete()
{
}

//=======================================================================
//function : Stream
//purpose  : 
//=======================================================================
Standard_OStreamPtr Storage_OStream::Stream() const
{
  return myStream;
}

//=======================================================================
//function : Name
//purpose  : 
//=======================================================================
TCollection_ExtendedString Storage_OStream::Name() const
{
  return "";
}

//=======================================================================
//function : Open
//purpose  : 
//=======================================================================
Storage_Error Storage_OStream::Open (const Storage_OpenMode theMode)
{
  if (theMode != Storage_VSWrite || theMode != Storage_VSAppend)
  {
    return Storage_VSOpenError;
  }
  
  Storage_Error anOpenResult = Storage_VSOk;
  
  if (OpenMode() == Storage_VSNone)
  {
    if (!myStream->good()) // not good for eof
    {
      anOpenResult = Storage_VSOpenError;
    }
    else
    {
      SetOpenMode (theMode);
      
      // clear flags and set the position where the next character is to be inserted 
      myStream->clear();
      if ( theMode == Storage_VSWrite )
        myStream->seekp(0, ios::beg);
      else
        myStream->seekp(0, ios::end);
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
Standard_Boolean Storage_OStream::IsEnd() const
{
  return myStream->eof();
}

//=======================================================================
//function : Tell
//purpose  : 
//=======================================================================
Storage_Position Storage_OStream::Tell()
{
  return Storage_Position (myStream->tellp());
}

//=======================================================================
//function : Seek
//purpose  : 
//=======================================================================
Standard_Boolean Storage_OStream::Seek (const Storage_Position& thePos, const Storage_SeekMode aMode )
{
  switch ( aMode )
  {
  case Storage_SMEnd:
    myStream->seekp(thePos, ios::end);
    break;
  case Storage_SMCur:
    myStream->seekp(thePos, ios::cur);
    break;
  case Storage_SMBegin:
  default:
    myStream->seekp(thePos, ios::beg);
    break;
  }

  return !myStream->fail();
}

//=======================================================================
//function : Close
//purpose  : 
//=======================================================================
Standard_Boolean Storage_OStream::Close()
{
  SetOpenMode( Storage_VSNone );

  return Standard_True;
}

//=======================================================================
//function : CanRead
//purpose  : 
//=======================================================================
Standard_Boolean Storage_OStream::CanRead() const
{
  return Standard_False;
}

//=======================================================================
//function : CanWrite
//purpose  : 
//=======================================================================
Standard_Boolean Storage_OStream::CanWrite() const
{
  return myStream->good();
}

//=======================================================================
//function : Read
//purpose  : 
//=======================================================================
Standard_Size Storage_OStream::Read( const Standard_Address /*theBuffer*/, const Standard_Size /*theSize*/ )
{
  return Standard_Size(0);
}

//=======================================================================
//function : Write
//purpose  : 
//=======================================================================
Standard_Size Storage_OStream::Write (const Standard_Address theBuffer, const Standard_Size theSize)
{
  myStream->write((char*)theBuffer, theSize);
  return theSize;
}

//=======================================================================
//function : Signature
//purpose  : 
//=======================================================================
TCollection_AsciiString Storage_OStream::Signature() const
{
  return TCollection_AsciiString();
}

//=======================================================================
//function : Print
//purpose  : 
//=======================================================================
Standard_OStream& Storage_OStream::Print (Standard_OStream& theOStream) const
{
  return theOStream;
}
