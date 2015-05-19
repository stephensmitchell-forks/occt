// Copyright (c) 1998-1999 Matra Datavision
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

#include <FSD_BinaryFile.ixx>
#include <OSD.hxx>
#include <OSD_OpenFile.hxx>

const Standard_CString MAGICNUMBER = "BINFILE";

//=======================================================================
//function : FSD_BinaryFile
//purpose  : 
//=======================================================================

FSD_BinaryFile::FSD_BinaryFile()
{
  myHeader.testindian  = -1;
  myHeader.binfo       = -1;
  myHeader.einfo       = -1;
  myHeader.bcomment    = -1;
  myHeader.ecomment    = -1;
  myHeader.btype       = -1;
  myHeader.etype       = -1;
  myHeader.broot       = -1;
  myHeader.eroot       = -1;
  myHeader.bref        = -1;
  myHeader.eref        = -1;
  myHeader.bdata       = -1;
  myHeader.edata       = -1;
}

//=======================================================================
//function : IsGoodFileType
//purpose  : INFO SECTION
//           write
//=======================================================================

Storage_Error FSD_BinaryFile::IsGoodFileType(const Handle(Storage_IODevice)& aDevice)
{
  FSD_BinaryFile      f;
  Storage_Error s;

  s = f.Open(aDevice, Storage_VSRead);

  if (s == Storage_VSOk) {
    TCollection_AsciiString l;
    Standard_Size        len = strlen(FSD_BinaryFile::MagicNumber());

    f.ReadChar(l,len);

    f.Close();

    if (strncmp(FSD_BinaryFile::MagicNumber(),l.ToCString(),len) != 0) {
      s = Storage_VSFormatError;
    }
  }

  return s;
}

//=======================================================================
//function : Open
//purpose  : 
//=======================================================================

Storage_Error FSD_BinaryFile::Open(const Handle(Storage_IODevice)& aDevice, const Storage_OpenMode aMode)
{
  SetDevice(aDevice);

  if ( Device().IsNull() )
    return Storage_VSOpenError;

  return Device()->Open(aMode);
  
/*
  Storage_Error result = Storage_VSOk;

  SetName(aName);

  if (OpenMode() == Storage_VSNone) {
    if (aMode == Storage_VSRead) {
      myStream = OSD_OpenFile(aName.ToCString(),"rb");
    }
    else if (aMode == Storage_VSWrite) {
      myStream = OSD_OpenFile(aName.ToCString(),"wb");
    }
    else if (aMode == Storage_VSReadWrite) {
      myStream = OSD_OpenFile(aName.ToCString(),"w+b");
    }
    
    if (myStream == 0L) {
      result = Storage_VSOpenError;
    }
    else {
      SetOpenMode(aMode);
    }
  }
  else {
    result = Storage_VSAlreadyOpen;
  }

  return result;
*/
}

//=======================================================================
//function : IsEnd
//purpose  : 
//=======================================================================

Standard_Boolean FSD_BinaryFile::IsEnd()
{
  return !Device().IsNull() ? Device()->IsEnd() : Standard_True;
}

//=======================================================================
//function : Close
//purpose  : 
//=======================================================================

Storage_Error FSD_BinaryFile::Close()
{
  Storage_Error result = Storage_VSOk;

  if (OpenMode() != Storage_VSNone) {
    Device()->Close();
    //    fclose(myStream);
    //    SetOpenMode(Storage_VSNone);
  }
  else {
    result = Storage_VSNotOpen;
  }

  return result;
}

//=======================================================================
//function : MagicNumber
//purpose  : ------------------ PROTECTED
//=======================================================================

const Standard_CString FSD_BinaryFile::MagicNumber()
{
  return MAGICNUMBER;
}

//=======================================================================
//function : ReadChar
//purpose  : read <rsize> character from the current position.
//=======================================================================

void FSD_BinaryFile::ReadChar(TCollection_AsciiString& buffer, const Standard_Size rsize)
{
  char             c;
  Standard_Size ccount = 0;

  buffer.Clear();

  while (!IsEnd() && (ccount < rsize)) {
    Device()->Read(&c, sizeof(char));
    buffer += c;
    ccount++;
  }
}

//=======================================================================
//function : SkipObject
//purpose  : 
//=======================================================================

void FSD_BinaryFile::SkipObject()
{

}

//=======================================================================
//function : PutReference
//purpose  : ---------------------- PUBLIC : PUT
//=======================================================================

Storage_BaseDriver& FSD_BinaryFile::PutReference(const Standard_Integer aValue)
{
  if ( Device().IsNull() )
    Storage_StreamWriteError::Raise();

#if DO_INVERSE
  Standard_Integer t = InverseInt (aValue);
#else
  Standard_Integer t = aValue;
#endif

  if ( Device()->Write( &t, sizeof(Standard_Integer) ) != sizeof(Standard_Integer) )
    Storage_StreamWriteError::Raise();

  return *this;
}

//=======================================================================
//function : PutCharacter
//purpose  : 
//=======================================================================

Storage_BaseDriver& FSD_BinaryFile::PutCharacter(const Standard_Character aValue)
{
  if ( Device().IsNull() )
    Storage_StreamWriteError::Raise();

  if ( Device()->Write((Standard_Address)&aValue,sizeof(Standard_Character)) != sizeof(Standard_Character) )
    Storage_StreamWriteError::Raise();
  return *this;
}

//=======================================================================
//function : PutExtCharacter
//purpose  : 
//=======================================================================

Storage_BaseDriver& FSD_BinaryFile::PutExtCharacter(const Standard_ExtCharacter aValue)
{
  if ( Device().IsNull() )
    Storage_StreamWriteError::Raise();

#if DO_INVERSE
  Standard_ExtCharacter t = InverseExtChar (aValue);
#else
  Standard_ExtCharacter t = aValue;
#endif

  if ( Device()->Write(&t, sizeof(Standard_ExtCharacter)) != sizeof(Standard_ExtCharacter) )
    Storage_StreamWriteError::Raise();

  return *this;
}

//=======================================================================
//function : PutInteger
//purpose  : 
//=======================================================================

Storage_BaseDriver& FSD_BinaryFile::PutInteger(const Standard_Integer aValue)
{
  if ( Device().IsNull() )
    Storage_StreamWriteError::Raise();

#if DO_INVERSE
  Standard_Integer t = InverseInt(aValue);
#else
  Standard_Integer t = aValue;
#endif
  
  if ( Device()->Write(&t,sizeof(Standard_Integer)) != sizeof(Standard_Integer))
    Storage_StreamWriteError::Raise();

  return *this;
}

//=======================================================================
//function : PutBoolean
//purpose  : 
//=======================================================================

Storage_BaseDriver& FSD_BinaryFile::PutBoolean(const Standard_Boolean aValue)
{
  if ( Device().IsNull() )
    Storage_StreamWriteError::Raise();

#if DO_INVERSE
  Standard_Integer t = InverseInt ((Standard_Integer) aValue);
#else
  Standard_Integer t = (Standard_Integer)aValue;
#endif
  
  if ( Device()->Write(&t,sizeof(Standard_Integer)) != sizeof(Standard_Integer) )
    Storage_StreamWriteError::Raise();

  return *this;
}

//=======================================================================
//function : PutReal
//purpose  : 
//=======================================================================

Storage_BaseDriver& FSD_BinaryFile::PutReal(const Standard_Real aValue)
{
  if ( Device().IsNull() )
    Storage_StreamWriteError::Raise();

#if DO_INVERSE
  Standard_Real t = InverseReal (aValue);
#else
  Standard_Real t = aValue;
#endif
  
  if ( Device()->Write(&t,sizeof(Standard_Real)) != sizeof(Standard_Real) )
    Storage_StreamWriteError::Raise();

  return *this;
}

//=======================================================================
//function : PutShortReal
//purpose  : 
//=======================================================================

Storage_BaseDriver& FSD_BinaryFile::PutShortReal(const Standard_ShortReal aValue)
{
  if ( Device().IsNull() )
    Storage_StreamWriteError::Raise();

#if DO_INVERSE
  Standard_ShortReal t = InverseShortReal (aValue);
#else
  Standard_ShortReal t = aValue;
#endif

  if ( Device()->Write(&t,sizeof(Standard_ShortReal)) != sizeof(Standard_ShortReal) )
    Storage_StreamWriteError::Raise();

  return *this;
}

//=======================================================================
//function : GetReference
//purpose  : ----------------- PUBLIC : GET
//=======================================================================

Storage_BaseDriver& FSD_BinaryFile::GetReference(Standard_Integer& aValue)
{
  if ( Device().IsNull() )
    Storage_StreamReadError::Raise();

  if ( Device()->Read(&aValue,sizeof(Standard_Integer)) != sizeof(Standard_Integer) )
    Storage_StreamTypeMismatchError::Raise();
#if DO_INVERSE
  aValue = InverseInt (aValue);
#endif
  return *this;
}

//=======================================================================
//function : GetCharacter
//purpose  : 
//=======================================================================

Storage_BaseDriver& FSD_BinaryFile::GetCharacter(Standard_Character& aValue)
{
  if ( Device().IsNull() )
    Storage_StreamReadError::Raise();

  if ( Device()->Read(&aValue,sizeof(Standard_Character)) != sizeof(Standard_Character) )
    Storage_StreamTypeMismatchError::Raise();

  return *this;
}

//=======================================================================
//function : GetExtCharacter
//purpose  : 
//=======================================================================

Storage_BaseDriver& FSD_BinaryFile::GetExtCharacter(Standard_ExtCharacter& aValue)
{
  if ( Device().IsNull() )
    Storage_StreamReadError::Raise();

  if ( Device()->Read(&aValue,sizeof(Standard_ExtCharacter)) != sizeof(Standard_ExtCharacter) )
    Storage_StreamTypeMismatchError::Raise();
#if DO_INVERSE
  aValue = InverseExtChar (aValue);
#endif
  return *this;
}

//=======================================================================
//function : GetInteger
//purpose  : 
//=======================================================================

Storage_BaseDriver& FSD_BinaryFile::GetInteger(Standard_Integer& aValue)
{
  if ( Device().IsNull() )
    Storage_StreamReadError::Raise();

  if ( Device()->Read(&aValue,sizeof(Standard_Integer)) != sizeof(Standard_Integer) )
    Storage_StreamTypeMismatchError::Raise();
#if DO_INVERSE
  aValue = InverseInt (aValue);
#endif
  return *this;
}

//=======================================================================
//function : GetBoolean
//purpose  : 
//=======================================================================

Storage_BaseDriver& FSD_BinaryFile::GetBoolean(Standard_Boolean& aValue)
{
  if ( Device().IsNull() )
    Storage_StreamReadError::Raise();

  if ( Device()->Read(&aValue,sizeof(Standard_Boolean)) != sizeof(Standard_Boolean) )
    Storage_StreamTypeMismatchError::Raise();
#if DO_INVERSE
  aValue = InverseInt ((Standard_Integer) aValue);
#endif
  return *this;
}

//=======================================================================
//function : GetReal
//purpose  : 
//=======================================================================

Storage_BaseDriver& FSD_BinaryFile::GetReal(Standard_Real& aValue)
{
  if ( Device().IsNull() )
    Storage_StreamReadError::Raise();

  if ( Device()->Read(&aValue,sizeof(Standard_Real)) != sizeof(Standard_Real))
    Storage_StreamTypeMismatchError::Raise();
#if DO_INVERSE
  aValue = InverseReal (aValue);
#endif
  return *this;
}

//=======================================================================
//function : GetShortReal
//purpose  : 
//=======================================================================

Storage_BaseDriver& FSD_BinaryFile::GetShortReal(Standard_ShortReal& aValue)
{
  if ( Device().IsNull() )
    Storage_StreamReadError::Raise();

  if ( Device()->Read(&aValue,sizeof(Standard_ShortReal)) != sizeof(Standard_ShortReal))
    Storage_StreamTypeMismatchError::Raise();
#if DO_INVERSE
  aValue = InverseShortReal (aValue);
#endif
  return *this;
}

//=======================================================================
//function : Destroy
//purpose  : 
//=======================================================================

void FSD_BinaryFile::Destroy()
{
  if (OpenMode() != Storage_VSNone) {
    Close();
  }
}

//=======================================================================
//function : BeginWriteInfoSection
//purpose  : -------------------------- INFO : WRITE
//=======================================================================

Storage_Error FSD_BinaryFile::BeginWriteInfoSection() 
{
  char ti[4];
  ti[0] = 1;
  ti[1] = 2;
  ti[2] = 3;
  ti[3] = 4;
  myHeader.testindian = *((int*)ti);
  Standard_Size aLen = strlen(FSD_BinaryFile::MagicNumber());
  if ( Device()->Write( (Standard_Address)FSD_BinaryFile::MagicNumber(), aLen ) != aLen )
    Storage_StreamWriteError::Raise();
  
  myHeader.binfo = Device()->Tell();
  WriteHeader();

  return Storage_VSOk;
}

//=======================================================================
//function : WriteInfo
//purpose  : 
//=======================================================================

void FSD_BinaryFile::WriteInfo(const Standard_Integer nbObj,
			 const TCollection_AsciiString& dbVersion,
			 const TCollection_AsciiString& date,
			 const TCollection_AsciiString& schemaName,
			 const TCollection_AsciiString& schemaVersion,
			 const TCollection_ExtendedString& appName,
			 const TCollection_AsciiString& appVersion,
			 const TCollection_ExtendedString& dataType,
			 const TColStd_SequenceOfAsciiString& userInfo) 
{
  Standard_Integer i;

  PutInteger(nbObj);
  WriteString(dbVersion);
  WriteString(date);
  WriteString(schemaName);
  WriteString(schemaVersion);
  WriteExtendedString(appName);
  WriteString(appVersion);
  WriteExtendedString(dataType);
  i = userInfo.Length();

  PutInteger(i);
  for (i = 1; i <= userInfo.Length(); i++) {
    WriteString(userInfo.Value(i));
  }
}

//=======================================================================
//function : EndWriteInfoSection
//purpose  : read
//=======================================================================

Storage_Error FSD_BinaryFile::EndWriteInfoSection() 
{
  myHeader.einfo = Device()->Tell();

  return Storage_VSOk;
}

//=======================================================================
//function : BeginReadInfoSection
//purpose  : 
//=======================================================================

Storage_Error FSD_BinaryFile::BeginReadInfoSection() 
{
  Storage_Error s = Storage_VSOk;
  TCollection_AsciiString l;
  Standard_Size        len = strlen(FSD_BinaryFile::MagicNumber());

  ReadChar(l,len);

  if (strncmp(FSD_BinaryFile::MagicNumber(),l.ToCString(),len) != 0) {
    s = Storage_VSFormatError;
  }
  else {
    ReadHeader();
  }

  return s;
}

//=======================================================================
//function : ReadInfo
//purpose  : ------------------- INFO : READ
//=======================================================================

void FSD_BinaryFile::ReadInfo(Standard_Integer& nbObj,
			TCollection_AsciiString& dbVersion,
			TCollection_AsciiString& date,
			TCollection_AsciiString& schemaName,
			TCollection_AsciiString& schemaVersion,
			TCollection_ExtendedString& appName,
			TCollection_AsciiString& appVersion,
			TCollection_ExtendedString& dataType,
			TColStd_SequenceOfAsciiString& userInfo) 
{
  GetInteger(nbObj);
  ReadString(dbVersion);
  ReadString(date);
  ReadString(schemaName);
  ReadString(schemaVersion);
  ReadExtendedString(appName);
  ReadString(appVersion);
  ReadExtendedString(dataType);

  Standard_Integer i,len = 0;

  GetInteger(len);
  TCollection_AsciiString line;

  for (i = 1; i <= len && !IsEnd(); i++) {
    ReadString(line);
    userInfo.Append(line);
  }
}

//=======================================================================
//function : EndReadInfoSection
//purpose  : COMMENTS SECTION
//           write
//=======================================================================

Storage_Error FSD_BinaryFile::EndReadInfoSection() 
{
  if ( Device()->Seek(myHeader.einfo))
    return Storage_VSOk;
  else
    return Storage_VSSectionNotFound;
}

//=======================================================================
//function : BeginWriteCommentSection
//purpose  : ---------------- COMMENTS : WRITE
//=======================================================================

Storage_Error FSD_BinaryFile::BeginWriteCommentSection() 
{
  myHeader.bcomment = Device()->Tell();
  return Storage_VSOk;
}

//=======================================================================
//function : WriteComment
//purpose  : 
//=======================================================================

void FSD_BinaryFile::WriteComment(const TColStd_SequenceOfExtendedString& aCom)
{
 Standard_Integer i,aSize;

 aSize = aCom.Length();
 PutInteger(aSize);
 for (i = 1; i <= aSize; i++) {
   WriteExtendedString(aCom.Value(i));
 }
}

//=======================================================================
//function : EndWriteCommentSection
//purpose  : read
//=======================================================================

Storage_Error FSD_BinaryFile::EndWriteCommentSection() 
{
  myHeader.ecomment = Device()->Tell();

  return Storage_VSOk;
}

//=======================================================================
//function : BeginReadCommentSection
//purpose  : ---------------- COMMENTS : READ
//=======================================================================

Storage_Error FSD_BinaryFile::BeginReadCommentSection() 
{
  if ( Device()->Seek(myHeader.bcomment) )
    return Storage_VSOk;
  else
    return Storage_VSSectionNotFound;
}

//=======================================================================
//function : ReadComment
//purpose  : 
//=======================================================================

void FSD_BinaryFile::ReadComment(TColStd_SequenceOfExtendedString& aCom)
{
  TCollection_ExtendedString line;
  Standard_Integer           len,i;

  GetInteger(len);
  for (i = 1; i <= len && !IsEnd(); i++) {
    ReadExtendedString(line);
    aCom.Append(line);
  }
}

//=======================================================================
//function : EndReadCommentSection
//purpose  : 
//=======================================================================

Storage_Error FSD_BinaryFile::EndReadCommentSection() 
{
  if ( Device()->Seek(myHeader.ecomment) )
    return Storage_VSOk;
  else
    return Storage_VSSectionNotFound;
}

//=======================================================================
//function : BeginWriteTypeSection
//purpose  : --------------- TYPE : WRITE
//=======================================================================

Storage_Error FSD_BinaryFile::BeginWriteTypeSection() 
{
  myHeader.btype = Device()->Tell();

  return Storage_VSOk;
}

//=======================================================================
//function : SetTypeSectionSize
//purpose  : 
//=======================================================================

void FSD_BinaryFile::SetTypeSectionSize(const Standard_Integer aSize) 
{
  PutInteger(aSize);
}

//=======================================================================
//function : WriteTypeInformations
//purpose  : 
//=======================================================================

void FSD_BinaryFile::WriteTypeInformations(const Standard_Integer typeNum,
				      const TCollection_AsciiString& typeName) 
{
  PutInteger(typeNum);
  WriteString(typeName);
}

//=======================================================================
//function : EndWriteTypeSection
//purpose  : read
//=======================================================================

Storage_Error FSD_BinaryFile::EndWriteTypeSection() 
{
  myHeader.etype = Device()->Tell();

  return Storage_VSOk;
}

//=======================================================================
//function : BeginReadTypeSection
//purpose  : ------------------- TYPE : READ
//=======================================================================

Storage_Error FSD_BinaryFile::BeginReadTypeSection() 
{
  if ( Device()->Seek(myHeader.btype) )
    return Storage_VSOk;
  else
    return Storage_VSSectionNotFound;
}

//=======================================================================
//function : TypeSectionSize
//purpose  : 
//=======================================================================

Standard_Integer FSD_BinaryFile::TypeSectionSize() 
{
  Standard_Integer i;

  GetInteger(i);
  return i;
}

//=======================================================================
//function : ReadTypeInformations
//purpose  : 
//=======================================================================

void FSD_BinaryFile::ReadTypeInformations(Standard_Integer& typeNum,TCollection_AsciiString& typeName) 
{
  GetInteger(typeNum);
  ReadString(typeName);
}

//=======================================================================
//function : EndReadTypeSection
//purpose  : ROOT SECTION
//           write
//=======================================================================

Storage_Error FSD_BinaryFile::EndReadTypeSection() 
{
  if ( Device()->Seek(myHeader.etype) )
    return Storage_VSOk;
  else
    return Storage_VSSectionNotFound;
}

//=======================================================================
//function : BeginWriteRootSection
//purpose  : -------------------- ROOT : WRITE
//=======================================================================

Storage_Error FSD_BinaryFile::BeginWriteRootSection() 
{
  myHeader.broot = Device()->Tell();

  return Storage_VSOk;
}

//=======================================================================
//function : SetRootSectionSize
//purpose  : 
//=======================================================================

void FSD_BinaryFile::SetRootSectionSize(const Standard_Integer aSize) 
{
  PutInteger(aSize);
}

//=======================================================================
//function : WriteRoot
//purpose  : 
//=======================================================================

void FSD_BinaryFile::WriteRoot(const TCollection_AsciiString& rootName, const Standard_Integer aRef, const TCollection_AsciiString& rootType) 
{
  PutReference(aRef);
  WriteString(rootName);
  WriteString(rootType);
}

//=======================================================================
//function : EndWriteRootSection
//purpose  : read
//=======================================================================

Storage_Error FSD_BinaryFile::EndWriteRootSection() 
{
  myHeader.eroot = Device()->Tell();

  return Storage_VSOk;
}

//=======================================================================
//function : BeginReadRootSection
//purpose  : ----------------------- ROOT : READ
//=======================================================================

Storage_Error FSD_BinaryFile::BeginReadRootSection() 
{
  if ( Device()->Seek(myHeader.broot) )
    return Storage_VSOk;
  else
    return Storage_VSSectionNotFound;
}

//=======================================================================
//function : RootSectionSize
//purpose  : 
//=======================================================================

Standard_Integer FSD_BinaryFile::RootSectionSize() 
{
  Standard_Integer i;
  
  GetInteger(i);
  return i;
}

//=======================================================================
//function : ReadRoot
//purpose  : 
//=======================================================================

void FSD_BinaryFile::ReadRoot(TCollection_AsciiString& rootName, Standard_Integer& aRef,TCollection_AsciiString& rootType) 
{
  GetReference(aRef);
  ReadString(rootName);
  ReadString(rootType);
}

//=======================================================================
//function : EndReadRootSection
//purpose  : REF SECTION
//           write
//=======================================================================

Storage_Error FSD_BinaryFile::EndReadRootSection() 
{
  if ( Device()->Seek(myHeader.eroot) )
    return Storage_VSOk;
  else
    return Storage_VSSectionNotFound;
}

//=======================================================================
//function : BeginWriteRefSection
//purpose  : -------------------------- REF : WRITE
//=======================================================================

Storage_Error FSD_BinaryFile::BeginWriteRefSection() 
{
  myHeader.bref = Device()->Tell();

  return Storage_VSOk;
}

//=======================================================================
//function : SetRefSectionSize
//purpose  : 
//=======================================================================

void FSD_BinaryFile::SetRefSectionSize(const Standard_Integer aSize) 
{
  PutInteger(aSize);
}

//=======================================================================
//function : WriteReferenceType
//purpose  : 
//=======================================================================

void FSD_BinaryFile::WriteReferenceType(const Standard_Integer reference,const Standard_Integer typeNum) 
{
  PutReference(reference);
  PutInteger(typeNum);
}

//=======================================================================
//function : EndWriteRefSection
//purpose  : read
//=======================================================================

Storage_Error FSD_BinaryFile::EndWriteRefSection() 
{
  myHeader.eref = Device()->Tell();

  return Storage_VSOk;
}

//=======================================================================
//function : BeginReadRefSection
//purpose  : ----------------------- REF : READ
//=======================================================================

Storage_Error FSD_BinaryFile::BeginReadRefSection() 
{
  if ( Device()->Seek(myHeader.bref) )
    return Storage_VSOk;
  else
    return Storage_VSSectionNotFound;
}

//=======================================================================
//function : RefSectionSize
//purpose  : 
//=======================================================================

Standard_Integer FSD_BinaryFile::RefSectionSize() 
{
  Standard_Integer i;

  GetInteger(i);
  return i;
}

//=======================================================================
//function : ReadReferenceType
//purpose  : 
//=======================================================================

void FSD_BinaryFile::ReadReferenceType(Standard_Integer& reference,
				 Standard_Integer& typeNum) 
{
  GetReference(reference);
  GetInteger(typeNum);
}

//=======================================================================
//function : EndReadRefSection
//purpose  : DATA SECTION
//           write
//=======================================================================

Storage_Error FSD_BinaryFile::EndReadRefSection() 
{
  if ( Device()->Seek(myHeader.eref) )
    return Storage_VSOk;
  else
    return Storage_VSSectionNotFound;
}

//=======================================================================
//function : BeginWriteDataSection
//purpose  : -------------------- DATA : WRITE
//=======================================================================

Storage_Error FSD_BinaryFile::BeginWriteDataSection() 
{
  myHeader.bdata = Device()->Tell();

  return Storage_VSOk;
}

//=======================================================================
//function : WritePersistentObjectHeader
//purpose  : 
//=======================================================================

void FSD_BinaryFile::WritePersistentObjectHeader(const Standard_Integer aRef,
					   const Standard_Integer aType) 
{
  PutReference(aRef);
  PutInteger(aType);
}

//=======================================================================
//function : BeginWritePersistentObjectData
//purpose  : 
//=======================================================================

void FSD_BinaryFile::BeginWritePersistentObjectData() 
{
}

//=======================================================================
//function : BeginWriteObjectData
//purpose  : 
//=======================================================================

void FSD_BinaryFile::BeginWriteObjectData() 
{
}

//=======================================================================
//function : EndWriteObjectData
//purpose  : 
//=======================================================================

void FSD_BinaryFile::EndWriteObjectData() 
{
}

//=======================================================================
//function : EndWritePersistentObjectData
//purpose  : 
//=======================================================================

void FSD_BinaryFile::EndWritePersistentObjectData() 
{
}

//=======================================================================
//function : EndWriteDataSection
//purpose  : read
//=======================================================================

Storage_Error FSD_BinaryFile::EndWriteDataSection() 
{
  myHeader.edata = Device()->Tell();
  
  Device()->Seek(myHeader.binfo);
  WriteHeader();
  return Storage_VSOk;
}

//=======================================================================
//function : BeginReadDataSection
//purpose  : ---------------------- DATA : READ
//=======================================================================

Storage_Error FSD_BinaryFile::BeginReadDataSection() 
{
  if ( Device()->Seek(myHeader.bdata) )
    return Storage_VSOk;
  else
    return Storage_VSSectionNotFound;
}

//=======================================================================
//function : ReadPersistentObjectHeader
//purpose  : 
//=======================================================================

void FSD_BinaryFile::ReadPersistentObjectHeader(Standard_Integer& aRef,
					  Standard_Integer& aType) 
{
  GetReference(aRef);
  GetInteger(aType);
}

//=======================================================================
//function : BeginReadPersistentObjectData
//purpose  : 
//=======================================================================

void FSD_BinaryFile::BeginReadPersistentObjectData() 
{
}

//=======================================================================
//function : BeginReadObjectData
//purpose  : 
//=======================================================================

void FSD_BinaryFile::BeginReadObjectData() 
{
}

//=======================================================================
//function : EndReadObjectData
//purpose  : 
//=======================================================================

void FSD_BinaryFile::EndReadObjectData() 
{
}

//=======================================================================
//function : EndReadPersistentObjectData
//purpose  : 
//=======================================================================

void FSD_BinaryFile::EndReadPersistentObjectData() 
{
}

//=======================================================================
//function : EndReadDataSection
//purpose  : 
//=======================================================================

Storage_Error FSD_BinaryFile::EndReadDataSection() 
{
  if ( Device()->Seek(myHeader.edata) )
    return Storage_VSOk;
  else
    return Storage_VSSectionNotFound;
}

//=======================================================================
//function : WriteString
//purpose  : write string at the current position.
//=======================================================================

void FSD_BinaryFile::WriteString(const TCollection_AsciiString& aString)
{
  Standard_Integer size;

  size = aString.Length();

  PutInteger(size);

  if (size > 0) {
    if ( Device().IsNull() || Device()->Write((Standard_Address)aString.ToCString(),size) != size )
      Storage_StreamWriteError::Raise();
  }
}

//=======================================================================
//function : ReadString
//purpose  : read string from the current position.
//=======================================================================

void FSD_BinaryFile::ReadString(TCollection_AsciiString& aString)
{
  Standard_Integer size = 0;

  GetInteger(size);
  if (size > 0) {
    Standard_Character *c = (Standard_Character *)Standard::Allocate((size+1) * sizeof(Standard_Character));
    if ( Device().IsNull() || Device()->Read(c,size) != size )
      Storage_StreamReadError::Raise();
    c[size] = '\0';
    aString = c;
    Standard::Free(c);
  }
  else {
    aString.Clear();
  }
}

//=======================================================================
//function : WriteExtendedString
//purpose  : write string at the current position.
//=======================================================================

void FSD_BinaryFile::WriteExtendedString(const TCollection_ExtendedString& aString)
{
  Standard_Integer size;

  size = aString.Length();

  PutInteger(size);

  if (size > 0) {
    Standard_ExtString anExtStr;
#if DO_INVERSE
    TCollection_ExtendedString aCopy = aString;
    anExtStr = aCopy.ToExtString();

    Standard_PExtCharacter pChar;
    //
    pChar=(Standard_PExtCharacter)anExtStr;
    
    for (Standard_Integer i=0; i < size; i++)
      pChar[i] = InverseExtChar (pChar[i]);
#else
    anExtStr = aString.ToExtString();
#endif
    Standard_Size aSize = sizeof(Standard_ExtCharacter)*aString.Length();
    if ( Device().IsNull() || Device()->Write((Standard_Address)anExtStr,aSize) != aSize )
      Storage_StreamWriteError::Raise();
  }
}

//=======================================================================
//function : ReadExtendedString
//purpose  : read string from the current position.
//=======================================================================

void FSD_BinaryFile::ReadExtendedString(TCollection_ExtendedString& aString)
{
  Standard_Integer size = 0;

  GetInteger(size);
  if (size > 0) {
    Standard_ExtCharacter *c = (Standard_ExtCharacter *)
      Standard::Allocate((size+1) * sizeof(Standard_ExtCharacter));
    if ( Device().IsNull() || Device()->Read(c,size*sizeof(Standard_ExtCharacter)) != size*sizeof(Standard_ExtCharacter) )
      Storage_StreamWriteError::Raise();
    c[size] = '\0';
#if DO_INVERSE
    for (Standard_Integer i=0; i < size; i++)
      c[i] = InverseExtChar (c[i]);
#endif
    aString = c;
    Standard::Free(c);
  }
  else {
    aString.Clear();
  }
}

//=======================================================================
//function : WriteHeader
//purpose  : 
//=======================================================================

void FSD_BinaryFile::WriteHeader()
{
  PutInteger(myHeader.testindian);
  PutInteger(myHeader.binfo);
  PutInteger(myHeader.einfo);
  PutInteger(myHeader.bcomment);
  PutInteger(myHeader.ecomment);
  PutInteger(myHeader.btype);
  PutInteger(myHeader.etype);
  PutInteger(myHeader.broot);
  PutInteger(myHeader.eroot);
  PutInteger(myHeader.bref);
  PutInteger(myHeader.eref);
  PutInteger(myHeader.bdata);
  PutInteger(myHeader.edata);
}

//=======================================================================
//function : ReadHeader
//purpose  : 
//=======================================================================

void FSD_BinaryFile::ReadHeader()
{
  GetInteger(myHeader.testindian);
  GetInteger(myHeader.binfo);
  GetInteger(myHeader.einfo);
  GetInteger(myHeader.bcomment);
  GetInteger(myHeader.ecomment);
  GetInteger(myHeader.btype);
  GetInteger(myHeader.etype);
  GetInteger(myHeader.broot);
  GetInteger(myHeader.eroot);
  GetInteger(myHeader.bref);
  GetInteger(myHeader.eref);
  GetInteger(myHeader.bdata);
  GetInteger(myHeader.edata);
}


//=======================================================================
//function : Tell
//purpose  : return position in the file. Return -1 upon error.
//=======================================================================

Storage_Position FSD_BinaryFile::Tell()
{
  return Device()->Tell();
}
