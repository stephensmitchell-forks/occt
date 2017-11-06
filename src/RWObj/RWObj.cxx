// Copyright (c) 2017 OPEN CASCADE SAS
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

#include <RWObj.hxx>

#include <NCollection_Vector.hxx>
#include <RWObj_Reader.hxx>

namespace
{
  //! RWObj_Reader implementation dumping OBJ file into single Poly_Triangulation.
  class Reader : public RWObj_Reader
  {
  public:

    //! Do not create any sub-meshes.
    virtual void addSubMesh (const RWObj_SubMesh& ,
                             const RWObj_SubMeshReason ) Standard_OVERRIDE {}

    //! Retrieve sub-mesh node position.
    virtual Graphic3d_Vec3 getSubMeshNode (Standard_Integer theIndex) const Standard_OVERRIDE
    {
      return myNodes.Value (theIndex - 1);
    }

    //! Add new node.
    virtual Standard_Integer addSubMeshNode (const Graphic3d_Vec3& thePnt) Standard_OVERRIDE
    {
      myNodes.Append (thePnt);
      return myNodes.Size();
    }

    //! Ignore normal.
    virtual Standard_Integer addSubMeshNormal (const Graphic3d_Vec3& ) Standard_OVERRIDE { return 0; }

    //! Ignore texture coordinates.
    virtual Standard_Integer addSubMeshNodeUV (const Graphic3d_Vec2& ) Standard_OVERRIDE { return 0; }

    //! Add element.
    virtual void addSubMeshElement (Standard_Integer theN1, Standard_Integer theN2, Standard_Integer theN3, Standard_Integer theN4) Standard_OVERRIDE
    {
      myTriangles.Append (Poly_Triangle (theN1, theN2, theN3));
      if (theN4 != -1)
      {
        myTriangles.Append (Poly_Triangle (theN1, theN3, theN4));
      }
    }

    //! Creates Poly_Triangulation from collected data
    Handle(Poly_Triangulation) GetTriangulation()
    {
      if (myTriangles.IsEmpty())
      {
        return Handle(Poly_Triangulation)();
      }

      Handle(Poly_Triangulation) aPoly = new Poly_Triangulation (myNodes.Length(), myTriangles.Length(), Standard_False);
      for (Standard_Integer aNodeIter = 0; aNodeIter < myNodes.Size(); ++aNodeIter)
      {
        const Graphic3d_Vec3& aNode = myNodes (aNodeIter);
        aPoly->ChangeNode (aNodeIter + 1).SetCoord (aNode.x(), aNode.y(), aNode.z());
      }

      for (Standard_Integer aTriIter = 0; aTriIter < myTriangles.Size(); ++aTriIter)
      {
        aPoly->ChangeTriangle (aTriIter + 1) = myTriangles (aTriIter);
      }

      return aPoly;
    }

  private:
    NCollection_Vector<Graphic3d_Vec3> myNodes;
    NCollection_Vector<Poly_Triangle> myTriangles;
  };

}

//=============================================================================
//function : Read
//purpose  :
//=============================================================================
Handle(Poly_Triangulation) RWObj::ReadFile (const Standard_CString theFile,
                                            const Handle(Message_ProgressIndicator)& theProgress)
{
  Reader aReader;
  aReader.Read (theFile, theProgress);
  // note that returned bool value is ignored intentionally -- even if something went wrong,
  // but some data have been read, we at least will return these data
  return aReader.GetTriangulation();
}
