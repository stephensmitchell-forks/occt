// Created on: 2015-12-17
// Created by: Vlad Romashko
// Copyright (c) 2007-2014 OPEN CASCADE SAS
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

#include <BinMDataStd_MeshDriver.hxx>
#include <BinObjMgt_Persistent.hxx>
#include <CDM_MessageDriver.hxx>
#include <Standard_Type.hxx>
#include <TDataStd_Mesh.hxx>
#include <TDF_Attribute.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BinMDataStd_MeshDriver,BinMDF_ADriver)

//=======================================================================
//function : BinMDataStd_MeshDriver
//purpose  : Constructor
//=======================================================================
BinMDataStd_MeshDriver::BinMDataStd_MeshDriver(const Handle(CDM_MessageDriver)& theMsgDriver)
     : BinMDF_ADriver (theMsgDriver, STANDARD_TYPE(TDataStd_Mesh)->Name())
{

}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================
Handle(TDF_Attribute) BinMDataStd_MeshDriver::NewEmpty() const
{
  return new TDataStd_Mesh();
}

//=======================================================================
//function : Paste
//purpose  : persistent -> transient (retrieve)
//=======================================================================
Standard_Boolean BinMDataStd_MeshDriver::Paste(const BinObjMgt_Persistent&  theSource,
                                               const Handle(TDF_Attribute)& theTarget,
                                               BinObjMgt_RRelocationTable&  ) const
{
  Handle(TDataStd_Mesh) mesh = Handle(TDataStd_Mesh)::DownCast(theTarget);

  Standard_Integer i;
  Standard_Real d, x, y, z;
  Standard_Integer n, n1, n2, n3, n4;
  Standard_Integer nbNodes(0), nbElements(0);
  Standard_Boolean hasUV(Standard_False);
  gp_Pnt p;

  theSource >> nbNodes;
  theSource >> nbElements;
  theSource >> hasUV;
  theSource >> d; //deflection

  // allocate the mesh
  Handle(Poly_Mesh) M = new Poly_Mesh(hasUV);

  // deflection
  M->Deflection(d);

  // read nodes
  for (i = 1; i <= nbNodes; i++)
  {
    theSource >> x;
    theSource >> y;
    theSource >> z;
    p.SetCoord(x, y, z);
    M->AddNode(p);
  }
      
  // read 2d nodes
  if (hasUV)
  {
    for (i = 1; i <= nbNodes; i++)
    {
      theSource >> x;
      theSource >> y;
      M->ChangeUVNode(i).SetCoord(x,y);
    }
  }
      
  // read triangles and quadrangles
  for (i = 1; i <= nbElements; i++)
  {
    theSource >> n;
    theSource >> n1;
    theSource >> n2;
    theSource >> n3;
    if (n == 3)
        M->AddElement(n1, n2, n3);
    else if (n == 4)
    {
        theSource >> n4;
        M->AddElement(n1, n2, n3, n4);
    }
  }

  // set mesh to Ocaf attribute
  mesh->Set(M);
  return !M.IsNull();
}

//=======================================================================
//function : Paste
//purpose  : transient -> persistent (store)
//=======================================================================
void BinMDataStd_MeshDriver::Paste(const Handle(TDF_Attribute)& theSource,
                                   BinObjMgt_Persistent&        theTarget,
                                   BinObjMgt_SRelocationTable&  ) const
{
  const Handle(TDataStd_Mesh) meshAttr = Handle(TDataStd_Mesh)::DownCast(theSource);
  const Handle(Poly_Mesh)& M = meshAttr->Get();
  if (!M.IsNull())
  {
    Standard_Integer nbNodes = M->NbNodes();
    Standard_Integer nbElements = M->NbElements();

    // write number of elements
    theTarget << nbNodes;
    theTarget << nbElements;
    theTarget << (M->HasUVNodes() ? 1 : 0);
    // write the deflection
    theTarget << M->Deflection();

    // write 3d nodes
    Standard_Integer i;
    for (i = 1; i <= nbNodes; i++)
    {
      theTarget << M->Node(i).X();
      theTarget << M->Node(i).Y();
      theTarget << M->Node(i).Z();
    }

    // write 2d nodes
    if (M->HasUVNodes())
    {
      for (i = 1; i <= nbNodes; i++)
      {
        theTarget << M->UVNode(i).X();
        theTarget << M->UVNode(i).Y();
      }
    }

    // write triangles and quadrangles
    Standard_Integer n, n1, n2, n3, n4;
    for (i = 1; i <= nbElements; i++)
    {
      M->Element(i, n1, n2, n3, n4);
      n = (n4 > 0) ? 4 : 3;
      theTarget << n;
      theTarget << n1;
      theTarget << n2;
      theTarget << n3;
      if (n4 > 0)
        theTarget << n4;
    }
  }
}
