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

#include <ObjDataWriter.h>

#include <FaceIterator.h>
#include <CafShapePrs.h>

#include <BRep_Tool.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepLProp_SLProps.hxx>
#include <Message.hxx>
#include <Message_Messenger.hxx>
#include <Message_ProgressSentry.hxx>
#include <OSD_OpenFile.hxx>
#include <Poly_Array1OfTriangle.hxx>
#include <Poly_Triangulation.hxx>
#include <Standard_CLocaleSentry.hxx>
#include <TDataStd_Name.hxx>
#include <TopoDS.hxx>

IMPLEMENT_STANDARD_RTTIEXT(ObjDataWriter, Standard_Transient)

//! Auxiliary class to write OBJ.
class ObjWriter
{

public:

  //! Main constructor.
  ObjWriter (const TCollection_AsciiString& theName,
             const Standard_Boolean         theHasNormals)
  : myFile (OSD_OpenFile (theName.ToCString(), "wb")),
    myName (theName),
    myHasNormals (theHasNormals == Standard_True)
  {
    if (myFile == NULL)
    {
      Message::DefaultMessenger()->Send (TCollection_AsciiString ("File can not be created!\n") + theName, Message_Fail);
      return;
    }
  }

  //! Destructor, will emit error message if file was not closed.
  ~ObjWriter()
  {
    if (myFile != NULL)
    {
      ::fclose (myFile);
      Message::DefaultMessenger()->Send (TCollection_AsciiString ("File can not be written!\n") + myName, Message_Fail);
    }
  }

  //! Return true if file has been opened.
  bool IsOpened() const { return myFile != NULL; }

  //! Correctly close the file.
  void Close()
  {
    ::fclose (myFile);
    myFile = NULL;
  }

  //! Write the header.
  bool WriteHeader (const Standard_Integer theNbNodes,
                    const Standard_Integer theNbElems)
  {
    return ::Fprintf (myFile, "# Exported by OpenCASCADE CAD Assistant [www.opencascade.com]\n"
                              "#  Vertices: %d\n"
                              "#     Faces: %d\n", theNbNodes, theNbElems) != 0;
  }

  //! Writing a triangle
  bool WriteTriangle (const Graphic3d_Vec4i& theTri)
  {
    if (myHasNormals)
    {
      return Fprintf (myFile, "f %d//%d %d//%d %d//%d\n",
                      theTri[0] + 1, theTri[0] + 1,
                      theTri[1] + 1, theTri[1] + 1,
                      theTri[2] + 1, theTri[2] + 1) != 0;
    }
    return Fprintf (myFile, "f %d %d %d\n",
                    theTri[0] + 1,
                    theTri[1] + 1,
                    theTri[2] + 1) != 0;
  }

  //! Writing a quad
  bool WriteQuad (const Graphic3d_Vec4i& theQuad)
  {
    if (myHasNormals)
    {
      return Fprintf (myFile, "f %d//%d %d//%d %d//%d %d//%d\n",
                      theQuad[0] + 1, theQuad[0] + 1,
                      theQuad[1] + 1, theQuad[1] + 1,
                      theQuad[2] + 1, theQuad[2] + 1,
                      theQuad[3] + 1, theQuad[3] + 1) != 0;
    }
    return Fprintf (myFile, "f %d %d %d %d\n",
                    theQuad[0] + 1,
                    theQuad[1] + 1,
                    theQuad[2] + 1,
                    theQuad[3] + 1) != 0;
  }

  //! Writing a vector
  bool WriteVertex (const Graphic3d_Vec3& theValue)
  {
    return Fprintf (myFile, "v %f %f %f\n",  theValue.x(), theValue.y(), theValue.z()) != 0;
  }

  //! Writing a vector
  bool WriteNormal (const Graphic3d_Vec3& theValue)
  {
    return Fprintf (myFile, "vn %f %f %f\n", theValue.x(), theValue.y(), theValue.z()) != 0;
  }

  //! Writing a group name
  bool WriteGroup (const TCollection_AsciiString& theValue)
  {
    return Fprintf (myFile, "g %s\n", theValue.ToCString()) != 0;
  }

private:

  FILE* myFile;
  TCollection_AsciiString myName;
  bool myHasNormals;
  bool myIsOpened;

};

//================================================================
// Function : Constructor
// Purpose  :
//================================================================
ObjDataWriter::ObjDataWriter()
{
  //
}

//================================================================
// Function : Write
// Purpose  :
//================================================================
Standard_Boolean ObjDataWriter::Write (const Handle(MeshDataSource)&            theDataSrc,
                                       const TCollection_AsciiString&           theFile,
                                       const Handle(Message_ProgressIndicator)& /*theProgress*/)
{
  if (theDataSrc.IsNull())
  {
    return Standard_False;
  }

  // header
  /*NCollection_IndexedMap<TCollection_AsciiString> aComments;
  for (NCollection_Sequence<TCollection_AsciiString>::Iterator aCommentIter (theDataSrc->FileComments()); aCommentIter.More(); aCommentIter.Next())
  {
    aComments.Add (aCommentIter.Value());
  }
  aComments.Add ("Exported by OpenCASCADE CAD Assistant [www.opencascade.com]");*/

  Standard_CLocaleSentry aLocaleSentry;
  ObjWriter anObjFile (theFile, theDataSrc->HasNormals());
  if (!anObjFile.IsOpened()
   || !anObjFile.WriteHeader (theDataSrc->NbNodes(), theDataSrc->NbElements()))
  {
    return Standard_False;
  }

  for (Standard_Integer aNodeIter = 0; aNodeIter < theDataSrc->NbNodes(); ++aNodeIter)
  {
    const Graphic3d_Vec3& aNode = theDataSrc->Nodes().Value (aNodeIter);
    if (!anObjFile.WriteVertex (aNode))
    {
      return Standard_False;
    }
  }
  if (theDataSrc->HasNormals())
  {
    for (Standard_Integer aNodeIter = 0; aNodeIter < theDataSrc->NbNodes(); ++aNodeIter)
    {
      const Graphic3d_Vec3& aNormal = theDataSrc->Normals().Value (aNodeIter);
      if (!anObjFile.WriteNormal (aNormal))
      {
        return Standard_False;
      }
    }
  }

  for (Standard_Integer anElemIter = 0; anElemIter < theDataSrc->NbElements(); ++anElemIter)
  {
    const Graphic3d_Vec4i& anElem = theDataSrc->Elements().Value (anElemIter);
    if (anElem[3] == -1)
    {
      if (!anObjFile.WriteTriangle (anElem))
      {
        return Standard_False;
      }
    }
    else
    {
      if (!anObjFile.WriteQuad (anElem))
      {
        return Standard_False;
      }
    }
  }

  anObjFile.Close();
  return Standard_True;
}

//! Trivial convertor
inline Graphic3d_Vec3 objXyzToVec (const gp_XYZ& thePnt)
{
  return Graphic3d_Vec3 ((float )thePnt.X(), (float )thePnt.Y(), (float )thePnt.Z());
}

//================================================================
// Function : Write
// Purpose  :
//================================================================
Standard_Boolean ObjDataWriter::Write (const AIS_ListOfInteractive&             thePrsList,
                                       const TCollection_AsciiString&           theFile,
                                       const Handle(Message_ProgressIndicator)& /*theProgress*/)
{
  if (thePrsList.IsEmpty())
  {
    return Standard_False;
  }

  Standard_Integer aNbNodesAll = 0;
  Standard_Integer aNbElemsAll = 0;
  for (FaceIterator aFaceIter (thePrsList); aFaceIter.More(); aFaceIter.Next())
  {
    aNbNodesAll += aFaceIter.Triangulation->Nodes().Length();
    aNbElemsAll += aFaceIter.Triangulation->Triangles().Length();
  }

  if (aNbNodesAll == 0
   || aNbElemsAll == 0)
  {
    Message::DefaultMessenger()->Send (TCollection_AsciiString ("No mesh data to save!\n"), Message_Fail);
    return Standard_False;
  }

  Standard_CLocaleSentry aLocaleSentry;
  ObjWriter anObjFile (theFile, Standard_True);
  if (!anObjFile.IsOpened()
   || !anObjFile.WriteHeader (aNbNodesAll, aNbElemsAll))
  {
    return Standard_False;
  }

  // write vertices
  BRepAdaptor_Surface aFaceAdaptor;
  BRepLProp_SLProps   aSLTool (1, 1e-12);
  gp_Dir              aNormal;
  Standard_Integer aFirstNode = 0;
  Graphic3d_Vec4i  aTriNodes (-1, -1, -1, -1);
  Standard_Integer aNbFaces = 0;
  TCollection_AsciiString aPrevGroup;
  for (FaceIterator aFaceIter (thePrsList); aFaceIter.More(); aFaceIter.Next())
  {
    ++aNbFaces;
    const Standard_Integer aLower     = aFaceIter.Triangulation->Triangles().Lower();
    const Standard_Boolean isMirrored = aFaceIter.Trsf.VectorialPart().Determinant() < 0.0;
    const Handle(Poly_Triangulation)& aTriangulation = aFaceIter.Triangulation;

    TCollection_AsciiString aRefName;
    Handle(TDataStd_Name) aNodeName;
    if (aFaceIter.ShapePrs->GetLabel().FindAttribute (TDataStd_Name::GetID(), aNodeName))
    {
      aRefName = TCollection_AsciiString (aNodeName->Get());
    }
    if (!aRefName.IsEmpty()
     && !aRefName.IsEqual (aPrevGroup)
     && !anObjFile.WriteGroup (aRefName))
    {
      return Standard_False;
    }

    // write nodes
    const TColgp_Array1OfPnt& aNodes = aTriangulation->Nodes();
    const gp_Trsf aTrsf = aFaceIter.Trsf;
    for (Standard_Integer aNodeIter = aNodes.Lower(); aNodeIter <= aNodes.Upper(); ++aNodeIter)
    {
      gp_Pnt aNode = aNodes (aNodeIter);
      aNode.Transform (aTrsf);
      if (!anObjFile.WriteVertex (objXyzToVec (aNode.XYZ())))
      {
        return Standard_False;
      }
    }

    // write normals
    TopoDS_Face aFace = TopoDS::Face (aFaceIter.Face.Oriented (TopAbs_FORWARD));
    aFace.Location (TopLoc_Location());
    aFaceAdaptor.Initialize (aFace, Standard_False);
    aSLTool.SetSurface (aFaceAdaptor);
    const TColgp_Array1OfPnt2d& aNodeUVs = aTriangulation->UVNodes();
    for (Standard_Integer aNodeIter = aNodes.Lower(); aNodeIter <= aNodes.Upper(); ++aNodeIter)
    {
      const gp_XY& anUV = aNodeUVs.Value (aNodeIter).XY();
      aSLTool.SetParameters (anUV.X(), anUV.Y());
      gp_Dir aNormal ((aSLTool.IsNormalDefined() ? aSLTool.Normal() : gp::DZ()).XYZ());
      aNormal.Transform (aTrsf);
      if (aFaceIter.Face.Orientation() == TopAbs_REVERSED)
      {
        aNormal.Reverse();
      }
      if (!anObjFile.WriteNormal (objXyzToVec (aNormal.XYZ())))
      {
        return Standard_False;
      }
    }

    // write indices
    for (Poly_Array1OfTriangle::Iterator anElemIter (aFaceIter.Triangulation->Triangles()); anElemIter.More(); anElemIter.Next())
    {
      if ((aFaceIter.Face.Orientation() == TopAbs_REVERSED) ^ isMirrored)
      {
        anElemIter.Value().Get (aTriNodes[0], aTriNodes[2], aTriNodes[1]);
      }
      else
      {
        anElemIter.Value().Get (aTriNodes[0], aTriNodes[1], aTriNodes[2]);
      }
      aTriNodes[0] = aFirstNode + aTriNodes[0] - aLower;
      aTriNodes[1] = aFirstNode + aTriNodes[1] - aLower;
      aTriNodes[2] = aFirstNode + aTriNodes[2] - aLower;
      if (!anObjFile.WriteTriangle (aTriNodes))
      {
        return Standard_False;
      }
    }
    aFirstNode += aFaceIter.Triangulation->NbNodes();
  }

  anObjFile.Close();
  return Standard_True;
}
