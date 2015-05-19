// File:      Storage_File.cxx
// Created:   20.05.15
// Author:    
// Copyright: Open CASCADE 2015

#include <Storage_File.ixx>

#include <TDocStd_PathParser.hxx>

#include <TCollection_AsciiString.hxx>

//=======================================================================
//function : Storage_File
//purpose  : Constructor
//=======================================================================
Storage_File::Storage_File( const TCollection_ExtendedString& theFilePath )
  : Storage_IODevice(),
    myFilePath( theFilePath )
{
}

//=======================================================================
//function : Delete
//purpose  : 
//=======================================================================
void Storage_File::Delete()
{
}

//=======================================================================
//function : Path
//purpose  : 
//=======================================================================
TCollection_ExtendedString Storage_File::Path() const
{
  return myFilePath;
}

//=======================================================================
//function : Name
//purpose  : 
//=======================================================================
TCollection_ExtendedString Storage_File::Name() const
{
  Standard_Integer aPos = -1;
  TCollection_ExtendedString name;
  TCollection_ExtendedString temp = myFilePath;
#ifdef WNT
  aPos = temp.SearchFromEnd( TCollection_ExtendedString( "\\" ) );
  if ( !( aPos > 0 ) )
#endif
    aPos = temp.SearchFromEnd( TCollection_ExtendedString( "/" ) );
  if ( aPos >0 ) 
    name = temp.Split( aPos );
  else
    name = temp;

  return name;
}

//=======================================================================
//function : Open
//purpose  : 
//=======================================================================
Storage_Error Storage_File::Open (const Storage_OpenMode theMode)
{
  Storage_Error anOpenResult = Storage_VSOk;
  
  #ifdef _WIN32
    // file name is treated as UTF-8 string and converted to UTF-16 one
  const TCollection_ExtendedString aPath( Path() );
  const wchar_t* aFilePath = (const wchar_t*)aPath.ToExtString();
  #else
  const TCollection_AsciiString aPath( Path() );
  const Standard_CString aFilePath = aPath.ToCString();
  #endif
  
  if (OpenMode() == Storage_VSNone)
  {
    if (theMode == Storage_VSRead)
    {
      myStream.open(aFilePath, ios::in | ios::binary );
    }
    else if (theMode == Storage_VSWrite)
    {
      myStream.open(aFilePath, ios::out | ios::binary );
    }
    else if (theMode == Storage_VSReadWrite)
    {
      myStream.open(aFilePath, ios::in | ios::out);
    }
    
    if (!myStream.rdbuf()->is_open()) {
      anOpenResult = Storage_VSOpenError;
    }
    else
    {
      SetOpenMode (theMode);
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
Standard_Boolean Storage_File::IsEnd() const
{
  return myStream.eof();
}

//=======================================================================
//function : Tell
//purpose  : 
//=======================================================================
Storage_Position Storage_File::Tell()
{
  return Storage_Position( OpenMode() == Storage_VSRead ? myStream.tellg() : myStream.tellp() );
}

//=======================================================================
//function : Seek
//purpose  : 
//=======================================================================
Standard_Boolean Storage_File::Seek (const Storage_Position& thePos)
{
  if ( OpenMode() == Storage_VSRead )
    myStream.seekg( thePos );
  else
    myStream.seekp( thePos );

  return !myStream.fail();
}

//=======================================================================
//function : Close
//purpose  : 
//=======================================================================
Standard_Boolean Storage_File::Close()
{
  myStream.close();

  SetOpenMode( Storage_VSNone );

  return Standard_True;
}

//=======================================================================
//function : CanRead
//purpose  : 
//=======================================================================
Standard_Boolean Storage_File::CanRead() const
{
  return myStream.good() && ( OpenMode() == Storage_VSRead || OpenMode() == Storage_VSReadWrite );
}

//=======================================================================
//function : CanWrite
//purpose  : 
//=======================================================================
Standard_Boolean Storage_File::CanWrite() const
{
  return myStream.good() && ( OpenMode() == Storage_VSWrite || OpenMode() == Storage_VSReadWrite );
}

//=======================================================================
//function : Read
//purpose  : 
//=======================================================================
Standard_Size Storage_File::Read( const Standard_Address theBuffer, const Standard_Size theSize )
{
  myStream.read((char*)theBuffer, theSize);

  return (Standard_Size)myStream.gcount();
}

//=======================================================================
//function : Write
//purpose  : 
//=======================================================================
Standard_Size Storage_File::Write (const Standard_Address theBuffer, const Standard_Size theSize)
{ 
  Storage_Position aPosBefore = Tell();
  
  myStream.write ((char*)theBuffer, theSize);
  
  Storage_Position aPosAfter = Tell();

  return aPosAfter - aPosBefore;
}

//=======================================================================
//function : Signature
//purpose  : 
//=======================================================================
TCollection_AsciiString Storage_File::Signature() const
{
  return myFilePath;
}

//=======================================================================
//function : Print
//purpose  : 
//=======================================================================
Standard_OStream& Storage_File::Print (Standard_OStream& theOStream) const
{
  return theOStream;
}
