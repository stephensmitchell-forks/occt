// Created on: 2008-03-20
// Created by: Alexander GRIGORIEV
// Copyright (c) 2008-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and / or modify it
// under the terms of the GNU Lesser General Public version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#include <NIS_Surface.hxx>
#include <NIS_SurfaceDrawer.hxx>
#include <NIS_Triangulated.hxx>
#include <BRepMesh_IncrementalMesh.hxx>
#include <BRep_ListIteratorOfListOfCurveRepresentation.hxx>
#include <BRep_PolygonOnTriangulation.hxx>
#include <BRep_TEdge.hxx>
#include <BRep_Tool.hxx>
#include <Geom_Surface.hxx>
#include <NCollection_Map.hxx>
#include <Poly_PolygonOnTriangulation.hxx>
#include <Poly_Triangulation.hxx>
#include <Precision.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopLoc_Location.hxx>
#include <TopTools_MapOfShape.hxx>
#include <TopTools_DataMapOfShapeInteger.hxx>
#include <TopTools_DataMapIteratorOfDataMapOfShapeInteger.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Edge.hxx>
#include <TShort_Array1OfShortReal.hxx>
#include <gp_Ax1.hxx>

IMPLEMENT_STANDARD_HANDLE (NIS_Surface, NIS_InteractiveObject)
IMPLEMENT_STANDARD_RTTIEXT(NIS_Surface, NIS_InteractiveObject)

//=======================================================================
//function : IsEqual
//purpose  : Compare two triangulations, for NCollection_Map interface.
//=======================================================================

inline Standard_Boolean IsEqual(const Handle_Poly_Triangulation& theT0,
                                const Handle_Poly_Triangulation& theT1)
{
  return (theT0 == theT1);
}

//=======================================================================
//function : NIS_Surface
//purpose  : 
//=======================================================================

NIS_Surface::NIS_Surface(const Handle_NCollection_BaseAllocator& theAlloc)
  : myAlloc             (theAlloc),
    mypNodes            (NULL),
    mypNormals          (NULL),
    mypTriangles        (NULL),
    myNNodes            (0),
    myNTriangles        (0),
    myIsWireframe       (Standard_False),
    myHilightIsWireframe(Standard_False),
    myIsShowFreeBoundary(Standard_False),
    myIsShowEdges       (Standard_True)
{
  if (myAlloc.IsNull())
    myAlloc = NCollection_BaseAllocator::CommonBaseAllocator();  
}

//=======================================================================
//function : NIS_Surface
//purpose  : 
//=======================================================================

NIS_Surface::NIS_Surface (const Handle(Poly_Triangulation)&       theTri,
                          const Handle_NCollection_BaseAllocator& theAlloc)
  : myAlloc             (theAlloc),
    mypNodes            (NULL),
    mypNormals          (NULL),
    myNNodes            (0),
    myNTriangles        (0),
    myIsWireframe       (Standard_False),
    myHilightIsWireframe(Standard_False),
    myIsShowFreeBoundary(Standard_False),
    myIsShowEdges       (Standard_True)
{
  if (myAlloc.IsNull())
    myAlloc = NCollection_BaseAllocator::CommonBaseAllocator();
  if (theTri.IsNull() == Standard_False)
  {
    // Alocate arrays of entities
    myNNodes = 3 * theTri->NbTriangles();
    myNTriangles = theTri->NbTriangles();
    mypNodes = static_cast<Standard_ShortReal*>
      (myAlloc->Allocate(sizeof(Standard_ShortReal) * 3 * myNNodes));
    mypNormals = static_cast<Standard_ShortReal *>
      (myAlloc->Allocate(sizeof(Standard_ShortReal) * 3 * myNNodes));
    mypTriangles = static_cast<Standard_Integer*>
      (myAlloc->Allocate(sizeof(Standard_Integer) * 3 * myNTriangles));

    // Copy the data from the original triangulation.
    Standard_Integer i, iN(0), iT(0);
    const Poly_Array1OfTriangle& arrTri = theTri->Triangles();
    const TColgp_Array1OfPnt& arrNodes = theTri->Nodes();
    for (i = arrTri.Lower(); i <= arrTri.Upper(); i++) {
      Standard_Integer iNode[3];
      arrTri(i).Get(iNode[0], iNode[1], iNode[2]);
      gp_XYZ aNorm = ((arrNodes(iNode[1]).XYZ() - arrNodes(iNode[0]).XYZ()) ^
                      (arrNodes(iNode[2]).XYZ() - arrNodes(iNode[0]).XYZ()));
      const Standard_Real aMagn = aNorm.Modulus();
      if (aMagn > Precision::Confusion())
        aNorm /= aMagn;
      else
        aNorm.SetCoord(0., 0., 1.);
      mypNodes[iN+0] = static_cast<Standard_ShortReal>(arrNodes(iNode[0]).X());
      mypNodes[iN+1] = static_cast<Standard_ShortReal>(arrNodes(iNode[0]).Y());
      mypNodes[iN+2] = static_cast<Standard_ShortReal>(arrNodes(iNode[0]).Z());
      mypNodes[iN+3] = static_cast<Standard_ShortReal>(arrNodes(iNode[1]).X());
      mypNodes[iN+4] = static_cast<Standard_ShortReal>(arrNodes(iNode[1]).Y());
      mypNodes[iN+5] = static_cast<Standard_ShortReal>(arrNodes(iNode[1]).Z());
      mypNodes[iN+6] = static_cast<Standard_ShortReal>(arrNodes(iNode[2]).X());
      mypNodes[iN+7] = static_cast<Standard_ShortReal>(arrNodes(iNode[2]).Y());
      mypNodes[iN+8] = static_cast<Standard_ShortReal>(arrNodes(iNode[2]).Z());
      mypNormals[iN+0] = static_cast<Standard_ShortReal>(aNorm.X());
      mypNormals[iN+1] = static_cast<Standard_ShortReal>(aNorm.Y());
      mypNormals[iN+2] = static_cast<Standard_ShortReal>(aNorm.Z());
      mypNormals[iN+3] = static_cast<Standard_ShortReal>(aNorm.X());
      mypNormals[iN+4] = static_cast<Standard_ShortReal>(aNorm.Y());
      mypNormals[iN+5] = static_cast<Standard_ShortReal>(aNorm.Z());
      mypNormals[iN+6] = static_cast<Standard_ShortReal>(aNorm.X());
      mypNormals[iN+7] = static_cast<Standard_ShortReal>(aNorm.Y());
      mypNormals[iN+8] = static_cast<Standard_ShortReal>(aNorm.Z());
      mypTriangles[iT+0] = iT+0;
      mypTriangles[iT+1] = iT+1;
      mypTriangles[iT+2] = iT+2;
      iN += 9;
      iT += 3;
    }
  }
}

//=======================================================================
//function : NIS_Surface
//purpose  : Constructor
//=======================================================================

NIS_Surface::NIS_Surface (const TopoDS_Shape&                theShape,
                          const Standard_Real                theDeflection,
                          const Standard_Boolean             theIsFreeBoundary,
                          const Handle_NCollection_BaseAllocator& theAlloc)
  : myAlloc             (theAlloc),
    mypNodes            (NULL),
    mypNormals          (NULL),
    mypTriangles        (NULL),
    myNNodes            (0),
    myNTriangles        (0),
    myIsWireframe       (Standard_False),
    myHilightIsWireframe(Standard_False),
    myIsShowFreeBoundary(theIsFreeBoundary),
    myIsShowEdges       (Standard_True)
{
  if (myAlloc.IsNull())
    myAlloc = NCollection_BaseAllocator::CommonBaseAllocator();
  Init (theShape, theDeflection);
}

//=======================================================================
//function : Init
//purpose  : Initialize the instance with a TopoDS_Shape.
//=======================================================================

void NIS_Surface::Init (const TopoDS_Shape& theShape,
                        const Standard_Real theDeflection)
{
  TopLoc_Location  aLoc, aLocSurf;
  Clear();

  //Upon initialization by TopoDS_Shape the check of triangulation is performed.
  //If at least one face has no triangulation but has a surface then the whole
  // shape is triangulated using BRepMesh. 
  //The whole shape triangulation is required to respect boundary connection
  Standard_Boolean hasTri = Standard_True;
  TopExp_Explorer eexp;
  TopExp_Explorer fexp (theShape, TopAbs_FACE);
  for (; fexp.More() && hasTri; fexp.Next())
  {
    const TopoDS_Face& aFace = TopoDS::Face(fexp.Current());

    const Handle(Poly_Triangulation)& aTriangulation
      = BRep_Tool::Triangulation (aFace, aLoc);
    const Handle(Geom_Surface)& aSurface = BRep_Tool::Surface(aFace, aLoc);

    if ((aTriangulation.IsNull() && Standard_False == aSurface.IsNull()) ||
        (Standard_False == aTriangulation.IsNull() &&
            theDeflection < aTriangulation->Deflection() ))
    {
      hasTri = Standard_False;
      break;
    }
    for (eexp.Init(aFace, TopAbs_EDGE); eexp.More(); eexp.Next())
    {
      const TopoDS_Edge& anEdge = TopoDS::Edge(eexp.Current());          

      const Handle(Poly_PolygonOnTriangulation)& aPolygon =
        BRep_Tool::PolygonOnTriangulation(anEdge, aTriangulation, aLoc);
      if (aPolygon.IsNull()) 
      {
        hasTri = Standard_False;
        break;
      }
    }
  }

  if( hasTri == Standard_False )
  {
      const Standard_Real anAngularDeflection = 20.0 * M_PI / 180.0;
      BRepMesh_IncrementalMesh aMeshTool(theShape, theDeflection,
          Standard_False, anAngularDeflection);
  }

  // Count the nodes and triangles in faces
  for (fexp.Init(theShape, TopAbs_FACE); fexp.More(); fexp.Next())
  {
    const TopoDS_Face& aFace = TopoDS::Face(fexp.Current());
    
    const Handle(Poly_Triangulation)& aTriangulation
      = BRep_Tool::Triangulation (aFace, aLoc);

    if (aTriangulation.IsNull() == Standard_False)
    {
      myNNodes     += aTriangulation->NbNodes();
      myNTriangles += aTriangulation->NbTriangles();
    }
  }

  // Create map of edges, to build wireframe for all edges.
  TopTools_DataMapOfShapeInteger mapEdges;  
  for (eexp.Init(theShape, TopAbs_EDGE); eexp.More(); eexp.Next())
  {
    const TopoDS_Shape& anEdge = eexp.Current();
    if (mapEdges.IsBound(anEdge))
      mapEdges.ChangeFind(anEdge) += 1;
    else
      mapEdges.Bind(anEdge, 1);
  }

  Standard_Integer nbFreeEdges = 0, nbEdges = 0;
  TopTools_DataMapIteratorOfDataMapOfShapeInteger anEIt(mapEdges);
  for (; anEIt.More(); anEIt.Next())
  {
    const Standard_Integer nb = anEIt.Value();
    if (nb > 1)
      nbEdges += 1;      
    else
      nbFreeEdges += 1;      
  }

  // Allocate arrays of entities
  if (myNNodes && myNTriangles) {
    mypNodes = static_cast<Standard_ShortReal *>
      (myAlloc->Allocate(sizeof(Standard_ShortReal) * 3 * myNNodes));
    mypNormals = static_cast<Standard_ShortReal *>
      (myAlloc->Allocate(sizeof(Standard_ShortReal) * 3 * myNNodes));
    mypTriangles = static_cast<Standard_Integer *>
      (myAlloc->Allocate(sizeof(Standard_Integer) * 3 * myNTriangles));
    myEdges.InitArray (2, nbEdges, myAlloc.operator->());
    myFreeEdges.InitArray (2, nbFreeEdges, myAlloc.operator->());

    // The second loop: copy all nodes and triangles face-by-face
    const Standard_Real eps2 = Precision::SquareConfusion();
    Standard_Integer nNodes (0), nTriangles (0), nEdges (0), nFEdges (0);
    for (fexp.ReInit(); fexp.More(); fexp.Next())
    {
      const TopoDS_Face& aFace = TopoDS::Face(fexp.Current());
      const Handle(Geom_Surface)& aSurf = BRep_Tool::Surface(aFace, aLocSurf);
      const Handle(Poly_Triangulation)& aTriangulation =
        BRep_Tool::Triangulation(aFace, aLoc);
      if (aTriangulation.IsNull() == Standard_False)
      {
        // Prepare transformation
        Standard_Integer i, aNodeInd(nNodes)/*, aNTriangles = 0*/;
        const gp_Trsf&   aTrf     = aLoc.Transformation();
        const gp_Trsf&   aTrfSurf = aLocSurf.Transformation();
        Standard_Boolean isReverse = (aFace.Orientation() == TopAbs_REVERSED);

        // Store all nodes of the current face in the data model
        const TColgp_Array1OfPnt&   tabNode = aTriangulation->Nodes();
        const TColgp_Array1OfPnt2d& tabUV   = aTriangulation->UVNodes();
        for (i = tabNode.Lower(); i <= tabNode.Upper(); i++)
        {
          Standard_Real t[3];
          tabNode(i).Transformed(aTrf).Coord (t[0], t[1], t[2]);
          //  write node to mesh data
          mypNodes[3*aNodeInd + 0] = static_cast<Standard_ShortReal>(t[0]);
          mypNodes[3*aNodeInd + 1] = static_cast<Standard_ShortReal>(t[1]);
          mypNodes[3*aNodeInd + 2] = static_cast<Standard_ShortReal>(t[2]);

          gp_Vec aD1U, aD1V;
          gp_Pnt aP;
          gp_XYZ aNorm(0., 0., 0.);

          if (aTriangulation->HasNormals()) {
            // Retrieve the normal direction from the triangulation
            aNorm.SetCoord(aTriangulation->Normals().Value(3*i-2),
                           aTriangulation->Normals().Value(3*i-1),
                           aTriangulation->Normals().Value(3*i-0));
          } else if (aSurf.IsNull() == Standard_False)
          {
            // Compute the surface normal at the Node.
            aSurf->D1(tabUV(i).X(), tabUV(i).Y(), aP, aD1U, aD1V);
            aNorm = (aD1U.Crossed(aD1V)).XYZ();
          }

          if (isReverse)
            aNorm.Reverse();
          const Standard_Real aMod = aNorm.SquareModulus();
          if (aMod > eps2) {
            gp_Dir aDirNorm(aNorm);
            aDirNorm.Transform(aTrfSurf);
            aDirNorm.Coord (t[0], t[1], t[2]);
          } else {
            t[0] = 0.;
            t[1] = 0.;
            t[2] = 1.;
          }
          mypNormals[3*aNodeInd + 0] = static_cast<Standard_ShortReal>(t[0]);
          mypNormals[3*aNodeInd + 1] = static_cast<Standard_ShortReal>(t[1]);
          mypNormals[3*aNodeInd + 2] = static_cast<Standard_ShortReal>(t[2]);

          aNodeInd++;
        }
        const Standard_Integer nNodes1 = nNodes - 1;
        // Store all triangles of the current face in the data model
        const Poly_Array1OfTriangle& tabTri  = aTriangulation->Triangles();
        for (i = tabTri.Lower(); i <= tabTri.Upper(); i++)
        {
          Standard_Integer aN[3];
          tabTri(i).Get (aN[0], aN[1], aN[2]);
          Standard_Integer * pTriangle = &mypTriangles[nTriangles*3];
          pTriangle[0] = aN[0] + nNodes1;
          if (isReverse) {
            pTriangle[1] = aN[2] + nNodes1;
            pTriangle[2] = aN[1] + nNodes1;
          } else {
            pTriangle[1] = aN[1] + nNodes1;
            pTriangle[2] = aN[2] + nNodes1;
          }
          const Standard_ShortReal aVec0[3] = {
            mypNodes[3*pTriangle[1]+0] - mypNodes[3*pTriangle[0]+0],
            mypNodes[3*pTriangle[1]+1] - mypNodes[3*pTriangle[0]+1],
            mypNodes[3*pTriangle[1]+2] - mypNodes[3*pTriangle[0]+2]
          };
          const Standard_ShortReal aVec1[3] = {
            mypNodes[3*pTriangle[2]+0] - mypNodes[3*pTriangle[0]+0],
            mypNodes[3*pTriangle[2]+1] - mypNodes[3*pTriangle[0]+1],
            mypNodes[3*pTriangle[2]+2] - mypNodes[3*pTriangle[0]+2]
          };
          const Standard_ShortReal aVecP[3] = {
            aVec0[1] * aVec1[2] - aVec0[2] * aVec1[1],
            aVec0[2] * aVec1[0] - aVec0[0] * aVec1[2],
            aVec0[0] * aVec1[1] - aVec0[1] * aVec1[0]
          };
          if (aVecP[0]*aVecP[0] + aVecP[1]*aVecP[1] + aVecP[2]*aVecP[2] > eps2)
            nTriangles++;
        }
        // Store all edge polygons on the current face.
        for (eexp.Init(aFace, TopAbs_EDGE); eexp.More(); eexp.Next())
        {
          const TopoDS_Edge& anEdge = TopoDS::Edge(eexp.Current());          
          if (mapEdges.IsBound(anEdge)) {
            Standard_Integer nbE = mapEdges.Find(anEdge);
            mapEdges.UnBind(anEdge);

            const Handle(Poly_PolygonOnTriangulation)& aPolygon =
              BRep_Tool::PolygonOnTriangulation(anEdge, aTriangulation, aLoc);
            if (aPolygon.IsNull() == Standard_False) {
              const TColStd_Array1OfInteger& arrNode = aPolygon->Nodes();
              // Allocate memory to store the current polygon indices.
              Standard_Integer aLen = arrNode.Length();
              int * pEdge = static_cast<int *>
                (myAlloc->Allocate(sizeof(int) * (aLen)));
              const gp_Pnt* pLast = &tabNode(arrNode(arrNode.Lower()));
              pEdge[0] = arrNode(arrNode.Lower()) + nNodes1;              
              Standard_Integer iPNode(arrNode.Lower() + 1), iENode(1);
              for (; iPNode <= arrNode.Upper(); iPNode++)
              {
                const Standard_Integer aN(arrNode(iPNode));
                if (pLast->SquareDistance(tabNode(aN)) < eps2)
                {
                  aLen--;
                } else {
                  pLast = &tabNode(aN);
                  pEdge[iENode++] = aN + nNodes1;
                }
              }
              // Do not save very short polygons
              if (aLen > 1) {
                if (nbE > 1)
                  myEdges.SetArray(nEdges++, aLen, pEdge, myAlloc.operator->());
                else
                  myFreeEdges.SetArray(nFEdges++, aLen, pEdge,
                                       myAlloc.operator->());
              }
              myAlloc->Free(pEdge);
            }
          }
        }
        nNodes += tabNode.Length();
      }
    }
    myEdges.SetRealNumbersOfArrays(nEdges);
    myFreeEdges.SetRealNumbersOfArrays(nFEdges);
    myNTriangles = nTriangles;
  }
  if (GetDrawer().IsNull() == Standard_False)
  {
    setDrawerUpdate();
  }
  setIsUpdateBox(Standard_True);
}

//=======================================================================
//function : ~NIS_Surface
//purpose  : Destructor
//=======================================================================

NIS_Surface::~NIS_Surface ()
{
  Clear();
}

//=======================================================================
//function : Clear
//purpose  : 
//=======================================================================

void NIS_Surface::Clear ()
{
  if (myNNodes) {
    myNNodes = 0;
    myAlloc->Free(mypNodes);
    myAlloc->Free(mypNormals);
  }
  if (myNTriangles) {
    myNTriangles = 0;
    myAlloc->Free(mypTriangles);
  }

  myEdges.Clear(myAlloc.operator->());
  myFreeEdges.Clear(myAlloc.operator->());
  
  if (GetDrawer().IsNull() == Standard_False) {
    GetDrawer()->SetUpdated(NIS_Drawer::Draw_Normal,
                            NIS_Drawer::Draw_Top,
                            NIS_Drawer::Draw_Transparent,
                            NIS_Drawer::Draw_Hilighted);
  }
  myBox.Clear();
}

//=======================================================================
//function : DefaultDrawer
//purpose  : 
//=======================================================================

NIS_Drawer * NIS_Surface::DefaultDrawer (NIS_Drawer * theDrawer) const
{
  NIS_SurfaceDrawer * aDrawer =
    theDrawer ? static_cast<NIS_SurfaceDrawer *>(theDrawer)
              : new NIS_SurfaceDrawer (Quantity_NOC_SLATEBLUE4);
  aDrawer->SetBackColor (Quantity_NOC_DARKGREEN);
  aDrawer->myIsWireframe = myIsWireframe;
  return aDrawer;
}

//=======================================================================
//function : SetBackColor
//purpose  : Set the normal color for presentation of back side of triangles.
//=======================================================================

void NIS_Surface::SetBackColor (const Quantity_Color&  theColor)
{
  const Handle(NIS_SurfaceDrawer) aDrawer =
    static_cast<NIS_SurfaceDrawer *>(DefaultDrawer(0L));
  aDrawer->Assign (GetDrawer());
  aDrawer->myBackColor = theColor;
  SetDrawer (aDrawer);
}

//=======================================================================
//function : SetSpecularity
//purpose  : 
//=======================================================================

void NIS_Surface::SetSpecularity (const Standard_Real  theValue)
{
  const Handle(NIS_SurfaceDrawer) aDrawer =
    static_cast<NIS_SurfaceDrawer *>(DefaultDrawer(0L));
  aDrawer->Assign (GetDrawer());
  aDrawer->SetSpecularity(theValue);
  SetDrawer (aDrawer);
}

//=======================================================================
//function : SetPolygonOffset
//purpose  : 
//=======================================================================

void NIS_Surface::SetPolygonOffset (const Standard_Real theValue)
{
  const Handle(NIS_SurfaceDrawer) aDrawer =
    static_cast<NIS_SurfaceDrawer *>(DefaultDrawer(0L));
  aDrawer->Assign (GetDrawer());
  aDrawer->myPolygonOffset = static_cast<Standard_ShortReal>(theValue);
  SetDrawer (aDrawer);
}

//=======================================================================
//function : SetTransformation
//purpose  : 
//=======================================================================

void NIS_Surface::SetTransformation (const gp_Trsf& theTrsf)
{
  const Handle(NIS_SurfaceDrawer) aDrawer =
    static_cast<NIS_SurfaceDrawer *>(DefaultDrawer(0L));
  aDrawer->Assign (GetDrawer());
  aDrawer->SetTransformation(theTrsf);
  SetDrawer (aDrawer);
  setIsUpdateBox(Standard_True);
}

//=======================================================================
//function : SetDisplayMode
//purpose  : Set the display mode: Shading or Wireframe.
//=======================================================================

void  NIS_Surface::SetDisplayMode (const NIS_Surface::DisplayMode theMode)
{
  Standard_Boolean isUpdate(Standard_False);
  if (myIsWireframe) {
    if (theMode != Wireframe) {
      myIsWireframe = Standard_False;
      myIsShowEdges = (theMode != PureShading);
      isUpdate = Standard_True;
    }
  } else {
    if (theMode == Wireframe) {
      myIsWireframe = Standard_True;
      myIsShowEdges = Standard_True;
      isUpdate = Standard_True;
    }
    else {
      if (myIsShowEdges) {
        if (theMode == PureShading) {
          myIsShowEdges = Standard_False;
          isUpdate = Standard_True;
        }
      }
      else {
        if (theMode != PureShading) {
          myIsShowEdges = Standard_True;
          isUpdate = Standard_True;
        }
      }
    }
  }
  if (isUpdate && !GetDrawer().IsNull()) {
    const Handle(NIS_SurfaceDrawer) aDrawer =
      static_cast<NIS_SurfaceDrawer *>(DefaultDrawer(0L));
    aDrawer->Assign (GetDrawer());
    aDrawer->myIsWireframe = myIsWireframe;
    aDrawer->myIsShowEdges = myIsShowEdges;
    SetDrawer(aDrawer);    
  }
}

//=======================================================================
//function : GetDisplayMode
//purpose  : Query the current display mode: Shading or Wireframe.
//=======================================================================

NIS_Surface::DisplayMode NIS_Surface::GetDisplayMode () const
{
  return myIsWireframe ? Wireframe : (myIsShowEdges ? Shading : PureShading);
}

//=======================================================================
//function : SetHilightDisplayMode
//purpose  : Set the hilight display mode: Shading or Wireframe.
//=======================================================================

void  NIS_Surface::SetHilightDisplayMode(const NIS_Surface::DisplayMode theMode)
{
  Standard_Boolean isUpdate = Standard_False;
  if (myIsWireframe == Standard_True) 
  {
    if (myHilightIsWireframe) 
    {
      myHilightIsWireframe = Standard_False;
      isUpdate = Standard_True;
    }
  } 
  else 
  {
    if (myHilightIsWireframe) {
      if (theMode != Wireframe) {
        myHilightIsWireframe = Standard_False;
        isUpdate = Standard_True;
      }
    } 
    else {
      if (theMode == Wireframe) {
        myHilightIsWireframe = Standard_True;
        isUpdate = Standard_True;
      }
    }
  }
  if (isUpdate) {
    const Handle(NIS_Drawer)& aCurrDrawer = GetDrawer();
    if (aCurrDrawer.IsNull() == Standard_False) {
      const Handle(NIS_SurfaceDrawer) aDrawer =
        static_cast<NIS_SurfaceDrawer *>(DefaultDrawer(0L));
      aDrawer->Assign (GetDrawer());
      aDrawer->myHilightIsWireframe = myHilightIsWireframe;
      SetDrawer(aDrawer);
    }
  }
}

//=======================================================================
//function : GetHilightDisplayMode
//purpose  : Query the current hilight display mode: Shading or Wireframe.
//=======================================================================

NIS_Surface::DisplayMode NIS_Surface::GetHilightDisplayMode () const
{
  return myHilightIsWireframe ? Wireframe : Shading;
}

//=======================================================================
//function : ShowFreeBoundary 
//purpose  : Display free boundary of the surface object with the
//           specified color
//=======================================================================
void NIS_Surface::ShowFreeBoundary (const Quantity_Color& theColor)
{
  myIsShowFreeBoundary = Standard_True;
  const Handle(NIS_SurfaceDrawer) aDrawer =
    static_cast<NIS_SurfaceDrawer *>(DefaultDrawer(0L));
  aDrawer->Assign (GetDrawer());
  aDrawer->myFreeEdgeColor = theColor;
  SetDrawer (aDrawer);
//   setDrawerUpdate(); - already called in SetDrawer()
}

//=======================================================================
//function : HideFreeBoundary
//purpose  : Hides special precentation of free boundaries
//=======================================================================
void NIS_Surface::HideFreeBoundary ()
{
  myIsShowFreeBoundary = Standard_False;
  setDrawerUpdate();
}

//=======================================================================
//function : Clone
//purpose  : 
//=======================================================================

void NIS_Surface::Clone (const Handle_NCollection_BaseAllocator& theAlloc,
                         Handle_NIS_InteractiveObject&           theDest) const
{
  Handle(NIS_Surface) aNewObj;
  if (theDest.IsNull()) {
    aNewObj = new NIS_Surface(theAlloc);
    theDest = aNewObj;
  } else {
    aNewObj = reinterpret_cast<NIS_Surface*> (theDest.operator->());
    aNewObj->myAlloc = theAlloc;
  }
  NIS_InteractiveObject::Clone(theAlloc, theDest);
  aNewObj->myNNodes = myNNodes;
  if (myNNodes > 0) {
    // copy nodes and normals
    const Standard_Size nBytes = myNNodes*3*sizeof(Standard_ShortReal);
    aNewObj->mypNodes = (Standard_ShortReal *)theAlloc->Allocate(nBytes);
    aNewObj->mypNormals = (Standard_ShortReal *)theAlloc->Allocate(nBytes);
    memcpy(aNewObj->mypNodes, mypNodes, nBytes);
    memcpy(aNewObj->mypNormals, mypNormals, nBytes);
  }
  aNewObj->myNTriangles = myNTriangles;
  if (myNTriangles > 0) {
    const Standard_Size nBytes = sizeof(Standard_Integer) * 3 * myNTriangles;
    aNewObj->mypTriangles = (Standard_Integer *)theAlloc->Allocate(nBytes);
    memcpy(aNewObj->mypTriangles, mypTriangles, nBytes);
  }

  myEdges.Clone(theAlloc, aNewObj->myEdges);
  myFreeEdges.Clone(theAlloc, aNewObj->myFreeEdges);
  
  aNewObj->myIsWireframe = myIsWireframe;
  aNewObj->myIsShowFreeBoundary = myIsShowFreeBoundary;
  aNewObj->myIsShowEdges = myIsShowEdges;
}

//=======================================================================
//function : NEdgeNodeInList 
//purpose  : 
//=======================================================================

Standard_Integer NIS_Surface::NEdgeNodeInList (const Standard_Integer ind) const
{
  return myEdges.NArrayIndexes(ind);  
}

//=======================================================================
//function : NFreeEdgeNodeInList 
//purpose  : 
//=======================================================================

Standard_Integer NIS_Surface::NFreeEdgeNodeInList
                                        (const Standard_Integer ind) const
{
  return myFreeEdges.NArrayIndexes(ind);  
}

//=======================================================================
//function : Intersect
//purpose  : 
//=======================================================================

Standard_Real NIS_Surface::Intersect (const gp_Ax1&       theAxis,
                                      const Standard_Real theOver) const
{
  Standard_Real aResult (RealLast());
  Standard_Real start[3], dir[3], anInter;

  // Obtain the current Transformation from the Drawer
  const Handle(NIS_SurfaceDrawer)& aDrawer =
    static_cast<const Handle(NIS_SurfaceDrawer)&> (GetDrawer());

  if (aDrawer.IsNull()) {
    theAxis.Location().Coord(start[0], start[1], start[2]);
    theAxis.Direction().Coord(dir[0], dir[1], dir[2]);
  } else {
    gp_Trsf aTrsf = aDrawer->GetTransformation();
    aTrsf.Invert();
    const gp_Ax1 anAxis = theAxis.Transformed(aTrsf);
    anAxis.Location().Coord(start[0], start[1], start[2]);
    anAxis.Direction().Coord(dir[0], dir[1], dir[2]);
  }

  if (myIsWireframe == Standard_False)
    for (Standard_Integer i = 0; i < myNTriangles; i++) {
      const Standard_Integer * pTri = &mypTriangles[3*i];
      if (NIS_Triangulated::tri_line_intersect (start, dir,
                                                &mypNodes[3*pTri[0]],
                                                &mypNodes[3*pTri[1]],
                                                &mypNodes[3*pTri[2]],
                                                &anInter))
        if (anInter < aResult)
          aResult = anInter;
    }
  else {
    anInter = intersectEdges(myEdges, theAxis, theOver);
    if (anInter < aResult)
      aResult = anInter;    
    anInter = intersectEdges(myFreeEdges, theAxis, theOver);      
    if (anInter < aResult)
      aResult = anInter;    
  }

  return aResult;
}

//=======================================================================
//function : Normal
//purpose  : 
//=======================================================================

Standard_Boolean NIS_Surface::Intersect (const gp_Ax1& theAxis, gp_Vec& theNormal) const
{
  Standard_Real aResult (RealLast());
  Standard_Real start[3], dir[3], anInter;
  Standard_Integer anIndex(-1);

  // Obtain the current Transformation from the Drawer
  const Handle(NIS_SurfaceDrawer)& aDrawer =
    static_cast<const Handle(NIS_SurfaceDrawer)&> (GetDrawer());

  if (aDrawer.IsNull()) {
    theAxis.Location().Coord(start[0], start[1], start[2]);
    theAxis.Direction().Coord(dir[0], dir[1], dir[2]);
  } else {
    gp_Trsf aTrsf = aDrawer->GetTransformation();
    aTrsf.Invert();
    const gp_Ax1 anAxis = theAxis.Transformed(aTrsf);
    anAxis.Location().Coord(start[0], start[1], start[2]);
    anAxis.Direction().Coord(dir[0], dir[1], dir[2]);
  }

  if (myIsWireframe == Standard_False)
    for (Standard_Integer i = 0; i < myNTriangles; i++) {
      const Standard_Integer * pTri = &mypTriangles[3*i];
      if (NIS_Triangulated::tri_line_intersect (start, dir,
                                                &mypNodes[3*pTri[0]],
                                                &mypNodes[3*pTri[1]],
                                                &mypNodes[3*pTri[2]],
                                                &anInter))
        if (anInter < aResult) {
          aResult = anInter;
          anIndex = i;
        }
    }

  if (anIndex < 0) 
    return Standard_False;

  const Standard_Integer * pTri = &mypTriangles[3*anIndex];
  gp_XYZ aPnt0(mypNodes[3*pTri[0]+0], mypNodes[3*pTri[0]+1], mypNodes[3*pTri[0]+2]);
  gp_XYZ aPnt1(mypNodes[3*pTri[1]+0], mypNodes[3*pTri[1]+1], mypNodes[3*pTri[1]+2]);
  gp_XYZ aPnt2(mypNodes[3*pTri[2]+0], mypNodes[3*pTri[2]+1], mypNodes[3*pTri[2]+2]);
  theNormal.SetXYZ(((aPnt1 - aPnt0) ^ (aPnt2 - aPnt0)));

  return Standard_True;
}

//=======================================================================
//function : intersectEdges
//purpose  : 
//=======================================================================

Standard_Real NIS_Surface::intersectEdges(const NIS_IndexLists&  theEdges, 
                                          const gp_Ax1&       theAxis,
                                          const Standard_Real theOver) const
{
  const Standard_Real anOver2 = theOver*theOver;
  double anInter;
  Standard_Real aResult (RealLast());
  for (Standard_Integer iEdge = 0; iEdge < theEdges.NArrays(); iEdge++) {     
    const Standard_Integer * anEdge =
      static_cast <Standard_Integer *>(theEdges.ArrayIndexes(iEdge));
    const Standard_Integer nNodes = theEdges.NArrayIndexes(iEdge);
    for (Standard_Integer i = 0; i < nNodes-1; i++) {
      // Node index is incremented for the head of polygon indice array
      if (NIS_Triangulated::seg_line_intersect (theAxis.Location().XYZ(),
                                                theAxis.Direction().XYZ(),
                                                anOver2,
                                                &mypNodes[3*anEdge[i+0]],
                                                &mypNodes[3*anEdge[i+1]],
                                                &anInter))
      if (anInter < aResult)
        aResult = anInter;
    }
  }
  return aResult;
}

//=======================================================================
//function : Intersect
//purpose  : 
//=======================================================================

Standard_Boolean NIS_Surface::Intersect (const Bnd_B3f&         theBox,
                                         const gp_Trsf&         theTrf,
                                         const Standard_Boolean isFullIn) const
{
  Standard_Boolean aResult (isFullIn);
  gp_Trsf aTrsf = theTrf;

  // Obtain the current Transformation from the Drawer
  const Handle(NIS_SurfaceDrawer)& aDrawer =
    static_cast<const Handle(NIS_SurfaceDrawer)&> (GetDrawer());

  if (aDrawer.IsNull() == Standard_False)
    aTrsf *= aDrawer->GetTransformation();

  if (myIsWireframe == Standard_False) {
    if (myNTriangles > 0) {
      for (Standard_Integer iNode = 0; iNode < myNNodes*3; iNode+=3) {
        gp_XYZ aPnt (static_cast<Standard_Real>(mypNodes[iNode+0]),
                     static_cast<Standard_Real>(mypNodes[iNode+1]),
                     static_cast<Standard_Real>(mypNodes[iNode+2]));
        aTrsf.Transforms(aPnt);
        if (theBox.IsOut (aPnt) == isFullIn) {
          aResult = !isFullIn;
          break;
        }
      }
    }
  } else {
    aResult = intersectEdges(myEdges, aTrsf, theBox, isFullIn);
    if(!aResult)
      aResult = intersectEdges(myFreeEdges, aTrsf, theBox, isFullIn);    
  }
  return aResult;
}

//=======================================================================
//function : intersectEdges
//purpose  : 
//=======================================================================

Standard_Boolean NIS_Surface::intersectEdges(const NIS_IndexLists&  theEdges, 
                                             const gp_Trsf&         theTrf,
                                             const Bnd_B3f&         theBox,
                                             const Standard_Boolean isFullIn) const
{
  Standard_Boolean aResult (isFullIn);
  for (Standard_Integer iEdge = 0; iEdge < theEdges.NArrays(); iEdge++) {     
    const Standard_Integer * anEdge =
      static_cast <Standard_Integer *>(theEdges.ArrayIndexes(iEdge));
    const Standard_Integer nNodes = theEdges.NArrayIndexes(iEdge);
    for (Standard_Integer i = 0; i < nNodes-1; i++) {        
      gp_Pnt aPnt[2] = {
        gp_Pnt(static_cast<Standard_Real>(mypNodes[3*anEdge[i+0]+0]),
               static_cast<Standard_Real>(mypNodes[3*anEdge[i+0]+1]),
               static_cast<Standard_Real>(mypNodes[3*anEdge[i+0]+2])),
        gp_Pnt(static_cast<Standard_Real>(mypNodes[3*anEdge[i+1]+0]),
               static_cast<Standard_Real>(mypNodes[3*anEdge[i+1]+1]),
               static_cast<Standard_Real>(mypNodes[3*anEdge[i+1]+2]))
      };
      aPnt[0].Transform(theTrf);
      aPnt[1].Transform(theTrf);
      if (isFullIn) {
        if (NIS_Triangulated::seg_box_included (theBox, aPnt) == 0) {
          aResult = Standard_False;
          break;
        }
      } else {
        if (NIS_Triangulated::seg_box_intersect (theBox, aPnt)) {
          aResult = Standard_True;
          break;
        }
      }
    }
  }
  return aResult;
}

//=======================================================================
//function : Intersect
//purpose  : Selection by polygon
//=======================================================================

Standard_Boolean NIS_Surface::Intersect
                    (const NCollection_List<gp_XY> &thePolygon,
                     const gp_Trsf                 &theTrf,
                     const Standard_Boolean         isFullIn) const
{
  Standard_Boolean aResult (isFullIn);
  gp_Trsf aTrsf = theTrf;

  // Obtain the current Transformation from the Drawer
  const Handle(NIS_SurfaceDrawer)& aDrawer =
    static_cast<const Handle(NIS_SurfaceDrawer)&> (GetDrawer());

  if (aDrawer.IsNull() == Standard_False)
    aTrsf *= aDrawer->GetTransformation();

  if (myIsWireframe == Standard_False) {
    if (myNTriangles > 0) {
      for (Standard_Integer iNode = 0; iNode < myNNodes*3; iNode+=3) {
        gp_XYZ aPnt (static_cast<Standard_Real>(mypNodes[iNode+0]),
                     static_cast<Standard_Real>(mypNodes[iNode+1]),
                     static_cast<Standard_Real>(mypNodes[iNode+2]));
        aTrsf.Transforms(aPnt);
        gp_XY aP2d(aPnt.X(), aPnt.Y());

        if (!NIS_Triangulated::IsIn(thePolygon, aP2d)) {
          if (isFullIn) {
            aResult = Standard_False;
            break;
          }
        } else {
          if (isFullIn == Standard_False) {
            aResult = Standard_True;
            break;
          }
        }
      }
    }
  } else {
    aResult = intersectEdges(myEdges, aTrsf, thePolygon, isFullIn);
    if(!aResult)
      aResult = intersectEdges(myFreeEdges, aTrsf, thePolygon, isFullIn);    
  }
  return aResult;
}

//=======================================================================
//function : intersectEdges
//purpose  : 
//=======================================================================

Standard_Boolean NIS_Surface::intersectEdges
                                (const NIS_IndexLists&          theEdges, 
                                 const gp_Trsf&                 theTrf,
                                 const NCollection_List<gp_XY>& thePolygon,
                                 const Standard_Boolean         isFullIn) const
{
  Standard_Boolean aResult (isFullIn);
  for (Standard_Integer iEdge = 0; iEdge < theEdges.NArrays(); iEdge++)
  {     
    const Standard_Integer * anEdge =
      static_cast <Standard_Integer *>(theEdges.ArrayIndexes(iEdge));
    const Standard_Integer nNodes = theEdges.NArrayIndexes(iEdge);
    for (Standard_Integer i = 0; i < nNodes-1; i++) {                  
      gp_Pnt aPnt[2] = {
        gp_Pnt(static_cast<Standard_Real>(mypNodes[3*anEdge[i+0]+0]),
               static_cast<Standard_Real>(mypNodes[3*anEdge[i+0]+1]),
               static_cast<Standard_Real>(mypNodes[3*anEdge[i+0]+2])),
        gp_Pnt(static_cast<Standard_Real>(mypNodes[3*anEdge[i+1]+0]),
               static_cast<Standard_Real>(mypNodes[3*anEdge[i+1]+1]),
               static_cast<Standard_Real>(mypNodes[3*anEdge[i+1]+2]))
      };
      aPnt[0].Transform(theTrf);
      aPnt[1].Transform(theTrf);
      const gp_XY aP2d[2] = { gp_XY(aPnt[0].X(), aPnt[0].Y()),
                              gp_XY(aPnt[1].X(), aPnt[1].Y()) };
      if (isFullIn) {
        if (NIS_Triangulated::seg_polygon_included (thePolygon, aP2d) == 0) {
          aResult = Standard_False;
          break;
        }
      } else {
        if (NIS_Triangulated::seg_polygon_intersect (thePolygon, aP2d)) {
          aResult = Standard_True;
          break;
        }
      }
    }
  }
  return aResult;
}

//=======================================================================
//function : computeBox
//purpose  : 
//=======================================================================

void NIS_Surface::computeBox ()
{
  NIS_Triangulated::ComputeBox(myBox, myNNodes, mypNodes, 3);

  const Handle(NIS_SurfaceDrawer)& aDrawer =
    static_cast<const Handle(NIS_SurfaceDrawer)&> (GetDrawer());

  if (aDrawer.IsNull() == Standard_False) {
    const gp_Trsf& aTrsf = aDrawer->GetTransformation();
    myBox = myBox.Transformed(aTrsf);
  }
}
