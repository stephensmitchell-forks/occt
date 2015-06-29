// File:      NIS_Tools.cxx
// Created:   29.07.11 08:46
// Copyright: Open Cascade 2011


#include <NIS_Tools.hxx>
#include <NIS_Triangulated.hxx>
#include <NIS_InteractiveContext.hxx>
#include <NIS_Point.hxx>

#include <TopExp_Explorer.hxx>
#include <TopTools_MapOfShape.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS.hxx>
#include <Poly_Triangulation.hxx>
#include <BRep_Tool.hxx>

//=======================================================================
//function : Decompose
//purpose  : Function for creating the Local selection
//=======================================================================

Standard_Boolean NIS_Tools::Decompose
                (const TopoDS_Shape&                            theSh,
                 const Standard_Real                            theToler,
                 const Standard_Integer                         theTypes,
                 const Handle(NIS_InteractiveContext)&          theCtx,
                 TopTools_DataMapOfIntegerShape&                theMap, 
                 const Quantity_Color&                          theColor,
                 const NIS_Surface::DisplayMode                 theHlgMode,
                 const Standard_Real                            theLineWidth)
{  
  Standard_Boolean aRes = Standard_False;
  if (theCtx.IsNull() || theSh.IsNull())
    return aRes;

  if (theTypes & Type_None)
    return aRes;

  TopTools_MapOfShape shMap;
  if (theTypes & Type_Face)
  {
    TopExp_Explorer fexp (theSh, TopAbs_FACE);
    for (; fexp.More(); fexp.Next())
    {      
      const TopoDS_Face& aFace = TopoDS::Face(fexp.Current());
      if (!shMap.Add(aFace))
        continue;

      Standard_Real aToler = theToler;
      TopLoc_Location aLoc;
      Handle(Poly_Triangulation) aPolyTri = BRep_Tool::Triangulation(aFace, aLoc);
      if (aPolyTri.IsNull() == Standard_False)
      {
        if (aPolyTri->Deflection() > theToler)
        {
          aToler = aPolyTri->Deflection();
        }
      }

      Handle(NIS_Surface) aSurface = new NIS_Surface(aFace, aToler);
      theCtx->Display(aSurface); 
      aSurface->SetHilightDisplayMode(theHlgMode);
      aSurface->SetColor(theColor);
      aSurface->SetBackColor(theColor);
      
      if (!theMap.IsBound(aSurface->ID()))
        theMap.Bind(aSurface->ID(), aFace);
    }
  }
  if (theTypes & Type_Edge)
  {
    TopExp_Explorer eexp (theSh, TopAbs_EDGE);
    for (; eexp.More(); eexp.Next())
    {
      const TopoDS_Edge& anEdge = TopoDS::Edge(eexp.Current());
      if (!shMap.Add(anEdge))
        continue;

      Handle(NIS_Triangulated) aTri = new NIS_Triangulated(anEdge, theToler);
      theCtx->Display(aTri);      
      if (!theMap.IsBound(aTri->ID()))
        theMap.Bind(aTri->ID(), anEdge);
      aTri->SetColor(theColor);
      aTri->SetLineWidth(theLineWidth);
    }
  }
  if (theTypes & Type_Vertex)
  {
    TopExp_Explorer vexp (theSh, TopAbs_VERTEX);
    for (; vexp.More(); vexp.Next())
    {
      const TopoDS_Vertex& aVertex = TopoDS::Vertex(vexp.Current());
      if (!shMap.Add(aVertex))
        continue;

      Handle(NIS_Point) aPoint = new NIS_Point(BRep_Tool::Pnt(aVertex).XYZ());
      theCtx->Display(aPoint);      
      if (!theMap.IsBound(aPoint->ID()))
        theMap.Bind(aPoint->ID(), aVertex);
      aPoint->SetColor(theColor);
      aPoint->SetMarkerType(NIS_MT_CIRCLE);
    }
  }    

  return !theMap.IsEmpty();
}

