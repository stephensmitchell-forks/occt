// File:      Storage_OStream.cxx
// Created:   20.05.15
// Author:    
// Copyright: Open CASCADE 2015

#include <Storage_IStream.ixx>

#include <TCollection_AsciiString.hxx>
#include <TCollection_ExtendedString.hxx>

//=======================================================================
//function : Storage_IStream
//purpose  : Constructor
//=======================================================================
Storage_IStream::Storage_IStream (Standard_IStream& theStream)
  : Storage_IODevice(),
    myStream (&theStream)
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
//function : Path
//purpose  : 
//=======================================================================
Standard_IStreamPtr Storage_IStream::Stream() const
{
  return myStream;
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
    if (!myStream->good()) // not good for eof
    {
      anOpenResult = Storage_VSOpenError;
    }
    else
    {
      SetOpenMode (theMode);
      
      // clear flags and set the position where the next character is to be inserted 
      myStream->clear();
      Seek(0);
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
  return myStream->eof();
}

//=======================================================================
//function : Tell
//purpose  : 
//=======================================================================
Storage_Position Storage_IStream::Tell()
{
  return Storage_Position (myStream->tellg());
}

//=======================================================================
//function : Seek
//purpose  : 
//=======================================================================
Standard_Boolean Storage_IStream::Seek (const Storage_Position& thePos, const Storage_SeekMode aMode )
{
  switch ( aMode )
  {
  case Storage_SMEnd:
    myStream->seekg(thePos, ios::end);
    break;
  case Storage_SMCur:
    myStream->seekg(thePos, ios::cur);
    break;
  case Storage_SMBegin:
  default:
    myStream->seekg(thePos, ios::beg);
    break;
  }

  return !myStream->fail();
}

//=======================================================================
//function : Close
//purpose  : 
//=======================================================================
Standard_Boolean Storage_IStream::Close()
{
  SetOpenMode( Storage_VSNone );
  myStream->clear();
  Seek(0);

  return Standard_True;
}

//=======================================================================
//function : CanRead
//purpose  : 
//=======================================================================
Standard_Boolean Storage_IStream::CanRead() const
{
  return myStream->good();
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
Standard_Size Storage_IStream::Read (const Standard_Address theBuffer, const Standard_Size theSize)
{
  myStream->read((char*)theBuffer, theSize);
  return (Standard_Size)myStream->gcount();
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
  return TCollection_AsciiString( (Standard_Integer)myStream );
}

//=======================================================================
//function : Print
//purpose  : 
//=======================================================================
Standard_OStream& Storage_IStream::Print (Standard_OStream& theOStream) const
{
  return theOStream;
}
