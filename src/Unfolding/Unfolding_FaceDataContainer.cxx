// File:	Unfolding_FaceDataContainer.cxx
// Created:	Tue Jul 22 13:12:24 2008
// Author:	Sergey KHROMOV
//		<skv@dimox>


#include <BRepBuilderAPI_Transform.hxx>
#include <gp_Trsf.hxx>
#include <Standard_Type.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopTools_DataMapIteratorOfDataMapOfShapeListOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopTools_ListOfShape.hxx>
#include <Unfolding_FaceDataContainer.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Unfolding_FaceDataContainer,MMgt_TShared)

//=======================================================================
//function : SetFace
//purpose  : 
//=======================================================================
void Unfolding_FaceDataContainer::SetFace(const TopoDS_Face &theFace)
{
  Standard_Boolean isSame = theFace.IsSame(myFace);
  myFace = theFace;

  if (!isSame) {
    // Update the map myEdgeMap
    myEdgeMap.Clear();

    TopExp_Explorer      anExp(myFace, TopAbs_EDGE);
    TopTools_ListOfShape anEmptyList;

    for (; anExp.More(); anExp.Next()) {
      const TopoDS_Shape &anEdge = anExp.Current();

      myEdgeMap.Bind(anEdge, anEmptyList);
    }
  }
}

//=======================================================================
//function : ApplyTrsf
//purpose  : 
//=======================================================================

void Unfolding_FaceDataContainer::ApplyTrsf(const gp_Trsf &theTrsf)
{
  BRepBuilderAPI_Transform aTrsf(theTrsf);

  if (!myUnfoldedFace.IsNull()) {
    aTrsf.Perform(myUnfoldedFace);

    if (aTrsf.IsDone())
      myUnfoldedFace = TopoDS::Face(aTrsf.Shape());
  }

  TopTools_DataMapIteratorOfDataMapOfShapeListOfShape anEdgeIter(myEdgeMap);
  TopTools_ListIteratorOfListOfShape                  aUEIter;

  for (; anEdgeIter.More(); anEdgeIter.Next()) {
    const TopoDS_Shape   &anEdge  = anEdgeIter.Key();
    TopTools_ListOfShape &anEdges = myEdgeMap.ChangeFind(anEdge);
    TopTools_ListOfShape  aTrsfEdges;

    for (aUEIter.Initialize(anEdges); aUEIter.More(); aUEIter.Next()) {
      const TopoDS_Shape &aUEdge = aUEIter.Value();

      aTrsf.Perform(aUEdge);

      if (aTrsf.IsDone())
	aTrsfEdges.Append(aTrsf.Shape());
    }

    anEdges.Clear();
    anEdges.Append(aTrsfEdges);
  }
}

//=======================================================================
//function : SetEdgesForEdge
//purpose  : 
//=======================================================================

void Unfolding_FaceDataContainer::SetEdgesForEdge
                             (const TopoDS_Edge          &theEdge,
			      const TopTools_ListOfShape &theUnfoldedEdges)
{
  if (myEdgeMap.IsBound(theEdge)) {
    TopTools_ListOfShape &anEdges = myEdgeMap.ChangeFind(theEdge);

    anEdges = theUnfoldedEdges;
  }
}

//=======================================================================
//function : GetEdgesForEdge
//purpose  : 
//=======================================================================

const TopTools_ListOfShape &Unfolding_FaceDataContainer::GetEdgesForEdge
                                          (const TopoDS_Edge &theEdge) const
{
  if (myEdgeMap.IsBound(theEdge)) {
    const TopTools_ListOfShape &anEdges = myEdgeMap.Find(theEdge);

    return anEdges;
  }

  static TopTools_ListOfShape anEmptyList;

  return anEmptyList;
}
