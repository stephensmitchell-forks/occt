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

#include <ObjDataSource.h>

#include <Standard_CLocaleSentry.hxx>
#include <Message.hxx>
#include <Message_Messenger.hxx>
#include <Message_ProgressSentry.hxx>
#include <OSD_OpenFile.hxx>
#include <OSD_Timer.hxx>
#include <TCollection_ExtendedString.hxx>
#include <Quantity_Color.hxx>

IMPLEMENT_STANDARD_RTTIEXT(ObjDataSource, MeshDataSource)

//! Simple wrapper.
struct FileSentry
{
  FILE* File;

  FileSentry (const TCollection_AsciiString& theFile)
  : File (OSD_OpenFile (theFile.ToCString(), "rb")) {}

  ~FileSentry()
  {
    if (File != NULL)
    {
      ::fclose (File);
    }
  }
};

//! Read 3 float values.
inline bool objReadVec3 (const char*     thePos,
                         char*&          theNext,
                         Graphic3d_Vec3& theVec)
{
  const char* aPos = thePos;
  theVec.x() = (float )Strtod (aPos, &theNext);
  aPos = theNext;
  theVec.y() = (float )Strtod (aPos, &theNext);
  aPos = theNext;
  theVec.z() = (float )Strtod (aPos, &theNext);
  return aPos != theNext;
}

//! Read string.
inline bool objReadName (const char*              thePos,
                         TCollection_AsciiString& theName)
{
  TCollection_AsciiString aName = TCollection_AsciiString (thePos);
  Standard_Integer aTail = aName.Length();
  if (aTail > 0
   && aName.Value (aTail) == '\n')
  {
    aName.SetValue (aTail--, '\0'); // replace '\n'
  }
  if (aTail > 0
   && aName.Value (aTail) == '\r')
  {
    aName.SetValue (aTail--, '\0'); // replace '\r'
  }
  theName = aName.ToCString();
  theName.LeftAdjust();
  theName.RightAdjust();
  return !theName.IsEmpty();
}

//! Reader of mtl files.
class ObjMaterialReader
{
public:

  //! Main constructor.
  ObjMaterialReader (NCollection_DataMap<TCollection_AsciiString, MeshMaterial>& theMaterials)
  : myFile (NULL), myMaterials (&theMaterials), myNbLines (0) {}

  //! Destructor.
  ~ObjMaterialReader()
  {
    if (myFile != NULL)
    {
      ::fclose (myFile);
    }
  }

  //! Read the file.
  Standard_Boolean Read (const TCollection_AsciiString& theFolder,
                         const TCollection_AsciiString& theFile)
  {
    myPath = theFolder + theFile;
    myFile = OSD_OpenFile (myPath.ToCString(), "rb");
    if (myFile == NULL)
    {
      Message::DefaultMessenger()->Send (TCollection_AsciiString ("OBJ material file '") + myPath + "' is not found!", Message_Warning);
      return Standard_False;
    }

    char aLine[256] = {};
    TCollection_AsciiString aMatName;
    MeshMaterial aMat;
    const Standard_Integer aNbMatOld = myMaterials->Extent();
    bool hasAspect = false;
    for (; ::feof (myFile) == 0 && ::fgets (aLine, 255, myFile) != NULL; )
    {
      ++myNbLines;

      const char* aPos = aLine;
      if (*aLine == '#'
       || *aLine == '\n'
       || *aLine == '\0')
      {
        continue;
      }

      if (::memcmp (aLine, "newmtl ", 7) == 0)
      {
        aPos += 7;
        if (!aMatName.IsEmpty())
        {
          if (hasAspect)
          {
            aMat.Aspect.SetMaterialType (Graphic3d_MATERIAL_PHYSIC);
            aMat.Aspect.SetMaterialName (aMatName.ToCString());
          }
          else
          {
            // reset incomplete material definition
            aMat.Aspect = Graphic3d_MaterialAspect();
          }
          myMaterials->Bind (aMatName, aMat);
          hasAspect = false;
        }

        aMatName = TCollection_AsciiString(aPos);
        aMat     = MeshMaterial();
        if (!objReadName (aPos, aMatName))
        {
          Message::DefaultMessenger()->Send (TCollection_AsciiString("Empty OBJ material at line ") + myNbLines + " in file " + myPath, Message_Warning);
        }
      }
      else if (::memcmp (aLine, "Ka ", 3) == 0)
      {
        aPos += 3;
        char* aNext = NULL;
        Graphic3d_Vec3 aColor;
        objReadVec3 (aPos, aNext, aColor);
        aPos = aNext;
        if (validateColor (aColor))
        {
          Quantity_Color aQColor (aColor.r(), aColor.g(), aColor.b(), Quantity_TOC_RGB);
          aMat.Aspect.SetAmbientColor (aQColor);
          hasAspect = true;
        }
      }
      else if (::memcmp (aLine, "Kd ", 3) == 0)
      {
        aPos += 3;
        char* aNext = NULL;
        Graphic3d_Vec3 aColor;
        objReadVec3 (aPos, aNext, aColor);
        aPos = aNext;
        if (validateColor (aColor))
        {
          Quantity_Color aQColor (aColor.r(), aColor.g(), aColor.b(), Quantity_TOC_RGB);
          aMat.Aspect.SetDiffuseColor (aQColor);
          hasAspect = true;
        }
      }
      else if (::memcmp (aLine, "Ks ", 3) == 0)
      {
        aPos += 3;
        char* aNext = NULL;
        Graphic3d_Vec3 aColor;
        objReadVec3 (aPos, aNext, aColor);
        aPos = aNext;
        if (validateColor (aColor))
        {
          Quantity_Color aQColor (aColor.r(), aColor.g(), aColor.b(), Quantity_TOC_RGB);
          aMat.Aspect.SetSpecularColor (aQColor);
        }
      }
      else if (::memcmp (aLine, "Ns ", 3) == 0)
      {
        aPos += 3;
        char* aNext = NULL;
        Standard_Real aSpecular = Strtod (aPos, &aNext);
        aPos = aNext;
        if (aSpecular >= 0.0
         && aSpecular <= 128.0)
        {
          aMat.Aspect.SetShininess (aSpecular / 128.0);
        }
      }
      else if (::memcmp (aLine, "Tr ", 3) == 0)
      {
        aPos += 3;
        char* aNext = NULL;
        Standard_Real anAlpha = Strtod (aPos, &aNext);
        aPos = aNext;
        if (validateScalar (anAlpha))
        {
          //aMat.Aspect.SetTransparency (anAlpha);
        }
      }
      else if (::memcmp (aLine, "map_Kd ", 7) == 0)
      {
        aPos += 7;
        if (!objReadName (aPos, aMat.Texture))
        {
          //
        }
        else
        {
          aMat.Texture = theFolder + aMat.Texture;
        }
      }
      /*else if (::memcmp (aLine, "illum ", 6) == 0)
      {
        aPos += 6;
        char* aNext = NULL;
        const int aModel = strtol (aPos, &aNext, 10);
        aPos = aNext;
        if (aModel < 0 || aModel > 10)
        {
          // unknown model
        }
      }*/
    }

    if (!aMatName.IsEmpty())
    {
      if (hasAspect)
      {
        aMat.Aspect.SetMaterialType (Graphic3d_MATERIAL_PHYSIC);
        aMat.Aspect.SetMaterialName (aMatName.ToCString());
      }
      else
      {
        // reset incomplete material definition
        aMat.Aspect = Graphic3d_MaterialAspect();
      }
      myMaterials->Bind (aMatName, aMat);
    }
    return myMaterials->Extent() != aNbMatOld;
  }

private:

  //! Validate scalar value
  inline bool validateScalar (const Standard_Real theValue)
  {
    if (theValue < 0.0
     || theValue > 1.0)
    {
      Message::DefaultMessenger()->Send (TCollection_AsciiString("Invalid scalar in OBJ material at line ") + myNbLines + " in file " + myPath, Message_Warning);
      return false;
    }
    return true;
  }

  //! Validate RGB color
  inline bool validateColor (const Graphic3d_Vec3& theVec)
  {
    if (theVec.r() < 0.0 || theVec.r() > 1.0
     || theVec.g() < 0.0 || theVec.g() > 1.0
     || theVec.b() < 0.0 || theVec.b() > 1.0)
    {
      Message::DefaultMessenger()->Send (TCollection_AsciiString("Invalid color in OBJ material at line ") + myNbLines + " in file " + myPath, Message_Warning);
      return false;
    }
    return true;
  }

private:

  FILE* myFile;
  TCollection_AsciiString myPath;
  NCollection_DataMap<TCollection_AsciiString, MeshMaterial>* myMaterials;
  Standard_Integer myNbLines;

};

//================================================================
// Function : Constructor
// Purpose  :
//================================================================
ObjDataSource::ObjDataSource()
{
  //
}

//! Hasher for 3 ordered integers.
class  ObjVec3iHasher
{
public:

  static Standard_Integer HashCode (const Graphic3d_Vec3i& theKey,
                                    const Standard_Integer theUpper)
  {
    return ::HashCode (::HashCodes ((Standard_CString )&theKey, sizeof(Graphic3d_Vec3i)), theUpper);
  }

  static Standard_Boolean IsEqual (const Graphic3d_Vec3i& theKey1,
                                   const Graphic3d_Vec3i& theKey2)
  {
    return theKey1[0] == theKey2[0]
        && theKey1[1] == theKey2[1]
        && theKey1[2] == theKey2[2];
  }

};

//================================================================
// Function : Read
// Purpose  :
//================================================================
Standard_Boolean ObjDataSource::Read (const TCollection_AsciiString&           theFile,
                                      const Handle(Message_ProgressIndicator)& theProgress,
                                      const Standard_Integer                   /*theIndexLimit*/,
                                      const Standard_Integer                   theMemLimitMiB)
{
  Clear();

  Standard_CLocaleSentry aLocaleSentry;
  FileSentry aFile (theFile);
  if (aFile.File == NULL)
  {
    Message::DefaultMessenger()->Send (TCollection_AsciiString ("File '") + theFile + "' is not found!", Message_Fail);
    return Standard_False;
  }

  // determine file location to load associated files
  TCollection_AsciiString aFolder;
  {
    OSD_Path aPath (theFile);
    aPath.SetName ("");
    aPath.SetExtension ("");
    aPath.SystemName (aFolder);
  }

  // determine length of file
  ::fseek64 (aFile.File, 0, SEEK_END);
  const int64_t aFileLen = ::ftell64 (aFile.File);
  if (aFileLen <= 0L)
  {
    Message::DefaultMessenger()->Send (TCollection_AsciiString ("File '") + theFile + "' is empty!", Message_Fail);
    return Standard_False;
  }
  ::fseek64 (aFile.File, 0, SEEK_SET);

  const Standard_Integer aNbMiBTotal  = Standard_Integer(aFileLen / (1024 * 1024));
  Standard_Integer       aNbMiBPassed = 0;
  Message_ProgressSentry aPSentry (theProgress, "Reading text OBJ file", 0, aNbMiBTotal, 1);
  OSD_Timer aTimer;
  aTimer.Start();

  // Each node in the Element specifies independent indices of Vertex position, Texture coordinates and Normal.
  // This scheme does not match natural definition of Primitive Array where each unique set of nodal properties defines Vertex
  // (thus node at the same location but with different normal should be duplicated).
  // The following code converts OBJ definition of nodal properties to Primitive Array definition.
  NCollection_Vector<Graphic3d_Vec3> aVerts;
  NCollection_Vector<Graphic3d_Vec2> aVertParams;
  NCollection_Vector<Graphic3d_Vec3> aNorms;
  NCollection_DataMap<Graphic3d_Vec3i, Standard_Integer, ObjVec3iHasher> aPackedIndices;
  NCollection_DataMap<TCollection_AsciiString, MeshMaterial> aMaterials;
  TCollection_AsciiString anActiveMat;
  MeshGroup aGroup;

  const size_t aMemLimitBytes = theMemLimitMiB == -1
                              ?  size_t(-1)
                              : (theMemLimitMiB * 1024 * 1024);
  size_t aMemEstim = 0;

  char aLine[256] = {};
  Standard_Integer aNbLines = 0;
  bool isStart = true;
  for (; ::feof (aFile.File) == 0 && ::fgets (aLine, 255, aFile.File) != NULL; )
  {
    ++aNbLines;
    if (aTimer.ElapsedTime() > 1.0)
    {
      if (!aPSentry.More())
      {
        return Standard_False;
      }

      const Standard_Integer aNbMiBRead = Standard_Integer(::ftell64 (aFile.File) / (1024 * 1024));
      for (; aNbMiBPassed < aNbMiBRead; ++aNbMiBPassed) { aPSentry.Next(); }
      aTimer.Reset();
      aTimer.Start();
    }

    const char* aPos = aLine;
    if (*aLine == '#')
    {
      if (isStart)
      {
        TCollection_AsciiString aComment (aLine + 1);
        aComment.LeftAdjust();
        aComment.RightAdjust();
        if (!aComment.IsEmpty())
        {
          myComments.Append (aComment);
        }
      }
      continue;
    }
    else if (*aLine == '\n'
          || *aLine == '\0')
    {

      continue;
    }
    isStart = false;

    if (::memcmp (aLine, "v ", 2) == 0)
    {
      aPos += 2;
      char* aNext = NULL;
      Graphic3d_Vec3 aVert;
      objReadVec3 (aPos, aNext, aVert);
      aPos = aNext;

      aMemEstim += sizeof(Graphic3d_Vec3);
      if (aMemEstim >= aMemLimitBytes)
      {
        Message::DefaultMessenger()->Send (TCollection_AsciiString("OBJ file contains more than ") + myElements.Size() + " elements and " + aVerts.Size() + " vertices"
                                         + "\nwhich does not fit into " + theMemLimitMiB + " MiB limit."
                                         + "\nMesh data will be truncated!", Message_Fail);
        return Standard_False;
      }

      aVerts.Append (aVert);
    }
    else if (::memcmp (aLine, "vn ", 3) == 0)
    {
      aPos += 3;
      char* aNext = NULL;
      Graphic3d_Vec3 aNorm;
      objReadVec3 (aPos, aNext, aNorm);
      aPos = aNext;

      aMemEstim += sizeof(Graphic3d_Vec3);
      if (aMemEstim >= aMemLimitBytes)
      {
        Message::DefaultMessenger()->Send (TCollection_AsciiString("OBJ file contains more than ") + myElements.Size() + " elements and " + aVerts.Size() + " vertices"
                                         + "\nwhich does not fit into " + theMemLimitMiB + " MiB limit."
                                         + "\nMesh data will be truncated!", Message_Fail);
        return Standard_False;
      }

      aNorms.Append (aNorm);
    }
    else if (::memcmp (aLine, "vt ", 3) == 0)
    {
      aPos += 3;
      char* aNext = NULL;
      Graphic3d_Vec2 anUV;
      anUV.x() = (float )Strtod (aPos, &aNext);
      aPos = aNext;
      anUV.y() = (float )Strtod (aPos, &aNext);
      aPos = aNext;

      aMemEstim += sizeof(Graphic3d_Vec2);
      if (aMemEstim >= aMemLimitBytes)
      {
        Message::DefaultMessenger()->Send (TCollection_AsciiString("OBJ file contains more than ") + myElements.Size() + " elements and " + aVerts.Size() + " vertices"
                                         + "\nwhich does not fit into " + theMemLimitMiB + " MiB limit."
                                         + "\nMesh data will be truncated!", Message_Fail);
        return Standard_False;
      }

      aVertParams.Append (anUV);
    }
    else if (::memcmp (aLine, "f ", 2) == 0)
    {
      aPos += 2;
      char* aNext = NULL;

      Graphic3d_Vec4i aTriNodes (-1, -1, -1, -1);
      for (int aNode = 0; aNode < 4; ++aNode)
      {
        Graphic3d_Vec3i a3Indices (-1, -1, -1);
        a3Indices[0] = strtol (aPos, &aNext, 10) - 1;
        if (aNext == aPos)
        {
          continue;
        }

        // parse UV index
        aPos = aNext;
        if (*aPos == '/')
        {
          ++aPos;
          a3Indices[1] = strtol (aPos, &aNext, 10) - 1;
          aPos = aNext;

          // parse Normal index
          if (*aPos == '/')
          {
            ++aPos;
            a3Indices[2] = strtol (aPos, &aNext, 10) - 1;
            aPos = aNext;
          }
        }
        if (*aPos != ' '
         && *aPos != '\n'
         && *aPos != '\0')
        {
          ++aPos;
        }

        // handle negative indices
        if (a3Indices[0] < -1)
        {
          a3Indices[0] += aVerts.Upper() + 2;
        }
        if (a3Indices[1] < -1)
        {
          a3Indices[1] += aVertParams.Upper() + 2;
        }
        if (a3Indices[2] < -1)
        {
          a3Indices[2] += aNorms.Upper() + 2;
        }

        Standard_Integer anIndex = -1;
        if (!aPackedIndices.Find (a3Indices, anIndex))
        {
          if (a3Indices[0] >= 0)
          {
            aMemEstim += sizeof(Graphic3d_Vec3);
          }
          if (a3Indices[1] >= 0)
          {
            aMemEstim += sizeof(Graphic3d_Vec2);
          }
          if (a3Indices[2] >= 0)
          {
            aMemEstim += sizeof(Graphic3d_Vec3);
          }
          aMemEstim += sizeof(Graphic3d_Vec4i) + sizeof(Standard_Integer); // naive map
          if (aMemEstim >= aMemLimitBytes)
          {
            Message::DefaultMessenger()->Send (TCollection_AsciiString("OBJ file contains more than ") + myElements.Size() + " elements and " + aVerts.Size() + " vertices"
                                             + "\nwhich does not fit into " + theMemLimitMiB + " MiB limit."
                                             + "\nMesh data will be truncated!", Message_Fail);
            return Standard_False;
          }

          if (a3Indices[0] < aVerts.Lower() || a3Indices[0] > aVerts.Upper())
          {
            Message::DefaultMessenger()->Send (TCollection_AsciiString("Invalid OBJ syntax at line ") + aNbLines, Message_Fail);
            return Standard_False;
          }
          AddNodePosition (aVerts.Value (a3Indices[0]));
          aGroup.NodeUpper = myNodes.Upper();
          if (aGroup.NodeLower < 0)
          {
            aGroup.NodeLower = aGroup.NodeUpper;
          }
          if (a3Indices[1] >= 0)
          {
            if (a3Indices[1] < aVertParams.Lower() || a3Indices[1] > aVertParams.Upper())
            {
              Message::DefaultMessenger()->Send (TCollection_AsciiString("Invalid OBJ syntax at line ") + aNbLines, Message_Fail);
              return Standard_False;
            }
            AddNodeUV (aVertParams.Value (a3Indices[1]));
          }
          if (a3Indices[2] >= 0)
          {
            if (a3Indices[2] < aNorms.Lower() || a3Indices[2] > aNorms.Upper())
            {
              Message::DefaultMessenger()->Send (TCollection_AsciiString("Invalid OBJ syntax at line ") + aNbLines, Message_Fail);
              return Standard_False;
            }
            AddNodeNormal (aNorms.Value (a3Indices[2]));
          }
          anIndex = myNodes.Upper();
          aPackedIndices.Bind (a3Indices, anIndex);
        }
        aTriNodes[aNode] = anIndex;
      }

      if (aTriNodes[0] < 0
       || aTriNodes[1] < 0
       || aTriNodes[2] < 0)
      {
        continue;
      }

      aMemEstim += sizeof(Graphic3d_Vec4i);
      if (aMemEstim >= aMemLimitBytes)
      {
        Message::DefaultMessenger()->Send (TCollection_AsciiString("OBJ file contains more than ") + myElements.Size() + " elements and " + aVerts.Size() + " vertices"
                                         + "\nwhich does not fit into " + theMemLimitMiB + " MiB limit."
                                         + "\nMesh data will be truncated!", Message_Fail);
        return Standard_False;
      }

      AddElement (aTriNodes);
      aGroup.ElemUpper = myElements.Upper();
      if (aGroup.ElemLower < 0)
      {
        aGroup.ElemLower = aGroup.ElemUpper;
        if (!anActiveMat.IsEmpty())
        {
          aMaterials.Find (anActiveMat, aGroup.Material);
        }
      }
    }
    else if (::memcmp (aLine, "g ", 2) == 0)
    {
      if (aGroup.ElemLower >= 0
       && aGroup.ElemLower != aGroup.ElemUpper)
      {
        myGroups.Append (aGroup);
        aGroup = MeshGroup();
        aPackedIndices.Clear(); // vertices might be duplicated after this...
      }

      TCollection_AsciiString aGroupName;
      if (!objReadName (aPos + 2, aGroupName))
      {
        Message::DefaultMessenger()->Send (TCollection_AsciiString("Invalid OBJ syntax at line ") + aNbLines, Message_Warning);
        continue;
      }
    }
    else if (::memcmp (aLine, "mtllib ", 7) == 0)
    {
      TCollection_AsciiString aMatPath;
      if (!objReadName (aPos + 7, aMatPath))
      {
        Message::DefaultMessenger()->Send (TCollection_AsciiString("Invalid OBJ syntax at line ") + aNbLines, Message_Warning);
        continue;
      }

      ObjMaterialReader aMatReader (aMaterials);
      aMatReader.Read (aFolder, aMatPath);
    }
    else if (::memcmp (aLine, "usemtl ", 7) == 0)
    {
      TCollection_AsciiString aMatName;
      if (!objReadName (aPos + 7, aMatName))
      {
        Message::DefaultMessenger()->Send (TCollection_AsciiString("Invalid OBJ syntax at line ") + aNbLines, Message_Warning);
        continue;
      }
      else if (!aMaterials.IsBound (aMatName))
      {
        Message::DefaultMessenger()->Send (TCollection_AsciiString("Use of undefined OBJ material at line ") + aNbLines, Message_Warning);
        continue;
      }

      anActiveMat = aMatName;
    }
  }

  if (aGroup.ElemLower >= 0
   && aGroup.ElemLower != aGroup.ElemUpper)
  {
    myGroups.Append (aGroup);
  }

  for (; aNbMiBPassed < aNbMiBTotal; ++aNbMiBPassed) { aPSentry.Next(); }
  return Standard_True;
}
